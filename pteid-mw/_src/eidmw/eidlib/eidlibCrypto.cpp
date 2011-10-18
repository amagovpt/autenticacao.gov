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
#define INCLUDE_OBJECT_CRL			4
#define INCLUDE_OBJECT_OCSP			5
#define INCLUDE_OBJECT_PINSIGN		6
#define	INCLUDE_OBJECT_CACERT		7
#define	INCLUDE_OBJECT_AUTHCERT		8
#define	INCLUDE_OBJECT_SIGNCERT		9

#define	INCLUDE_OBJECT_FIRSTPIN		1000
#define	INCLUDE_OBJECT_FIRSTCERT	2000
#define	INCLUDE_OBJECT_FIRSTCHILD	3000

#define ANY_INDEX ULONG_MAX

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- BEID_Certificate ------------------------------------
*****************************************************************************************/
BEID_Certificate::BEID_Certificate(const SDK_Context *context,APL_Certif *impl):BEID_Crypto(context,impl)
{
}

BEID_Certificate::~BEID_Certificate()
{
}

const char *BEID_Certificate::getLabel()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getLabel();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Certificate::getID()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getID();
	
	END_TRY_CATCH

	return out;
}

BEID_CertifStatus BEID_Certificate::getStatus()
{
	BEID_CertifStatus out =BEID_CERTIF_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertStatus(pimpl->getStatus());
	
	END_TRY_CATCH

	return out;
}

BEID_CertifStatus BEID_Certificate::getStatus(BEID_ValidationLevel crl, BEID_ValidationLevel ocsp)
{
	BEID_CertifStatus out =BEID_CERTIF_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertStatus(pimpl->getStatus(ConvertValidationLevel(crl), ConvertValidationLevel(ocsp)));
	
	END_TRY_CATCH

	return out;
}

BEID_CertifType BEID_Certificate::getType()
{
	BEID_CertifType out =BEID_CERTIF_TYPE_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertType(pimpl->getType());
	
	END_TRY_CATCH

	return out;
}

const BEID_ByteArray& BEID_Certificate::getCertData()
{
	BEID_ByteArray *out = NULL;
	
	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<BEID_ByteArray *>(getObject(INCLUDE_OBJECT_CERTDATA));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pbytearray=dynamic_cast<BEID_ByteArray *>(getObject(INCLUDE_OBJECT_CERTDATA));
		//if(!pbytearray)
		//{
			out = new BEID_ByteArray(m_context,pimpl->getData());
			if(out)
				m_objects[INCLUDE_OBJECT_CERTDATA]=out;
			else
				throw BEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

void BEID_Certificate::getFormattedData(BEID_ByteArray &data)
{
	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	CByteArray bytearray;

	pimpl->getFormattedData(bytearray);
	data=bytearray;
	
	END_TRY_CATCH

	return;
}

bool BEID_Certificate::isRoot()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isRoot();
	
	END_TRY_CATCH

	return out;
}

bool BEID_Certificate::isTest()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isTest();
	
	END_TRY_CATCH

	return out;
}

bool BEID_Certificate::isFromBeidValidChain()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isFromBeidValidChain();
	
	END_TRY_CATCH

	return out;
}

bool BEID_Certificate::isFromCard()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->isFromCard();
	
	END_TRY_CATCH

	return out;
}

BEID_Certificate &BEID_Certificate::getIssuer()
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pIssuer=dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));
		//if(!pIssuer)
		//{
			APL_Certif *aplIssuer=pimpl->getIssuer();
			if(aplIssuer)
			{
				out = new BEID_Certificate(m_context,aplIssuer);
				if(out)
					m_objects[INCLUDE_OBJECT_CERTISSUER]=out;
				else
					throw BEID_ExUnknown();
			}
			else
			{
				throw BEID_ExCertNoIssuer();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

unsigned long BEID_Certificate::countChildren()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->countChildren();
	
	END_TRY_CATCH

	return out;
}

