/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2017, 2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2013 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#include "APLReader.h"
#include "APLCard.h"
#include "APLCrypto.h"
#include "APLCertif.h"
#include "APLConfig.h"
#include "CardFile.h"
#include "CardLayer.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "XadesSignature.h"
#include "SigContainer.h"
#include "PDFSignature.h"
#include "MiscUtil.h"
#include "Log.h"
#include "SODParser.h"

#include <time.h>
#include <sys/types.h>

namespace eIDMW {

#define CHALLENGE_LEN 20

/*****************************************************************************************
---------------------------------------- APL_Card --------------------------------------------
*****************************************************************************************/
APL_CryptoFwk *APL_Card::m_cryptoFwk = NULL;

APL_Card::APL_Card(APL_ReaderContext *reader) {
	if (!m_cryptoFwk)
		m_cryptoFwk = AppLayer.getCryptoFwk();

	m_reader = reader;
}

APL_Card::~APL_Card() {}

CReader *APL_Card::getCalReader() const { return m_reader->getCalReader(); }

APL_CardType APL_Card::getType() const { return APL_CARDTYPE_UNKNOWN; }

void APL_Card::CalLock() { return m_reader->CalLock(); }

void APL_Card::CalUnlock() { return m_reader->CalUnlock(); }

unsigned long APL_Card::readFile(const char *csPath, CByteArray &oData, unsigned long ulOffset,
								 unsigned long ulMaxLength) {

	// BEGIN_CAL_OPERATION(m_reader)
	m_reader->CalLock();
	try {
		oData = m_reader->getCalReader()->ReadFile(csPath, ulOffset, (ulMaxLength == 0 ? FULL_FILE : ulMaxLength));
	} catch (CMWException &e) {
		MWLOG(LEV_ERROR, MOD_APL,
			  "Error trying to read file error: 0x%x file: %s line: %d, csPath: %s, uloffset: %lu, ulmaxlen: %lu",
			  e.GetError(), e.GetFile().c_str(), e.GetLine(), csPath, ulOffset, ulMaxLength);
		m_reader->CalUnlock();
		if (e.GetError() == EIDMW_ERR_INCOMPATIBLE_READER)
			throw e;

		return 0;
	} catch (...) {
		MWLOG(LEV_ERROR, MOD_APL, "Error trying to read file csPath: %s, uloffset: %lu, ulmaxlen: %lu", csPath,
			  ulOffset, ulMaxLength);
		m_reader->CalUnlock();
		throw;
		return 0;
	}
	m_reader->CalUnlock();

	// END_CAL_OPERATION(m_reader)

	return oData.Size();
}

bool APL_Card::writeFile(const char *csPath, const CByteArray &oData, unsigned long ulOffset) {

	BEGIN_CAL_OPERATION(m_reader)
	m_reader->getCalReader()->WriteFile(csPath, ulOffset, oData);
	END_CAL_OPERATION(m_reader)

	return true; // Otherwise, there is exception
}

void APL_Card::initPaceAuthentication(const char *secret, size_t secretLen, APL_PACEAuthenticationType secretType) {
	PaceSecretType paceSecretType = PaceSecretType::PACECAN;

	if (secretType == APL_PACEAuthenticationType::APL_PACE_CAN)
		paceSecretType = PaceSecretType::PACECAN;
	else if (secretType == APL_PACEAuthenticationType::APL_PACE_MRZ) {
		paceSecretType = PaceSecretType::PACEMRZ;
	} else {
		// not supported types
	}

	BEGIN_CAL_OPERATION(m_reader)
	m_reader->getCalReader()->initPaceAuthentication(secret, secretLen, paceSecretType);
	END_CAL_OPERATION(m_reader)
}

CByteArray APL_Card::sendAPDU(const CByteArray &cmd) {

	CByteArray out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->SendAPDU(cmd);
	END_CAL_OPERATION(m_reader)

	return out;
}

CByteArray APL_Card::Sign(const CByteArray &oData, bool signatureKey, const unsigned long paddingType) {
	CByteArray out;
	BEGIN_CAL_OPERATION(m_reader)
	tPrivKey signing_key;
	// Private key IDs can be found with pkcs15-tool --list-keys from OpenSC package
	if (signatureKey)
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x46);
	else
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x45);

	out = m_reader->getCalReader()->Sign(signing_key, paddingType, oData);
	END_CAL_OPERATION(m_reader)

	return out;
}

