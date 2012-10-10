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
#include "PhotoPteid.h"
#include "ByteArray.h"
#include "CardPteid.h"
#include "PDFSignature.h"
#include "dialogs.h"
#include "Util.h"
#include <sstream>

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
#define INCLUDE_OBJECT_ROOT_CA_PK   	10

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

#define BCD_DATE_LEN					4

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

PTEID_ByteArray PTEID_Card::Sign(const PTEID_ByteArray& data, bool signatureKey)
{
	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

		APL_Card *pcard=static_cast<APL_Card *>(m_impl);

	CByteArray cData(data.GetBytes(),data.Size());
	CByteArray result=pcard->Sign(cData, signatureKey);
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


void PTEID_EIDCard::SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, const char *location, const char *reason,
			const char *outfile_path)
{
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	//Accessing PTEID_PDFSignature private parts because we're friends :)
	PDFSignature *pdf_sig = sig_handler.mp_signature; 
	if (page_sector != 0 && page != 0)
		pdf_sig->setVisible(page, page_sector);

	pcard->SignPDF(pdf_sig, location, reason, outfile_path);
	
	END_TRY_CATCH
}

PTEID_PDFSignature::PTEID_PDFSignature(const char *input_path)
{
	mp_signature = new PDFSignature(input_path);
}

PTEID_PDFSignature::~PTEID_PDFSignature()
{
	delete mp_signature;
}

char *PTEID_PDFSignature::getOccupiedSectors(int page)
{
	if (page < 1)
		throw PTEID_ExParamRange();
	return mp_signature->getOccupiedSectors(page);

}

