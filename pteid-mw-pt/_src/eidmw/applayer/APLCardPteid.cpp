/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2011-2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2023 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
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
#include "APLReader.h"
#include "PaceAuthentication.h"
#include "APLCardPteid.h"
#include "CardPteid.h"
#include "IcaoDg14.h"
#include "SODParser.h"
#include "TLVBuffer.h"
#include "APLCertif.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "eidErrors.h"
#include "Util.h"
#include "Log.h"
#include "MWException.h"
#include "CardLayer.h"
#include "MiscUtil.h"
#include "MutualAuthentication.h"
#include "StringOps.h"
#include "APLConfig.h"
#include "APLCCXmlDoc.h"
#include "RemoteAddress.h"
#include "RemoteAddressRequest.h"

#include <memory>
#include <ctime>
#include <future>

namespace eIDMW {

/*****************************************************************************************
---------------------------------------- APL_EIDCard -----------------------------------------
*****************************************************************************************/

APL_EIDCard::APL_EIDCard(APL_ReaderContext *reader, APL_CardType cardType) : APL_SmartCard(reader) {
	m_cardType = cardType;
	m_CCcustomDoc = NULL;
	m_docid = NULL;
	m_personal = NULL;
	m_address = NULL;
	m_sod = NULL;
	m_docinfo = NULL;

	m_FileTrace = NULL;
	m_FileID = NULL;
	m_FileAddress = NULL;
	m_FileSod = NULL;
	m_FilePersoData = NULL;
	m_FileTokenInfo = NULL;

	m_cardinfosign = NULL;

	m_fileCertAuthentication = NULL;
	m_fileCertSignature = NULL;
	m_fileCertRootSign = NULL;
	m_fileCertRoot = NULL;
	m_fileCertRootAuth = NULL;

	m_sodCheck = false;
	m_tokenLabel = NULL;
	m_tokenSerial = NULL;
	m_appletVersion = NULL;
}

APL_EIDCard::~APL_EIDCard() {
	if (m_CCcustomDoc) {
		delete m_CCcustomDoc;
		m_CCcustomDoc = NULL;
	}
	if (m_docid) {
		delete m_docid;
		m_docid = NULL;
	}
	if (m_personal) {
		delete m_personal;
		m_personal = NULL;
	}
	if (m_address) {
		delete m_address;
		m_address = NULL;
	}
	if (m_sod) {
		delete m_sod;
		m_sod = NULL;
	}

	if (m_docinfo) {
		delete m_docinfo;
		m_docinfo = NULL;
	}

	if (m_FileTrace) {
		delete m_FileTrace;
		m_FileTrace = NULL;
	}

	if (m_FileID) {
		delete m_FileID;
		m_FileID = NULL;
	}

	if (m_FileAddress) {
		delete m_FileAddress;
		m_FileAddress = NULL;
	}

	if (m_FileSod) {
		delete m_FileSod;
		m_FileSod = NULL;
	}

	if (m_FilePersoData) {
		delete m_FilePersoData;
		m_FilePersoData = NULL;
	}

	if (m_FileTokenInfo) {
		delete m_FileTokenInfo;
		m_FileTokenInfo = NULL;
	}

	if (m_cardinfosign) {
		delete m_cardinfosign;
		m_cardinfosign = NULL;
	}

	if (m_fileCertAuthentication) {
		delete m_fileCertAuthentication;
		m_fileCertAuthentication = NULL;
	}

	if (m_fileCertSignature) {
		delete m_fileCertSignature;
		m_fileCertSignature = NULL;
	}

	if (m_fileCertRootAuth) {
		delete m_fileCertRootAuth;
		m_fileCertRootAuth = NULL;
	}

	if (m_fileCertRootSign) {
		delete m_fileCertRootSign;
		m_fileCertRootSign = NULL;
	}

	if (m_fileCertRoot) {
		delete m_fileCertRoot;
		m_fileCertRoot = NULL;
	}

	if (m_tokenLabel) {
		delete m_tokenLabel;
		m_tokenLabel = NULL;
	}

	if (m_tokenSerial) {
		delete m_tokenSerial;
		m_tokenSerial = NULL;
	}
}

APL_EidFile_Trace *APL_EIDCard::getFileTrace() {
	CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
	CByteArray appID = {PTEID_1_APPLET_AID, sizeof(PTEID_1_APPLET_AID)};
	if (m_cardType == APL_CARDTYPE_PTEID_IAS5) {
		appID = {PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)};
	}
	m_FileTrace = new APL_EidFile_Trace(this, appID);

	return m_FileTrace;
}

APL_EidFile_ID *APL_EIDCard::getFileID() {
	if (!m_FileID) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_FileID) {
			if (m_cardType == APL_CARDTYPE_PTEID_IAS5)
				m_FileID = new APL_EidFile_ID_V2(this);
			else
				m_FileID = new APL_EidFile_ID(this);
		}
	}

	return m_FileID;
}

APL_EidFile_Address *APL_EIDCard::getFileAddress() {
	if (!m_FileAddress) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_FileAddress)
			m_FileAddress = new APL_EidFile_Address(this);
	}

	return m_FileAddress;
}

/* Discard the current address and SOD file objects after a successful
   Address Change process
*/
void APL_EIDCard::invalidateAddressSOD() {
	if (m_FileSod) {
		delete m_FileSod;
		m_FileSod = NULL;
	}
	if (m_FileAddress) {
		delete m_FileAddress;
		m_FileAddress = NULL;
	}
	if (m_address) {
		m_address->invalidateAddress();
	}
}

APL_EidFile_Sod *APL_EIDCard::getFileSod() {
	if (!m_FileSod) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_FileSod) {

			if (m_cardType == APL_CARDTYPE_PTEID_IAS5) {
				m_FileSod = new APL_EidFile_Sod(this, PTEID_FILE_SOD_V2);
			} else {
				CByteArray ba_validityEndDate; // Validity date field from card ID file
				// Read the validity end date directly from cardlayer to avoid recursive calls from
				// APL_EidFile_ID::getValidityEndDate
				unsigned long bytes_read =
					this->readFile(PTEID_FILE_ID, ba_validityEndDate, PTEIDNG_FIELD_ID_POS_ValidityEndDate,
								   PTEIDNG_FIELD_ID_LEN_ValidityEndDate);

				ba_validityEndDate.TrimRight('\0');
				m_FileSod = new APL_EidFile_Sod(this);
				MWLOG(LEV_DEBUG, MOD_APL, "ValidityEndDate freshly read: %s", ba_validityEndDate.GetBytes());
			}
		}
	}
	return m_FileSod;
}

APL_EidFile_PersoData *APL_EIDCard::getFilePersoData() {
	if (m_cardType == APL_CARDTYPE_PTEID_IAS5)
		throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);

	if (!m_FilePersoData) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_FilePersoData) {
			m_FilePersoData = new APL_EidFile_PersoData(this);
		}
	}
	return m_FilePersoData;
}

APL_EidFile_TokenInfo *APL_EIDCard::getFileTokenInfo() {
	if (!m_FileTokenInfo) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_FileTokenInfo) {
			m_FileTokenInfo = new APL_EidFile_TokenInfo(this);
		}
	}
	return m_FileTokenInfo;
}

APL_CardType APL_EIDCard::getType() const { return m_cardType; }

unsigned long APL_EIDCard::readFile(const char *csPath, CByteArray &oData, unsigned long ulOffset,
									unsigned long ulMaxLength) {

	return APL_SmartCard::readFile(csPath, oData, ulOffset, ulMaxLength);
}

APL_CCXML_Doc &APL_EIDCard::getXmlCCDoc(APL_XmlUserRequestedInfo &userRequestedInfo) {
	if (m_CCcustomDoc)
		delete m_CCcustomDoc;

	CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
	m_CCcustomDoc = new APL_CCXML_Doc(this, userRequestedInfo);

	return *m_CCcustomDoc;
}

APL_DocEId &APL_EIDCard::getID() {
	if (!m_docid) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_docid) {
			m_docid = new APL_DocEId(this);
		}
	}

	return *m_docid;
}

