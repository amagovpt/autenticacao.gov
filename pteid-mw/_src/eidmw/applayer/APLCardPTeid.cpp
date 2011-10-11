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
#include "APLCardPTeid.h"
#include "CardPTeid.h"
#include "TLVBuffer.h"
#include "APLCertif.h"
#include "cryptoFwkPTeid.h"
#include "CardPTeidDef.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "CardLayer.h"
#include "MiscUtil.h"

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- APL_EIDCard -----------------------------------------
*****************************************************************************************/

APL_AccessWarningLevel APL_EIDCard::m_lWarningLevel=APL_ACCESSWARNINGLEVEL_TO_ASK;

APL_EIDCard::APL_EIDCard(APL_ReaderContext *reader):APL_SmartCard(reader)
{
	m_docfull=NULL;
	m_docid=NULL;
	m_picture=NULL;
	m_docinfo=NULL;

	m_FileID=NULL;
	m_FileIDSign=NULL;
	m_FileAddress=NULL;
	m_FileAddressSign=NULL;
	m_FilePhoto=NULL;
	m_FilePersoData=NULL;
	m_FileTokenInfo=NULL;

	m_FileRrn=NULL;
	m_FileRrnCa=NULL;
	
	m_CertRrn=NULL;
	m_CertRrnCa=NULL;

	m_cardinfosign=NULL;

	m_fileCertAuthentication=NULL;
	m_fileCertSignature=NULL;
	m_fileCertCA=NULL;
	m_fileCertRoot=NULL;
}

APL_EIDCard::~APL_EIDCard()
{
	if(m_docfull)
	{
		delete m_docfull;
		m_docfull=NULL;
	}
	if(m_docid)
	{
		delete m_docid;
		m_docid=NULL;
	}
	if(m_picture)
	{
		delete m_picture;
		m_picture=NULL;
	}

	if(m_docinfo)
	{
		delete m_docinfo;
		m_docinfo=NULL;
	}

	if(m_FileID)
	{
		delete m_FileID;
		m_FileID=NULL;
	}

	if(m_FileIDSign)
	{
		delete m_FileIDSign;
		m_FileIDSign=NULL;
	}

	if(m_FileAddress)
	{
		delete m_FileAddress;
		m_FileAddress=NULL;
	}

	if(m_FileAddressSign)
	{
		delete m_FileAddressSign;
		m_FileAddressSign=NULL;
	}

	if(m_FilePhoto)
	{
		delete m_FilePhoto;
		m_FilePhoto=NULL;
	}

	if(m_FilePersoData)
	{
		delete m_FilePersoData;
		m_FilePersoData=NULL;
	}

	if(m_FileTokenInfo)
	{
		delete m_FileTokenInfo;
		m_FileTokenInfo=NULL;
	}

	if(m_FileRrn)
	{
		delete m_FileRrn;
		m_FileRrn=NULL;
	}

	if(m_FileRrnCa)
	{
		delete m_FileRrnCa;
		m_FileRrnCa=NULL;
	}

	if(m_cardinfosign)
	{
		delete m_cardinfosign;
		m_cardinfosign=NULL;
	}

	if(m_fileCertAuthentication)
	{
		delete m_fileCertAuthentication;
		m_fileCertAuthentication=NULL;
	}

	if(m_fileCertSignature)
	{
		delete m_fileCertSignature;
		m_fileCertSignature=NULL;
	}

	if(m_fileCertCA)
	{
		delete m_fileCertCA;
		m_fileCertCA=NULL;
	}

	if(m_fileCertRoot)
	{
		delete m_fileCertRoot;
		m_fileCertRoot=NULL;
	}
}

bool APL_EIDCard::isCardForbidden()
{
	if(isTestCard() && !m_allowTestParam)
		return true;

	return false;
}

