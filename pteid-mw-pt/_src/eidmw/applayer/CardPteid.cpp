/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011, 2014-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
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

#include "CardPteid.h"
#include "Util.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "CardFile.h"
#include "APLCertif.h"
#include "MWException.h"
#include "eidErrors.h"
#include "APLConfig.h"
#include "PhotoPteid.h"
#include "MiscUtil.h"
#include "APLPublicKey.h"
#include "Log.h"
#include "SODParser.h"

// C++ 11 header used for date parsing
#include <regex>
#include <ctime>

#include <openssl/err.h>
#include <iostream>
#include <fstream>

#include <cassert>
#include <climits>

namespace eIDMW {
/*****************************************************************************************
---------------------------------------- APL_EidFile_TRACE -----------------------------------------
*****************************************************************************************/
APL_EidFile_Trace::APL_EidFile_Trace(APL_EIDCard *card, CByteArray &appID)
	: APL_CardFile(card, PTEID_FILE_TRACE, NULL, appID) {}

APL_EidFile_Trace::~APL_EidFile_Trace() {}

tCardFileStatus APL_EidFile_Trace::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	return CARDFILESTATUS_OK;
}

void APL_EidFile_Trace::EmptyFields() {
	m_Validation.clear();
	m_mappedFields = false;
}

// Check for issuing date in "CC home delivery" period of card production
bool activeFlagShouldBeChecked(const char *issuing_date) {

	// create tm with 01/09/2019:
	std::tm timeinfo = std::tm();
	timeinfo.tm_year = 2019 - 1900;
	timeinfo.tm_mon = 8;  // month: september
	timeinfo.tm_mday = 1; // day: 1st
	std::time_t beginCCHOMEPeriod = std::mktime(&timeinfo);

	// create tm with 19/04/20:
	timeinfo.tm_year = 2021 - 1900;
	timeinfo.tm_mon = 3;   // month: April
	timeinfo.tm_mday = 19; // day: 19th

	std::time_t endCCHOMEPeriod = std::mktime(&timeinfo);

	std::cmatch cm;
	std::regex expression("(\\d{2}) (\\d{2}) (\\d{4})");

	std::regex_match(issuing_date, cm, expression);

	int day = 0, month = 0, year = 0;

	for (unsigned i = 0; i < cm.size(); i++) {
		switch (i) {
		// case 0 is ignored as it matches the full regex pattern
		case 1:
			day = std::stoi(cm[i].str());
			break;
		case 2:
			month = std::stoi(cm[i].str());
			break;
		case 3:
			year = std::stoi(cm[i].str());
			break;
		}
	}

	if (day == 0 || month == 0 || year == 0)
		return true;

	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;

	std::time_t issuingTime = std::mktime(&timeinfo);

	return issuingTime > endCCHOMEPeriod || issuingTime < beginCCHOMEPeriod;
}

bool APL_EidFile_Trace::MapFields() {
	if (m_mappedFields)
		return true;

	if (!m_card)
		return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	APL_EidFile_ID *idFile = pcard->getFileID();

	const char *issuing_date = idFile->getValidityBeginDate();

	if (!activeFlagShouldBeChecked(issuing_date)) {
		MWLOG(LEV_DEBUG, MOD_APL,
			  "Skipping Active Flag check because this card was issued between 1/9/2019 e 19/04/2021.");

		m_Validation = "O Cartão de Cidadão encontra-se activo";
		isCardActive = true;

	} else {
		CByteArray pteidngtraceBuffer;
		unsigned long ulLen = 0;
		CByteArray Validation;

		// Card Validation
		pteidngtraceBuffer = m_data.GetBytes(PTEIDNG_FIELD_TRACE_POS_VALIDATION, PTEIDNG_FIELD_TRACE_LEN_VALIDATION);

		Validation = pteidngtraceBuffer;
		if (Validation.ToString() == "01")
			m_Validation = "O Cartão de Cidadão encontra-se activo";
		else
			m_Validation = "O Cartão de Cidadão encontra-se inactivo";

		isCardActive = (pteidngtraceBuffer.GetByte(0) == PTEIDNG_ACTIVE_CARD);
	}

	m_mappedFields = true;

	return true;
}

bool APL_EidFile_Trace::ShowData() {
	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status = getStatus(false);
	if (status == CARDFILESTATUS_OK)
		return true;

	return false;
}

const char *APL_EidFile_Trace::getValidation() {
	if (ShowData())
		return m_Validation.c_str();

	return "";
}

bool APL_EidFile_Trace::isActive() {
	if (ShowData())
		return isCardActive;
	return false;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_MRZ -----------------------------------------
*****************************************************************************************/
APL_EidFile_MRZ::APL_EidFile_MRZ(APL_EIDCard *card)
	: APL_CardFile(card, PTEID_FILE_MRZ, NULL, {PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)}) {}

APL_EidFile_MRZ::~APL_EidFile_MRZ() {}

void APL_EidFile_MRZ::doSODCheck(bool check) {
	m_SODCheck = check;
	if (check) {
		m_isVerified = false;
		m_mappedFields = false;
	}
}

const char *APL_EidFile_MRZ::getMRZ() {
	if (ShowData())
		return m_Mrz.c_str();

	return "";
}

void APL_EidFile_MRZ::EmptyFields() { m_Mrz.clear(); }

bool APL_EidFile_MRZ::MapFields() {
	CByteArray len_buffer = m_data.GetBytes(4, 1);
	len_buffer.TrimRight('\0');

	CByteArray buffer = m_data.GetBytes(5, (unsigned long)len_buffer.GetByte(0));
	buffer.TrimRight('\0');
	m_Mrz.assign(reinterpret_cast<const char *>(buffer.GetBytes()), buffer.Size());

	return true;
}

tCardFileStatus APL_EidFile_MRZ::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_ID -----------------------------------------
*****************************************************************************************/
APL_EidFile_ID::APL_EidFile_ID(APL_EIDCard *card) : APL_CardFile(card, PTEID_FILE_ID, NULL) { m_photo = NULL; }

APL_EidFile_ID::APL_EidFile_ID(APL_EIDCard *card, const char *csPath) : APL_CardFile(card, csPath, NULL) {
	m_photo = NULL;
}

APL_EidFile_ID::~APL_EidFile_ID() {}

