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
#include "APLCard.h"
#include "APLCertif.h"
#include "APLConfig.h"
#include "APLCrypto.h"
#include "APLReader.h"
#include "CardFile.h"
#include "CardLayer.h"
#include "CardPteidDef.h"
#include "IcaoDg1.h"
#include "IcaoDg11.h"
#include "IcaoDg14.h"
#include "IcaoDg2.h"
#include "IcaoDg3.h"
#include "Log.h"
#include "MiscUtil.h"
#include "MultiPassCard.h"
#include "PDFSignature.h"
#include "PaceAuthentication.h"
#include "SODParser.h"
#include "SigContainer.h"
#include "Util.h"
#include "XadesSignature.h"
#include "aa_oids.h"
#include "cryptoFwkPteid.h"

#include <openssl/rand.h>
#include <time.h>
#include <sys/types.h>
#include <openssl/err.h>

#include <fstream>
#include <sstream>

namespace eIDMW {

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

CByteArray APL_Card::sendAPDU(const APDU &apdu) {
	CByteArray out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->SendAPDU(apdu);
	END_CAL_OPERATION(m_reader)

	return out;
}

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
	m_reader->getCalReader()->ResetApplication();
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

CByteArray APL_SmartCard::sendAPDU(const APDU &apdu, APL_Pin *pin, const char *csPinCode) {
	unsigned long lRemaining = 0;
	if (pin)
		if (csPinCode != NULL)
			pin->verifyPin(csPinCode, lRemaining);
	return APL_Card::sendAPDU(apdu);
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

void EIDMW_DocumentReport::setActiveAuthenticationReport(const EIDMW_ActiveAuthenticationReport &report) {
	if (report.type == EIDMW_ReportType::Error) {
		m_hasFailed = true;
	}

	m_activeAuthenticationReport = report;
}

const EIDMW_ActiveAuthenticationReport &EIDMW_DocumentReport::getActiveAuthenticationReport() const {
	return m_activeAuthenticationReport;
}

void EIDMW_DocumentReport::setChipAuthenticationReport(const EIDMW_ChipAuthenticationReport &report) {
	if (report.type == EIDMW_ReportType::Error) {
		m_hasFailed = true;
	}

	m_chipAuthenticationReport = report;
}

const EIDMW_ChipAuthenticationReport &EIDMW_DocumentReport::getChipAuthenticationReport() const {
	return m_chipAuthenticationReport;
}

void EIDMW_DocumentReport::setSodReport(const EIDMW_SodReport &report) {
	if (report.type == EIDMW_ReportType::Error) {
		m_hasFailed = true;
	}

	m_sodReport = report;
}

const EIDMW_SodReport &EIDMW_DocumentReport::getSodReport() const { return m_sodReport; }

void EIDMW_DocumentReport::addDataGroupReport(DataGroupID id, const EIDMW_DataGroupReport &report) {
	if (m_dataGroupReports.find(id) != m_dataGroupReports.end()) {
		return;
	}

	if (report.type == EIDMW_ReportType::Error) {
		m_hasFailed = true;
	}

	m_dataGroupReports[id] = report;
}

const EIDMW_DataGroupReport &EIDMW_DocumentReport::getDataGroupReport(DataGroupID id) const {
	if (m_dataGroupReports.find(id) != m_dataGroupReports.end()) {
		return m_dataGroupReports.at(id);
	}

	// ...
	auto [_, __] = m_card->readDatagroup(id);
	return m_dataGroupReports.at(id);
}

// ICAO
APL_ICAO::APL_ICAO(APL_ReaderContext *reader) : APL_SmartCard(reader), m_reader(reader) {}

const std::unordered_map<DataGroupID, std::string> APL_ICAO::DATAGROUP_PATHS = {
	{DataGroupID::DG1, "0101"},	 {DataGroupID::DG2, "0102"},  {DataGroupID::DG3, "0103"},  {DataGroupID::DG4, "0104"},
	{DataGroupID::DG5, "0105"},	 {DataGroupID::DG6, "0106"},  {DataGroupID::DG7, "0107"},  {DataGroupID::DG8, "0108"},
	{DataGroupID::DG9, "0109"},	 {DataGroupID::DG10, "010A"}, {DataGroupID::DG11, "010B"}, {DataGroupID::DG12, "010C"},
	{DataGroupID::DG13, "010D"}, {DataGroupID::DG14, "010E"}, {DataGroupID::DG15, "010F"}, {DataGroupID::DG16, "0110"},
};

const std::vector<int> APL_ICAO::EXPECTED_TAGS = {
	DG1, DG2, DG3, DG4, DG5, DG6, DG7, DG8, DG9, DG10, DG11, DG12, DG13, DG14, DG15, DG16,
};

APL_ICAO::~APL_ICAO() {}

void APL_ICAO::resetCardState() {
	auto reader = m_reader->getCalReader();
	unsigned char apdu_break_sm[] = {0x0C, 0xA4, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	reader->SendAPDU(CByteArray(apdu_break_sm, sizeof(apdu_break_sm)));

}

void APL_ICAO::initializeCard() {
	m_ready = true;

	selectApplication({MRTD_APPLICATION, sizeof(MRTD_APPLICATION)});
	loadAvailableDataGroups();

	m_reports.setCard(this);

	auto aa_report = performActiveAuthentication();
	m_reports.setActiveAuthenticationReport(aa_report);

	auto ca_report = performChipAuthentication();
	m_reports.setChipAuthenticationReport(ca_report);
}

std::vector<DataGroupID> APL_ICAO::getAvailableDatagroups() {
	std::vector<DataGroupID> datagroups;
	{
		if (!m_ready) {
			initializeCard();
		}

		for (const auto &hash : m_SodAttributes->getHashes()) {
			datagroups.emplace_back(static_cast<DataGroupID>(hash.first));
		}
	}

	return datagroups;
}

std::pair<EIDMW_DataGroupReport, CByteArray> APL_ICAO::readDatagroup(DataGroupID tag) {
	CByteArray out;
	EIDMW_DataGroupReport report;

	if (!m_ready) {
		initializeCard();
	}

	BEGIN_CAL_OPERATION(m_reader) {
		m_reader->getCalReader()->SelectApplication({MRTD_APPLICATION, sizeof(MRTD_APPLICATION)});
		out = readFile(DATAGROUP_PATHS.at(tag));
	}
	END_CAL_OPERATION(m_reader);

	// store hashes
	report.storedHash = m_SodAttributes->get(tag);
	AppLayer.getCryptoFwk()->GetHash(out, m_SodAttributes->getHashFunction(), &report.computedHash);
	report.type = EIDMW_ReportType::Success;

	// verify sod
	auto sod = verifySOD(tag, out);
	if (!sod) {
		report.error_code = EIDMW_SOD_ERR_HASH_NO_MATCH_ICAO_DG;
		report.type = EIDMW_ReportType::Error;
	}

	m_reports.addDataGroupReport(tag, report);

	return {report, out};
}

IcaoDg1 *APL_ICAO::readDataGroup1() {
	if (m_mrzDg1.get() != nullptr)
		return m_mrzDg1.get();

	auto [report, arrayDg1] = readDatagroup(DG1);
	if (arrayDg1.Size() == 0)
		return NULL;

	m_mrzDg1.reset(new IcaoDg1(arrayDg1.GetBytes(5, arrayDg1.Size() - 5)));
	return m_mrzDg1.get();
}

IcaoDg2 *APL_ICAO::readDataGroup2() {
	if (m_faceDg2.get() != nullptr)
		return m_faceDg2.get();

	auto [report, arrayDg2] = readDatagroup(DG2);
	if (arrayDg2.Size() == 0)
		return NULL;

	m_faceDg2.reset(new IcaoDg2(arrayDg2));
	return m_faceDg2.get();
}

IcaoDg3 *APL_ICAO::readDataGroup3() {
	if (m_fingersDg3.get() != nullptr)
		return m_fingersDg3.get();

	auto [report, arrayDg3] = readDatagroup(DG3);
	if (arrayDg3.Size() == 0)
		return NULL;

	m_fingersDg3.reset(new IcaoDg3(arrayDg3));
	return m_fingersDg3.get();
}

IcaoDg11 *APL_ICAO::readDataGroup11() {
	if (m_infoDg11.get() != nullptr)
		return m_infoDg11.get();

	auto [report, arrayDg11] = readDatagroup(DG11);
	if (arrayDg11.Size() == 0)
		return NULL;

	m_infoDg11.reset(new IcaoDg11(arrayDg11));
	return m_infoDg11.get();
}

void APL_ICAO::loadAvailableDataGroups() {
	// Already loaded SOD and datagroup list
	if (m_SodAttributes)
		return;

	CByteArray oData = readFile(SOD_PATH);

	CByteArray sod;
	auto report = verifySodFileIntegrity(oData, sod);
	m_reports.setSodReport(report);
	if (report.type == EIDMW_ReportType::Error) {
		MWLOG(LEV_ERROR, MOD_APL, "APL_ICAO: Failed to verify SOD validity");
	}

	SODParser parser;
	parser.ParseSodEncapsulatedContent(sod, EXPECTED_TAGS);

	m_SodAttributes = std::make_unique<SODAttributes>(parser.getAttributes());
}

void logFullSODFile(const CByteArray &sod_data) {
	char *sod_hex = byteArrayToHexString(sod_data.GetBytes(), sod_data.Size());
	MWLOG(LEV_DEBUG, MOD_APL, "EF.SOD: %s", sod_hex);
	free(sod_hex);
}

EIDMW_SodReport APL_ICAO::verifySodFileIntegrity(const CByteArray &data, CByteArray &out_sod) {
	EIDMW_SodReport report;

	logFullSODFile(data);
	auto csca_store = AppLayer.getCryptoFwk()->getMasterListStore();
	if (!csca_store) {
		throw CMWEXCEPTION(EIDMW_SOD_ERR_NO_MASTERLIST);
	}

	PKCS7 *p7 = nullptr;
	char *error_msg = nullptr;
	bool sod_verified = false;

	ERR_load_crypto_strings();
	OpenSSL_add_all_digests();

	const unsigned char *temp = data.GetBytes();
	size_t length = data.Size();
	temp += 4; // Skip the ASN.1 Application 23 tag + 3-byte length (DER type 77)

	p7 = d2i_PKCS7(nullptr, &temp, length);
	if (!p7) {
		error_msg = Openssl_errors_to_string();
		MWLOG(LEV_ERROR, MOD_APL, "APL_ICAO: Failed to decode SOD PKCS7 object! Openssl errors:\n%s",
			  error_msg != nullptr ? error_msg : "N/A");
		free(error_msg);
		report.type = EIDMW_ReportType::Error;
		report.error_code = EIDMW_SOD_ERR_INVALID_PKCS7;
		return report;
	}

	BIO *out = BIO_new(BIO_s_mem());
	/* Some document signing certificates may have wrong values in Extended Key Usage field which generates a "purpose"
	   validation error. Set the cert verify parameters to have unrestricted purpose X509_PURPOSE_ANY
	*/
	X509_VERIFY_PARAM *verify_params = X509_STORE_get0_param(csca_store);
	X509_VERIFY_PARAM_set_purpose(verify_params, X509_PURPOSE_ANY);
	X509_STORE_set1_param(csca_store, verify_params);

	sod_verified = PKCS7_verify(p7, nullptr, csca_store, nullptr, out, 0) == 1;

	// failed to verify SOD but we still need the contents
	if (!sod_verified) {
		report.type = EIDMW_ReportType::Error;

		// By default. In most cases, should be overwritten by the checks below
		report.error_code = EIDMW_SOD_ERR_GENERIC_VALIDATION;

		unsigned long err = ERR_peek_last_error();
		int lib = ERR_GET_LIB(err);
		int reason = ERR_GET_REASON(err);

		error_msg = Openssl_errors_to_string();
		MWLOG(LEV_ERROR, MOD_APL,
			  "verifySodFileIntegrity:: Error validating SOD signature. OpenSSL errors (%d:%d):\n%s", lib, reason,
			  error_msg ? error_msg : "N/A");
		free(error_msg);

		if ((lib == ERR_LIB_X509 && reason == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT) ||
			(lib == ERR_LIB_PKCS7 && reason == PKCS7_R_CERTIFICATE_VERIFY_ERROR)) {
			// signer isnt in the CSCA (untrusted)
			report.error_code = EIDMW_SOD_ERR_UNTRUSTED_DOC_SIGNER;
		} else if (lib == ERR_LIB_PKCS7 && reason == PKCS7_R_SIGNATURE_FAILURE) {
			// signature is not valid
			report.error_code = EIDMW_SOD_ERR_VERIFY_SOD_SIGN;
		} else if (lib == ERR_LIB_PKCS7 && reason == PKCS7_R_SIGNER_CERTIFICATE_NOT_FOUND) {
			// couldnt find signer
			report.error_code = EIDMW_SOD_ERR_SIGNER_CERT_NOT_FOUND;
		}

		PKCS7_verify(p7, nullptr, nullptr, nullptr, out, PKCS7_NOVERIFY | PKCS7_NOSIGS);

		// Get signer for debug purposes
		STACK_OF(X509) *signers = PKCS7_get0_signers(p7, nullptr, 0);
		if (sk_X509_num(signers) > 0) {
			X509 *signer = sk_X509_value(signers, 0);
			unsigned char *der = nullptr;
			int len = i2d_X509(signer, &der);
			if (len > 0) {
				report.signer = CByteArray(der, len);
				OPENSSL_free(der);
			}
		}

		sk_X509_free(signers);
	}

	unsigned char *p;
	size_t size = BIO_get_mem_data(out, &p);
	out_sod.Append(p, size);

	BIO_free_all(out);
	PKCS7_free(p7);

	return report;
}

CByteArray APL_ICAO::readFile(const std::string &csPath) const {
	auto out = m_reader->getCalReader()->ReadFile(csPath);

	// remove padding
	auto length = der_certificate_length(out);
	out.Chop(out.Size() - length);

	return out;
}

bool APL_ICAO::verifySOD(DataGroupID tag, const CByteArray &data) {
	if (!m_ready) {
		initializeCard();
	}

	return m_SodAttributes->validateHash(tag, data);
}

CByteArray extractPublicKeyFromDG15(CByteArray &dg15_data) {
	CByteArray pubkey_data;
	long len = 0;
	int tag = 0, xclass = 0;
	const unsigned char *der_p = dg15_data.GetBytes();
	int ret = ASN1_get_object(&der_p, &len, &tag, &xclass, dg15_data.Size());
	if (ret & 0x80) {
		MWLOG(LEV_ERROR, MOD_APL, "DG15 ASN.1 failed to decode! Error code: %08x", ERR_get_error());
	} else if (tag != 15 && xclass != V_ASN1_APPLICATION) {
		MWLOG(LEV_ERROR, MOD_APL,
			  "Malformed DG15: expected tag 0x6F (APPLICATION 15)! Found ASN.1 tag with class: %d and tag number: %d",
			  xclass, tag);
	} else {
		pubkey_data.Append(der_p, len);
	}
	return pubkey_data;
}

EIDMW_ActiveAuthenticationReport APL_ICAO::performActiveAuthentication() {
	EIDMW_ActiveAuthenticationReport report;

	MWLOG(LEV_DEBUG, MOD_APL, "Active Authentication for ICAO eMRTD");

	auto cryptFwk = AppLayer.getCryptoFwk();

	// CByteArray security_file;
	CByteArray secopt_file = readFile(PTEID_FILE_SECURITY);
	report.dg14.Append(secopt_file);

	char *dg14_str = byteArrayToHexString(secopt_file.GetBytes(), secopt_file.Size());
	MWLOG(LEV_DEBUG, MOD_APL, "DG14 file: %s", dg14_str);
	free(dg14_str);

	// Store hashes
	report.storedHashDg14 = m_SodAttributes->get(DG14);
	cryptFwk->GetHash(secopt_file, m_SodAttributes->getHashFunction(), &report.hashDg14);

	// verify hash of security file with hash in SOD
	if (!verifySOD(DG14, secopt_file)) {
		MWLOG(LEV_ERROR, MOD_APL, "Security option hash does not match with SOD");

		report.error_code = EIDMW_SOD_ERR_HASH_NO_MATCH_SECURITY;
		report.type = EIDMW_ReportType::Error;
		return report;
	}

	try {
		// read public key DG15
		CByteArray pubkey_file = readFile(PTEID_FILE_PUB_KEY_AA);
		report.dg15 = pubkey_file;

		// Store hashes
		report.storedHashDg15 = m_SodAttributes->get(DG15);
		cryptFwk->GetHash(pubkey_file, m_SodAttributes->getHashFunction(), &report.hashDg15);

		// verify hash of public key with hash in SOD
		if (!verifySOD(DG15, pubkey_file)) {
			MWLOG(LEV_ERROR, MOD_APL, "Public key hash does not match with SOD");

			report.error_code = EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY;
			report.type = EIDMW_ReportType::Error;
			return report;
		}

		// read OID from security file
		auto obj = getSecurityOptionOidByOid(secopt_file, {SECURITY_OPTION_ALGORITHM_OID});
		if (obj == nullptr) {
			MWLOG(LEV_WARN, MOD_APL,
				  "Didn't find active authentication algorithm OID in security options file. This means we should try "
				  "AA with RSA!");
		}
		report.oid = OBJ_nid2sn(OBJ_obj2nid(obj));

		char *dg15_str = byteArrayToHexString(pubkey_file.GetBytes(), pubkey_file.Size());
		MWLOG(LEV_DEBUG, MOD_APL, "DG15 file (card pubkey): %s", dg15_str);
		free(dg15_str);

		// Check the first tag of DG15 file (6F)
		CByteArray pubkey = extractPublicKeyFromDG15(pubkey_file);

		cryptFwk->performActiveAuthentication(obj, pubkey, this);
	} catch (CMWException &e) {
		report.error_code = e.GetError();
		report.type = EIDMW_ReportType::Error;

		if (e.GetError() == EIDMW_ERR_FILE_NOT_FOUND) {
			MWLOG(LEV_WARN, MOD_APL, "DG15 not found! Active Authentication not available for this document.");
			return report;
		} else {
			MWLOG(LEV_WARN, MOD_APL, "Active Authentication failed! Error code %08x thrown at %s:%ld", e.GetError(),
				  e.GetFile().c_str(), e.GetLine());
			return report;
		}
	}

	return report;
}

EIDMW_ChipAuthenticationReport APL_ICAO::performChipAuthentication() {
	EIDMW_ChipAuthenticationReport report;

	// Chip authentication is performed after Active Authentication.
	// No need to re-verify dg14 hashes as it was already performed during the previous AA step
	auto dg14 = readFile(DATAGROUP_PATHS.at(DG14));

	auto pkey = getChipAuthenticationKey(dg14);
	unsigned char *buffer = nullptr;
	int len = i2d_PublicKey(pkey, &buffer);
	report.pubKey = CByteArray(buffer, len);

	auto oid_info = getChipAuthenticationOid(dg14);
	report.oid = oid_info.short_name;

	auto status = getCalReader()->initChipAuthentication(pkey, oid_info.object);
	if (!status) {
		report.type = EIDMW_ReportType::Error;
	}

	return report;
}

const EIDMW_DocumentReport &APL_ICAO::getDocumentReport() {
	if (!m_ready) {
		initializeCard();
	}

	return m_reports;
}

const CByteArray &APL_ICAO::getRawData(APL_RawDataType type) { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

APLPublicKey *APL_ICAO::getRootCAPubKey() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

const char *APL_ICAO::getTokenSerialNumber() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

const char *APL_ICAO::getTokenLabel() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

APL_CardType APL_ICAO::getType() const { return APL_CARDTYPE_ICAO; };

void APL_ICAO::loadMasterList(const char *filePath) {
	auto cryptoFwk = AppLayer.getCryptoFwk();
	cryptoFwk->loadMasterList(filePath);
}

APL_CardType APL_MultiPass::getType() const {
	return APL_CARDTYPE_MULTIPASS;
};

/* Check digit algorithm specified in ICAO doc 9303 part 2 (??) */
int icao_check_digit(const char *mrz_field) {
	int sum = 0;
	int weight = 0;
	const char *str = mrz_field;

	for (size_t i = 0; i < strlen(str); i++) {
		if (i % 3 == 0) {
			weight = 7;
		} else if (i % 3 == 1) {
			weight = 3;
		} else {
			weight = 1;
		}
		if (isdigit(str[i])) {
			sum += weight * (str[i] - '0');
		} else if (isalpha(str[i])) {
			sum += weight * (tolower(str[i]) - 'a' + 10);
		}
	}

	return sum % 10;
}
typedef struct {
	unsigned char * mrz_bytes;
	size_t mrz_length;
} mrz_info_t;

mrz_info_t *multipass_mrz_info_from_sod_bytes(mrz_info_t *in) {
	mrz_info_t *output = (mrz_info_t *)OPENSSL_zalloc(sizeof(mrz_info_t));

	size_t hex_len = in->mrz_length * 2 + 1;
	char *hex_string = (char *)OPENSSL_zalloc(hex_len);
	OPENSSL_buf2hexstr_ex(hex_string, hex_len, NULL, in->mrz_bytes, in->mrz_length, '\0');

	// Split the string in 3 fields to mimic the standard MRZ fields used in BAC
	std::string field1(hex_string, 12);
	std::string field2(hex_string + 12, 6);
	std::string field3(hex_string + 18, 6);

	// Add check digits
	field1 += ('0' + icao_check_digit(field1.c_str()));
	field2 += ('0' + icao_check_digit(field2.c_str()));
	field3 += ('0' + icao_check_digit(field3.c_str()));

	std::string mrz_info_str = field1 + field2 + field3;

	output->mrz_bytes = (unsigned char *)OPENSSL_zalloc(mrz_info_str.size());
	memcpy(output->mrz_bytes, mrz_info_str.c_str(), mrz_info_str.size());
	output->mrz_length = mrz_info_str.size();

	OPENSSL_free(hex_string);
	// return encoded bytes
	return output;
}

CByteArray APL_MultiPass::readTokenData() {
	printf("Printing from multi pass class APL\n");

	printf("Step 1. Select Application...\n");
	selectApplication({MULTIPASS_APPLET, sizeof(MULTIPASS_APPLET)});
	printf("Done\n");


	printf("Step 2. Read File...\n");
	CByteArray sod_data;
	auto ret = readFile("011D", sod_data, 0UL);
	printf("Done\n");

	// remove padding (? not sure if still needed)
	printf("Step 3. Getting certificate length...\n");
	auto length = der_certificate_length(sod_data);
	sod_data.Chop(sod_data.Size() - length);
	printf("Done\n");

	// get last 12 bytes
	printf("Step 4. Getting sod last 12 bytes...\n");
	CByteArray sod_last12 = sod_data.GetBytes(sod_data.Size() - 12);
	printf("Done\n");

	printf("Step 5. Creating MRZ from the 12 bytes...\n");
	mrz_info_t sod_info = {sod_last12.GetBytes(), 12};
	mrz_info_t*mrz_bytes = multipass_mrz_info_from_sod_bytes(&sod_info);
	printf("Done\n");

	printf("Step 6. Opening BAC Channel...\n");
	getCalReader()->openBACChannel({mrz_bytes->mrz_bytes, mrz_bytes->mrz_length});
	printf("Done\n");
	
	return {};
}

APL_MultiPass::APL_MultiPass(APL_ReaderContext *reader) : APL_SmartCard(reader), m_reader(reader) {}
APL_MultiPass::~APL_MultiPass() {}

const CByteArray &APL_MultiPass::getRawData(APL_RawDataType type) { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

APLPublicKey *APL_MultiPass::getRootCAPubKey() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

const char *APL_MultiPass::getTokenSerialNumber() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

const char *APL_MultiPass::getTokenLabel() { throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED); }

} // namespace eIDMW
