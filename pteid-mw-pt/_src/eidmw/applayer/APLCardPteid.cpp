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
#include "APLCardPteid.h"
#include "CardPteid.h"
#include "TLVBuffer.h"
#include "APLCertif.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "CardLayer.h"
#include "MiscUtil.h"
#include "StringOps.h"

using namespace std;

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- APL_EIDCard -----------------------------------------
*****************************************************************************************/
APL_AccessWarningLevel APL_EIDCard::m_lWarningLevel=APL_ACCESSWARNINGLEVEL_TO_ASK;

APL_EIDCard::APL_EIDCard(APL_ReaderContext *reader, APL_CardType cardType):APL_SmartCard(reader)
{
	m_cardType = cardType;
	m_docfull=NULL;
	m_CCcustomDoc=NULL;
	m_docid=NULL;
	m_personal=NULL;
	m_address=NULL;
	m_sod=NULL;
	m_docinfo=NULL;

	m_FileTrace=NULL;
	m_FileID=NULL;
	m_FileIDSign=NULL;
	m_FileAddress=NULL;
	m_FileAddressSign=NULL;
	m_FileSod=NULL;
	m_FilePersoData=NULL;
	m_FileTokenInfo=NULL;

	m_cardinfosign=NULL;

	m_fileCertAuthentication=NULL;
	m_fileCertSignature=NULL;
	m_fileCertRootSign=NULL;
	m_fileCertRoot=NULL;
	m_fileCertRootAuth=NULL;
}

APL_EIDCard::~APL_EIDCard()
{
	if(m_docfull)
	{
		delete m_docfull;
		m_docfull=NULL;
	}
	if(m_CCcustomDoc)
	{
		delete m_CCcustomDoc;
		m_CCcustomDoc=NULL;
	}
	if(m_docid)
	{
		delete m_docid;
		m_docid=NULL;
	}
	if(m_personal)
	{
		delete m_personal;
		m_personal=NULL;
	}
	if(m_address)
	{
		delete m_address;
		m_address=NULL;
	}
	if(m_sod)
	{
		delete m_sod;
		m_sod=NULL;
	}

	if(m_docinfo)
	{
		delete m_docinfo;
		m_docinfo=NULL;
	}

	if(m_FileTrace)
	{
		delete m_FileTrace;
		m_FileTrace=NULL;
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

	if(m_FileSod)
	{
		delete m_FileSod;
		m_FileSod=NULL;
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

	if(m_fileCertRootAuth)
	{
		delete m_fileCertRootAuth;
		m_fileCertRootAuth=NULL;
	}

	if(m_fileCertRootSign)
	{
		delete m_fileCertRootSign;
		m_fileCertRootSign=NULL;
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
		if(bRet && getFileTrace()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileID()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileIDSign()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileAddress()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileAddressSign()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
		if(bRet && getFileSod()->getStatus(true)!=CARDFILESTATUS_OK)
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
	else if(strcmp(fileID,PTEID_FILE_TRACE)==0)
	{
		in=parser->getRawDataEid()->traceData;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS)==0)
	{
		in=parser->getRawDataEid()->addrData;
	}
	else if(strcmp(fileID,PTEID_FILE_ADDRESS_SIGN)==0)
	{
		in=parser->getRawDataEid()->addrSigData;
	}
	else if(strcmp(fileID,PTEID_FILE_SOD)==0)
	{
		in=parser->getRawDataEid()->sodData;
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
	else if(strcmp(fileID,PTEID_FILE_SOD)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_SOD);
	}
	else if(strcmp(fileID,PTEID_FILE_CARDINFO)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_CARDINFO);
	}
	else if(strcmp(fileID,PTEID_FILE_TOKENINFO)==0)
	{
		parser->readDataTlv(in,PTEID_TLV_TAG_FILE_TOKENINFO);
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
	else if(strcmp(fileID,PTEID_FILE_SOD)==0)
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
	else if(strcmp(fileID,PTEID_FILE_SOD)==0)
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

APL_EidFile_Trace *APL_EIDCard::getFileTrace()
{
	if(!m_FileTrace)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileTrace)
		{
			m_FileTrace=new APL_EidFile_Trace(this);
		}
	}

	return m_FileTrace;
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

APL_EidFile_Sod *APL_EIDCard::getFileSod()
{
	if(!m_FileSod)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileSod)
		{
			m_FileSod=new APL_EidFile_Sod(this);
		}
	}
	return m_FileSod;
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

APL_CardType APL_EIDCard::getType() const
{
	return m_cardType;
}

bool APL_EIDCard::isTestCard()
{
	APL_Certif *root = NULL;

	bool bOut = false;

	if(root)
		bOut=root->isTest();

	return bOut;
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

			if(!m_fileCertRootAuth)
			{
				m_fileCertRootAuth=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_ROOT_AUTH);
				//If status ok, we add the certificate to the store
				if(m_fileCertRootAuth->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertRootAuth,APL_CERTIF_TYPE_ROOT_AUTH,true,false,m_certificateCount,NULL,NULL)))
						throw CMWEXCEPTION(EIDMW_ERR_CHECK);
					m_certificateCount++;
				}
			}

			if(!m_fileCertRootSign)
			{
				m_fileCertRootSign=new APL_CardFile_Certificate(this,PTEID_FILE_CERT_ROOT_SIGN);
				//If status ok, we add the certificate to the store
				if(m_fileCertRootSign->getStatus(true)==CARDFILESTATUS_OK)
				{
					if(NULL == (getCertificates()->addCert(m_fileCertRootSign,APL_CERTIF_TYPE_ROOT_SIGN,true,false,m_certificateCount,NULL,NULL)))
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
	case APL_DOCTYPE_ADDRESS:
		return getAddr();
	case APL_DOCTYPE_SOD:
		return getSod();
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


APL_CCXML_Doc& APL_EIDCard::getXmlCCDoc(APL_XmlUserRequestedInfo& userRequestedInfo){
	if(!m_CCcustomDoc)
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
			if(!m_CCcustomDoc)
			{
				m_CCcustomDoc=new APL_CCXML_Doc(this, userRequestedInfo);
			}
		}

		return *m_CCcustomDoc;
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


APL_PersonalNotesEId& APL_EIDCard::getPersonalNotes()
{
	if(!m_personal)
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
			if(!m_personal)
			{
				m_personal=new APL_PersonalNotesEId(this);
			}
		}

		return *m_personal;
}


APL_AddrEId& APL_EIDCard::getAddr()
{
	if(!m_address)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_address)
		{
			m_address=new APL_AddrEId(this);
		}
	}

	return *m_address;
}

