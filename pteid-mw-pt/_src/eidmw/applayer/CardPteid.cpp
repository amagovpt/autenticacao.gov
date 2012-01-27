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
#include <iostream>
#include <fstream>

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
#include "APLPublicKey.h"

#include "Log.h"

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

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus filestatus;

	//Remove since it's rrn related
	/*if(!CertRrn)
		return CARDFILESTATUS_ERROR_RRN;

	//If the status of the RRN file is not OK,
	//The rrn status is return
	filestatus=CertRrn->getFileStatus();
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	//Get the status of the certificate RRN
	APL_CertifStatus rrnstatus=CertRrn->getStatus();
	if((rrnstatus==APL_CERTIF_STATUS_TEST || rrnstatus==APL_CERTIF_STATUS_ISSUER) && !pcard->getAllowTestCard())
		return CARDFILESTATUS_ERROR_TEST;

	if(rrnstatus==APL_CERTIF_STATUS_DATE && !pcard->getAllowBadDate())
		return CARDFILESTATUS_ERROR_DATE;

	//We test the oid of the RRN
	//except for test card because, test card may have a bad oid
	if(!pcard->isTestCard())
		if(!m_cryptoFwk->VerifyOidRrn(CertRrn->getData()))
			return CARDFILESTATUS_ERROR_RRN;

	APL_EidFile_IDSign *sign=pcard->getFileIDSign();

	//If the status of the IDSign file is not OK,
	//The IDSign status is return
	filestatus=sign->getStatus(true);
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;*/

	//if(!m_cryptoFwk->VerifySignatureSha1(m_data,sign->getData(),CertRrn->getData()))
	//	return CARDFILESTATUS_ERROR_SIGNATURE;

	//If this is not a test card, the rrn status must be OK, unless we return an error
	//For a test card, the status could be something else (for ex ISSUER)
	/*L_CERTIF_STATUS_VALID
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_CRL
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_OCSP)
			return CARDFILESTATUS_ERROR_CERT;
	}*/

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
	char cBuffer[15500];
	unsigned char ucBuffer[15500];
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());
    CByteArray Validation;

	//Card Validation
    pteidngtraceBuffer = m_data.GetBytes(PTEIDNG_FIELD_TRACE_POS_VALIDATION, PTEIDNG_FIELD_TRACE_LEN_VALIDATION);
    pteidngtraceBuffer.TrimRight('\0');
    Validation = pteidngtraceBuffer;
    if (Validation.ToString() == "01")
    	m_Validation = "O Cartão de Cidadão encontra-se activo";
    else
    	m_Validation = "O Cartão de Cidadão não encontra-se activo";


    isCardActive = (Validation.GetByte(0) == PTEIDNG_ACTIVE_CARD);

    m_mappedFields = true;

    return true;
}

bool APL_EidFile_Trace::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	bool bAllowTest=pcard->getAllowTestCard();
	bool bAllowBadDate=pcard->getAllowBadDate();

	tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
	if(status==CARDFILESTATUS_OK)
		return true;

	//If the autorisation changed, we read the card again
	if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
		|| (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

		status=LoadData(true);

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
	photo = NULL;
}

APL_EidFile_ID::~APL_EidFile_ID()
{
}

