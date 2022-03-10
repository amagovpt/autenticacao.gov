/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2020-2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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
#include "eidlibException.h"
#include "eidlib.h"
#include "MWException.h"
#include "eidErrors.h"

#include <iostream>


namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- PTEID_Exception --------------------------------------
*****************************************************************************************/
PTEID_Exception::PTEID_Exception(long lError):m_lError(lError)
{
}

PTEID_Exception::~PTEID_Exception()
{
}

long PTEID_Exception::GetError() const
{
	return m_lError;
}

/*	Returns the corresponding error message for a given error code
	the error codes are defined in common/eidErrors.h,
	update GetMessage function when more errors have been added or removed
*/
const char* PTEID_Exception::GetMessage()
{
#ifdef EIDMW_JAVA_WRAPPER
	long mask_low_32bit = 0xFFFFFFFF;
	//Clear the 32 MSB which can be set to 1 from a signed int to long conversion
	m_lError &= mask_low_32bit;
#endif
	if (!m_lError) {
		error_message = "No error code was provided";
	} else {
		switch(m_lError) {

			// Results of calling the function with incorrect parameters
			case EIDMW_ERR_PARAM_BAD:
				error_message = "A function parameter has an unexpected value";
				break;
			case EIDMW_ERR_PARAM_RANGE:
				error_message = "A function parameter exceeded the allowed range";
				break;
			case EIDMW_ERR_BAD_PATH:
				error_message = "Bad file path (invalid characters, length no multiple of 4) ";
				break;
			case EIDMW_ERR_ALGO_BAD:
				error_message = "Unknown/unsupported algorithm";
				break;
			case EIDMW_ERR_PIN_OPERATION:
				error_message = "Invalid/unsupported PIN operation";
				break;
			case EIDMW_ERR_PIN_FORMAT:
				error_message = "PIN not allowed for this card (invalid characters, too short/long)";
				break;
			case EIDMW_ERR_NOT_IMPLEMENTED:
				error_message = "The function called is not implemented.";
				break;

			// Card errors
			case EIDMW_ERR_CARD:
				error_message = "Generic card error";
				break;
			case EIDMW_ERR_NOT_AUTHENTICATED:
				error_message = "Not Authenticated (no PIN specified)";
				break;
			case EIDMW_ERR_NOT_SUPPORTED:
				error_message = "This command is not supported by this card";
				break;
			case EIDMW_ERR_PIN_BAD:
				error_message = "PIN is incorrect";
				break;
			case EIDMW_ERR_PIN_BLOCKED:
				error_message = "PIN is blocked";
				break;
			case EIDMW_ERR_NO_CARD:
				error_message = "No card present or card has been removed";
				break;
			case EIDMW_ERR_BAD_P1P2:
				error_message = "Bad parameter P1 or P2";
				break;
			case EIDMW_ERR_CMD_NOT_ALLOWED:
				error_message = "Command not allowed";
				break;
			case EIDMW_ERR_FILE_NOT_FOUND:
				error_message = "File not found";
				break;
			case EIDMW_ERR_APPLET_VERSION_NOT_FOUND:
				error_message = "Unable to read applet version from the card";
				break;

			// Reader errors
			case EIDMW_ERR_CARD_COMM:
				error_message = "Error communicating with the card";
				break;
			case EIDMW_ERR_NO_READER:
				error_message = "No reader has been found";
				break;
			case EIDMW_ERR_PINPAD:
				error_message = "The pinpad reader returned an error";
				break;
			case EIDMW_ERR_CANT_CONNECT:
				error_message = "A card is present but unable to connect";
				break;
			case EIDMW_ERR_CARD_RESET:
				error_message = "The card connection has been reset";
				break;
			case EIDMW_ERR_CARD_SHARING:
				error_message = "The card can't be accessed because of other connections";
				break;
			case EIDMW_ERR_NOT_TRANSACTED:
				error_message = "Can't end a transaction that wasn't started";
				break;

			// Internal errors
			case EIDMW_ERR_LIMIT:
				error_message = "An internal limit has been reached";
				break;
			case EIDMW_ERR_CHECK:
				error_message = "An internal check failed";
				break;
			case EIDMW_ERR_UNKNOWN:
				error_message = "An unknown error occurred";
				break;
			case EIDMW_PINPAD_ERR:
				error_message = "The pinpad reader received a wrong/unknown value";
				break;
			case EIDMW_CANT_LOAD_LIB:
				error_message = "Dynamic library couldn't be loaded (not found at the specified location)";
				break;
			case EIDMW_ERR_MEMORY:
				error_message = "Memory error";
				break;
			case EIDMW_ERR_DELETE_CACHE:
				error_message = "Couldn't delete cache file(s)";
				break;
			case EIDMW_CONF:
				error_message = "Error getting or writing config data";
				break;
			case EIDMW_CACHE_TAMPERED:
				error_message = "The cached files have been tampered with";
				break;

			// User errors/events
			case EIDMW_ERR_PIN_CANCEL:
				error_message = "User pressed Cancel in PIN dialog";
				break;
			case EIDMW_ERR_TIMEOUT:
				error_message = "Pinpad timeout";
				break;
			case EIDMW_NEW_PINS_DIFFER:
				error_message = "The new PINs that were entered differ";
				break;
			case EIDMW_WRONG_PIN_FORMAT:
				error_message = "A PIN with invalid length or format was entered";
				break;
			case EIDMW_ERR_OP_CANCEL:
				error_message = "User canceled the operation.";
				break;

			// Parser errors
			case EIDMW_WRONG_ASN1_FORMAT:
				error_message = "Could not find expected elements in parsed ASN.1 vector";
				break;
			case EIDMW_XERCES_INIT_ERROR:
				error_message = "Could not initialize Xerces-C library";
				break;

			// I/O errors
			case EIDMW_FILE_NOT_OPENED:
				error_message = "File could not be opened";
				break;
			case EIDMW_PERMISSION_DENIED:
				error_message = "Search permission is denied for a component of the path prefix of path";
				break;
			case EIDMW_INVALID_PATH:
				error_message = "The path provided is invalid";
				break;
			case EIDMW_TOO_MANY_OPENED_FILES:
				error_message = "Too many files are currently open in the system";
				break;
			case EIDMW_ERR_LOGGER_APPLEAVING:
				error_message = "Call of the Logger after destruct time";
				break;
			case EIDMW_ERR_CARDTYPE_UNKNOWN:
				error_message = "This card type is unknown";
				break;
			case EIDMW_ERR_RELEASE_NEEDED:
				error_message = "No release of SDK object has been done before closing the application";
				break;
			case EIDMW_ERR_BAD_TRANSACTION:
				error_message = "Illegal access to a resource protected by locks / transactions";
				break;

			//SOD related errors
			case EIDMW_SOD_UNEXPECTED_VALUE:
				error_message = "SOD Error: unexpected value";
				break;
			case EIDMW_SOD_UNEXPECTED_ASN1_TAG:
				error_message = "SOD Error: unexpected ASN.1 tag";
				break;
			case EIDMW_SOD_UNEXPECTED_ALGO_OID:
				error_message = "SOD Error: unexpected algorithm ID";
				break;
			case EIDMW_SOD_ERR_HASH_NO_MATCH_ID:
				error_message = "SOD Error: inconsistent identity data";
				break;
			case EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS:
				error_message = "SOD Error: inconsistent address file";
				break;
			case EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE:
				error_message = "SOD Error: inconsistent picture data";
				break;
			case EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY:
				error_message = "SOD Error: inconsistent public key";
				break;
			case EIDMW_SOD_ERR_VERIFY_SOD_SIGN:
				error_message = "SOD Error: invalid signature";
				break;

			// OTP related errors
			case EIDMW_OTP_CONNECTION_ERROR:
				error_message = "OTP Error: generic connection error";
				break;
			case EIDMW_OTP_PROTOCOL_ERROR:
				error_message = "OTP Error: protocol error";
				break;
			case EIDMW_OTP_CERTIFICATE_ERROR:
				error_message = "OTP Error: certificate verification failed";
				break;
			case EIDMW_OTP_UNKNOWN_ERROR:
				error_message = "OTP Error: unknown error";
				break;

			// SAM related errors
			case EIDMW_SAM_CONNECTION_ERROR:
				error_message = "SAM Error: server connection error";
				break;
			case EIDMW_SAM_PROTOCOL_ERROR:
				error_message = "SAM Error: unexpected server response";
				break;
			case EIDMW_SAM_UNKNOWN_ERROR:
				error_message = "SAM Error: generic error";
				break;
			case EIDMW_SSL_PROTOCOL_ERROR:
				error_message = "SAM Error: connection SSL error";
				break;
			case EIDMW_SAM_UNSUPPORTED_CARD:
				error_message = "SAM Error: unsupported card";
				break;
			case EIDMW_SAM_UNCONFIRMED_CHANGE:
				error_message = "SAM Error: address change unconfirmed by the server";
				break;
			case EIDMW_SAM_PROXY_AUTH_FAILED:
				error_message = "SAM Error: proxy server authentication failure";
				break;
			case EIDMW_SAM_PROXY_UNSUPPORTED:
				error_message = "SAM Error: change address proxy server is not supported";
				break;

			//Errors related to CVC authentication and Secure Messaging
			case EIDMW_ERR_CVC_GENERIC_ERROR:
				error_message = "CVC Error: generic error";
				break;
			case EIDMW_ERR_CVC_PERMISSION_DENIED:
				error_message = "CVC Error: permission denied";
				break;
			case EIDMW_ERR_CVC_PATH_NOT_FOUND:
				error_message = "CVC Error: file not found";
				break;

			case EIDMW_ERR_CERT_NOCRL:
				error_message = "This Certificate has no CRL";
				break;
			case EIDMW_ERR_CERT_NOOCSP:
				error_message = "This Certificate has no OCSP responder";
				break;
			case EIDMW_ERR_CERT_NOROOT:
				error_message = "No root is found for the store or certificate";
				break;
			case EIDMW_ERR_USER_MUST_ANSWER:
				error_message = "User did not yet allow or disallow to read the card";
				break;

			// Signature related errors
			case EIDMW_TIMESTAMP_ERROR:
				error_message = "Timestamp error during signature";
				break;
			case EIDMW_LTV_ERROR:
				error_message = "Error adding LTV to signature.";
				break;
			case EIDMW_PDF_INVALID_ERROR:
				error_message = "Invalid PDF document";
				break;
			case EIDMW_PDF_UNSUPPORTED_ERROR:
				error_message = "Unsupported PDF document";
				break;
			case EIDMW_PDF_INVALID_PAGE_ERROR:
				error_message = "Invalid page on PDF document";
				break;
			case EIDMW_XADES_UNKNOWN_ERROR:
				error_message = "XAdES error: unknown error";
				break;

			case EIDMW_ERR_CMD_BAD_CREDENTIALS:
				error_message = "CMD credentials are not correctly configured.";
				break;
			case EIDMW_ERR_CMD_CONNECTION:
				error_message = "Connection to CMD services failed.";
				break;
			case EIDMW_ERR_CMD_INVALID_CODE:
				error_message = "The PIN or security code introduced is incorrect.";
				break;
			case EIDMW_ERR_CMD_INACTIVE_ACCOUNT:
				error_message = "The CMD account is expired or inactive.";
				break;
			
			default:
				error_message = "Unmapped error: " + std::to_string(m_lError);
				break;
		}
	}
	return error_message.c_str();
}

