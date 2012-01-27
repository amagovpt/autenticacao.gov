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

EIDMW_CMN_API class APLPublicKey {
public:
	APLPublicKey(CByteArray &modulus, CByteArray &exponent);
	virtual ~APLPublicKey();
	CByteArray *getModulus();
	CByteArray *getExponent();

private:
	CByteArray *m_modulus;
	CByteArray *m_exponent;
};
} /* namespace eIDMW */
#endif /* PTEIDPUBLICKEY_H_ */