int APL_Card::SignPDF(PDFSignature *pdf_sig, const char *location, const char *reason, const char *outfile_path) {

	if (pdf_sig) {
		pdf_sig->setCard(this);
		return pdf_sig->signFiles(location, reason, outfile_path, true);
	}

	return -1;
}

CByteArray &APL_Card::SignXades(const char **paths, unsigned int n_paths, const char *output_path,
								APL_SignatureLevel level) {
	if (paths == NULL || n_paths < 1 || !CPathUtil::checkExistingFiles(paths, n_paths))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	if (level == LEVEL_LT) {
		MWLOG(LEV_ERROR, MOD_CMD, "APL_Card::SignXades(): Signature Level LEVEL_LT is not supported.");
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	XadesSignature sig(this);

	if (level == LEVEL_TIMESTAMP) {
		sig.enableTimestamp();
	} else if (level == LEVEL_LTV) {
		sig.enableLongTermValidation();
	}

	CByteArray &signature = sig.signXades(paths, n_paths);

	if (sig.shouldThrowTimestampException()) {
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);
	}

	if (sig.shouldThrowLTVException()) {
		throw CMWEXCEPTION(EIDMW_LTV_ERROR);
	}

	SigContainer::createASiC(signature, paths, n_paths, output_path);

	// Write zip container signature and referenced files in zip container

	return signature;
}

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

char *generateFinalPath(const char *output_dir, const char *path) {

	char *zip_filename = Basename((char *)path);
	// We need a local copy of basename(path) because basename may return a pointer to some part
	// of its argument and we don't want to change the input by replacing the last dot
	char *tmp_path = new char[strlen(zip_filename) + 1];
	strcpy(tmp_path, zip_filename);

	replace_lastdot_inplace(tmp_path);

	// Buffer for the filename components plus ".zip" plus PATH_SEP and terminating NULL
	char *final_path = new char[strlen(output_dir) + strlen(tmp_path) + 6 + 1 + 1];
	sprintf(final_path, "%s" PATH_SEP "%s.zip", output_dir, tmp_path);
	delete[] tmp_path;

	return final_path;
}

void APL_Card::SignXadesIndividual(const char **paths, unsigned int n_paths, const char *output_dir) {
	SignIndividual(paths, n_paths, output_dir, false, false);
}

void APL_Card::SignXadesTIndividual(const char **paths, unsigned int n_paths, const char *output_dir) {
	SignIndividual(paths, n_paths, output_dir, true, false);
}

void APL_Card::SignXadesAIndividual(const char **paths, unsigned int n_paths, const char *output_dir) {
	SignIndividual(paths, n_paths, output_dir, false, true);
}

// Implementation of the PIN-caching version of SignXades()
// It signs each input file seperately and creates a .zip container for each
void APL_Card::SignIndividual(const char **paths, unsigned int n_paths, const char *output_dir, bool timestamp,
							  bool xades_a) {
	bool throwTimestampException = false;
	bool throwLTVException = false;

	if (paths == NULL || n_paths < 1 || !CPathUtil::checkExistingFiles(paths, n_paths))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	const char *files_to_sign[1];

	for (unsigned int i = 0; i != n_paths; i++) {
		XadesSignature sig(this);
		if (timestamp)
			sig.enableTimestamp();
		else if (xades_a)
			sig.enableLongTermValidation();

		CByteArray *ts_data = NULL;

		files_to_sign[0] = paths[i];
		CByteArray &signature = sig.signXades(files_to_sign, 1);

		if (sig.shouldThrowTimestampException())
			throwTimestampException = true;

		if (sig.shouldThrowLTVException())
			throwLTVException = true;

		const char *output_file = generateFinalPath(output_dir, paths[i]);
		SigContainer::createASiC(signature, files_to_sign, 1, output_file);
		delete[] output_file;

		// Set SSO on after first iteration to avoid more PinCmd() user interaction for the remaining
		//  iterations
		if (i == 0)
			getCalReader()->setSSO(true);

		delete &signature;
	}

	getCalReader()->setSSO(false);

	if (throwTimestampException)
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);

	if (throwLTVException)
		throw CMWEXCEPTION(EIDMW_LTV_ERROR);
}