tCardFileStatus APL_EidFile_ID::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus filestatus;

	//Remove since it's rrn related
	/*if(!CertRrn)
		return CARDFILESTATUS_ERROR_RRN;

	//If the status of the RRN file is not OK, 
	//The rrn status is return
	filestatus=CertRrn->getFileStatus();
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	//Get the status of the certificate RRN
	APL_CertifStatus rrnstatus=CertRrn->getStatus();
	if((rrnstatus==APL_CERTIF_STATUS_TEST || rrnstatus==APL_CERTIF_STATUS_ISSUER) && !pcard->getAllowTestCard())
		return CARDFILESTATUS_ERROR_TEST; 

	if(rrnstatus==APL_CERTIF_STATUS_DATE && !pcard->getAllowBadDate())
		return CARDFILESTATUS_ERROR_DATE; 

	//We test the oid of the RRN 
	//except for test card because, test card may have a bad oid
	if(!pcard->isTestCard())
		if(!m_cryptoFwk->VerifyOidRrn(CertRrn->getData()))
			return CARDFILESTATUS_ERROR_RRN;

	APL_EidFile_IDSign *sign=pcard->getFileIDSign();

	//If the status of the IDSign file is not OK, 
	//The IDSign status is return
	filestatus=sign->getStatus(true);
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;*/

	//if(!m_cryptoFwk->VerifySignatureSha1(m_data,sign->getData(),CertRrn->getData()))
	//	return CARDFILESTATUS_ERROR_SIGNATURE;

	//If this is not a test card, the rrn status must be OK, unless we return an error
	//For a test card, the status could be something else (for ex ISSUER)
	/*L_CERTIF_STATUS_VALID
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_CRL
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_OCSP)
			return CARDFILESTATUS_ERROR_CERT; 
	}*/

	return CARDFILESTATUS_OK;
}

void APL_EidFile_ID::EmptyFields()
{
	m_IDFile.clear();
	m_DocumentVersion.clear();
	m_Country.clear();
	m_LogicalNumber.clear();
	m_ChipNumber.clear();
	m_ValidityBeginDate.clear();
	m_ValidityEndDate.clear();
	m_LocalofRequest.clear();
	m_CivilianIdNumber.clear();
	m_Surname.clear();
	m_GivenName.clear();
	m_Nationality.clear();
	m_LocationOfBirth.clear();
	m_DateOfBirth.clear();
	m_Gender.clear();
	m_DocumentType.clear();
	m_SpecialStatus.clear();
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
	if (photo){
		delete photo;
		photo = NULL;
	}
	m_PhotoHash.ClearContents();
	if (cardKey){
		delete cardKey;
		cardKey = NULL;
	}
	m_mappedFields = false;
}

