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
#pragma once

#ifndef __EIDLIB_EXCEPTION_H__
#define __EIDLIB_EXCEPTION_H__

#include "eidlibdefines.h"

namespace eIDMW
{

class CMWException;

/******************************************************************************//**
  * Base class for the BEID SDK Exceptions
  *********************************************************************************/
class BEID_Exception
{
public:
    BEIDSDK_API BEID_Exception(long lError);	/**< Constructor - Need error code that comes from eidErrors.h */
	BEIDSDK_API virtual ~BEID_Exception();		/**< Destructor */

    BEIDSDK_API long GetError() const;			/**< Return the error code */

	NOEXPORT_BEIDSDK static BEID_Exception THROWException(CMWException &e);

private:
	long m_lError;					/**< Error code of the exception (see eidErrors.h)*/
};

/******************************************************************************//**
  * Exception class Release Needed (error code = EIDMW_ERR_RELEASE_NEEDED)
  *
  *	Throw when the application close without calling the BEID_RealeaseSDK
  *********************************************************************************/
class BEID_ExReleaseNeeded: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExReleaseNeeded();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExReleaseNeeded();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Release Needed (error code = EIDMW_ERR_UNKNOWN)
  *********************************************************************************/
class BEID_ExUnknown: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExUnknown();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExUnknown();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Document Type Unknown (error code = EIDMW_ERR_DOCTYPE_UNKNOWN)
  *
  *	Throw when the document type doesn't exist for this card
  * Used in : - BEID_Card::getDocument(BEID_DocumentType type)
  *********************************************************************************/
class BEID_ExDocTypeUnknown: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExDocTypeUnknown();			/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExDocTypeUnknown();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class File Type Unknown (error code = EIDMW_ERR_FILETYPE_UNKNOWN)
  *
  *	Throw when the Raw Data type doesn't exist for this card
  * Used in : - BEID_Card::getRawData(BEID_RawDataType type)
  *********************************************************************************/
class BEID_ExFileTypeUnknown: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExFileTypeUnknown();			/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExFileTypeUnknown();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Parameter Range (error code = EIDMW_ERR_PARAM_RANGE)
  *
  *	Throw when the element ask is out of range
  * Used in : - BEID_ReaderSet::getReaderName(unsigned long ulIndex)
  *           - BEID_ReaderSet::getReaderByNum(unsigned long ulIndex)
  *           - BEID_Certificate::getChildren(unsigned long ulIndex)
  *           - BEID_Certificates::getCertByNumber(unsigned long ulIndex)
  *           - BEID_Pins::getPinByNumber(unsigned long ulIndex)
  *********************************************************************************/
class BEID_ExParamRange: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExParamRange();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExParamRange();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Command Not Allowed (error code = EIDMW_ERR_CMD_NOT_ALLOWED)
  *
  *	Throw when the command asked is not allowed
  * Used in : - BEID_SmartCard::writeFile(const char *fileID,const BEID_ByteArray &out,BEID_Pin *pin,const char *csPinCode)
  *********************************************************************************/
class BEID_ExCmdNotAllowed: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCmdNotAllowed();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCmdNotAllowed();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Command Not Supported (error code = EIDMW_ERR_NOT_SUPPORTED)
  *
  *	Throw when the command asked is not supported by the card
  *********************************************************************************/
class BEID_ExCmdNotSupported: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCmdNotSupported();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCmdNotSupported();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class No Card Present (error code = EIDMW_ERR_NO_CARD)
  *
  *	Throw when no card is present
  * Used in : - BEID_ReaderContext::getCardType()
  *           - BEID_ReaderContext::getCard()
  *           - BEID_ReaderContext::getEIDCard()
  *           - BEID_ReaderContext::getKidsCard()
  *           - BEID_ReaderContext::getForeignerCard()
  *           - BEID_ReaderContext::getSISCard()
  *********************************************************************************/
class BEID_ExNoCardPresent: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExNoCardPresent();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExNoCardPresent();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Card Type (error code = EIDMW_ERR_CARDTYPE_BAD)
  *
  *	Throw when the card type doesn't correspond to what is asked
  * Used in : - BEID_ReaderContext::getEIDCard()
  *           - BEID_ReaderContext::getKidsCard()
  *           - BEID_ReaderContext::getForeignerCard()
  *           - BEID_ReaderContext::getSISCard()
  *********************************************************************************/
class BEID_ExCardBadType: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCardBadType();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCardBadType();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Card Type Unknown (error code = EIDMW_ERR_CARDTYPE_UNKNOWN)
  *
  *	Throw when the card type is not supported
  * Used in : - BEID_ReaderContext::getCard()
  *********************************************************************************/
class BEID_ExCardTypeUnknown: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCardTypeUnknown();			/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCardTypeUnknown();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Certificate No Issuer (error code = EIDMW_ERR_CERT_NOISSUER)
  *
  *	Throw when ask for the issuer of a root certificate
  * Used in : - BEID_Certificate::getIssuer()
  *********************************************************************************/
class BEID_ExCertNoIssuer: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCertNoIssuer();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCertNoIssuer();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Certificate No CRL (error code = EIDMW_ERR_CERT_NOCRL)
  *
  *	Throw when ask for the CRL when the certificate contains no CDP
  * Used in : - BEID_Certificate::getCrl()
  *********************************************************************************/
class BEID_ExCertNoCrl: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCertNoCrl();					/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCertNoCrl();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Certificate No OCSP responder (error code = EIDMW_ERR_CERT_NOOCSP)
  *
  *	Throw when ask for the OCSP response when there is no responder
  * Used in : - BEID_Certificate::getOcspResponse()
  *********************************************************************************/
class BEID_ExCertNoOcsp: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCertNoOcsp();					/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCertNoOcsp();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Certificate No OCSP responder (error code = EIDMW_ERR_CERT_NOROOT)
  *
  *	Throw when ask for the Root
  * Used in : - BEID_Certificate::getRoot()
  *********************************************************************************/
class BEID_ExCertNoRoot: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCertNoRoot();					/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCertNoRoot();			/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Usage (error code = EIDMW_ERR_BAD_USAGE)
  *
  *	Throw when a class/function is not used correctly
  * Used in : - BEID_Crl class
  *********************************************************************************/
class BEID_ExBadUsage: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExBadUsage();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExBadUsage();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Transaction (error code = EIDMW_ERR_BAD_TRANSACTION)
  *
  *	Throw when a class/function is not used correctly
  * Used in : - BEID_ReaderContext::Lock()
  *			  - BEID_ReaderContext::Unlock()
  *********************************************************************************/
class BEID_ExBadTransaction: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExBadTransaction();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExBadTransaction();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Card changed (error code = EIDMW_ERR_CARD_CHANGED)
  *
  *	Throw when accessing to a card object but the card has been changed
  * Used in : - BEID_Object::checkContextStillOk()
  *********************************************************************************/
class BEID_ExCardChanged: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExCardChanged();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExCardChanged();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Readerset changed (error code = EIDMW_ERR_READERSET_CHANGED)
  *
  *	Throw when accessing to a card object but the readerset has been changed
  * Used in : - BEID_Object::checkContextStillOk()
  *********************************************************************************/
class BEID_ExReaderSetChanged: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExReaderSetChanged();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExReaderSetChanged();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class No reader (error code = EIDMW_ERR_NO_READER)
  *
  *	Throw when the reader set is empty
  * Used in : - BEID_Object::checkContextStillOk()
  *********************************************************************************/
class BEID_ExNoReader: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExNoReader();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExNoReader();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Not allow by user (error code = EIDMW_ERR_NOT_ALLOW_BY_USER)
  *
  *	Throw when the data could not be read from the card because the user do not allow
  *********************************************************************************/
class BEID_ExNotAllowByUser: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExNotAllowByUser();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExNotAllowByUser();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class User must answer (error code = EIDMW_ERR_USER_MUST_ANSWER)
  *
  *	Throw when the user did not answer yet to allow or disallow reading the data from the card
  *********************************************************************************/
class BEID_ExUserMustAnswer: public BEID_Exception
{
public:
	BEIDSDK_API BEID_ExUserMustAnswer();				/**< Constructor */
	BEIDSDK_API virtual ~BEID_ExUserMustAnswer();		/**< Destructor */
};
}

#endif //__EIDLIB_EXCEPTION_H__
