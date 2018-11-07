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
#include "MWException.h"
#include "eidErrors.h"
#include "InternalUtil.h"

#include "APLCrypto.h"
#include "APLCertif.h"

#include <vector>
#include <limits.h>

//UNIQUE INDEX FOR RETRIEVING OBJECT
#define	INCLUDE_OBJECT_CERTDATA		1
#define	INCLUDE_OBJECT_ROOTCERT		2
#define	INCLUDE_OBJECT_CERTISSUER	3
#define INCLUDE_OBJECT_PINSIGN		6
#define	INCLUDE_OBJECT_ROOTAUTHCERT		12
#define	INCLUDE_OBJECT_AUTHCERT		8
#define	INCLUDE_OBJECT_SIGNCERT		9
#define	INCLUDE_OBJECT_ROOTSIGNCERT		10

#define	INCLUDE_OBJECT_FIRSTPIN		1000
#define	INCLUDE_OBJECT_FIRSTPINREF	1100
#define	INCLUDE_OBJECT_FIRSTCERT	2000
#define	INCLUDE_OBJECT_FIRSTCHILD	3000

#define ANY_INDEX ULONG_MAX

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- PTEID_Certificate ------------------------------------
*****************************************************************************************/
PTEID_Certificate::PTEID_Certificate(const SDK_Context *context,APL_Certif *impl):PTEID_Crypto(context,impl)
{
}

PTEID_Certificate::~PTEID_Certificate()
{
}

const char *PTEID_Certificate::getLabel()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getLabel();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Certificate::getID()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getID();
	
	END_TRY_CATCH

	return out;
}

PTEID_CertifType PTEID_Certificate::getType()
{
	PTEID_CertifType out =PTEID_CERTIF_TYPE_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertType(pimpl->getType());
	
	END_TRY_CATCH

	return out;
}

PTEID_CertifStatus PTEID_Certificate::getStatus()
{
        PTEID_CertifStatus out = PTEID_CERTIF_STATUS_UNKNOWN;

        BEGIN_TRY_CATCH

        APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
        out = ConvertCertStatus(pimpl->getStatus());
       
        END_TRY_CATCH

        return out;
}


const PTEID_ByteArray& PTEID_Certificate::getCertData()
{
	PTEID_ByteArray *out = NULL;
	
	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_CERTDATA));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pbytearray=dynamic_cast<PTEID_ByteArray *>(getObject(INCLUDE_OBJECT_CERTDATA));
		//if(!pbytearray)
		//{
			out = new PTEID_ByteArray(m_context,pimpl->getData());
			if(out)
				m_objects[INCLUDE_OBJECT_CERTDATA]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

void PTEID_Certificate::getFormattedData(PTEID_ByteArray &data)
{
	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	CByteArray bytearray;

	pimpl->getFormattedData(bytearray);
	data=bytearray;
	
	END_TRY_CATCH

	return;
}

bool PTEID_Certificate::isRoot()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isRoot();
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Certificate::isTest()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isTest();
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Certificate::isFromPteidValidChain()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isFromPteidValidChain();
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Certificate::isFromCard()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isFromCard();
	
	END_TRY_CATCH

	return out;
}

PTEID_Certificate &PTEID_Certificate::getIssuer()
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pIssuer=dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));
		//if(!pIssuer)
		//{
			APL_Certif *aplIssuer=pimpl->getIssuer();
			if (aplIssuer == NULL || aplIssuer == pimpl)
			{
				throw PTEID_ExCertNoIssuer();
			}
			else
			{
				out = new PTEID_Certificate(m_context,aplIssuer);
				if(out)
					m_objects[INCLUDE_OBJECT_CERTISSUER]=out;
				else
					throw PTEID_ExUnknown();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

unsigned long PTEID_Certificate::countChildren()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->countChildren();
	
	END_TRY_CATCH

	return out;
}

PTEID_Certificate &PTEID_Certificate::getChildren(unsigned long ulIndex)
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCHILD+ulIndex));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pChild=dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCHILD+ulIndex));
		//if(!pChild)
		//{
			APL_Certif *aplChild=pimpl->getChildren(ulIndex);
			if(aplChild)
			{
				out = new PTEID_Certificate(m_context,aplChild);
				if(out)
					m_objects[INCLUDE_OBJECT_FIRSTCHILD+ulIndex]=out;
				else
					throw PTEID_ExUnknown();
			}
			else
			{
				throw PTEID_ExParamRange();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const char *PTEID_Certificate::getSerialNumber()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getSerialNumber();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Certificate::getOwnerName()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getOwnerName();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Certificate::getIssuerName()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getIssuerName();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Certificate::getValidityBegin()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getValidityBegin();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Certificate::getValidityEnd()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getValidityEnd();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Certificate::getKeyLength()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getKeyLength();
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
---------------------------------------- PTEID_Certificates ------------------------------------
*****************************************************************************************/
PTEID_Certificates::PTEID_Certificates(const SDK_Context *context,APL_Certifs *impl):PTEID_Crypto(context,impl)
{
}

PTEID_Certificates::~PTEID_Certificates()
{
}

unsigned long PTEID_Certificates::countFromCard()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);
	out = pimpl->countFromCard();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Certificates::countAll()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);
	out = pimpl->countAll();
	
	END_TRY_CATCH

	return out;
}