APL_PersonalNotesEId &APL_EIDCard::getPersonalNotes() {
	if (!m_personal) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_personal) {
			if (m_cardType == APL_CARDTYPE_PTEID_IAS5) {
				throw CMWEXCEPTION(EIDMW_ERR_NOT_SUPPORTED);
			}

			m_personal = new APL_PersonalNotesEId(this);
		}
	}

	return *m_personal;
}

APL_AddrEId &APL_EIDCard::getAddr() {
	if (!m_address) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_address) {
			m_address = new APL_AddrEId(this);
		}
	}

	return *m_address;
}

APL_SodEid &APL_EIDCard::getSod() {
	if (!m_sod) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_sod) {
			m_sod = new APL_SodEid(this);
		}
	}

	return *m_sod;
}

APL_DocVersionInfo &APL_EIDCard::getDocInfo() {
	if (!m_docinfo) {
		CAutoMutex autoMutex(&m_Mutex); // We lock for only one instanciation
		if (!m_docinfo) {
			m_docinfo = new APL_DocVersionInfo(this);
		}
	}

	return *m_docinfo;
}

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
	unsigned char *mrz_bytes;
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

CByteArray APL_EIDCard::readTokenData() {
	MWLOG_CTX(LEV_INFO, MOD_APL, "Reading token from MultiPass application");

	try {
		// Load all certificates and create x509 store
		auto store = std::async(std::launch::async, []() {
			X509_STORE *store = X509_STORE_new();
			APL_Certifs certs = APL_Certifs(true);
			certs.initSODCAs();
			for (unsigned long i = 0; i < certs.countSODCAs(); i++) {
				APL_Certif *sod_ca = certs.getSODCA(i);
				X509 *pX509 = NULL;
				const unsigned char *p = sod_ca->getData().GetBytes();
				pX509 = d2i_X509(&pX509, &p, sod_ca->getData().Size());
				X509_STORE_add_cert(store, pX509);
				MWLOG(LEV_DEBUG, MOD_APL, "%d. Adding certificate Subject CN: %s", i, sod_ca->getOwnerName());
			}

			return store;
		});

		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Selecting multi-pass application");
		selectApplication({MULTIPASS_APPLET, sizeof(MULTIPASS_APPLET)});

		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Reading SOD file");
		CByteArray sod_data;
		auto ret = readFile("011D", sod_data, 0UL);
		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Multi-pass SOD read size: %ld", sod_data.Size());

		// remove padding (? not sure if still needed)
		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Getting certificate length of multi-pass SOD");
		auto length = der_certificate_length(sod_data);
		sod_data.Chop(sod_data.Size() - length);
		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Multi-pass SOD size: %ld", sod_data.Size());

		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Generating MRZ from the last 12 bytes of multi-pass SOD");
		CByteArray sod_last12 = sod_data.GetBytes(sod_data.Size() - 12);
		mrz_info_t sod_info = {sod_last12.GetBytes(), 12};
		mrz_info_t *mrz_bytes = multipass_mrz_info_from_sod_bytes(&sod_info);

		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Opening BAC channel");
		getCalReader()->openBACChannel({mrz_bytes->mrz_bytes, mrz_bytes->mrz_length});

		// Get SOD contents and its validation state
		auto sod_contents = AppLayer.getCryptoFwk()->getSodContents(sod_data, store.get());
		if (sod_contents.first != 0) {
			MWLOG_CTX(LEV_ERROR, MOD_APL, "Failed to verify SOD validity with error: %x", sod_contents.first);
		}

		const std::vector<int> expected_tags = {13, 14, 15};
		SODParser sodParser;
		sodParser.ParseSodEncapsulatedContent(sod_contents.second, expected_tags);

		auto dg14 = getCalReader()->ReadFile("010E");
		if (!sodParser.getAttributes().validateHash(14, dg14)) {
			LOG_AND_THROW(LEV_ERROR, MOD_APL, EIDMW_SOD_ERR_ACTIVE_AUTHENTICATION, "DG 14 hash verification failed");
		}

		auto pkey = getChipAuthenticationKey(dg14);
		unsigned char *buffer = nullptr;
		int len = i2d_PublicKey(pkey, &buffer);

		auto oid_info = getChipAuthenticationOid(dg14);
		auto status = getCalReader()->initChipAuthentication(pkey, oid_info.object);
		if (!status) {
			LOG_AND_THROW(LEV_ERROR, MOD_APL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Failed to upgrade to Chip Authentication");
		}

		MWLOG_CTX(LEV_DEBUG, MOD_APL, "Reading multi-pass token");
		CByteArray token = getCalReader()->readMultiPassToken();

		// verify token hash
		if (!sodParser.getAttributes().validateHash(13, token)) {
			LOG_AND_THROW(LEV_ERROR, MOD_APL, EIDMW_SOD_ERR_HASH_NO_MATCH_FILE,
						  "Token (DG 13) hash verification failed");
		}

		return token;
	} catch (CMWException &e) {
		MWLOG_CTX(LEV_ERROR, MOD_APL, "Token Read Failed: %ld", e.GetError());
		throw;
	}
}

const CByteArray &APL_EIDCard::getRawData(APL_RawDataType type) {
	switch (type) {
	case APL_RAWDATA_ID:
		return getRawData_Id();
	case APL_RAWDATA_TRACE:
		return getRawData_Trace();
	case APL_RAWDATA_ADDR:
		return getRawData_Addr();
	case APL_RAWDATA_SOD:
		return getRawData_Sod();
	case APL_RAWDATA_CARD_INFO:
		return getRawData_CardInfo();
	case APL_RAWDATA_TOKEN_INFO:
		return getRawData_TokenInfo();
	case APL_RAWDATA_PERSO_DATA:
		return getRawData_PersoData();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
}

const CByteArray &APL_EIDCard::getRawData_Trace() { return getFileTrace()->getData(); }

const CByteArray &APL_EIDCard::getRawData_Id() { return getFileID()->getData(); }

const CByteArray &APL_EIDCard::getRawData_Addr() { return getFileAddress()->getData(); }

const CByteArray &APL_EIDCard::getRawData_Sod() { return getFileSod()->getData(); }

const CByteArray &APL_EIDCard::getRawData_CardInfo() { return getFileInfo()->getData(); }

const CByteArray &APL_EIDCard::getRawData_TokenInfo() { return getFileTokenInfo()->getData(); }

const CByteArray &APL_EIDCard::getRawData_PersoData() { return getFilePersoData()->getData(); }

const char *APL_EIDCard::getTokenSerialNumber() {
	if (!m_tokenSerial) {

		// BEGIN_CAL_OPERATION(m_reader)
		m_reader->CalLock();
		try {
			m_tokenSerial = new std::string(m_reader->getCalReader()->GetSerialNr());
		} catch (...) {
			m_reader->CalUnlock();
			delete m_tokenSerial;
			throw;
		}
		m_reader->CalUnlock();
		// END_CAL_OPERATION(m_reader)
	}
	return m_tokenSerial->c_str();
}

const char *APL_EIDCard::getTokenLabel() {
	if (!m_tokenLabel) {

		// BEGIN_CAL_OPERATION(m_reader)
		m_reader->CalLock();
		try {
			m_tokenLabel = new std::string(m_reader->getCalReader()->GetCardLabel());
		} catch (...) {
			m_reader->CalUnlock();
			delete m_tokenLabel;
			throw;
		}
		m_reader->CalUnlock();
		// END_CAL_OPERATION(m_reader)
	}
	return m_tokenLabel->c_str();
}

const char *APL_EIDCard::getAppletVersion() {

	if (!m_appletVersion) {
		m_reader->CalLock();
		try {
			m_appletVersion = new std::string(m_reader->getCalReader()->GetAppletVersion());
		} catch (...) {
			m_reader->CalUnlock();
			delete m_appletVersion;
			throw;
		}
		m_reader->CalUnlock();
	}
	return m_appletVersion->c_str();
}

APLPublicKey *APL_EIDCard::getRootCAPubKey() {

	if (!m_RootCAPubKey) {
		CByteArray out;
		CByteArray modulus;
		CByteArray exponent;
		CByteArray ec_pubkey;

		BEGIN_CAL_OPERATION(m_reader)
		out = m_reader->getCalReader()->RootCAPubKey();
		END_CAL_OPERATION(m_reader)

		const unsigned long EC_PUBKEY_OFFSET = 10;

		switch(m_reader->getCardType()) {

		case APL_CARDTYPE_PTEID_IAS101:
			modulus = out.GetBytes(PTEIDNG_FIELD_ROOTCA_PK_POS_MODULUS_IAS101, PTEIDNG_FIELD_ROOTCA_PK_LEN_MODULUS);
			exponent = out.GetBytes(PTEIDNG_FIELD_ROOTCA_PK_POS_EXPONENT_IAS_101, PTEIDNG_FIELD_ROOTCA_PK_LEN_EXPONENT);
			break;
		case APL_CARDTYPE_PTEID_IAS07:
			modulus = out.GetBytes(PTEIDNG_FIELD_ROOTCA_PK_POS_MODULUS_IAS07, PTEIDNG_FIELD_ROOTCA_PK_LEN_MODULUS);
			exponent = out.GetBytes(PTEIDNG_FIELD_ROOTCA_PK_POS_EXPONENT_IAS_07, PTEIDNG_FIELD_ROOTCA_PK_LEN_EXPONENT);
			break;
		case APL_CARDTYPE_PTEID_IAS5:
			ec_pubkey = out.GetBytes(EC_PUBKEY_OFFSET);
			break;
		case APL_CARDTYPE_UNKNOWN:
			throw CMWEXCEPTION(EIDMW_ERR_CARDTYPE_UNKNOWN);
			break;
		}

		if (ec_pubkey.Size() > 0) {
			m_RootCAPubKey = new APLPublicKey(ec_pubkey);
		}
		else {
			m_RootCAPubKey = new APLPublicKey(modulus,exponent);
		}
	}

	return m_RootCAPubKey;
}

bool APL_EIDCard::isActive() { return getFileTrace()->isActive(); }

bool APL_EIDCard::Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN) {
	bool out = false;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->Activate(pinCode, BCDDate, blockActivationPIN);
	END_CAL_OPERATION(m_reader)

	return out;
}

void APL_EIDCard::doSODCheck(bool check) {
	m_sodCheck = check;

	if (m_FileAddress)
		m_FileAddress->doSODCheck(check);
	if (m_FileID)
		m_FileID->doSODCheck(check);

	if (!m_FileSod) {
		m_FileSod = getFileSod();
		m_FileSod->doSODCheck(check);
	}
}

/*****************************************************************************************
---------------------------------------- APL_CCXML_Doc -------------------------------------------
*****************************************************************************************/
APL_CCXML_Doc::APL_CCXML_Doc(APL_EIDCard *card, APL_XmlUserRequestedInfo &xmlUserRequestedInfo) {
	m_card = card;
	m_xmlUserRequestedInfo = &xmlUserRequestedInfo;
}

APL_CCXML_Doc::~APL_CCXML_Doc() {}

CByteArray APL_CCXML_Doc::getXML(bool bNoHeader) {
	CByteArray xml;
	std::string *ts, *sn, *sa, *tk;
	std::string rootATTRS;

	ts = m_xmlUserRequestedInfo->getTimeStamp();
	sn = m_xmlUserRequestedInfo->getServerName();
	sa = m_xmlUserRequestedInfo->getServerAddress();
	tk = m_xmlUserRequestedInfo->getTokenID();

	if (!bNoHeader)
		xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	if (ts || sn || sa || tk) {
		xml += XML_ROOT_S;
		if (ts) {
			XML_ATTRIBUTE(xml, XML_ROOT_ELEMENT_ATTR_TIMESTAMP, ts->c_str());
		}
		if (sn) {
			XML_ATTRIBUTE(xml, XML_ROOT_ELEMENT_ATTR_SERVERNAME, sn->c_str());
		}
		if (sa) {
			XML_ATTRIBUTE(xml, XML_ROOT_ELEMENT_ATTR_SERVERADDRESS, sa->c_str());
		}
		if (tk) {
			XML_ATTRIBUTE(xml, XML_ROOT_ELEMENT_ATTR_TOKENID, tk->c_str());
		}
		xml += XML_ROOT_E;
	} else
		xml += XML_OPEN_TAG_NEWLINE(XML_ROOT_ELEMENT);

	xml += m_card->getID().getXML(true, *m_xmlUserRequestedInfo);
	xml += m_card->getAddr().getXML(true, *m_xmlUserRequestedInfo);
	xml += m_card->getPersonalNotes().getXML(true, *m_xmlUserRequestedInfo);
	xml += XML_CLOSE_TAG(XML_ROOT_ELEMENT);

	return xml;
}

/*****************************************************************************************
-------------------------------- APL_XmlUserRequestedInfo ---------------------------------------
*****************************************************************************************/
APL_XmlUserRequestedInfo::APL_XmlUserRequestedInfo() {
	xmlSet = new std::set<enum XMLUserData>;
	_serverName = NULL;
	_serverAddress = NULL;
	_timeStamp = NULL;
	_tokenID = NULL;
}

APL_XmlUserRequestedInfo::APL_XmlUserRequestedInfo(const char *timeStamp, const char *serverName,
												   const char *serverAddress) {
	xmlSet = new std::set<enum XMLUserData>;
	_timeStamp = (timeStamp) ? new std::string(timeStamp) : NULL;
	_serverName = (serverName) ? new std::string(serverName) : NULL;
	_serverAddress = (serverAddress) ? new std::string(serverAddress) : NULL;
	_tokenID = NULL;
}

APL_XmlUserRequestedInfo::APL_XmlUserRequestedInfo(const char *timeStamp, const char *serverName,
												   const char *serverAddress, const char *tokenID) {
	xmlSet = new std::set<enum XMLUserData>;

	_timeStamp = (timeStamp) ? new std::string(timeStamp) : NULL;
	_serverName = (serverName) ? new std::string(serverName) : NULL;
	_serverAddress = (serverAddress) ? new std::string(serverAddress) : NULL;
	_tokenID = (tokenID) ? new std::string(tokenID) : NULL;
}

APL_XmlUserRequestedInfo::~APL_XmlUserRequestedInfo() {
	if (xmlSet)
		delete xmlSet;
	if (_timeStamp)
		delete _timeStamp;
	if (_serverAddress)
		delete _serverAddress;
	if (_serverName)
		delete _serverName;
	if (_tokenID)
		delete _tokenID;
}

void APL_XmlUserRequestedInfo::add(XMLUserData xmlUData) { xmlSet->insert(xmlUData); }

bool APL_XmlUserRequestedInfo::contains(XMLUserData xmlUData) { return (xmlSet->find(xmlUData) != xmlSet->end()); }

void APL_XmlUserRequestedInfo::remove(XMLUserData xmlUData) { xmlSet->erase(xmlUData); }

bool APL_XmlUserRequestedInfo::checkAndRemove(XMLUserData xmlUData) {
	bool contains;

	contains = (xmlSet->find(xmlUData) != xmlSet->end());
	if (!contains)
		xmlSet->erase(xmlUData);

	return contains;
}

bool APL_XmlUserRequestedInfo::isEmpty() { return xmlSet->empty(); }

std::string *APL_XmlUserRequestedInfo::getTimeStamp() { return _timeStamp; }

std::string *APL_XmlUserRequestedInfo::getServerName() { return _serverName; }

std::string *APL_XmlUserRequestedInfo::getServerAddress() { return _serverAddress; }

std::string *APL_XmlUserRequestedInfo::getTokenID() { return _tokenID; }

/*****************************************************************************************
---------------------------------------- APL_DocEId ---------------------------------------------
*****************************************************************************************/
APL_DocEId::APL_DocEId(APL_EIDCard *card) {
	m_card = card;

	// m_FirstName.clear();
	m_cryptoFwk = AppLayer.getCryptoFwk();

	_xmlUInfo = NULL;
}

APL_DocEId::~APL_DocEId() {}

CByteArray APL_DocEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo) {

	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_DocEId::getXML(bool bNoHeader) {
	CByteArray xml;
	CByteArray basicInfo;
	CByteArray civilInfo;
	CByteArray idNum;
	CByteArray cardValues;
	CByteArray b64photo;
	CByteArray *photo;
	bool addBasicInfo = false;
	bool addIdNum = false;
	bool addCardValues = false;
	bool addCivilInfo = false;
	std::string temp;

	// photo
	if (_xmlUInfo->contains(XML_PHOTO)) {
		photo = getPhotoObj()->getPhotoPNG();
		m_cryptoFwk->b64Encode(*photo, b64photo);
		BUILD_XML_ELEMENT(xml, XML_PHOTO_ELEMENT, b64photo);
	}

	// basicInformation
	if (_xmlUInfo->contains(XML_NAME)) {
		std::string s;
		s += getGivenName();
		s += " ";
		s += getSurname();
		BUILD_XML_ELEMENT(basicInfo, XML_NAME_ELEMENT, s.c_str());
		addBasicInfo = true;
	}
	if (_xmlUInfo->contains(XML_GIVEN_NAME)) {
		BUILD_XML_ELEMENT(basicInfo, XML_GIVEN_NAME_ELEMENT, getGivenName());
		addBasicInfo = true;
	}
	if (_xmlUInfo->contains(XML_SURNAME)) {
		BUILD_XML_ELEMENT(basicInfo, XML_SURNAME_ELEMENT, getSurname());
		addBasicInfo = true;
	}
	if (_xmlUInfo->contains(XML_NIC)) {
		BUILD_XML_ELEMENT(basicInfo, XML_NIC_ELEMENT, getCivilianIdNumber());
		addBasicInfo = true;
	}
	if (_xmlUInfo->contains(XML_EXPIRY_DATE)) {
		BUILD_XML_ELEMENT(basicInfo, XML_EXPIRY_DATE_ELEMENT, getValidityEndDate());
		addBasicInfo = true;
	}
	if (addBasicInfo) {
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_BASIC_INFO_ELEMENT, basicInfo);
	}

	// CivilInformation
	if (_xmlUInfo->contains(XML_GENDER)) {
		BUILD_XML_ELEMENT(civilInfo, XML_GENDER_ELEMENT, getGender());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_HEIGHT)) {
		BUILD_XML_ELEMENT(civilInfo, XML_HEIGHT_ELEMENT, getHeight());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_NATIONALITY)) {
		BUILD_XML_ELEMENT(civilInfo, XML_NATIONALITY_ELEMENT, getNationality());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_DATE_OF_BIRTH)) {
		BUILD_XML_ELEMENT(civilInfo, XML_DATE_OF_BIRTH_ELEMENT, getDateOfBirth());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_GIVEN_NAME_FATHER)) {
		BUILD_XML_ELEMENT(civilInfo, XML_GIVEN_NAME_FATHER_ELEMENT, getGivenNameFather());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_SURNAME_FATHER)) {
		BUILD_XML_ELEMENT(civilInfo, XML_SURNAME_FATHER_ELEMENT, getSurnameFather());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_GIVEN_NAME_MOTHER)) {
		BUILD_XML_ELEMENT(civilInfo, XML_GIVEN_NAME_MOTHER_ELEMENT, getGivenNameMother());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_SURNAME_MOTHER)) {
		BUILD_XML_ELEMENT(civilInfo, XML_SURNAME_MOTHER_ELEMENT, getSurnameMother());
		addCivilInfo = true;
	}
	if (_xmlUInfo->contains(XML_ACCIDENTAL_INDICATIONS)) {
		BUILD_XML_ELEMENT(civilInfo, XML_ACCIDENTAL_INDICATIONS_ELEMENT, getAccidentalIndications());
		addCivilInfo = true;
	}
	if (addCivilInfo) {
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_CIVIL_INFO_ELEMENT, civilInfo);
	}

	// IdentificationNumbers
	if (_xmlUInfo->contains(XML_DOCUMENT_NO)) {
		BUILD_XML_ELEMENT(idNum, XML_DOCUMENT_NO_ELEMENT, getDocumentNumber());
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_TAX_NO)) {
		BUILD_XML_ELEMENT(idNum, XML_TAX_NO_ELEMENT, getTaxNo());
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_SOCIAL_SECURITY_NO)) {
		BUILD_XML_ELEMENT(idNum, XML_SOCIAL_SECURITY_NO_ELEMENT, getSocialSecurityNumber());
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_HEALTH_NO)) {
		BUILD_XML_ELEMENT(idNum, XML_HEALTH_NO_ELEMENT, getHealthNumber());
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_MRZ1)) {
		temp = getMRZ1();
		replace(temp, XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ1_ELEMENT, temp);
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_MRZ2)) {
		temp = getMRZ2();
		replace(temp, XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ2_ELEMENT, temp);
		addIdNum = true;
	}
	if (_xmlUInfo->contains(XML_MRZ3)) {
		temp = getMRZ3();
		replace(temp, XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ3_ELEMENT, temp);
		addIdNum = true;
	}
	if (addIdNum) {
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_IDENTIFICATION_NUMBERS_ELEMENT, idNum);
	}

	// CardValues
	if (_xmlUInfo->contains(XML_CARD_VERSION)) {
		BUILD_XML_ELEMENT(cardValues, XML_CARD_VERSION_ELEMENT, getDocumentVersion());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_CARD_NUMBER_PAN)) {
		BUILD_XML_ELEMENT(cardValues, XML_CARD_NUMBER_PAN_ELEMENT, getDocumentPAN());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_ISSUING_DATE)) {
		BUILD_XML_ELEMENT(cardValues, XML_ISSUING_DATE_ELEMENT, getValidityBeginDate());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_ISSUING_ENTITY)) {
		BUILD_XML_ELEMENT(cardValues, XML_ISSUING_ENTITY_ELEMENT, getIssuingEntity());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_DOCUMENT_TYPE)) {
		BUILD_XML_ELEMENT(cardValues, XML_DOCUMENT_TYPE_ELEMENT, getDocumentType());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_LOCAL_OF_REQUEST)) {
		BUILD_XML_ELEMENT(cardValues, XML_LOCAL_OF_REQUEST_ELEMENT, getLocalofRequest());
		addCardValues = true;
	}
	if (_xmlUInfo->contains(XML_VERSION)) {
		BUILD_XML_ELEMENT(cardValues, XML_VERSION_ELEMENT, "0");
		addCardValues = true;
	}
	if (addCardValues) {
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_CARD_VALUES_ELEMENT, cardValues);
	};

	return xml;
}

