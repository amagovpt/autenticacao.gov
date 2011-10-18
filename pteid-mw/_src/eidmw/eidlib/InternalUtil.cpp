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
#include "InternalUtil.h"

#include "eidErrors.h"

namespace eIDMW
{

BEID_CardType ConvertCardType(APL_CardType eCardType)
{
	switch(eCardType)
	{ 
	case APL_CARDTYPE_BEID_EID:
		return BEID_CARDTYPE_EID;

	case APL_CARDTYPE_BEID_KIDS:
		return BEID_CARDTYPE_KIDS;

	case APL_CARDTYPE_BEID_FOREIGNER:
		return BEID_CARDTYPE_FOREIGNER;

	case APL_CARDTYPE_BEID_SIS:
		return BEID_CARDTYPE_SIS;

	default:
		return BEID_CARDTYPE_UNKNOWN;
	}
}


BEID_CertifStatus ConvertCertStatus(APL_CertifStatus eStatus)
{
	//Convert the APL_CertifStatus into a status for the 'outside world"

	switch(eStatus)
	{ 
	case APL_CERTIF_STATUS_TEST:
		return BEID_CERTIF_STATUS_TEST;

	case APL_CERTIF_STATUS_DATE:
		return BEID_CERTIF_STATUS_DATE;

	case APL_CERTIF_STATUS_REVOKED:
		return BEID_CERTIF_STATUS_REVOKED;

	case APL_CERTIF_STATUS_CONNECT:
		return BEID_CERTIF_STATUS_CONNECT;

	case APL_CERTIF_STATUS_ISSUER:
		return BEID_CERTIF_STATUS_ISSUER;

	case APL_CERTIF_STATUS_UNKNOWN:
		return BEID_CERTIF_STATUS_UNKNOWN;

	case APL_CERTIF_STATUS_VALID:
		return BEID_CERTIF_STATUS_VALID;

	case APL_CERTIF_STATUS_VALID_CRL:
		return BEID_CERTIF_STATUS_VALID_CRL;

	case APL_CERTIF_STATUS_VALID_OCSP:
		return BEID_CERTIF_STATUS_VALID_OCSP;

	default:
		return BEID_CERTIF_STATUS_ERROR;
	}
}

BEID_CertifType ConvertCertType(APL_CertifType eType)
{
	switch(eType)
	{ 
	case APL_CERTIF_TYPE_ROOT:
		return BEID_CERTIF_TYPE_ROOT;

	case APL_CERTIF_TYPE_CA:
		return BEID_CERTIF_TYPE_CA;

	case APL_CERTIF_TYPE_AUTHENTICATION:
		return BEID_CERTIF_TYPE_AUTHENTICATION;

	case APL_CERTIF_TYPE_SIGNATURE:
		return BEID_CERTIF_TYPE_SIGNATURE;

	default:
		return BEID_CERTIF_TYPE_UNKNOWN;
	}
}

BEID_CrlStatus ConvertCrlStatus(APL_CrlStatus eStatus)
{
	switch(eStatus)
	{ 
	case APL_CRL_STATUS_UNKNOWN:
		return BEID_CRL_STATUS_UNKNOWN;

	case APL_CRL_STATUS_VALID:
		return BEID_CRL_STATUS_VALID;

	case APL_CRL_STATUS_CONNECT:
		return BEID_CRL_STATUS_CONNECT;

	default:
		return BEID_CRL_STATUS_ERROR;
	}
}

APL_HashAlgo ConvertHashAlgo(BEID_HashAlgo eAlgo)
{
	switch(eAlgo)
	{ 
	case BEID_ALGO_MD5:
		return APL_ALGO_MD5;

	default:
		return APL_ALGO_SHA1;
	}
}

APL_SaveFileType ConvertFileType(BEID_FileType fileType)
{
	switch(fileType)
	{ 
	case BEID_FILETYPE_TLV:
		return APL_SAVEFILETYPE_TLV;

	case BEID_FILETYPE_CSV:
		return APL_SAVEFILETYPE_CSV;

	case BEID_FILETYPE_XML:
		return APL_SAVEFILETYPE_XML;

	default:
		return APL_SAVEFILETYPE_UNKNOWN;
	}
}

tLOG_Level ConvertLogLevel(BEID_LogLevel level)
{
	switch(level)
	{ 
	case BEID_LOG_LEVEL_ERROR:
		return LOG_LEVEL_ERROR;

	case BEID_LOG_LEVEL_WARNING:
		return LOG_LEVEL_WARNING;

	case BEID_LOG_LEVEL_INFO:
		return LOG_LEVEL_INFO;

	case BEID_LOG_LEVEL_DEBUG:
		return LOG_LEVEL_DEBUG;

	default:
		return LOG_LEVEL_CRITICAL;
	}
}

APL_ValidationLevel ConvertValidationLevel(BEID_ValidationLevel eLevel)
{
	switch(eLevel)
	{ 
	case BEID_VALIDATION_LEVEL_OPTIONAL:
		return APL_VALIDATION_LEVEL_OPTIONAL;

	case BEID_VALIDATION_LEVEL_MANDATORY:
		return APL_VALIDATION_LEVEL_MANDATORY;

	default:
		return APL_VALIDATION_LEVEL_NONE;
	}
}
}
