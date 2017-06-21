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
 * NSSCryptoProvider := Provider to support NSS
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#include <xsec/enc/NSS/NSSCryptoX509.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyDSA.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyRSA.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyHMAC.hpp>
#include <xsec/enc/NSS/NSSCryptoHash.hpp>
#include <xsec/enc/NSS/NSSCryptoHashHMAC.hpp>
#include <xsec/enc/NSS/NSSCryptoSymmetricKey.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#if defined (XSEC_HAVE_NSS)

#include <xercesc/util/Janitor.hpp>

XSEC_USING_XERCES(ArrayJanitor);

int NSSCryptoProvider::m_initialised = 0;

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

void NSSCryptoProvider::Init(const char * dbDir) {

	++m_initialised;

	if (m_initialised > 1)
		return;

	SECStatus s;

	if (dbDir != NULL)
		s = NSS_Init(dbDir);
	else
		s = NSS_NoDB_Init(NULL);

	if (s != SECSuccess) {

		throw XSECCryptoException(XSECCryptoException::MemoryError,
			"NSSCryptoProvider:NSSCryptoProvider - Error initializing NSS");

	}

}

NSSCryptoProvider::NSSCryptoProvider(const char * dbDir) {

	Init(dbDir);

}


// --------------------------------------------------------------------------------
//           Empty constructor
// --------------------------------------------------------------------------------

NSSCryptoProvider::NSSCryptoProvider()
{

	Init(NULL);

}

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoProvider::~NSSCryptoProvider()
{

  if (m_initialised == 1) {

    PK11_LogoutAll();

	  SECStatus s = NSS_Shutdown();

    // Note that NSS will fail to shut down if any memory leaks are detected (NSS
    // objects were not released). This is security measure to prevent abuse of your
    // private keys.
    if (s != SECSuccess) {

      //throw XSECCryptoException(XSECCryptoException::GeneralError,
		  //	"NSSCryptoProvider:NSSCryptoProvider - Error shuting down NSS");

    }

  }
  m_initialised--;

}

// --------------------------------------------------------------------------------
//           Get NSS string
// --------------------------------------------------------------------------------

const XMLCh * NSSCryptoProvider::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVNSS;

}

// --------------------------------------------------------------------------------
//           Hash SHA1
// --------------------------------------------------------------------------------

XSECCryptoHash	* NSSCryptoProvider::hashSHA1() const {

	NSSCryptoHash * ret;

	XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_SHA1));

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash SHA
// --------------------------------------------------------------------------------

XSECCryptoHash	* NSSCryptoProvider::hashSHA(int length) const {

	NSSCryptoHash * ret;

  switch (length) {

  case 160: XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_SHA1));
		break;
	case 256: XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_SHA256));
		break;
	case 384: XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_SHA384));
		break;
	case 512: XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_SHA512));
		break;
	default:
    ret = NULL;
	}

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash HMAC SHA1
// --------------------------------------------------------------------------------

XSECCryptoHash * NSSCryptoProvider::hashHMACSHA1() const {

	NSSCryptoHashHMAC * ret;

	XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_SHA1));

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash HMAC SHA
// --------------------------------------------------------------------------------

XSECCryptoHash * NSSCryptoProvider::hashHMACSHA(int length) const {

	NSSCryptoHashHMAC * ret;

  switch (length) {

  case 160: XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_SHA1));
		break;
	case 256: XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_SHA256));
		break;
	case 384: XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_SHA384));
		break;
	case 512: XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_SHA512));
		break;
	default:
    ret = NULL;
	}

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash MD5
// --------------------------------------------------------------------------------

XSECCryptoHash	* NSSCryptoProvider::hashMD5() const {

	NSSCryptoHash * ret;

	XSECnew(ret, NSSCryptoHash(XSECCryptoHash::HASH_MD5));

	return ret;

}

// --------------------------------------------------------------------------------
//           Hash HMAC MD5
// --------------------------------------------------------------------------------

XSECCryptoHash * NSSCryptoProvider::hashHMACMD5()const {

	NSSCryptoHashHMAC * ret;

	XSECnew(ret, NSSCryptoHashHMAC(XSECCryptoHash::HASH_MD5));

	return ret;

}

// --------------------------------------------------------------------------------
//           Get HMAC key
// --------------------------------------------------------------------------------

XSECCryptoKeyHMAC * NSSCryptoProvider::keyHMAC(void) const {

	NSSCryptoKeyHMAC * ret;

	XSECnew(ret, NSSCryptoKeyHMAC());

	return ret;

}

// --------------------------------------------------------------------------------
//           Get DSA key
// --------------------------------------------------------------------------------

XSECCryptoKeyDSA * NSSCryptoProvider::keyDSA() const {
	
	NSSCryptoKeyDSA * ret;

	XSECnew(ret, NSSCryptoKeyDSA());

	return ret;

}

