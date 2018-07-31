/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#include "TLVBuffer.h"
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

//C++ 11 header used for date parsing
#include <regex>
#include <ctime>

#include <openssl/err.h>
#include <iostream>
#include <fstream>



using namespace std;

std::string m_SerialNumber;

namespace eIDMW
{
/*****************************************************************************************
---------------------------------------- APL_EidFile_TRACE -----------------------------------------
*****************************************************************************************/
APL_EidFile_Trace::APL_EidFile_Trace(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_TRACE,NULL)
{
}

APL_EidFile_Trace::~APL_EidFile_Trace()
{
}

tCardFileStatus APL_EidFile_Trace::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	return CARDFILESTATUS_OK;
}

void APL_EidFile_Trace::EmptyFields()
{
	m_Validation.clear();
	m_mappedFields = false;
}

bool APL_EidFile_Trace::MapFields()
{
	if (m_mappedFields)
		return true;

	CByteArray pteidngtraceBuffer;
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());
    CByteArray Validation;

	//Card Validation
    pteidngtraceBuffer = m_data.GetBytes(PTEIDNG_FIELD_TRACE_POS_VALIDATION, PTEIDNG_FIELD_TRACE_LEN_VALIDATION);

    Validation = pteidngtraceBuffer;
    if (Validation.ToString() == "01")
    	m_Validation = "O Cartão de Cidadão encontra-se activo";
    else
    	m_Validation = "O Cartão de Cidadão encontra-se inactivo";


    isCardActive = (pteidngtraceBuffer.GetByte(0) == PTEIDNG_ACTIVE_CARD);

    m_mappedFields = true;

    return true;
}

bool APL_EidFile_Trace::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status=getStatus(false);
	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

const char *APL_EidFile_Trace::getValidation()
{
	if(ShowData())
		return m_Validation.c_str();

	return "";
}

bool APL_EidFile_Trace::isActive(){
	if(ShowData())
		return isCardActive;
	return false;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_ID -----------------------------------------
*****************************************************************************************/
APL_EidFile_ID::APL_EidFile_ID(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_ID,NULL)
{
	m_photo = NULL;
}

APL_EidFile_ID::~APL_EidFile_ID()
{
}


void APL_EidFile_ID::PackIdData(CByteArray &cb) {
	cb.Append((unsigned char*)m_IssuingEntity.c_str(),m_IssuingEntity.length());
	cb.Append((unsigned char*)m_Country.c_str(),m_Country.length());
	cb.Append((unsigned char*)m_DocumentType.c_str(),m_DocumentType.length());
	cb.Append((unsigned char*)m_DocumentNumber.c_str(),m_DocumentNumber.length());
	cb.Append((unsigned char*)m_ChipNumber.c_str(),m_ChipNumber.length());
	cb.Append((unsigned char*)m_DocumentVersion.c_str(),m_DocumentVersion.length());
	cb.Append((unsigned char*)m_ValidityBeginDate.c_str(),m_ValidityBeginDate.length());
	cb.Append((unsigned char*)m_LocalofRequest.c_str(),m_LocalofRequest.length());
	cb.Append((unsigned char*)m_ValidityEndDate.c_str(),m_ValidityEndDate.length());
	cb.Append((unsigned char*)m_Surname.c_str(),m_Surname.length());
	cb.Append((unsigned char*)m_GivenName.c_str(),m_GivenName.length());
	cb.Append((unsigned char*)m_Gender.c_str(),m_Gender.length());
	cb.Append((unsigned char*)m_Nationality.c_str(),m_Nationality.length());
	cb.Append((unsigned char*)m_DateOfBirth.c_str(),m_DateOfBirth.length());
	cb.Append((unsigned char*)m_Height.c_str(),m_Height.length());
	cb.Append((unsigned char*)m_CivilianIdNumber.c_str(),m_CivilianIdNumber.length());
	cb.Append((unsigned char*)m_SurnameMother.c_str(),m_SurnameMother.length());
	cb.Append((unsigned char*)m_GivenNameMother.c_str(),m_GivenNameMother.length());
	cb.Append((unsigned char*)m_SurnameFather.c_str(),m_SurnameFather.length());
	cb.Append((unsigned char*)m_GivenNameFather.c_str(),m_GivenNameFather.length());
	cb.Append((unsigned char*)m_AccidentalIndications.c_str(),m_AccidentalIndications.length());
	cb.Append((unsigned char*)m_TaxNo.c_str(),m_TaxNo.length());
	cb.Append((unsigned char*)m_SocialSecurityNo.c_str(),m_SocialSecurityNo.length());
	cb.Append((unsigned char*)m_HealthNo.c_str(),m_HealthNo.length());
}

void APL_EidFile_ID::PackPublicKeyData(CByteArray &cb){
	cb.Append(*cardKey->getModulus());
	cb.Append(*cardKey->getExponent());
}


void APL_EidFile_ID::PackPictureData(CByteArray &cb){
	cb.Append(*m_photo->getCbeff());
	cb.Append(*m_photo->getFacialrechdr());
	cb.Append(*m_photo->getFacialinfo());
	cb.Append(*m_photo->getImageinfo());
	cb.Append(*m_photo->getPhotoRaw());
}


tCardFileStatus APL_EidFile_ID::VerifyFile()
{
	if(!m_card)
			return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	MapFieldsInternal();

	if (m_SODCheck) {
		CByteArray pkData;
		CByteArray idData;
		CByteArray picData;
		PackPublicKeyData(pkData);
		PackIdData(idData);
		PackPictureData(picData);

		if (!m_cryptoFwk->VerifyHashSha256(pkData,pcard->getFileSod()->getCardPublicKeyHash()))
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY);

		if (!m_cryptoFwk->VerifyHashSha256(idData,pcard->getFileSod()->getIdHash()))
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ID);

		if (!m_cryptoFwk->VerifyHashSha256(picData,pcard->getFileSod()->getPictureHash()))
			throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE);
	}
	m_isVerified = true;

	return CARDFILESTATUS_OK;
}