const char *APL_DocEId::getValidation() { return m_card->getFileTrace()->getValidation(); }

const char *APL_DocEId::getDocumentVersion() { return m_card->getFileID()->getDocumentVersion(); }

const char *APL_DocEId::getDocumentType() { return m_card->getFileID()->getDocumentType(); }

const char *APL_DocEId::getCountry() { return m_card->getFileID()->getCountry(); }

const char *APL_DocEId::getGivenName() { return m_card->getFileID()->getGivenName(); }

const char *APL_DocEId::getSurname() { return m_card->getFileID()->getSurname(); }

const char *APL_DocEId::getGender() { return m_card->getFileID()->getGender(); }

const char *APL_DocEId::getDateOfBirth() { return m_card->getFileID()->getDateOfBirth(); }

const char *APL_DocEId::getNationality() { return m_card->getFileID()->getNationality(); }

const char *APL_DocEId::getDocumentPAN() { return m_card->getFileID()->getDocumentPAN(); }

const char *APL_DocEId::getValidityBeginDate() { return m_card->getFileID()->getValidityBeginDate(); }

const char *APL_DocEId::getValidityEndDate() { return m_card->getFileID()->getValidityEndDate(); }

const char *APL_DocEId::getHeight() { return m_card->getFileID()->getHeight(); }

