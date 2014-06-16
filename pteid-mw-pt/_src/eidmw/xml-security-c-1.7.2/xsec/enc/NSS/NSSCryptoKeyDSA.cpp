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
 * NSSCryptoKeyDSA := DSA Keys
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/enc/NSS/NSSCryptoKeyDSA.hpp>
#include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/framework/XSECError.hpp>

#include <xercesc/util/Janitor.hpp>

#if defined (XSEC_HAVE_NSS)

XSEC_USING_XERCES(ArrayJanitor);

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoKeyDSA::NSSCryptoKeyDSA(SECKEYPublicKey * pubkey, SECKEYPrivateKey	*	privkey) {

  // NOTE - We OWN those handles
	mp_pubkey  = pubkey;
	mp_privkey = privkey;

	mp_P = NULL;
	mp_Q = NULL;
	mp_G = NULL;
	mp_Y = NULL;

};

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoKeyDSA::~NSSCryptoKeyDSA() {

	// Clean up

	if (mp_pubkey != 0)
		SECKEY_DestroyPublicKey(mp_pubkey);

  if (mp_privkey != 0)
		SECKEY_DestroyPrivateKey(mp_privkey);

	if (mp_P != NULL)
		 SECITEM_FreeItem(mp_P, PR_TRUE);

	if (mp_Q != NULL)
		SECITEM_FreeItem(mp_Q, PR_TRUE);

	if (mp_G != NULL)
		SECITEM_FreeItem(mp_G, PR_TRUE);

	if (mp_Y != NULL)
		SECITEM_FreeItem(mp_Y, PR_TRUE);

};

// --------------------------------------------------------------------------------
//           Get key type
// --------------------------------------------------------------------------------

XSECCryptoKey::KeyType NSSCryptoKeyDSA::getKeyType() const {

  // Find out what we have
	if (mp_pubkey == NULL) {

		if (mp_privkey != 0)
			return KEY_DSA_PRIVATE;
		
		// Check if we have parameters loaded
		if (mp_P == NULL ||
			mp_Q == NULL ||
			mp_G == NULL ||
			mp_Y == NULL)
			return KEY_NONE;
		else
			return KEY_DSA_PUBLIC;
	}

	if (mp_privkey != 0)
		return KEY_DSA_PAIR;

	// If we have m_key - it must be public
	return KEY_DSA_PUBLIC;

}

// --------------------------------------------------------------------------------
//           Load P parameter
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadPBase64BigNums(const char * b64, unsigned int len) {

	if (mp_P != NULL) {
		SECITEM_FreeItem(mp_P, PR_TRUE);
		mp_P = NULL;		// In case we get an exception
	}

	mp_P = NSSCryptoProvider::b642SI(b64, len);

}

// --------------------------------------------------------------------------------
//           Load Q parameter
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadQBase64BigNums(const char * b64, unsigned int len) {

	if (mp_Q != NULL) {
		SECITEM_FreeItem(mp_Q, PR_TRUE);
		mp_Q = NULL;		// In case we get an exception
	}

	mp_Q = NSSCryptoProvider::b642SI(b64, len);
}

// --------------------------------------------------------------------------------
//           Load G parameter
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadGBase64BigNums(const char * b64, unsigned int len) {

	if (mp_G != NULL) {
		SECITEM_FreeItem(mp_G, PR_TRUE);
		mp_G = NULL;		// In case we get an exception
	}

	mp_G = NSSCryptoProvider::b642SI(b64, len);
}

// --------------------------------------------------------------------------------
//           Load Y parameter
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadYBase64BigNums(const char * b64, unsigned int len) {

	if (mp_Y != NULL) {
		SECITEM_FreeItem(mp_Y, PR_TRUE);
		mp_Y = NULL;		// In case we get an exception
	}

	mp_Y = NSSCryptoProvider::b642SI(b64, len);
}

// --------------------------------------------------------------------------------
//           Load Y parameter
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadJBase64BigNums(const char * b64, unsigned int len) {
	//Do nothing
}