void APL_EidFile_ID::EmptyFields()
{
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
	if (cardKey){
		delete cardKey;
		cardKey = NULL;
	}
	m_mappedFields = false;
	m_isVerified = false;
	m_SODCheck = false;
}

void APL_EidFile_ID::MapFieldsInternal(){
	CByteArray pteidngidBuffer;

	// we dont want to read the fields every time
	if (m_mappedFields)
		return;

	//IDVersion - Card Version
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentVersion, PTEIDNG_FIELD_ID_LEN_DocumentVersion);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentVersion.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ChipNr
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumberPAN, PTEIDNG_FIELD_ID_LEN_DocumentNumberPAN);
	pteidngidBuffer.TrimRight('\0');
	m_ChipNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());
	m_SerialNumber = m_ChipNumber;

	//Country
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Country,PTEIDNG_FIELD_ID_LEN_Country);
	pteidngidBuffer.TrimRight('\0');
	m_Country.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ValidityBeginDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityBeginDate,PTEIDNG_FIELD_ID_LEN_ValidityBeginDate);
	pteidngidBuffer.TrimRight('\0');
	m_ValidityBeginDate.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ValidityEndDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityEndDate,PTEIDNG_FIELD_ID_LEN_ValidityEndDate);
	pteidngidBuffer.TrimRight('\0');
	m_ValidityEndDate.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//IssuingMunicipality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_LocalofRequest, PTEIDNG_FIELD_ID_LEN_LocalofRequest);
	pteidngidBuffer.TrimRight('\0');
	m_LocalofRequest.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Civilian Identification Number (NIC)
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_CivilianIdNumber, PTEIDNG_FIELD_ID_LEN_CivilianIdNumber);
	pteidngidBuffer.TrimRight('\0');
	m_CivilianIdNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Surname
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Surname, PTEIDNG_FIELD_ID_LEN_Surname);
	pteidngidBuffer.TrimRight('\0');
	m_Surname.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//FirstName_1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Name, PTEIDNG_FIELD_ID_LEN_Name);
	pteidngidBuffer.TrimRight('\0');
	m_GivenName.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Nationality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Nacionality, PTEIDNG_FIELD_ID_LEN_Nacionality);
	pteidngidBuffer.TrimRight('\0');
	m_Nationality.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//DateOfBirth
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DateOfBirth, PTEIDNG_FIELD_ID_LEN_DateOfBirth);
	pteidngidBuffer.TrimRight('\0');
	m_DateOfBirth.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Gender
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Gender, PTEIDNG_FIELD_ID_LEN_Gender);
	pteidngidBuffer.TrimRight('\0');
	m_Gender.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//DocumentType
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentType, PTEIDNG_FIELD_ID_LEN_DocumentType);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentType.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Height
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Height, PTEIDNG_FIELD_ID_LEN_Height);
	pteidngidBuffer.TrimRight('\0');
	m_Height.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//DocumentNumber
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumber, PTEIDNG_FIELD_ID_LEN_DocumentNumber);
	pteidngidBuffer.TrimRight('\0');
	m_DocumentNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//TaxNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_TaxNo, PTEIDNG_FIELD_ID_LEN_TaxNo);
	pteidngidBuffer.TrimRight('\0');
	m_TaxNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//SocialSecurityNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SocialSecurityNo, PTEIDNG_FIELD_ID_LEN_SocialSecurityNo);
	pteidngidBuffer.TrimRight('\0');
	m_SocialSecurityNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//HealthNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_HealthNo, PTEIDNG_FIELD_ID_LEN_HealthNo);
	pteidngidBuffer.TrimRight('\0');
	m_HealthNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//IssuingEntity
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_IssuingEntity, PTEIDNG_FIELD_ID_LEN_IssuingEntity);
	pteidngidBuffer.TrimRight('\0');
	m_IssuingEntity.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//GivenNameFather
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_GivenNameFather, PTEIDNG_FIELD_ID_LEN_GivenNameFather);
	pteidngidBuffer.TrimRight('\0');
	m_GivenNameFather.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//SurnameFather
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SurnameFather, PTEIDNG_FIELD_ID_LEN_SurnameFather);
	pteidngidBuffer.TrimRight('\0');
	m_SurnameFather.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//GivenNameMother
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_GivenNameMother, PTEIDNG_FIELD_ID_LEN_GivenNameMother);
	pteidngidBuffer.TrimRight('\0');
	m_GivenNameMother.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//SurnameMother
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SurnameMother, PTEIDNG_FIELD_ID_LEN_SurnameMother);
	pteidngidBuffer.TrimRight('\0');
	m_SurnameMother.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Photo
	{
		CByteArray cbeff;
		CByteArray facialrechdr;
		CByteArray facialinfo;
		CByteArray imageinfo;
		CByteArray photoRAW;

		photoRAW = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Photo, PTEIDNG_FIELD_ID_LEN_Photo);
		photoRAW.TrimRight(0);
		cbeff = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_CBEFF, PTEIDNG_FIELD_ID_LEN_CBEFF);
		facialrechdr = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FACIALRECHDR, PTEIDNG_FIELD_ID_LEN_FACIALRECHDR);
		facialinfo = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FACIALINFO, PTEIDNG_FIELD_ID_LEN_FACIALINFO);
		imageinfo = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_IMAGEINFO, PTEIDNG_FIELD_ID_LEN_IMAGEINFO);
		m_photo = new PhotoPteid(photoRAW, cbeff, facialrechdr, facialinfo, imageinfo);
	}

	//Card Authentication Key (modulus + exponent)
	{
		CByteArray modulus = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_MODULUS, PTEIDNG_FIELD_ID_LEN_MODULUS);
		CByteArray exponent = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_EXPONENT, PTEIDNG_FIELD_ID_LEN_EXPONENT);

		cardKey = new APLPublicKey(modulus,exponent);
	}
	//MRZ1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz1, PTEIDNG_FIELD_ID_LEN_Mrz1);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ1.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//MRZ2
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz2, PTEIDNG_FIELD_ID_LEN_Mrz2);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ2.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//MRZ3
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz3, PTEIDNG_FIELD_ID_LEN_Mrz3);
	pteidngidBuffer.TrimRight('\0');
	m_MRZ3.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//AccidentalIndications
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_AccidentalIndications, PTEIDNG_FIELD_ID_LEN_AccidentalIndications);
	pteidngidBuffer.TrimRight('\0');
	m_AccidentalIndications.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	m_mappedFields = true;
}