BEID_Certificate &BEID_Certificate::getChildren(unsigned long ulIndex)
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCHILD+ulIndex));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pChild=dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCHILD+ulIndex));
		//if(!pChild)
		//{
			APL_Certif *aplChild=pimpl->getChildren(ulIndex);
			if(aplChild)
			{
				out = new BEID_Certificate(m_context,aplChild);
				if(out)
					m_objects[INCLUDE_OBJECT_FIRSTCHILD+ulIndex]=out;
				else
					throw BEID_ExUnknown();
			}
			else
			{
				throw BEID_ExParamRange();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

BEID_Crl &BEID_Certificate::getCRL()
{
	BEID_Crl *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<BEID_Crl *>(getObject(INCLUDE_OBJECT_CRL));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pCrl=dynamic_cast<BEID_Crl *>(getObject(INCLUDE_OBJECT_CRL));
		//if(!pCrl)
		//{
			APL_Crl *aplCrl=pimpl->getCRL();
			if(aplCrl)
			{
				out = new BEID_Crl(m_context,aplCrl);
				if(out)
					m_objects[INCLUDE_OBJECT_CRL]=out;
				else
					throw BEID_ExUnknown();
			}
			else
			{
				throw BEID_ExCertNoCrl();
			}
		//}
	}

	END_TRY_CATCH

	return *out;
}

BEID_OcspResponse &BEID_Certificate::getOcspResponse()
{
	BEID_OcspResponse *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);

	out = dynamic_cast<BEID_OcspResponse *>(getObject(INCLUDE_OBJECT_OCSP));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pOcsp=dynamic_cast<BEID_OcspResponse *>(getObject(INCLUDE_OBJECT_OCSP));
		//if(!pOcsp)
		//{
			APL_OcspResponse *aplOcsp=pimpl->getOcspResponse();
			if(aplOcsp)
			{
				out = new BEID_OcspResponse(m_context,aplOcsp);
				if(out)
					m_objects[INCLUDE_OBJECT_OCSP]=out;
				else
					throw BEID_ExUnknown();
			}
			else
			{
				throw BEID_ExCertNoOcsp();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

const char *BEID_Certificate::getSerialNumber()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getSerialNumber();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Certificate::getOwnerName()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getOwnerName();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Certificate::getIssuerName()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getIssuerName();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Certificate::getValidityBegin()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getValidityBegin();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Certificate::getValidityEnd()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getValidityEnd();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Certificate::getKeyLength()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = pimpl->getKeyLength();
	
	END_TRY_CATCH

	return out;
}

BEID_CertifStatus  BEID_Certificate::verifyCRL(bool forceDownload)
{
	BEID_CertifStatus out = BEID_CERTIF_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertStatus(pimpl->verifyCRL(forceDownload));
	
	END_TRY_CATCH

	return out;
}

BEID_CertifStatus BEID_Certificate::verifyOCSP()
{
	BEID_CertifStatus out = BEID_CERTIF_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Certif *pimpl=static_cast<APL_Certif *>(m_impl);
	out = ConvertCertStatus(pimpl->verifyOCSP());
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
---------------------------------------- BEID_Certificates ------------------------------------
*****************************************************************************************/
BEID_Certificates::BEID_Certificates(const SDK_Context *context,APL_Certifs *impl):BEID_Crypto(context,impl)
{
}

BEID_Certificates::BEID_Certificates():BEID_Crypto(NULL,new APL_Certifs())
{
	m_delimpl=true;
}

BEID_Certificates::~BEID_Certificates()
{
}

unsigned long BEID_Certificates::countFromCard()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);
	out = pimpl->countFromCard();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Certificates::countAll()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);
	out = pimpl->countAll(true);
	
	END_TRY_CATCH

	return out;
}

BEID_Certificate &BEID_Certificates::getCertFromCard(unsigned long ulIndex)
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	out = dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCERT+ulIndex));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_FIRSTCERT+ulIndex));
		//if(!pcert)
		//{
			out = new BEID_Certificate(m_context,pimpl->getCertFromCard(ulIndex));
			if(out)
				m_objects[INCLUDE_OBJECT_FIRSTCERT+ulIndex]=out;
			else
				throw BEID_ExParamRange();

		//}
	}
	
	END_TRY_CATCH

	return *out;
}

