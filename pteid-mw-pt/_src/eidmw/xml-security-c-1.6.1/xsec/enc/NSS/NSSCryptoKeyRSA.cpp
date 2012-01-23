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
 * NSSCryptoKeyRSA := RSA Keys
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/enc/NSS/NSSCryptoKeyRSA.hpp>
#include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#include <xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/framework/XSECError.hpp>

#include <xercesc/util/Janitor.hpp>

#if defined (XSEC_HAVE_NSS)

#include <secder.h>
#include <secdig.h>

XSEC_USING_XERCES(ArrayJanitor);

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoKeyRSA::NSSCryptoKeyRSA(SECKEYPublicKey * pubkey, SECKEYPrivateKey * privkey) {

  // NOTE - We OWN those handles
  mp_pubkey   = pubkey;
  mp_privkey  = privkey;
  
  mp_modulus  = NULL;
  mp_exponent = NULL;

};

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoKeyRSA::~NSSCryptoKeyRSA() {

  // Clean up

	if (mp_pubkey != 0)
		SECKEY_DestroyPublicKey(mp_pubkey);

  if (mp_privkey != 0)
		SECKEY_DestroyPrivateKey(mp_privkey);

	if (mp_modulus != NULL)
		 SECITEM_FreeItem(mp_modulus, PR_TRUE);

	if (mp_exponent != NULL)
		SECITEM_FreeItem(mp_exponent, PR_TRUE);

};

// --------------------------------------------------------------------------------
//           Get key type
// --------------------------------------------------------------------------------

XSECCryptoKey::KeyType NSSCryptoKeyRSA::getKeyType() const {

	// Find out what we have
	if (mp_pubkey == 0) {

		if (mp_privkey != 0)
			return KEY_RSA_PRIVATE;

		if (mp_exponent == NULL || mp_modulus == NULL)
			return KEY_NONE;
		else
			return KEY_RSA_PUBLIC;

	}

	if (mp_privkey != 0)
		return KEY_RSA_PAIR;

	return KEY_RSA_PUBLIC;
}

// --------------------------------------------------------------------------------
//           Set OAEP parameters
// --------------------------------------------------------------------------------

void NSSCryptoKeyRSA::setOAEPparams(unsigned char * params, unsigned int paramsLen) {

  if (params != NULL && paramsLen != 0) {

		throw XSECCryptoException(XSECCryptoException::UnsupportedError,
			"NSS::setOAEPParams - OAEP parameters are not supported by NSS");

  }

}

// --------------------------------------------------------------------------------
//           Get OAEP parameters length
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::getOAEPparamsLen(void) const {

	return 0;

}

// --------------------------------------------------------------------------------
//           Get OAEP parameters
// --------------------------------------------------------------------------------

const unsigned char * NSSCryptoKeyRSA::getOAEPparams(void) const {

	return NULL;

}


// --------------------------------------------------------------------------------
//           Load modulus
// --------------------------------------------------------------------------------

void NSSCryptoKeyRSA::loadPublicModulusBase64BigNums(const char * b64, unsigned int len) {

	if (mp_modulus != NULL) {

		SECITEM_FreeItem(mp_modulus, PR_TRUE);
		mp_modulus = NULL;		// In case we get an exception

	}

  mp_modulus = NSSCryptoProvider::b642SI(b64, len);

}

// --------------------------------------------------------------------------------
//           Load exponent
// --------------------------------------------------------------------------------

void NSSCryptoKeyRSA::loadPublicExponentBase64BigNums(const char * b64, unsigned int len) {

	if (mp_exponent != NULL) {

		SECITEM_FreeItem(mp_exponent, PR_TRUE);
		mp_exponent = NULL;		// In case we get an exception

	}

  mp_exponent = NSSCryptoProvider::b642SI(b64, len);

}

// --------------------------------------------------------------------------------
//           Import key
// --------------------------------------------------------------------------------

void NSSCryptoKeyRSA::importKey(void) {

	if (mp_pubkey != 0 || mp_exponent == NULL || mp_modulus == NULL)
		return;


  PRArenaPool * arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);

  if(arena == NULL) {

      throw XSECCryptoException(XSECCryptoException::GeneralError,
			  "NSS:RSA Error attempting create new arena");

  }

  mp_pubkey = (SECKEYPublicKey*)PORT_ArenaZAlloc(arena, sizeof(SECKEYPublicKey));

  if(mp_pubkey == NULL ) {

      PORT_FreeArena(arena, PR_FALSE);

      throw XSECCryptoException(XSECCryptoException::GeneralError,
			  "NSS:RSA Error attempting create new arena");

  }

  mp_pubkey->arena = arena;
  mp_pubkey->keyType = rsaKey;

  SECStatus s = SECITEM_CopyItem(arena, &(mp_pubkey->u.rsa.modulus), mp_modulus);

  if (s != SECSuccess) {

    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::DSAError,
		  "NSS:RSA Error attempting to import modulus");

  }

  s = SECITEM_CopyItem(arena, &(mp_pubkey->u.rsa.publicExponent), mp_exponent);

  if (s != SECSuccess) {

    PORT_FreeArena(arena, PR_FALSE);

    throw XSECCryptoException(XSECCryptoException::DSAError,
		  "NSS:RSA Error attempting to import exponent");

  }

}