bool APL_EidFile_ID::MapFields()
{
	// do nothing verify will load the data and then verify if sod check is active
	return true;
}

bool APL_EidFile_ID::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status=getStatus(false);
	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

const char *APL_EidFile_ID::getDocumentVersion()
{
	if(ShowData())
		return m_DocumentVersion.c_str();

	return "";
}

const char *APL_EidFile_ID::getCountry()
{
	if(ShowData())
		return m_Country.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentType()
{
	if(ShowData())
		return m_DocumentType.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenName()
{
	if(ShowData())
		return m_GivenName.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurname()
{
	if(ShowData())
		return m_Surname.c_str();

	return "";
}

const char *APL_EidFile_ID::getGender()
{
	if(ShowData())
		return m_Gender.c_str();

	return "";
}

const char *APL_EidFile_ID::getDateOfBirth()
{
	if(ShowData())
		return m_DateOfBirth.c_str();

	return "";
}

const char *APL_EidFile_ID::getNationality()
{
	if(ShowData())
		return m_Nationality.c_str();

	return "";
}


const char *APL_EidFile_ID::getDocumentPAN()
{
	if(ShowData())
		return m_ChipNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityBeginDate()
{
	if(ShowData())
		return m_ValidityBeginDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getValidityEndDate()
{
	if(ShowData())
		return m_ValidityEndDate.c_str();

	return "";
}

const char *APL_EidFile_ID::getHeight()
{
	if(ShowData())
		return m_Height.c_str();

	return "";
}

const char *APL_EidFile_ID::getDocumentNumber()
{
	if(ShowData())
		return m_DocumentNumber.c_str();

	return "";
}

const char *APL_EidFile_ID::getTaxNo()
{
	if(ShowData())
		return m_TaxNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getSocialSecurityNumber()
{
	if(ShowData())
		return m_SocialSecurityNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getHealthNumber()
{
	if(ShowData())
		return m_HealthNo.c_str();

	return "";
}

const char *APL_EidFile_ID::getIssuingEntity()
{
	if(ShowData())
		return m_IssuingEntity.c_str();

	return "";
}

const char *APL_EidFile_ID::getLocalofRequest()
{
	if(ShowData())
		return m_LocalofRequest.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenNameFather()
{
	if(ShowData())
		return m_GivenNameFather.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurnameFather()
{
	if(ShowData())
		return m_SurnameFather.c_str();

	return "";
}

const char *APL_EidFile_ID::getGivenNameMother()
{
	if(ShowData())
		return m_GivenNameMother.c_str();

	return "";
}

const char *APL_EidFile_ID::getSurnameMother()
{
	if(ShowData())
		return m_SurnameMother.c_str();

	return "";
}

const char *APL_EidFile_ID::getParents()
{

	string m_Parents = m_GivenNameFather + " " + m_SurnameFather + " * " + m_GivenNameMother + " " + m_SurnameMother;

	if(ShowData())
		return _strdup(m_Parents.c_str());

	return "";
}

PhotoPteid *APL_EidFile_ID::getPhotoObj()
{

	if(ShowData())
		return m_photo;

	return NULL;
}

APLPublicKey *APL_EidFile_ID::getCardAuthKeyObj(){
	if(ShowData())
		return cardKey;

	return NULL;
}

const char *APL_EidFile_ID::getMRZ1()
{
	if(ShowData())
		return m_MRZ1.c_str();

	return "";
}

const char *APL_EidFile_ID::getMRZ2()
{
	if(ShowData())
		return m_MRZ2.c_str();

	return "";
}

const char *APL_EidFile_ID::getMRZ3()
{
	if(ShowData())
		return m_MRZ3.c_str();

	return "";
}

const char *APL_EidFile_ID::getAccidentalIndications()
{
	if(ShowData())
		return m_AccidentalIndications.c_str();

	return "";
}

const char *APL_EidFile_ID::getCivilianIdNumber()
{
	if(ShowData())
		return m_CivilianIdNumber.c_str();

	return "";
}

const CByteArray& APL_EidFile_ID::getPhotoHash()
{
	if(ShowData())
		return m_PhotoHash;

	return EmptyByteArray;
}

void  APL_EidFile_ID::doSODCheck(bool check){
	m_SODCheck = check;
	if (check){
		m_isVerified = false;
		m_mappedFields = false;
	}
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Address -----------------------------------------
*****************************************************************************************/
const string APL_EidFile_Address::m_NATIONAL = "N";
const string APL_EidFile_Address::m_FOREIGN = "I";
APL_EidFile_Address::APL_EidFile_Address(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_ADDRESS,NULL)
{
}

APL_EidFile_Address::~APL_EidFile_Address()
{
}


void APL_EidFile_Address::PackAddressData(CByteArray &cb, bool isNational){

	if (isNational){
		cb.Append((unsigned char*)m_CountryCode.c_str(),m_CountryCode.length());
		cb.Append((unsigned char*)m_DistrictCode.c_str(),m_DistrictCode.length());
		cb.Append((unsigned char*)m_DistrictDescription.c_str(),m_DistrictDescription.length());
		cb.Append((unsigned char*)m_MunicipalityCode.c_str(),m_MunicipalityCode.length());
		cb.Append((unsigned char*)m_MunicipalityDescription.c_str(),m_MunicipalityDescription.length());
		cb.Append((unsigned char*)m_CivilParishCode.c_str(),m_CivilParishCode.length());
		cb.Append((unsigned char*)m_CivilParishDescription.c_str(),m_CivilParishDescription.length());
		cb.Append((unsigned char*)m_AbbrStreetType.c_str(),m_AbbrStreetType.length());
		cb.Append((unsigned char*)m_StreetType.c_str(),m_StreetType.length());
		cb.Append((unsigned char*)m_StreetName.c_str(),m_StreetName.length());
		cb.Append((unsigned char*)m_AbbrBuildingType.c_str(),m_AbbrBuildingType.length());
		cb.Append((unsigned char*)m_BuildingType.c_str(),m_BuildingType.length());
		cb.Append((unsigned char*)m_DoorNo.c_str(),m_DoorNo.length());
		cb.Append((unsigned char*)m_Floor.c_str(),m_Floor.length());
		cb.Append((unsigned char*)m_Side.c_str(),m_Side.length());
		cb.Append((unsigned char*)m_Place.c_str(),m_Place.length());
		cb.Append((unsigned char*)m_Locality.c_str(),m_Locality.length());
		cb.Append((unsigned char*)m_Zip4.c_str(),m_Zip4.length());
		cb.Append((unsigned char*)m_Zip3.c_str(),m_Zip3.length());
		cb.Append((unsigned char*)m_PostalLocality.c_str(),m_PostalLocality.length());
		cb.Append((unsigned char*)m_Generated_Address_Code.c_str(),m_Generated_Address_Code.length());
	} else {
		cb.Append((unsigned char*)m_CountryCode.c_str(), m_CountryCode.length());
		cb.Append((unsigned char*)m_Foreign_Country.c_str(),m_Foreign_Country.length());
		cb.Append((unsigned char*)m_Foreign_Generic_Address.c_str(),m_Foreign_Generic_Address.length());
		cb.Append((unsigned char*)m_Foreign_City.c_str(),m_Foreign_City.length());
		cb.Append((unsigned char*)m_Foreign_Region.c_str(),m_Foreign_Region.length());
		cb.Append((unsigned char*)m_Foreign_Locality.c_str(),m_Foreign_Locality.length());
		cb.Append((unsigned char*)m_Foreign_Postal_Code.c_str(),m_Foreign_Postal_Code.length());
		cb.Append((unsigned char*)m_Generated_Address_Code.c_str(),m_Generated_Address_Code.length());
	}
}


void APL_EidFile_Address::MapFieldsInternal()
{

	if (m_mappedFields) // have we mapped the fields yet?
		return;

	CByteArray pteidngAddressBuffer;

	// Address Type
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_TYPE);
	pteidngAddressBuffer.TrimRight('\0');
	m_AddressType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Country code
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_COUNTRY, PTEIDNG_FIELD_ADDRESS_LEN_COUNTRY);
	pteidngAddressBuffer.TrimRight('\0');
	m_CountryCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	if (m_AddressType == m_FOREIGN)
		ForeignerAddressFields();
	else
		AddressFields();

	// So we've mapped the fields no need to map them again
	m_mappedFields = true;
}


//Check for issuing date in "buggy" period of card production
bool addressSODShouldBeChecked(const char * issuing_date) 
{

	// create tm with 01/01/2017:
	std::tm timeinfo = std::tm();
	timeinfo.tm_year = 2017 - 1900;
	timeinfo.tm_mon = 0;          // month: january
	timeinfo.tm_mday = 1;        // day: 1st
	std::time_t beginBuggyPeriod = std::mktime (&timeinfo);
	
	timeinfo.tm_year = 2018 - 1900;
	timeinfo.tm_mon = 1;          // month: february
	timeinfo.tm_mday = 20;        // day: 20th

	std::time_t endBuggyPeriod = std::mktime (&timeinfo);

	std::cmatch cm;
	std::regex expression("(\\d{2}) (\\d{2}) (\\d{4})"); 

    std::regex_match (issuing_date, cm, expression);

    int day = 0, month = 0, year = 0;

    for (unsigned i=0; i < cm.size(); i++) {
    	switch (i)
    	{
    		//case 0 is ignored as it matches the full regex pattern
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
	timeinfo.tm_mon = month - 1;      // month: february
	timeinfo.tm_mday = day;           // day: 20th

	std::time_t issuingTime = std::mktime (&timeinfo);

    return issuingTime > endBuggyPeriod || issuingTime < beginBuggyPeriod;
}

tCardFileStatus APL_EidFile_Address::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	if (m_isVerified)
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	MapFieldsInternal();

	if (m_SODCheck) {
		
		APL_EidFile_ID * idFile = pcard->getFileID();

		const char * issuing_date = idFile->getValidityBeginDate();

		if (!addressSODShouldBeChecked(issuing_date)) {
			MWLOG(LEV_DEBUG, MOD_APL, 
				"Skipping Address SOD check because this card was issued between 01/01/2017 and 20/02/2018");
		}
		else {
			CByteArray addrData;
			PackAddressData(addrData, m_AddressType != m_FOREIGN);

			if (!m_cryptoFwk->VerifyHashSha256(addrData,pcard->getFileSod()->getAddressHash()))
				throw CMWEXCEPTION(EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS);
		}
	}

	m_isVerified = true;
	return CARDFILESTATUS_OK;
}

bool APL_EidFile_Address::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status=getStatus(false);
	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_Address::EmptyFields()
{
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

void APL_EidFile_Address::AddressFields()
{
		CByteArray pteidngAddressBuffer;

		//District Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_DistrictCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //District Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_DistrictDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Municipality Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY, PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_MunicipalityCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Municipality Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_MunicipalityDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //CivilParish Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH, PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_CivilParishCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //CivilParish Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_CivilParishDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Abbreviated Street Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_STREET_TYPE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_AbbrStreetType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Street Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_STREET_TYPE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_StreetType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Street Name
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREETNAME, PTEIDNG_FIELD_ADDRESS_LEN_STREETNAME);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_StreetName.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Abbreviated Building Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_BUILDING_TYPE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_AbbrBuildingType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Building Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_BUILDING_TYPE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_BuildingType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //DoorNo
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DOORNO, PTEIDNG_FIELD_ADDRESS_LEN_DOORNO);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_DoorNo.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Floor
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_FLOOR, PTEIDNG_FIELD_ADDRESS_LEN_FLOOR);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Floor.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Side
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_SIDE, PTEIDNG_FIELD_ADDRESS_LEN_SIDE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Side.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Place
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_PLACE, PTEIDNG_FIELD_ADDRESS_LEN_PLACE);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Place.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Locality
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_LOCALITY);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Locality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Zip4
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP4, PTEIDNG_FIELD_ADDRESS_LEN_ZIP4);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Zip4.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Zip3
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP3, PTEIDNG_FIELD_ADDRESS_LEN_ZIP3);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Zip3.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Postal Locality
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_POSTALLOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_POSTALLOCALITY);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_PostalLocality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Generated Address Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_GENADDRESS_NUM, PTEIDNG_FIELD_ADDRESS_LEN_GENADDRESS_NUM);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Generated_Address_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

}

void APL_EidFile_Address::ForeignerAddressFields()
{
	CByteArray pteidngAddressBuffer;

	//Foreign Country
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_COUNTRY_DESCRIPTION, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_COUNTRY_DESCRIPTION);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Country.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	//Foreign Generic Address
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_ADDRESS, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_ADDRESS);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_Generic_Address.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	//Foreign City
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_CITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_CITY);
	pteidngAddressBuffer.TrimRight('\0');
	m_Foreign_City.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Region
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_REGION, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_REGION);
    pteidngAddressBuffer.TrimRight('\0');
    m_Foreign_Region.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Locality
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_LOCALITY);
    pteidngAddressBuffer.TrimRight('\0');
    m_Foreign_Locality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Postal Code
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_POSTAL_CODE, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_POSTAL_CODE);
    pteidngAddressBuffer.TrimRight('\0');
    m_Foreign_Postal_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Generated Address Code
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_GENADDRESS_NUM, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_GENADDRESS_NUM);
    pteidngAddressBuffer.TrimRight('\0');
    m_Generated_Address_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

}