CByteArray &APL_Card::SignXadesT(const char **paths, unsigned int n_paths, const char *output_file) {
	if (paths == NULL || n_paths < 1 || !CPathUtil::checkExistingFiles(paths, n_paths))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	XadesSignature sig(this);
	sig.enableTimestamp();

	CByteArray &signature = sig.signXades(paths, n_paths);

	// Write zip container signature and referenced files in zip container
	SigContainer::createASiC(signature, paths, n_paths, output_file);

	if (sig.shouldThrowTimestampException())
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);

	return signature;
}

CByteArray &APL_Card::SignXadesA(const char **paths, unsigned int n_paths, const char *output_file) {
	if (paths == NULL || n_paths < 1 || !CPathUtil::checkExistingFiles(paths, n_paths))
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	XadesSignature sig(this);
	sig.enableLongTermValidation();

	CByteArray &signature = sig.signXades(paths, n_paths);

	// Write zip container signature and referenced files in zip container
	SigContainer::createASiC(signature, paths, n_paths, output_file);

	if (sig.shouldThrowTimestampException())
		throw CMWEXCEPTION(EIDMW_TIMESTAMP_ERROR);

	if (sig.shouldThrowLTVException())
		throw CMWEXCEPTION(EIDMW_LTV_ERROR);

	return signature;
}

void APL_Card::SignASiC(const char *path, APL_SignatureLevel level) {
	if (path == NULL || !CPathUtil::checkExistingFiles(&path, 1)) {
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

	XadesSignature sig(this);
	if (level == LEVEL_TIMESTAMP) {
		sig.enableTimestamp();
	} else if (level == LEVEL_LTV) {
		sig.enableLongTermValidation();
	}

	sig.signASiC(path);
}

/*****************************************************************************************
---------------------------------------- APL_SmartCard -----------------------------------
*****************************************************************************************/
APL_SmartCard::APL_SmartCard(APL_ReaderContext *reader) : APL_Card(reader) {
	m_pins = NULL;
	m_certs = NULL;
	m_fileinfo = NULL;

	APL_Config conf_allowTest(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_ALLOWTESTC);
	m_allowTestParam = conf_allowTest.getLong() ? true : false;

	m_allowTestAsked = false;
	m_allowTestAnswer = false;

	m_RootCAPubKey = NULL;

	m_certificateCount = COUNT_UNDEF;
	m_pinCount = COUNT_UNDEF;
}

APL_SmartCard::~APL_SmartCard() {
	if (m_pins) {
		delete m_pins;
		m_pins = NULL;
	}

	if (m_certs) {
		delete m_certs;
		m_certs = NULL;
	}

	if (m_fileinfo) {
		delete m_fileinfo;
		m_fileinfo = NULL;
	}

	if (m_RootCAPubKey) {
		delete m_RootCAPubKey;
		m_RootCAPubKey = NULL;
	}
}

APL_CardFile_Info *APL_SmartCard::getFileInfo() {
	if (!m_fileinfo) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for unly one instanciation
		if (!m_fileinfo) // We test again to be sure it isn't instanciated between the first if and the lock
		{
			m_fileinfo = new APL_CardFile_Info(this);
		}
	}

	return m_fileinfo;
}

void APL_SmartCard::selectApplication(const CByteArray &applicationId) const {

	BEGIN_CAL_OPERATION(m_reader)
	m_reader->getCalReader()->SelectApplication(applicationId);
	END_CAL_OPERATION(m_reader)
}

CByteArray APL_SmartCard::sendAPDU(const CByteArray &cmd, APL_Pin *pin, const char *csPinCode) {

	unsigned long lRemaining = 0;
	if (pin)
		if (csPinCode != NULL)
			pin->verifyPin(csPinCode, lRemaining);

	return APL_Card::sendAPDU(cmd);
}

unsigned long APL_SmartCard::readFile(const char *csPath, CByteArray &oData, unsigned long ulOffset,
									  unsigned long ulMaxLength) {
	return APL_Card::readFile(csPath, oData, ulOffset, ulMaxLength);
}