BEID_Certificate &BEID_Certificates::getCert(unsigned long ulIndex)
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	APL_Certif *pAplCert=pimpl->getCert(ulIndex,true);

	out = dynamic_cast<BEID_Certificate *>(getObject(pAplCert));
	
	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<BEID_Certificate *>(getObject(pAplCert));
		//if(!pcert)
		//{
			out = new BEID_Certificate(m_context,pAplCert);
			if(out)
				addObject(out);
			else
				throw BEID_ExParamRange();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

BEID_Certificate &BEID_Certificates::getCert(BEID_CertifType type)
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	unsigned long idxObject;
	APL_CertifType aplType;
	bool bOnlyVisible=true;
	std::cout << "getcert 1 \n";
 	switch(type)
	{
		case BEID_CERTIF_TYPE_ROOT:
			std::cout << "root \n";
			idxObject=INCLUDE_OBJECT_ROOTCERT;
			aplType=APL_CERTIF_TYPE_ROOT;
			break;
		case BEID_CERTIF_TYPE_CA:
			idxObject=INCLUDE_OBJECT_CACERT;
			aplType=APL_CERTIF_TYPE_CA;
			break;
		case BEID_CERTIF_TYPE_AUTHENTICATION:
			idxObject=INCLUDE_OBJECT_AUTHCERT;
			aplType=APL_CERTIF_TYPE_AUTHENTICATION;
			break;
		case BEID_CERTIF_TYPE_SIGNATURE:
			idxObject=INCLUDE_OBJECT_SIGNCERT;
			aplType=APL_CERTIF_TYPE_SIGNATURE;
			break;
		default:
			throw BEID_ExBadUsage();
	}

	out = dynamic_cast<BEID_Certificate *>(getObject(idxObject));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//proot=dynamic_cast<BEID_Certificate *>(getObject(idxObject));
		//if(!proot)
		//{
			out = new BEID_Certificate(m_context,pimpl->getCert(aplType,ANY_INDEX,bOnlyVisible));
			if(out)
				m_objects[idxObject]=out;
			else
				throw BEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

BEID_Certificate &BEID_Certificates::getRoot()
{
	return getCert(BEID_CERTIF_TYPE_ROOT);
}

BEID_Certificate &BEID_Certificates::getCA()
{
	return getCert(BEID_CERTIF_TYPE_CA);
}

BEID_Certificate &BEID_Certificates::getAuthentication()
{
	return getCert(BEID_CERTIF_TYPE_AUTHENTICATION);
}

BEID_Certificate &BEID_Certificates::getSignature()
{
	return getCert(BEID_CERTIF_TYPE_SIGNATURE);
}

BEID_Certificate &BEID_Certificates::addCertificate(BEID_ByteArray &cert)
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Certifs *pimpl=static_cast<APL_Certifs *>(m_impl);

	CByteArray baCert(cert.GetBytes(),cert.Size());
	APL_Certif *pAplCert=pimpl->addCert(baCert);

	out = dynamic_cast<BEID_Certificate *>(getObject(pAplCert));
	
	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pcert=dynamic_cast<BEID_Certificate *>(getObject(pAplCert));
		//if(!pcert)
		//{
			out = new BEID_Certificate(m_context,pAplCert);
			if(out)
				addObject(out);
			else
				throw BEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}


/*****************************************************************************************
---------------------------------------- BEID_Pin -------------------------------------------
*****************************************************************************************/
BEID_Pin::BEID_Pin(const SDK_Context *context,APL_Pin *impl):BEID_Crypto(context,impl)
{
}

BEID_Pin::~BEID_Pin()
{
}

unsigned long BEID_Pin::getIndex()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getIndex();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Pin::getType()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getType();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Pin::getId()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getId();
	
	END_TRY_CATCH

	return out;
}

