/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2014, 2016-2025 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2014 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include <time.h>
#include <fstream>
#include <algorithm>

#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "cryptoFwkPteid.h"
#include "APLCertif.h"
#include "APLReader.h"
#include "APLConfig.h"
#include "CardFile.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "CertStatusCache.h"
#include "MiscUtil.h"
#include "PKIFetcher.h"
#include "CardPteidDef.h"
#include "Log.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#include <sys/stat.h>
#include <cassert>

#ifdef WIN32
#include <io.h>
#endif

namespace eIDMW {

#ifdef _WIN32
inline struct tm *localtime_r(const time_t *clock, struct tm *result) {
	if (!clock || !result)
		return NULL;
	memcpy(result, localtime(clock), sizeof(*result));
	return result;
}
#endif

APL_CertifStatus ConvertStatus(FWK_CertifStatus eStatus, APL_ValidationProcess eProcess) {
	// Convert the status out of the Crypto framework into an APL_CertifStatus
	switch (eStatus) {
	case FWK_CERTIF_STATUS_REVOKED:
		return APL_CERTIF_STATUS_REVOKED;

	case FWK_CERTIF_STATUS_UNKNOWN:
		return APL_CERTIF_STATUS_UNKNOWN;

	case FWK_CERTIF_STATUS_CONNECT:
		return APL_CERTIF_STATUS_CONNECT;

	case FWK_CERTIF_STATUS_ERROR:
		return APL_CERTIF_STATUS_ERROR;

	case FWK_CERTIF_STATUS_VALID:
		return APL_CERTIF_STATUS_VALID;

	case FWK_CERTIF_STATUS_SUSPENDED:
		return APL_CERTIF_STATUS_SUSPENDED;

	default:
		return APL_CERTIF_STATUS_UNCHECK;
	}
}

/*****************************************************************************************
---------------------------------------- APL_Certifs --------------------------------------
*****************************************************************************************/
APL_Certifs::APL_Certifs(APL_SmartCard *card) {
	init(card);

	loadFromFile();
	loadCard();
	reOrderCerts();

	initSODCAs();
	defaultSODCertifs = true;
}

APL_Certifs::APL_Certifs(bool loadFromCertsDir) {
	init(NULL);
	if (loadFromCertsDir) {
		loadFromFile();
	}
}

// This should select the certificates which are part of the chain for SOD signatures for production cards

void APL_Certifs::initSODCAs() {
	APL_Certif *cert = NULL;
	unsigned long ulCount = 0;

	defaultSODCertifs = true;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for (itrOrder = m_certifsOrder.begin(); itrOrder != m_certifsOrder.end(); itrOrder++) {
		itrCert = m_certifs.find(*itrOrder);
		if (itrCert == m_certifs.end()) {
			// The certif is not in the map
			// Should not happen
			MWLOG(LEV_ERROR, MOD_APL, "Exception in initSODCAs(): certificate not found!");
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert = itrCert->second;
		// Add certificate if it was NOT loaded from card
		if (!cert->isFromCard()) {
			m_sod_cas.push_back(cert);
			MWLOG(LEV_DEBUG, MOD_APL, "initSODCAs(): Adding certificate for card: %s Valid from: %s to: %s",
				  cert->getOwnerName(), cert->getValidityBegin(), cert->getValidityEnd());
		}
	}
}

void APL_Certifs::clearSODCAs() {
	MWLOG(LEV_DEBUG, MOD_APL, "resetSODCAs() called");
	m_sod_cas = std::vector<APL_Certif *>();
}

void APL_Certifs::addToSODCAs(const CByteArray &cert_ba) {
	if (defaultSODCertifs) {
		clearSODCAs();
		defaultSODCertifs = false;
	}

	APL_Certif *cert = new APL_Certif(this, cert_ba, APL_CERTIF_TYPE_ROOT);
	MWLOG(LEV_DEBUG, MOD_APL, "addToSODCAs(): Adding certificate %s", cert->getOwnerName());
	m_sod_cas.push_back(cert);
}

unsigned long APL_Certifs::countSODCAs() {
	MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: countSODCAs returns %lu", m_sod_cas.size());
	assert(m_sod_cas.size() <= ULONG_MAX);
	return (unsigned long) m_sod_cas.size();
}

APL_Certif *APL_Certifs::getSODCA(int index) {
	if (index >= m_sod_cas.size())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return m_sod_cas[index];
}

void APL_Certifs::init(APL_SmartCard *card) {

	APL_Config certs_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	APL_Config certs_dir_test(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST);

	m_certs_dir = certs_dir.getString();

	// Load test certificates from different directory
	if (CConfig::isTestModeEnabled())
		m_certs_dir = certs_dir_test.getString();

	m_cryptoFwk = AppLayer.getCryptoFwk();
	// Crypto framework needs the card instance to access the CA certs
	m_cryptoFwk->setActiveCard(card);

	m_card = card;
	m_certExtension = "der";
}

APL_Certifs::~APL_Certifs(void) {
	std::map<unsigned long, APL_Certif *>::iterator itr;

	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		delete itr->second;
		itr->second = NULL;
	}
	m_certifs.clear();
	m_certifsOrder.clear();
}

unsigned long APL_Certifs::countFromCard() {
	if (!m_card)
		return 0;

	return m_card->certificateCount();
}

unsigned long APL_Certifs::countAll() { return (unsigned long)m_certifs.size(); }

APL_Certif *APL_Certifs::getCertFromCard(unsigned long ulIndex) {
	if (!m_card)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	if (ulIndex >= countFromCard())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	APL_Certif *cert = NULL;

	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		cert = itr->second;
		if (cert->getIndexOnCard() == ulIndex)
			return cert;
	}