// --------------------------------------------------------------------------------
//           Verify a signature encoded as a Base64 string
// --------------------------------------------------------------------------------

bool NSSCryptoKeyRSA::verifySHA1PKCS1Base64Signature(const unsigned char * hashBuf, 
								 unsigned int hashLen,
								 const char * base64Signature,
								 unsigned int sigLen,
								 hashMethod hm) {

	// Use the currently loaded key to validate the Base64 encoded signature

	if (mp_pubkey == 0) {

      // Try to import from the parameters
		  importKey();

      if (mp_pubkey == 0) {

			    throw XSECCryptoException(XSECCryptoException::RSAError,
				      "NSS:RSA - Attempt to validate signature with empty key");

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
  data.data = 0;
  data.len = 0;
  SECOidTag hashalg;
  PRArenaPool * arena = 0;
  SGNDigestInfo *di = 0;
  SECItem * res;

  switch (hm) {

  case (HASH_MD5):
      hashalg = SEC_OID_MD5;
      break;
  case (HASH_SHA1):
      hashalg = SEC_OID_SHA1;
      break;
  case (HASH_SHA256):
      hashalg = SEC_OID_SHA256;
      break;
  case (HASH_SHA384):
      hashalg = SEC_OID_SHA384;
      break;
  case (HASH_SHA512):
      hashalg = SEC_OID_SHA512;
      break;
  default:
      throw XSECCryptoException(XSECCryptoException::RSAError,
          "NSS:RSA - Unsupported hash algorithm in RSA sign");
  }

  arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);
  if (!arena) {
      throw XSECCryptoException(XSECCryptoException::RSAError,
          "NSS:RSA - Error creating arena");
  }

  di = SGN_CreateDigestInfo(hashalg, (unsigned char * )hashBuf, hashLen);
  if (di == NULL) {

      PORT_FreeArena(arena, PR_FALSE);

      throw XSECCryptoException(XSECCryptoException::RSAError,
          "NSS:RSA - Error creating digest info");
  }

  res = SEC_ASN1EncodeItem(arena, &data, di, NSS_Get_sgn_DigestInfoTemplate(NULL, 0));

  if (!res) {
      SGN_DestroyDigestInfo(di);
      PORT_FreeArena(arena, PR_FALSE);

      throw XSECCryptoException(XSECCryptoException::RSAError,
          "NSS:RSA - Error encoding digest info for RSA sign");
  }

	// Verify signature
	SECStatus s = PK11_Verify(mp_pubkey, &signature, &data, NULL);

  return s == SECSuccess;

}

// --------------------------------------------------------------------------------
//           Sign and encode result as a Base64 string
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::signSHA1PKCS1Base64Signature(unsigned char * hashBuf,
		unsigned int hashLen,
		char * base64SignatureBuf,
		unsigned int base64SignatureBufLen,
		hashMethod hm) {

	// Sign a pre-calculated hash using this key

	if (mp_privkey == 0) {
		
		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Attempt to sign data using a public or un-loaded key");
		
	}
	
	unsigned char * rawSig;
	XSECnew(rawSig, unsigned char[base64SignatureBufLen]);
	ArrayJanitor<unsigned char> j_rawSig(rawSig);
	
	SECItem signature;
	signature.type = siBuffer;
	signature.data = rawSig;
	signature.len = base64SignatureBufLen;
	
	SECItem data;
	data.data = 0;
	SECOidTag hashalg;
	PRArenaPool * arena = 0;
	SGNDigestInfo *di = 0;
	SECItem * res;

	switch (hm) {

	case (HASH_MD5):
		hashalg = SEC_OID_MD5;
		break;
	case (HASH_SHA1):
		hashalg = SEC_OID_SHA1;
		break;
	case (HASH_SHA256):
		hashalg = SEC_OID_SHA256;
		break;
	case (HASH_SHA384):
		hashalg = SEC_OID_SHA384;
		break;
	case (HASH_SHA512):
		hashalg = SEC_OID_SHA512;
		break;
	default:
		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Unsupported hash algorithm in RSA sign");
	}

	arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);
	if (!arena) {
		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error creating arena");
	}

	di = SGN_CreateDigestInfo(hashalg, hashBuf, hashLen);	
	if (di == NULL) {
		
		PORT_FreeArena(arena, PR_FALSE);

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error creating digest info");
	}

	res = SEC_ASN1EncodeItem(arena, &data, di, NSS_Get_sgn_DigestInfoTemplate(NULL, 0));

	if (!res) {
		SGN_DestroyDigestInfo(di);
		PORT_FreeArena(arena, PR_FALSE);

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error encoding digest info for RSA sign");
	}

/*	data.type = siBuffer;
	data.data = hashBuf;
	data.len = hashLen;*/

	/* As of V1.3.1 - create a DigestInfo block */

	
	SECStatus s = PK11_Sign(mp_privkey, &signature, &data);
	
	SGN_DestroyDigestInfo(di);
	PORT_FreeArena(arena, PR_FALSE);

	if (s != SECSuccess) {
		
		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error during signing operation");
		
	}
	
	// Now encode
	XSCryptCryptoBase64 b64;
	b64.encodeInit();
	unsigned int ret = b64.encode(signature.data, signature.len, (unsigned char *) base64SignatureBuf, base64SignatureBufLen);
	ret += b64.encodeFinish((unsigned char *) &base64SignatureBuf[ret], base64SignatureBufLen - ret);
	
	return ret;
	
}