const char *APL_DocEId::getDocumentNumber() { return m_card->getFileID()->getDocumentNumber(); }

const char *APL_DocEId::getTaxNo() { return m_card->getFileID()->getTaxNo(); }

const char *APL_DocEId::getSocialSecurityNumber() { return m_card->getFileID()->getSocialSecurityNumber(); }

const char *APL_DocEId::getHealthNumber() { return m_card->getFileID()->getHealthNumber(); }

const char *APL_DocEId::getIssuingEntity() { return m_card->getFileID()->getIssuingEntity(); }

const char *APL_DocEId::getLocalofRequest() { return m_card->getFileID()->getLocalofRequest(); }

const char *APL_DocEId::getGivenNameFather() { return m_card->getFileID()->getGivenNameFather(); }

const char *APL_DocEId::getSurnameFather() { return m_card->getFileID()->getSurnameFather(); }

const char *APL_DocEId::getGivenNameMother() { return m_card->getFileID()->getGivenNameMother(); }

const char *APL_DocEId::getSurnameMother() { return m_card->getFileID()->getSurnameMother(); }

const char *APL_DocEId::getParents() { return m_card->getFileID()->getParents(); }

PhotoPteid *APL_DocEId::getPhotoObj() { return m_card->getFileID()->getPhotoObj(); }