	{
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation

		for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
			cert = itr->second;
			if (cert->getIndexOnCard() == ulIndex)
				return cert;
		}

		cert = new APL_Certif(m_card, this, ulIndex);

		// Don't load the self-signed root cert from Card!
		//  We ship the right version in eidstore...
		if (strcmp(cert->getLabel(), "ROOT CA") == 0)
			return NULL;

		unsigned long ulUniqueId = cert->getUniqueId();
		itr = m_certifs.find(ulUniqueId);
		if (itr == m_certifs.end()) {
			m_certifs[ulUniqueId] = cert;
			m_certifsOrder.push_back(ulUniqueId);
		}
		else {
			APL_Certif *foundCert = itr->second;
			delete foundCert;
			m_certifs[ulUniqueId] = cert;
		}

		return cert;
	}
}

/* This is used to add certificate from file. If the same certificate was already loaded from the card, we store the new
   one in m_certifs instead, we need this so that initSODCAs() picks up all the certificates in eidstore
 */
APL_Certif *APL_Certifs::addCert(const CByteArray &certIn, APL_CertifType type, bool needToSetIssuer) {

	std::map<unsigned long, APL_Certif *>::const_iterator itr;

	unsigned long ulUniqueId = m_cryptoFwk->GetCertUniqueID(certIn);

	{
		CAutoMutex autoMutex(&m_Mutex); // We lock for unly one instanciation

		APL_Certif *cert = NULL;
		auto certsIt = m_certifs.find(ulUniqueId);
		if (certsIt != m_certifs.end() &&
			(certsIt->second->isFromCard())) { // return existing cert if this cert is already loaded from card
			return certsIt->second;
		}
		if (m_cryptoFwk->isSelfIssuer(certIn)) {
			type = APL_CERTIF_TYPE_ROOT;
		}
		cert = new APL_Certif(this, certIn, type);
		m_certifs[ulUniqueId] = cert;

		std::vector<unsigned long>::iterator itrOrder;
		// Check for duplicate
		itrOrder = std::find(m_certifsOrder.begin(), m_certifsOrder.end(), ulUniqueId);
		if (itrOrder == m_certifsOrder.end())
			m_certifsOrder.push_back(ulUniqueId);

		if (needToSetIssuer)
			cert->resetIssuer();
		cert->resetRoot();

		return cert;
	}
}

APL_Certif *APL_Certifs::addCert(APL_CardFile_Certificate *file, APL_CertifType type, bool bOnCard,
								 unsigned long ulIndex, const CByteArray *cert_data) {
	if (!file && !cert_data)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	std::map<unsigned long, APL_Certif *>::const_iterator itr;

	unsigned long ulUniqueId;

	if (file) {
		if (file->getStatus(false) != CARDFILESTATUS_OK)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		ulUniqueId = file->getUniqueId();
	} else {
		APL_CardFile_Certificate filetmp(m_card, "", cert_data);
		ulUniqueId = filetmp.getUniqueId();
	}

	{
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation

		itr = m_certifs.find(ulUniqueId);
		if (itr != m_certifs.end()) {

			if (m_certifs[ulUniqueId]->m_type == APL_CERTIF_TYPE_UNKNOWN)
				m_certifs[ulUniqueId]->m_type = type;

			// We put the added cert at the end of order list
			std::vector<unsigned long>::iterator itrOrder;
			for (itrOrder = m_certifsOrder.begin(); itrOrder != m_certifsOrder.end(); itrOrder++) {
				if (ulUniqueId == (*itrOrder)) {
					m_certifsOrder.erase(itrOrder);
					m_certifsOrder.push_back(ulUniqueId);
					break;
				}
			}

			return m_certifs[ulUniqueId];
		}

		APL_Certif *cert = NULL;
		cert = new APL_Certif(this, file, type, bOnCard, ulIndex, cert_data);
		m_certifs[ulUniqueId] = cert;
		m_certifsOrder.push_back(ulUniqueId);

		resetFlags();

		return cert;
	}
}