int PTEID_PDFSignature::getPageCount()
{
	return mp_signature->getPageCount();
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
	persoNotesDirty = false;
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
		if(out)
			delete out;
		out = new PTEID_CCXML_Doc(m_context,&pcard->getXmlCCDoc(*(userRequestedInfo.customXml)));
		if(out)
			m_objects[INCLUDE_OBJECT_CUSTOMDOC]=out;
		else
			throw PTEID_ExUnknown();

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

PTEID_PublicKey& PTEID_EIDCard::getRootCAPubKey()
{
	PTEID_PublicKey *out = NULL;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out = dynamic_cast<PTEID_PublicKey *>(getObject(INCLUDE_OBJECT_ROOT_CA_PK));

	if(!out)
	{

		out = new PTEID_PublicKey(m_context,*pcard->getRootCAPubKey());
		if(out)
			m_objects[INCLUDE_OBJECT_ROOT_CA_PK]=out;
		else
			throw PTEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

bool PTEID_EIDCard::isActive(){
	bool out = false;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	out =  pcard->isActive();

	END_TRY_CATCH

	return out;
}

bool PTEID_EIDCard::ChangeCapPin(const char *new_pin){
	bool out = false;

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard = static_cast<APL_EIDCard *>(m_impl);

	out = pcard->ChangeCapPin(new_pin);
	
	END_TRY_CATCH

	return out;
}

bool testPIN(const char* pin){
	int i;
	std::stringstream ss(pin);

	if ((ss >> i).fail() || !(ss >> std::ws).eof())
		return false;
	return true;
}

bool PTEID_EIDCard::ChangeCapPinCompLayer(const char *old_pin, const char *new_pin, unsigned long &ulRemaining){
	bool out = false;
	bool validPins = false;
	std::string *oldPin;
	std::string *newPin;
	PTEID_Pin &pin = getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);

	if (old_pin && strlen(old_pin) > 0)
		validPins = testPIN(old_pin);
	if (new_pin && strlen(new_pin) > 0)
		validPins &= testPIN(new_pin);

	if (!validPins){
		wchar_t wsPin1[9]; // 8 + \0
		wchar_t wsPin2[9]; // 8 + \0
		DlgPinOperation pinOperation = DLG_PIN_OP_CHANGE;
		DlgPinUsage usage = DLG_PIN_AUTH;
		DlgPinInfo pinInfo = {4, 8, PIN_FLAG_DIGITS};
		DlgRet ret;
		std::wstring wideLabel(utilStringWiden(pin.getLabel()));

		ret = DlgAskPins(DLG_PIN_OP_CHANGE,
				DLG_PIN_AUTH, wideLabel.c_str(),
				pinInfo, wsPin1,8+1,
				pinInfo, wsPin2,8+1);

		if (ret == DLG_OK){
			oldPin = new string (utilStringNarrow(wsPin1));
			newPin = new string (utilStringNarrow(wsPin2));
		} else
			return false;
	} else {
 		oldPin = new string(old_pin);
		newPin = new string(new_pin);
	}

	if (ChangeCapPin(newPin->c_str())){
		if (!pin.changePin(oldPin->c_str(),newPin->c_str(),ulRemaining, pin.getLabel(),false)){
			ChangeCapPin(oldPin->c_str());
			out = false;
		} else {
			out = true;
		}
	}
	delete oldPin;
	delete newPin;

	return out;
}


void PTEID_EIDCard::doSODCheck(bool check){

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
	pcard->doSODCheck(check);

	END_TRY_CATCH
}

bool PTEID_EIDCard::Activate(const char *pinCode, PTEID_ByteArray &BCDDate){
	bool out = false;
	CByteArray cBCDDate = CByteArray(BCDDate.GetBytes(),BCDDate.Size());

	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);
	out =  pcard->Activate(pinCode,cBCDDate);

	END_TRY_CATCH

	return out;
}

bool PTEID_EIDCard::writePersonalNotes(const PTEID_ByteArray &out,PTEID_Pin *pin,const char *csPinCode){
	BEGIN_TRY_CATCH

	//martinho: TODO: isto terá de ser muito melhorado...
	persoNotesDirty = writeFile("3F005F00EF07", out, pin, csPinCode);

	END_TRY_CATCH

	return persoNotesDirty;
}

const char *PTEID_EIDCard::readPersonalNotes(){
	char *out = NULL;
	BEGIN_TRY_CATCH

	APL_EIDCard *pcard=static_cast<APL_EIDCard *>(m_impl);

	// ensure that the file is read after being written
	pcard->getFilePersoData()->getStatus(persoNotesDirty);
	// ensure that the attributes are mapped again persoNotesDirty = true.
	out = (char*)pcard->getPersonalNotes().getPersonalNotes(persoNotesDirty);

	END_TRY_CATCH

	return out;
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
----------------------------- PTEID_XmlUserRequestedInfo ---------------------------------
*****************************************************************************************/
PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo():PTEID_Object(NULL,NULL)
{
	customXml = new APL_XmlUserRequestedInfo();
}

PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress):PTEID_Object(NULL,NULL)
{
	customXml = new APL_XmlUserRequestedInfo(timeStamp, serverName, serverAddress);
}

PTEID_XmlUserRequestedInfo::PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress, const char *tokenID):PTEID_Object(NULL,NULL)
{
	customXml = new APL_XmlUserRequestedInfo(timeStamp, serverName, serverAddress, tokenID);
}

PTEID_XmlUserRequestedInfo::~PTEID_XmlUserRequestedInfo()
{
	if (customXml)
		delete customXml;
}

void PTEID_XmlUserRequestedInfo::add(XMLUserData xmlUData){
	customXml->add(xmlUData);
}

/*****************************************************************************************
----------------------------- pteid compatibility layer ---------------------------------
*****************************************************************************************/
PTEID_ReaderContext* readerContext = NULL;

// MARTINHO OK
PTEIDSDK_API long PTEID_Init(char *ReaderName){

	try {
		if (NULL == ReaderName || strlen(ReaderName) == 0)
			readerContext = &ReaderSet.getReader();
		else
			readerContext = &ReaderSet.getReaderByName(ReaderName);

		PTEID_EIDCard &card = readerContext->getEIDCard();
	}
	catch(PTEID_ExCardBadType &ex)
	{
		std::cout << "This is not an eid card" << std::endl;
	}
	catch(PTEID_ExNoCardPresent &ex)
	{
		return -1104;
	}
	catch(PTEID_ExNoReader &ex)
	{
		return -1101;
	}
	catch(PTEID_Exception &ex)
	{
		std::cout << "PTEID_Exception exception " << ex.GetError()<< std::endl;
	}
	catch(...)
	{
		return -1;
	}
	return 0;
}