PTEID_Exception PTEID_Exception::THROWException(CMWException &e)
{
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidlib",
		"PTEID_Exception generated from %s:%ld error code: %08x", e.GetFile().c_str(), e.GetLine(), e.GetError());

	switch(e.GetError())
	{
	case EIDMW_ERR_RELEASE_NEEDED:
		throw PTEID_ExReleaseNeeded();
	case EIDMW_ERR_UNKNOWN:
		throw PTEID_ExUnknown();
	case EIDMW_ERR_PARAM_RANGE:
		throw PTEID_ExParamRange();
	case EIDMW_ERR_CMD_NOT_ALLOWED:
		throw PTEID_ExCmdNotAllowed();
	case EIDMW_ERR_NOT_SUPPORTED:
		throw PTEID_ExCmdNotSupported();
	case EIDMW_ERR_NO_CARD:
		throw PTEID_ExNoCardPresent();
	case EIDMW_ERR_CARDTYPE_UNKNOWN:
		throw PTEID_ExCardTypeUnknown();
	case EIDMW_ERR_CERT_NOROOT:
		throw PTEID_ExCertNoRoot();
	case EIDMW_ERR_BAD_USAGE:
		throw PTEID_ExBadUsage();
	case EIDMW_ERR_BAD_TRANSACTION:
		throw PTEID_ExBadTransaction();
	case EIDMW_ERR_NO_READER:
		throw PTEID_ExNoReader();
	case EIDMW_SOD_UNEXPECTED_VALUE:
	case EIDMW_SOD_UNEXPECTED_ASN1_TAG:
	case EIDMW_SOD_UNEXPECTED_ALGO_OID:
	case EIDMW_SOD_ERR_HASH_NO_MATCH_ID:
	case EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS:
	case EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE:
	case EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY:
	case EIDMW_SOD_ERR_VERIFY_SOD_SIGN:
		throw PTEID_ExSOD(e.GetError());

	case EIDMW_ERR_CARD:
	case EIDMW_ERR_TIMEOUT:
	case EIDMW_ERR_PIN_CANCEL:
	case EIDMW_NEW_PINS_DIFFER:
	case EIDMW_WRONG_PIN_FORMAT:
	case EIDMW_PINPAD_ERR:
	default:
		throw PTEID_Exception(e.GetError());
	}
}