APL_Certif *APL_Certifs::getCert(unsigned long ulIndex) {

	APL_Certif *cert = NULL;
	unsigned long ulCount = 0;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for (itrOrder = m_certifsOrder.begin(); itrOrder != m_certifsOrder.end(); itrOrder++) {
		itrCert = m_certifs.find(*itrOrder);
		if (itrCert == m_certifs.end()) {
			// The certif is not in the map
			// Shouldn't happen
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert = itrCert->second;

		if (ulCount == ulIndex)
			return cert;
		else
			ulCount++;
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	// if (ulIndex >= my_certifs.size())
	//	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	// return my_certifs[ulIndex];
}

APL_Certif *APL_Certifs::getCertUniqueId(unsigned long ulUniqueId) {
	std::map<unsigned long, APL_Certif *>::const_iterator itrAll;

	itrAll = m_certifs.find(ulUniqueId);
	if (itrAll == m_certifs.end())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_certifs[ulUniqueId];
}

unsigned long APL_Certifs::countCert(APL_CertifType type) {
	unsigned long count = 0;
	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	APL_Certif *cert = NULL;

	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		cert = itr->second;

		if (cert->isType(type))
			count++;
	}

	return count;
}

APL_Certif *APL_Certifs::getCert(APL_CertifType type, unsigned long ulIndex) {
	APL_Certif *cert = NULL;
	unsigned long ulCount = 0;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for (itrOrder = m_certifsOrder.begin(); itrOrder != m_certifsOrder.end(); itrOrder++) {
		itrCert = m_certifs.find(*itrOrder);
		if (itrCert == m_certifs.end()) {
			// The certif is not in the map
			// Should not happen
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert = itrCert->second;

		if (cert->isType(type)) {
			// If no index we return the first certificate that matches
			if (ulIndex == ANY_INDEX) {
				return cert;
			} else {
				if (ulCount == ulIndex)
					return cert;
				else
					ulCount++;
			}
		}
	}

	if (type == APL_CERTIF_TYPE_ROOT)
		throw CMWEXCEPTION(EIDMW_ERR_CERT_NOROOT);

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

unsigned long APL_Certifs::countRoot() { return countCert(APL_CERTIF_TYPE_ROOT); }

APL_Certif *APL_Certifs::getRoot(unsigned long ulIndex) {
	try {
		return getCert(APL_CERTIF_TYPE_ROOT);
	} catch (CMWException &e) {
		if (e.GetError() == (long)EIDMW_ERR_PARAM_RANGE)
			throw CMWEXCEPTION(EIDMW_ERR_CERT_NOROOT);

		throw;
	}
}

unsigned long APL_Certifs::countAuthentication() { return countCert(APL_CERTIF_TYPE_AUTHENTICATION); }

APL_Certif *APL_Certifs::getAuthentication(unsigned long ulIndex) { return getCert(APL_CERTIF_TYPE_AUTHENTICATION); }

unsigned long APL_Certifs::countSignature() { return countCert(APL_CERTIF_TYPE_SIGNATURE); }

APL_Certif *APL_Certifs::getSignature(unsigned long ulIndex) { return getCert(APL_CERTIF_TYPE_SIGNATURE); }

unsigned long APL_Certifs::countCA() { return countCert(APL_CERTIF_TYPE_ROOT_AUTH); }

APL_Certif *APL_Certifs::getSignatureSubCA() { return getCert(APL_CERTIF_TYPE_ROOT_SIGN); }

APL_Certif *APL_Certifs::getAuthenticationSubCA() { return getCert(APL_CERTIF_TYPE_ROOT_AUTH); }

unsigned long APL_Certifs::countChildren(const APL_Certif *certif) {
	unsigned long ulCount = 0;
	APL_Certif *children = NULL;

	std::map<unsigned long, APL_Certif *> *store;
	store = &m_certifs;

	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	for (itr = store->begin(); itr != store->end(); itr++) {
		children = itr->second;
		if (children->getIssuer() == certif) {
			ulCount++;
		}
	}

	return ulCount;
}

APL_Certif *APL_Certifs::getChildren(const APL_Certif *certif, unsigned long ulIndex) {
	APL_Certif *children = NULL;
	unsigned long ulCount = 0;

	std::map<unsigned long, APL_Certif *> *store;
	store = &m_certifs;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for (itrOrder = m_certifsOrder.begin(); itrOrder != m_certifsOrder.end(); itrOrder++) {
		itrCert = m_certifs.find(*itrOrder);
		if (itrCert == m_certifs.end()) {
			// The certif is not in the map
			// Should not happend
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		children = itrCert->second;
		if (children->getIssuer() == certif) {
			if (ulCount == ulIndex)
				return children;
			else
				ulCount++;
		}
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

void APL_Certifs::loadCard() {
	APL_Certif *cert;

	for (unsigned long i = 0; i < countFromCard(); i++) {
		cert = getCertFromCard(i);
	}

	resetFlags();
}

void APL_Certifs::loadFromFile() {
	bool bStopRequest = false;
	scanDir(m_certs_dir.c_str(), "", m_certExtension.c_str(), bStopRequest, this, &APL_Certifs::foundCertificate);
#ifndef WIN32
	bStopRequest = false;
	APL_Config cachePath(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR_CERTS);
	scanDir(cachePath.getString(), "/", m_certExtension.c_str(), bStopRequest, this, &APL_Certifs::foundCertificate);
#endif
}

void APL_Certifs::reOrderCerts() {
	std::vector<unsigned long> cardRoots;
	APL_Certif *citizenSign = NULL;
	APL_Certif *citizenAuth = NULL;
	APL_Certif *citizenSignSubCA = NULL;
	APL_Certif *citizenAuthSubCA = NULL;

	for (auto uniqueIndex : m_certifsOrder) {
		auto itr = m_certifs.find(uniqueIndex);
		APL_Certif *cert = itr->second;
		if (cert->isAuthentication()) {
			citizenAuth = cert;
		} else if (cert->isSignature()) {
			citizenSign = cert;
		} else {
			cardRoots.push_back(uniqueIndex);
		}
	}

	if (citizenAuth && citizenAuth->getIssuer()) {
		citizenAuthSubCA = citizenAuth->getIssuer();
	}

	if (citizenSign && citizenSign->getIssuer()) {
		citizenSignSubCA = citizenSign->getIssuer();
	}

	if (citizenSignSubCA)
		cardRoots.erase(std::remove(cardRoots.begin(), cardRoots.end(), citizenSignSubCA->getUniqueId()),
						cardRoots.end());
	if (citizenAuthSubCA)
		cardRoots.erase(std::remove(cardRoots.begin(), cardRoots.end(), citizenAuthSubCA->getUniqueId()),
						cardRoots.end());

	m_certifsOrder.clear();
	if (citizenAuth)
		m_certifsOrder.push_back(citizenAuth->getUniqueId());

	if (citizenSign)
		m_certifsOrder.push_back(citizenSign->getUniqueId());

	if (citizenSignSubCA) {
		m_certifsOrder.push_back(citizenSignSubCA->getUniqueId());
		citizenSignSubCA->setType(APL_CERTIF_TYPE_ROOT_SIGN);
	}

	if (citizenAuthSubCA) {
		m_certifsOrder.push_back(citizenAuthSubCA->getUniqueId());
		citizenAuthSubCA->setType(APL_CERTIF_TYPE_ROOT_AUTH);
	}

	m_certifsOrder.insert(m_certifsOrder.end(), cardRoots.begin(), cardRoots.end());
}

void APL_Certifs::foundCertificate(const char *dir, const char *SubDir, const char *File, void *param) {
	APL_Certifs *certifs = static_cast<APL_Certifs *>(param);
	std::string path = dir;
	FILE *m_stream;
	long int bufsize;
	unsigned char *buf;
	CByteArray *cert;

#ifdef WIN32
	errno_t werr;
	path += "\\"; // Quick Fix for a messy situation with the certificates subdir
#endif
	path += SubDir;
#ifdef WIN32
	path += (strlen(SubDir) != 0 ? "\\" : "");
#else
	path += (strlen(SubDir) != 0 ? "/" : "");
#endif
	path += File;

#ifdef WIN32
	if ((werr = fopen_s(&m_stream, path.c_str(), "rb")) != 0)
		goto err;
#else
	if ((m_stream = fopen(path.c_str(), "rb")) == NULL)
		goto err;
#endif

	if (fseek(m_stream, 0L, SEEK_END))
		goto err;

	bufsize = ftell(m_stream);
	buf = (unsigned char *)malloc(bufsize * sizeof(unsigned char));

	if (fseek(m_stream, 0L, SEEK_SET)) {
		free(buf);
		goto err;
	}

	if (fread(buf, sizeof(unsigned char), bufsize, m_stream) != bufsize) {
		free(buf);
		goto err;
	}

	cert = new CByteArray(buf, bufsize);
	certifs->addCert(*cert, APL_CERTIF_TYPE_UNKNOWN);
	delete cert;

	free(buf);
	fclose(m_stream);
	return;

err:
	if (m_stream)
		fclose(m_stream);

	MWLOG(LEV_DEBUG, MOD_APL, L"APL_Certifs::foundCertificate: problem with file %ls ",
		  utilStringWiden(std::string(path)).c_str());
}

APL_Certif *APL_Certifs::findIssuer(const APL_Certif *cert) {
	APL_Certif *issuer = NULL;
	// First we look in the already loaded
	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		issuer = itr->second;
		if (m_cryptoFwk->isIssuer(cert->getData(), issuer->getData())) {
			return issuer;
		}
	}

	APL_Certif *downloadedCert = NULL;
	/* cert_dl_count avoids infinite or too long cert download sequences */
	if (m_card != NULL && m_card->getType() == APL_CARDTYPE_PTEID_IAS5 && cert_dl_count < CERT_MAX_DOWNLOADS)
		downloadedCert = downloadCAIssuerCertificate(cert);

	return downloadedCert;
}

APL_Certif *APL_Certifs::findCrlIssuer(const CByteArray &crldata) {
	APL_Certif *issuer = NULL;

	// First we look in the already loaded
	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		issuer = itr->second;
		if (m_cryptoFwk->isCrlIssuer(crldata, issuer->getData()))
			return issuer;
	}

	// TODO

	// If still not found we can go and see in windows certificate store

	return NULL;
}

void APL_Certifs::resetFlags() {
	// Reset the issuer and root flags
	// the order is important
	resetIssuers(); // First we define the issuers
	resetRoots();	// Then set the root flag
}

void APL_Certifs::resetIssuers() {
	APL_Certif *cert = NULL;

	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		cert = itr->second;
		cert->resetIssuer();
	}
}

void APL_Certifs::resetRoots() {
	APL_Certif *cert = NULL;

	std::map<unsigned long, APL_Certif *>::const_iterator itr;
	for (itr = m_certifs.begin(); itr != m_certifs.end(); itr++) {
		cert = itr->second;
		cert->resetRoot();
	}
}

APL_Certif *APL_Certifs::downloadCAIssuerCertificate(const APL_Certif *cert) {
	std::string caIssuerUrl;
	m_cryptoFwk->GetCAIssuerUrl(cert->getData(), caIssuerUrl);
	if (caIssuerUrl.empty())
		return NULL;

	PKIFetcher pkiFetcher;
	MWLOG(LEV_DEBUG, MOD_APL, "APL_Cert::downloadCAIssuerCertificate: Trying to download issuer certificate url: %s",
		  caIssuerUrl.c_str());
	CByteArray issuer_cert_data;
	issuer_cert_data = pkiFetcher.fetch_PKI_file(caIssuerUrl.c_str());
	if (issuer_cert_data.Size() == 0) {
		issuer_cert_data = EmptyByteArray;
		MWLOG(LEV_ERROR, MOD_APL, "APL_Cert::downloadCAIssuerCertificate: Unable to download issuer certificate");
		return NULL;
	}

	if (m_cryptoFwk->isSelfIssuer(issuer_cert_data)) {
		MWLOG(LEV_WARN, MOD_APL, "APL_Cert::downloadCAIssuerCertificate: discarding downloaded root certificate!");
		return NULL;
	}
	cert_dl_count++;

	APL_Certif *issuerCertObj = addCert(issuer_cert_data, APL_CERTIF_TYPE_UNKNOWN, true);
	reOrderCerts();
	if (issuerCertObj != NULL) {
#ifdef WIN32
		APL_Config cachePath(CConfig::isTestModeEnabled() ? CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST
														  : CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
#else
		APL_Config cachePath(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR_CERTS);
		struct stat buffer;
		if (stat(cachePath.getString(), &buffer)) {
			mkdir(cachePath.getString(), 0700);
		}
#endif
		std::string certPath =
			std::string(cachePath.getString()) + "/" + issuerCertObj->getSerialNumber() + "." + m_certExtension;
		FILE *certWrite = fopen(certPath.c_str(), "wb");
		if (certWrite != NULL) {
			fwrite(issuer_cert_data.GetBytes(), sizeof(unsigned char), issuer_cert_data.Size(), certWrite);
			fclose(certWrite);
		} else {
			MWLOG(LEV_ERROR, MOD_APL, "Error trying to open certificate to write at %s, cert owner name: %s",
				  issuerCertObj->getOwnerName(), certPath.c_str());
		}
	}
	return issuerCertObj;
}

APL_SmartCard *APL_Certifs::getCard() { return m_card; }

/*****************************************************************************************
---------------------------------------- APL_Certif --------------------------------------
*****************************************************************************************/
APL_Certif::APL_Certif(APL_SmartCard *card, APL_Certifs *store, unsigned long ulIndex) {
	m_cryptoFwk = AppLayer.getCryptoFwk();
	m_statusCache = AppLayer.getCertStatusCache();

	m_ulIndex = ulIndex;

	m_type = APL_CERTIF_TYPE_UNKNOWN;

	m_certP15 = card->getP15Cert(ulIndex);
	m_certP15Ok = true;

	m_store = store;
	if (card->getType() == APL_CARDTYPE_PTEID_IAS5) {
		CByteArray CC2_EID = {PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)};
		m_certFile = new APL_CardFile_Certificate(card, m_certP15.csPath.c_str(), NULL, CC2_EID);
	} else {
		m_certFile = new APL_CardFile_Certificate(card, m_certP15.csPath.c_str());
	}
	setCardCertificateType();
	m_delCertFile = true;

	m_initInfo = false;
	m_onCard = true;

	m_issuer = NULL;

	m_test = 0;
	m_root = 0;

	m_countChildren = 0xFFFFFFFF;

	m_crl = NULL;
	m_ocsp = NULL;

	m_info = NULL;
}

void APL_Certif::setCardCertificateType() {
	switch (m_certP15.ulID) // From File EF0C: we are using CertIDs: 45, 46, 51, 52, 50 (Auth, Signature,
	// Signature SubCA, Authentication SubCA, RootCA)
	{
	case 0x45:
		m_type = APL_CERTIF_TYPE_AUTHENTICATION;
		break;
	case 0x46:
		m_type = APL_CERTIF_TYPE_SIGNATURE;
		break;
	case 0x51:
		m_type = APL_CERTIF_TYPE_ROOT_SIGN;
		break;
	case 0x52:
		m_type = APL_CERTIF_TYPE_ROOT_AUTH;
		break;
	case 0x50:
		m_type = APL_CERTIF_TYPE_ROOT;
		break;
	}
}



APL_Certif::APL_Certif(APL_Certifs *store, APL_CardFile_Certificate *file, APL_CertifType type, bool bOnCard,
					    unsigned long ulIndex, const CByteArray *cert) {
	m_cryptoFwk = AppLayer.getCryptoFwk();
	m_statusCache = AppLayer.getCertStatusCache();

	m_ulIndex = ulIndex;

	m_type = type;

	m_store = store;
	if (file) {
		m_certFile = file;
		m_delCertFile = false;
	} else {
		m_certFile = new APL_CardFile_Certificate(store->getCard(), "", cert);
		m_delCertFile = true;
	}

	m_initInfo = false;

	m_issuer = NULL;

	m_onCard = bOnCard;

	m_test = 0;
	m_root = -1;

	m_countChildren = 0xFFFFFFFF;

	m_crl = NULL;
	m_ocsp = NULL;

	m_info = NULL;
}

APL_Certif::APL_Certif(APL_Certifs *store, const CByteArray &cert, APL_CertifType type) {
	m_cryptoFwk = AppLayer.getCryptoFwk();
	m_statusCache = AppLayer.getCertStatusCache();

	m_ulIndex = ANY_INDEX;

	m_type = type;

	m_store = store;
	m_certFile = new APL_CardFile_Certificate(store->getCard(), "", &cert);
	m_delCertFile = true;

	m_initInfo = false;

	m_issuer = NULL;

	m_onCard = false;

	m_test = -1;
	m_root = -1;

	m_countChildren = 0xFFFFFFFF;

	m_crl = NULL;
	m_ocsp = NULL;

	m_info = NULL;
}

APL_Certif::~APL_Certif(void) {
	if (m_delCertFile && m_certFile) {
		delete m_certFile;
		m_certFile = NULL;
	}

	if (m_crl) {
		delete m_crl;
		m_crl = NULL;
	}

	if (m_ocsp) {
		delete m_ocsp;
		m_ocsp = NULL;
	}

	if (m_info) {
		delete m_info;
		m_info = NULL;
	}
}

APL_CertifType APL_Certif::getType() const {

	return m_type;
}

unsigned long APL_Certif::getIndexOnCard() const { return m_ulIndex; }

const char *APL_Certif::getLabel() {
	if (m_certP15.csLabel.empty())
		return getOwnerName();

	return m_certP15.csLabel.c_str();
}

unsigned long APL_Certif::getID() const { return m_certP15.ulID; }

unsigned long APL_Certif::getUniqueId() const { return m_certFile->getUniqueId(); }

const CByteArray &APL_Certif::getData() const { return m_certFile->getData(); }

void APL_Certif::getFormattedData(CByteArray &data) const {
	const CByteArray &raw_cert_data = m_certFile->getData();
	long cert_len = der_certificate_length(raw_cert_data);
	data = CByteArray(raw_cert_data.GetBytes(), cert_len);
}

void APL_Certif::resetIssuer() { m_issuer = m_store->findIssuer(this); }

void APL_Certif::resetRoot() {
	if (m_cryptoFwk->isSelfIssuer(getData()))
		m_root = 1;
	else
		m_root = 0;
}

APL_Certif *APL_Certif::getIssuer() {
	// If this is the root, there is no issuer
	if (m_root) {
		return this;
	}
	return m_issuer;
}

APL_Certif *APL_Certif::getRoot() {
	if (m_root)
		return this;

	if (m_issuer)
		return m_issuer->getRoot();

	return this;
}

unsigned long APL_Certif::countChildren(bool bForceRecount) {
	if (bForceRecount || m_countChildren == 0xFFFFFFFF) {
		m_countChildren = m_store->countChildren(this);
	}

	return m_countChildren;
}

APL_Certif *APL_Certif::getChildren(unsigned long ulIndex) { return m_store->getChildren(this, ulIndex); }

bool APL_Certif::isTest()  { return (m_test != 0); }

bool APL_Certif::isType(APL_CertifType type) const { return (getType() == type); }

bool APL_Certif::isRoot() const { return (m_root == 1); }

bool APL_Certif::isAuthentication() const { return isType(APL_CERTIF_TYPE_AUTHENTICATION); }

bool APL_Certif::isSignature() const { return isType(APL_CERTIF_TYPE_SIGNATURE); }

bool APL_Certif::isCA() const { return isType(APL_CERTIF_TYPE_ROOT_AUTH); }

bool APL_Certif::isFromPteidValidChain() {
	APL_Certif *root = getRoot();

	if (root) {
		if (root->isTest())
			return false;
		else
			return true;
	} else
		return false;
}

tCardFileStatus APL_Certif::getFileStatus() { return m_certFile->getStatus(false); }

APL_CertifStatus APL_Certif::getStatus(bool useCache, bool validateChain) {
	APL_ValidationLevel crl = APL_VALIDATION_LEVEL_NONE;
	APL_ValidationLevel ocsp = APL_VALIDATION_LEVEL_MANDATORY;
	MWLOG(LEV_DEBUG, MOD_APL, "APL_Certif::getStatus for certificate. Certificate Label: %s", this->getLabel());
	/*MWLOG(LEV_DEBUG, MOD_APL, "APL_Certif::getStatus for cert: %s", this->getOwnerName());*/

	return getStatus(crl, ocsp, useCache, validateChain);
}

APL_CertifStatus APL_Certif::getStatus(APL_ValidationLevel crl, APL_ValidationLevel ocsp, bool useCache,
									   bool validateChain) {
	// CSC_Status statusNone=CSC_STATUS_NONE;
	// CSC_Status statusCrl=CSC_STATUS_NONE;
	CSC_Status statusOcsp = CSC_STATUS_NONE;

	statusOcsp = m_statusCache->getCertStatus(getUniqueId(), CSC_VALIDATION_OCSP, m_store, useCache, validateChain);
	// fprintf(stderr, "DEBUG APL_Certif::getStatus() returned: %d\n", statusOcsp);

	// If no crl neither ocsp and valid => VALID
	if (statusOcsp == CSC_STATUS_VALID_SIGN)
		return APL_CERTIF_STATUS_VALID;
	else if (statusOcsp == CSC_STATUS_SUSPENDED)
		return APL_CERTIF_STATUS_SUSPENDED;
	else if (statusOcsp == CSC_STATUS_REVOKED)
		return APL_CERTIF_STATUS_REVOKED;
	else if (statusOcsp == CSC_STATUS_DATE)
		return APL_CERTIF_STATUS_EXPIRED;
	else if (statusOcsp == CSC_STATUS_CONNECT)
		return APL_CERTIF_STATUS_CONNECT;
	else
		return APL_CERTIF_STATUS_ERROR;
}

APL_Crl *APL_Certif::getCRL() {
	if (!m_crl) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instance
		if (!m_crl) {
			std::string url;
			std::string delta_url;
			// This will get both CRLs, the main one and the freshest CRL.
			m_cryptoFwk->GetCDPUrl(getData(), delta_url, NID_freshest_crl);

			if (m_cryptoFwk->GetCDPUrl(getData(), url, NID_crl_distribution_points)) {
				MWLOG(LEV_DEBUG, MOD_APL, "Creating APL_CRL with main CRL: %s and deltaCRL: %s", url.c_str(),
					  delta_url.empty() ? "(empty)" : delta_url.c_str());
				m_crl = new APL_Crl(url.c_str(), delta_url.c_str(), m_cryptoFwk->getCertSerialNumber(getData()));
			}
		}
	}
	return m_crl;
}

APL_OcspResponse *APL_Certif::getOcspResponse() {
	if (!m_ocsp) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instance
		if (!m_ocsp) {
			std::string url;
			if (m_cryptoFwk->GetOCSPUrl(getData(), url))
				m_ocsp = new APL_OcspResponse(url.c_str(), this);
		}
	}

	return m_ocsp;
}

APL_CertifStatus APL_Certif::validationCRL() {
	MWLOG(LEV_DEBUG, MOD_APL, "APL_Certif::validationCRL() for certificate %s", this->getLabel());
	// GETS CRL
	APL_Crl *crl = getCRL();

	// If there is no crl (ex. root), validation is ok
	if (!crl)
		return APL_CERTIF_STATUS_VALID;

	return crl->verifyCert(false);
}

APL_CertifStatus APL_Certif::verifyCRL(bool forceDownload) {
	return getStatus(APL_VALIDATION_LEVEL_MANDATORY, APL_VALIDATION_LEVEL_NONE);
}

APL_CertifStatus APL_Certif::validationOCSP() {
	APL_OcspResponse *ocsp = getOcspResponse();

	// This cert does not contain an OCSP URL
	if (!ocsp)
		return APL_CERTIF_STATUS_UNKNOWN;

	return ocsp->verifyCert();
}

APL_CertifStatus APL_Certif::verifyOCSP() {
	return getStatus(APL_VALIDATION_LEVEL_NONE, APL_VALIDATION_LEVEL_MANDATORY);
}

CByteArray APL_Certif::getOCSPResponse() {
	CByteArray response;

	APL_Certif *issuer = NULL;

	if (isRoot())
		issuer = this;
	else
		issuer = getIssuer();

	if (issuer == NULL)
		return response;

	if (m_cryptoFwk->GetOCSPResponse(getData(), issuer->getData(), &response))
		return response;

	return response;
}

void APL_Certif::initInfo() {
	if (m_initInfo)
		return;

	if (!m_info) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_info)
			m_info = new tCertifInfo;
	}

	if (m_cryptoFwk->getCertInfo(getData(), *m_info))
		m_initInfo = true;
}

