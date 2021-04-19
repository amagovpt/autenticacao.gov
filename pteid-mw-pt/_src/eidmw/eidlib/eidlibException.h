/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
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

#include <string>
#include "eidlibdefines.h"

namespace eIDMW
{

class CMWException;

/******************************************************************************//**
  * Base class for the PTEID SDK Exceptions
  *********************************************************************************/
class PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_Exception(long lError);	/**< Constructor - Need error code that comes from eidErrors.h */
	PTEIDSDK_API virtual ~PTEID_Exception();		/**< Destructor */

	PTEIDSDK_API long GetError() const;			/**< Return the error code */
	PTEIDSDK_API const char * GetMessage();		        /**< Returns the error message based on the error code */

	NOEXPORT_PTEIDSDK static PTEID_Exception THROWException(CMWException &e);

private:
	long m_lError;					/**< Error code of the exception (see eidErrors.h)*/
	std::string error_message;
};

/******************************************************************************//**
  * Exception class Release Needed (error code = EIDMW_ERR_RELEASE_NEEDED)
  *
  *	Throw when the application close without calling the PTEID_RealeaseSDK
  *********************************************************************************/
class PTEID_ExReleaseNeeded: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExReleaseNeeded();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExReleaseNeeded();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Release Needed (error code = EIDMW_ERR_UNKNOWN)
  *********************************************************************************/
class PTEID_ExUnknown: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExUnknown();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExUnknown();		/**< Destructor */
};


/******************************************************************************//**
  * Exception class File Type Unknown (error code = EIDMW_ERR_FILETYPE_UNKNOWN)
  *
  *	Throw when the Raw Data type doesn't exist for this card
  * Used in : - PTEID_Card::getRawData(PTEID_RawDataType type)
  *********************************************************************************/
class PTEID_ExFileTypeUnknown: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExFileTypeUnknown();			/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExFileTypeUnknown();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Parameter Range (error code = EIDMW_ERR_PARAM_RANGE)
  *
  *	Throw when the element ask is out of range
  * Used in : - PTEID_ReaderSet::getReaderName(unsigned long ulIndex)
  *           - PTEID_ReaderSet::getReaderByNum(unsigned long ulIndex)
  *           - PTEID_Certificate::getChildren(unsigned long ulIndex)
  *           - PTEID_Certificates::getCertByNumber(unsigned long ulIndex)
  *           - PTEID_Pins::getPinByNumber(unsigned long ulIndex)
  *********************************************************************************/
class PTEID_ExParamRange: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExParamRange();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExParamRange();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Command Not Allowed (error code = EIDMW_ERR_CMD_NOT_ALLOWED)
  *
  *	Throw when the command asked is not allowed
  * Used in : - PTEID_SmartCard::writeFile(const char *fileID,const PTEID_ByteArray &out,PTEID_Pin *pin,const char *csPinCode)
  *********************************************************************************/
class PTEID_ExCmdNotAllowed: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExCmdNotAllowed();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExCmdNotAllowed();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Command Not Supported (error code = EIDMW_ERR_NOT_SUPPORTED)
  *
  *	Throw when the command asked is not supported by the card
  *********************************************************************************/
class PTEID_ExCmdNotSupported: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExCmdNotSupported();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExCmdNotSupported();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class No Card Present (error code = EIDMW_ERR_NO_CARD)
  *
  *	Throw when no card is present
  * Used in : - PTEID_ReaderContext::getCardType()
  *           - PTEID_ReaderContext::getCard()
  *           - PTEID_ReaderContext::getEIDCard()
  *********************************************************************************/
class PTEID_ExNoCardPresent: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExNoCardPresent();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExNoCardPresent();	/**< Destructor */
};

/******************************************************************************//**
  * Exception class Card Type Unknown (error code = EIDMW_ERR_CARDTYPE_UNKNOWN)
  *
  *	Throw when the card type is not supported
  * Used in : - PTEID_ReaderContext::getCard()
  *********************************************************************************/
class PTEID_ExCardTypeUnknown: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExCardTypeUnknown();			/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExCardTypeUnknown();	/**< Destructor */
};


/******************************************************************************//**
  * Exception class Certificate No OCSP responder (error code = EIDMW_ERR_CERT_NOROOT)
  *
  *	Throw when ask for the Root
  * Used in : - PTEID_Certificate::getRoot()
  *********************************************************************************/