PTEID_Certificate &PTEID_Certificates::getCertFromCard(unsigned long ulIndex)
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	out = dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCERT+ulIndex));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<PTEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCERT+ulIndex));
		//if(!pcert)
		//{
			out = new PTEID_Certificate(m_context,pimpl->getCertFromCard(ulIndex));
			if(out)
				m_objects[INCLUDE_OBJECT_FIRSTCERT+ulIndex]=out;
			else
				throw PTEID_ExParamRange();

		//}
	}
	
	END_TRY_CATCH

	return *out;
}

void PTEID_Certificates::addToSODCAs(PTEID_ByteArray &cert)
{
	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	CByteArray cert_ba(cert.GetBytes(), cert.Size());
	pimpl->addToSODCAs(cert_ba);
	
	END_TRY_CATCH
}

void PTEID_Certificates::resetSODCAs()
{

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);
	pimpl->clearSODCAs();
	pimpl->initSODCAs();
	
	END_TRY_CATCH

}

PTEID_Certificate &PTEID_Certificates::getCert(unsigned long ulIndex)
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	APL_Certif *pAplCert=pimpl->getCert(ulIndex);

	out = dynamic_cast<PTEID_Certificate *>(getObject(pAplCert));
	
	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<PTEID_Certificate *>(getObject(pAplCert));
		//if(!pcert)
		//{
			out = new PTEID_Certificate(m_context,pAplCert);
			if(out)
				addObject(out);
			else
				throw PTEID_ExParamRange();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}


PTEID_Certificate &PTEID_Certificates::getCert(PTEID_CertifType type)
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	unsigned long idxObject;
	APL_CertifType aplType;
	
 	switch(type)
	{
		case PTEID_CERTIF_TYPE_ROOT:
			idxObject=INCLUDE_OBJECT_ROOTCERT;
			aplType=APL_CERTIF_TYPE_ROOT;
			break;
		case PTEID_CERTIF_TYPE_ROOT_AUTH:
			idxObject=INCLUDE_OBJECT_ROOTAUTHCERT;
			aplType=APL_CERTIF_TYPE_ROOT_AUTH;
			break;
		case PTEID_CERTIF_TYPE_AUTHENTICATION:
			idxObject=INCLUDE_OBJECT_AUTHCERT;
			aplType=APL_CERTIF_TYPE_AUTHENTICATION;
			break;
		case PTEID_CERTIF_TYPE_SIGNATURE:
			idxObject=INCLUDE_OBJECT_SIGNCERT;
			aplType=APL_CERTIF_TYPE_SIGNATURE;
			break;
		case PTEID_CERTIF_TYPE_ROOT_SIGN:
			idxObject=INCLUDE_OBJECT_ROOTSIGNCERT;
			aplType=APL_CERTIF_TYPE_ROOT_SIGN;
			break;
		default:
			throw PTEID_ExBadUsage();
	}

	out = dynamic_cast<PTEID_Certificate *>(getObject(idxObject));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//proot=dynamic_cast<PTEID_Certificate *>(getObject(idxObject));
		//if(!proot)
		//{
			out = new PTEID_Certificate(m_context,pimpl->getCert(aplType, ANY_INDEX));
			if(out)
				m_objects[idxObject]=out;
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_Certificate &PTEID_Certificates::getRoot()
{
	return getCert(PTEID_CERTIF_TYPE_ROOT);
}

PTEID_Certificate &PTEID_Certificates::getCA()
{
	return getCert(PTEID_CERTIF_TYPE_ROOT_AUTH);
}

PTEID_Certificate &PTEID_Certificates::getAuthentication()
{
	return getCert(PTEID_CERTIF_TYPE_AUTHENTICATION);
}

PTEID_Certificate &PTEID_Certificates::getSignature()
{
	return getCert(PTEID_CERTIF_TYPE_SIGNATURE);
}

PTEID_Certificate &PTEID_Certificates::addCertificate(PTEID_ByteArray &cert)
{
	PTEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	CByteArray baCert(cert.GetBytes(),cert.Size());
	APL_Certif *pAplCert=pimpl->addCert(baCert, APL_CERTIF_TYPE_ROOT, false);

	out = dynamic_cast<PTEID_Certificate *>(getObject(pAplCert));
	
	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<PTEID_Certificate *>(getObject(pAplCert));
		//if(!pcert)
		//{
			out = new PTEID_Certificate(m_context,pAplCert);
			if(out)
				addObject(out);
			else
				throw PTEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}