bool APL_Certif::verifyDateValidity() { return m_cryptoFwk->VerifyDateValidity(getData()); }

bool APL_Certif::isFromCard() { return m_onCard; }

APL_Certifs *APL_Certif::getCertificates() { return m_store; }

const char *APL_Certif::getSerialNumber() {
	initInfo();

	return m_info->serialNumber.c_str();
}

const char *APL_Certif::getOwnerName() {
	initInfo();

	return m_info->ownerName.c_str();
}

const char *APL_Certif::getSubjectSerialNumber() {
	initInfo();

	return m_info->subjectSerialNumber.c_str();
}

const char *APL_Certif::getIssuerName() {
	initInfo();

	return m_info->issuerName.c_str();
}

const char *APL_Certif::getValidityBegin() {
	initInfo();

	return m_info->validityNotBefore.c_str();
}

const char *APL_Certif::getValidityEnd() {
	initInfo();

	return m_info->validityNotAfter.c_str();
}

unsigned long APL_Certif::getKeyLength() {
	initInfo();

	return m_info->keyLength;
}

/*****************************************************************************************
---------------------------------------- APL_Crl --------------------------------------
*****************************************************************************************/
/*
APL_Crl::APL_Crl(const char *uri, const char *delta_uri)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();

	m_uri=uri;
	m_delta_uri=delta_uri;

	m_initOk=false;

	m_certif=NULL;
	m_issuer=NULL;

	m_info=NULL;
}*/

