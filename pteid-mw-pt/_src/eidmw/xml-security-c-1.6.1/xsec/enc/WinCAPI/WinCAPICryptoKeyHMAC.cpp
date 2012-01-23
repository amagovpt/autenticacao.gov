/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * WinCAPICryptoKeyHMAC := Windows HMAC keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: WinCAPICryptoKeyHMAC.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#if defined (XSEC_HAVE_WINCAPI)

WinCAPICryptoKeyHMAC::WinCAPICryptoKeyHMAC(HCRYPTPROV prov) :m_keyBuf("") {

	m_keyBuf.isSensitive();
	m_keyLen = 0;
	m_k = 0;
	m_p = prov;

};

void WinCAPICryptoKeyHMAC::setKey(unsigned char * inBuf, unsigned int inLength) {

	m_keyBuf.sbMemcpyIn(inBuf, inLength);
	m_keyLen = inLength;

}

unsigned int WinCAPICryptoKeyHMAC::getKey(safeBuffer &outBuf) const {

	outBuf = m_keyBuf;
	return m_keyLen;

}

XSECCryptoKey * WinCAPICryptoKeyHMAC::clone() const {

	WinCAPICryptoKeyHMAC * ret;

	XSECnew(ret, WinCAPICryptoKeyHMAC(m_p));

	ret->m_keyBuf = m_keyBuf;
	ret->m_keyLen = m_keyLen;

	if (m_k != 0) {
#if (_WIN32_WINNT > 0x0400)
		if (CryptDuplicateKey(m_k,
			 				  0,
							  0,
							  &(ret->m_k)) == 0 ) {

			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:KeyHMAC Error attempting to clone key parameters");

		}
#else
		throw XSECCryptoException(XSECCryptoException::MDError,
			"Unable to clone keys in Windows NT 4.0 and below");
#endif
	}
	else
		ret->m_k = 0;

	return ret;

}

// --------------------------------------------------------------------------------
//           Windows Specific Keys
// --------------------------------------------------------------------------------

void WinCAPICryptoKeyHMAC::setWinKey(HCRYPTKEY k) {

	if (m_k != 0) {

		CryptDestroyKey(m_k);

	}

	m_k = k;

}

HCRYPTKEY WinCAPICryptoKeyHMAC::getWinKey(void) const {

	return m_k;

}

HCRYPTPROV WinCAPICryptoKeyHMAC::getWinKeyProv(void) const {

	return m_p;

}

#endif /* XSEC_HAVE_WINCAPI */
