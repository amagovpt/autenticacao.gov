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
 * NSSCryptoKeyHMAC := HMAC Keys
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/enc/NSS/NSSCryptoKeyHMAC.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#if defined (XSEC_HAVE_NSS)

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoKeyHMAC::NSSCryptoKeyHMAC() :m_keyBuf("") {

	m_keyBuf.isSensitive();
	m_keyLen = 0;

};

// --------------------------------------------------------------------------------
//           Set key
// --------------------------------------------------------------------------------

void NSSCryptoKeyHMAC::setKey(unsigned char * inBuf, unsigned int inLength) {

	m_keyBuf.sbMemcpyIn(inBuf, inLength);
	m_keyLen = inLength;

}

// --------------------------------------------------------------------------------
//           Get key
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyHMAC::getKey(safeBuffer &outBuf) const {

	outBuf = m_keyBuf;
	return m_keyLen;

}

// --------------------------------------------------------------------------------
//           Replicate key
// --------------------------------------------------------------------------------

XSECCryptoKey * NSSCryptoKeyHMAC::clone() const {

	NSSCryptoKeyHMAC * ret;

	XSECnew(ret, NSSCryptoKeyHMAC());

	ret->m_keyBuf = m_keyBuf;
	ret->m_keyLen = m_keyLen;

	return ret;

}

#endif /* XSEC_HAVE_NSS */