/*
APL_Crl::APL_Crl(const char *uri, const char *delta_uri, APL_Certif *certif)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	//m_cache=AppLayer.getCrlDownloadCache();

	m_uri=uri;
	m_delta_uri=delta_uri;

	m_initOk=false;


	m_serial_number=m_cryptoFwk->getCertSerialNumber(certif->getData());
	m_certif=certif;
	m_issuer=NULL;

	m_info=NULL;
}
*/

APL_Crl::APL_Crl(const char *uri, const char *delta_uri, void *serial_number) {
	m_cryptoFwk = AppLayer.getCryptoFwk();
	// m_cache=AppLayer.getCrlDownloadCache();

	m_uri = uri;
	m_delta_uri = delta_uri;

	m_initOk = false;

	m_serial_number = serial_number;
	/*m_certif=NULL;
	m_issuer=NULL;*/

	m_info = NULL;
}

APL_Crl::~APL_Crl(void) {
	if (m_serial_number) {
		ASN1_INTEGER_free(reinterpret_cast<ASN1_INTEGER *>(m_serial_number));
	}
}

// Initialize the member if not yet done (m_init=false) or forced by passing crlIn!=NULL
/*
void APL_Crl::init()
{
	if(!m_initOk)
	{
		CByteArray data;
		getData(data);
	}
}*/