bool APL_EIDCard::initVirtualReader()
{
	printf("QQ Here start!\n");
	bool bRet = true;

	m_reader->getSuperParser()->initReadFunction(&readVirtualFileRAW,&readVirtualFileTLV,&readVirtualFileCSV,&readVirtualFileXML);

	//If it is a new version file, we launch the certificates for the validation
	char *stop;
	CByteArray baCert;
	CByteArray baCertP15;
	CByteArray baCount;
	m_reader->getSuperParser()->readData(PTEID_FILE_CERTS_COUNT,baCount);
	unsigned long ulCountCerts=strtoul((char*)baCount.GetBytes(),&stop,10);

	//If it is a old version file, we launch the root for the validation
	if(ulCountCerts>0)
	{
		for(unsigned long i=0;i<ulCountCerts;i++)
		{
			m_reader->getSuperParser()->readData(PTEID_FILE_CERTS,baCert,i);
			m_reader->getSuperParser()->readData(PTEID_FILE_CERTS_P15,baCertP15,i);
			getCertificates()->addCert(NULL,APL_CERTIF_TYPE_UNKNOWN,false,false,i,&baCert,&baCertP15);
		}
	}
	
	getCertificates()->resetFlags();

	//PINS
	CByteArray baPin;
	m_reader->getSuperParser()->readData(PTEID_FILE_PINS_COUNT,baCount);
	unsigned long ulCountPins=strtoul((char*)baCount.GetBytes(),&stop,10);

	//If it is a old version file, we launch the root for the validation
	for(unsigned long i=0;i<ulCountPins;i++)
	{
		m_reader->getSuperParser()->readData(PTEID_FILE_PINS_P15,baPin,i);
		getPins()->addPin(i,&baPin);
	}

	bool oldAllowTestAsked = m_allowTestAsked;
	bool oldAllowTestAnswer = m_allowTestAnswer;

	m_allowTestAsked=true;
	m_allowTestAnswer=true;

	try
	{
		if(bRet && getFileID()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileIDSign()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileAddress()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileAddressSign()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFilePhoto()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileRRN()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileTokenInfo()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileInfo()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;

		if(bRet && getChallenge().Size()!=20)
			bRet=false;
		if(bRet && getChallengeResponse().Size()==0)
			bRet=false;
	}
	catch(CMWException &e)
	{
		e.GetError();		//Avoid warning
		bRet=false;
	}

	m_allowTestAsked=oldAllowTestAsked;
	m_allowTestAnswer=oldAllowTestAnswer;

	return bRet;
}

unsigned long APL_EIDCard::readVirtualFileRAW(APL_SuperParser *parser, const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,PTEID_FILE_ID)==0)
	{
		in=parser->getRawDataEid()->idData;
	}
	else if(strcmp(fileID,PTEID_FILE_ID_SIGN)==0)
	{
		in=parser->getRawDataEid()->idSigData;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS)==0)
	{
		in=parser->getRawDataEid()->addrData;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS_SIGN)==0)
	{
		in=parser->getRawDataEid()->addrSigData;
	}
	else if(strcmp(fileID,PTEID_FILE_PHOTO)==0)
	{
		in=parser->getRawDataEid()->pictureData;
	}
	else if(strcmp(fileID,PTEID_FILE_PERSODATA)==0)
	{
		in=parser->getRawDataEid()->persodata;
	}
	else if(strcmp(fileID,PTEID_FILE_CARDINFO)==0)
	{
		in=parser->getRawDataEid()->cardData;
	}
	else if(strcmp(fileID,PTEID_FILE_TOKENINFO)==0)
	{
		in=parser->getRawDataEid()->tokenInfo;
	}
	else if(strcmp(fileID,PTEID_FILE_RRN)==0)
	{
		in=parser->getRawDataEid()->certRN;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE)==0)
	{
		in=parser->getRawDataEid()->challenge;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE_RESPONSE)==0)
	{
		in=parser->getRawDataEid()->response;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_COUNT)==0)
	{
		if(parser->getRawDataEid()->version==1)
		{
			in.ClearContents();
			in+="0";			//Not supported => 0 certificates
		}
		else
		{
			throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND); //Not supported
		}
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS)==0)
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND); //Not supported
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_P15)==0)
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND); //Not supported
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_COUNT)==0)
	{
		if(parser->getRawDataEid()->version==1)
		{
			in.ClearContents();
			in+="0";			//Not supported => 0 pins
		}
		else
		{
			throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND); //Not supported
		}
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_P15)==0)
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND); //Not supported
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_EIDCard::readVirtualFileTLV(APL_SuperParser *parser, const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,PTEID_FILE_ID)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_ID);
	}
	else if(strcmp(fileID,PTEID_FILE_ID_SIGN)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_IDSIGN);
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_ADDR);
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS_SIGN)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_ADDRSIGN);
	}
	else if(strcmp(fileID,PTEID_FILE_PHOTO)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_PHOTO);
	}
	else if(strcmp(fileID,PTEID_FILE_CARDINFO)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_CARDINFO);
	}
	else if(strcmp(fileID,PTEID_FILE_TOKENINFO)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_TOKENINFO);
	}
	else if(strcmp(fileID,PTEID_FILE_RRN)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_RRN);
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_CHALLENGE);
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE_RESPONSE)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_CHALLENGE_RESPONSE);
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_COUNT)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_CERTS,0);
		unsigned long ulCount=0;
		try
		{
			ulCount=in.GetLong(0);
		}
		catch(CMWException &e)
		{
				if(e.GetError()!=(long)EIDMW_ERR_PARAM_RANGE)
				throw e;
		}
		char buffer[10];
		sprintf_s(buffer,sizeof(buffer),"%ld",ulCount);
		in.ClearContents();
		in+=buffer;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS)==0)
	{
		CByteArray ba;

		parser->readDataTlv(ba,PTEID_TLV_TAG_FILE_CERTS,(unsigned char)(idx+1));

		CTLVBuffer certTlv=CTLVBuffer();
		certTlv.ParseFileTLV(ba.GetBytes(),ba.Size());

		CByteArray cert;

		CTLV *field=certTlv.GetTagData(0x00);
		if(field!=NULL)
			cert.Append(field->GetData(),field->GetLength());
		else
			cert=ba;

		in=cert;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_P15)==0)
	{
		CByteArray ba;

		parser->readDataTlv(ba,PTEID_TLV_TAG_FILE_CERTS,(unsigned char)(idx+1));

		CTLVBuffer certTlv=CTLVBuffer();
		certTlv.ParseFileTLV(ba.GetBytes(),ba.Size());

		CByteArray cert;

		CTLV *field=certTlv.GetTagData(0x01);
		if(field!=NULL)
			cert.Append(field->GetData(),field->GetLength());
		else
			cert=ba;

		in=cert;
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_COUNT)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_PINS,0);
		unsigned long ulCount=0;
		try
		{
			ulCount=in.GetLong(0);
		}
		catch(CMWException &e)
		{
				if(e.GetError()!=(long)EIDMW_ERR_PARAM_RANGE)
				throw e;
		}
		char buffer[10];
		sprintf_s(buffer,sizeof(buffer),"%ld",ulCount);
		in.ClearContents();
		in+=buffer;
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_P15)==0)
	{
		CByteArray ba;

		parser->readDataTlv(ba,PTEID_TLV_TAG_FILE_PINS,(unsigned char)(idx+1));

		CTLVBuffer pinTlv=CTLVBuffer();
		pinTlv.ParseFileTLV(ba.GetBytes(),ba.Size());

		CByteArray pin;

		CTLV *field=pinTlv.GetTagData(0x00);
		if(field!=NULL)
			pin.Append(field->GetData(),field->GetLength());
		else
			pin=ba;

		in=pin;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_EIDCard::readVirtualFileCSV(APL_SuperParser *parser, const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,PTEID_FILE_ID)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_ID);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ID_SIGN)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_IDSIGN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_ADDR);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS_SIGN)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_ADDRSIGN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_PHOTO)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_PHOTO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CARDINFO)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_CARDINFO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_TOKENINFO)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_TOKENINFO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_RRN)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_RRN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_CHALLENGE);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE_RESPONSE)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_CHALLENGE_RESPONSE);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_COUNT)==0)
	{
		parser->readDataCsv(in,PTEID_CSV_TAG_FILE_CERTS_COUNT);
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_CERTS_COUNT,PTEID_CSV_TAG_FILE_CERTS_FIRST,PTEID_CSV_TAG_FILE_CERTS_STEP,idx);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_P15)==0)
	{
		parser->readDataCsv(b64,PTEID_CSV_TAG_FILE_CERTS_COUNT,PTEID_CSV_TAG_FILE_CERTS_P15_FIRST,PTEID_CSV_TAG_FILE_CERTS_STEP,idx);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_COUNT)==0)
	{
		char *stop;
		CByteArray baTmp;
		parser->readDataCsv(baTmp,PTEID_CSV_TAG_FILE_CERTS_COUNT);
		unsigned long ulCount=strtoul((char*)baTmp.GetBytes(),&stop,10);
		unsigned long ulPos= PTEID_CSV_TAG_FILE_CERTS_COUNT + 1 + ulCount * PTEID_CSV_TAG_FILE_CERTS_STEP;
		parser->readDataCsv(in,ulPos);
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_P15)==0)
	{
		char *stop;
		CByteArray baTmp;
		parser->readDataCsv(baTmp,PTEID_CSV_TAG_FILE_CERTS_COUNT);
		unsigned long ulCount=strtoul((char*)baTmp.GetBytes(),&stop,10);
		unsigned long ulPos= PTEID_CSV_TAG_FILE_CERTS_COUNT + 1 + ulCount * PTEID_CSV_TAG_FILE_CERTS_STEP;

		parser->readDataCsv(b64,ulPos,ulPos+PTEID_CSV_TAG_FILE_PINS_STEP,PTEID_CSV_TAG_FILE_PINS_STEP,idx);
		bDecode=true;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_EIDCard::readVirtualFileXML(APL_SuperParser *parser, const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,PTEID_FILE_ID)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_ID);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ID_SIGN)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_IDSIGN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_ADDR);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS_SIGN)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_ADDRSIGN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_PHOTO)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_PHOTO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CARDINFO)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_CARDINFO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_TOKENINFO)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_TOKENINFO);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_RRN)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_RRN);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_CHALLENGE);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CHALLENGE_RESPONSE)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_CHALLENGE_RESPONSE);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_COUNT)==0)
	{
		unsigned long count=parser->countDataXml(PTEID_XML_PATH_FILE_CERTS);
		in.ClearContents();
		char buffer[5];
		sprintf_s(buffer,sizeof(buffer),"%ld",count);
		in.Append((unsigned char*)buffer,sizeof(buffer)-1);
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_CERTS,idx);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_CERTS_P15)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_CERTS_P15,idx);
		bDecode=true;
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_COUNT)==0)
	{
		unsigned long count=parser->countDataXml(PTEID_XML_PATH_FILE_PINS_P15);
		in.ClearContents();
		char buffer[5];
		sprintf_s(buffer,sizeof(buffer),"%ld",count);
		in.Append((unsigned char*)buffer,sizeof(buffer)-1);
	}
	else if(strcmp(fileID,PTEID_FILE_PINS_P15)==0)
	{
		parser->readDataXml(b64,PTEID_XML_PATH_FILE_PINS_P15,idx);
		bDecode=true;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

APL_EidFile_ID *APL_EIDCard::getFileID()
{
	if(!m_FileID)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileID)
		{
			m_FileID=new APL_EidFile_ID(this);
		}
	}

	return m_FileID;
}