APLPublicKey *APL_DocEId::getCardAuthKeyObj() { return m_card->getFileID()->getCardAuthKeyObj(); }

const char *APL_DocEId::getMRZ1() { return m_card->getFileID()->getMRZ1(); }

const char *APL_DocEId::getMRZ2() { return m_card->getFileID()->getMRZ2(); }

const char *APL_DocEId::getMRZ3() { return m_card->getFileID()->getMRZ3(); }

const char *APL_DocEId::getAccidentalIndications() { return m_card->getFileID()->getAccidentalIndications(); }

const char *APL_DocEId::getCivilianIdNumber() { return m_card->getFileID()->getCivilianIdNumber(); }

/*****************************************************************************************
----------------------------------- APL_PersonalNotesEid ----------------------------------------
*****************************************************************************************/
APL_PersonalNotesEId::APL_PersonalNotesEId(APL_EIDCard *card) { m_card = card; }

APL_PersonalNotesEId::~APL_PersonalNotesEId() {}

CByteArray APL_PersonalNotesEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo) {
	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_PersonalNotesEId::getXML(bool bNoHeader) {
	CByteArray xml;
	std::string str;

	if (_xmlUInfo->contains(XML_PERSONAL_NOTES)) {
		str = getPersonalNotes();
		replace(str, XML_ESCAPE_AMP);
		replace(str, XML_ESCAPE_APOS);
		replace(str, XML_ESCAPE_GT);
		replace(str, XML_ESCAPE_LT);
		replace(str, XML_ESCAPE_QUOTE);

		BUILD_XML_ELEMENT(xml, XML_PERSONAL_NOTES_ELEMENT, str.c_str());
	}

	return xml;
}

const char *APL_PersonalNotesEId::getPersonalNotes(bool forceMap) {
	return m_card->getFilePersoData()->getPersoData(forceMap);
}

/*****************************************************************************************
---------------------------------------- APL_AddrEId ---------------------------------------------
*****************************************************************************************/

APL_AddrEId::APL_AddrEId(APL_EIDCard *card) {
	m_card = card;
	remoteAddressLoaded = false;
}

APL_AddrEId::~APL_AddrEId() {}

CByteArray APL_AddrEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo) {
	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_AddrEId::getXML(bool bNoHeader) {
	CByteArray xml;
	CByteArray address;
	bool addAddress = false;

	/*	if (isNationalAddress()){ //specification for xml does not include foreign addresses, this will stay here if
	 * specification changes */
	if (_xmlUInfo->contains(XML_DISTRICT)) {
		BUILD_XML_ELEMENT(address, XML_DISTRICT_ELEMENT, getDistrict());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_MUNICIPALITY)) {
		BUILD_XML_ELEMENT(address, XML_MUNICIPALITY_ELEMENT, getMunicipality());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_CIVIL_PARISH)) {
		BUILD_XML_ELEMENT(address, XML_CIVIL_PARISH_ELEMENT, getCivilParish());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_ABBR_STREET_TYPE)) {
		BUILD_XML_ELEMENT(address, XML_ABBR_STREET_TYPE_ELEMENT, getAbbrStreetType());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_STREET_TYPE)) {
		BUILD_XML_ELEMENT(address, XML_STREET_TYPE_ELEMENT, getStreetType());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_STREET_NAME)) {
		BUILD_XML_ELEMENT(address, XML_STREET_NAME_ELEMENT, getStreetName());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_ABBR_BUILDING_TYPE)) {
		BUILD_XML_ELEMENT(address, XML_ABBR_BUILDING_TYPE_ELEMENT, getAbbrBuildingType());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_BUILDING_TYPE)) {
		BUILD_XML_ELEMENT(address, XML_BUILDING_TYPE_ELEMENT, getBuildingType());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_DOOR_NO)) {
		BUILD_XML_ELEMENT(address, XML_DOOR_NO_ELEMENT, getDoorNo());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_FLOOR)) {
		BUILD_XML_ELEMENT(address, XML_FLOOR_ELEMENT, getFloor());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_SIDE)) {
		BUILD_XML_ELEMENT(address, XML_SIDE_ELEMENT, getSide());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_PLACE)) {
		BUILD_XML_ELEMENT(address, XML_PLACE_ELEMENT, getPlace());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_LOCALITY)) {
		BUILD_XML_ELEMENT(address, XML_LOCALITY_ELEMENT, getLocality());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_ZIP4)) {
		BUILD_XML_ELEMENT(address, XML_ZIP4_ELEMENT, getZip4());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_ZIP3)) {
		BUILD_XML_ELEMENT(address, XML_ZIP3_ELEMENT, getZip3());
		addAddress = true;
	}
	if (_xmlUInfo->contains(XML_POSTAL_LOCALITY)) {
		BUILD_XML_ELEMENT(address, XML_POSTAL_LOCALITY_ELEMENT, getPostalLocality());
		addAddress = true;
	}
	if (addAddress) {
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_ADDRESS_ELEMENT, address);
	}

	/* } else { //specification for xml does not include foreign addresses, this will stay here if specification changes
		if (_xmlUInfo->contains(XML_FOREIGN_COUNTRY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_COUNTRY_ELEMENT, getForeignCountry());
			addAddress = true;
		}
		if (_xmlUInfo->contains(XML_FOREIGN_ADDRESS)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_ADDRESS_ELEMENT, getForeignAddress());
			addAddress = true;
		}
		if (_xmlUInfo->contains(XML_FOREIGN_CITY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_CITY_ELEMENT, getForeignCity());
			addAddress = true;
		}
		if (_xmlUInfo->contains(XML_FOREIGN_REGION)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_REGION_ELEMENT, getForeignRegion());
			addAddress = true;
		}
		if (_xmlUInfo->contains(XML_FOREIGN_LOCALITY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_LOCALITY_ELEMENT, getForeignLocality());
			addAddress = true;
		}
		if (_xmlUInfo->contains(XML_FOREIGN_POSTAL_CODE)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_POSTAL_CODE_ELEMENT, getForeignPostalCode());
			addAddress = true;
		}
	}
	 */
	return xml;
}

bool checkResultSW12(CByteArray &result, unsigned int *p_sw12) {
	unsigned long ulRespLen = result.Size();
	const unsigned char *result_ptr = result.GetBytes();

	unsigned int ulSW12 = (unsigned int)(256 * result_ptr[ulRespLen - 2] + result_ptr[ulRespLen - 1]);
	*p_sw12 = ulSW12;

	return ulSW12 == 0x9000;
}

void json_parse_string(std::string &dest_string, cJSON *json_obj, const char *item_name) {

	cJSON *item = cJSON_GetObjectItem(json_obj, item_name);

	// Clean string content
	dest_string = "";

	if (cJSON_IsString(item)) {
		dest_string.append(item->valuestring);
	} else {
		MWLOG(LEV_DEBUG, MOD_APL, "Address field %s is missing", item_name);
	}
}