class PTEID_ExCertNoRoot: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExCertNoRoot();					/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExCertNoRoot();			/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Usage (error code = EIDMW_ERR_BAD_USAGE)
  *
  *	Throw when a class/function is not used correctly
  * Used in : - PTEID_Crl class
  *********************************************************************************/
class PTEID_ExBadUsage: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExBadUsage();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExBadUsage();		/**< Destructor */
};

/******************************************************************************//**
  * Exception class Bad Transaction (error code = EIDMW_ERR_BAD_TRANSACTION)
  *
  *	Throw when a class/function is not used correctly
  * Used in : - PTEID_ReaderContext::Lock()
  *			  - PTEID_ReaderContext::Unlock()
  *********************************************************************************/
class PTEID_ExBadTransaction: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExBadTransaction();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExBadTransaction();		/**< Destructor */
};


/******************************************************************************//**
  * Exception class No reader (error code = EIDMW_ERR_NO_READER)
  *
  *	Throw when the reader set is empty
  * Used in : - PTEID_Object::checkContextStillOk()
  *********************************************************************************/
class PTEID_ExNoReader: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExNoReader();				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExNoReader();		/**< Destructor */
};

/******************************************************************************//**
 * Exception class SOD error error codes:
 *
 *  	EIDMW_SOD_UNEXPECTED_VALUE
 *		EIDMW_SOD_UNEXPECTED_ASN1_TAG
 *		EIDMW_SOD_UNEXPECTED_ALGO_OID
 *		EIDMW_SOD_ERR_HASH_NO_MATCH_ID
 *		EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS
 *		EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE
 *		EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY
 *
 *	Throw when the SOD is not correct
 *********************************************************************************/
class PTEID_ExSOD: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExSOD(long lError);				/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExSOD();				/**< Destructor */
};

/******************************************************************************//**
  * Exception class PTEID_ExBatchSignatureFailed error codes:
  *
  * All error codes related with pdf signatures (except timestamp/LTV errors), e.g.:
  *
  *		EIDMW_PDF_INVALID_ERROR
  *		EIDMW_PDF_UNSUPPORTED_ERROR
  *		EIDMW_PDF_INVALID_PAGE_ERROR
  *
  * Throw when batch signature fails.
  * Used in : - int PTEID_EIDCard::SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y, const char *location, const char *reason, const char *outfile_path)
  *
  * Use GetFailedSignatureIndex() to know which of the signatures(from the batch) failed.
  *********************************************************************************/
class PTEID_ExBatchSignatureFailed: public PTEID_Exception
{
public:
	PTEIDSDK_API PTEID_ExBatchSignatureFailed(long lError, unsigned int failedIndex);	/**< Constructor */
	PTEIDSDK_API virtual ~PTEID_ExBatchSignatureFailed();	/**< Destructor */

	unsigned int GetFailedSignatureIndex() const { return m_failedSignatureIndex; }; /**< Returns the index (within batch) of the failed signature */

private:
	long m_failedSignatureIndex;
};

//DEPRECATED EXCEPTION Classes: not currently used
class PTEID_ExCardBadType : public PTEID_Exception
{
	public:
		PTEIDSDK_API PTEID_ExCardBadType();				/**< Constructor */
		PTEIDSDK_API virtual ~PTEID_ExCardBadType();		/**< Destructor */
};

class PTEID_ExReaderSetChanged : public PTEID_Exception
{
	public:
		PTEIDSDK_API PTEID_ExReaderSetChanged();				/**< Constructor */
		PTEIDSDK_API virtual ~PTEID_ExReaderSetChanged();		/**< Destructor */
};

class PTEID_ExCardChanged: public PTEID_Exception
{
public:
  PTEIDSDK_API PTEID_ExCardChanged();       /**< Constructor */
  PTEIDSDK_API virtual ~PTEID_ExCardChanged();    /**< Destructor */
};

class PTEID_ExCertNoIssuer: public PTEID_Exception
{
public:
  PTEIDSDK_API PTEID_ExCertNoIssuer();        /**< Constructor */
  PTEIDSDK_API virtual ~PTEID_ExCertNoIssuer();   /**< Destructor */
};

class PTEID_ExDocTypeUnknown: public PTEID_Exception
{
public:
  PTEIDSDK_API PTEID_ExDocTypeUnknown();      /**< Constructor */
  PTEIDSDK_API virtual ~PTEID_ExDocTypeUnknown(); /**< Destructor */
};

}

#endif //__EIDLIB_EXCEPTION_H__
