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

#ifndef __CRYPTOBEIDFWK_H__
#define __CRYPTOBEIDFWK_H__

#include <memory>
#include "Mutex.h"
#include "ByteArray.h"
#include "openssl/evp.h"
#include "cryptoFramework.h"
#include "APLReader.h"

namespace eIDMW
{

/******************************************************************************//**
  * Class for all Beid cryptographic features 
  *
  * The goal of this class is to provide facilities to openSSL usage
  * This is only for internal use, no export is forseen. 
  *********************************************************************************/
class APL_CryptoFwkBeid : public APL_CryptoFwk
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_CryptoFwkBeid(void);

	/**
	  * Verify if the cert is the correct Beid root
	  */
	bool VerifyRoot(const CByteArray &cert);

private:
	/**
	  * Constructor"
	  */
	APL_CryptoFwkBeid();

	APL_CryptoFwkBeid(const APL_CryptoFwkBeid &service);				/**< Copy not allowed - not implemented */
	APL_CryptoFwkBeid &operator=(const APL_CryptoFwkBeid &service);	/**< Copy not allowed - not implemented */

friend void CAppLayer::startAllServices();			/**< This method must access private constructor */
};

}

#endif // __CRYPTOBEIDFWK_H__
