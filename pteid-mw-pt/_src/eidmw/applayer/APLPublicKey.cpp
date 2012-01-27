/*
 * CardAuthenticationKey.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: ruim
 */

#include "APLPublicKey.h"

namespace eIDMW {

APLPublicKey::APLPublicKey(CByteArray &modulus, CByteArray &exponent){
	m_modulus = new CByteArray(modulus);
	m_exponent = new CByteArray(exponent);
}

APLPublicKey::~APLPublicKey(){
	if (m_modulus)
		delete m_modulus;
	if (m_exponent)
		delete m_exponent;
}

CByteArray *APLPublicKey::getModulus(){
	return m_modulus;
}

CByteArray *APLPublicKey::getExponent(){
	return m_exponent;
}

} /* namespace eIDMW */
