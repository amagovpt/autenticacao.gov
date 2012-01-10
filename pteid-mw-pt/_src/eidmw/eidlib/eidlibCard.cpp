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
#include "eidlib.h"

#include "eidlibException.h"
#include "InternalUtil.h"
#include "MWException.h"
#include "eidErrors.h"

#include "APLCard.h"
#include "APLCardPteid.h"
#include "APLCrypto.h"
#include "APLCertif.h"

#include "ByteArray.h"

//UNIQUE INDEX FOR RETRIEVING OBJECT
#define INCLUDE_OBJECT_DOCEID			1
#define INCLUDE_OBJECT_SODID			2
#define INCLUDE_OBJECT_PINS				3
#define INCLUDE_OBJECT_CERTIFICATES		4
#define INCLUDE_OBJECT_DOCINFO			5
#define INCLUDE_OBJECT_FULLDOC			6
#define INCLUDE_OBJECT_CHALLENGE		7
#define INCLUDE_OBJECT_RESPONSE			8
#define INCLUDE_OBJECT_CUSTOMDOC		9

#define INCLUDE_OBJECT_RAWDATA_ID			21
#define INCLUDE_OBJECT_RAWDATA_ID_SIG		22
#define INCLUDE_OBJECT_RAWDATA_ADDR			23
#define INCLUDE_OBJECT_RAWDATA_ADDR_SIG		24
#define INCLUDE_OBJECT_RAWDATA_SOD			25
#define INCLUDE_OBJECT_RAWDATA_CARD_INFO	26
#define INCLUDE_OBJECT_RAWDATA_TOKEN_INFO	27
#define INCLUDE_OBJECT_RAWDATA_CHALLENGE	29
#define INCLUDE_OBJECT_RAWDATA_RESPONSE		30
#define INCLUDE_OBJECT_RAWDATA_PERSO_DATA   31
#define INCLUDE_OBJECT_RAWDATA_TRACE		32

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- Card --------------------------------------------
*****************************************************************************************/
PTEID_Card::PTEID_Card(const SDK_Context *context,APL_Card *impl):PTEID_Object(context,impl)
{
}

PTEID_Card::~PTEID_Card()
{
}