APL_EidFile_IDSign *APL_EIDCard::getFileIDSign()
{
	if(!m_FileIDSign)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileIDSign)
		{
			m_FileIDSign=new APL_EidFile_IDSign(this);
		}
	}

	return m_FileIDSign;
}

APL_EidFile_Address *APL_EIDCard::getFileAddress()
{
	if(!m_FileAddress)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileAddress)
		{
			m_FileAddress=new APL_EidFile_Address(this);
		}
	}

	return m_FileAddress;
}

APL_EidFile_AddressSign *APL_EIDCard::getFileAddressSign()
{
	if(!m_FileAddressSign)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileAddressSign)
		{
			m_FileAddressSign=new APL_EidFile_AddressSign(this);
		}
	}

	return m_FileAddressSign;
}

APL_EidFile_Photo *APL_EIDCard::getFilePhoto()
{
	if(!m_FilePhoto)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FilePhoto)
		{
			m_FilePhoto=new APL_EidFile_Photo(this);
		}
	}
	return m_FilePhoto;
}

APL_EidFile_PersoData *APL_EIDCard::getFilePersoData()
{
	if(!m_FilePersoData)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FilePersoData)
		{
			m_FilePersoData=new APL_EidFile_PersoData(this);
		}
	}
	return m_FilePersoData;
}

APL_EidFile_TokenInfo *APL_EIDCard::getFileTokenInfo()
{
	if(!m_FileTokenInfo)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileTokenInfo)
		{
			m_FileTokenInfo=new APL_EidFile_TokenInfo(this);
		}
	}
	return m_FileTokenInfo;
}

APL_EidFile_RRN *APL_EIDCard::getFileRRN()
{
	if(!m_FileRrn)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileRrn)
		{
			m_FileRrn=new APL_EidFile_RRN(this);
			//If status ok, we add the certificate to the store
			if(m_FileRrn->getStatus(true)==CARDFILESTATUS_OK)
				if(NULL == (m_CertRrn = getCertificates()->addCert(m_FileRrn,APL_CERTIF_TYPE_RRN,true,true,ANY_INDEX,NULL,NULL)))
					throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}

	return m_FileRrn;
}

APL_EidFile_RRNCA *APL_EIDCard::getFileRRNCA()
{
	if(!m_FileRrnCa)
	{
		setFileRRNCA(NULL);
	}

	return m_FileRrnCa;
}

APL_EidFile_RRNCA *APL_EIDCard::setFileRRNCA(const CByteArray *data)
{
	CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation

	if(m_FileRrnCa)
	{
		delete m_FileRrnCa;
		m_FileRrnCa=NULL;
	}

	m_FileRrnCa=new APL_EidFile_RRNCA(this,data);
	//If status ok, we add the certificate to the store
	if(m_FileRrnCa->getStatus(true)==CARDFILESTATUS_OK)
		if(NULL == (m_CertRrnCa = getCertificates()->addCert(m_FileRrnCa,APL_CERTIF_TYPE_UNKNOWN,true,true,ANY_INDEX,NULL,NULL)))
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_FileRrnCa;
}

APL_Certif *APL_EIDCard::getRrn()						
{
	if(!m_CertRrn)
	{
		if(getFileRRN() == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	
	return m_CertRrn;

}

APL_Certif *APL_EIDCard::getRrnCa()					
{
	if(!m_CertRrnCa)
	{
		if(getFileRRNCA() == NULL)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

	return m_CertRrnCa;
}

APL_CardType APL_EIDCard::getType() const
{
	return APL_CARDTYPE_PTEID_EID;
}

bool APL_EIDCard::isTestCard()
{
	APL_Certif *rrn = NULL;
	APL_Certif *root = NULL;

	bool bOut = false;

	try
	{
		rrn=getRrn();
		if(rrn) root=rrn->getRoot();
	}
	catch(...)
	{
		return true;
	}

	if(root)
		bOut=root->isTest();

	return bOut;
}

APL_CertifStatus APL_EIDCard::getDataStatus()
{
	return getRrn()->getStatus();
}

unsigned long APL_EIDCard::readFile(const char *csPath, CByteArray &oData, unsigned long  ulOffset, unsigned long  ulMaxLength)
{
	if(!m_reader->isVirtualReader())
	{
		bool bWarning=false;
		for(long i=0;_pteid_files_to_warn[i]!=NULL;i++)
		{
			if(strcmp(csPath,_pteid_files_to_warn[i])==0)
			{
				bWarning=true;
				break;
			}
		}

		if(bWarning)
		{
			askWarningLevel();
		}
	}

	return APL_SmartCard::readFile(csPath,oData,ulOffset,ulMaxLength);
}

unsigned long APL_EIDCard::certificateCount()
{
	if(m_reader->isVirtualReader()) //Virtual Reader
		return 0;

	try
	{
		return APL_SmartCard::certificateCount();
	}
	catch(...)
	{
	}

	if(m_certificateCount==COUNT_UNDEF)
	{
		//PKCS15 is broken
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(m_certificateCount==COUNT_UNDEF)
		{
			m_certificateCount=0;

			if(!m_fileCertAuthentication)
			{
				m_fileCertAuthentication=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_AUTHENTICATION);
				//If status ok, we add the certificate to the store
				if(m_fileCertAuthentication->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertAuthentication,APL_CERTIF_TYPE_AUTHENTICATION,true,false,m_certificateCount,NULL,NULL)))
						throw CMWEXCEPTION(EIDMW_ERR_CHECK);
					m_certificateCount++;
				}
			}

			if(!m_fileCertSignature)
			{
				m_fileCertSignature=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_SIGNATURE);
				//If status ok, we add the certificate to the store
				if(m_fileCertSignature->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertSignature,APL_CERTIF_TYPE_SIGNATURE,true,false,m_certificateCount,NULL,NULL)))
						throw CMWEXCEPTION(EIDMW_ERR_CHECK);
					m_certificateCount++;
				}
			}

			if(!m_fileCertCA)
			{
				m_fileCertCA=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_CA);
				//If status ok, we add the certificate to the store
				if(m_fileCertCA->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertCA,APL_CERTIF_TYPE_CA,true,false,m_certificateCount,NULL,NULL)))
						throw CMWEXCEPTION(EIDMW_ERR_CHECK);
					m_certificateCount++;
				}
			}

			if(!m_fileCertRoot)
			{
				m_fileCertRoot=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_ROOT);
				//If status ok, we add the certificate to the store
				if(m_fileCertRoot->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertRoot,APL_CERTIF_TYPE_ROOT,true,false,m_certificateCount,NULL,NULL)))
						throw CMWEXCEPTION(EIDMW_ERR_CHECK);
					m_certificateCount++;
				}
			}
		}
	}
	return m_certificateCount;
}

APL_XMLDoc& APL_EIDCard::getDocument(APL_DocumentType type)
{
	switch(type)
	{
	case APL_DOCTYPE_FULL:
		return getFullDoc();
	case APL_DOCTYPE_ID:
		return getID();
	case APL_DOCTYPE_PICTURE:
		return getPicture();
	case APL_DOCTYPE_INFO:
		return getDocInfo();
	case APL_DOCTYPE_PINS:
		return *getPins();
	case APL_DOCTYPE_CERTIFICATES:
		return *getCertificates();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_DOCTYPE_UNKNOWN);
	}
}