// --------------------------------------------------------------------------------
//           Import key
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::importKey(void) {
	
	if (mp_pubkey != 0 ||
		mp_P == NULL ||
		mp_Q == NULL ||
		mp_G == NULL || 
		mp_Y == NULL)
		return;

  PRArenaPool * arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);

  if(arena == NULL) {

      throw XSECCryptoException(XSECCryptoException::GeneralError,
			  "NSS:DSA Error attempting create new arena");

  }

  mp_pubkey = (SECKEYPublicKey*)PORT_ArenaZAlloc(arena, sizeof(SECKEYPublicKey));

  if(mp_pubkey == NULL ) {

      PORT_FreeArena(arena, PR_FALSE);

      throw XSECCryptoException(XSECCryptoException::MemoryError,
			  "NSS:DSA Error attempting create new arena");

  }

  mp_pubkey->arena = arena;
  mp_pubkey->u.dsa.params.arena = arena;
  mp_pubkey->keyType = dsaKey;

  SECStatus s = SECITEM_CopyItem(arena, &(mp_pubkey->u.dsa.params.prime), mp_P);

  if (s != SECSuccess) {
  
    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::MemoryError,
		  "NSS:DSA Error attempting to import P key parameter");
  
  }

  s = SECITEM_CopyItem(arena, &(mp_pubkey->u.dsa.params.subPrime), mp_Q);

  if (s != SECSuccess) {

    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::MemoryError,
		  "NSS:DSA Error attempting to import Q key parameter");
  
  }

  s = SECITEM_CopyItem(arena, &(mp_pubkey->u.dsa.params.base), mp_G);

  if (s != SECSuccess) {

    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::MemoryError,
		  "NSS:DSA Error attempting to import G key parameter");

  }

  s = SECITEM_CopyItem(arena, &(mp_pubkey->u.dsa.publicValue), mp_Y);
  
  if (s != SECSuccess) {

    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::MemoryError,
		  "NSS:DSA Error attempting to import Y key parameter");
  
  }

}

// --------------------------------------------------------------------------------
//           Verify a signature encoded as a Base64 string
// --------------------------------------------------------------------------------

bool NSSCryptoKeyDSA::verifyBase64Signature(unsigned char * hashBuf, 
								 unsigned int hashLen,
								 char * base64Signature,
								 unsigned int sigLen) {

	// Use the currently loaded key to validate the Base64 encoded signature

	if (mp_pubkey == 0) {

      // Try to import from the parameters
		  importKey();

      if (mp_pubkey == 0) {

        throw XSECCryptoException(XSECCryptoException::DSAError,
				      "NSS:DSA - Attempt to validate signature with empty key");

      }

	}

	// Decode the signature
	unsigned char * rawSig;
	unsigned int rawSigLen;
	XSECnew(rawSig, unsigned char[sigLen]);
	ArrayJanitor<unsigned char> j_rawSig(rawSig);

	// Decode the signature
	XSCryptCryptoBase64 b64;

	b64.decodeInit();
	rawSigLen = b64.decode((unsigned char *) base64Signature, sigLen, rawSig, sigLen);
	rawSigLen += b64.decodeFinish(&rawSig[rawSigLen], sigLen - rawSigLen);
	
  SECItem signature;
  signature.type = siBuffer;
  signature.data = rawSig;
  signature.len = rawSigLen;

  SECItem data;
  data.type = siBuffer;
  data.data = (unsigned char *)hashBuf;
  data.len = hashLen;

	// Verify signature
	SECStatus s = PK11_Verify(mp_pubkey, &signature, &data, NULL);

  return s == SECSuccess;

}

// --------------------------------------------------------------------------------
//           Sign and encode result as a Base64 string
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyDSA::signBase64Signature(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen) {

	// Sign a pre-calculated hash using this key

	if (mp_privkey == 0) {

		throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Attempt to sign data using a public or un-loaded key");

  }

  unsigned int signatureLen = PK11_SignatureLen(mp_privkey);

  unsigned char * rawSig;
	XSECnew(rawSig, unsigned char[signatureLen]);
	ArrayJanitor<unsigned char> j_rawSig(rawSig);

  SECItem signature;
  signature.type = siBuffer;
  signature.data = rawSig;
  signature.len = signatureLen;

  SECItem data;
  data.type = siBuffer;
  data.data = hashBuf;
  data.len = hashLen;

  SECStatus s = PK11_Sign(mp_privkey, &signature, &data);

  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Error during signing operation");

  }

  // Now encode
	XSCryptCryptoBase64 b64;
	b64.encodeInit();
	unsigned int ret = b64.encode(signature.data, signature.len,
                               (unsigned char *) base64SignatureBuf, base64SignatureBufLen);
	ret += b64.encodeFinish((unsigned char *) &base64SignatureBuf[ret],
                          base64SignatureBufLen - ret);

  return ret;

}