BEID_PinUsage BEID_Pin::getUsageCode()
{
	BEID_PinUsage out = BEID_PIN_USG_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	PinUsage usage=pimpl->getUsageCode();
	switch(usage)
	{
	case DLG_USG_PIN_AUTH:
		out = BEID_PIN_USG_AUTH;
		break;

	case DLG_USG_PIN_SIGN:
		out = BEID_PIN_USG_SIGN;
		break;

	case DLG_USG_PIN_ADDRESS:
		out = BEID_PIN_USG_ADDRESS;
		break;

	default:
		out = BEID_PIN_USG_UNKNOWN;
		break;
	}
	
	END_TRY_CATCH

	return out;
}

long BEID_Pin::getTriesLeft()
{
	long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getTriesLeft();
	
	END_TRY_CATCH

	return out;
}

unsigned long BEID_Pin::getFlags()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getFlags();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Pin::getLabel()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->getLabel();
	
	END_TRY_CATCH

	return out;
}

bool BEID_Pin::verifyPin()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	unsigned long ulRemaining=0;
	out = pimpl->verifyPin("",ulRemaining);
	
	END_TRY_CATCH

	return out;
}

bool BEID_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining)
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->verifyPin(csPin,ulRemaining);
	
	END_TRY_CATCH

	return out;
}

bool BEID_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName)
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);
	out = pimpl->changePin(csPin1,csPin2,ulRemaining, PinName);
	
	END_TRY_CATCH

	return out;
}

bool BEID_Pin::changePin()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	unsigned long ulRemaining=0;
	out = pimpl->changePin("","",ulRemaining, "");
	
	END_TRY_CATCH

	return out;
}

const BEID_ByteArray &BEID_Pin::getSignature()
{
	BEID_ByteArray *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pin *pimpl=static_cast<APL_Pin *>(m_impl);

	out = dynamic_cast<BEID_ByteArray *>(getObject(INCLUDE_OBJECT_PINSIGN));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pbytearray=dynamic_cast<BEID_ByteArray *>(getObject(INCLUDE_OBJECT_PINSIGN));
		//if(!pbytearray)
		//{
			out = new BEID_ByteArray(m_context,pimpl->getSignature());
			if(out)
				m_objects[INCLUDE_OBJECT_PINSIGN]=out;
			else
				throw BEID_ExUnknown();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}
/*****************************************************************************************
---------------------------------------- BEID_Pins -------------------------------------------
*****************************************************************************************/
BEID_Pins::BEID_Pins(const SDK_Context *context,APL_Pins *impl):BEID_Crypto(context,impl)
{
}

BEID_Pins::~BEID_Pins()
{
}

unsigned long BEID_Pins::count()
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	APL_Pins *pimpl=static_cast<APL_Pins *>(m_impl);
	out = pimpl->count();
	
	END_TRY_CATCH

	return out;
}

BEID_Pin &BEID_Pins::getPinByNumber(unsigned long ulIndex)
{
	BEID_Pin *out = NULL;

	BEGIN_TRY_CATCH

	APL_Pins *pimpl=static_cast<APL_Pins *>(m_impl);

	out = dynamic_cast<BEID_Pin *>(getObject(INCLUDE_OBJECT_FIRSTPIN+ulIndex));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//ppin=dynamic_cast<BEID_Pin *>(getObject(INCLUDE_OBJECT_FIRSTPIN+ulIndex));
		//if(!ppin)
		//{
			out = new BEID_Pin(m_context,pimpl->getPinByNumber(ulIndex));
			if(out)
				m_objects[INCLUDE_OBJECT_FIRSTPIN+ulIndex]=out;
			else
				throw BEID_ExParamRange();
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

/*****************************************************************************************
---------------------------------------- BEID_Crl ------------------------------------
*****************************************************************************************/
BEID_Crl::BEID_Crl(const SDK_Context *context,APL_Crl *impl):BEID_Object(context,impl)
{
}

BEID_Crl::BEID_Crl(const char *uri):BEID_Object(NULL,new APL_Crl(uri))
{
	m_delimpl=true;
}

BEID_Crl::~BEID_Crl()
{
	if(m_delimpl && m_impl)
	{
		APL_Crl *pimpl=static_cast<APL_Crl *>(m_impl);
		delete pimpl;
		m_impl=NULL;
	}
}

const char *BEID_Crl::getUri()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Crl *pimpl=static_cast<APL_Crl *>(m_impl);
	out = pimpl->getUri();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_Crl::getIssuerName()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_Crl *pimpl=static_cast<APL_Crl *>(m_impl);
	out = pimpl->getIssuerName();
	
	END_TRY_CATCH

	return out;
}