void APL_EidFile_ID::PackIdData(CByteArray &cb) {
	assert(m_IssuingEntity.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_IssuingEntity.c_str(), (unsigned long) m_IssuingEntity.length());
	assert(m_Country.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_Country.c_str(), (unsigned long) m_Country.length());
	assert(m_DocumentType.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_DocumentType.c_str(), (unsigned long) m_DocumentType.length());
	assert(m_DocumentNumber.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_DocumentNumber.c_str(), (unsigned long) m_DocumentNumber.length());
	assert(m_ChipNumber.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_ChipNumber.c_str(), (unsigned long) m_ChipNumber.length());
	assert(m_DocumentVersion.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_DocumentVersion.c_str(), (unsigned long) m_DocumentVersion.length());
	assert(m_ValidityBeginDate.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_ValidityBeginDate.c_str(),  (unsigned long) m_ValidityBeginDate.length());
	assert(m_LocalofRequest.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_LocalofRequest.c_str(),  (unsigned long) m_LocalofRequest.length());
	assert(m_ValidityEndDate.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_ValidityEndDate.c_str(),  (unsigned long) m_ValidityEndDate.length());
	assert(m_Surname.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_Surname.c_str(),  (unsigned long) m_Surname.length());
	assert(m_GivenName.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_GivenName.c_str(),  (unsigned long) m_GivenName.length());
	assert(m_Gender.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_Gender.c_str(),  (unsigned long) m_Gender.length());
	assert(m_Nationality.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_Nationality.c_str(),  (unsigned long) m_Nationality.length());
	assert(m_DateOfBirth.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_DateOfBirth.c_str(),  (unsigned long) m_DateOfBirth.length());
	assert(m_Height.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_Height.c_str(),  (unsigned long) m_Height.length());
	assert(m_CivilianIdNumber.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_CivilianIdNumber.c_str(),  (unsigned long) m_CivilianIdNumber.length());
	assert(m_SurnameMother.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_SurnameMother.c_str(),  (unsigned long) m_SurnameMother.length());
	assert(m_GivenNameMother.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_GivenNameMother.c_str(),  (unsigned long) m_GivenNameMother.length());
	assert(m_SurnameFather.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_SurnameFather.c_str(),  (unsigned long) m_SurnameFather.length());
	assert(m_GivenNameFather.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_GivenNameFather.c_str(),  (unsigned long) m_GivenNameFather.length());
	assert(m_AccidentalIndications.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_AccidentalIndications.c_str(),  (unsigned long) m_AccidentalIndications.length());
	assert(m_TaxNo.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_TaxNo.c_str(),  (unsigned long) m_TaxNo.length());
	assert(m_SocialSecurityNo.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_SocialSecurityNo.c_str(),  (unsigned long) m_SocialSecurityNo.length());
	assert(m_HealthNo.length() <= ULLONG_MAX);
	cb.Append((unsigned char *)m_HealthNo.c_str(),  (unsigned long) m_HealthNo.length());
}

void APL_EidFile_ID::PackPublicKeyData(CByteArray &cb) {
	cb.Append(*cardKey->getModulus());
	cb.Append(*cardKey->getExponent());
}

void APL_EidFile_ID::PackPictureData(CByteArray &cb) {
	cb.Append(*m_photo->getCbeff());
	cb.Append(*m_photo->getFacialrechdr());
	cb.Append(*m_photo->getFacialinfo());
	cb.Append(*m_photo->getImageinfo());
	cb.Append(*m_photo->getPhotoRaw());
}

tCardFileStatus APL_EidFile_ID::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	MapFieldsInternal();

	if (m_SODCheck) {
		CByteArray pkData;
		CByteArray idData;
		CByteArray picData;
		PackPublicKeyData(pkData);
		PackIdData(idData);
		PackPictureData(picData);

		if (!m_cryptoFwk->VerifyHashSha256(pkData, pcard->getFileSod()->getCardPublicKeyHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY: %s",
				  pcard->getFileSod()->getCardPublicKeyHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY);
		}
		if (!m_cryptoFwk->VerifyHashSha256(idData, pcard->getFileSod()->getIdHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "SOD_ERR_HASH_NO_MATCH_ID: %s",
				  pcard->getFileSod()->getIdHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ID);
		}
		if (!m_cryptoFwk->VerifyHashSha256(picData, pcard->getFileSod()->getPictureHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "SOD_ERR_HASH_NO_MATCH_PICTURE: %s",
				  pcard->getFileSod()->getPictureHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE);
		}
	}
	m_isVerified = true;

	return CARDFILESTATUS_OK;
}

void APL_EidFile_ID::EmptyFields() {
	m_DocumentVersion.clear();
	m_Country.clear();
	m_ChipNumber.clear();
	m_ValidityBeginDate.clear();
	m_ValidityEndDate.clear();
	m_LocalofRequest.clear();
	m_CivilianIdNumber.clear();
	m_Surname.clear();
	m_GivenName.clear();
	m_Nationality.clear();
	m_DateOfBirth.clear();
	m_Gender.clear();
	m_DocumentType.clear();
	m_HealthNo.clear();
	m_DocumentNumber.clear();
	m_TaxNo.clear();
	m_SocialSecurityNo.clear();
	m_Height.clear();
	m_IssuingEntity.clear();
	m_GivenNameFather.clear();
	m_SurnameFather.clear();
	m_GivenNameMother.clear();
	m_SurnameMother.clear();
	if (m_photo) {
		delete m_photo;
		m_photo = NULL;
	}
	m_PhotoHash.ClearContents();
	if (cardKey) {
		delete cardKey;
		cardKey = NULL;
	}
	m_mappedFields = false;
	m_isVerified = false;
	m_SODCheck = false;
}