// --------------------------------------------------------------------------------
//           Get RSA key
// --------------------------------------------------------------------------------

XSECCryptoKeyRSA * NSSCryptoProvider::keyRSA() const {
	
	NSSCryptoKeyRSA * ret;

	XSECnew(ret, NSSCryptoKeyRSA());

	return ret;

}

// --------------------------------------------------------------------------------
//           Get symmetric key
// --------------------------------------------------------------------------------

XSECCryptoSymmetricKey	* NSSCryptoProvider::keySymmetric(
                               XSECCryptoSymmetricKey::SymmetricKeyType alg) const {

	// Only temporary

	NSSCryptoSymmetricKey * ret;
	
	XSECnew(ret, NSSCryptoSymmetricKey(alg));

	return ret;

}

// --------------------------------------------------------------------------------
//           Get X509 class
// --------------------------------------------------------------------------------

XSECCryptoX509 * NSSCryptoProvider::X509() const {

	NSSCryptoX509 * ret;

	XSECnew(ret, NSSCryptoX509());

	return ret;

}

// --------------------------------------------------------------------------------
//           Get Base64
// --------------------------------------------------------------------------------

XSECCryptoBase64 * NSSCryptoProvider::base64() const {

	// NSS does provide a Base64 decoder/encoder,
	// but rather use the internal implementation.

	XSCryptCryptoBase64 * ret;

	XSECnew(ret, XSCryptCryptoBase64());

	return ret;

}

// --------------------------------------------------------------------------------
//     Translate a Base64 number to a SECItem
// --------------------------------------------------------------------------------

SECItem * NSSCryptoProvider::b642SI(const char * b64, unsigned int b64Len) {

    SECItem * rv;

    unsigned char * os;
	XSECnew(os, unsigned char[b64Len]);
	ArrayJanitor<unsigned char> j_os(os);

	// Decode
	XSCryptCryptoBase64 b;

	b.decodeInit();
	unsigned int retLen = b.decode((unsigned char *) b64, b64Len, os, b64Len);
	retLen += b.decodeFinish(&os[retLen], b64Len - retLen);

	rv = SECITEM_AllocItem(NULL, NULL, retLen);
	rv->len = retLen;
	memcpy(rv->data, os, retLen);

	return rv;

}

// --------------------------------------------------------------------------------
//     Translate a SECItem to a Base64 I2OSP number 
// --------------------------------------------------------------------------------

unsigned char * NSSCryptoProvider::SI2b64(SECItem * n, unsigned int &retLen) {

	unsigned char * b64;
	// Naieve length calculation
	unsigned int bufLen = n->len * 2 + 4;

	XSECnew(b64, unsigned char[bufLen]);
	ArrayJanitor<unsigned char> j_b64(b64);

  // Encode
	XSCryptCryptoBase64 b;

	b.encodeInit();
	retLen = b.encode(n->data, (unsigned int) n->len, b64, bufLen);
	retLen += b.encodeFinish(&b64[retLen], bufLen - retLen);

	j_b64.release();
	return b64;

}

// --------------------------------------------------------------------------------
//           Check if hash algorithm is supported
// --------------------------------------------------------------------------------

bool NSSCryptoProvider::algorithmSupported(XSECCryptoHash::HashType alg) const {

	switch (alg) {

	case (XSECCryptoHash::HASH_SHA1) :
	case (XSECCryptoHash::HASH_MD5) :

		return true;

	case (XSECCryptoHash::HASH_SHA256) :
	case (XSECCryptoHash::HASH_SHA384) :
	case (XSECCryptoHash::HASH_SHA512) :
	case (XSECCryptoHash::HASH_SHA224) :

		return false;

	default:

		return false;

	}

	return false;

}

// --------------------------------------------------------------------------------
//           Check if cypher algorithm is supported
// --------------------------------------------------------------------------------

bool NSSCryptoProvider::algorithmSupported(XSECCryptoSymmetricKey::SymmetricKeyType alg) const {

	switch (alg) {

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
	case (XSECCryptoSymmetricKey::KEY_AES_192) :
	case (XSECCryptoSymmetricKey::KEY_AES_256) :

		//return m_haveAES;
    return true;

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		return true;

	default:

		return false;

	}

	return false;

}


// --------------------------------------------------------------------------------
//           Generate random data
// --------------------------------------------------------------------------------

unsigned int NSSCryptoProvider::getRandom(unsigned char * buffer, unsigned int numOctets) const {

  SECStatus s = PK11_GenerateRandom(buffer, numOctets);

  if (s != SECSuccess) {

		throw XSECException(XSECException::InternalError,
			"NSSCryptoProvider() - Error generating Random data");

  }

	return numOctets;

}

#endif /* XSEC_HAVE_NSS */
