/*
 * CardAuthenticationKey.h
 *
 *  Created on: Jan 26, 2012
 *      Author: ruim
 */

#pragma once

#ifndef APLCARDAUTHENTICATIONKEY_H_
#define APLCARDAUTHENTICATIONKEY_H_

#include "Export.h"
#include "ByteArray.h"

namespace eIDMW {

EIDMW_CMN_API class APLCardAuthenticationKey {
public:
	APLCardAuthenticationKey(CByteArray &modulus, CByteArray &exponent);
	virtual ~APLCardAuthenticationKey();
	CByteArray *getModulus();
	CByteArray *getExponent();

private:
	CByteArray *m_modulus;
	CByteArray *m_exponent;
};
} /* namespace eIDMW */
#endif /* APLCARDAUTHENTICATIONKEY_H_ */