void APL_AddrEId::mapNationalFields(cJSON *json_obj) {
	m_AddressType = "";
	m_AddressType.append("N");

	json_parse_string(m_CountryCode, json_obj, "countryCode");
	json_parse_string(m_DistrictCode, json_obj, "districtCode");
	json_parse_string(m_DistrictName, json_obj, "district");
	json_parse_string(m_MunicipalityCode, json_obj, "municipalityCode");
	json_parse_string(m_MunicipalityName, json_obj, "municipality");
	json_parse_string(m_CivilParishCode, json_obj, "parishCode");
	json_parse_string(m_CivilParishName, json_obj, "parish");
	json_parse_string(m_AbbrStreetType, json_obj, "abbrStreetType");
	json_parse_string(m_StreetType, json_obj, "streetType");
	json_parse_string(m_StreetName, json_obj, "streetName");
	json_parse_string(m_AbbrBuildingType, json_obj, "abbrBuildingType");
	json_parse_string(m_BuildingType, json_obj, "buildingType");
	json_parse_string(m_DoorNo, json_obj, "doorNo");
	json_parse_string(m_Floor, json_obj, "floor");
	json_parse_string(m_Side, json_obj, "side");
	json_parse_string(m_Locality, json_obj, "locality");
	json_parse_string(m_Zip4, json_obj, "zip4");
	json_parse_string(m_Zip3, json_obj, "zip3");
	json_parse_string(m_PostalLocality, json_obj, "postalLocality");
	json_parse_string(m_Place, json_obj, "place");
	json_parse_string(m_Generated_Address_Code, json_obj, "generatedAddressCode");
}

void APL_AddrEId::mapForeignFields(cJSON *json_obj) {
	m_AddressType = "";
	m_AddressType.append("I");

	json_parse_string(m_CountryCode, json_obj, "countryCode");

	json_parse_string(m_Foreign_Country, json_obj, "foreignCountry");
	json_parse_string(m_Foreign_City, json_obj, "foreignCity");
	json_parse_string(m_Foreign_Region, json_obj, "foreignRegion");
	json_parse_string(m_Foreign_Locality, json_obj, "foreignLocality");
	json_parse_string(m_Foreign_Postal_Code, json_obj, "foreignPostalCode");
	json_parse_string(m_Generated_Address_Code, json_obj, "generatedAddressCode");
	json_parse_string(m_Foreign_Generic_Address, json_obj, "foreignGenericAddress");
}

long APL_AddrEId::validateRemoteAddressData(const char *json_response, RemoteAddressProtocol protocol) {
	long exception_code = 0;
	std::unique_ptr<RA_GetAddressResponse> getaddr_resp(validateReadAddressResponse(json_response));
	if (getaddr_resp->address_obj != NULL) {
		remoteAddressLoaded = true;
		if (!getaddr_resp->is_foreign_address) {
			mapNationalFields(getaddr_resp->address_obj);
		} else {
			mapForeignFields(getaddr_resp->address_obj);
		}
	} else {
		const int INVALID_STATE_ERR = 1015;
		if (getaddr_resp->error_code == INVALID_STATE_ERR) {
			MWLOG(LEV_ERROR, MOD_APL, "Card in invalid state for address reading. Error code %d", INVALID_STATE_ERR);
			exception_code = EIDMW_REMOTEADDR_INVALID_STATE;
		} else {
			MWLOG(LEV_ERROR, MOD_APL,
				  "Unexpected server response for readAddress endpoint in protocol %d, no HTTP error code but "
				  "empty/malformed response",
				  protocol);
			exception_code = EIDMW_REMOTEADDR_SERVER_ERROR;
		}
	}
	return exception_code;
}

std::string buildRemoteAddressURL(RemoteAddressProtocol protocol, int endpoint) {
	const std::string ENDPOINT_DH1 = "/readaddressCC2/ecdh1";
	const std::string ENDPOINT_DH2 = "/readaddressCC2/ecdh2";
	const std::string ENDPOINT_MA1 = "/readaddressCC2/mutualauth1";
	const std::string ENDPOINT_MA2 = "/readaddressCC2/mutualauth2";

	const std::string ENDPOINT_DH = "/readaddress/sendDHParams";
	const std::string ENDPOINT_SIGNCHALLENGE = "/readaddress/signChallenge";
	const std::string ENDPOINT_READADDRESS = "/readaddress/readAddress";

	APL_Config conf_baseurl(CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_BASEURL);
	APL_Config conf_baseurl_cc2(CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_CC2_BASEURL);
	std::string ra_url;

	if (CConfig::isTestModeEnabled()) {
		APL_Config conf_baseurl_t(CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_BASEURL_TEST);
		ra_url += conf_baseurl_t.getString();
	} else {
		ra_url += (protocol == CC2_PROTOCOL ? conf_baseurl_cc2.getString() : conf_baseurl.getString());
	}

	switch (endpoint) {
	case 1:
		ra_url += (protocol == CC1_PROTOCOL ? ENDPOINT_DH : ENDPOINT_DH1);
		break;
	case 2:
		ra_url += (protocol == CC1_PROTOCOL ? ENDPOINT_SIGNCHALLENGE : ENDPOINT_DH2);
		break;
	case 3:
		ra_url += (protocol == CC1_PROTOCOL ? ENDPOINT_READADDRESS : ENDPOINT_MA1);
		break;
	case 4:
		if (protocol == CC2_PROTOCOL) {
			ra_url += ENDPOINT_MA2;
		}
		break;
	}

	return ra_url;
}

