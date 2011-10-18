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


namespace eIDMW
{
	
/*****************************************************************************************
---------------------------------------- BEID_Exception --------------------------------------
*****************************************************************************************/
BEID_Exception::BEID_Exception(long lError):m_lError(lError)
{
}

BEID_Exception::~BEID_Exception()
{
}

long BEID_Exception::GetError() const
{
	return m_lError;
}

BEID_Exception BEID_Exception::THROWException(CMWException &e)
{
	switch(e.GetError())
	{
	case EIDMW_ERR_RELEASE_NEEDED:
		throw BEID_ExReleaseNeeded();
	case EIDMW_ERR_UNKNOWN:
		throw BEID_ExUnknown();
	case EIDMW_ERR_DOCTYPE_UNKNOWN:
		throw BEID_ExDocTypeUnknown();
	case EIDMW_ERR_FILETYPE_UNKNOWN:
		throw BEID_ExFileTypeUnknown();
	case EIDMW_ERR_PARAM_RANGE:
		throw BEID_ExParamRange();
	case EIDMW_ERR_CMD_NOT_ALLOWED:
		throw BEID_ExCmdNotAllowed();
	case EIDMW_ERR_NOT_SUPPORTED:
		throw BEID_ExCmdNotSupported();
	case EIDMW_ERR_NO_CARD:
		throw BEID_ExNoCardPresent();
	case EIDMW_ERR_CARDTYPE_BAD:
		throw BEID_ExCardBadType();
	case EIDMW_ERR_CARDTYPE_UNKNOWN:
		throw BEID_ExCardTypeUnknown();
	case EIDMW_ERR_CERT_NOISSUER:
		throw BEID_ExCertNoIssuer();
	case EIDMW_ERR_CERT_NOCRL:
		throw BEID_ExCertNoCrl();
	case EIDMW_ERR_CERT_NOOCSP:
		throw BEID_ExCertNoOcsp();
	case EIDMW_ERR_CERT_NOROOT:
		throw BEID_ExCertNoRoot();
	case EIDMW_ERR_BAD_USAGE:
		throw BEID_ExBadUsage();
	case EIDMW_ERR_BAD_TRANSACTION:
		throw BEID_ExBadTransaction();
	case EIDMW_ERR_CARD_CHANGED:
		throw BEID_ExCardChanged();
	case EIDMW_ERR_READERSET_CHANGED:
		throw BEID_ExReaderSetChanged();
	case EIDMW_ERR_NO_READER:
		throw BEID_ExNoReader();
	case EIDMW_ERR_NOT_ALLOW_BY_USER:
		throw BEID_ExNotAllowByUser();

	case EIDMW_ERR_CARD:
	default:
		throw BEID_Exception(e.GetError());
	}
	return BEID_Exception(e.GetError());
}

/*****************************************************************************************
---------------------------------------- BEID_ExReleaseNeeded --------------------------------------
*****************************************************************************************/
BEID_ExReleaseNeeded::BEID_ExReleaseNeeded():BEID_Exception(EIDMW_ERR_RELEASE_NEEDED)
{
}

BEID_ExReleaseNeeded::~BEID_ExReleaseNeeded()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExUnknown --------------------------------------
*****************************************************************************************/
BEID_ExUnknown::BEID_ExUnknown():BEID_Exception(EIDMW_ERR_UNKNOWN)
{
}

BEID_ExUnknown::~BEID_ExUnknown()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExDocTypeUnknown --------------------------------------
*****************************************************************************************/
BEID_ExDocTypeUnknown::BEID_ExDocTypeUnknown():BEID_Exception(EIDMW_ERR_DOCTYPE_UNKNOWN)
{
}

BEID_ExDocTypeUnknown::~BEID_ExDocTypeUnknown()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExFileTypeUnknown --------------------------------------
*****************************************************************************************/
BEID_ExFileTypeUnknown::BEID_ExFileTypeUnknown():BEID_Exception(EIDMW_ERR_FILETYPE_UNKNOWN)
{
}

BEID_ExFileTypeUnknown::~BEID_ExFileTypeUnknown()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExParamRange --------------------------------------
*****************************************************************************************/
BEID_ExParamRange::BEID_ExParamRange():BEID_Exception(EIDMW_ERR_PARAM_RANGE)
{
}

BEID_ExParamRange::~BEID_ExParamRange()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCmdNotAllowed --------------------------------------
*****************************************************************************************/
BEID_ExCmdNotAllowed::BEID_ExCmdNotAllowed():BEID_Exception(EIDMW_ERR_CMD_NOT_ALLOWED)
{
}

BEID_ExCmdNotAllowed::~BEID_ExCmdNotAllowed()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCmdNotSupported --------------------------------------
*****************************************************************************************/
BEID_ExCmdNotSupported::BEID_ExCmdNotSupported():BEID_Exception(EIDMW_ERR_NOT_SUPPORTED)
{
}

BEID_ExCmdNotSupported::~BEID_ExCmdNotSupported()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExNoCardPresent --------------------------------------
*****************************************************************************************/
BEID_ExNoCardPresent::BEID_ExNoCardPresent():BEID_Exception(EIDMW_ERR_NO_CARD)
{
}

BEID_ExNoCardPresent::~BEID_ExNoCardPresent()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCardBadType --------------------------------------
*****************************************************************************************/
BEID_ExCardBadType::BEID_ExCardBadType():BEID_Exception(EIDMW_ERR_CARDTYPE_BAD)
{
}

BEID_ExCardBadType::~BEID_ExCardBadType()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCardTypeUnknown --------------------------------------
*****************************************************************************************/
BEID_ExCardTypeUnknown::BEID_ExCardTypeUnknown():BEID_Exception(EIDMW_ERR_CARDTYPE_UNKNOWN)
{
}

BEID_ExCardTypeUnknown::~BEID_ExCardTypeUnknown()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCertNoIssuer --------------------------------------
*****************************************************************************************/
BEID_ExCertNoIssuer::BEID_ExCertNoIssuer():BEID_Exception(EIDMW_ERR_CERT_NOISSUER)
{
}

BEID_ExCertNoIssuer::~BEID_ExCertNoIssuer()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCertNoCrl --------------------------------------
*****************************************************************************************/
BEID_ExCertNoCrl::BEID_ExCertNoCrl():BEID_Exception(EIDMW_ERR_CERT_NOCRL)
{
}

BEID_ExCertNoCrl::~BEID_ExCertNoCrl()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCertNoOcsp --------------------------------------
*****************************************************************************************/
BEID_ExCertNoOcsp::BEID_ExCertNoOcsp():BEID_Exception(EIDMW_ERR_CERT_NOOCSP)
{
}

BEID_ExCertNoOcsp::~BEID_ExCertNoOcsp()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCertNoRoot --------------------------------------
*****************************************************************************************/
BEID_ExCertNoRoot::BEID_ExCertNoRoot():BEID_Exception(EIDMW_ERR_CERT_NOROOT)
{
}

BEID_ExCertNoRoot::~BEID_ExCertNoRoot()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExBadUsage --------------------------------------
*****************************************************************************************/
BEID_ExBadUsage::BEID_ExBadUsage():BEID_Exception(EIDMW_ERR_BAD_USAGE)
{
}

BEID_ExBadUsage::~BEID_ExBadUsage()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExBadTransaction --------------------------------------
*****************************************************************************************/
BEID_ExBadTransaction::BEID_ExBadTransaction():BEID_Exception(EIDMW_ERR_BAD_TRANSACTION)
{
}

BEID_ExBadTransaction::~BEID_ExBadTransaction()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExCardChanged --------------------------------------
*****************************************************************************************/
BEID_ExCardChanged::BEID_ExCardChanged():BEID_Exception(EIDMW_ERR_CARD_CHANGED)
{
}

BEID_ExCardChanged::~BEID_ExCardChanged()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExReaderSetChanged --------------------------------------
*****************************************************************************************/
BEID_ExReaderSetChanged::BEID_ExReaderSetChanged():BEID_Exception(EIDMW_ERR_READERSET_CHANGED)
{
}

BEID_ExReaderSetChanged::~BEID_ExReaderSetChanged()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExNoReader --------------------------------------
*****************************************************************************************/
BEID_ExNoReader::BEID_ExNoReader():BEID_Exception(EIDMW_ERR_NO_READER)
{
}

BEID_ExNoReader::~BEID_ExNoReader()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExNotAllowByUser --------------------------------------
*****************************************************************************************/
BEID_ExNotAllowByUser::BEID_ExNotAllowByUser():BEID_Exception(EIDMW_ERR_NOT_ALLOW_BY_USER)
{
}

BEID_ExNotAllowByUser::~BEID_ExNotAllowByUser()
{
}

/*****************************************************************************************
---------------------------------------- BEID_ExUserMustAnswer --------------------------------------
*****************************************************************************************/
BEID_ExUserMustAnswer::BEID_ExUserMustAnswer():BEID_Exception(EIDMW_ERR_USER_MUST_ANSWER)
{
}

BEID_ExUserMustAnswer::~BEID_ExUserMustAnswer()
{
}
}