// --------------------------------------------------------------------------------
//           Clone key
// --------------------------------------------------------------------------------

XSECCryptoKey * NSSCryptoKeyRSA::clone() const {

	NSSCryptoKeyRSA * ret;

	XSECnew(ret, NSSCryptoKeyRSA(mp_pubkey, mp_privkey));
	
	if (mp_pubkey != 0) {

    ret->mp_pubkey = SECKEY_CopyPublicKey(mp_pubkey);
  
    if (ret->mp_pubkey == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:RSA Error attempting to clone (copy) public key");

    }

  }

  if (mp_privkey != 0) {

    ret->mp_privkey = SECKEY_CopyPrivateKey(mp_privkey);

    if (ret->mp_privkey == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:RSA Error attempting to clone (copy) private key");

    }

  }

  // Clone modulus
  if (mp_modulus != 0) {
    ret->mp_modulus = SECITEM_DupItem(mp_modulus);

    if (ret->mp_modulus == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
	  	  "NSS:RSA Error attempting to clone (copy) modulus");

    }
  }

  // Clone exponent
  if (mp_exponent != 0) {
    ret->mp_exponent = SECITEM_DupItem(mp_exponent);

    if (ret->mp_exponent == 0) {

      throw XSECCryptoException(XSECCryptoException::MemoryError,
		    "NSS:RSA Error attempting to clone (copy) exponent");

    }
  }

  return ret;

}

// --------------------------------------------------------------------------------
//           decrypt a buffer
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::privateDecrypt(const unsigned char * inBuf,
								 unsigned char * plainBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength,
								 PaddingType padding,
								 hashMethod hm) {

	// Perform a decrypt
	if (mp_privkey == 0) {

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Attempt to decrypt data with empty key");

  }

	unsigned int decryptSize = inLength;

  SECStatus s;
  unsigned char *ptr = NULL;

	switch (padding) {

	case XSECCryptoKeyRSA::PAD_PKCS_1_5 :

    s = PK11_PubDecryptRaw(mp_privkey,
                           plainBuf,
                           (unsigned int*)&decryptSize,
                           maxOutLength,
                           (unsigned char*)inBuf,
                           inLength);

    if (s != SECSuccess) {

			throw XSECCryptoException(XSECCryptoException::RSAError,
				"NSS:RSA privateKeyDecrypt - Error Decrypting PKCS1_5 padded RSA encrypt");

		}

        //do the padding (http://www.w3.org/TR/xmlenc-core/#rsa-1_5)
        ptr = (unsigned char*) memchr(plainBuf, 0x02, decryptSize);
		if( ptr )
		{
			unsigned int bytesToRemove = ((ptr-plainBuf)+1);
			memmove(plainBuf, ptr+1, decryptSize-bytesToRemove);
			decryptSize -= bytesToRemove;
		}

		ptr = (unsigned char*) memchr(plainBuf, 0x00, decryptSize);
		if( ptr )
		{
			unsigned int bytesToRemove = ((ptr-plainBuf)+1);
			memmove(plainBuf, ptr+1, decryptSize-bytesToRemove);
			decryptSize -= bytesToRemove;
		}

		break;

	case XSECCryptoKeyRSA::PAD_OAEP_MGFP1 :

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - OAEP padding method not supported in NSS yet");

		break;


	default :

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Unknown padding method");

	}

	return decryptSize;

}