bool APL_EidFile_ID::MapFields()
{
	// we dont want to read the fields every time
	if (m_mappedFields)
		return true;

	CByteArray pteidngidBuffer;
	char cBuffer[15500];
	unsigned char ucBuffer[15500];
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

	//IDVersion - Card Version
    pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentVersion, PTEIDNG_FIELD_ID_LEN_DocumentVersion);
    pteidngidBuffer.TrimRight(' ');
    m_DocumentVersion.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ChipNr
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumberPAN, PTEIDNG_FIELD_ID_LEN_DocumentNumberPAN);
	pteidngidBuffer.TrimRight(' ');
	m_ChipNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());
	m_SerialNumber = m_ChipNumber;

	//Country
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Country,PTEIDNG_FIELD_ID_LEN_Country);
	pteidngidBuffer.TrimRight(' ');
	m_Country.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ValidityBeginDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityBeginDate,PTEIDNG_FIELD_ID_LEN_ValidityBeginDate);
	pteidngidBuffer.TrimRight(' ');
	m_ValidityBeginDate.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//ValidityEndDate
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_ValidityEndDate,PTEIDNG_FIELD_ID_LEN_ValidityEndDate);
	pteidngidBuffer.TrimRight(' ');
	m_ValidityEndDate.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//IssuingMunicipality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_LocalofRequest, PTEIDNG_FIELD_ID_LEN_LocalofRequest);
	pteidngidBuffer.TrimRight(' ');
	m_LocalofRequest.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Civilian Identification Number (NIC)
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_CivilianIdNumber, PTEIDNG_FIELD_ID_LEN_CivilianIdNumber);
	pteidngidBuffer.TrimRight(' ');
	m_CivilianIdNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Surname
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Surname, PTEIDNG_FIELD_ID_LEN_Surname);
	pteidngidBuffer.TrimRight(' ');
	m_Surname.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//FirstName_1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Name, PTEIDNG_FIELD_ID_LEN_Name);
	pteidngidBuffer.TrimRight(' ');
	m_GivenName.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());
	/*std::string in_utf8;
	in_utf8 = IBM850_toUtf8(m_FirstName1);
	m_FirstName1 = in_utf8;*/

	//Nationality
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Nacionality, PTEIDNG_FIELD_ID_LEN_Nacionality);
	pteidngidBuffer.TrimRight(' ');
	m_Nationality.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//LocationOfBirth
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillUTF8Data(PTEID_FIELD_TAG_ID_LocationOfBirth, cBuffer, &ulLen);
	m_LocationOfBirth.assign(cBuffer, 0, ulLen);

	//DateOfBirth
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DateOfBirth, PTEIDNG_FIELD_ID_LEN_DateOfBirth);
	pteidngidBuffer.TrimRight(' ');
	m_DateOfBirth.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//Gender
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Gender, PTEIDNG_FIELD_ID_LEN_Gender);
	pteidngidBuffer.TrimRight(' ');
	m_Gender.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//DocumentType
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentType, PTEIDNG_FIELD_ID_LEN_DocumentType);
	pteidngidBuffer.TrimRight(' ');
	m_DocumentType.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//SpecialStatus
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(PTEID_FIELD_TAG_ID_SpecialStatus, cBuffer, &ulLen);
	m_SpecialStatus.assign(cBuffer, 0, ulLen);

	//PhotoHash
    ulLen = sizeof(cBuffer);
	memset(ucBuffer,0,ulLen);
	oTLVBuffer.FillBinaryData(PTEID_FIELD_TAG_ID_PhotoHash, ucBuffer, &ulLen);
	m_PhotoHash.ClearContents();
	m_PhotoHash.Append(ucBuffer,ulLen);

	//Duplicata
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(PTEID_FIELD_TAG_ID_Duplicata, cBuffer, &ulLen);
	m_Duplicata.assign(cBuffer, 0, ulLen);

	//SpecialOrganization
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(PTEID_FIELD_TAG_ID_SpecialOrganization, cBuffer, &ulLen);
	m_SpecialOrganization.assign(cBuffer, 0, ulLen);

	//Height
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Height, PTEIDNG_FIELD_ID_LEN_Height);
	pteidngidBuffer.TrimRight(' ');
	m_Height.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//DocumentNumber
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_DocumentNumber, PTEIDNG_FIELD_ID_LEN_DocumentNumber);
	pteidngidBuffer.TrimRight(' ');
	m_DocumentNumber.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//TaxNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_TaxNo, PTEIDNG_FIELD_ID_LEN_TaxNo);
	pteidngidBuffer.TrimRight(' ');
	m_TaxNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//SocialSecurityNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_SocialSecurityNo, PTEIDNG_FIELD_ID_LEN_SocialSecurityNo);
	pteidngidBuffer.TrimRight(' ');
	m_SocialSecurityNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//HealthNo
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_HealthNo, PTEIDNG_FIELD_ID_LEN_HealthNo);
	pteidngidBuffer.TrimRight(' ');
	m_HealthNo.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//IssuingEntity
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_IssuingEntity, PTEIDNG_FIELD_ID_LEN_IssuingEntity);
	pteidngidBuffer.TrimRight(' ');
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
		photo = new PhotoPteid(photoRAW, cbeff, facialrechdr, facialinfo, imageinfo);
	}

	//Card Authentication Key (modulus + exponent)
	{
		CByteArray modulus = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_MODULUS, PTEIDNG_FIELD_ID_LEN_MODULUS);
		CByteArray exponent = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_EXPONENT, PTEIDNG_FIELD_ID_LEN_EXPONENT);

		cardKey = new APLPublicKey(modulus,exponent);
	}
	//MRZ1
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz1, PTEIDNG_FIELD_ID_LEN_Mrz1);
	pteidngidBuffer.TrimRight(' ');
	m_MRZ1.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//MRZ2
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz2, PTEIDNG_FIELD_ID_LEN_Mrz2);
	pteidngidBuffer.TrimRight(' ');
	m_MRZ2.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//MRZ3
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_Mrz3, PTEIDNG_FIELD_ID_LEN_Mrz3);
	pteidngidBuffer.TrimRight(' ');
	m_MRZ3.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	//AccidentalIndications
	pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_AccidentalIndications, PTEIDNG_FIELD_ID_LEN_AccidentalIndications);
	pteidngidBuffer.TrimRight(' ');
	m_AccidentalIndications.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());

	/* ID File Caching
	 * try
	{
		ofstream myfile;
		APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);
		std::string	m_cachedirpath = conf_dir.getString();
		std::string pteidfile = m_cachedirpath;
		pteidfile.append("/pteidng-");
		pteidfile.append(m_SerialNumber);
		pteidfile.append("-");
		pteidfile.append(PTEID_FILE_ID);
		pteidfile.append(".bin");
		myfile.open (pteidfile.c_str(), ios::binary);
		pteidngidBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FILE, PTEIDNG_FIELD_ID_LEN_FILE);
		m_IDFile.assign((char*)(pteidngidBuffer.GetBytes()), pteidngidBuffer.Size());
		myfile << m_IDFile;
		myfile.close();
	}
	catch(CMWException& e)
	{
		MWLOG(LEV_INFO, MOD_APL, L"Write cache file %ls on disk failed", PTEID_FILE_ID);
	}*/

	//MemberOfFamily - See if this segfaults in every platform
	//m_MemberOfFamily = m_GivenNameMother + " " + m_SurnameMother + " * " + m_GivenNameFather + " " + m_SurnameFather;

	m_mappedFields = true;

	return true;
}