void APL_EidFile_ID::MapFieldsInternal() {
	CByteArray pteidngidBuffer;

	// we dont want to read the fields every time
	if (m_mappedFields)
		return;

	// IDVersion - Card Version
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentVersion, PTEIDNG_FIELD_ID_LEN_DocumentVersion);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentVersion.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// ChipNr
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumberPAN, PTEIDNG_FIELD_ID_LEN_DocumentNumberPAN);
	pteidngidBuffer.TrimRight('\0');
	m_ChipNumber.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Country
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Country, PTEIDNG_FIELD_ID_LEN_Country);
	pteidngidBuffer.TrimRight('\0');
	m_Country.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// ValidityBeginDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityBeginDate, PTEIDNG_FIELD_ID_LEN_ValidityBeginDate);
	pteidngidBuffer.TrimRight('\0');
	m_ValidityBeginDate.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// ValidityEndDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityEndDate, PTEIDNG_FIELD_ID_LEN_ValidityEndDate);
	pteidngidBuffer.TrimRight('\0');
	m_ValidityEndDate.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// IssuingMunicipality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_LocalofRequest, PTEIDNG_FIELD_ID_LEN_LocalofRequest);
	pteidngidBuffer.TrimRight('\0');
	m_LocalofRequest.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Civilian Identification Number (NIC)
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_CivilianIdNumber, PTEIDNG_FIELD_ID_LEN_CivilianIdNumber);
	pteidngidBuffer.TrimRight('\0');
	m_CivilianIdNumber.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Surname
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Surname, PTEIDNG_FIELD_ID_LEN_Surname);
	pteidngidBuffer.TrimRight('\0');
	m_Surname.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// FirstName_1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Name, PTEIDNG_FIELD_ID_LEN_Name);
	pteidngidBuffer.TrimRight('\0');
	m_GivenName.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Nationality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Nacionality, PTEIDNG_FIELD_ID_LEN_Nacionality);
	pteidngidBuffer.TrimRight('\0');
	m_Nationality.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// DateOfBirth
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DateOfBirth, PTEIDNG_FIELD_ID_LEN_DateOfBirth);
	pteidngidBuffer.TrimRight('\0');
	m_DateOfBirth.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Gender
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Gender, PTEIDNG_FIELD_ID_LEN_Gender);
	pteidngidBuffer.TrimRight('\0');
	m_Gender.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// DocumentType
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentType, PTEIDNG_FIELD_ID_LEN_DocumentType);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentType.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Height
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Height, PTEIDNG_FIELD_ID_LEN_Height);
	pteidngidBuffer.TrimRight('\0');
	m_Height.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// DocumentNumber
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumber, PTEIDNG_FIELD_ID_LEN_DocumentNumber);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentNumber.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// TaxNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_TaxNo, PTEIDNG_FIELD_ID_LEN_TaxNo);
	pteidngidBuffer.TrimRight('\0');
	m_TaxNo.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// SocialSecurityNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SocialSecurityNo, PTEIDNG_FIELD_ID_LEN_SocialSecurityNo);
	pteidngidBuffer.TrimRight('\0');
	m_SocialSecurityNo.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// HealthNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_HealthNo, PTEIDNG_FIELD_ID_LEN_HealthNo);
	pteidngidBuffer.TrimRight('\0');
	m_HealthNo.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// IssuingEntity
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_IssuingEntity, PTEIDNG_FIELD_ID_LEN_IssuingEntity);
	pteidngidBuffer.TrimRight('\0');
	m_IssuingEntity.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// GivenNameFather
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_GivenNameFather, PTEIDNG_FIELD_ID_LEN_GivenNameFather);
	pteidngidBuffer.TrimRight('\0');
	m_GivenNameFather.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// SurnameFather
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SurnameFather, PTEIDNG_FIELD_ID_LEN_SurnameFather);
	pteidngidBuffer.TrimRight('\0');
	m_SurnameFather.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// GivenNameMother
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_GivenNameMother, PTEIDNG_FIELD_ID_LEN_GivenNameMother);
	pteidngidBuffer.TrimRight('\0');
	m_GivenNameMother.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// SurnameMother
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SurnameMother, PTEIDNG_FIELD_ID_LEN_SurnameMother);
	pteidngidBuffer.TrimRight('\0');
	m_SurnameMother.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// Photo
	{
		CByteArray cbeff;
		CByteArray facialrechdr;
		CByteArray facialinfo;
		CByteArray imageinfo;
		CByteArray photoRAW;

		photoRAW = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Photo, PTEIDNG_FIELD_ID_LEN_Photo);
		MWLOG(LEV_DEBUG, MOD_APL, "SOD photoRAW: %d", photoRAW.Size());
		photoRAW.TrimRight(0);
		MWLOG(LEV_DEBUG, MOD_APL, "SOD photoRAW: %d", photoRAW.Size());
		cbeff = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_CBEFF, PTEIDNG_FIELD_ID_LEN_CBEFF);
		facialrechdr = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FACIALRECHDR, PTEIDNG_FIELD_ID_LEN_FACIALRECHDR);
		facialinfo = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FACIALINFO, PTEIDNG_FIELD_ID_LEN_FACIALINFO);
		imageinfo = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_IMAGEINFO, PTEIDNG_FIELD_ID_LEN_IMAGEINFO);
		m_photo = new PhotoPteid(photoRAW, cbeff, facialrechdr, facialinfo, imageinfo);
	}

	// Card Authentication Key (modulus + exponent)
	{
		CByteArray modulus = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_MODULUS, PTEIDNG_FIELD_ID_LEN_MODULUS);
		CByteArray exponent = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_EXPONENT, PTEIDNG_FIELD_ID_LEN_EXPONENT);

		cardKey = new APLPublicKey(modulus, exponent);
	}
	// MRZ1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz1, PTEIDNG_FIELD_ID_LEN_Mrz1);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ1.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// MRZ2
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz2, PTEIDNG_FIELD_ID_LEN_Mrz2);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ2.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// MRZ3
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz3, PTEIDNG_FIELD_ID_LEN_Mrz3);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ3.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	// AccidentalIndications
	pteidngidBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ID_POS_AccidentalIndications, PTEIDNG_FIELD_ID_LEN_AccidentalIndications);
	pteidngidBuffer.TrimRight('\0');
	m_AccidentalIndications.assign((char *)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	m_mappedFields = true;
}

bool APL_EidFile_ID::MapFields() {
	// do nothing verify will load the data and then verify if sod check is active
	return true;
}

bool APL_EidFile_ID::ShowData() {
	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status = getStatus(false);
	if (status == CARDFILESTATUS_OK)
		return true;

	return false;
}

const char *APL_EidFile_ID::getDocumentVersion() {
	if (ShowData())
		return m_DocumentVersion.c_str();

	return "";
}