PTEID_CardType PTEID_Card::getType()
{
	PTEID_CardType out = PTEID_CARDTYPE_UNKNOWN;
	BEGIN_TRY_CATCH

	APL_Card *pcard=static_cast<APL_Card *>(m_impl);
	out = ConvertCardType(pcard->getType());
	
	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::sendAPDU(const PTEID_ByteArray& cmd)
{
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard=static_cast<APL_Card *>(m_impl);

	CByteArray apdu(cmd.GetBytes(),cmd.Size());
	CByteArray result=pcard->sendAPDU(apdu);
	out.Append(result.GetBytes(),result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::Sign(const PTEID_ByteArray& data)
{
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard=static_cast<APL_Card *>(m_impl);

	CByteArray cData(data.GetBytes(),data.Size());
	CByteArray result=pcard->Sign(cData);
	out.Append(result.GetBytes(),result.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_Card::readFile(const char *fileID, unsigned long  ulOffset, unsigned long  ulMaxLength)
{
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard=static_cast<APL_Card *>(m_impl);

	CByteArray oData;

	pcard->readFile(fileID, oData, ulOffset,ulMaxLength);

	out.Append(oData.GetBytes(),oData.Size());

	END_TRY_CATCH

	return out;
}

bool PTEID_Card::writeFile(const char *fileID,const PTEID_ByteArray& oData,unsigned long ulOffset)
{
	bool out = false;
	BEGIN_TRY_CATCH

	APL_Card *pcard=static_cast<APL_Card *>(m_impl);

	CByteArray bytearray(oData.GetBytes(),oData.Size());
	out = pcard->writeFile(fileID,bytearray,ulOffset);
	
	END_TRY_CATCH
	return out;
}

/*****************************************************************************************
---------------------------------------- PTEID_MemoryCard --------------------------------------
*****************************************************************************************/
PTEID_MemoryCard::PTEID_MemoryCard(const SDK_Context *context,APL_Card *impl):PTEID_Card(context,impl)
{
}

PTEID_MemoryCard::~PTEID_MemoryCard()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_SmartCard --------------------------------------
*****************************************************************************************/
PTEID_SmartCard::PTEID_SmartCard(const SDK_Context *context,APL_Card *impl):PTEID_Card(context,impl)
{
}

PTEID_SmartCard::~PTEID_SmartCard()
{
}

void PTEID_SmartCard::selectApplication(const PTEID_ByteArray &applicationId)
{
	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray(applicationId.GetBytes(),applicationId.Size());

	pcard->selectApplication(bytearray);
	
	END_TRY_CATCH
}

PTEID_ByteArray PTEID_SmartCard::sendAPDU(const PTEID_ByteArray& cmd,PTEID_Pin *pin,const char *csPinCode)
{
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	APL_Pin *pimplPin=NULL;

	if(pin)
		pimplPin=pcard->getPins()->getPinByNumber(pin->getIndex());

	CByteArray apdu(cmd.GetBytes(),cmd.Size());

	CByteArray result=pcard->sendAPDU(apdu,pimplPin,csPinCode);

	out.Append(result.GetBytes(),result.Size());
	
	END_TRY_CATCH

	return out;
}

long PTEID_SmartCard::readFile(const char *fileID, PTEID_ByteArray &in,PTEID_Pin *pin,const char *csPinCode)
{
	long out = 0;	

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray;

	APL_Pin *pimplPin=NULL;

	if(pin)
		pimplPin=pcard->getPins()->getPinByNumber(pin->getIndex());

	out=pcard->readFile(fileID,bytearray,pimplPin,csPinCode);
	in=bytearray;
	
	END_TRY_CATCH

	return out;
}

bool PTEID_SmartCard::writeFile(const char *fileID,const PTEID_ByteArray &baOut,PTEID_Pin *pin,const char *csPinCode)
{
	bool out = false;	

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	CByteArray bytearray(baOut.GetBytes(),baOut.Size());

	APL_Pin *pimplPin=NULL;

	if(pin)
		pimplPin=pcard->getPins()->getPinByNumber(pin->getIndex());

	out=pcard->writeFile(fileID,bytearray,pimplPin,csPinCode);
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_SmartCard::pinCount() 
{ 
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);
	out = pcard->pinCount();
	
	END_TRY_CATCH

	return out;
}

PTEID_Pins& PTEID_SmartCard::getPins()
{
	PTEID_Pins *out = NULL;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	out=dynamic_cast<PTEID_Pins *>(getObject(INCLUDE_OBJECT_PINS));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//ppins=dynamic_cast<PTEID_Pins *>(getObject(INCLUDE_OBJECT_PINS));
		//if(!ppins)
		//{
			out = new PTEID_Pins(m_context,pcard->getPins());
			if(out)
				m_objects[INCLUDE_OBJECT_PINS]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

unsigned long PTEID_SmartCard::certificateCount()
{
	unsigned long out = 0;
	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);
	out = pcard->certificateCount();
	
	END_TRY_CATCH
	
	return out;
}

PTEID_Certificates& PTEID_SmartCard::getCertificates()
{
	PTEID_Certificates *out = NULL;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	out = dynamic_cast<PTEID_Certificates *>(getObject(INCLUDE_OBJECT_CERTIFICATES));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcerts=dynamic_cast<PTEID_Certificates *>(getObject(INCLUDE_OBJECT_CERTIFICATES));
		//if(!pcerts)
		//{
			out = new PTEID_Certificates(m_context,pcard->getCertificates());
			if(out)
				m_objects[INCLUDE_OBJECT_CERTIFICATES]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_SmartCard::getChallenge(bool bForceNewInit)
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	if(bForceNewInit)
	{
		delObject(INCLUDE_OBJECT_CHALLENGE);
		delObject(INCLUDE_OBJECT_RESPONSE);
	}

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_CHALLENGE));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_CHALLENGE));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getChallenge(bForceNewInit));
			if(out)
				m_objects[INCLUDE_OBJECT_CHALLENGE]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_SmartCard::getChallengeResponse()
{
	PTEID_ByteArray *out = NULL;
	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RESPONSE));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RESPONSE));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getChallengeResponse());
			if(out)
				m_objects[INCLUDE_OBJECT_RESPONSE]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

bool PTEID_SmartCard::verifyChallengeResponse(const PTEID_ByteArray &challenge, const PTEID_ByteArray &response) const
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_SmartCard *pcard=static_cast<APL_SmartCard *>(m_impl);

	CByteArray baChallenge(challenge.GetBytes(),challenge.Size());
	CByteArray baResponse(response.GetBytes(),response.Size());

	out = pcard->verifyChallengeResponse(baChallenge,baResponse);
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
---------------------------------------- PTEID_EIDCard -----------------------------------------
*****************************************************************************************/
PTEID_EIDCard::PTEID_EIDCard(const SDK_Context *context,APL_Card *impl):PTEID_SmartCard(context,impl)
{
}

PTEID_EIDCard::~PTEID_EIDCard()
{
}