bool APL_EidFile_ID::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	bool bAllowTest=pcard->getAllowTestCard();
	bool bAllowBadDate=pcard->getAllowBadDate();

	tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
	if(status==CARDFILESTATUS_OK)
		return true;
	//If the autorisation changed, we read the card again
	if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
		|| (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

		status=LoadData(true);

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

const char *APL_EidFile_ID::getLocationOfBirth()
{
	if(ShowData())
		return m_LocationOfBirth.c_str();

	return "";
}

const char *APL_EidFile_ID::getNationality()
{
	if(ShowData())
		return m_Nationality.c_str();

	return "";
}

const char *APL_EidFile_ID::getDuplicata()
{
	if(ShowData())
		return m_Duplicata.c_str();

	return "";
}

const char *APL_EidFile_ID::getSpecialOrganization()
{
	if(ShowData())
		return m_SpecialOrganization.c_str();

	return "";
}

const char *APL_EidFile_ID::getMemberOfFamily()
{
	if(ShowData())
		return m_MemberOfFamily.c_str();

	return "";
}

const char *APL_EidFile_ID::getLogicalNumber()
{
	if(ShowData())
		return m_LogicalNumber.c_str();

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

const char *APL_EidFile_ID::getSpecialStatus()
{
	if(ShowData())
		return m_SpecialStatus.c_str();

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
		return strdup(m_Parents.c_str());

	return "";
}

PhotoPteid *APL_EidFile_ID::getPhotoObj()
{

	if(ShowData())
		return photo;

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

/*****************************************************************************************
---------------------------------------- APL_EidFile_IDSign -----------------------------------------
*****************************************************************************************/
APL_EidFile_IDSign::APL_EidFile_IDSign(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_ID_SIGN,NULL)
{
}

APL_EidFile_IDSign::~APL_EidFile_IDSign()
{
}

/**
  * No verification here.
  * This file is needed for other verifications. 
  * If it is corrupted, these verifications will failed.
  */
tCardFileStatus APL_EidFile_IDSign::VerifyFile()
{
	return CARDFILESTATUS_OK;
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

tCardFileStatus APL_EidFile_Address::VerifyFile()
{
	if(!m_card)
		return CARDFILESTATUS_ERROR;
	
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	tCardFileStatus filestatus;

	//APL_Certif *CertRrn=pcard->getRrn();

	/*if(!CertRrn)
		return CARDFILESTATUS_ERROR_RRN;

	//If the status of the RRN file is not OK, 
	//The rrn status is return
	filestatus=CertRrn->getFileStatus();
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	//Get the status of the certificate RRN
	APL_CertifStatus rrnstatus=CertRrn->getStatus();
	if((rrnstatus==APL_CERTIF_STATUS_TEST || rrnstatus==APL_CERTIF_STATUS_ISSUER) && !pcard->getAllowTestCard())
		return CARDFILESTATUS_ERROR_TEST; 

	if(rrnstatus==APL_CERTIF_STATUS_DATE && !pcard->getAllowBadDate())
		return CARDFILESTATUS_ERROR_DATE; 

	//We test the oid of the RRN 
	//except for test card because, test card may have a bad oid
	if(!pcard->isTestCard())
		if(!m_cryptoFwk->VerifyOidRrn(CertRrn->getData()))
			return CARDFILESTATUS_ERROR_RRN;

	APL_EidFile_AddressSign *sign=pcard->getFileAddressSign();

	//If the status of the IDSign file is not OK, 
	//The IDSign status is return
	filestatus=sign->getStatus(true);
	if(filestatus!=CARDFILESTATUS_OK)
		return filestatus;

	APL_EidFile_IDSign *idsign=pcard->getFileIDSign();

	CByteArray dataToSign;					//To check the signature,
	dataToSign.Append(m_data);				//we need to remove the null byte
	dataToSign.TrimRight();					//at the end of the address file
	dataToSign.Append(idsign->getData());	//then, we add the signature of the id file

	if(!m_cryptoFwk->VerifySignatureSha1(dataToSign,sign->getData(),CertRrn->getData()))
		return CARDFILESTATUS_ERROR_SIGNATURE;

	//If this is not a test card, the rrn status must be OK, unless we return an error
	//For a test card, the status could be something else (for ex ISSUER)
	if(!pcard->isTestCard())
	{
		if(rrnstatus!=APL_CERTIF_STATUS_VALID 
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_CRL
			&& rrnstatus!=APL_CERTIF_STATUS_VALID_OCSP)
			return CARDFILESTATUS_ERROR_CERT; 
	}*/

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_Address::ShowData()
{
	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	bool bAllowTest=pcard->getAllowTestCard();
	bool bAllowBadDate=pcard->getAllowBadDate();

	tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
	if(status==CARDFILESTATUS_OK)
		return true;

	//If the autorisation changed, we read the card again
	if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
		|| (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

		status=LoadData(true);

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
}

void APL_EidFile_Address::AddressFields()
{
		CByteArray pteidngAddressBuffer;
		char cBuffer[1200];
		unsigned long ulLen=0;

		CTLVBuffer oTLVBuffer;
	    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

		//District Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_DistrictCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //District Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DISTRICT_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_DistrictDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Municipality Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY, PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_MunicipalityCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Municipality Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_MunicipalityDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //CivilParish Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH, PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_CivilParishCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //CivilParish Description
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH_DESCRIPTION, PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH_DESCRIPTION);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_CivilParishDescription.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Abbreviated Street Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_STREET_TYPE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_AbbrStreetType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Street Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREET_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_STREET_TYPE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_StreetType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Street Name
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_STREETNAME, PTEIDNG_FIELD_ADDRESS_LEN_STREETNAME);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_StreetName.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Abbreviated Building Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ABBR_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_ABBR_BUILDING_TYPE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_AbbrBuildingType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Building Type
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_BUILDING_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_BUILDING_TYPE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_BuildingType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //DoorNo
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_DOORNO, PTEIDNG_FIELD_ADDRESS_LEN_DOORNO);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_DoorNo.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Floor
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_FLOOR, PTEIDNG_FIELD_ADDRESS_LEN_FLOOR);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Floor.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Side
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_SIDE, PTEIDNG_FIELD_ADDRESS_LEN_SIDE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Side.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Place
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_PLACE, PTEIDNG_FIELD_ADDRESS_LEN_PLACE);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Place.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Locality
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_LOCALITY);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Locality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Zip4
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP4, PTEIDNG_FIELD_ADDRESS_LEN_ZIP4);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Zip4.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Zip3
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_ZIP3, PTEIDNG_FIELD_ADDRESS_LEN_ZIP3);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_Zip3.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Postal Locality
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_POSTALLOCALITY, PTEIDNG_FIELD_ADDRESS_LEN_POSTALLOCALITY);
	    pteidngAddressBuffer.TrimRight(' ');
	    m_PostalLocality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    //Generated Address Code
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_GENADDRESS_NUM, PTEIDNG_FIELD_ADDRESS_LEN_GENADDRESS_NUM);
	    pteidngAddressBuffer.TrimRight('\0');
	    m_Generated_Address_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	    /* lmedinas: Address File caching */
	    /*ofstream myfile;
	    APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);
	    std::string	m_cachedirpath = conf_dir.getString();
	    std::string pteidfile = m_cachedirpath;
	    pteidfile.append("/pteidng-");
	    pteidfile.append(m_SerialNumber);
	    pteidfile.append("-");
	    pteidfile.append(PTEID_FILE_ADDRESS);
	    pteidfile.append(".bin");
	    myfile.open (pteidfile.c_str());
	    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FILE, PTEIDNG_FIELD_ID_LEN_FILE);
	    m_AddressFile.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());
	    myfile << m_AddressFile;
	    myfile.close();*/
}