/* Force PACE SM to "wake up" if it was active before Remote Address mutual auth
   Using contact interface this has the effect of returning to "cleartext mode"
*/
void APL_AddrEId::breakSecureMessaging() {

	CReader *reader = m_card->getCalReader();
	if (reader != NULL) {
		reader->setNextAPDUClearText();

		const unsigned char select_nonexistent_ef[] = {0x00, 0xA4, 0x02, 0x00, 0x02, 0xAB, 0xCD};
		const unsigned char reselect_eid_app[] = {0x00, 0xA4, 0x04, 0x00, 0x07};
		CByteArray plaintext_reselect(reselect_eid_app, sizeof(reselect_eid_app));
		plaintext_reselect.Append({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
		CByteArray plaintext_dummy_apdu{select_nonexistent_ef, sizeof(select_nonexistent_ef)};

		reader->SendAPDU(plaintext_dummy_apdu);
		reader->SendAPDU(plaintext_reselect);
	}
}

void APL_AddrEId::handleRemoteAddressError(bool sm_started, long exception_code) {
	if (sm_started) {
		breakSecureMessaging();
	}
	throw CMWEXCEPTION(exception_code);
}

void APL_AddrEId::loadRemoteAddress_CC2() {

	std::string url_endpoint_ecdh1 = buildRemoteAddressURL(CC2_PROTOCOL, 1);
	std::string url_endpoint_ecdh2 = buildRemoteAddressURL(CC2_PROTOCOL, 2);
	std::string url_endpoint_mutual_auth1 = buildRemoteAddressURL(CC2_PROTOCOL, 3);
	std::string url_endpoint_mutual_auth2 = buildRemoteAddressURL(CC2_PROTOCOL, 4);
	long exception_code = 0;
	bool sm_started = false;
	const unsigned long ADDRESS_PIN_IDX = 2;

	MutualAuthentication mutual_authentication(m_card);
	MWLOG(LEV_DEBUG, MOD_APL, "Starting to load Remote Address - Reading SOD data");
	m_card->selectApplication({PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)});

	CByteArray dg13_data;
	CByteArray sod_data = getSodData(m_card);
	CByteArray authCert_data;
	MWLOG(LEV_DEBUG, MOD_APL, "Remote Address - Reading ID data (DG13)");
	m_card->selectApplication({PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)});
	m_card->readFile(PTEID_FILE_ID_V2, dg13_data);

	m_card->selectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});

	std::string serialNumber = m_card->getTokenSerialNumber();
	// Read ECDH parameters from card
	CByteArray ecdh_params = mutual_authentication.getECDHParams();

	m_card->readFile(PTEID_FILE_CERT_AUTHENTICATION_V2, authCert_data);

	// Send VERIFY command for Address PIN with cached value
	// In CC2 we can't guarantee that the verified state from a previous call to verify address PIN is kept in the card
	unsigned long ulRemaining = 0;
	m_card->getCalReader()->setSSO(true);
	m_card->getCalReader()->PinCmd(PIN_OP_VERIFY, m_card->getPin(ADDRESS_PIN_IDX), "", "", ulRemaining);
	MWLOG(LEV_DEBUG, MOD_APL, "Verified address PIN with cached value. Tries left: %lu", ulRemaining);

	// Clear cached PIN
	m_card->getCalReader()->setSSO(false);

	char *json_str = build_json_ecdh1(ecdh_params, dg13_data, sod_data, authCert_data, serialNumber);

	// Step 1 of the protocol
	PostResponse resp = post_json_remoteaddress(url_endpoint_ecdh1.c_str(), json_str, NULL);

	MWLOG(LEV_INFO, MOD_APL, "%s Endpoint (1) returned HTTP code: %ld", __FUNCTION__, resp.http_code);
	std::string cookie = parseCookieFromHeaders(resp.http_headers);

	if (cookie.size() > 0) {
		MWLOG(LEV_DEBUG, MOD_APL, "%s: Received cookie: %s", __FUNCTION__, cookie.c_str());
	}

	std::string ecdh1_kifd = parseECDH1Response(resp.http_response.c_str());

	if (ecdh1_kifd.empty()) {
		handleRemoteAddressError(sm_started, EIDMW_REMOTEADDR_SERVER_ERROR);
	}

	char *kicc = mutual_authentication.generalAuthenticate(ecdh1_kifd.c_str());

	if (!kicc) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Address loading aborted in generalAuthenticate!", __FUNCTION__);
		handleRemoteAddressError(sm_started, EIDMW_REMOTEADDR_SERVER_ERROR);
	}
	sm_started = true;

	json_str = build_json_ecdh2(kicc);

	// Step 2 of the protocol
	PostResponse resp2 = post_json_remoteaddress(url_endpoint_ecdh2.c_str(), json_str, cookie.c_str());

	MWLOG(LEV_DEBUG, MOD_APL, "%s: 2nd POST. HTTP code: %ld Received data: %s", __FUNCTION__, resp2.http_code,
		  resp2.http_response.c_str());

	RA_ECDH2Response ecdh2_obj = parseECDH2Response(resp2.http_response.c_str());

	if (ecdh2_obj.external_auth_apdus.size() == 0) {
		MWLOG(LEV_DEBUG, MOD_APL, "%s: 2nd POST. HTTP code: %ld Received data: %s", __FUNCTION__, resp2.http_code,
			  resp2.http_response.c_str());
		handleRemoteAddressError(sm_started, EIDMW_REMOTEADDR_SERVER_ERROR);
	}

	auto resp_external_auth = mutual_authentication.sendSequenceOfPrebuiltAPDUs(ecdh2_obj.external_auth_apdus);

	json_str = build_json_mutualauth_1(resp_external_auth);
	// Step 3 of the protocol
	PostResponse resp3 = post_json_remoteaddress(url_endpoint_mutual_auth1.c_str(), json_str, cookie.c_str());

	std::optional<RA_MutualAuthResponse> mutualauth1_obj = parseMutualAuthResponse1(resp3.http_response.c_str());

	if (!mutualauth1_obj.has_value()) {
		MWLOG(LEV_DEBUG, MOD_APL, "%s: 3rd POST. HTTP code: %ld Received data: %s", __FUNCTION__, resp3.http_code,
			  resp3.http_response.c_str());
		handleRemoteAddressError(sm_started, EIDMW_REMOTEADDR_SERVER_ERROR);
	}

	auto resp_internal_auth = mutual_authentication.remoteAddressStep3(mutualauth1_obj.value().signed_challenge_command,
																	   mutualauth1_obj.value().internal_auth_commands,
																	   mutualauth1_obj.value().pin_status_command);

	json_str = build_json_mutualauth_2(resp_internal_auth);

	// Step 4 of the protocol: returns address data if all goes well
	PostResponse resp4 = post_json_remoteaddress(url_endpoint_mutual_auth2.c_str(), json_str, cookie.c_str());
	MWLOG(LEV_DEBUG, MOD_APL, "%s: 4th POST. HTTP code: %ld Received data: %s", __FUNCTION__, resp4.http_code,
		  resp4.http_response.c_str());

	exception_code = validateRemoteAddressData(resp4.http_response.c_str(), CC2_PROTOCOL);

	breakSecureMessaging();

	if (exception_code != 0) {
		throw CMWEXCEPTION(exception_code);
	}
}

void APL_AddrEId::loadRemoteAddress() {
	long exception_code = 0;

	if (remoteAddressLoaded) {
		return;
	}

	if (m_card->getType() == APL_CARDTYPE_PTEID_IAS5) {
		loadRemoteAddress_CC2();
		return;
	}

	std::string url_endpoint_dh = buildRemoteAddressURL(CC1_PROTOCOL, 1);
	std::string url_endpoint_signchallenge = buildRemoteAddressURL(CC1_PROTOCOL, 2);
	std::string url_endpoint_readaddress = buildRemoteAddressURL(CC1_PROTOCOL, 3);

	MutualAuthentication mutual_authentication(m_card);
	DHParams dh_params;

	CByteArray sod_data = getSodData(m_card);
	CByteArray authCert_data = getAuthCert(m_card);

	mutual_authentication.getDHParams(&dh_params, true);

	char *json_str =
		build_json_obj_dhparams(dh_params, m_card->getFileID(), m_card->getFileAddress(), sod_data, authCert_data);

	// 1st POST
	PostResponse resp = post_json_remoteaddress(url_endpoint_dh.c_str(), json_str, NULL);

	MWLOG(LEV_INFO, MOD_APL, "%s Endpoint (1) returned HTTP code: %ld", __FUNCTION__, resp.http_code);

	std::string received_cookie = parseCookieFromHeaders(resp.http_headers);

	RA_DHParamsResponse dh_params_resp = parseDHParamsResponse(resp.http_response.c_str());

	if (dh_params_resp.error_code == 0) {
		if (!mutual_authentication.sendKIFD((char *)dh_params_resp.kifd.c_str())) {
			MWLOG(LEV_ERROR, MOD_APL, "loadRemoteAddress(): Card failed to accept DH server public key KIFD!");
			exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
			goto cleanup;
		}

		// 2nd POST
		char *kicc = mutual_authentication.getKICC();

		bool verified = mutual_authentication.verifyCert_CV_IFD((char *)dh_params_resp.cv_ifd_cert.c_str());

		if (!verified) {
			MWLOG(LEV_ERROR, MOD_APL, "Card failed to verify server-provided CV certificate!");
			exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
			goto cleanup;
		}

		char *chr = mutual_authentication.getPK_IFD_AUT((char *)dh_params_resp.cv_ifd_cert.c_str());

		char *challenge = mutual_authentication.generateChallenge(chr);
		if (challenge == NULL) {
			exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
			goto cleanup;
		}

		json_str = build_json_obj_sign_challenge(challenge, kicc);
		PostResponse resp =
			post_json_remoteaddress(url_endpoint_signchallenge.c_str(), json_str, received_cookie.c_str());

		MWLOG(LEV_INFO, MOD_APL, "%s Endpoint (2) returned HTTP code: %ld", __FUNCTION__, resp.http_code);

		RA_SignChallengeResponse signed_challenge_obj = parseSignChallengeResponse(resp.http_response.c_str());

		if (signed_challenge_obj.error_code == 0) {
			char *signed_challenge = (char *)signed_challenge_obj.signed_challenge.c_str();
			bool ret = mutual_authentication.verifySignedChallenge(signed_challenge);

			if (!ret) {
				MWLOG(LEV_ERROR, MOD_APL, "Card rejected server-provided signature. Process aborted!");
				exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
				goto cleanup;
			}

			MWLOG(LEV_DEBUG, MOD_APL, "From now on using Secure Messaging with the smartcard...");
			bool is_hex = true;
			CByteArray mse_internal_auth_cmd(signed_challenge_obj.set_se_command, is_hex);
			CByteArray internal_auth_cmd(signed_challenge_obj.internal_auth_command, is_hex);

			const unsigned char le_byte[] = {0x00};

			// Needed for T=1 smartcard protocol
			mse_internal_auth_cmd.Append(le_byte, sizeof(le_byte));
			internal_auth_cmd.Append(le_byte, sizeof(le_byte));

			// TODO: maybe catch cardlayer exceptions in sendAPDU and translate to SMARTCARD_ERROR

			CByteArray resp_mse_internal_auth = m_card->sendAPDU(mse_internal_auth_cmd);

			unsigned int sw12 = 0;
			if (!checkResultSW12(resp_mse_internal_auth, &sw12)) {
				MWLOG(LEV_ERROR, MOD_APL, "MSE SET INTERNAL Auth failed! SW12: %04X", sw12);
				exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
				goto cleanup;
			}

			CByteArray resp_internal_auth = m_card->sendAPDU(internal_auth_cmd);
			sw12 = 0;
			if (!checkResultSW12(resp_internal_auth, &sw12)) {
				MWLOG(LEV_ERROR, MOD_APL, "INTERNAL AUTHENTICATION failed! SW12: %04X", sw12);
				exception_code = EIDMW_REMOTEADDR_SMARTCARD_ERROR;
				goto cleanup;
			}

			json_str = build_json_obj_read_address(resp_mse_internal_auth, resp_internal_auth);
			resp = post_json_remoteaddress(url_endpoint_readaddress.c_str(), json_str, received_cookie.c_str());

			MWLOG(LEV_INFO, MOD_APL, "%s Endpoint (3) returned HTTP code: %ld", __FUNCTION__, resp.http_code);

			exception_code = validateRemoteAddressData(resp.http_response.c_str(), CC1_PROTOCOL);
		}
	}

cleanup:
	if (exception_code != 0) {
		throw CMWEXCEPTION(exception_code);
	}
}