// MARTINHO - ver segfault
PTEIDSDK_API long PTEID_Exit(unsigned long ulMode){
	readerContext->Release();
	PTEID_ReleaseSDK();
	readerContext = NULL;

	return 0;
}

// MARTINHO OK
PTEIDSDK_API tCompCardType PTEID_GetCardType(){

	if (readerContext!=NULL){
		PTEID_CardType type = readerContext->getCardType();
		switch (type){
			case PTEID_CARDTYPE_IAS07:
				return COMP_CARD_TYPE_IAS07;
				break;
			case PTEID_CARDTYPE_IAS101:
				return COMP_CARD_TYPE_IAS101;
				break;
			default:
				return COMP_CARD_TYPE_ERR;
		}
	}
	return COMP_CARD_TYPE_ERR;
}


PTEIDSDK_API long PTEID_GetID(PTEID_ID *IDData){

	if (readerContext!=NULL){
		PTEID_EId &id = readerContext->getEIDCard().getID();

		memset(IDData, '\0', sizeof(PTEID_ID));

		IDData->version = 0;
		strncpy(IDData->deliveryEntity, id.getIssuingEntity(),PTEID_MAX_DELIVERY_ENTITY_LEN-1);
		strncpy(IDData->country, id.getCountry(), PTEID_MAX_COUNTRY_LEN-1);
		strncpy(IDData->documentType, id.getDocumentType(), PTEID_MAX_DOCUMENT_TYPE_LEN-1);
		strncpy(IDData->cardNumber, id.getDocumentNumber(), PTEID_MAX_CARDNUMBER_LEN-1);
		strncpy(IDData->cardNumberPAN, id.getDocumentPAN(), PTEID_MAX_CARDNUMBER_PAN_LEN-1);
		strncpy(IDData->cardVersion, id.getDocumentVersion(), PTEID_MAX_CARDVERSION_LEN-1);
		strncpy(IDData->deliveryDate, id.getValidityBeginDate(), PTEID_MAX_DATE_LEN-1);
		strncpy(IDData->locale, id.getLocalofRequest(), PTEID_MAX_LOCALE_LEN-1);
		strncpy(IDData->validityDate, id.getValidityEndDate(), PTEID_MAX_DATE_LEN-1);
		strncpy(IDData->name, id.getSurname(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->firstname, id.getGivenName(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->sex, id.getGender(), PTEID_MAX_SEX_LEN-1);
		strncpy(IDData->nationality, id.getNationality(), PTEID_MAX_NATIONALITY_LEN-1);
		strncpy(IDData->birthDate, id.getDateOfBirth(), PTEID_MAX_DATE_LEN-1);
		strncpy(IDData->height, id.getHeight(), PTEID_MAX_HEIGHT_LEN-1);
		strncpy(IDData->numBI, id.getCivilianIdNumber(), PTEID_MAX_NUMBI_LEN-1);
		strncpy(IDData->nameFather, id.getSurnameFather(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->firstnameFather, id.getGivenNameFather(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->nameMother, id.getSurnameMother(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->firstnameMother, id.getGivenNameMother(), PTEID_MAX_NAME_LEN-1);
		strncpy(IDData->numNIF, id.getTaxNo(), PTEID_MAX_NUMNIF_LEN-1);
		strncpy(IDData->numSS, id.getSocialSecurityNumber(), PTEID_MAX_NUMSS_LEN-1);
		strncpy(IDData->numSNS, id.getHealthNumber(), PTEID_MAX_NUMSNS_LEN-1);
		strncpy(IDData->notes, id.getAccidentalIndications(),PTEID_MAX_INDICATIONEV_LEN-1);
		strncpy(IDData->mrz1, id.getMRZ1(), PTEID_MAX_MRZ_LEN-1);
		strncpy(IDData->mrz2, id.getMRZ2(), PTEID_MAX_MRZ_LEN-1);
		strncpy(IDData->mrz3, id.getMRZ3(), PTEID_MAX_MRZ_LEN-1);
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetAddr(PTEID_ADDR *AddrData){
	int pin_id;
	unsigned long remaining_tries;

	if (readerContext!=NULL){

		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long PinIdx=0; PinIdx < pins.count(); PinIdx++){
				PTEID_Pin&	Pin	= pins.getPinByNumber(PinIdx);
				if (strstr(Pin.getLabel(), "PIN da Morada")){
					if (!Pin.verifyPin("",remaining_tries))
						return -1;
					break;
				}
		}

		PTEID_Address &addr = readerContext->getEIDCard().getAddr();

		memset(AddrData, '\0', sizeof(PTEID_ADDR));

		AddrData->version = 0;
		strncpy(AddrData->country, addr.getCountryCode(), PTEID_MAX_ADDR_COUNTRY_LEN-1);
		strncpy(AddrData->numMor, addr.getGeneratedAddressCode(), PTEID_MAX_NUMMOR_LEN-1);

		if (addr.isNationalAddress())
		{
			strncpy(AddrData->addrType, COMP_LAYER_NATIONAL_ADDRESS, PTEID_MAX_ADDR_TYPE_LEN-1);
			strncpy(AddrData->district, addr.getDistrictCode(), PTEID_MAX_DISTRICT_LEN-1);
			strncpy(AddrData->districtDesc, addr.getDistrict(), PTEID_MAX_DISTRICT_DESC_LEN-1);
			strncpy(AddrData->municipality, addr.getMunicipalityCode(), PTEID_MAX_DISTRICT_CON_LEN-1);
			strncpy(AddrData->municipalityDesc, addr.getMunicipality(), PTEID_MAX_DISTRICT_CON_DESC_LEN-1);
			strncpy(AddrData->freguesia, addr.getCivilParishCode(), PTEID_MAX_DISTRICT_FREG_LEN-1);
			strncpy(AddrData->freguesiaDesc, addr.getCivilParish(), PTEID_MAX_DISTRICT_FREG_DESC_LEN-1);
			strncpy(AddrData->streettypeAbbr, addr.getAbbrStreetType(), PTEID_MAX_ROAD_ABBR_LEN-1);
			strncpy(AddrData->streettype, addr.getStreetType(), PTEID_MAX_ROAD_LEN-1);
			strncpy(AddrData->street, addr.getStreetName(), PTEID_MAX_ROAD_DESIG_LEN-1);
			strncpy(AddrData->buildingAbbr, addr.getAbbrBuildingType(), PTEID_MAX_HOUSE_ABBR_LEN-1);
			strncpy(AddrData->building, addr.getBuildingType(), PTEID_MAX_HOUSE_LEN-1);
			strncpy(AddrData->door, addr.getDoorNo(), PTEID_MAX_NUMDOOR_LEN-1);
			strncpy(AddrData->floor, addr.getFloor(), PTEID_MAX_FLOOR_LEN-1);
			strncpy(AddrData->side, addr.getSide(), PTEID_MAX_SIDE_LEN-1);
			strncpy(AddrData->place, addr.getPlace(), PTEID_MAX_PLACE_LEN-1);
			strncpy(AddrData->locality, addr.getLocality(), PTEID_MAX_LOCALITY_LEN-1);
			strncpy(AddrData->cp4, addr.getZip4(), PTEID_MAX_CP4_LEN-1);
			strncpy(AddrData->cp3, addr.getZip3(), PTEID_MAX_CP3_LEN-1);
			strncpy(AddrData->postal, addr.getPostalLocality(), PTEID_MAX_POSTAL_LEN-1);
		} else {
			strncpy(AddrData->addrType, COMP_LAYER_FOREIGN_ADDRESS, PTEID_MAX_ADDR_TYPE_LEN-1);
			strncpy(AddrData->countryDescF, addr.getForeignCountry(), PTEID_MAX_ADDR_COUNTRYF_DESC_LEN-1);
			strncpy(AddrData->addressF, addr.getForeignAddress(), PTEID_MAX_ADDRF_LEN-1);
			strncpy(AddrData->cityF, addr.getForeignCity(), PTEID_MAX_CITYF_LEN-1);
			strncpy(AddrData->regioF, addr.getForeignRegion(), PTEID_MAX_REGIOF_LEN-1);
			strncpy(AddrData->localityF, addr.getForeignLocality(), PTEID_MAX_LOCALITYF_LEN-1);
			strncpy(AddrData->postalF, addr.getForeignPostalCode(), PTEID_MAX_POSTALF_LEN-1);
		}
	}
	return 0;
}

PTEIDSDK_API long PTEID_GetPic(PTEID_PIC *PicData){

	if (readerContext!=NULL){

		PTEID_Photo &photoOjb = readerContext->getEIDCard().getID().getPhotoObj();

		memset(PicData, 0, sizeof(PTEID_PIC));

		PTEID_ByteArray &scratch = photoOjb.getphotoRAW();
		memcpy(PicData->picture, scratch.GetBytes(), (PTEID_MAX_PICTUREH_LEN >= scratch.Size()) ? scratch.Size(): PTEID_MAX_PICTUREH_LEN);
		PicData->piclength = scratch.Size();
		scratch = photoOjb.getphotoCbeff();
		memcpy(PicData->cbeff, scratch.GetBytes(), (PTEID_MAX_CBEFF_LEN >= scratch.Size()) ? scratch.Size(): PTEID_MAX_CBEFF_LEN);
		scratch = photoOjb.getphotoFacialrechdr();
		memcpy(PicData->facialrechdr, scratch.GetBytes(), (PTEID_MAX_FACRECH_LEN >= scratch.Size()) ? scratch.Size(): PTEID_MAX_FACRECH_LEN);
		scratch = photoOjb.getphotoFacialinfo();
		memcpy(PicData->facialinfo, scratch.GetBytes(), (PTEID_MAX_FACINFO_LEN >= scratch.Size()) ? scratch.Size(): PTEID_MAX_FACINFO_LEN);
		scratch = photoOjb.getphotoImageinfo();
		memcpy(PicData->imageinfo, scratch.GetBytes(), (PTEID_MAX_IMAGEINFO_LEN>= scratch.Size()) ? scratch.Size(): PTEID_MAX_IMAGEINFO_LEN);
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetCertificates(PTEID_Certifs *Certifs){
	if (readerContext!=NULL){
		PTEID_Certificates &certificates = readerContext->getEIDCard().getCertificates();
		PTEID_ByteArray ba;
		memset(Certifs, 0, sizeof(PTEID_Certifs));

		for (int i=0 ; i< certificates.countAll(); i++){
			PTEID_Certificate &cert = certificates.getCert(i);
			cert.getFormattedData(ba);
			memcpy(Certifs->certificates[i].certif, ba.GetBytes(),(PTEID_MAX_CERT_LEN >= ba.Size()) ? ba.Size() : PTEID_MAX_CERT_LEN);
			Certifs->certificates[i].certifLength = (PTEID_MAX_CERT_LEN >= ba.Size()) ? ba.Size() : PTEID_MAX_CERT_LEN;
			strncpy(Certifs->certificates[i].certifLabel, cert.getLabel(), (PTEID_MAX_CERT_LABEL_LEN >= strlen(cert.getLabel()) ? strlen(cert.getLabel()) : PTEID_MAX_CERT_LABEL_LEN-1));
		}
		Certifs->certificatesLength = certificates.countAll();
	}

	return 0;
}


PTEIDSDK_API long PTEID_VerifyPIN(unsigned char PinId,	char *Pin, long *triesLeft){
	unsigned long id;
	unsigned long tleft=-1;
	bool ret;

	if (readerContext!=NULL){
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131)
			return 0;

		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin&	pin	= pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == PinId){
				ret = pin.verifyPin("",tleft);
				//martinho: verify pin is not working properly for readers without pinpad at this moment,
				//this is a workaround
				*triesLeft = pin.getTriesLeft();

				if (ret)
					return 0;
				return -1;
			}
		}
	}

	return 0;
}

PTEIDSDK_API long PTEID_VerifyPIN_No_Alert(unsigned char PinId,	char *Pin, long *triesLeft){

	if (readerContext!=NULL){
		//martinho: in this phase we do not have a distinct functionality for this feature.
		return PTEID_VerifyPIN(PinId, Pin, triesLeft);
	}

	return 0;
}

PTEIDSDK_API long PTEID_ChangePIN(unsigned char PinId, char *pszOldPin, char *pszNewPin, long *triesLeft){

	const char *a1 = pszOldPin, *a2 = pszNewPin;
	unsigned long int tries = -1;

	if (readerContext!=NULL){
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131)
			return 0;

		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin&	pin	= pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == PinId)
				if (pin.changePin(pszOldPin ,pszNewPin, tries, pin.getLabel())){
					*triesLeft = pin.getTriesLeft();
					return 0;
				} else
					return -1;
		}
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetPINs(PTEIDPins *Pins){
	long int i=0;
	unsigned long currentId= 0;

	if (readerContext!=NULL){
		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin&	pin	= pins.getPinByNumber(pinIdx);
			if (pin.getId() == 1 || pin.getId() == 2 || pin.getId() == 3){
				currentId = pin.getId()-1;
				Pins->pins[currentId].flags = pin.getFlags();
				Pins->pins[currentId].usageCode = pin.getId(); // martinho: might not be the intended use, but gives the expected compatible result.
				Pins->pins[currentId].pinType = pin.getType();
				memset(Pins->pins[currentId].label, '\0', PTEID_MAX_PIN_LABEL_LEN);
				strncpy(Pins->pins[currentId].label, pin.getLabel(), (PTEID_MAX_PIN_LABEL_LEN > strlen(pin.getLabel()) ? strlen(pin.getLabel()) : PTEID_MAX_PIN_LABEL_LEN-1));
				Pins->pins[currentId].triesLeft = pin.getTriesLeft();
				Pins->pins[currentId].id = pin.getPinRef();
				Pins->pins[currentId].shortUsage = NULL; //martinho don't know where it is used, current MW returns NULL also
				Pins->pins[currentId].longUsage = NULL; //martinho don't know where it is used, current MW returns NULL also
				i++;
			}
		}
		Pins->pinsLength = i;
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetTokenInfo(PTEID_TokenInfo *tokenData){

	if (readerContext!=NULL){
		PTEID_CardVersionInfo &versionInfo = readerContext->getEIDCard().getVersionInfo();

		memset(tokenData->label, '\0', PTEID_MAX_CERT_LABEL_LEN);
		strncpy(tokenData->label, versionInfo.getTokenLabel(), (PTEID_MAX_CERT_LABEL_LEN > strlen(versionInfo.getTokenLabel()) ? strlen(versionInfo.getTokenLabel()) : PTEID_MAX_CERT_LABEL_LEN -1));
		memset(tokenData->serial, '\0', PTEID_MAX_ID_NUMBER_LEN);
		strncpy(tokenData->serial, versionInfo.getSerialNumber(), (PTEID_MAX_ID_NUMBER_LEN > strlen(versionInfo.getSerialNumber()) ? strlen(versionInfo.getSerialNumber()) : PTEID_MAX_ID_NUMBER_LEN -1));
	}

	return 0;
}

PTEIDSDK_API long PTEID_ReadSOD(unsigned char *out, unsigned long *outlen){
	if (readerContext!=NULL){
		PTEID_ByteArray temp;
		PTEID_EIDCard &card = readerContext->getEIDCard();

		temp = card.getSod().getData();
		CByteArray cb((unsigned char*)temp.GetBytes(), temp.Size());
		cb.TrimRight(0);
		memset(out,0,*outlen);
		if (cb.Size() < *outlen)
			*outlen = cb.Size();
		memcpy(out,cb.GetBytes(), *outlen);
	}

	return 0;
}

PTEIDSDK_API long PTEID_UnblockPIN(unsigned char PinId,	char *pszPuk, char *pszNewPin, long *triesLeft){
	unsigned long id;
	unsigned long tleft;

	if (readerContext!=NULL){
		if (PinId != 1 && PinId != 129 && PinId != 130 && PinId != 131)
			return 0;

		PTEID_Pins &pins = readerContext->getEIDCard().getPins();
		for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
			PTEID_Pin&	pin	= pins.getPinByNumber(pinIdx);
			if (pin.getPinRef() == PinId){
				pin.unlockPin(pszPuk, pszNewPin,tleft);
				*triesLeft = tleft;
			}
		}
	}

	return 0;
}

PTEIDSDK_API long PTEID_UnblockPIN_Ext(unsigned char PinId,	char *pszPuk, char *pszNewPin, long *triesLeft, unsigned long ulFlags){
	if (readerContext!=NULL){
		return PTEID_UnblockPIN(PinId,	pszPuk,pszNewPin,triesLeft);
	}

	return -1;
}

PTEIDSDK_API long PTEID_SelectADF(unsigned char *adf, long adflen){
	if (readerContext!=NULL){
		PTEID_EIDCard &card = readerContext->getEIDCard();
		unsigned char ap[4] = {0x00, 0xA4, 0x00, 0x0C};
		PTEID_ByteArray apdu(ap,(unsigned long)(sizeof(ap)/sizeof(unsigned char)));
		apdu.Append((unsigned char*)&adflen,sizeof(unsigned char));
		apdu.Append(adf,(unsigned long) adflen);
		card.sendAPDU(apdu);
	}

	return 0;
}

PTEIDSDK_API long PTEID_ReadFile(unsigned char *file,int filelen,unsigned char *out,unsigned long *outlen,unsigned char PinId){

	if (readerContext!=NULL && (PinId == PTEID_ADDRESS_PIN_ID || PinId == PTEID_NO_PIN_NEEDED)){
		PTEID_EIDCard &card = readerContext->getEIDCard();
		CByteArray filePath;
		PTEID_ByteArray in;
		PTEID_Pin*	pin = NULL;

		if (PinId != PTEID_NO_PIN_NEEDED){
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
				pin	= &(pins.getPinByNumber(pinIdx));
				if (pin->getPinRef() == PTEID_ADDRESS_PIN_ID){
					break;
				}
			}
		}

		filePath.Append(file,filelen);
		card.readFile(filePath.ToString(false).c_str(),in, pin,"");

		CByteArray cb(in.GetBytes(), in.Size());
		cb.TrimRight(0);

		*outlen = (*outlen>cb.Size() ? cb.Size() : *outlen);
		memcpy(out, cb.GetBytes(),*outlen);
	}

	return 0;
}

PTEIDSDK_API long PTEID_WriteFile(unsigned char *file, int filelen,	unsigned char *in, unsigned long inlen,	unsigned char PinId){
	if (readerContext!=NULL && (PinId == 1 || PinId == 129)){
		PTEID_EIDCard &card = readerContext->getEIDCard();
		CByteArray temp;
		PTEID_ByteArray out;
		PTEID_Pin*	pin = NULL;

		if (PinId != PTEID_NO_PIN_NEEDED){
			PinId = 1;
			PTEID_Pins &pins = readerContext->getEIDCard().getPins();
			for (unsigned long pinIdx=0; pinIdx < pins.count(); pinIdx++){
				pin	= &(pins.getPinByNumber(pinIdx));
				if (pin->getId() == PinId)
					break;
			}
		}

		out.Append(in,inlen);
		temp.Append(file,filelen);
		if (card.writeFile(temp.ToString(false).c_str(),out, pin,""))
			return 0;
		return -1;
	}

	return 0;
}

PTEIDSDK_API long PTEID_IsActivated(unsigned long *pulStatus){

	if (readerContext!=NULL)
		*pulStatus = (readerContext->getEIDCard().isActive() ? PTEID_ACTIVE_CARD : PTEID_INACTIVE_CARD);
	return 0;
}

PTEIDSDK_API long PTEID_Activate(char *pszPin, unsigned char *pucDate, unsigned long ulMode){
	long retval = 0;
	if (readerContext!=NULL){
		PTEID_ByteArray bcd(pucDate,BCD_DATE_LEN);
		if (readerContext->getEIDCard().Activate(pszPin,bcd))
			return 0;
		return -1;
	}

	return 0;
}

PTEIDSDK_API long PTEID_SetSODChecking(int bDoCheck){
	if (readerContext!=NULL)
		readerContext->getEIDCard().doSODCheck(bDoCheck!=0);

	return 0;
}

PTEIDSDK_API long PTEID_SetSODCAs(PTEID_Certifs *Certifs){
	if (readerContext!=NULL){
		for(int i=0;i<Certifs->certificatesLength;i++){
			PTEID_ByteArray *pba = new PTEID_ByteArray(Certifs->certificates[i].certif,Certifs->certificates[i].certifLength);
			readerContext->getEIDCard().getCertificates().addCertificate(*pba);
			delete pba;
		}
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetCardAuthenticationKey(PTEID_RSAPublicKey *pCardAuthPubKey){
	if (readerContext!=NULL){
		PTEID_PublicKey &cardKey = readerContext->getEIDCard().getID().getCardAuthKeyObj();

		memcpy(pCardAuthPubKey->modulus, cardKey.getCardAuthKeyModulus().GetBytes(), cardKey.getCardAuthKeyModulus().Size());
		pCardAuthPubKey->modulusLength = cardKey.getCardAuthKeyModulus().Size();
		memcpy(pCardAuthPubKey->exponent, cardKey.getCardAuthKeyExponent().GetBytes(), cardKey.getCardAuthKeyExponent().Size());
		pCardAuthPubKey->exponentLength = cardKey.getCardAuthKeyExponent().Size();
	}

	return 0;
}

PTEIDSDK_API long PTEID_GetCVCRoot(PTEID_RSAPublicKey *pCVCRootKey){
	if (readerContext!=NULL){
		PTEID_PublicKey &rootCAKey = readerContext->getEIDCard().getRootCAPubKey();

		memcpy(pCVCRootKey->modulus, rootCAKey.getCardAuthKeyModulus().GetBytes(), rootCAKey.getCardAuthKeyModulus().Size());
		pCVCRootKey->modulusLength = rootCAKey.getCardAuthKeyModulus().Size();
		memcpy(pCVCRootKey->exponent, rootCAKey.getCardAuthKeyExponent().GetBytes(), rootCAKey.getCardAuthKeyExponent().Size());
		pCVCRootKey->exponentLength = rootCAKey.getCardAuthKeyExponent().Size();
	}

	return 0;
}

PTEIDSDK_API long PTEID_SendAPDU(const unsigned char *ucRequest, unsigned long ulRequestLen, unsigned char *ucResponse, unsigned long *ulResponseLen){
	if (readerContext!=NULL){
		PTEID_EIDCard &card = readerContext->getEIDCard();
		PTEID_ByteArray sApdu(ucRequest,ulRequestLen);
		PTEID_ByteArray resp;

		resp = card.sendAPDU(sApdu);

		*ulResponseLen = (*ulResponseLen > resp.Size()) ? resp.Size() : *ulResponseLen;
		memcpy(ucResponse,resp.GetBytes(),*ulResponseLen);
	}

	return 0;
}

PTEIDSDK_API long PTEID_CAP_ChangeCapPin(const char *csServer, const unsigned char *ucServerCaCert,	unsigned long ulServerCaCertLen, tProxyInfo *proxyInfo,	const char *pszOldPin, const char *pszNewPin, long *triesLeft){

	PTEID_EIDCard& card = readerContext->getEIDCard();
	bool ret = card.ChangeCapPinCompLayer(pszOldPin, pszNewPin, (unsigned long int&)*triesLeft);

	return ret ? 0 : -1;
}

/* not implemented */
PTEIDSDK_API tCapPinChangeState PTEID_CAP_GetCapPinChangeProgress(){
	return CAP_INITIALISING;
}

/* not implemented */
PTEIDSDK_API void PTEID_CAP_SetCapPinChangeCallback(void(_USERENTRY * callback)(tCapPinChangeState state)){
	return;
}

/* not implemented */
PTEIDSDK_API void PTEID_CAP_CancelCapPinChange(){
	return;
}


}