void APL_EidFile_Address::ForeignerAddressFields()
{
	CByteArray pteidngAddressBuffer;
	char cBuffer[1200];
	unsigned long ulLen=0;

	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

	//Foreign Country
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_COUNTRY_DESCRIPTION, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_COUNTRY_DESCRIPTION);
	pteidngAddressBuffer.TrimRight(' ');
	m_Foreign_Country.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	//Foreign Generic Address
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_ADDRESS, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_ADDRESS);
	pteidngAddressBuffer.TrimRight(' ');
	m_Foreign_Generic_Address.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	//Foreign City
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_CITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_CITY);
	pteidngAddressBuffer.TrimRight(' ');
	m_Foreign_City.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Region
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_REGION, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_REGION);
    pteidngAddressBuffer.TrimRight(' ');
    m_Foreign_Region.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Locality
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_LOCALITY, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_LOCALITY);
    pteidngAddressBuffer.TrimRight(' ');
    m_Foreign_Locality.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Postal Code
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_POSTAL_CODE, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_POSTAL_CODE);
    pteidngAddressBuffer.TrimRight(' ');
    m_Foreign_Postal_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    //Foreign Generated Address Code
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_GENADDRESS_NUM, PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_GENADDRESS_NUM);
    pteidngAddressBuffer.TrimRight(' ');
    m_Generated_Address_Code.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

    /* martinho - do not cache the address file
    ofstream myfile;
    APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);
    std::string	m_cachedirpath = conf_dir.getString();
    std::string pteidfile = m_cachedirpath;
    pteidfile.append("/pteidng-");
    pteidfile.append(m_SerialNumber);
    pteidfile.append("-");
    pteidfile.append(PTEID_FILE_ADDRESS);
    pteidfile.append(".bin");
    myfile.open (pteidfile.c_str());
    pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ID_POS_FILE, PTEIDNG_FIELD_ID_LEN_FILE);
    m_AddressFile.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());
    myfile << m_AddressFile;
    myfile.close();
	*/
}

