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
#include "eidlibException.h"
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

PTEID_Exception PTEID_Exception::THROWException(CMWException &e)
{
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
	return PTEID_Exception(e.GetError());
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