unsigned long APL_SmartCard::readFile(const char *fileID, CByteArray &in, APL_Pin *pin, const char *csPinCode) {
	unsigned long lRemaining = 0;
	if (pin)
		if (csPinCode != NULL)
			pin->verifyPin(csPinCode, lRemaining);

	return readFile(fileID, in, 0UL, 0UL);
}

bool APL_SmartCard::writeFile(const char *fileID, const CByteArray &out, APL_Pin *pin, const char *csPinCode,
							  unsigned long ulOffset) {
	unsigned long lRemaining = 0;

	if (pin)
		if (csPinCode != NULL)
			pin->verifyPin(csPinCode, lRemaining);

	return APL_Card::writeFile(fileID, out, ulOffset);
}

unsigned long APL_SmartCard::pinCount() {

	if (m_pinCount == COUNT_UNDEF) {
		try {
			BEGIN_CAL_OPERATION(m_reader)
			m_pinCount = m_reader->getCalReader()->PinCount();
			END_CAL_OPERATION(m_reader)
		} catch (...) {
			m_pinCount = 0;
		}
	}

	return m_pinCount;
}

APL_Pins *APL_SmartCard::getPins() {
	if (!m_pins) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for unly one instanciation
		if (!m_pins) // We test again to be sure it isn't instanciated between the first if and the lock
		{
			m_pins = new APL_Pins(this);
		}
	}

	return m_pins;
}

tPin APL_SmartCard::getPin(unsigned long ulIndex) {

	if (ulIndex >= pinCount())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	tPin out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->GetPin(ulIndex);
	END_CAL_OPERATION(m_reader)

	return out;
}

unsigned long APL_SmartCard::pinStatus(const tPin &Pin) {

	unsigned long out = 0;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->PinStatus(Pin);
	END_CAL_OPERATION(m_reader)

	return out;
}

bool APL_SmartCard::isPinVerified(const tPin &Pin) {
	bool verified = false;

	BEGIN_CAL_OPERATION(m_reader)
	verified = m_reader->getCalReader()->isPinVerified(Pin);
	END_CAL_OPERATION(m_reader)

	return verified;
}

bool APL_SmartCard::pinCmd(tPinOperation operation, const tPin &Pin, const char *csPin1In, const char *csPin2In,
						   unsigned long &ulRemaining, bool bShowDlg, void *wndGeometry) {

	bool ret = false;

	const char *csPin1 = csPin1In;
	if (!csPin1)
		csPin1 = "";

	const char *csPin2 = csPin2In;
	if (!csPin2)
		csPin2 = "";

	BEGIN_CAL_OPERATION(m_reader)
	ret = m_reader->getCalReader()->PinCmd(operation, Pin, csPin1, csPin2, ulRemaining, bShowDlg, wndGeometry);
	END_CAL_OPERATION(m_reader)

	return ret;
}

unsigned long APL_SmartCard::certificateCount() {
	if (m_certificateCount == COUNT_UNDEF) {
		BEGIN_CAL_OPERATION(m_reader)
		m_certificateCount = m_reader->getCalReader()->CertCount();
		// Minus one because we're excluding the Root Cert from card
		// (we're assuming it's always the last in the PKCS15 structure)
		if (m_reader->getCardType() != APL_CARDTYPE_PTEID_IAS5)
			m_certificateCount--;
		END_CAL_OPERATION(m_reader)
	}

	return m_certificateCount;
}

APL_Certifs *APL_SmartCard::getCertificates() {
	if (!m_certs) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instantiation
		if (!m_certs) // We test again to be sure it isn't instantiated between the first if and the lock
		{
			m_certs = new APL_Certifs(this);
			for (unsigned long ulIndex = 0; ulIndex < certificateCount(); ulIndex++) {
				APL_Certif *cert = NULL;
				cert = m_certs->getCertFromCard(ulIndex);
			}
		}
	}

	return m_certs;
}

tCert APL_SmartCard::getP15Cert(unsigned long ulIndex) {
	if (ulIndex >= certificateCount())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	tCert out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->GetCert(ulIndex);
	END_CAL_OPERATION(m_reader)

	return out;
}

// ICAO
APL_ICAO::APL_ICAO(APL_ReaderContext *reader) : m_reader(reader) {}