APL_EIdFullDoc& APL_EIDCard::getFullDoc()
{
	if(!m_docfull)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_docfull)
		{
			m_docfull=new APL_EIdFullDoc(this);
		}
	}

	return *m_docfull;
}

APL_DocEId& APL_EIDCard::getID()
{
	if(!m_docid)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_docid)
		{
			m_docid=new APL_DocEId(this);
		}
	}

	return *m_docid;
}

APL_PictureEid& APL_EIDCard::getPicture()
{
	if(!m_picture)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_picture)
		{
			m_picture=new APL_PictureEid(this);
		}
	}

	return *m_picture;
}

APL_DocVersionInfo& APL_EIDCard::getDocInfo()
{
	if(!m_docinfo)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_docinfo)
		{
			m_docinfo=new APL_DocVersionInfo(this);
		}
	}

	return *m_docinfo;
}

const CByteArray &APL_EIDCard::getCardInfoSignature()
{
	if(!m_cardinfosign)
	{
		if(m_reader->isVirtualReader())
		{
			return EmptyByteArray;
		}
		else
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
			if(!m_cardinfosign)
			{
				CByteArray param;
				CByteArray result;

				BEGIN_CAL_OPERATION(m_reader)
				result = m_reader->getCalReader()->Ctrl(CTRL_PTEID_GETSIGNEDCARDDATA,param);
				END_CAL_OPERATION(m_reader)

				m_cardinfosign=new CByteArray(result.GetBytes(28,128));
			}
		}
	}

	return *m_cardinfosign;

}
const CByteArray& APL_EIDCard::getRawData(APL_RawDataType type)
{
	switch(type)
	{
	case APL_RAWDATA_ID:
		return getRawData_Id();
	case APL_RAWDATA_ID_SIG:
		return getRawData_IdSig();
	case APL_RAWDATA_ADDR:
		return getRawData_Addr();
	case APL_RAWDATA_ADDR_SIG:
		return getRawData_AddrSig();
	case APL_RAWDATA_PICTURE:
		return getRawData_Picture();
	case APL_RAWDATA_CARD_INFO:
		return getRawData_CardInfo();
	case APL_RAWDATA_TOKEN_INFO:
		return getRawData_TokenInfo();
	case APL_RAWDATA_CERT_RRN:
		return getRawData_CertRRN();
	case APL_RAWDATA_CHALLENGE:
		return getRawData_Challenge();
	case APL_RAWDATA_RESPONSE:
		return getRawData_Response();
	case APL_RAWDATA_PERSO_DATA:
		return getRawData_PersoData();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_FILETYPE_UNKNOWN);
	}
}

const CByteArray& APL_EIDCard::getRawData_Id()
{
	return getFileID()->getData();
}

const CByteArray& APL_EIDCard::getRawData_IdSig()
{
	return getFileIDSign()->getData();
}

const CByteArray& APL_EIDCard::getRawData_Addr()
{
	return getFileAddress()->getData();
}

const CByteArray& APL_EIDCard::getRawData_AddrSig()
{
	return getFileAddressSign()->getData();
}

const CByteArray& APL_EIDCard::getRawData_Picture()
{
	return getFilePhoto()->getData();
}

const CByteArray& APL_EIDCard::getRawData_CardInfo()
{
	return getFileInfo()->getData();
}

const CByteArray& APL_EIDCard::getRawData_TokenInfo()
{
	return getFileTokenInfo()->getData();
}

const CByteArray& APL_EIDCard::getRawData_PersoData()
{
	return getFilePersoData()->getData();
}

const CByteArray& APL_EIDCard::getRawData_CertRRN()
{
	return getFileRRN()->getData();
}

const CByteArray& APL_EIDCard::getRawData_Challenge()
{
	return getChallenge();
}

const CByteArray& APL_EIDCard::getRawData_Response()
{
	return getChallengeResponse();
}

void APL_EIDCard::askWarningLevel()
{
	APL_AccessWarningLevel lWarningLevel=getWarningLevel();

	if(lWarningLevel==APL_ACCESSWARNINGLEVEL_REFUSED)
		throw CMWEXCEPTION(EIDMW_ERR_NOT_ALLOW_BY_USER);

	if(lWarningLevel==APL_ACCESSWARNINGLEVEL_BEING_ASKED)
		throw CMWEXCEPTION(EIDMW_ERR_USER_MUST_ANSWER);

	if(lWarningLevel==APL_ACCESSWARNINGLEVEL_TO_ASK)
	{
		//Ask
		int result=DLG_ERR;
		try
		{
			m_lWarningLevel=APL_ACCESSWARNINGLEVEL_BEING_ASKED;
			result = DlgDisplayModal(DLG_ICON_WARN,DLG_MESSAGE_USER_WARNING,L"",DLG_BUTTON_YES + DLG_BUTTON_NO, DLG_BUTTON_YES, DLG_BUTTON_NO);
		}
		catch(...)
		{
			setWarningLevel(APL_ACCESSWARNINGLEVEL_REFUSED);
			throw;
		}

		if(result==DLG_YES)
		{
			setWarningLevel(APL_ACCESSWARNINGLEVEL_ACCEPTED);
		}
		else
		{
			setWarningLevel(APL_ACCESSWARNINGLEVEL_REFUSED);
			throw CMWEXCEPTION(EIDMW_ERR_NOT_ALLOW_BY_USER);
		}
	}
}

void APL_EIDCard::setWarningLevel(APL_AccessWarningLevel lWarningLevel)
{
	//APL_ACCESSWARNINGLEVEL_BEING_ASKED is an internal status that could not be set from outside
	if(lWarningLevel!=APL_ACCESSWARNINGLEVEL_REFUSED 
		&& lWarningLevel!=APL_ACCESSWARNINGLEVEL_TO_ASK
		&& lWarningLevel!=APL_ACCESSWARNINGLEVEL_ACCEPTED)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	m_lWarningLevel=lWarningLevel;
}

APL_AccessWarningLevel APL_EIDCard::getWarningLevel()
{
	return m_lWarningLevel;
}

bool APL_EIDCard::isApplicationAllowed()
{
	try
	{
		askWarningLevel();
	}
	catch(CMWException &e)
	{
		e=e;
	}

	return (m_lWarningLevel==1);
}
/*****************************************************************************************
---------------------------------------- APL_KidsCard ---------------------------------------
*****************************************************************************************/
APL_KidsCard::APL_KidsCard(APL_ReaderContext *reader):APL_EIDCard(reader)
{
}

APL_KidsCard::~APL_KidsCard()
{
}

APL_CardType APL_KidsCard::getType() const
{
	return APL_CARDTYPE_PTEID_KIDS;
}

/*****************************************************************************************
---------------------------------------- CForeignerCard ----------------------------------
*****************************************************************************************/
APL_ForeignerCard::APL_ForeignerCard(APL_ReaderContext *reader):APL_EIDCard(reader)
{
}

APL_ForeignerCard::~APL_ForeignerCard()
{
}

APL_CardType APL_ForeignerCard::getType() const
{
	return APL_CARDTYPE_PTEID_FOREIGNER;
}

/*****************************************************************************************
---------------------------------------- APL_EIdFullDoc -------------------------------------------
*****************************************************************************************/
APL_EIdFullDoc::APL_EIdFullDoc(APL_EIDCard *card)
{	
	m_card=card;
}

APL_EIdFullDoc::~APL_EIdFullDoc()
{
}