// --------------------------------------------------------------------------------
//           Encrypt a buffer
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::publicEncrypt(const unsigned char * inBuf,
								 unsigned char * cipherBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength,
								 PaddingType padding,
								 hashMethod hm) {

	// Perform an encrypt
	if (mp_pubkey == 0) {

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Attempt to encrypt data with empty key");

  }

	unsigned int encryptSize = SECKEY_PublicKeyStrength(mp_pubkey);

  if (maxOutLength < encryptSize) {

    throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Too small buffer for encrypted buffer output");

  }

  SECStatus s;

  unsigned char * buf;
  XSECnew(buf, unsigned char[encryptSize]);
  ArrayJanitor<unsigned char> j_buf(buf);

	switch (padding) {

	case XSECCryptoKeyRSA::PAD_PKCS_1_5 :

    // do the padding (http://www.w3.org/TR/xmlenc-core/#rsa-1_5)
    {
      
      // generate random data for padding
      SECStatus s = PK11_GenerateRandom(buf, encryptSize);

      if (s != SECSuccess) {
      
        throw XSECException(XSECException::InternalError,
			    "NSSCryptoKeyRSA() - Error generating Random data");

      }

      // first byte have to be 0x02 
      buf[0] = 0x00;
      buf[1] = 0x02;

      // check that there are no 0x00 bytes among random data
      for (unsigned int i = 2; i < encryptSize - inLength - 1; i++) {

        while (buf[i] == 0x00) {
          // replace all 0x00 occurences in random data with random value
          PK11_GenerateRandom(&buf[i], 1);
        }

      }

      // before key add 0x00 byte
      buf[encryptSize - inLength - 1] = 0x00;

      // at the end of input buffer (to be encrypted) add key
      memcpy(&buf[encryptSize - inLength], inBuf, inLength);

    }

    // encrypt
    s = PK11_PubEncryptRaw(mp_pubkey, cipherBuf, (unsigned char*)buf, encryptSize, NULL);

    if (s != SECSuccess) {

			throw XSECCryptoException(XSECCryptoException::RSAError,
				"NSS:RSA publicKeyEncrypt - Error performing encrypt");

    }

  	break;

	case XSECCryptoKeyRSA::PAD_OAEP_MGFP1 :

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - OAEP padding method not supported in NSS yet");

		break;

	default :

		throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Unknown padding method");

	}

	return encryptSize;

}

// --------------------------------------------------------------------------------
//           Get key size in bytes
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::getLength(void)  const {

  unsigned int ret = 0;

  if (mp_pubkey != 0) {

    ret = SECKEY_PublicKeyStrength(mp_pubkey);

  } else if (mp_privkey != 0) {

    ret = PK11_GetPrivateModulusLen(mp_privkey);

  }

  return ret;

}

// --------------------------------------------------------------------------------
//           Load parameters from key (utility function)
// --------------------------------------------------------------------------------

void NSSCryptoKeyRSA::loadParamsFromKey(void) {

	if (mp_pubkey == 0)
			return;

  mp_modulus = SECITEM_DupItem(&(mp_pubkey->u.rsa.modulus));

  if (mp_modulus == 0)
  {

    throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error during extracting modulus from public key");
 
  }

  mp_exponent = SECITEM_DupItem(&(mp_pubkey->u.rsa.publicExponent));

  if (mp_exponent == 0)
  {

    throw XSECCryptoException(XSECCryptoException::RSAError,
			"NSS:RSA - Error during extracting exponent from public key");

  }
}

// --------------------------------------------------------------------------------
//           Get exponent
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::getExponentBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_exponent == 0) {

		return 0;	// Nothing we can do

	}

	if (mp_exponent == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b =  NSSCryptoProvider::SI2b64(mp_exponent, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;
}

// --------------------------------------------------------------------------------
//           Get modulus
// --------------------------------------------------------------------------------

unsigned int NSSCryptoKeyRSA::getModulusBase64BigNums(char * b64, unsigned int len) {

	if (mp_pubkey == 0 && mp_modulus == 0) {

		return 0;	// Nothing we can do

	}

	if (mp_modulus == NULL) {

		loadParamsFromKey();

	}

	unsigned int bLen = 0;
	unsigned char * b =  NSSCryptoProvider::SI2b64(mp_modulus, bLen);
	if (bLen > len)
		bLen = len;
	memcpy(b64, b, bLen);
	delete[] b;

	return bLen;
}

#endif /* XSEC_HAVE_NSS */