bool PTEID_EIDCard::isApplicationAllowed()
{
	bool out = true;

	try														
	{
		out = APL_EIDCard::isApplicationAllowed();
	}
	catch(CMWException &e)
	{									
		e.GetError();				    
		throw PTEID_Exception::THROWException(e);	
	}				

	return out;
}

bool PTEID_EIDCard::isTestCard()
{
	bool out = true;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
	out = pcard->isTestCard();
	
	END_TRY_CATCH

	return out;
}

bool PTEID_EIDCard::getAllowTestCard()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
	out =  pcard->getAllowTestCard();
	
	END_TRY_CATCH

	return out;
}

void PTEID_EIDCard::setAllowTestCard(bool allow)
{
	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
	pcard->setAllowTestCard(allow);
	
	END_TRY_CATCH
}

PTEID_XMLDoc& PTEID_EIDCard::getDocument(PTEID_DocumentType type)
{
	switch(type)
	{
	case PTEID_DOCTYPE_FULL:
		return getFullDoc();
	case PTEID_DOCTYPE_ID:
		return getID();
	case PTEID_DOCTYPE_ADDRESS:
		return getAddr();
	case PTEID_DOCTYPE_SOD:
		return getSod();
	case PTEID_DOCTYPE_INFO:
		return getVersionInfo();
	case PTEID_DOCTYPE_PINS:
		return getPins();
	case PTEID_DOCTYPE_CERTIFICATES:
		return getCertificates();
	default:
		throw PTEID_ExDocTypeUnknown();
	}
}

PTEID_CCXML_Doc& PTEID_EIDCard::getXmlCCDoc(PTEID_XmlUserRequestedInfo& userRequestedInfo)
{
	PTEID_CCXML_Doc *out = NULL;

	BEGIN_TRY_CATCH
		APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
		out = dynamic_cast<PTEID_CCXML_Doc *>(getObject(INCLUDE_OBJECT_CUSTOMDOC));
		if(!out){
			out = new PTEID_CCXML_Doc(m_context,&pcard->getXmlCCDoc(*(userRequestedInfo.customXml)));
			if(out)
				m_objects[INCLUDE_OBJECT_CUSTOMDOC]=out;
			else
				throw PTEID_ExUnknown();
		}
	END_TRY_CATCH

	return *out;
}


