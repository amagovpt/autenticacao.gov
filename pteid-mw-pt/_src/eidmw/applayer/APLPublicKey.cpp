/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2024 André Guerreiro - <aguerreiro1985@gmail.com>

 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "APLPublicKey.h"

namespace eIDMW {

APLPublicKey::APLPublicKey(CByteArray &modulus, CByteArray &exponent) {
	m_modulus = new CByteArray(modulus);
	m_exponent = new CByteArray(exponent);
	m_curve_identifier = NOT_ECC_CURVE;
	m_ecc_publickey = NULL;
}

/* Constructor used for IAS v5 cards with ECC keys  */
APLPublicKey::APLPublicKey(CByteArray &ec_publickey) {
	m_modulus = NULL;
	m_exponent = NULL;
	m_ecc_publickey = new CByteArray(ec_publickey);
	m_curve_identifier = match_curve_size(ec_publickey);
}

bool APLPublicKey::isECCPublicKey() { return m_curve_identifier != NOT_ECC_CURVE; }

APLPublicKey::~APLPublicKey() {
	if (m_modulus)
		delete m_modulus;
	if (m_exponent)
		delete m_exponent;
}

ECC_Curve_Identifier APLPublicKey::match_curve_size(CByteArray &ec_publickey) {
	long curve_len = ec_publickey.Size();
	if (curve_len >= 132)
		return NIST_P521;
	else if (curve_len >= 96)
		return NIST_P384;
	else if (curve_len >= 64)
		return NIST_P256;
	else
		return NOT_ECC_CURVE;
}

CByteArray *APLPublicKey::getECCPublicKey() { return m_ecc_publickey; }

CByteArray *APLPublicKey::getModulus() { return m_modulus; }

CByteArray *APLPublicKey::getExponent() { return m_exponent; }

ECC_Curve_Identifier APLPublicKey::getECCPublicKeyCurveIdentifier() { return m_curve_identifier; }

} /* namespace eIDMW */