/*****************************************************************************************
---------------------------------------- PTEID_ExReleaseNeeded --------------------------------------
*****************************************************************************************/
PTEID_ExReleaseNeeded::PTEID_ExReleaseNeeded():PTEID_Exception(EIDMW_ERR_RELEASE_NEEDED)
{
}

PTEID_ExReleaseNeeded::~PTEID_ExReleaseNeeded()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExUnknown --------------------------------------
*****************************************************************************************/
PTEID_ExUnknown::PTEID_ExUnknown():PTEID_Exception(EIDMW_ERR_UNKNOWN)
{
}

PTEID_ExUnknown::~PTEID_ExUnknown()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExParamRange --------------------------------------
*****************************************************************************************/
PTEID_ExParamRange::PTEID_ExParamRange():PTEID_Exception(EIDMW_ERR_PARAM_RANGE)
{
}

PTEID_ExParamRange::~PTEID_ExParamRange()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCmdNotAllowed --------------------------------------
*****************************************************************************************/
PTEID_ExCmdNotAllowed::PTEID_ExCmdNotAllowed():PTEID_Exception(EIDMW_ERR_CMD_NOT_ALLOWED)
{
}

PTEID_ExCmdNotAllowed::~PTEID_ExCmdNotAllowed()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCmdNotSupported --------------------------------------
*****************************************************************************************/
PTEID_ExCmdNotSupported::PTEID_ExCmdNotSupported():PTEID_Exception(EIDMW_ERR_NOT_SUPPORTED)
{
}