bool APL_EidFile_Address::MapFields()
{
	// MARTINHO: have we mapped the fields yet?
	if (m_mappedFields)
		return true;

	CByteArray pteidngAddressBuffer;
	char cBuffer[1200];
	unsigned long ulLen=0;

	CTLVBuffer oTLVBuffer;
	oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

	// Address Type
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_TYPE, PTEIDNG_FIELD_ADDRESS_LEN_TYPE);
	pteidngAddressBuffer.TrimRight(' ');
	m_AddressType.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	// Country code
	pteidngAddressBuffer = m_data.GetBytes(PTEIDNG_FIELD_ADDRESS_POS_COUNTRY, PTEIDNG_FIELD_ADDRESS_LEN_COUNTRY);
	pteidngAddressBuffer.TrimRight('\0');
	m_CountryCode.assign((char*)(pteidngAddressBuffer.GetBytes()), pteidngAddressBuffer.Size());

	if (m_AddressType == m_FOREIGN)
		ForeignerAddressFields();
	else
		AddressFields();

	// MARTINHO: so we've mapped the fields no need to map them again
	m_mappedFields = true;

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
	return (m_AddressType.compare(m_NATIONAL));
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

/*****************************************************************************************
---------------------------------------- APL_EidFile_AddressSign -----------------------------------------
*****************************************************************************************/
APL_EidFile_AddressSign::APL_EidFile_AddressSign(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_ADDRESS_SIGN,NULL)
{
}