bool APL_EIdFullDoc::isAllowed()
{
	try
	{
		if(m_card->getFileID()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFileAddress()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFilePhoto()->getStatus(true)==CARDFILESTATUS_OK)
			return true;
	}
	catch(CMWException& e)
	{
		if (e.GetError() == EIDMW_ERR_NOT_ALLOW_BY_USER)
			return false;
		else
			throw;
	}
	return false;
}

CByteArray APL_EIdFullDoc::getXML(bool bNoHeader)
{
/*
	<pteid_card>
		<doc_version></doc_version>
		<card_type></card_type>
		<biographic>
		</biographic>
		<biometric>
		</biometric>
		<scard>
		</scard>
		<challenge_response>
			<challenge encoding="base64">
			</challenge>
			<response encoding="base64">
			</response>
		</challenge_response>
		<cryptographic>
			<rrn>
			</rrn>
			<certificates>
			</certificates>
			<pins>
			</pins>
		</cryptographic>
	</pteid_card>
*/

	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<pteid_card>\n";
	xml+="	<doc_version>1</doc_version>\n";
	xml+="	<card_type>";
	switch(m_card->getType())
	{
	case APL_CARDTYPE_PTEID_EID:
		xml+=CARDTYPE_NAME_PTEID_EID;
		break;
	case APL_CARDTYPE_PTEID_KIDS:
		xml+=CARDTYPE_NAME_PTEID_KIDS;
		break;
	case APL_CARDTYPE_PTEID_FOREIGNER:
		xml+=CARDTYPE_NAME_PTEID_FOREIGNER;
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	xml+=   "</card_type>\n";
	xml+=m_card->getID().getXML(true);
	xml+=m_card->getPicture().getXML(true);
	xml+=m_card->getDocInfo().getXML(true);

	CByteArray baFileB64;
	xml+="	<challenge_response>\n";
	if(m_cryptoFwk->b64Encode(m_card->getChallenge(),baFileB64))
	{
		xml+="	<challenge encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="	</challenge>\n";
	}
	if(m_cryptoFwk->b64Encode(m_card->getChallengeResponse(),baFileB64))
	{
		xml+="	<response encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="	</response>\n";
	}
	xml+="	</challenge_response>\n";

	xml+="	<cryptographic>\n";
	xml+="	<rrn>\n";
	xml+=m_card->getRrn()->getXML(true);
	xml+="	</rrn>\n";
	xml+=m_card->getCertificates()->getXML(true);
	xml+=m_card->getPins()->getXML(true);
	xml+="	</cryptographic>\n";
	xml+="</pteid_card>\n";

	return xml;
}

CByteArray APL_EIdFullDoc::getCSV()
{
/*
doc_version;card_type;biographic;biometric;certificates;pins;
*/

	CByteArray csv;

	csv+="1";
	csv+=CSV_SEPARATOR;
	switch(m_card->getType())
	{
	case APL_CARDTYPE_PTEID_EID:
		csv+=CARDTYPE_NAME_PTEID_EID;
		break;
	case APL_CARDTYPE_PTEID_KIDS:
		csv+=CARDTYPE_NAME_PTEID_KIDS;
		break;
	case APL_CARDTYPE_PTEID_FOREIGNER:
		csv+=CARDTYPE_NAME_PTEID_FOREIGNER;
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	csv+=CSV_SEPARATOR;
	csv+=m_card->getID().getCSV();
	csv+=m_card->getPicture().getCSV();
	csv+=m_card->getDocInfo().getCSV();

	CByteArray baFileB64;

	if(m_cryptoFwk->b64Encode(m_card->getChallenge(),baFileB64,false))
		csv+=		baFileB64;
	csv+=CSV_SEPARATOR;

	if(m_cryptoFwk->b64Encode(m_card->getChallengeResponse(),baFileB64,false))
		csv+=		baFileB64;
	csv+=CSV_SEPARATOR;

	csv+=m_card->getRrn()->getCSV();

	csv+=m_card->getCertificates()->getCSV();
	csv+=m_card->getPins()->getCSV();
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_EIdFullDoc::getTLV()
{
	CTLVBuffer tlv;

	CByteArray baVersion;
	baVersion.Append(0x02);		//Version 2 (Version 1 was the old MW)
	tlv.SetTagData(PTEID_TLV_TAG_VERSION,baVersion.GetBytes(),baVersion.Size());
	CByteArray baCardType;
	switch(m_card->getType())
	{
	case APL_CARDTYPE_PTEID_EID:
		baCardType.Append(CARDTYPE_NAME_PTEID_EID);
		break;
	case APL_CARDTYPE_PTEID_KIDS:
		baCardType.Append(CARDTYPE_NAME_PTEID_KIDS);
		break;
	case APL_CARDTYPE_PTEID_FOREIGNER:
		baCardType.Append(CARDTYPE_NAME_PTEID_FOREIGNER);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	tlv.SetTagData(PTEID_TLV_TAG_CARDTYPE,baCardType.GetBytes(),baCardType.Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_RRN,m_card->getFileRRN()->getData().GetBytes(),m_card->getFileRRN()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_CHALLENGE,m_card->getChallenge().GetBytes(),m_card->getChallenge().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_CHALLENGE_RESPONSE,m_card->getChallengeResponse().GetBytes(),m_card->getChallengeResponse().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	ba.Append(m_card->getID().getTLV());
	ba.Append(m_card->getPicture().getTLV());
	ba.Append(m_card->getDocInfo().getTLV());
	ba.Append(m_card->getCertificates()->getTLV());
	ba.Append(m_card->getPins()->getTLV());

	return ba;
}

/*****************************************************************************************
---------------------------------------- APL_DocEId ---------------------------------------------
*****************************************************************************************/
APL_DocEId::APL_DocEId(APL_EIDCard *card)
{
	m_card=card;

	m_FirstName.clear();
}

APL_DocEId::~APL_DocEId()
{
}

bool APL_DocEId::isAllowed()
{
	try
	{
		if(m_card->getFileID()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFileAddress()->getStatus(true)==CARDFILESTATUS_OK)
			return true;
	}
	catch(CMWException& e)
	{
		if (e.GetError() == EIDMW_ERR_NOT_ALLOW_BY_USER)
			return false;
		else
			throw;
	}
	return false;
}

CByteArray APL_DocEId::getXML(bool bNoHeader)
{
/* MISSING Fields from PTeid
	<biographic>
		<document>
			<version></version>
			<type></type>
			<id>
				<name></name>
				<surname></surname>
				<gender></gender>
				<date_of_birth></date_of_birth>
				<location_of_birth></location_of_birth>
				<nobility></nobility>
				<nationality></nationality>
				<national_nr></national_nr>
				<special_organization></special_organization>
				<member_of_family></member_of_family>
				<special_status></special_status>
			</id>
			<card>
				<logical_nr></logical_nr>
				<chip_nr></chip_nr>
				<validity>
					<date_begin></date_begin>
					<date_end></date_end>
				</validity>
				<issuing_municipality></issuing_municipality>
			</card>
		</document>
		<address>
			<version></version>
			<street></street>
			<zip></zip>
			<municipality></municipality>
			<country></country>
		</address>
		<files>
			<file_id encoding="base64">
			</file_id>
			<file_id_sign encoding="base64">
			</file_id_sign>
			<file_address encoding="base64">
			</file_address>
			<file_address_sign encoding="base64">
			</file_address_sign>
		</files>
	</biographic>
*/

	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<biographic>\n";
	xml+=" <document>\n";
	xml+="		<version>";
	xml+=			getDocumentVersion();
	xml+=		"</version>\n";
	xml+="		<type>";
	xml+=			getDocumentType();
	xml+=		"</type>\n";
	xml+=" 	<id>\n";
	xml+=" 		<name>";
	xml+=				getFirstName1();
	xml+=			"</name>\n";
	xml+=" 		<surname>";
	xml+=				getSurname();
	xml+=			"</surname>\n";
	xml+=" 		<gender>";
	xml+=				getGender();
	xml+=			"</gender>\n";
	xml+=" 		<date_of_birth>";
	xml+=				getDateOfBirth();
	xml+=			"</date_of_birth>\n";
	xml+=" 		<location_of_birth>";
	xml+=				getLocationOfBirth();
	xml+=			"</location_of_birth>\n";
	xml+=" 		<nobility>";
	xml+=				getNobility();
	xml+=			"</nobility>\n";
	xml+=" 		<nationality>";
	xml+=				getNationality();
	xml+=			"</nationality>\n";
	xml+=" 		<national_nr>";
	xml+=				getNationalNumber();
	xml+=			"</national_nr>\n";
	xml+=" 		<duplicata>";
	xml+=				getDuplicata();
	xml+=			"</duplicata>\n";
	xml+=" 		<special_organization>";
	xml+=				getSpecialOrganization();
	xml+=			"</special_organization>\n";
	xml+=" 		<member_of_family>";
	xml+=				getMemberOfFamily();
	xml+=			"</member_of_family>\n";
	xml+=" 		<special_status>";
	xml+=				getSpecialStatus();
	xml+=			"</special_status>\n";
	xml+="		</id>\n";
	xml+=" 	<card>\n";
	xml+=" 		<logical_nr>";
	xml+=				getLogicalNumber();
	xml+=			"</logical_nr>\n";
	xml+=" 		<chip_nr>";
	xml+=				getChipNumber();
	xml+=			"</chip_nr>\n";
	xml+=" 		<validity>\n";
	xml+=" 	 		<date_begin>";
	xml+=					getValidityBeginDate();
	xml+=			"</date_begin>\n";
	xml+=" 			<date_end>";
	xml+=					getValidityEndDate();
	xml+=			"</date_end>\n";
	xml+=" 		</validity>\n";
	xml+=" 		<issuing_municipality>";
	xml+=				getIssuingMunicipality();
	xml+=			"</issuing_municipality>\n";
	xml+="	 </card>\n";
	xml+=" </document>\n";
	xml+=" <address>\n";
	xml+=" 	<version>";
	xml+=			getAddressVersion();
	xml+=		"</version>\n";
	xml+=" 	<street>";
	xml+=			getStreet();
	xml+=		"</street>\n";
	xml+=" 	<zip>";
	xml+=			getZipCode();
	xml+=		"</zip>\n";
	xml+=" 	<municipality>";
	xml+=			getMunicipality();
	xml+=		"</municipality>\n";
	xml+=" 	<country>";
	xml+=			getCountry();
	xml+=		"</country>\n";
	xml+=" </address>\n";
	xml+="	<files>\n";

	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileID()->getData(),baFileB64))
	{
		xml+="		<file_id encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="		</file_id>\n";
	}
	if(m_cryptoFwk->b64Encode(m_card->getFileIDSign()->getData(),baFileB64))
	{
		xml+="		<file_id_sign encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="		</file_id_sign>\n";
	}
	if(m_cryptoFwk->b64Encode(m_card->getFileAddress()->getData(),baFileB64))
	{
		xml+="		<file_address encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="		</file_address>\n";
	}
	if(m_cryptoFwk->b64Encode(m_card->getFileAddressSign()->getData(),baFileB64))
	{
		xml+="		<file_address_sign encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="		</file_address_sign>\n";
	}
	xml+="	</files>\n";
	xml+="</biographic>\n";

	return xml;
}

CByteArray APL_DocEId::getCSV()
{
/*
version;type;name;surname;gender;date_of_birth;location_of_birth;nobility;nationality;
	national_nr;special_organization;member_of_family;special_status;logical_nr;chip_nr;
	date_begin;date_end;issuing_municipality;version;street;zip;municipality;country;
	file_id;file_id_sign;file_address;file_address_sign;
*/

	CByteArray csv;

	csv+=getDocumentVersion();
	csv+=CSV_SEPARATOR;
	csv+=getDocumentType();
	csv+=CSV_SEPARATOR;
	csv+=getFirstName1();
	csv+=CSV_SEPARATOR;
	csv+=getSurname();
	csv+=CSV_SEPARATOR;
	csv+=getGender();
	csv+=CSV_SEPARATOR;
	csv+=getDateOfBirth();
	csv+=CSV_SEPARATOR;
	csv+=getLocationOfBirth();
	csv+=CSV_SEPARATOR;
	csv+=getNobility();
	csv+=CSV_SEPARATOR;
	csv+=getNationality();
	csv+=CSV_SEPARATOR;
	csv+=getNationalNumber();
	csv+=CSV_SEPARATOR;
	csv+=getDuplicata();
	csv+=CSV_SEPARATOR;
	csv+=getSpecialOrganization();
	csv+=CSV_SEPARATOR;
	csv+=getMemberOfFamily();
	csv+=CSV_SEPARATOR;
	csv+=getSpecialStatus();
	csv+=CSV_SEPARATOR;
	csv+=getLogicalNumber();
	csv+=CSV_SEPARATOR;
	csv+=getChipNumber();
	csv+=CSV_SEPARATOR;
	csv+=getValidityBeginDate();
	csv+=CSV_SEPARATOR;
	csv+=getValidityEndDate();
	csv+=CSV_SEPARATOR;
	csv+=getIssuingMunicipality();
	csv+=CSV_SEPARATOR;
	csv+=getAddressVersion();
	csv+=CSV_SEPARATOR;
	csv+=getStreet();
	csv+=CSV_SEPARATOR;
	csv+=getZipCode();
	csv+=CSV_SEPARATOR;
	csv+=getMunicipality();
	csv+=CSV_SEPARATOR;
	csv+=getCountry();
	csv+=CSV_SEPARATOR;

	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileID()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(m_card->getFileIDSign()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(m_card->getFileAddress()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(m_card->getFileAddressSign()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_DocEId::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_ID,m_card->getFileID()->getData().GetBytes(),m_card->getFileID()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_IDSIGN,m_card->getFileIDSign()->getData().GetBytes(),m_card->getFileIDSign()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_ADDR,m_card->getFileAddress()->getData().GetBytes(),m_card->getFileAddress()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_ADDRSIGN,m_card->getFileAddressSign()->getData().GetBytes(),m_card->getFileAddressSign()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const char *APL_DocEId::getDocumentVersion()
{
	return m_card->getFileID()->getDocumentVersion();
}

const char *APL_DocEId::getDocumentType()
{
	return m_card->getFileID()->getDocumentType();
}

const char *APL_DocEId::getFirstName1()
{
	return m_card->getFileID()->getFirstName1();
}

const char *APL_DocEId::getSurname()
{
	return m_card->getFileID()->getSurname();
}

const char *APL_DocEId::getGender()
{
	return m_card->getFileID()->getGender();
}

const char *APL_DocEId::getDateOfBirth()
{
	return m_card->getFileID()->getDateOfBirth();
}

const char *APL_DocEId::getLocationOfBirth()
{
	return m_card->getFileID()->getLocationOfBirth();
}

const char *APL_DocEId::getNobility()
{
	return m_card->getFileID()->getNobility();
}

const char *APL_DocEId::getNationality()
{
	return m_card->getFileID()->getNationality();
}

const char *APL_DocEId::getNationalNumber()
{
	return m_card->getFileID()->getNationalNumber();
}

const char *APL_DocEId::getDuplicata()
{
	return m_card->getFileID()->getDuplicata();
}

const char *APL_DocEId::getSpecialOrganization()
{
	return m_card->getFileID()->getSpecialOrganization();
}

const char *APL_DocEId::getMemberOfFamily()
{
	return m_card->getFileID()->getMemberOfFamily();
}

const char *APL_DocEId::getLogicalNumber()
{
	return m_card->getFileID()->getLogicalNumber();
}

const char *APL_DocEId::getChipNumber()
{
	return m_card->getFileID()->getChipNumber();
}

const char *APL_DocEId::getValidityBeginDate()
{
	return m_card->getFileID()->getValidityBeginDate();
}

const char *APL_DocEId::getValidityEndDate()
{
	return m_card->getFileID()->getValidityEndDate();
}

const char *APL_DocEId::getIssuingMunicipality()
{
	return m_card->getFileID()->getIssuingMunicipality();
}

const char *APL_DocEId::getSpecialStatus()
{
	return m_card->getFileID()->getSpecialStatus();
}

const char *APL_DocEId::getHeight()
{
	return m_card->getFileID()->getHeight();

}

const char *APL_DocEId::getDocumentNumber()
{
	return m_card->getFileID()->getDocumentNumber();
}

const char *APL_DocEId::getTaxNo()
{
	return m_card->getFileID()->getTaxNo();
}

const char *APL_DocEId::getSocialSecurityNo()
{
	return m_card->getFileID()->getSocialSecurityNo();
}

const char *APL_DocEId::getHealthNo()
{
	return m_card->getFileID()->getHealthNo();
}

const char *APL_DocEId::getIssuingEntity()
{
	return m_card->getFileID()->getIssuingEntity();
}

const char *APL_DocEId::getGivenNameFather()
{
	return m_card->getFileID()->getGivenNameFather();
}

const char *APL_DocEId::getSurnameFather()
{
	return m_card->getFileID()->getSurnameFather();
}

const char *APL_DocEId::getGivenNameMother()
{
	return m_card->getFileID()->getGivenNameMother();
}

const char *APL_DocEId::getSurnameMother()
{
	return m_card->getFileID()->getSurnameMother();
}

const char *APL_DocEId::getPhoto()
{
	return m_card->getFileID()->getPhoto();
}

const char *APL_DocEId::getAddressVersion()
{
	return m_card->getFileAddress()->getAddressVersion();
}

const char *APL_DocEId::getStreet()
{
	return m_card->getFileAddress()->getStreet();
}

const char *APL_DocEId::getZipCode()
{
	return m_card->getFileAddress()->getZipCode();
}

const char *APL_DocEId::getMunicipality()
{
	return m_card->getFileAddress()->getMunicipality();
}

const char *APL_DocEId::getCountry()
{
	return m_card->getFileAddress()->getCountry();
}

const char *APL_DocEId::getCivilParish()
{
	return m_card->getFileAddress()->getCivilParish();
}

const char *APL_DocEId::getStreetName()
{
	return m_card->getFileAddress()->getStreetName();
}

const char *APL_DocEId::getStreetType1()
{
	return m_card->getFileAddress()->getStreetType1();
}

const char *APL_DocEId::getStreetType2()
{
	return m_card->getFileAddress()->getStreetType2();
}

const char *APL_DocEId::getBuildingType1()
{
	return m_card->getFileAddress()->getBuildingType1();
}

const char *APL_DocEId::getBuildingType2()
{
	return m_card->getFileAddress()->getBuildingType2();
}

const char *APL_DocEId::getDoorNo()
{
	return m_card->getFileAddress()->getDoorNo();
}

const char *APL_DocEId::getFloor()
{
	return m_card->getFileAddress()->getFloor();
}

const char *APL_DocEId::getSide()
{
	return m_card->getFileAddress()->getSide();
}

const char *APL_DocEId::getLocality()
{
	return m_card->getFileAddress()->getLocality();
}

const char *APL_DocEId::getZip4()
{
	return m_card->getFileAddress()->getZip4();
}

const char *APL_DocEId::getZip3()
{
	return m_card->getFileAddress()->getZip3();
}

const char *APL_DocEId::getPostalLocality()
{
	return m_card->getFileAddress()->getPostalLocality();
}

const char *APL_DocEId::getDistrict()
{
	return m_card->getFileAddress()->getDistrict();
}

const char *APL_DocEId::getPersoData()
{
	return m_card->getFilePersoData()->getPersoData();
}

/*****************************************************************************************
---------------------------------------- APL_PictureEid -----------------------------------------
*****************************************************************************************/
APL_PictureEid::APL_PictureEid(APL_EIDCard *card)
{	
	m_card=card;
}

APL_PictureEid::~APL_PictureEid()
{
}

bool APL_PictureEid::isAllowed()
{
	try
	{
		if(m_card->getFilePhoto()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFileID()->getStatus(true)==CARDFILESTATUS_OK)
			return true;
	}
	catch(CMWException& e)
	{
		if (e.GetError() == EIDMW_ERR_NOT_ALLOW_BY_USER)
			return false;
		else
			throw;
	}
	return false;
}

CByteArray APL_PictureEid::getXML(bool bNoHeader)
{
/*
	<biometric>
		<picture type=�jpg�>
			<data encoding=�base64�></data>
			<hash encoding=�base64� method=�md5�></hash>
		</picture>
	</biometric>
*/

	CByteArray xml;
	CByteArray baB64;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<biometric>\n";
	xml+="	<picture type=\"jpg\">\n";
	xml+="		<data encoding=\"base64\">\n";
	if(m_cryptoFwk->b64Encode(getData(),baB64))
		xml+=		baB64;
	xml+="		</data>\n";
	xml+="		<hash encoding=\"base64\" method=\"md5\">\n";
	if(m_cryptoFwk->b64Encode(getHash(),baB64))
		xml+=		baB64;
	xml+="		</hash>\n";
	xml+="	</picture>\n";
	xml+="</biometric>\n";

	return xml;
}

CByteArray APL_PictureEid::getCSV()
{
/*
data;hash;
*/

	CByteArray csv;
	CByteArray baB64;

	if(m_cryptoFwk->b64Encode(getData(),baB64,false))
		csv+=		baB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(getHash(),baB64,false))
		csv+=		baB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_PictureEid::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_PHOTO,m_card->getFilePhoto()->getData().GetBytes(),m_card->getFilePhoto()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const CByteArray& APL_PictureEid::getData()
{
	return m_card->getFilePhoto()->getData();
}

const CByteArray& APL_PictureEid::getHash()
{
	return m_card->getFileID()->getPhotoHash();
}

/*****************************************************************************************
---------------------------------------- APL_DocVersionInfo --------------------------------------------
*****************************************************************************************/
APL_DocVersionInfo::APL_DocVersionInfo(APL_EIDCard *card)
{
	m_card=card;
}

APL_DocVersionInfo::~APL_DocVersionInfo()
{
}

bool APL_DocVersionInfo::isAllowed()
{
	return true;
}

CByteArray APL_DocVersionInfo::getXML(bool bNoHeader)
{
/*
	<scard>
		<serial_nr></serial_nr>
		<component_code></component_code>
		<os_nr></os_nr>
		<os_version></os_version>
		<softmask_nr></softmask_nr>
		<softmask_version></softmask_version>
		<applet_version></applet_version>
		<global_os_version></global_os_version>
		<applet_interface_version></applet_interface_version>
		<PKCS1_support></PKCS1_support>
		<key_exchange_version></key_exchange_version>
		<application_lifecycle></application_lifecycle>
		<graph_perso></graph_perso>
		<elec_perso></elec_perso>
		<elec_perso_interface></elec_perso_interface>
		<files>
			<file_datainfo encoding="base64">
			</file_datainfo>
			<file_tokeninfo encoding="base64">
			</file_tokeninfo>			
		</files>
	</scard>
*/

	CByteArray xml;
	CByteArray b64;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<scard>\n";
	xml+="	<serial_nr>";
	xml+=		getSerialNumber();
	xml+=	"</serial_nr>\n";
	xml+="	<component_code>";
	xml+=		getComponentCode();
	xml+=	"</component_code>\n";
	xml+="	<os_nr>";
	xml+=		getOsNumber();
	xml+=	"</os_nr>\n";
	xml+="	<os_version>";
	xml+=		getOsVersion();
	xml+=	"</os_version>\n";
	xml+="	<softmask_nr>";
	xml+=		getSoftmaskNumber();
	xml+=	"</softmask_nr>\n";
	xml+="	<softmask_version>";
	xml+=		getSoftmaskVersion();
	xml+=	"</softmask_version>\n";
	xml+="	<applet_version>";
	xml+=		getAppletVersion();
	xml+=	"</applet_version>\n";
	xml+="	<global_os_version>";
	xml+=		getGlobalOsVersion();
	xml+=	"</global_os_version>\n";
	xml+="	<applet_interface_version>";
	xml+=		getAppletInterfaceVersion();
	xml+=	"</applet_interface_version>\n";
	xml+="	<PKCS1_support>";
	xml+=		getPKCS1Support();
	xml+=	"</PKCS1_support>\n";
	xml+="	<key_exchange_version>";
	xml+=		getKeyExchangeVersion();
	xml+=	"</key_exchange_version>\n";
	xml+="	<application_lifecycle>";
	xml+=		getAppletLifeCicle();
	xml+=	"</application_lifecycle>\n";
	xml+="	<graph_perso>";
	xml+=		getGraphicalPersonalisation();
	xml+=	"</graph_perso>\n";
	xml+="	<elec_perso>";
	xml+=		getElectricalPersonalisation();
	xml+=	"</elec_perso>\n";
	xml+="	<elec_perso_interface>";
	xml+=		getElectricalPersonalisationInterface();
	xml+=	"</elec_perso_interface>\n";
	xml+="	<files>\n";
	xml+="		<file_datainfo encoding=\"base64\">\n";
	if(m_cryptoFwk->b64Encode(m_card->getFileInfo()->getData(),b64))
		xml+=b64;
	xml+="		</file_datainfo>\n";
	xml+="		<file_tokeninfo encoding=\"base64\">\n";
	if(m_cryptoFwk->b64Encode(m_card->getFileTokenInfo()->getData(),b64))
		xml+=b64;
	xml+="		</file_tokeninfo>\n";
	xml+="	</files>\n";
	xml+="</scard>\n";

	return xml;
}

CByteArray APL_DocVersionInfo::getCSV()
{
/*
serial_nr;component_code;os_nr;os_version;softmask_nr;softmask_version;applet_version;
	global_os_version;applet_interface_version;PKCS1_support;key_exchange_version;
	application_lifecycle;graph_perso;elec_perso;elec_perso_interface;
*/

	CByteArray csv;
	CByteArray b64;

	csv+=getSerialNumber();
	csv+=CSV_SEPARATOR;
	csv+=getComponentCode();
	csv+=CSV_SEPARATOR;
	csv+=getOsNumber();
	csv+=CSV_SEPARATOR;
	csv+=getOsVersion();
	csv+=CSV_SEPARATOR;
	csv+=getSoftmaskNumber();
	csv+=CSV_SEPARATOR;
	csv+=getSoftmaskVersion();
	csv+=CSV_SEPARATOR;
	csv+=getAppletVersion();
	csv+=CSV_SEPARATOR;
	csv+=getGlobalOsVersion();
	csv+=CSV_SEPARATOR;
	csv+=getAppletInterfaceVersion();
	csv+=CSV_SEPARATOR;
	csv+=getPKCS1Support();
	csv+=CSV_SEPARATOR;
	csv+=getKeyExchangeVersion();
	csv+=CSV_SEPARATOR;
	csv+=getAppletLifeCicle();
	csv+=CSV_SEPARATOR;
	csv+=getGraphicalPersonalisation();
	csv+=CSV_SEPARATOR;
	csv+=getElectricalPersonalisation();
	csv+=CSV_SEPARATOR;
	csv+=getElectricalPersonalisationInterface();
	csv+=CSV_SEPARATOR;

	if(m_cryptoFwk->b64Encode(m_card->getFileInfo()->getData(),b64,false))
		csv+=b64;
	csv+=CSV_SEPARATOR;

	if(m_cryptoFwk->b64Encode(m_card->getFileTokenInfo()->getData(),b64,false))
		csv+=b64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_DocVersionInfo::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_CARDINFO,m_card->getFileInfo()->getData().GetBytes(),m_card->getFileInfo()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_TOKENINFO,m_card->getFileTokenInfo()->getData().GetBytes(),m_card->getFileTokenInfo()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const char *APL_DocVersionInfo::getSerialNumber()
{
	return m_card->getFileInfo()->getSerialNumber();
}

const char *APL_DocVersionInfo::getComponentCode()
{
	return m_card->getFileInfo()->getComponentCode();
}

const char *APL_DocVersionInfo::getOsNumber()
{
	return m_card->getFileInfo()->getOsNumber();
}

const char *APL_DocVersionInfo::getOsVersion()
{
	return m_card->getFileInfo()->getOsVersion();
}

const char *APL_DocVersionInfo::getSoftmaskNumber()
{
	return m_card->getFileInfo()->getSoftmaskNumber();
}

const char *APL_DocVersionInfo::getSoftmaskVersion()
{
	return m_card->getFileInfo()->getSoftmaskVersion();
}

const char *APL_DocVersionInfo::getAppletVersion()
{
	return m_card->getFileInfo()->getAppletVersion();
}

const char *APL_DocVersionInfo::getGlobalOsVersion()
{
	return m_card->getFileInfo()->getGlobalOsVersion();
}

const char *APL_DocVersionInfo::getAppletInterfaceVersion()
{
	return m_card->getFileInfo()->getAppletInterfaceVersion();
}

const char *APL_DocVersionInfo::getPKCS1Support()
{
	return m_card->getFileInfo()->getPKCS1Support();
}

const char *APL_DocVersionInfo::getKeyExchangeVersion()
{
	return m_card->getFileInfo()->getKeyExchangeVersion();
}

const char *APL_DocVersionInfo::getAppletLifeCicle()
{
	return m_card->getFileInfo()->getAppletLifeCicle();
}

const char *APL_DocVersionInfo::getGraphicalPersonalisation()
{
	return m_card->getFileTokenInfo()->getGraphicalPersonalisation();
}

const char *APL_DocVersionInfo::getElectricalPersonalisation()
{
	return m_card->getFileTokenInfo()->getElectricalPersonalisation();
}

const char *APL_DocVersionInfo::getElectricalPersonalisationInterface()
{
	return m_card->getFileTokenInfo()->getElectricalPersonalisationInterface();
}

const CByteArray &APL_DocVersionInfo::getSignature()
{
	return m_card->getCardInfoSignature();
}
}