const char *APL_Crl::getUri() { return m_uri.c_str(); }

APL_CertifStatus APL_Crl::verifyCert(bool forceDownload) {
	/*
	if(!m_certif)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);
	*/

	FWK_CertifStatus eStatus;
	CByteArray baCrl;
	CByteArray baDeltaCRL;

	// Gets the CRL
	switch (getData(baCrl, m_uri)) {
	case APL_CRL_STATUS_ERROR:
		return APL_CERTIF_STATUS_ERROR;
	case APL_CRL_STATUS_CONNECT:
		return APL_CERTIF_STATUS_CONNECT;
	case APL_CRL_STATUS_UNKNOWN:
	case APL_CRL_STATUS_VALID:
	default:
		break;
	}

	if (!m_delta_uri.empty()) {

		// Gets the Delta CRL
		switch (getData(baDeltaCRL, m_delta_uri)) {
		case APL_CRL_STATUS_ERROR:
			return APL_CERTIF_STATUS_ERROR;
		case APL_CRL_STATUS_CONNECT:
			return APL_CERTIF_STATUS_CONNECT;
		case APL_CRL_STATUS_UNKNOWN:
		case APL_CRL_STATUS_VALID:
		default:
			break;
		}
	}

	// Gets an updated CRL from the CRL and the Delta CRL
	X509_CRL *updated_CRL = m_cryptoFwk->updateCRL(baCrl, baDeltaCRL);

	MWLOG(LEV_DEBUG, MOD_APL, "APL_Crl::verifyCert validating cert in CRL and deltaCRL");
	// Validates CRL
	eStatus = m_cryptoFwk->CRLValidation(reinterpret_cast<ASN1_INTEGER *>(m_serial_number), updated_CRL);

	// Returns the Status
	return ConvertStatus(eStatus, APL_VALIDATION_PROCESS_CRL);
}