const char *APL_EidFile_ID::getCountry() {
	if (ShowData())
		return m_Country.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentType() {
	if (ShowData())
		return m_DocumentType.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenName() {
	if (ShowData())
		return m_GivenName.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurname() {
	if (ShowData())
		return m_Surname.c_str();

	return "";
}

const char *APL_EidFile_ID::getGender() {
	if (ShowData())
		return m_Gender.c_str();

	return "";
}

const char *APL_EidFile_ID::getDateOfBirth() {
	if (ShowData())
		return m_DateOfBirth.c_str();

	return "";
}

const char *APL_EidFile_ID::getNationality() {
	if (ShowData())
		return m_Nationality.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentPAN() {
	if (ShowData())
		return m_ChipNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityBeginDate() {
	if (ShowData())
		return m_ValidityBeginDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityEndDate() {
	if (ShowData())
		return m_ValidityEndDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getHeight() {
	if (ShowData())
		return m_Height.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentNumber() {
	if (ShowData())
		return m_DocumentNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getTaxNo() {
	if (ShowData())
		return m_TaxNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getSocialSecurityNumber() {
	if (ShowData())
		return m_SocialSecurityNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getHealthNumber() {
	if (ShowData())
		return m_HealthNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getIssuingEntity() {
	if (ShowData())
		return m_IssuingEntity.c_str();

	return "";
}

const char *APL_EidFile_ID::getLocalofRequest() {
	if (ShowData())
		return m_LocalofRequest.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenNameFather() {
	if (ShowData())
		return m_GivenNameFather.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurnameFather() {
	if (ShowData())
		return m_SurnameFather.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenNameMother() {
	if (ShowData())
		return m_GivenNameMother.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurnameMother() {
	if (ShowData())
		return m_SurnameMother.c_str();

	return "";
}

const char *APL_EidFile_ID::getParents() {

	std::string m_Parents =
		m_GivenNameFather + " " + m_SurnameFather + " * " + m_GivenNameMother + " " + m_SurnameMother;

	if (ShowData())
		return _strdup(m_Parents.c_str());

	return "";
}

PhotoPteid *APL_EidFile_ID::getPhotoObj() {

	if (ShowData())
		return m_photo;

	return NULL;
}

APLPublicKey *APL_EidFile_ID::getCardAuthKeyObj() {
	if (ShowData())
		return cardKey;

	return NULL;
}

const char *APL_EidFile_ID::getMRZ1() {
	if (ShowData())
		return m_MRZ1.c_str();

	return "";
}

const char *APL_EidFile_ID::getMRZ2() {
	if (ShowData())
		return m_MRZ2.c_str();

	return "";
}

const char *APL_EidFile_ID::getMRZ3() {
	if (ShowData())
		return m_MRZ3.c_str();

	return "";
}

const char *APL_EidFile_ID::getAccidentalIndications() {
	if (ShowData())
		return m_AccidentalIndications.c_str();

	return "";
}

const char *APL_EidFile_ID::getCivilianIdNumber() {
	if (ShowData())
		return m_CivilianIdNumber.c_str();

	return "";
}

const CByteArray &APL_EidFile_ID::getPhotoHash() {
	if (ShowData())
		return m_PhotoHash;

	return EmptyByteArray;
}

void APL_EidFile_ID::doSODCheck(bool check) {
	m_SODCheck = check;
	if (check) {
		m_isVerified = false;
		m_mappedFields = false;
	}
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_PHOTO -----------------------------------------
*****************************************************************************************/
APL_EidFile_Photo::APL_EidFile_Photo(APL_EIDCard *card)
	: APL_CardFile(card, PTEID_FILE_PHOTO, NULL, {PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)}) {
}

APL_EidFile_Photo::~APL_EidFile_Photo() {}

CByteArray &APL_EidFile_Photo::getPhotoRaw() {
	if (ShowData())
		return m_PhotoRaw;

	return EmptyByteArray;
}

void APL_EidFile_Photo::doSODCheck(bool check) {
	m_SODCheck = check;
	if (check) {
		m_isVerified = false;
		m_mappedFields = false;
	}
}

void APL_EidFile_Photo::EmptyFields() {
	m_PhotoRaw.ClearContents();
	m_SODCheck = false;
	m_isVerified = false;
	m_mappedFields = false;
}

tCardFileStatus APL_EidFile_Photo::VerifyFile() { return !m_card ? CARDFILESTATUS_ERROR : CARDFILESTATUS_OK; }

bool APL_EidFile_Photo::MapFields() {
	if (m_mappedFields)
		return true;

	size_t tag_offset{};

	// find 5F2E tag followed by 0x82.
	// stop iterating 5 bytes before (0x5F 0x2E 0x82 XX XX)
	for (size_t i = 0; i < m_data.Size() - 5; i++) {
		assert(i + 2 <= ULLONG_MAX);
		if (m_data.GetByte((unsigned long) i) == 0x5F && m_data.GetByte((unsigned long) (i + 1)) == 0x2E && m_data.GetByte((unsigned long) (i + 2)) == 0x82) {
			tag_offset = i;
			break;
		}
	}

	if (tag_offset == 0 || tag_offset >= m_data.Size())
		return false;

	assert(tag_offset <= ULLONG_MAX);
	const auto len = der_get_length(m_data.GetBytes((unsigned long) tag_offset));
	if (len == 0)
		return false;
	// Skip the 3 data blocks defined in ISO/IEC 19794-5 spec
	size_t photo_offset = tag_offset + 5 + PTEIDNG_FIELD_ID_LEN_FACIALRECHDR + PTEIDNG_FIELD_ID_LEN_FACIALINFO +
								 PTEIDNG_FIELD_ID_LEN_IMAGEINFO;
	assert(photo_offset <= ULLONG_MAX);
	m_PhotoRaw.Append(m_data.GetBytes((unsigned long) photo_offset));

	m_mappedFields = true;
	return true;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_ID_V2 -----------------------------------------
*****************************************************************************************/
APL_EidFile_ID_V2::APL_EidFile_ID_V2(APL_EIDCard *card) : APL_EidFile_ID(card, PTEID_FILE_ID_V2) {
	m_appId = {PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)};
	m_EidFile_MRZ = nullptr;
	m_EidFile_Photo = nullptr;
}

APL_EidFile_ID_V2::~APL_EidFile_ID_V2() {}

bool APL_EidFile_ID_V2::LoadPhotoFile() {
	if (m_EidFile_Photo == nullptr) {
		APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);
		m_EidFile_Photo = new APL_EidFile_Photo(pcard);
	}

	;
	// Could not load or parse photo file
	if (m_EidFile_Photo->getStatus(false) != CARDFILESTATUS_OK) {
		throw CMWEXCEPTION(EIDMW_ERR_CARD);
	}

	m_photo = new PhotoPteid(m_EidFile_Photo->getPhotoRaw());

	if (m_SODCheck) {
		// SOD verification that was delayed and separated from ID & MRZ verification
		const auto photo_data = m_EidFile_Photo->getData();
		const auto pcard = dynamic_cast<APL_EIDCard *>(m_card);
		const auto file_sod = pcard->getFileSod();
		if (!m_cryptoFwk->VerifyHashSha256(photo_data, file_sod->getPictureHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE: %s",
				  file_sod->getPictureHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE);
		}
	}

	return true;
}

bool APL_EidFile_ID_V2::LoadMRZFile() {
	if (m_EidFile_MRZ == nullptr) {
		APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);
		m_EidFile_MRZ = new APL_EidFile_MRZ(pcard);
	}

	return m_EidFile_MRZ->getStatus(false) == CARDFILESTATUS_OK;
}

bool APL_EidFile_ID_V2::ShowData() {
	bool mrz_status = LoadMRZFile();
	bool id_status = getStatus(false) == CARDFILESTATUS_OK;

	return mrz_status && id_status;
}

tCardFileStatus APL_EidFile_ID_V2::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	MapFieldsInternal();

	if (m_SODCheck) {
		const auto pcard = dynamic_cast<APL_EIDCard *>(m_card);
		const auto file_sod = pcard->getFileSod();

		const auto id_data = m_data;
		const auto mrz_data = m_EidFile_MRZ->getData();

		if (!m_cryptoFwk->VerifyHashSha256(mrz_data, file_sod->getMrzHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "SOD_ERR_HASH_NO_MATCH_MRZ: %s",
				  file_sod->getMrzHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_MRZ);
		}

		if (!m_cryptoFwk->VerifyHashSha256(id_data, file_sod->getIdHash())) {
			MWLOG(LEV_DEBUG, MOD_APL, "SOD_ERR_HASH_NO_MATCH_ID: %s",
				  file_sod->getIdHash().ToString(true, false).c_str());
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ID);
		}
	}

	m_isVerified = true;
	return CARDFILESTATUS_OK;
}

void APL_EidFile_ID_V2::MapFieldsInternal() {
	const auto id_file = eIDMW::decode_id_data(m_data);

	if (id_file == NULL) {
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

	m_DocumentVersion.assign((char *)(id_file->document_version->data), id_file->document_version->length);
	m_Country.assign((char *)(id_file->issuing_state->data), id_file->issuing_state->length);
	m_ValidityBeginDate.assign((char *)(id_file->issuing_date->data), id_file->issuing_date->length);
	m_ValidityEndDate.assign((char *)(id_file->expire_date->data), id_file->expire_date->length);
	m_LocalofRequest.assign((char *)(id_file->place_order->data), id_file->place_order->length);
	m_CivilianIdNumber.assign((char *)(id_file->civil_id->data), id_file->civil_id->length);
	m_Surname.assign((char *)(id_file->surname->data), id_file->surname->length);
	m_GivenName.assign((char *)(id_file->givenname->data), id_file->givenname->length);
	m_Nationality.assign((char *)(id_file->nationality_code->data), id_file->nationality_code->length);
	m_DateOfBirth.assign((char *)(id_file->date_birth->data), id_file->date_birth->length);
	m_Gender.assign((char *)(id_file->gender->data), id_file->gender->length);
	m_DocumentType.assign((char *)(id_file->document_label->data), id_file->document_label->length);
	m_Height.assign((char *)(id_file->height->data), id_file->height->length);
	m_DocumentNumber.assign((char *)(id_file->document_number->data), id_file->document_number->length);
	m_TaxNo.assign((char *)(id_file->tax_id->data), id_file->tax_id->length);
	m_SocialSecurityNo.assign((char *)(id_file->socialsec_id->data), id_file->socialsec_id->length);
	m_HealthNo.assign((char *)(id_file->health_id->data), id_file->health_id->length);
	m_IssuingEntity.assign((char *)(id_file->issuing_entity->data), id_file->issuing_entity->length);
	m_GivenNameFather.assign((char *)(id_file->father_givenname->data), id_file->father_givenname->length);
	m_SurnameFather.assign((char *)(id_file->father_surname->data), id_file->father_surname->length);
	m_GivenNameMother.assign((char *)(id_file->mother_givenname->data), id_file->mother_givenname->length);
	m_SurnameMother.assign((char *)(id_file->mother_surname->data), id_file->mother_surname->length);
	// Optional field in ID file
	if (id_file->possible_indications != NULL) {
		m_AccidentalIndications.assign((char *)(id_file->possible_indications->data),
									   id_file->possible_indications->length);
	}

	const auto full_mrz = m_EidFile_MRZ->getMRZ();
	const auto mrz_len = strlen(full_mrz);
	// Porto Seguro card type has a smaller MRZ field
	if (mrz_len < (PTEIDNG_FIELD_ID_LEN_Mrz1 + PTEIDNG_FIELD_ID_LEN_Mrz2 + PTEIDNG_FIELD_ID_LEN_Mrz3)) {
		const char *p_mrz_line_sep = strchr(full_mrz, '/');
		auto mrz_offset = p_mrz_line_sep - full_mrz;
		m_MRZ1.assign(full_mrz, mrz_offset);
		m_MRZ2.assign(p_mrz_line_sep + 1, mrz_len - mrz_offset - 1);
	} else {
		m_MRZ1.assign(full_mrz, PTEIDNG_FIELD_ID_LEN_Mrz1);
		m_MRZ2.assign(full_mrz + PTEIDNG_FIELD_ID_LEN_Mrz1, PTEIDNG_FIELD_ID_LEN_Mrz2);
		m_MRZ3.assign(full_mrz + PTEIDNG_FIELD_ID_LEN_Mrz1 + PTEIDNG_FIELD_ID_LEN_Mrz2, PTEIDNG_FIELD_ID_LEN_Mrz3);
	}

	// TODO (DEV-CC2): parse public key from DG13 data to make it available in eidlib

	IDFILE_free(id_file);
	m_mappedFields = true;
}

// Deprecated for CC v2
const char *APL_EidFile_ID_V2::getDocumentPAN() { return ""; }

PhotoPteid *APL_EidFile_ID_V2::getPhotoObj() {
	if (m_photo) {
		return m_photo;
	}

	if (m_EidFile_Photo == nullptr) {
		const auto status = LoadPhotoFile();
		if (!status)
			return NULL;
	}

	return m_photo;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Address -----------------------------------------
*****************************************************************************************/
const std::string APL_EidFile_Address::m_NATIONAL = "N";
const std::string APL_EidFile_Address::m_FOREIGN = "I";
APL_EidFile_Address::APL_EidFile_Address(APL_EIDCard *card) : APL_CardFile(card, PTEID_FILE_ADDRESS, NULL) {}

APL_EidFile_Address::~APL_EidFile_Address() {}

void APL_EidFile_Address::PackAddressData(CByteArray &cb, bool isNational) {

	if (isNational) {
		assert(m_CountryCode.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_CountryCode.c_str(), (unsigned long) m_CountryCode.length());
		assert(m_DistrictCode.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_DistrictCode.c_str(), (unsigned long) m_DistrictCode.length());
		assert(m_DistrictDescription.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_DistrictDescription.c_str(), (unsigned long) m_DistrictDescription.length());
		assert(m_MunicipalityCode.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_MunicipalityCode.c_str(), (unsigned long) m_MunicipalityCode.length());
		assert(m_MunicipalityDescription.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_MunicipalityDescription.c_str(), (unsigned long) m_MunicipalityDescription.length());
		assert(m_CivilParishCode.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_CivilParishCode.c_str(), (unsigned long) m_CivilParishCode.length());
		assert(m_CivilParishDescription.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_CivilParishDescription.c_str(), (unsigned long) m_CivilParishDescription.length());
		assert(m_AbbrStreetType.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_AbbrStreetType.c_str(), (unsigned long) m_AbbrStreetType.length());
		assert(m_StreetType.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_StreetType.c_str(), (unsigned long) m_StreetType.length());
		assert(m_StreetName.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_StreetName.c_str(), (unsigned long) m_StreetName.length());
		assert(m_AbbrBuildingType.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_AbbrBuildingType.c_str(), (unsigned long) m_AbbrBuildingType.length());
		assert(m_BuildingType.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_BuildingType.c_str(), (unsigned long) m_BuildingType.length());
		assert(m_DoorNo.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_DoorNo.c_str(), (unsigned long) m_DoorNo.length());
		assert(m_Floor.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Floor.c_str(), (unsigned long) m_Floor.length());
		assert(m_Side.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Side.c_str(), (unsigned long) m_Side.length());
		assert(m_Place.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Place.c_str(), (unsigned long) m_Place.length());
		assert(m_Locality.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Locality.c_str(), (unsigned long) m_Locality.length());
		assert(m_Zip4.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Zip4.c_str(), (unsigned long) m_Zip4.length());
		assert(m_Zip3.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Zip3.c_str(), (unsigned long) m_Zip3.length());
		assert(m_PostalLocality.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_PostalLocality.c_str(), (unsigned long) m_PostalLocality.length());
		assert(m_Generated_Address_Code.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Generated_Address_Code.c_str(), (unsigned long) m_Generated_Address_Code.length());
	} else {
		assert(m_CountryCode.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_CountryCode.c_str(), (unsigned long) m_CountryCode.length());
		assert(m_Foreign_Country.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_Country.c_str(), (unsigned long) m_Foreign_Country.length());
		assert(m_Foreign_Generic_Address.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_Generic_Address.c_str(), (unsigned long) m_Foreign_Generic_Address.length());
		assert(m_Foreign_City.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_City.c_str(), (unsigned long) m_Foreign_City.length());
		assert(m_Foreign_Region.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_Region.c_str(), (unsigned long) m_Foreign_Region.length());
		assert(m_Foreign_Locality.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_Locality.c_str(), (unsigned long) m_Foreign_Locality.length());
		assert(m_Foreign_Postal_Code.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Foreign_Postal_Code.c_str(), (unsigned long) m_Foreign_Postal_Code.length());
		assert(m_Generated_Address_Code.length() <= ULLONG_MAX);
		cb.Append((unsigned char *)m_Generated_Address_Code.c_str(), (unsigned long) m_Generated_Address_Code.length());
	}
}

void APL_EidFile_Address::MapFieldsInternal() {

	if (m_mappedFields) // have we mapped the fields yet?
		return;

	CByteArray pteidngAddressBuffer;

	// Address Type
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_AddressType.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Country code
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_COUNTRY, PTEIDNG_FIELD_ADDRESS_LEN_COUNTRY);
	pteidngAddressBuffer.TrimRight('\0');
	m_CountryCode.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	if (m_AddressType == m_FOREIGN)
		ForeignerAddressFields();
	else
		AddressFields();

	// So we've mapped the fields no need to map them again
	m_mappedFields = true;
}

// Check for issuing date in "buggy" period of card production
bool addressSODShouldBeChecked(const char *issuing_date) {

	// create tm with 01/01/2017:
	std::tm timeinfo = std::tm();
	timeinfo.tm_year = 2017 - 1900;
	timeinfo.tm_mon = 0;  // month: january
	timeinfo.tm_mday = 1; // day: 1st
	std::time_t beginBuggyPeriod = std::mktime(&timeinfo);

	timeinfo.tm_year = 2018 - 1900;
	timeinfo.tm_mon = 1;   // month: february
	timeinfo.tm_mday = 20; // day: 20th

	std::time_t endBuggyPeriod = std::mktime(&timeinfo);

	std::cmatch cm;
	std::regex expression("(\\d{2}) (\\d{2}) (\\d{4})");

	std::regex_match(issuing_date, cm, expression);

	int day = 0, month = 0, year = 0;

	for (unsigned i = 0; i < cm.size(); i++) {
		switch (i) {
		// case 0 is ignored as it matches the full regex pattern
		case 1:
			day = std::stoi(cm[i].str());
			break;
		case 2:
			month = std::stoi(cm[i].str());
			break;
		case 3:
			year = std::stoi(cm[i].str());
			break;
		}
	}

	if (day == 0 || month == 0 || year == 0)
		return true;

	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;

	std::time_t issuingTime = std::mktime(&timeinfo);

	return issuingTime > endBuggyPeriod || issuingTime < beginBuggyPeriod;
}

tCardFileStatus APL_EidFile_Address::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	MapFieldsInternal();

	if (m_SODCheck) {

		APL_EidFile_ID *idFile = pcard->getFileID();

		const char *issuing_date = idFile->getValidityBeginDate();

		if (!addressSODShouldBeChecked(issuing_date)) {
			MWLOG(LEV_DEBUG, MOD_APL,
				  "Skipping Address SOD check because this card was issued between 01/01/2017 and 20/02/2018");
		} else {
			CByteArray addrData;
			PackAddressData(addrData, m_AddressType != m_FOREIGN);

			if (!m_cryptoFwk->VerifyHashSha256(addrData, pcard->getFileSod()->getAddressHash())) {
				MWLOG(LEV_DEBUG, MOD_APL, "SOD getCardAddressHash: %s",
					  pcard->getFileSod()->getAddressHash().ToString(true, false).c_str());
				throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS);
			}
		}
	}

	m_isVerified = true;
	return CARDFILESTATUS_OK;
}

bool APL_EidFile_Address::ShowData() {
	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status = getStatus(false);
	if (status == CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_Address::EmptyFields() {
	m_AddressFile.clear();

	// common address fields
	m_AddressType.clear();
	m_CountryCode.clear();
	m_Generated_Address_Code.clear();

	// portuguese address fields
	m_DistrictCode.clear();
	m_DistrictDescription.clear();
	m_MunicipalityCode.clear();
	m_MunicipalityDescription.clear();
	m_CivilParishCode.clear();
	m_CivilParishDescription.clear();
	m_AbbrStreetType.clear();
	m_StreetType.clear();
	m_StreetName.clear();
	m_AbbrBuildingType.clear();
	m_BuildingType.clear();
	m_DoorNo.clear();
	m_Floor.clear();
	m_Side.clear();
	m_Place.clear();
	m_Locality.clear();
	m_Zip4.clear();
	m_Zip3.clear();
	m_PostalLocality.clear();

	// foreign address fields
	m_Foreign_Country.clear();
	m_Foreign_Generic_Address.clear();
	m_Foreign_City.clear();
	m_Foreign_Region.clear();
	m_Foreign_Locality.clear();
	m_Foreign_Postal_Code.clear();

	m_mappedFields = false;
	m_isVerified = false;
	m_SODCheck = false;
}

void APL_EidFile_Address::AddressFields() {
	CByteArray pteidngAddressBuffer;

	// District Code
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT);
	pteidngAddressBuffer.TrimRight('\0');
	m_DistrictCode.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// District Description
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT_DESCRIPTION);
	pteidngAddressBuffer.TrimRight('\0');
	m_DistrictDescription.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Municipality Code
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY, PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_MunicipalityCode.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Municipality Description
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY_DESCRIPTION,
										   PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY_DESCRIPTION);
	pteidngAddressBuffer.TrimRight('\0');
	m_MunicipalityDescription.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// CivilParish Code
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH, PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH);
	pteidngAddressBuffer.TrimRight('\0');
	m_CivilParishCode.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// CivilParish Description
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH_DESCRIPTION,
										   PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH_DESCRIPTION);
	pteidngAddressBuffer.TrimRight('\0');
	m_CivilParishDescription.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Abbreviated Street Type
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_STREET_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_AbbrStreetType.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Street Type
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_STREET_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_StreetType.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Street Name
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREETNAME, PTEIDNG_FIELD_ADDRESS_LEN_STREETNAME);
	pteidngAddressBuffer.TrimRight('\0');
	m_StreetName.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Abbreviated Building Type
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_BUILDING_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_AbbrBuildingType.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Building Type
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_BUILDING_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_BuildingType.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// DoorNo
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DOORNO, PTEIDNG_FIELD_ADDRESS_LEN_DOORNO);
	pteidngAddressBuffer.TrimRight('\0');
	m_DoorNo.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Floor
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_FLOOR, PTEIDNG_FIELD_ADDRESS_LEN_FLOOR);
	pteidngAddressBuffer.TrimRight('\0');
	m_Floor.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Side
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_SIDE, PTEIDNG_FIELD_ADDRESS_LEN_SIDE);
	pteidngAddressBuffer.TrimRight('\0');
	m_Side.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Place
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_PLACE, PTEIDNG_FIELD_ADDRESS_LEN_PLACE);
	pteidngAddressBuffer.TrimRight('\0');
	m_Place.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Locality
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_LOCALITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_Locality.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Zip4
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP4, PTEIDNG_FIELD_ADDRESS_LEN_ZIP4);
	pteidngAddressBuffer.TrimRight('\0');
	m_Zip4.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Zip3
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP3, PTEIDNG_FIELD_ADDRESS_LEN_ZIP3);
	pteidngAddressBuffer.TrimRight('\0');
	m_Zip3.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Postal Locality
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_POSTALLOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_POSTALLOCALITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_PostalLocality.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Generated Address Code
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_GENADDRESS_NUM, PTEIDNG_FIELD_ADDRESS_LEN_GENADDRESS_NUM);
	pteidngAddressBuffer.TrimRight('\0');
	m_Generated_Address_Code.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());
}

