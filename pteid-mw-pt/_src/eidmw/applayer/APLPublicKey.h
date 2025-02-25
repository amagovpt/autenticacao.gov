/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2024 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/
#pragma once

#include "Export.h"
#include "ByteArray.h"

namespace eIDMW {

enum ECC_Curve_Identifier { NOT_ECC_CURVE, NIST_P256, NIST_P384, NIST_P521 };

class APLPublicKey {
public:
	APLPublicKey(CByteArray &modulus, CByteArray &exponent);
	APLPublicKey(CByteArray &ec_publickey);
	EIDMW_APL_API virtual ~APLPublicKey();
	EIDMW_APL_API CByteArray *getModulus();
	EIDMW_APL_API CByteArray *getExponent();
	EIDMW_APL_API bool isECCPublicKey();
	EIDMW_APL_API CByteArray *getECCPublicKey();
	EIDMW_APL_API ECC_Curve_Identifier getECCPublicKeyCurveIdentifier();

private:
	static ECC_Curve_Identifier match_curve_size(CByteArray &ec_publickey);
	CByteArray *m_modulus;
	CByteArray *m_exponent;
	CByteArray *m_ecc_publickey;
	ECC_Curve_Identifier m_curve_identifier;
};
} /* namespace eIDMW */
