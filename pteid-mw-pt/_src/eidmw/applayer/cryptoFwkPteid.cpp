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
#include "cryptoFwkPteid.h"

#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "CardPteidDef.h"

namespace eIDMW
{

/* *******************
*** APL_CryptoFwkPteid ***
******************** */
APL_CryptoFwkPteid::APL_CryptoFwkPteid():APL_CryptoFwk()
{
}

APL_CryptoFwkPteid::~APL_CryptoFwkPteid(void)
{
}

bool APL_CryptoFwkPteid::VerifyRoot(const CByteArray &cert)
{
	return APL_CryptoFwk::VerifyRoot(cert,_pteid_root_certs);
}

}