APL_SodEid& APL_EIDCard::getSod()
{
	if(!m_sod)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_sod)
		{
			m_sod=new APL_SodEid(this);
		}
	}

	return *m_sod;
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
	case APL_RAWDATA_TRACE:
		return getRawData_Trace();
	case APL_RAWDATA_ADDR:
		return getRawData_Addr();
	case APL_RAWDATA_ADDR_SIG:
		return getRawData_AddrSig();
	case APL_RAWDATA_SOD:
		return getRawData_Sod();
	case APL_RAWDATA_CARD_INFO:
		return getRawData_CardInfo();
	case APL_RAWDATA_TOKEN_INFO:
		return getRawData_TokenInfo();
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

const CByteArray& APL_EIDCard::getRawData_Trace()
{
	return getFileTrace()->getData();
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

const CByteArray& APL_EIDCard::getRawData_Sod()
{
	return getFileSod()->getData();
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
	// Fix this: This modification hides the accept window.
	// Application will access to the card always without user interaction
	setWarningLevel(APL_ACCESSWARNINGLEVEL_ACCEPTED);
	/*
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
	*/
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
			//&& m_card->getFileAddress()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFileSod()->getStatus(true)==CARDFILESTATUS_OK)
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
	case APL_CARDTYPE_PTEID_IAS07:
	case APL_CARDTYPE_PTEID_IAS101:
		xml+=CARDTYPE_NAME_PTEID_EID;
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	xml+=   "</card_type>\n";
	xml+=m_card->getID().getXML(true);
	xml+=m_card->getSod().getXML(true);
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
	case APL_CARDTYPE_PTEID_IAS07:
	case APL_CARDTYPE_PTEID_IAS101:
		csv+=CARDTYPE_NAME_PTEID_EID;
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	csv+=CSV_SEPARATOR;
	csv+=m_card->getID().getCSV();
	csv+=m_card->getSod().getCSV();
	csv+=m_card->getDocInfo().getCSV();

	CByteArray baFileB64;

	if(m_cryptoFwk->b64Encode(m_card->getChallenge(),baFileB64,false))
		csv+=		baFileB64;
	csv+=CSV_SEPARATOR;

	if(m_cryptoFwk->b64Encode(m_card->getChallengeResponse(),baFileB64,false))
		csv+=		baFileB64;
	csv+=CSV_SEPARATOR;

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
	case APL_CARDTYPE_PTEID_IAS07:
	case APL_CARDTYPE_PTEID_IAS101:
		baCardType.Append(CARDTYPE_NAME_PTEID_EID);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
	tlv.SetTagData(PTEID_TLV_TAG_CARDTYPE,baCardType.GetBytes(),baCardType.Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_CHALLENGE,m_card->getChallenge().GetBytes(),m_card->getChallenge().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_CHALLENGE_RESPONSE,m_card->getChallengeResponse().GetBytes(),m_card->getChallengeResponse().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	ba.Append(m_card->getID().getTLV());
	ba.Append(m_card->getSod().getTLV());
	ba.Append(m_card->getDocInfo().getTLV());
	ba.Append(m_card->getCertificates()->getTLV());
	ba.Append(m_card->getPins()->getTLV());

	return ba;
}

/*****************************************************************************************
---------------------------------------- APL_CCXML_Doc -------------------------------------------
*****************************************************************************************/
APL_CCXML_Doc::APL_CCXML_Doc(APL_EIDCard *card, APL_XmlUserRequestedInfo&  xmlUserRequestedInfo)
{
	m_card=card;
	m_xmlUserRequestedInfo = &xmlUserRequestedInfo;
}

APL_CCXML_Doc::~APL_CCXML_Doc()
{
}

bool APL_CCXML_Doc::isAllowed()
{
	try
	{
		if(m_card->getFileID()->getStatus(true)==CARDFILESTATUS_OK
			//&& m_card->getFileAddress()->getStatus(true)==CARDFILESTATUS_OK
			&& m_card->getFileSod()->getStatus(true)==CARDFILESTATUS_OK)
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

CByteArray APL_CCXML_Doc::getXML(bool bNoHeader)
{
	CByteArray xml;
	string *ts, *sn, *sa;
	string rootATTRS;

	ts = m_xmlUserRequestedInfo->getTimeStamp();
	sn = m_xmlUserRequestedInfo->getServerName();
	sa = m_xmlUserRequestedInfo->getServerAddress();

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	if (ts && sn && sa){
		xml+=XML_ROOT_S;
		XML_ATTRIBUTE(xml,XML_ROOT_ELEMENT_ATTR_TIMESTAMP,ts->c_str());
		XML_ATTRIBUTE(xml,XML_ROOT_ELEMENT_ATTR_SERVERNAME,sn->c_str());
		XML_ATTRIBUTE(xml,XML_ROOT_ELEMENT_ATTR_SERVERADDRESS,sa->c_str());
		xml+=XML_ROOT_E;
	} else
		xml+=XML_OPEN_TAG_NEWLINE(XML_ROOT_ELEMENT);
	xml+=m_card->getID().getXML(true,*m_xmlUserRequestedInfo);
	xml+=m_card->getAddr().getXML(true, *m_xmlUserRequestedInfo);
	xml+=m_card->getPersonalNotes().getXML(true, *m_xmlUserRequestedInfo);
	xml+=XML_CLOSE_TAG(XML_ROOT_ELEMENT);

	return xml;
}

CByteArray APL_CCXML_Doc::getCSV(){
	CByteArray cb;

	return cb;
}

CByteArray APL_CCXML_Doc::getTLV(){
	CByteArray cb;

	return cb;
}


/*****************************************************************************************
-------------------------------- APL_XmlUserRequestedInfo ---------------------------------------
*****************************************************************************************/
APL_XmlUserRequestedInfo::APL_XmlUserRequestedInfo()
{
	xmlSet = new set<enum XMLUserData>;
	_serverName = NULL;
	_serverAddress = NULL;
	_timeStamp = NULL;
}

APL_XmlUserRequestedInfo::APL_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress)
{
	xmlSet = new set<enum XMLUserData>;
	_timeStamp = new string(timeStamp);
	_serverName = new string(serverName);
	_serverAddress = new string(serverAddress);
}

APL_XmlUserRequestedInfo::~APL_XmlUserRequestedInfo()
{
	if (xmlSet)
		delete xmlSet;
	if (_timeStamp)
		delete _timeStamp;
	if (_serverAddress)
		delete _serverAddress;
	if (_serverName)
		delete _serverName;
}

void APL_XmlUserRequestedInfo::add(XMLUserData xmlUData)
{
	xmlSet->insert(xmlUData);
}

bool APL_XmlUserRequestedInfo::contains(XMLUserData xmlUData)
{
	return (xmlSet->find(xmlUData) != xmlSet->end());
}

void APL_XmlUserRequestedInfo::remove(XMLUserData xmlUData)
{
	xmlSet->erase(xmlUData);
}

bool APL_XmlUserRequestedInfo::checkAndRemove(XMLUserData xmlUData)
{
	bool contains;

	contains = (xmlSet->find(xmlUData) != xmlSet->end());
	if (!contains)
		xmlSet->erase(xmlUData);

	return contains;
}

bool APL_XmlUserRequestedInfo::isEmpty(){
	return xmlSet->empty();
}

std::string* APL_XmlUserRequestedInfo::getTimeStamp(){
	return _timeStamp;
}

std::string* APL_XmlUserRequestedInfo::getServerName(){
	return _serverName;
}

std::string* APL_XmlUserRequestedInfo::getServerAddress(){
	return _serverAddress;
}


/*****************************************************************************************
---------------------------------------- APL_DocEId ---------------------------------------------
*****************************************************************************************/
APL_DocEId::APL_DocEId(APL_EIDCard *card)
{
	m_card=card;

	m_FirstName.clear();

	_xmlUInfo = NULL;
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

CByteArray APL_DocEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo){

	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_DocEId::getXML(bool bNoHeader)
{
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
	string temp;

	// provide all the id fields
	if(_xmlUInfo->isEmpty())
		_xmlUInfo = NULL;

	// photo
	if (!_xmlUInfo || _xmlUInfo->contains(XML_PHOTO)){
		photo = getPhotoObj()->getPhotoPNG();
		m_cryptoFwk->b64Encode(*photo,b64photo);
		BUILD_XML_ELEMENT(xml, XML_PHOTO_ELEMENT, b64photo);
	}

	// basicInformation
	if (!_xmlUInfo || _xmlUInfo->contains(XML_NAME)){
		string s;
		s+= getGivenName();
		s+=" ";
		s+=getSurname();
		BUILD_XML_ELEMENT(basicInfo, XML_NAME_ELEMENT, s.c_str());
		addBasicInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_GIVEN_NAME)){
		BUILD_XML_ELEMENT(basicInfo,XML_GIVEN_NAME_ELEMENT,getGivenName());
		addBasicInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_SURNAME)){
		BUILD_XML_ELEMENT(basicInfo,XML_SURNAME_ELEMENT,getSurname());
		addBasicInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_NIC)){
		BUILD_XML_ELEMENT(basicInfo,XML_NIC_ELEMENT,getCivilianIdNumber());
		addBasicInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_EXPIRY_DATE)){
		BUILD_XML_ELEMENT(basicInfo,XML_EXPIRY_DATE_ELEMENT, getValidityEndDate());
		addBasicInfo = true;
	}
	if (addBasicInfo){
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_BASIC_INFO_ELEMENT, basicInfo);
	}

	// CivilInformation
	if (!_xmlUInfo || _xmlUInfo->contains(XML_GENDER)){
		BUILD_XML_ELEMENT(civilInfo, XML_GENDER_ELEMENT, getGender());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_HEIGHT)){
		BUILD_XML_ELEMENT(civilInfo, XML_HEIGHT_ELEMENT, getHeight());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_NATIONALITY)){
		BUILD_XML_ELEMENT(civilInfo, XML_NATIONALITY_ELEMENT, getNationality());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_DATE_OF_BIRTH)){
		BUILD_XML_ELEMENT(civilInfo, XML_DATE_OF_BIRTH_ELEMENT, getDateOfBirth());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_GIVEN_NAME_FATHER)){
		BUILD_XML_ELEMENT(civilInfo, XML_GIVEN_NAME_FATHER_ELEMENT, getGivenNameFather());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_SURNAME_FATHER)){
		BUILD_XML_ELEMENT(civilInfo, XML_SURNAME_FATHER_ELEMENT, getSurnameFather());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_GIVEN_NAME_MOTHER)){
		BUILD_XML_ELEMENT(civilInfo, XML_GIVEN_NAME_MOTHER_ELEMENT, getGivenNameMother());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_SURNAME_MOTHER)){
		BUILD_XML_ELEMENT(civilInfo, XML_SURNAME_MOTHER_ELEMENT, getSurnameMother());
		addCivilInfo = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_ACCIDENTAL_INDICATIONS)){
		BUILD_XML_ELEMENT(civilInfo, XML_ACCIDENTAL_INDICATIONS_ELEMENT, getAccidentalIndications());
		addCivilInfo = true;
	}
	if (addCivilInfo){
		BUILD_XML_ELEMENT_NEWLINE(xml,XML_CIVIL_INFO_ELEMENT, civilInfo);
	}

	// IdentificationNumbers
	if (!_xmlUInfo || _xmlUInfo->contains(XML_DOCUMENT_NO)){
		BUILD_XML_ELEMENT(idNum, XML_DOCUMENT_NO_ELEMENT, getDocumentNumber());
		addIdNum = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_TAX_NO)){
		BUILD_XML_ELEMENT(idNum, XML_TAX_NO_ELEMENT, getTaxNo());
		addIdNum = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_SOCIAL_SECURITY_NO)){
		BUILD_XML_ELEMENT(idNum, XML_SOCIAL_SECURITY_NO_ELEMENT, getTaxNo());
		addIdNum = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_HEALTH_NO)){
		BUILD_XML_ELEMENT(idNum, XML_HEALTH_NO_ELEMENT, getHealthNumber());
		addIdNum = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_MRZ1)){
		temp = getMRZ1();
		replace(temp,XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ1_ELEMENT, temp);
		addIdNum = true;
	}

	if (!_xmlUInfo || _xmlUInfo->contains(XML_MRZ2)){
		temp = getMRZ2();
		replace(temp,XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ2_ELEMENT, temp);
		addIdNum = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_MRZ3)){
		temp = getMRZ3();
		replace(temp,XML_ESCAPE_LT);
		BUILD_XML_ELEMENT(idNum, XML_MRZ3_ELEMENT, temp);
		addIdNum = true;
	}
	if (addIdNum){
		BUILD_XML_ELEMENT_NEWLINE(xml,XML_IDENTIFICATION_NUMBERS_ELEMENT, idNum);
	}

	// CardValues
	if (!_xmlUInfo || _xmlUInfo->contains(XML_CARD_VERSION)){
		BUILD_XML_ELEMENT(cardValues, XML_CARD_VERSION_ELEMENT, getDocumentVersion());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_CARD_NUMBER_PAN)){
		BUILD_XML_ELEMENT(cardValues, XML_CARD_NUMBER_PAN_ELEMENT, getDocumentPAN());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_ISSUING_DATE)){
		BUILD_XML_ELEMENT(cardValues, XML_ISSUING_DATE_ELEMENT, getValidityBeginDate());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_ISSUING_ENTITY)){
		BUILD_XML_ELEMENT(cardValues, XML_ISSUING_ENTITY_ELEMENT, getIssuingEntity());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_DOCUMENT_TYPE)){
		BUILD_XML_ELEMENT(cardValues, XML_DOCUMENT_TYPE_ELEMENT, getDocumentType());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_LOCAL_OF_REQUEST)){
		BUILD_XML_ELEMENT(cardValues, XML_LOCAL_OF_REQUEST_ELEMENT, getLocalofRequest());
		addCardValues = true;
	}
	if (!_xmlUInfo || _xmlUInfo->contains(XML_VERSION)){
		BUILD_XML_ELEMENT(cardValues, XML_VERSION_ELEMENT, "0");
		addCardValues = true;
	}
	if (addCardValues){
		BUILD_XML_ELEMENT_NEWLINE(xml, XML_CARD_VALUES_ELEMENT, cardValues);
	};

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
	csv+=getGivenName();
	csv+=CSV_SEPARATOR;
	csv+=getSurname();
	csv+=CSV_SEPARATOR;
	csv+=getGender();
	csv+=CSV_SEPARATOR;
	csv+=getDateOfBirth();
	csv+=CSV_SEPARATOR;
	csv+=getLocationOfBirth();
	csv+=CSV_SEPARATOR;
	csv+=getNationality();
	csv+=CSV_SEPARATOR;
	csv+=getCivilianIdNumber();
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
	csv+=getDocumentPAN();
	csv+=CSV_SEPARATOR;
	csv+=getValidityBeginDate();
	csv+=CSV_SEPARATOR;
	csv+=getValidityEndDate();
	csv+=CSV_SEPARATOR;

	// MARTINHO: fica aqui para n ficar esquecido também, provavelmente nem o CSV vai ser utilizado.
	csv+=getMRZ1();
	csv+=CSV_SEPARATOR;
	csv+=getMRZ2();
	csv+=CSV_SEPARATOR;
	csv+=getMRZ3();
	csv+=CSV_SEPARATOR;


	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileID()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(m_card->getFileIDSign()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_DocEId::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_ID,m_card->getFileID()->getData().GetBytes(),m_card->getFileID()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_IDSIGN,m_card->getFileIDSign()->getData().GetBytes(),m_card->getFileIDSign()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const char *APL_DocEId::getValidation()
{
	return m_card->getFileTrace()->getValidation();
}

const char *APL_DocEId::getDocumentVersion()
{
	return m_card->getFileID()->getDocumentVersion();
}

const char *APL_DocEId::getDocumentType()
{
	return m_card->getFileID()->getDocumentType();
}

const char *APL_DocEId::getCountry()
{
	return m_card->getFileID()->getCountry();
}

const char *APL_DocEId::getGivenName()
{
	return m_card->getFileID()->getGivenName();
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

const char *APL_DocEId::getNationality()
{
	return m_card->getFileID()->getNationality();
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

const char *APL_DocEId::getDocumentPAN()
{
	return m_card->getFileID()->getDocumentPAN();
}

const char *APL_DocEId::getValidityBeginDate()
{
	return m_card->getFileID()->getValidityBeginDate();
}

const char *APL_DocEId::getValidityEndDate()
{
	return m_card->getFileID()->getValidityEndDate();
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

const char *APL_DocEId::getSocialSecurityNumber()
{
	return m_card->getFileID()->getSocialSecurityNumber();
}

const char *APL_DocEId::getHealthNumber()
{
	return m_card->getFileID()->getHealthNumber();
}

const char *APL_DocEId::getIssuingEntity()
{
	return m_card->getFileID()->getIssuingEntity();
}

const char *APL_DocEId::getLocalofRequest()
{
	return m_card->getFileID()->getLocalofRequest();
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

const char *APL_DocEId::getParents()
{
	return m_card->getFileID()->getParents();
}

PhotoPteid *APL_DocEId::getPhotoObj()
{
	return m_card->getFileID()->getPhotoObj();
}

APLCardAuthenticationKey *APL_DocEId::getCardAuthKeyObj(){
	return m_card->getFileID()->getCardAuthKeyObj();
}

const char *APL_DocEId::getMRZ1(){
	return m_card->getFileID()->getMRZ1();
}

const char *APL_DocEId::getMRZ2(){
	return m_card->getFileID()->getMRZ2();
}

const char *APL_DocEId::getMRZ3(){
	return m_card->getFileID()->getMRZ3();
}

const char *APL_DocEId::getAccidentalIndications(){
	return m_card->getFileID()->getAccidentalIndications();
}

const char *APL_DocEId::getCivilianIdNumber(){
	return m_card->getFileID()->getCivilianIdNumber();
}

const char *APL_DocEId::getPersoData()
{
	return m_card->getFilePersoData()->getPersoData();
}

/*****************************************************************************************
----------------------------------- APL_PersonalNotesEid ----------------------------------------
*****************************************************************************************/
APL_PersonalNotesEId::APL_PersonalNotesEId(APL_EIDCard *card)
{
	m_card=card;
}

APL_PersonalNotesEId::~APL_PersonalNotesEId()
{
}

bool APL_PersonalNotesEId::isAllowed()
{
	try
	{
		if(m_card->getFilePersoData()->getStatus(true)==CARDFILESTATUS_OK)
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

CByteArray APL_PersonalNotesEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo){
	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_PersonalNotesEId::getXML(bool bNoHeader)
{
	CByteArray xml;
	string str;


	if (_xmlUInfo->isEmpty())
		_xmlUInfo = NULL;

	if (!_xmlUInfo || _xmlUInfo->contains(XML_PERSONAL_NOTES)){
		str = getPersonalNotes();
		replace(str,XML_ESCAPE_AMP);
		replace(str,XML_ESCAPE_APOS);
		replace(str,XML_ESCAPE_GT);
		replace(str,XML_ESCAPE_LT);
		replace(str,XML_ESCAPE_QUOTE);

		BUILD_XML_ELEMENT(xml, XML_PERSONAL_NOTES_ELEMENT, str.c_str());
	}

	return xml;
}

CByteArray APL_PersonalNotesEId::getCSV()
{
	CByteArray csv;

	return csv;
}

CByteArray APL_PersonalNotesEId::getTLV()
{
	CTLVBuffer tlv;
	CByteArray ba;

	return ba;
}

const char *APL_PersonalNotesEId::getPersonalNotes()
{
	return m_card->getFilePersoData()->getPersoData();
}

/*****************************************************************************************
---------------------------------------- APL_AddrEId ---------------------------------------------
*****************************************************************************************/
APL_AddrEId::APL_AddrEId(APL_EIDCard *card)
{
	m_card=card;
}

APL_AddrEId::~APL_AddrEId()
{
}

bool APL_AddrEId::isAllowed()
{
	try
	{
		if(m_card->getFileAddress()->getStatus(true)==CARDFILESTATUS_OK)
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

CByteArray APL_AddrEId::getXML(bool bNoHeader, APL_XmlUserRequestedInfo &xmlUInfo){
	CByteArray ca;

	_xmlUInfo = &xmlUInfo;
	ca = getXML(bNoHeader);
	_xmlUInfo = NULL;

	return ca;
}

CByteArray APL_AddrEId::getXML(bool bNoHeader)
{
	CByteArray xml;
	CByteArray address;
	bool addAddress = false;

	// provide all the address fields
	if(_xmlUInfo->isEmpty())
		_xmlUInfo = NULL;

	if (isNationalAddress()){
		if (!_xmlUInfo || _xmlUInfo->contains(XML_DISTRICT)){
			BUILD_XML_ELEMENT(address, XML_DISTRICT_ELEMENT, getDistrict());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_MUNICIPALITY)){
			BUILD_XML_ELEMENT(address, XML_MUNICIPALITY_ELEMENT, getMunicipality());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_CIVIL_PARISH)){
			BUILD_XML_ELEMENT(address, XML_CIVIL_PARISH_ELEMENT, getCivilParish());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_ABBR_STREET_TYPE)){
			BUILD_XML_ELEMENT(address, XML_ABBR_STREET_TYPE_ELEMENT, getAbbrStreetType());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_STREET_TYPE)){
			BUILD_XML_ELEMENT(address, XML_STREET_TYPE_ELEMENT, getStreetType());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_STREET_NAME)){
			BUILD_XML_ELEMENT(address, XML_STREET_NAME_ELEMENT, getStreetName());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_ABBR_BUILDING_TYPE)){
			BUILD_XML_ELEMENT(address, XML_ABBR_BUILDING_TYPE_ELEMENT, getAbbrBuildingType());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_BUILDING_TYPE)){
			BUILD_XML_ELEMENT(address, XML_BUILDING_TYPE_ELEMENT, getBuildingType());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_DOOR_NO)){
			BUILD_XML_ELEMENT(address, XML_DOOR_NO_ELEMENT, getDoorNo());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FLOOR)){
			BUILD_XML_ELEMENT(address, XML_FLOOR_ELEMENT, getFloor());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_SIDE)){
			BUILD_XML_ELEMENT(address, XML_SIDE_ELEMENT, getSide());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_PLACE)){
			BUILD_XML_ELEMENT(address, XML_PLACE_ELEMENT, getPlace());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_LOCALITY)){
			BUILD_XML_ELEMENT(address, XML_LOCALITY_ELEMENT, getLocality());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_ZIP4)){
			BUILD_XML_ELEMENT(address, XML_ZIP4_ELEMENT, getZip4());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_ZIP3)){
			BUILD_XML_ELEMENT(address, XML_ZIP3_ELEMENT, getZip3());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_POSTAL_LOCALITY)){
			BUILD_XML_ELEMENT(address, XML_POSTAL_LOCALITY_ELEMENT, getPostalLocality());
			addAddress = true;
		}
		if (addAddress){
			BUILD_XML_ELEMENT_NEWLINE(xml,XML_ADDRESS_ELEMENT, address);
		}
	} else {
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_COUNTRY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_COUNTRY_ELEMENT, getForeignCountry());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_ADDRESS)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_ADDRESS_ELEMENT, getForeignAddress());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_CITY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_CITY_ELEMENT, getForeignCity());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_REGION)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_REGION_ELEMENT, getForeignRegion());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_LOCALITY)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_LOCALITY_ELEMENT, getForeignLocality());
			addAddress = true;
		}
		if (!_xmlUInfo || _xmlUInfo->contains(XML_FOREIGN_POSTAL_CODE)){
			BUILD_XML_ELEMENT(address, XML_FOREIGN_POSTAL_CODE_ELEMENT, getForeignPostalCode());
			addAddress = true;
		}
	}

	return xml;
}

