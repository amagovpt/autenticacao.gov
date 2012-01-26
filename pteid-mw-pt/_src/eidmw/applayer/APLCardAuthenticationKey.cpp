/*
 * CardAuthenticationKey.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: ruim
 */

#include "APLCardAuthenticationKey.h"

namespace eIDMW {

APLCardAuthenticationKey::APLCardAuthenticationKey(CByteArray &modulus, CByteArray &exponent){
	m_modulus = new CByteArray(modulus);
	m_exponent = new CByteArray(exponent);
}

APLCardAuthenticationKey::~APLCardAuthenticationKey(){
	if (m_modulus)
		delete m_modulus;
	if (m_exponent)
		delete m_exponent;
}

CByteArray *APLCardAuthenticationKey::getModulus(){
	return m_modulus;
}

CByteArray *APLCardAuthenticationKey::getExponent(){
	return m_exponent;
}

} /* namespace eIDMW */