PTEID_ExCmdNotSupported::~PTEID_ExCmdNotSupported()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExNoCardPresent --------------------------------------
*****************************************************************************************/
PTEID_ExNoCardPresent::PTEID_ExNoCardPresent():PTEID_Exception(EIDMW_ERR_NO_CARD)
{
}

PTEID_ExNoCardPresent::~PTEID_ExNoCardPresent()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCardTypeUnknown --------------------------------------
*****************************************************************************************/
PTEID_ExCardTypeUnknown::PTEID_ExCardTypeUnknown():PTEID_Exception(EIDMW_ERR_CARDTYPE_UNKNOWN)
{
}

PTEID_ExCardTypeUnknown::~PTEID_ExCardTypeUnknown()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCertNoRoot --------------------------------------
*****************************************************************************************/
PTEID_ExCertNoRoot::PTEID_ExCertNoRoot():PTEID_Exception(EIDMW_ERR_CERT_NOROOT)
{
}

PTEID_ExCertNoRoot::~PTEID_ExCertNoRoot()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExBadUsage --------------------------------------
*****************************************************************************************/
PTEID_ExBadUsage::PTEID_ExBadUsage():PTEID_Exception(EIDMW_ERR_BAD_USAGE)
{
}

PTEID_ExBadUsage::~PTEID_ExBadUsage()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExBadTransaction --------------------------------------
*****************************************************************************************/
PTEID_ExBadTransaction::PTEID_ExBadTransaction():PTEID_Exception(EIDMW_ERR_BAD_TRANSACTION)
{
}