void APL_EidFile_Address::ForeignerAddressFields() {
	CByteArray pteidngAddressBuffer;

	// Foreign Country
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_COUNTRY_DESCRIPTION,
										   PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_COUNTRY_DESCRIPTION);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Country.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign Generic Address
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_ADDRESS, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_ADDRESS);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Generic_Address.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign City
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_CITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_CITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_City.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign Region
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_REGION, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_REGION);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Region.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign Locality
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_LOCALITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Locality.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign Postal Code
	pteidngAddressBuffer =
		m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_POSTAL_CODE, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_POSTAL_CODE);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Postal_Code.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Foreign Generated Address Code
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_GENADDRESS_NUM,
										   PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_GENADDRESS_NUM);
	pteidngAddressBuffer.TrimRight('\0');
	m_Generated_Address_Code.assign((char *)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());
}

bool APL_EidFile_Address::MapFields() {
	// do nothing verify will load the data and then verify if sod check is active
	return true;
}

const char *APL_EidFile_Address::getMunicipality() {
	if (ShowData())
		return m_MunicipalityDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getMunicipalityCode() {
	if (ShowData())
		return m_MunicipalityCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getDistrict() {
	if (ShowData())
		return m_DistrictDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getDistrictCode() {
	if (ShowData())
		return m_DistrictCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getCivilParish() {
	if (ShowData())
		return m_CivilParishDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getCivilParishCode() {
	if (ShowData())
		return m_CivilParishCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getStreetName() {
	if (ShowData())
		return m_StreetName.c_str();

	return "";
}

const char *APL_EidFile_Address::getAbbrStreetType() {
	if (ShowData())
		return m_AbbrStreetType.c_str();

	return "";
}

const char *APL_EidFile_Address::getStreetType() {
	if (ShowData())
		return m_StreetType.c_str();

	return "";
}

const char *APL_EidFile_Address::getAbbrBuildingType() {
	if (ShowData())
		return m_AbbrBuildingType.c_str();

	return "";
}

const char *APL_EidFile_Address::getBuildingType() {
	if (ShowData())
		return m_BuildingType.c_str();

	return "";
}

const char *APL_EidFile_Address::getDoorNo() {
	if (ShowData())
		return m_DoorNo.c_str();

	return "";
}

const char *APL_EidFile_Address::getFloor() {
	if (ShowData())
		return m_Floor.c_str();

	return "";
}

const char *APL_EidFile_Address::getSide() {
	if (ShowData())
		return m_Side.c_str();

	return "";
}

const char *APL_EidFile_Address::getLocality() {
	if (ShowData())
		return m_Locality.c_str();

	return "";
}

const char *APL_EidFile_Address::getZip4() {
	if (ShowData())
		return m_Zip4.c_str();

	return "";
}

const char *APL_EidFile_Address::getZip3() {
	if (ShowData())
		return m_Zip3.c_str();

	return "";
}

const char *APL_EidFile_Address::getPlace() {
	if (ShowData())
		return m_Place.c_str();

	return "";
}

const char *APL_EidFile_Address::getPostalLocality() {
	if (ShowData())
		return m_PostalLocality.c_str();

	return "";
}

const char *APL_EidFile_Address::getGeneratedAddressCode() {
	if (ShowData())
		return m_Generated_Address_Code.c_str();

	return "";
}

const char *APL_EidFile_Address::getCountryCode() {
	if (ShowData())
		return m_CountryCode.c_str();

	return "";
}

bool APL_EidFile_Address::isNationalAddress() {
	ShowData();
	return (m_AddressType.compare(m_NATIONAL) == 0);
}

const char *APL_EidFile_Address::getForeignCountry() {
	if (ShowData())
		return m_Foreign_Country.c_str();

	return "";
}

const char *APL_EidFile_Address::getForeignAddress() {
	if (ShowData())
		return m_Foreign_Generic_Address.c_str();

	return "";
}

const char *APL_EidFile_Address::getForeignCity() {
	if (ShowData())
		return m_Foreign_City.c_str();

	return "";
}

const char *APL_EidFile_Address::getForeignRegion() {
	if (ShowData())
		return m_Foreign_Region.c_str();

	return "";
}

const char *APL_EidFile_Address::getForeignLocality() {
	if (ShowData())
		return m_Foreign_Locality.c_str();

	return "";
}

const char *APL_EidFile_Address::getForeignPostalCode() {
	if (ShowData())
		return m_Foreign_Postal_Code.c_str();

	return "";
}

void APL_EidFile_Address::doSODCheck(bool check) { m_SODCheck = check; }

/*****************************************************************************************
---------------------------------------- APL_EidFile_Sod -----------------------------------------
*****************************************************************************************/
APL_EidFile_Sod::APL_EidFile_Sod(APL_EIDCard *card) : APL_CardFile(card, PTEID_FILE_SOD, NULL) {}

APL_EidFile_Sod::APL_EidFile_Sod(APL_EIDCard *card, const char *csPath) : APL_CardFile(card, csPath, NULL) {
	if (card->getType() == APL_CARDTYPE_PTEID_IAS5) {
		m_appId = {PTEID_2_APPLET_NATIONAL_DATA, sizeof(PTEID_2_APPLET_NATIONAL_DATA)};
	}
}

APL_EidFile_Sod::~APL_EidFile_Sod() {}

tCardFileStatus APL_EidFile_Sod::VerifyFile() {
	tCardFileStatus filestatus = CARDFILESTATUS_ERROR_SIGNATURE;

	if (m_isVerified) // no need to check again
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	if (!m_SODCheck) // check is not activated
		return CARDFILESTATUS_OK;

	PKCS7 *p7 = NULL;
	char *error_msg = NULL;
	bool verifySOD = false;

	ERR_load_crypto_strings();
	OpenSSL_add_all_digests();

	const unsigned char *temp = m_data.GetBytes();

	long int len = m_data.Size();
	temp += 4; // Skip the ASN.1 Application 23 tag + 3-byte length (DER type 77)

	p7 = d2i_PKCS7(NULL, (const unsigned char **)&temp, len);

	if (!p7) {
		error_msg = Openssl_errors_to_string();
		MWLOG(LEV_ERROR, MOD_APL, "EidFile_Sod: Failed to decode SOD PKCS7 object! Openssl errors:\n%s",
			  error_msg != NULL ? error_msg : "N/A");
		free(error_msg);
		throw CMWEXCEPTION(EIDMW_SOD_ERR_INVALID_PKCS7);
	}

	X509_STORE *store = X509_STORE_new();

	// Load only the SOD relevant root certificates
	for (unsigned long i = 0; i < pcard->getCertificates()->countSODCAs(); i++) {
		APL_Certif *sod_ca = pcard->getCertificates()->getSODCA(i);
		X509 *pX509 = NULL;
		const unsigned char *p = sod_ca->getData().GetBytes();
		pX509 = d2i_X509(&pX509, &p, sod_ca->getData().Size());
		X509_STORE_add_cert(store, pX509);
		MWLOG(LEV_DEBUG, MOD_APL, "%d. Adding certificate Subject CN: %s", i, sod_ca->getOwnerName());
	}

	BIO *Out = BIO_new(BIO_s_mem());

	verifySOD = PKCS7_verify(p7, NULL, store, NULL, Out, 0) == 1;
	if (verifySOD) {
		unsigned char *p;
		long size;
		size = BIO_get_mem_data(Out, &p);
		m_encapsulatedContent.Append(p, size);

		m_isVerified = true;
		filestatus = CARDFILESTATUS_OK;
	} else {
		error_msg = Openssl_errors_to_string();

		// Log specific OpenSSL error
		MWLOG(LEV_ERROR, MOD_APL, "EidFile_Sod:: Error validating SOD signature. OpenSSL errors:\n%s",
			  error_msg ? error_msg : "N/A");
		free(error_msg);
	}

	X509_STORE_free(store);
	BIO_free_all(Out);
	PKCS7_free(p7);

	if (!verifySOD)
		throw CMWEXCEPTION(EIDMW_SOD_ERR_VERIFY_SOD_SIGN);

	return filestatus;
}

bool APL_EidFile_Sod::ShowData() {
	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status = getStatus(false);
	if (status == CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_Sod::EmptyFields() {
	m_mappedFields = false;
	m_isVerified = false;
	m_picHash.ClearContents();
	m_pkHash.ClearContents();
	m_idHash.ClearContents();
	m_addressHash.ClearContents();
	m_encapsulatedContent.ClearContents();
}

void APL_EidFile_Sod::doSODCheck(bool check) {
	m_SODCheck = check;
	if (check) {
		m_isVerified = false;
		m_mappedFields = false;
	}
}

bool APL_EidFile_Sod::MapFields() {
	if (m_mappedFields) // map only one time
		return true;

	if (!m_SODCheck) // map only if sod check is active
		return true;

	const auto card_type = m_card->getType();

	SODParser parser;

	// Select the valid tags dependent on the card type
	std::vector<int> valid_tags;
	if (card_type == APL_CARDTYPE_PTEID_IAS5)
		valid_tags = PTEID_FILE_VALID_SOD_FILE_TAGS_V2;
	else
		valid_tags = PTEID_FILE_VALID_SOD_FILE_TAGS;

	parser.ParseSodEncapsulatedContent(m_encapsulatedContent, valid_tags);
	SODAttributes &attr = parser.getHashes();

	// Hashes are sorted differently based on card type
	if (card_type == APL_CARDTYPE_PTEID_IAS5) {
		m_mrzHash.Append(attr.hashes[0]);
		m_picHash.Append(attr.hashes[1]);
		m_idHash.Append(attr.hashes[2]);
		m_pkHash.Append(attr.hashes[4]);
	} else {
		m_idHash.Append(attr.hashes[0]);
		m_addressHash.Append(attr.hashes[1]);
		m_picHash.Append(attr.hashes[2]);
		m_pkHash.Append(attr.hashes[3]);
	}

	m_mappedFields = true;

	return true;
}

const CByteArray &APL_EidFile_Sod::getMrzHash() {
	if (ShowData())
		return m_mrzHash;
	return EmptyByteArray;
}

const CByteArray &APL_EidFile_Sod::getAddressHash() {
	if (ShowData())
		return m_addressHash;
	return EmptyByteArray;
}

const CByteArray &APL_EidFile_Sod::getPictureHash() {
	if (ShowData())
		return m_picHash;
	return EmptyByteArray;
}

const CByteArray &APL_EidFile_Sod::getCardPublicKeyHash() {
	if (ShowData())
		return m_pkHash;
	return EmptyByteArray;
}

const CByteArray &APL_EidFile_Sod::getIdHash() {
	if (ShowData())
		return m_idHash;
	return EmptyByteArray;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_PersoData -----------------------------------------
*****************************************************************************************/
APL_EidFile_PersoData::APL_EidFile_PersoData(APL_EIDCard *card) : APL_CardFile(card, PTEID_FILE_PERSODATA, NULL) {}

APL_EidFile_PersoData::~APL_EidFile_PersoData() {}

tCardFileStatus APL_EidFile_PersoData::VerifyFile() {
	if (!m_card)
		return CARDFILESTATUS_ERROR;

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_PersoData::ShowData() {
	APL_EIDCard *pcard = dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status = getStatus(false);
	if (status == CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_PersoData::EmptyFields() {
	m_PersoData.clear();
	m_mappedFields = false;
}

bool APL_EidFile_PersoData::MapFields() {
	if (m_mappedFields)
		return true;

	CByteArray pteidngPersoDataBuffer;
	unsigned long ulLen = 0;

	// PersoData
	pteidngPersoDataBuffer = m_data.GetBytes(PTEIDNG_FIELD_PERSODATA_POS_FILE, PTEIDNG_FIELD_PERSODATA_LEN_FILE);
	pteidngPersoDataBuffer.TrimRight(' ');
	m_PersoData.assign((char *)(pteidngPersoDataBuffer.GetBytes()), pteidngPersoDataBuffer.Size());

	m_mappedFields = true;

	return true;
}

const char *APL_EidFile_PersoData::getPersoData(bool forceMap) {

	if (forceMap)
		m_mappedFields = !forceMap;
	if (ShowData()) {
		return m_PersoData.c_str();
	}

	return "";
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_TokenInfo -----------------------------------------
*****************************************************************************************/
APL_EidFile_TokenInfo::APL_EidFile_TokenInfo(APL_EIDCard *card) : APL_CardFile(card, PTEID_FILE_TOKENINFO, NULL) {}

APL_EidFile_TokenInfo::~APL_EidFile_TokenInfo() {}

tCardFileStatus APL_EidFile_TokenInfo::VerifyFile() { return CARDFILESTATUS_OK; }

void APL_EidFile_TokenInfo::EmptyFields() {
	m_GraphicalPersonalisation.clear();
	m_ElectricalPersonalisation.clear();
	m_ElectricalPersonalisationInterface.clear();
	m_mappedFields = false;
}

bool APL_EidFile_TokenInfo::MapFields() {
	if (m_mappedFields)
		return true;

	char buffer[50];

	// Graphical Personalisation
	sprintf_s(buffer, sizeof(buffer), "%02X", m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_GraphicalPersonalisation));
	m_GraphicalPersonalisation = buffer;

	// Electrical Personalisation
	sprintf_s(buffer, sizeof(buffer), "%02X", m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisation));
	m_ElectricalPersonalisation = buffer;

	// Electrical Personalisation Interface
	sprintf_s(buffer, sizeof(buffer), "%02X",
			  m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisationInterface));
	m_ElectricalPersonalisationInterface = buffer;

	m_mappedFields = true;

	return true;
}

const char *APL_EidFile_TokenInfo::getGraphicalPersonalisation() {
	if (ShowData())
		return m_GraphicalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisation() {
	if (ShowData())
		return m_ElectricalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisationInterface() {
	if (ShowData())
		return m_ElectricalPersonalisationInterface.c_str();

	return "";
}

} // namespace eIDMW