// Get data from the file and make the verification
APL_CrlStatus APL_Crl::getData(CByteArray &data, std::string &crl_uri) {
	PKIFetcher crl_fetcher;
	APL_CrlStatus eRetStatus = APL_CRL_STATUS_ERROR;
	// Can be changed to update with delta CRL
	data = crl_fetcher.fetch_PKI_file(crl_uri.c_str());

	// If ok, we get the info, unless we return an empty bytearray
	if (data.Size() == 0) {
		data = EmptyByteArray;
		MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: Returning an empty array");
		eRetStatus = APL_CRL_STATUS_CONNECT;
	} else
		eRetStatus = APL_CRL_STATUS_VALID;

	m_initOk = true;

	return eRetStatus;
}

/*
APL_Certif *APL_Crl::getIssuer()
{
	if(!m_certif)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	init();

	return m_issuer;
}

const char *APL_Crl::getIssuerName()
{
	init();

	return m_info->issuerName.c_str();
}*/

/*****************************************************************************************
---------------------------------------- APL_OcspResponse --------------------------------------
*****************************************************************************************/
APL_OcspResponse::APL_OcspResponse(const char *uri, APL_Certif *certif) {
	MWLOG(LEV_DEBUG, MOD_APL, "OCSPResponse ctor for URI: %s", uri);
	m_cryptoFwk = AppLayer.getCryptoFwk();

	m_uri = uri;
	m_certif = certif;

	m_certid = NULL;

	m_response = NULL;
	m_status = APL_CERTIF_STATUS_UNCHECK;

	APL_Config conf_NormalDelay(
		CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY); // The validity is the same as for certificate status cache
	m_delay = conf_NormalDelay.getLong();
}