bool APL_EidFile_Address::MapFields()
{
	// do nothing verify will load the data and then verify if sod check is active
	return true;
}

const char *APL_EidFile_Address::getMunicipality()
{
	if(ShowData())
		return m_MunicipalityDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getMunicipalityCode()
{
	if(ShowData())
		return m_MunicipalityCode.c_str();

	return "";
}


const char *APL_EidFile_Address::getDistrict()
{
	if(ShowData())
		return m_DistrictDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getDistrictCode()
{
	if(ShowData())
		return m_DistrictCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getCivilParish()
{
	if(ShowData())
		return m_CivilParishDescription.c_str();

	return "";
}

const char *APL_EidFile_Address::getCivilParishCode()
{
	if(ShowData())
		return m_CivilParishCode.c_str();

	return "";
}

const char *APL_EidFile_Address::getStreetName()
{
	if(ShowData())
		return m_StreetName.c_str();

	return "";
}

const char *APL_EidFile_Address::getAbbrStreetType()
{
	if(ShowData())
		return m_AbbrStreetType.c_str();

	return "";
}

const char *APL_EidFile_Address::getStreetType()
{
	if(ShowData())
		return m_StreetType.c_str();

	return "";
}

const char *APL_EidFile_Address::getAbbrBuildingType()
{
	if(ShowData())
		return m_AbbrBuildingType.c_str();

	return "";
}

const char *APL_EidFile_Address::getBuildingType()
{
	if(ShowData())
		return m_BuildingType.c_str();

	return "";
}

const char *APL_EidFile_Address::getDoorNo()
{
	if(ShowData())
		return m_DoorNo.c_str();

	return "";
}

const char *APL_EidFile_Address::getFloor()
{
	if(ShowData())
		return m_Floor.c_str();

	return "";
}

const char *APL_EidFile_Address::getSide()
{
	if(ShowData())
		return m_Side.c_str();

	return "";
}

const char *APL_EidFile_Address::getLocality()
{
	if(ShowData())
		return m_Locality.c_str();

	return "";
}

const char *APL_EidFile_Address::getZip4()
{
	if(ShowData())
		return m_Zip4.c_str();

	return "";
}

const char *APL_EidFile_Address::getZip3()
{
	if(ShowData())
		return m_Zip3.c_str();

	return "";
}

const char *APL_EidFile_Address::getPlace()
{
	if(ShowData())
		return m_Place.c_str();

	return "";
}

const char *APL_EidFile_Address::getPostalLocality()
{
	if(ShowData())
		return m_PostalLocality.c_str();

	return "";
}

const char *APL_EidFile_Address::getGeneratedAddressCode()
{
	if(ShowData())
		return m_Generated_Address_Code.c_str();

	return "";
}

const char * APL_EidFile_Address::getCountryCode()
{
	if(ShowData())
			return m_CountryCode.c_str();

		return "";
}

bool APL_EidFile_Address::isNationalAddress(){
	ShowData();
	return (m_AddressType.compare(m_NATIONAL)==0);
}

const char *APL_EidFile_Address::getForeignCountry()
{
	if(ShowData())
		return m_Foreign_Country.c_str();

	return "";
}

const char * APL_EidFile_Address::getForeignAddress()
{
	if(ShowData())
			return m_Foreign_Generic_Address.c_str();

		return "";
}

const char *APL_EidFile_Address::getForeignCity()
{
	if(ShowData())
		return m_Foreign_City.c_str();

	return "";
}

const char * APL_EidFile_Address::getForeignRegion()
{
	if(ShowData())
			return m_Foreign_Region.c_str();

		return "";
}

const char *APL_EidFile_Address::getForeignLocality()
{
	if(ShowData())
		return m_Foreign_Locality.c_str();

	return "";
}

const char * APL_EidFile_Address::getForeignPostalCode()
{
	if(ShowData())
			return m_Foreign_Postal_Code.c_str();

		return "";
}

void APL_EidFile_Address::doSODCheck(bool check){
	m_SODCheck = check;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Sod -----------------------------------------
*****************************************************************************************/
APL_EidFile_Sod::APL_EidFile_Sod(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_SOD,NULL)
{

}

APL_EidFile_Sod::~APL_EidFile_Sod()
{
}

tCardFileStatus APL_EidFile_Sod::VerifyFile()
{
	tCardFileStatus filestatus = CARDFILESTATUS_ERROR_SIGNATURE;

	if (m_isVerified) // no need to check again
		return CARDFILESTATUS_OK;


	if (!m_SODCheck) // check is not activated
		return CARDFILESTATUS_OK;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	PKCS7 *p7 = NULL;
	bool verifySOD = false;
	
	ERR_load_crypto_strings();
	OpenSSL_add_all_digests();

	const unsigned char *temp = m_data.GetBytes();
	long int len = m_data.Size();
	temp+=4; //jump the response message template format 2" (DER type 77)

	p7 = d2i_PKCS7(NULL, (const unsigned char **)&temp, len);

	//STACK_OF(X509) *pSigners = PKCS7_get0_signers(p7, NULL, 0);

	X509_STORE *store = X509_STORE_new();

	// Load only the SOD relevant root certificates

	// load all certificates, let openssl do the job and find the needed ones...
	for (unsigned long i = 0; i < pcard->getCertificates()->countSODCAs(); i++){
		APL_Certif * sod_ca = pcard->getCertificates()->getSODCA(i);
		X509 *pX509 = NULL;
		const unsigned char *p = sod_ca->getData().GetBytes();

		pX509 = d2i_X509(&pX509, &p, sod_ca->getData().Size());
		X509_STORE_add_cert(store, pX509);
		MWLOG(LEV_DEBUG, MOD_APL, "%d. Adding certificate Subject CN: %s", i, sod_ca->getOwnerName());
	}
	
	BIO *Out = BIO_new(BIO_s_mem());

	//verifySOD = PKCS7_verify(p7,pSigners,store,NULL,Out,0)==1;
	verifySOD = PKCS7_verify(p7, NULL,store,NULL,Out,0)==1;
	if (verifySOD) {
		unsigned char *p;
		long size;
		size = BIO_get_mem_data(Out, &p);
		m_encapsulatedContent.Append(p,size);

		m_isVerified = true;
		filestatus = CARDFILESTATUS_OK;
	}
	else
	{
		char * validation_error = ERR_error_string(ERR_get_error(), NULL);
		//Log specific OpenSSL error
		MWLOG(LEV_ERROR, MOD_APL, "EidFile_Sod:: Error validating SOD signature. OpenSSL error: %s", validation_error);
			
	}

	X509_STORE_free(store);
	//sk_X509_free(pSigners);
	BIO_free_all(Out);
	PKCS7_free(p7);

	if (!verifySOD)
		throw CMWEXCEPTION(EIDMW_SOD_ERR_VERIFY_SOD_SIGN);

	return filestatus;
}

bool APL_EidFile_Sod::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	tCardFileStatus status=getStatus(false);
	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

void APL_EidFile_Sod::EmptyFields()
{
	m_mappedFields = false;
	m_isVerified = false;
	m_picHash.ClearContents();
	m_pkHash.ClearContents();
	m_idHash.ClearContents();
	m_addressHash.ClearContents();
	m_encapsulatedContent.ClearContents();
}

void APL_EidFile_Sod::doSODCheck(bool check){
	m_SODCheck = check;
	if (check) {
		m_isVerified = false;
		m_mappedFields = false;
	}
}

bool APL_EidFile_Sod::MapFields()
{
	if (m_mappedFields) // map only one time
		return true;

	if (!m_SODCheck) // map only if sod check is active
		return true;

	SODParser parser;

	parser.ParseSodEncapsulatedContent(m_encapsulatedContent);

	SODAttributes &attr = parser.getHashes();

	m_idHash.Append(attr.hashes[0]);
	m_addressHash.Append(attr.hashes[1]);
	m_picHash.Append(attr.hashes[2]);
	m_pkHash.Append(attr.hashes[3]);

	m_mappedFields = true;

	return true;
}

const CByteArray& APL_EidFile_Sod::getAddressHash(){
	if(ShowData())
		return m_addressHash;
	return EmptyByteArray;
}

const CByteArray& APL_EidFile_Sod::getPictureHash(){
	if(ShowData())
		return m_picHash;
	return EmptyByteArray;
}

const CByteArray& APL_EidFile_Sod::getCardPublicKeyHash(){
	if(ShowData())
		return m_pkHash;
	return EmptyByteArray;
}

const CByteArray& APL_EidFile_Sod::getIdHash(){
	if(ShowData())
		return m_idHash;
	return EmptyByteArray;
}


/*****************************************************************************************
---------------------------------------- APL_EidFile_PersoData -----------------------------------------
*****************************************************************************************/
APL_EidFile_PersoData::APL_EidFile_PersoData(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_PERSODATA,NULL)
{
}

APL_EidFile_PersoData::~APL_EidFile_PersoData()
{
}

tCardFileStatus APL_EidFile_PersoData::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_PersoData::ShowData()
{
        APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

        tCardFileStatus status=getStatus(false);
        if(status==CARDFILESTATUS_OK)
                return true;

        return false;
}

void APL_EidFile_PersoData::EmptyFields()
{
	m_PersoData.clear();
	m_mappedFields = false;
}

bool APL_EidFile_PersoData::MapFields()
{
	if (m_mappedFields)
		return true;

	CByteArray pteidngPersoDataBuffer;
    unsigned long ulLen=0;

    CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

    //PersoData
    pteidngPersoDataBuffer = m_data.GetBytes(PTEIDNG_FIELD_PERSODATA_POS_FILE, PTEIDNG_FIELD_PERSODATA_LEN_FILE);
    pteidngPersoDataBuffer.TrimRight(' ');
    m_PersoData.assign((char*)(pteidngPersoDataBuffer.GetBytes()), pteidngPersoDataBuffer.Size());

    m_mappedFields = true;

    return true;
}

const char *APL_EidFile_PersoData::getPersoData(bool forceMap)
{

	if (forceMap)
		m_mappedFields = !forceMap;
	if(ShowData()){
		return m_PersoData.c_str();
	}

	return "";
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_TokenInfo -----------------------------------------
*****************************************************************************************/
APL_EidFile_TokenInfo::APL_EidFile_TokenInfo(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_TOKENINFO,NULL)
{
}

APL_EidFile_TokenInfo::~APL_EidFile_TokenInfo()
{
}

tCardFileStatus APL_EidFile_TokenInfo::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

void APL_EidFile_TokenInfo::EmptyFields()
{
	m_GraphicalPersonalisation.clear();
	m_ElectricalPersonalisation.clear();
	m_ElectricalPersonalisationInterface.clear();
	m_mappedFields = false;
}

bool APL_EidFile_TokenInfo::MapFields()
{
	if (m_mappedFields)
		return true;

	char buffer[50];

	//Graphical Personalisation
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_GraphicalPersonalisation));
	m_GraphicalPersonalisation=buffer;

	//Electrical Personalisation
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisation));
	m_ElectricalPersonalisation=buffer;

	//Electrical Personalisation Interface
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(PTEID_FIELD_BYTE_TOKENINFO_ElectricalPersonalisationInterface));
	m_ElectricalPersonalisationInterface=buffer;

	m_mappedFields = true;

	return true;

}

const char *APL_EidFile_TokenInfo::getGraphicalPersonalisation()
{
	if(ShowData())
		return m_GraphicalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisation()
{
	if(ShowData())
		return m_ElectricalPersonalisation.c_str();

	return "";
}

const char *APL_EidFile_TokenInfo::getElectricalPersonalisationInterface()
{
	if(ShowData())
		return m_ElectricalPersonalisationInterface.c_str();

	return "";
}

}