CByteArray APL_AddrEId::getCSV()
{
	CByteArray csv;

	/*
	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileAddress()->getData(),baFileB64,false))
		csv+=baFileB64;
	*/
	return csv;
}

CByteArray APL_AddrEId::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_ADDR,m_card->getFileAddress()->getData().GetBytes(),m_card->getFileAddress()->getData().Size());
	tlv.SetTagData(PTEID_TLV_TAG_FILE_ADDRSIGN,m_card->getFileAddressSign()->getData().GetBytes(),m_card->getFileAddressSign()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const char *APL_AddrEId::getMunicipality()
{
	return m_card->getFileAddress()->getMunicipality();
}

const char *APL_AddrEId::getMunicipalityCode()
{
	return m_card->getFileAddress()->getMunicipalityCode();
}

const char *APL_AddrEId::getPlace()
{
	return m_card->getFileAddress()->getPlace();
}

const char *APL_AddrEId::getCivilParish()
{
	return m_card->getFileAddress()->getCivilParish();
}

const char *APL_AddrEId::getCivilParishCode()
{
	return m_card->getFileAddress()->getCivilParishCode();
}

const char *APL_AddrEId::getStreetName()
{
	return m_card->getFileAddress()->getStreetName();
}

const char *APL_AddrEId::getAbbrStreetType()
{
	return m_card->getFileAddress()->getAbbrStreetType();
}

const char *APL_AddrEId::getStreetType()
{
	return m_card->getFileAddress()->getStreetType();
}

const char *APL_AddrEId::getAbbrBuildingType()
{
	return m_card->getFileAddress()->getAbbrBuildingType();
}

const char *APL_AddrEId::getBuildingType()
{
	return m_card->getFileAddress()->getBuildingType();
}

const char *APL_AddrEId::getDoorNo()
{
	return m_card->getFileAddress()->getDoorNo();
}

const char *APL_AddrEId::getFloor()
{
	return m_card->getFileAddress()->getFloor();
}

const char *APL_AddrEId::getSide()
{
	return m_card->getFileAddress()->getSide();
}

const char *APL_AddrEId::getLocality()
{
	return m_card->getFileAddress()->getLocality();
}

const char *APL_AddrEId::getZip4()
{
	return m_card->getFileAddress()->getZip4();
}

const char *APL_AddrEId::getZip3()
{
	return m_card->getFileAddress()->getZip3();
}

const char *APL_AddrEId::getPostalLocality()
{
	return m_card->getFileAddress()->getPostalLocality();
}

const char *APL_AddrEId::getGeneratedAddressCode()
{
	return m_card->getFileAddress()->getGeneratedAddressCode();
}

const char *APL_AddrEId::getDistrict()
{
	return m_card->getFileAddress()->getDistrict();
}

const char *APL_AddrEId::getDistrictCode()
{
	return m_card->getFileAddress()->getDistrictCode();
}

const char *APL_AddrEId::getCountryCode()
{
	return m_card->getFileAddress()->getCountryCode();
}

bool APL_AddrEId::isNationalAddress()
{
	return m_card->getFileAddress()->isNationalAddress();
}

const char *APL_AddrEId::getForeignCountry()
{
	return m_card->getFileAddress()->getForeignCountry();
}

const char *APL_AddrEId::getForeignAddress()
{
	return m_card->getFileAddress()->getForeignAddress();
}

const char *APL_AddrEId::getForeignCity()
{
	return m_card->getFileAddress()->getForeignCity();
}

const char *APL_AddrEId::getForeignRegion()
{
	return m_card->getFileAddress()->getForeignRegion();
}

const char *APL_AddrEId::getForeignLocality()
{
	return m_card->getFileAddress()->getForeignLocality();
}

const char *APL_AddrEId::getForeignPostalCode()
{
	return m_card->getFileAddress()->getForeignPostalCode();
}

/*****************************************************************************************
---------------------------------------- APL_SodEid -----------------------------------------
*****************************************************************************************/
APL_SodEid::APL_SodEid(APL_EIDCard *card)
{	
	m_card=card;
}

APL_SodEid::~APL_SodEid()
{
}

bool APL_SodEid::isAllowed()
{
	try
	{
		if(m_card->getFileSod()->getStatus(true)==CARDFILESTATUS_OK
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

CByteArray APL_SodEid::getXML(bool bNoHeader)
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
	/*if(m_cryptoFwk->b64Encode(getHash(),baB64))
		xml+=		baB64;*/
	xml+="		</hash>\n";
	xml+="	</picture>\n";
	xml+="</biometric>\n";

	return xml;
}

CByteArray APL_SodEid::getCSV()
{
/*
data;hash;
*/

	CByteArray csv;
	CByteArray baB64;

	if(m_cryptoFwk->b64Encode(getData(),baB64,false))
		csv+=		baB64;
	csv+=CSV_SEPARATOR;
	/*if(m_cryptoFwk->b64Encode(getHash(),baB64,false))
		csv+=		baB64;*/
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_SodEid::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(PTEID_TLV_TAG_FILE_SOD,m_card->getFileSod()->getData().GetBytes(),m_card->getFileSod()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const CByteArray& APL_SodEid::getData()
{
	return m_card->getFileSod()->getData();
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

bool APL_DocVersionInfo::isActive()
{
	return m_card->getFileTrace()->isActive();
}

const char *APL_DocVersionInfo::getSerialNumber()
{
	return m_card->getFileTokenInfo()->getTokenSerialNumber();
}

const char * APL_DocVersionInfo::getTokenLabel(){
	return m_card->getFileTokenInfo()->getTokenLabel();
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