BEID_Certificate &BEID_Crl::getIssuer()
{
	BEID_Certificate *out = NULL;

	BEGIN_TRY_CATCH

	APL_Crl *pimpl=static_cast<APL_Crl *>(m_impl);

	out = dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));

	if(!out)
	{
		//CAutoMutex autoMutex(m_mutex);

		//pIssuer=dynamic_cast<BEID_Certificate *>(getObject(INCLUDE_OBJECT_CERTISSUER));
		//if(!pIssuer)
		//{
			APL_Certif *aplIssuer=pimpl->getIssuer();
			if(aplIssuer)
			{
				out = new BEID_Certificate(m_context,aplIssuer);
				if(out)
					m_objects[INCLUDE_OBJECT_CERTISSUER]=out;
				else
					throw BEID_ExUnknown();
			}
			else
			{
				throw BEID_ExCertNoIssuer();
			}
		//}
	}
	
	END_TRY_CATCH

	return *out;
}

BEID_CrlStatus BEID_Crl::getData(BEID_ByteArray &crl,bool bForceDownload)
{
	BEID_CrlStatus out = BEID_CRL_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_Crl *pimpl=static_cast<APL_Crl *>(m_impl);

	APL_CrlStatus eStatus;
	CByteArray bytearray;

	eStatus=pimpl->getData(bytearray,bForceDownload);
	crl=bytearray;

	out = ConvertCrlStatus(eStatus);
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
---------------------------------------- BEID_OcspResponse ------------------------------------
*****************************************************************************************/
BEID_OcspResponse::BEID_OcspResponse(const SDK_Context *context,APL_OcspResponse *impl):BEID_Object(context,impl)
{
}

BEID_OcspResponse::BEID_OcspResponse(const char *uri,BEID_HashAlgo hashAlgorithm,const BEID_ByteArray &issuerNameHash,const BEID_ByteArray &issuerKeyHash,const BEID_ByteArray &serialNumber):BEID_Object(NULL,NULL)
{
	CByteArray baIssuerNameHash(issuerNameHash.GetBytes(),issuerNameHash.Size());
	CByteArray baIssuerKeyHash(issuerKeyHash.GetBytes(),issuerKeyHash.Size());
	CByteArray baSerialNumber(serialNumber.GetBytes(),serialNumber.Size());

	m_impl = new APL_OcspResponse(uri,ConvertHashAlgo(hashAlgorithm),baIssuerNameHash,baIssuerKeyHash,baSerialNumber);

	m_delimpl=true;
}

BEID_OcspResponse::~BEID_OcspResponse()
{
	if(m_delimpl && m_impl)
	{
		APL_OcspResponse *pimpl=static_cast<APL_OcspResponse *>(m_impl);
		delete pimpl;
		m_impl=NULL;
	}
}

const char *BEID_OcspResponse::getUri()
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	APL_OcspResponse *pimpl=static_cast<APL_OcspResponse *>(m_impl);
	out = pimpl->getUri();
	
	END_TRY_CATCH

	return out;
}

BEID_CertifStatus BEID_OcspResponse::getResponse(BEID_ByteArray &response)
{
	BEID_CertifStatus out = BEID_CERTIF_STATUS_UNKNOWN;

	BEGIN_TRY_CATCH

	APL_OcspResponse *pimpl=static_cast<APL_OcspResponse *>(m_impl);

	APL_CertifStatus eStatus;
	CByteArray bytearray;

	eStatus=pimpl->getResponse(bytearray);
	response=bytearray;

	out = ConvertCertStatus(eStatus);

	END_TRY_CATCH

	return out;
}

}