APL_EidFile_AddressSign::~APL_EidFile_AddressSign()
{
}

/**
  * No verification here.
  * This file is needed for other verifications. 
  * If it is corrupted, these verifications will failed.
  */
tCardFileStatus APL_EidFile_AddressSign::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Sod -----------------------------------------
*****************************************************************************************/
APL_EidFile_Sod::APL_EidFile_Sod(APL_EIDCard *card):APL_CardFile(card,PTEID_FILE_PHOTO,NULL)
{
}

APL_EidFile_Sod::~APL_EidFile_Sod()
{
}

tCardFileStatus APL_EidFile_Sod::VerifyFile()
{
	/*if(!m_card)
		return CARDFILESTATUS_ERROR;

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

	//The hash for the photo is in the ID file
	//const CByteArray &hash=pcard->getFileID()->getPhotoHash();
	const CByteArray &hash=pcard->getFileID()->getData();

	//If the status of the ID file is not OK, the hash is not valid.
	//The id status is return
	tCardFileStatus idstatus=pcard->getFileID()->getStatus();
	if(idstatus!=CARDFILESTATUS_OK)
		return idstatus;

	//We check if the hash correspond to the photo
	if(!m_cryptoFwk->VerifyHashSha1(m_data,hash))
		return CARDFILESTATUS_ERROR_HASH;*/

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_Sod::ShowData()
{
        APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

        bool bAllowTest=pcard->getAllowTestCard();
        bool bAllowBadDate=pcard->getAllowBadDate();

        tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
        if(status==CARDFILESTATUS_OK)
                return true;

        //If the autorisation changed, we read the card again
        if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
                || (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

                status=LoadData(true);

        if(status==CARDFILESTATUS_OK)
                return true;

        return false;
}

void APL_EidFile_Sod::EmptyFields()
{
	m_Sod.clear();
	m_mappedFields = false;
}

bool APL_EidFile_Sod::MapFields()
{
	if (m_mappedFields)
		return true;

	CByteArray pteidngSodBuffer;
    char cBuffer[1200];
    unsigned long ulLen=0;

    CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(m_data.GetBytes(), m_data.Size());

    //PersoData
    pteidngSodBuffer = m_data.GetBytes(0, 50);
    pteidngSodBuffer.TrimRight(' ');
    m_Sod.assign((char*)(pteidngSodBuffer.GetBytes()), pteidngSodBuffer.Size());

    /* File Caching
     * try
    {
    	ofstream myfile;
    	APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);
    	std::string	m_cachedirpath = conf_dir.getString();
    	std::string pteidfile = m_cachedirpath;
    	pteidfile.append("/pteidng-");
    	pteidfile.append(m_SerialNumber);
    	pteidfile.append("-");
    	pteidfile.append(PTEID_FILE_SOD);
    	pteidfile.append(".bin");
    	myfile.open (pteidfile.c_str());
    	pteidngSodBuffer = m_data.GetBytes(PTEIDNG_FIELD_SOD_POS_FILE, PTEIDNG_FIELD_SOD_LEN_FILE);
    	m_SodFile.assign((char*)(pteidngSodBuffer.GetBytes()), pteidngSodBuffer.Size());
    	myfile << m_SodFile;
    	myfile.close();
    }
    catch(CMWException& e)
    {
    	MWLOG(LEV_INFO, MOD_APL, L"Write cache file %ls on disk failed", PTEID_FILE_SOD);
    }*/

    m_mappedFields = true;

    return true;
}

const char *APL_EidFile_Sod::getSod()
{
	if(ShowData())
		return m_Sod.c_str();

	return "";
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

	APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);
	/*
	//The hash for the photo is in the ID file
	//const CByteArray &hash=pcard->getFileID()->getPhotoHash();
	const CByteArray &hash=pcard->getFileID()->getData();

	//If the status of the ID file is not OK, the hash is not valid.
	//The id status is return
	tCardFileStatus idstatus=pcard->getFileID()->getStatus();
	if(idstatus!=CARDFILESTATUS_OK)
		return idstatus;

	//We check if the hash correspond to the photo
	if(!m_cryptoFwk->VerifyHashSha1(m_data,hash))
		return CARDFILESTATUS_ERROR_HASH;*/

	return CARDFILESTATUS_OK;
}

bool APL_EidFile_PersoData::ShowData()
{
        APL_EIDCard *pcard=dynamic_cast<APL_EIDCard *>(m_card);

        bool bAllowTest=pcard->getAllowTestCard();
        bool bAllowBadDate=pcard->getAllowBadDate();

        tCardFileStatus status=getStatus(true,&bAllowTest,&bAllowBadDate);
        if(status==CARDFILESTATUS_OK)
                return true;

        //If the autorisation changed, we read the card again
        if((status==CARDFILESTATUS_ERROR_TEST && pcard->getAllowTestCard())
                || (status==CARDFILESTATUS_ERROR_DATE && pcard->getAllowBadDate()))

                status=LoadData(true);

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
    char cBuffer[1200];
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

const char *APL_EidFile_PersoData::getPersoData()
{
	if(ShowData())
		return m_PersoData.c_str();

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

	CByteArray temp;
	char *ascii;

	/* 	martinho: yes it is an asn1 file but we only need 2 fields, so lets extract them.
		TODO: read the file as asn1, this might be useful with the asn1c compiler.

		EFCIAFILE DEFINITIONS ::=
		BEGIN

	  		EFCIA ::= SEQUENCE {
	  			value		INTEGER,
	  			number		OCTET STRING,
	  			applet		UTF8String,
				text  [0] IMPLICIT IA5String,
				whatever	BIT STRING
	  		}
		END
	*/

	temp = m_data.GetBytes(PTEID_FIELD_TOKENINFO_POS_LABEL, PTEID_FIELD_TOKENINFO_LEN_LABEL);
	temp.TrimRight('\0');
	m_label.assign((char*)(temp.GetBytes()), temp.Size());

	temp = m_data.GetBytes(PTEID_FIELD_TOKENINFO_POS_SERIAL, PTEID_FIELD_TOKENINFO_LEN_SERIAL);
	temp.TrimRight('\0');
	//martinho: no need to create a bcd2ascii, this one works fine
	ascii = bin2AsciiHex(temp.GetBytes(),temp.Size());
	m_TokenSerialNumber.assign(ascii);





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


const char *APL_EidFile_TokenInfo::getTokenLabel(){
	if(ShowData())
			return m_label.c_str();

		return "";
}

const char *APL_EidFile_TokenInfo::getTokenSerialNumber(){
	if(ShowData())
			return m_TokenSerialNumber.c_str();

		return "";
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
