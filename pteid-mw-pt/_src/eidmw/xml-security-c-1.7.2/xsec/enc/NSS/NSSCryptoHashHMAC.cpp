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
 * NSSCryptoHashHMAC := NSS Implementation of HMAC
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/enc/NSS/NSSCryptoHashHMAC.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/enc/NSS/NSSCryptoKeyHMAC.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#if defined (XSEC_HAVE_NSS)

#include <memory.h>

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoHashHMAC::NSSCryptoHashHMAC(HashType alg) {

  switch (alg) {

	case (XSECCryptoHash::HASH_SHA1) :
	
		mp_md = PK11_CreateDigestContext(SEC_OID_SHA1);
		break;

  case (XSECCryptoHash::HASH_SHA256) :
	
		mp_md = PK11_CreateDigestContext(SEC_OID_SHA256);
		break;

  case (XSECCryptoHash::HASH_SHA384) :
	
		mp_md = PK11_CreateDigestContext(SEC_OID_SHA384);
		break;

  case (XSECCryptoHash::HASH_SHA512) :
	
		mp_md = PK11_CreateDigestContext(SEC_OID_SHA512);
		break;

	case (XSECCryptoHash::HASH_MD5) :
	
		mp_md = PK11_CreateDigestContext(SEC_OID_MD5);
		break;

	default :

		throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Unknown algorithm"); 

	}

	m_hashType  = alg;

  m_blockSize = XSEC_MAX_HASH_BLOCK_SIZE;

	SECStatus s = PK11_DigestBegin(mp_md);
  
  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Error initializing Message Digest"); 

  }

}

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoHashHMAC::~NSSCryptoHashHMAC() {

	if (mp_md != 0)
		PK11_DestroyContext(mp_md, PR_TRUE);

}

// --------------------------------------------------------------------------------
//           Reset
// --------------------------------------------------------------------------------

void NSSCryptoHashHMAC::reset() {

	SECStatus s = PK11_DigestBegin(mp_md);
  
  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Error initializing Message Digest"); 

  }

}

// --------------------------------------------------------------------------------
//           Key manipulation
// --------------------------------------------------------------------------------

void NSSCryptoHashHMAC::setKey(XSECCryptoKey *key) {

  if (key->getKeyType() != XSECCryptoKey::KEY_HMAC) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:HashHMAC - Non HMAC Key passed to HashHMAC");

	}

  m_keyLen = ((XSECCryptoKeyHMAC *) key)->getKey(m_keyBuf);

  PK11SlotInfo * slot = PK11_GetInternalKeySlot();

  if (slot == 0) {

    throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:HashHMAC - Failed to get internal key slot");

  }

  SECItem keyItem;
  keyItem.data = (unsigned char *) m_keyBuf.rawBuffer();
  keyItem.len  = m_keyLen;

  CK_MECHANISM_TYPE hmacType;

  switch (m_hashType) {

	case (XSECCryptoHash::HASH_SHA1) :
	
		hmacType = CKM_SHA_1_HMAC;

		break;

	case (XSECCryptoHash::HASH_MD5) :

		hmacType = CKM_MD5_HMAC;

		break;

	default :

		hmacType = 0;

	}

  if(hmacType == 0) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Unknown algorithm");

	}

  PK11SymKey * symKey = PK11_ImportSymKey(slot,
                                         hmacType, 
                                         PK11_OriginUnwrap,
                                         CKA_SIGN,
                                         &keyItem,
                                         NULL);

  SECItem noParams;
  noParams.data = 0;
  noParams.len  = 0;

	mp_md = PK11_CreateContextBySymKey(hmacType, 
                                     CKA_SIGN,
                                     symKey, 
                                     &noParams);

	if(mp_md == 0) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Unknown algorithm");

	}

  if (symKey)
    PK11_FreeSymKey(symKey);

  if (slot)
    PK11_FreeSlot(slot);

}

// --------------------------------------------------------------------------------
//           Perform hash operation
// --------------------------------------------------------------------------------

void NSSCryptoHashHMAC::hash(unsigned char * data,
								 unsigned int length) {

  if (mp_md == 0) {

		throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:HashHMAC - hash called prior to setKey");

  }

  SECStatus s = PK11_DigestOp(mp_md, data, length);

  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Error Hashing Data");

  }

}

// --------------------------------------------------------------------------------
//           Finish
// --------------------------------------------------------------------------------

unsigned int NSSCryptoHashHMAC::finish(unsigned char * hash,
									   unsigned int maxLength) {

  unsigned int retLen = XSEC_MAX_HASH_SIZE;

  SECStatus s = PK11_DigestFinal(mp_md, m_mdValue, &retLen, XSEC_MAX_HASH_SIZE);

  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::MDError,
			"NSS:Hash - Error getting hash value"); 

  }

  m_mdLen = retLen;

  retLen = (maxLength > m_mdLen ? m_mdLen : maxLength);
	memcpy(hash, m_mdValue, retLen);

	return (unsigned int) retLen;

}

// --------------------------------------------------------------------------------
//           Get information
// --------------------------------------------------------------------------------

XSECCryptoHash::HashType NSSCryptoHashHMAC::getHashType(void) const {

	return m_hashType;			// This could be any kind of hash

}

#endif /* XSEC_HAVE_NSS */