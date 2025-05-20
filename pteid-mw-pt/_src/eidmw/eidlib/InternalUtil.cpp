/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
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

namespace eIDMW {

PTEID_CardType ConvertCardType(APL_CardType eCardType) {
	switch (eCardType) {
	case APL_CARDTYPE_PTEID_IAS07:
		return PTEID_CARDTYPE_IAS07;
	case APL_CARDTYPE_PTEID_IAS101:
		return PTEID_CARDTYPE_IAS101;
	case APL_CARDTYPE_PTEID_IAS5:
		return PTEID_CARDTYPE_IAS5;
	case APL_CARDTYPE_ICAO:
		return ICAO_CARDTYPE_MRTD;
	default:
		return PTEID_CARDTYPE_UNKNOWN;
	}
}

PTEID_CertifStatus ConvertCertStatus(APL_CertifStatus eStatus) {
	// Convert the APL_CertifStatus into a status for the 'outside world"

	switch (eStatus) {

	case APL_CERTIF_STATUS_REVOKED:
		return PTEID_CERTIF_STATUS_REVOKED;

	case APL_CERTIF_STATUS_CONNECT:
		return PTEID_CERTIF_STATUS_CONNECT;

	case APL_CERTIF_STATUS_SUSPENDED:
		return PTEID_CERTIF_STATUS_SUSPENDED;

	case APL_CERTIF_STATUS_ISSUER:
		return PTEID_CERTIF_STATUS_ISSUER;

	case APL_CERTIF_STATUS_UNKNOWN:
		return PTEID_CERTIF_STATUS_UNKNOWN;

	case APL_CERTIF_STATUS_VALID:
		return PTEID_CERTIF_STATUS_VALID;

	case APL_CERTIF_STATUS_EXPIRED:
		return PTEID_CERTIF_STATUS_EXPIRED;

	default:
		return PTEID_CERTIF_STATUS_ERROR;
	}
}

PTEID_CertifType ConvertCertType(APL_CertifType eType) {
	switch (eType) {
	case APL_CERTIF_TYPE_ROOT:
		return PTEID_CERTIF_TYPE_ROOT;

	case APL_CERTIF_TYPE_ROOT_AUTH:
		return PTEID_CERTIF_TYPE_ROOT_AUTH;

	case APL_CERTIF_TYPE_ROOT_SIGN:
		return PTEID_CERTIF_TYPE_ROOT_SIGN;

	case APL_CERTIF_TYPE_AUTHENTICATION:
		return PTEID_CERTIF_TYPE_AUTHENTICATION;

	case APL_CERTIF_TYPE_SIGNATURE:
		return PTEID_CERTIF_TYPE_SIGNATURE;

	default:
		return PTEID_CERTIF_TYPE_UNKNOWN;
	}
}

tLOG_Level ConvertLogLevel(PTEID_LogLevel level) {
	switch (level) {
	case PTEID_LOG_LEVEL_ERROR:
		return LOG_LEVEL_ERROR;

	case PTEID_LOG_LEVEL_WARNING:
		return LOG_LEVEL_WARNING;

	case PTEID_LOG_LEVEL_INFO:
		return LOG_LEVEL_INFO;

	case PTEID_LOG_LEVEL_DEBUG:
		return LOG_LEVEL_DEBUG;

	default:
		return LOG_LEVEL_CRITICAL;
	}
}

APL_SignatureLevel ConvertSignatureLevel(PTEID_SignatureLevel level) {
	switch (level) {
	case PTEID_LEVEL_BASIC:
		return LEVEL_BASIC;

	case PTEID_LEVEL_TIMESTAMP:
		return LEVEL_TIMESTAMP;

	case PTEID_LEVEL_LT:
		return LEVEL_LT;

	case PTEID_LEVEL_LTV:
		return LEVEL_LTV;

	default:
		return LEVEL_BASIC;
	}
}

} // namespace eIDMW