/*****************************************************************************************
---------------------------------------- PTEID_Pin -------------------------------------------
*****************************************************************************************/
/* This allocation is not needed on MSVC and generates a linking error
   (duplicate definitions for the symbols) */
#ifndef WIN32 
const unsigned long PTEID_Pin::AUTH_PIN;
const unsigned long PTEID_Pin::SIGN_PIN;
const unsigned long PTEID_Pin::ADDR_PIN;
#endif

PTEID_Pin::PTEID_Pin(const SDK_Context *context,APL_Pin *impl):PTEID_Crypto(context,impl)
{
}

PTEID_Pin::~PTEID_Pin()
{
}

unsigned long PTEID_Pin::getIndex()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getIndex();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Pin::getType()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getType();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Pin::getId()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getId();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Pin::getPinRef()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getPinRef();

	END_TRY_CATCH

	return out;
}

bool PTEID_Pin::unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long flags)
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->unlockPin(pszPuk, pszNewPin, triesLeft, flags);

	END_TRY_CATCH

	return out;
}


PTEID_PinUsage PTEID_Pin::getUsageCode()
{
	PTEID_PinUsage out = PTEID_PIN_USG_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	PinUsage usage=pimpl->getUsageCode();
	switch(usage)
	{
	case DLG_USG_PIN_AUTH:
		out = PTEID_PIN_USG_AUTH;
		break;

	case DLG_USG_PIN_SIGN:
		out = PTEID_PIN_USG_SIGN;
		break;

	case DLG_USG_PIN_ADDRESS:
		out = PTEID_PIN_USG_ADDRESS;
		break;

	default:
		out = PTEID_PIN_USG_UNKNOWN;
		break;
	}
	
	END_TRY_CATCH

	return out;
}

long PTEID_Pin::getTriesLeft()
{
	long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getTriesLeft();
	
	END_TRY_CATCH

	return out;
}

unsigned long PTEID_Pin::getFlags()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getFlags();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Pin::getLabel()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getLabel();
	
	END_TRY_CATCH

	return out;
}

const char *PTEID_Pin::getLabelById( unsigned long id )
{
	const char *out;
    static const char* pinLabels[] = { "Unknown", "Authentication PIN", "Signature PIN", "Address PIN" };

    if ( ( id >= 1 ) && ( id <= 3 ) ){
        out = pinLabels[id];
    } else{
        out = pinLabels[0];
    }

	return out;
}

bool PTEID_Pin::verifyPin()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	unsigned long ulRemaining=0;
	out = pimpl->verifyPin("",ulRemaining);
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg, void *wndGeometry )
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->verifyPin(csPin,ulRemaining,bShowDlg, wndGeometry );
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg, void *wndGeometry)
{

	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->changePin(csPin1,csPin2,ulRemaining, PinName, bShowDlg, wndGeometry );
	
	END_TRY_CATCH

	return out;
}

bool PTEID_Pin::changePin()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	unsigned long ulRemaining=0;
	out = pimpl->changePin("","",ulRemaining, "");
	
	END_TRY_CATCH

	return out;
}
/*****************************************************************************************
---------------------------------------- PTEID_Pins -------------------------------------------
*****************************************************************************************/
PTEID_Pins::PTEID_Pins(const SDK_Context *context,APL_Pins *impl):PTEID_Crypto(context,impl)
{
}

PTEID_Pins::~PTEID_Pins()
{
}

unsigned long PTEID_Pins::count()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pins *pimpl=static_cast<APL_Pins *>(m_impl);
	out = pimpl->count();
	
	END_TRY_CATCH

	return out;
}

PTEID_Pin &PTEID_Pins::getPinByNumber(unsigned long ulIndex)
{
	PTEID_Pin *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pins *pimpl=static_cast<APL_Pins *>(m_impl);

	out = dynamic_cast<PTEID_Pin *>(getObject(INCLUDE_OBJECT_FIRSTPIN+ulIndex));

	if(!out)
	{
		out = new PTEID_Pin(m_context,pimpl->getPinByNumber(ulIndex));
		if(out)
			m_objects[INCLUDE_OBJECT_FIRSTPIN+ulIndex]=out;
			else
			throw PTEID_ExParamRange();
	}
	
	END_TRY_CATCH

	return *out;
}

PTEID_Pin &PTEID_Pins::getPinByPinRef(unsigned long pinRef)
{
	PTEID_Pin *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pins *pimpl=static_cast<APL_Pins *>(m_impl);

	// offset the PTEID_Pin
	out = dynamic_cast<PTEID_Pin *>(getObject(INCLUDE_OBJECT_FIRSTPINREF+pinRef));

	if(!out)
	{
		out = new PTEID_Pin(m_context,pimpl->getPinByPinRef(pinRef));
		if(out)
			m_objects[INCLUDE_OBJECT_FIRSTPINREF+pinRef]=out;
		else
			throw PTEID_ExParamRange();
	}

	END_TRY_CATCH

	return *out;
}

}