APL_OcspResponse::APL_OcspResponse(const char *uri, FWK_HashAlgo hashAlgorithm, const CByteArray &issuerNameHash,
								   const CByteArray &issuerKeyHash, const CByteArray &serialNumber) {
	m_cryptoFwk = AppLayer.getCryptoFwk();

	m_uri = uri;
	m_certif = NULL;

	m_certid = new tOcspCertID;

	m_response = NULL;
	m_status = APL_CERTIF_STATUS_UNCHECK;

	m_certid->hashAlgorithm = hashAlgorithm;

	m_certid->issuerNameHash = new CByteArray(issuerNameHash);
	m_certid->issuerKeyHash = new CByteArray(issuerKeyHash);
	m_certid->serialNumber = new CByteArray(serialNumber);

	APL_Config conf_NormalDelay(
		CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY); // The validity is the same as for certificate status cache
	m_delay = conf_NormalDelay.getLong();
}

APL_OcspResponse::~APL_OcspResponse(void) {
	if (m_certid) {
		delete m_certid;
		m_certid = NULL;
	}

	if (m_response) {
		delete m_response;
		m_response = NULL;
	}
}

const char *APL_OcspResponse::getUri() { return m_uri.c_str(); }

APL_CertifStatus APL_OcspResponse::verifyCert() { return getResponse(NULL); }

APL_CertifStatus APL_OcspResponse::getResponse(CByteArray &response) { return getResponse(&response); }

APL_CertifStatus APL_OcspResponse::getResponse(CByteArray *response) {
	MWLOG(LEV_DEBUG, MOD_APL, L"getOCSPResponse called");
	// If we already have a response, we check if the status was acceptable and if it's still valid
	if (m_response) {
		if ((m_status == APL_CERTIF_STATUS_VALID_OCSP || m_status == APL_CERTIF_STATUS_REVOKED ||
			 m_status == APL_CERTIF_STATUS_SUSPENDED) &&
			CTimestampUtil::checkTimestamp(m_validity, CSC_VALIDITY_FORMAT)) {
			if (response)
				*response = *m_response;

			return m_status;
		} else {
			m_response->ClearContents(); // If we had to make a request again...
		}
	} else {
		m_response = new CByteArray; // First response
	}

	FWK_CertifStatus status = FWK_CERTIF_STATUS_UNCHECK;

	if (m_certif) {
		APL_Certif *issuer = m_certif->getIssuer();

		if (issuer == NULL)
			issuer = m_certif;

		status = m_cryptoFwk->GetOCSPResponse(m_certif->getData(), issuer->getData(), m_response);
	} else {
		/* XX: OpenSSL 1.1 migration: this condition is never hit  */
		//	status=m_cryptoFwk->GetOCSPResponse(m_uri.c_str(),*m_certid,m_response);
	}

	if (response)
		*response = *m_response;

	CTimestampUtil::getTimestamp(m_validity, m_delay, CSC_VALIDITY_FORMAT);
	m_status = ConvertStatus(status, APL_VALIDATION_PROCESS_OCSP);

	return m_status;
}

} // namespace eIDMW