// --------------------------------------------------------------------------------
//           Clone key
// --------------------------------------------------------------------------------

XSECCryptoKey * NSSCryptoKeyDSA::clone() const {

	NSSCryptoKeyDSA * ret = NULL;

	XSECnew(ret, NSSCryptoKeyDSA(mp_pubkey, mp_privkey));
	
  // Clone public key
	if (mp_pubkey != 0) {

		ret->mp_pubkey = SECKEY_CopyPublicKey(mp_pubkey);

    if (ret->mp_pubkey == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:DSA Error attempting to clone (copy) public key");
  
    }

  }

  // Clone private key
  if (mp_privkey != 0) {

    ret->mp_privkey = SECKEY_CopyPrivateKey(mp_privkey);

    if (ret->mp_privkey == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:DSA Error attempting to clone (copy) private key");
  
    }

  }

  // Clone parameter P
  if (mp_P != 0) {
    ret->mp_P = SECITEM_DupItem(mp_P);

    if (ret->mp_P == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
	  	  "NSS:DSA Error attempting to clone (copy) P key parameter");
  
    }
  }

  // Clone parameter Q
  if (mp_Q != 0) {
    ret->mp_Q = SECITEM_DupItem(mp_Q);

    if (ret->mp_Q == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:DSA Error attempting to clone (copy) Q key parameter");
  
    }
  }

  // Clone parameter G
  if (mp_G != 0) {
    ret->mp_G = SECITEM_DupItem(mp_G);

    if (ret->mp_G == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:DSA Error attempting to clone (copy) G key parameter");
  
    }
  }

  // Clone parameter Y
  if (mp_Y != 0) {
    ret->mp_Y = SECITEM_DupItem(mp_Y);

    if (ret->mp_Y == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:DSA Error attempting to clone (copy) Y key parameter");
  
    }
  }

	return ret;

}

// --------------------------------------------------------------------------------
//           Some utility functions
// --------------------------------------------------------------------------------

void NSSCryptoKeyDSA::loadParamsFromKey(void) {

	if (mp_pubkey == 0)
			return;

  mp_P = SECITEM_DupItem(&(mp_pubkey->u.dsa.params.prime));

  if (mp_P == 0) {

    throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Error during extracting P from public key");

  }

  mp_Q = SECITEM_DupItem(&(mp_pubkey->u.dsa.params.subPrime));

  if (mp_Q == 0) {

    throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Error during extracting Q from public key");

  }

  mp_G = SECITEM_DupItem(&(mp_pubkey->u.dsa.params.base));

  if (mp_G == 0) {

    throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Error during extracting G from public key");

  }

  mp_Y = SECITEM_DupItem(&(mp_pubkey->u.dsa.publicValue));

  if (mp_Y == 0) {

    throw XSECCryptoException(XSECCryptoException::DSAError,
			"NSS:DSA - Error during extracting Y from public key");
  
  }

}

// --------------------------------------------------------------------------------
//           Get P parameter
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyDSA::getPBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_P == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_P == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b = NSSCryptoProvider::SI2b64(mp_P, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

// --------------------------------------------------------------------------------
//           Get Q parameter
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyDSA::getQBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_Q == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_Q == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b = NSSCryptoProvider::SI2b64(mp_Q, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

// --------------------------------------------------------------------------------
//           Get G parameter
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyDSA::getGBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_G == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_G == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b = NSSCryptoProvider::SI2b64(mp_G, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

// --------------------------------------------------------------------------------
//           Get Y parameter
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyDSA::getYBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_Y == NULL) {

		return 0;	// Nothing we can do

	}

	if (mp_Y == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b = NSSCryptoProvider::SI2b64(mp_Y, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;

}

#endif /* XSEC_HAVE_NSS */