PTEID_EIdFullDoc& PTEID_EIDCard::getFullDoc()
{
	PTEID_EIdFullDoc *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_EIdFullDoc *>(getObject(INCLUDE_OBJECT_FULLDOC));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pdoc=dynamic_cast<PTEID_EIdFullDoc *>(getObject(INCLUDE_OBJECT_FULLDOC));
		//if(!pdoc)
		//{
			out = new PTEID_EIdFullDoc(m_context,&pcard->getFullDoc());
			if(out)
				m_objects[INCLUDE_OBJECT_FULLDOC]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_EId& PTEID_EIDCard::getID()
{
	PTEID_EId *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out=dynamic_cast<PTEID_EId *>(getObject(INCLUDE_OBJECT_DOCEID));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pdoc=dynamic_cast<PTEID_EId *>(getObject(INCLUDE_OBJECT_DOCEID));
		//if(!pdoc)
		//{
			out = new PTEID_EId(m_context,&pcard->getID());
			if(out)
				m_objects[INCLUDE_OBJECT_DOCEID]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_Address& PTEID_EIDCard::getAddr()
{
	PTEID_Address *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out=dynamic_cast<PTEID_Address *>(getObject(INCLUDE_OBJECT_DOCEID));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pdoc=dynamic_cast<PTEID_EId *>(getObject(INCLUDE_OBJECT_DOCEID));
		//if(!pdoc)
		//{
			out = new PTEID_Address(m_context,&pcard->getAddr());
			if(out)
				m_objects[INCLUDE_OBJECT_DOCEID]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

PTEID_Sod& PTEID_EIDCard::getSod()
{
	PTEID_Sod *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_Sod *>(getObject(INCLUDE_OBJECT_SODID));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//ppicture=dynamic_cast<PTEID_Picture *>(getObject(INCLUDE_OBJECT_PICTUREEID));
		//if(!ppicture)
		//{
			out = new PTEID_Sod(m_context,&pcard->getSod());
			if(out)
				m_objects[INCLUDE_OBJECT_SODID]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_CardVersionInfo& PTEID_EIDCard::getVersionInfo()
{
	PTEID_CardVersionInfo *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_CardVersionInfo *>(getObject(INCLUDE_OBJECT_DOCINFO));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pdoc=dynamic_cast<PTEID_CardVersionInfo *>(getObject(INCLUDE_OBJECT_DOCINFO));
		//if(!pdoc)
		//{
			out = new PTEID_CardVersionInfo(m_context,&pcard->getDocInfo());
			if(out)
				m_objects[INCLUDE_OBJECT_DOCINFO]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_Certificate &PTEID_EIDCard::getCert(PTEID_CertifType type)
{
	return getCertificates().getCert(type);
}

PTEID_Certificate &PTEID_EIDCard::getRoot()
{
	return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT);
}

PTEID_Certificate &PTEID_EIDCard::getCA()
{
	return getCertificates().getCert(PTEID_CERTIF_TYPE_ROOT_AUTH);
}

PTEID_Certificate &PTEID_EIDCard::getAuthentication()
{
	return getCertificates().getCert(PTEID_CERTIF_TYPE_AUTHENTICATION);
}

PTEID_Certificate &PTEID_EIDCard::getSignature()
{
	return getCertificates().getCert(PTEID_CERTIF_TYPE_SIGNATURE);
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData(PTEID_RawDataType type)
{	
	switch(type)
	{
	case PTEID_RAWDATA_ID:
		return getRawData_Id();
	case PTEID_RAWDATA_ID_SIG:
		return getRawData_IdSig();
	case PTEID_RAWDATA_TRACE:
		return getRawData_Trace();
	case PTEID_RAWDATA_ADDR:
		return getRawData_Addr();
	case PTEID_RAWDATA_ADDR_SIG:
		return getRawData_AddrSig();
	case PTEID_RAWDATA_SOD:
		return getRawData_Sod();
	case PTEID_RAWDATA_CARD_INFO:
		return getRawData_CardInfo();
	case PTEID_RAWDATA_TOKEN_INFO:
		return getRawData_TokenInfo();
	case PTEID_RAWDATA_CHALLENGE:
		return getRawData_Challenge();
	case PTEID_RAWDATA_RESPONSE:
		return getRawData_Response();
	case PTEID_RAWDATA_PERSO_DATA:
		return getRawData_PersoData();
	default:
		throw PTEID_ExFileTypeUnknown();
	}
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Id()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ID));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ID));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Id());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_ID]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_IdSig()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ID_SIG));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ID_SIG));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_IdSig());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_ID_SIG]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Trace()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_TRACE));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ADDR));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Trace());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_TRACE]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Addr()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ADDR));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ADDR));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Addr());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_ADDR]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_AddrSig()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ADDR_SIG));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_ADDR_SIG));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_AddrSig());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_ADDR_SIG]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Sod()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_SOD));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_SOD));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Sod());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_SOD]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_CardInfo()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_CARD_INFO));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_CARD_INFO));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_CardInfo());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_CARD_INFO]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_TokenInfo()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_TOKEN_INFO));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_TOKEN_INFO));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_TokenInfo());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_TOKEN_INFO]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_PersoData()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_PERSO_DATA));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_PERSO_DATA));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_PersoData());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_PERSO_DATA]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}

	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Challenge()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_CHALLENGE));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_CHALLENGE));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Challenge());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_CHALLENGE]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const PTEID_ByteArray& PTEID_EIDCard::getRawData_Response()
{
	PTEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_RESPONSE));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//praw=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_RAWDATA_RESPONSE));
		//if(!praw)
		//{
			out = new PTEID_ByteArray(m_context,pcard->getRawData_Response());
			if(out)
				m_objects[INCLUDE_OBJECT_RAWDATA_RESPONSE]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

/*****************************************************************************************
---------------------------------------- PTEID_KidsCard -----------------------------------------
*****************************************************************************************/
PTEID_KidsCard::PTEID_KidsCard(const SDK_Context *context,APL_Card *impl):PTEID_EIDCard(context,impl)
{
}

PTEID_KidsCard::~PTEID_KidsCard()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ForeignerCard -----------------------------------------
*****************************************************************************************/
PTEID_ForeignerCard::PTEID_ForeignerCard(const SDK_Context *context,APL_Card *impl):PTEID_EIDCard(context,impl)
{
}

PTEID_ForeignerCard::~PTEID_ForeignerCard()
{
}

/*****************************************************************************************
----------------------------- PTEID_XmlUserRequestedInfo ---------------------------------
*****************************************************************************************/
PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo():PTEID_Object(NULL,NULL)
{
	customXml = new APL_XmlUserRequestedInfo();
}

PTEID_XmlUserRequestedInfo::~PTEID_XmlUserRequestedInfo()
{
	if (customXml)
		delete customXml;
}

void PTEID_XmlUserRequestedInfo::add(XMLUserData xmlUData){
	customXml->add(xmlUData);
}


}
