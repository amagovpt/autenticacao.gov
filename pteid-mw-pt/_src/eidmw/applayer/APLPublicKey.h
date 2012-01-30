/*
 * APLPublicKey.h
 *
 *  Created on: Jan 26, 2012
 *      Author: ruim
 */

#pragma once

#ifndef APLPUBLICKEY_H_
#define APLPUBLICKEY_H_

#include "Export.h"
#include "ByteArray.h"

namespace eIDMW {

class APLPublicKey {
public:
	EIDMW_APL_API APLPublicKey(CByteArray &modulus, CByteArray &exponent);
	EIDMW_APL_API virtual ~APLPublicKey();
	EIDMW_APL_API CByteArray *getModulus();
	EIDMW_APL_API CByteArray *getExponent();

private:
	CByteArray *m_modulus;
	CByteArray *m_exponent;
};
} /* namespace eIDMW */
#endif /* PTEIDPUBLICKEY_H_ */