const std::unordered_map<APL_ICAO::DataGroupID, std::string> APL_ICAO::DATAGROUP_PATHS = {
	{APL_ICAO::DataGroupID::DG1, "0101"},  {APL_ICAO::DataGroupID::DG2, "0102"},  {APL_ICAO::DataGroupID::DG3, "0103"},
	{APL_ICAO::DataGroupID::DG4, "0104"},  {APL_ICAO::DataGroupID::DG5, "0105"},  {APL_ICAO::DataGroupID::DG6, "0106"},
	{APL_ICAO::DataGroupID::DG7, "0107"},  {APL_ICAO::DataGroupID::DG8, "0108"},  {APL_ICAO::DataGroupID::DG9, "0109"},
	{APL_ICAO::DataGroupID::DG10, "010A"}, {APL_ICAO::DataGroupID::DG11, "010B"}, {APL_ICAO::DataGroupID::DG12, "010C"},
	{APL_ICAO::DataGroupID::DG13, "010D"}, {APL_ICAO::DataGroupID::DG14, "010E"}, {APL_ICAO::DataGroupID::DG15, "010F"},
	{APL_ICAO::DataGroupID::DG16, "0110"},
};

const std::vector<int> APL_ICAO::EXPECTED_TAGS = {
	DG1, DG2, DG3, DG4, DG5, DG6, DG7, DG8, DG9, DG10, DG11, DG12, DG13, DG14, DG15, DG16,
};

std::vector<APL_ICAO::DataGroupID> APL_ICAO::getAvailableDatagroups() {
	std::vector<DataGroupID> datagroups;
	BEGIN_CAL_OPERATION(m_reader);
	{
		m_reader->getCalReader()->SelectApplication({MRTD_APPLICATION, sizeof(MRTD_APPLICATION)});

		CByteArray oData = m_reader->getCalReader()->ReadFile(SOD_PATH).GetBytes(65);

		SODParser parser;
		parser.ParseSodEncapsulatedContent(oData, EXPECTED_TAGS);

		m_SodAttributes = std::make_unique<SODAttributes>(parser.getAttributes());

		for (const auto &hash : m_SodAttributes->getHashes()) {
			datagroups.emplace_back(static_cast<DataGroupID>(hash.first));
		}
	}
	END_CAL_OPERATION(m_reader);

	return datagroups;
}

void APL_ICAO::initPaceAuthentication(const char *secret, size_t length, APL_PACEAuthenticationType secretType) {
	PaceSecretType paceSecretType = PaceSecretType::PACECAN;

	if (secretType == APL_PACEAuthenticationType::APL_PACE_CAN)
		paceSecretType = PaceSecretType::PACECAN;
	else if (secretType == APL_PACEAuthenticationType::APL_PACE_MRZ) {
		paceSecretType = PaceSecretType::PACEMRZ;
	} else {
		// not supported types
	}

	BEGIN_CAL_OPERATION(m_reader)
	m_reader->getCalReader()->initPaceAuthentication(secret, length, paceSecretType);
	END_CAL_OPERATION(m_reader)
}

CByteArray APL_ICAO::readDatagroup(APL_ICAO::DataGroupID tag) {
	CByteArray out;

	m_reader->CalLock();
	try {
		out = m_reader->getCalReader()->ReadFile(DATAGROUP_PATHS.at(tag));
	} catch (CMWException &e) {
		m_reader->CalUnlock();
		if (e.GetError() == EIDMW_ERR_INCOMPATIBLE_READER)
			throw e;

		return {};
	} catch (...) {
		m_reader->CalUnlock();
		throw;
		return {};
	}
	m_reader->CalUnlock();

	// verify sod
	auto sod = verifySOD(tag, out);
	if (!sod) {
		throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ICAO_DG);
	}

	return out;
}

bool APL_ICAO::verifySOD(DataGroupID tag, const CByteArray& data) {
	auto &hashes = m_SodAttributes->getHashes();
	if (hashes.find(tag) == hashes.end()) {
		return false;
	}

	auto cryptFwk = AppLayer.getCryptoFwk();
	return cryptFwk->VerifyHashSha256(data, hashes.at(tag));
}

} // namespace eIDMW