void APL_AddrEId::invalidateAddress() { remoteAddressLoaded = false; }

const char *APL_AddrEId::getMunicipality() {

	loadRemoteAddress();
	return m_MunicipalityName.c_str();
}

const char *APL_AddrEId::getMunicipalityCode() {
	loadRemoteAddress();
	return m_MunicipalityCode.c_str();
}

const char *APL_AddrEId::getPlace() {
	loadRemoteAddress();
	return m_Place.c_str();
}

const char *APL_AddrEId::getCivilParish() {
	loadRemoteAddress();
	return m_CivilParishName.c_str();
}

const char *APL_AddrEId::getCivilParishCode() {
	loadRemoteAddress();
	return m_CivilParishCode.c_str();
}

const char *APL_AddrEId::getStreetName() {
	loadRemoteAddress();
	return m_StreetName.c_str();
}

const char *APL_AddrEId::getAbbrStreetType() {
	loadRemoteAddress();
	return m_AbbrStreetType.c_str();
}

const char *APL_AddrEId::getStreetType() {
	loadRemoteAddress();
	return m_StreetType.c_str();
}

const char *APL_AddrEId::getAbbrBuildingType() {
	loadRemoteAddress();
	return m_AbbrBuildingType.c_str();
}

const char *APL_AddrEId::getBuildingType() {
	loadRemoteAddress();
	return m_BuildingType.c_str();
}

const char *APL_AddrEId::getDoorNo() {
	loadRemoteAddress();
	return m_DoorNo.c_str();
}

const char *APL_AddrEId::getFloor() {
	loadRemoteAddress();
	return m_Floor.c_str();
}

const char *APL_AddrEId::getSide() {
	loadRemoteAddress();
	return m_Side.c_str();
}

const char *APL_AddrEId::getLocality() {
	loadRemoteAddress();
	return m_Locality.c_str();
}

const char *APL_AddrEId::getZip4() {
	loadRemoteAddress();
	return m_Zip4.c_str();
}

const char *APL_AddrEId::getZip3() {
	loadRemoteAddress();
	return m_Zip3.c_str();
}

const char *APL_AddrEId::getPostalLocality() {
	loadRemoteAddress();
	return m_PostalLocality.c_str();
}

const char *APL_AddrEId::getGeneratedAddressCode() {
	loadRemoteAddress();
	return m_Generated_Address_Code.c_str();
}

const char *APL_AddrEId::getDistrict() {
	loadRemoteAddress();
	return m_DistrictName.c_str();
}

const char *APL_AddrEId::getDistrictCode() {
	loadRemoteAddress();
	return m_DistrictCode.c_str();
}

const char *APL_AddrEId::getCountryCode() {
	loadRemoteAddress();
	return m_CountryCode.c_str();
}

bool APL_AddrEId::isNationalAddress() {
	loadRemoteAddress();
	return m_AddressType == "N";
}

const char *APL_AddrEId::getForeignCountry() {
	loadRemoteAddress();
	return m_Foreign_Country.c_str();
}

const char *APL_AddrEId::getForeignAddress() {
	loadRemoteAddress();
	return m_Foreign_Generic_Address.c_str();
}

const char *APL_AddrEId::getForeignCity() {
	loadRemoteAddress();
	return m_Foreign_City.c_str();
}

const char *APL_AddrEId::getForeignRegion() {
	loadRemoteAddress();
	return m_Foreign_Region.c_str();
}

const char *APL_AddrEId::getForeignLocality() {
	loadRemoteAddress();
	return m_Foreign_Locality.c_str();
}

const char *APL_AddrEId::getForeignPostalCode() {
	loadRemoteAddress();
	return m_Foreign_Postal_Code.c_str();
}

/*****************************************************************************************
---------------------------------------- APL_SodEid -----------------------------------------
*****************************************************************************************/
APL_SodEid::APL_SodEid(APL_EIDCard *card) { m_card = card; }

APL_SodEid::~APL_SodEid() {}

const CByteArray &APL_SodEid::getData() {
	const CByteArray &cb = m_card->getFileSod()->getData();

	m_card->getFileSod()->getAddressHash();

	return cb;
}

/*****************************************************************************************
---------------------------------------- APL_DocVersionInfo --------------------------------------------
*****************************************************************************************/
APL_DocVersionInfo::APL_DocVersionInfo(APL_EIDCard *card) { m_card = card; }

APL_DocVersionInfo::~APL_DocVersionInfo() {}

const char *APL_DocVersionInfo::getSerialNumber() { return m_card->getTokenSerialNumber(); }

const char *APL_DocVersionInfo::getTokenLabel() { return m_card->getTokenLabel(); }

const char *APL_DocVersionInfo::getComponentCode() { return m_card->getFileInfo()->getComponentCode(); }

const char *APL_DocVersionInfo::getOsNumber() { return m_card->getFileInfo()->getOsNumber(); }

const char *APL_DocVersionInfo::getOsVersion() { return m_card->getFileInfo()->getOsVersion(); }

const char *APL_DocVersionInfo::getSoftmaskNumber() { return m_card->getFileInfo()->getSoftmaskNumber(); }

const char *APL_DocVersionInfo::getSoftmaskVersion() { return m_card->getFileInfo()->getSoftmaskVersion(); }

const char *APL_DocVersionInfo::getAppletVersion() { return m_card->getAppletVersion(); }

const char *APL_DocVersionInfo::getGlobalOsVersion() { return m_card->getFileInfo()->getGlobalOsVersion(); }

const char *APL_DocVersionInfo::getAppletInterfaceVersion() {
	return m_card->getFileInfo()->getAppletInterfaceVersion();
}

const char *APL_DocVersionInfo::getPKCS1Support() { return m_card->getFileInfo()->getPKCS1Support(); }

const char *APL_DocVersionInfo::getKeyExchangeVersion() { return m_card->getFileInfo()->getKeyExchangeVersion(); }

const char *APL_DocVersionInfo::getAppletLifeCicle() { return m_card->getFileInfo()->getAppletLifeCicle(); }

const char *APL_DocVersionInfo::getGraphicalPersonalisation() {
	return m_card->getFileTokenInfo()->getGraphicalPersonalisation();
}

const char *APL_DocVersionInfo::getElectricalPersonalisation() {
	return m_card->getFileTokenInfo()->getElectricalPersonalisation();
}

const char *APL_DocVersionInfo::getElectricalPersonalisationInterface() {
	return m_card->getFileTokenInfo()->getElectricalPersonalisationInterface();
}
} // namespace eIDMW