PTEID_ExBadTransaction::~PTEID_ExBadTransaction()
{
}


/*****************************************************************************************
---------------------------------------- PTEID_ExNoReader --------------------------------------
*****************************************************************************************/
PTEID_ExNoReader::PTEID_ExNoReader():PTEID_Exception(EIDMW_ERR_NO_READER)
{
}

PTEID_ExNoReader::~PTEID_ExNoReader()
{
}


/*****************************************************************************************
---------------------------- PTEID_ExUnexpectedValue --------------------------------------
*****************************************************************************************/
PTEID_ExSOD::PTEID_ExSOD(long lError):PTEID_Exception(lError)
{
}

PTEID_ExSOD::~PTEID_ExSOD()
{
}

/*****************************************************************************************
---------------------------- PTEID_ExBatchSignatureFailed --------------------------------
*****************************************************************************************/
PTEID_ExBatchSignatureFailed::PTEID_ExBatchSignatureFailed(long lError, unsigned int index):
	PTEID_Exception(lError), m_failedSignatureIndex(index)
{
}

PTEID_ExBatchSignatureFailed::~PTEID_ExBatchSignatureFailed()
{
}

//Deprecated Classes - we leave them here just to support old pteidlibj versions

/*****************************************************************************************
----------------------------------------- PTEID_ExCardBadType --------------------------------------
-*****************************************************************************************/
PTEID_ExCardBadType::PTEID_ExCardBadType() :PTEID_Exception(-1)
{
}

PTEID_ExCardBadType::~PTEID_ExCardBadType()
{
}


/*****************************************************************************************
----------------------------------------- PTEID_ExReaderSetChanged --------------------------------------
-*****************************************************************************************/
PTEID_ExReaderSetChanged::PTEID_ExReaderSetChanged() :PTEID_Exception(-1)
{
}

PTEID_ExReaderSetChanged::~PTEID_ExReaderSetChanged()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCardChanged --------------------------------------
*****************************************************************************************/
PTEID_ExCardChanged::PTEID_ExCardChanged():PTEID_Exception(-1)
{
}

PTEID_ExCardChanged::~PTEID_ExCardChanged()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExCertNoIssuer --------------------------------------
*****************************************************************************************/
PTEID_ExCertNoIssuer::PTEID_ExCertNoIssuer():PTEID_Exception(-1)
{
}

PTEID_ExCertNoIssuer::~PTEID_ExCertNoIssuer()
{
}


/*****************************************************************************************
---------------------------------------- PTEID_ExDocTypeUnknown --------------------------------------
*****************************************************************************************/
PTEID_ExDocTypeUnknown::PTEID_ExDocTypeUnknown():PTEID_Exception(-1)
{
}

PTEID_ExDocTypeUnknown::~PTEID_ExDocTypeUnknown()
{
}

/*****************************************************************************************
---------------------------------------- PTEID_ExFileTypeUnknown --------------------------------------
*****************************************************************************************/
PTEID_ExFileTypeUnknown::PTEID_ExFileTypeUnknown():PTEID_Exception(-1)
{
}

PTEID_ExFileTypeUnknown::~PTEID_ExFileTypeUnknown()
{
}



}
