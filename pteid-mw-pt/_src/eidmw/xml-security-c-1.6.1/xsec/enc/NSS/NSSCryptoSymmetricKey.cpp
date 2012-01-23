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
 * XSECCryptoSymmetricKey := Bulk encryption algorithms should all be
 *							implemented via this interface
 *
 * Author(s): Milan Tomic
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/enc/NSS/NSSCryptoProvider.hpp>
#include <xsec/enc/NSS/NSSCryptoSymmetricKey.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

#include <xercesc/util/Janitor.hpp>

#if defined (XSEC_HAVE_NSS)

#include "prerror.h"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructor
// --------------------------------------------------------------------------------

NSSCryptoSymmetricKey::NSSCryptoSymmetricKey(XSECCryptoSymmetricKey::SymmetricKeyType type) :
m_keyType(type),
m_keyMode(MODE_CBC),
m_initialised(false),
mp_k(NULL)
{

	

}

// --------------------------------------------------------------------------------
//           Destructor
// --------------------------------------------------------------------------------

NSSCryptoSymmetricKey::~NSSCryptoSymmetricKey() {

  if (mp_k != 0)
	  PK11_FreeSymKey(mp_k);

}

// --------------------------------------------------------------------------------
//           Get key type
// --------------------------------------------------------------------------------

XSECCryptoSymmetricKey::SymmetricKeyType NSSCryptoSymmetricKey::getSymmetricKeyType() const {

	return m_keyType;

}

// --------------------------------------------------------------------------------
//           Get provider name
// --------------------------------------------------------------------------------

const XMLCh * NSSCryptoSymmetricKey::getProviderName() const {

	return DSIGConstants::s_unicodeStrPROVNSS;

}

// --------------------------------------------------------------------------------
//           Replicate key
// --------------------------------------------------------------------------------

XSECCryptoKey * NSSCryptoSymmetricKey::clone() const {

	NSSCryptoSymmetricKey * ret;

	XSECnew(ret, NSSCryptoSymmetricKey(m_keyType));

	if (mp_k != 0) {

    ret->mp_k = PK11_ReferenceSymKey(mp_k);

  }

  ret->m_keyType = m_keyType;

	return ret;

}

// --------------------------------------------------------------------------------
//           Store the key value
// --------------------------------------------------------------------------------

void NSSCryptoSymmetricKey::setKey(const unsigned char * key, unsigned int keyLen) {

  if (mp_k != 0) {
		PK11_FreeSymKey(mp_k);
    mp_k = 0;
  }

  CK_MECHANISM_TYPE cipherMech;

  switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

    cipherMech = CKM_DES3_CBC_PAD;
    break;
  
  case (XSECCryptoSymmetricKey::KEY_AES_128) :
  case (XSECCryptoSymmetricKey::KEY_AES_192) :
  case (XSECCryptoSymmetricKey::KEY_AES_256) :
  
    cipherMech = CKM_AES_CBC_PAD;
    break;
  
  default:
  
    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Unknown key type");

  }

  // Turn the raw key into a SECItem
  SECItem keyItem;
  keyItem.data = (unsigned char*)key;
  keyItem.len = keyLen;

  PK11SlotInfo * slot = PK11_GetBestSlot(cipherMech, NULL);

  // Turn the SECItem into a key object
  mp_k = PK11_ImportSymKey(slot,
                          cipherMech,
                          PK11_OriginUnwrap,
                          CKA_ENCRYPT,
                          &keyItem,
                          NULL);

  if (slot)
	  PK11_FreeSlot(slot);

}

// --------------------------------------------------------------------------------
//           Decrypt context initialisation
// --------------------------------------------------------------------------------

int NSSCryptoSymmetricKey::decryptCtxInit(const unsigned char * iv) {

	// Returns amount of IV data used (in bytes)
	// Sets m_initialised if the key is OK and the IV is OK.

	if (m_initialised)
		return 0;

	if (mp_k == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Cannot initialise without key"); 

	}

	// Set up the context according to the required cipher type

	switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		// A 3DES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {

				return 0;	// Cannot initialise without an IV

			}

      SECItem ivItem;
      ivItem.data = (unsigned char*)iv;
      ivItem.len = 8;
	  int encryptAlg = (m_doPad == true ? CKM_DES3_CBC_PAD : CKM_DES3_CBC);

      SECItem * secParam = PK11_ParamFromIV(encryptAlg, &ivItem);
      mp_ctx = PK11_CreateContextBySymKey(encryptAlg, CKA_DECRYPT, mp_k, secParam);

      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);
			
      m_ivSize = 8;
		}
		else {
			SECItem * secParam = PK11_ParamFromIV(CKM_DES3_ECB, NULL);
      mp_ctx = PK11_CreateContextBySymKey(CKM_DES3_ECB, CKA_DECRYPT, mp_k, secParam);
      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);	
      m_ivSize = 0;
		}

		break;

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
  case (XSECCryptoSymmetricKey::KEY_AES_192) :
  case (XSECCryptoSymmetricKey::KEY_AES_256) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {

				return 0;	// Cannot initialise without an IV

			}

      SECItem ivItem;
      ivItem.data = (unsigned char*)iv;
      ivItem.len = 16;

      SECItem * secParam = PK11_ParamFromIV(CKM_AES_CBC_PAD, &ivItem);
      mp_ctx = PK11_CreateContextBySymKey(CKM_AES_CBC_PAD, CKA_DECRYPT, mp_k, secParam);

      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);

      m_ivSize = 16;

		}
		else {
			SECItem * secParam = PK11_ParamFromIV(CKM_AES_ECB, NULL);
			mp_ctx = PK11_CreateContextBySymKey(CKM_AES_ECB, CKA_DECRYPT, mp_k, secParam);
      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);

      m_ivSize = 0;

		}

		break;
	
	default :

		// Cannot do this without an IV
		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Unknown key type"); 

	}

	// Reset some parameters
	m_initialised = true;

	// Return number of bytes chewed up by IV
	return m_ivSize;
}

// --------------------------------------------------------------------------------
//           Decrypt initialisation
// --------------------------------------------------------------------------------

bool NSSCryptoSymmetricKey::decryptInit(bool doPad, 
											SymmetricKeyMode mode, 
											const unsigned char * iv) {

	m_initialised = false;
    m_ivSent = iv == NULL;
	m_doPad = doPad;
	m_keyMode = mode;
	decryptCtxInit(iv);
	return true;

}

// --------------------------------------------------------------------------------
//           Decrypt
// --------------------------------------------------------------------------------

unsigned int NSSCryptoSymmetricKey::decrypt(const unsigned char * inBuf, 
								 unsigned char * plainBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {

	// NOTE: This won't actually stop NSS blowing the buffer, so the onus is
	// on the caller.

	unsigned int offset = 0;
	if (!m_initialised) {
		offset = decryptCtxInit(inBuf);
		if (offset > inLength) {
			throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Not enough data passed in to get IV");
		}
	}

	int outl = inLength - offset;

  SECStatus s = PK11_CipherOp(mp_ctx, plainBuf, &outl, maxOutLength, (unsigned char*)inBuf, inLength);

  if (s != SECSuccess) {

    throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Error during NSS decrypt");

  }

  // remove IV
  if (m_ivSent) {
    memmove(plainBuf, &plainBuf[m_ivSize], outl);
    outl -= m_ivSize;
    m_ivSent = false;
  }

	return outl;

}

// --------------------------------------------------------------------------------
//           Decrypt finalisation
// --------------------------------------------------------------------------------

unsigned int NSSCryptoSymmetricKey::decryptFinish(unsigned char * plainBuf,
													  unsigned int maxOutLength) {

	unsigned int outl = 0;

  SECStatus s = PK11_DigestFinal(mp_ctx, plainBuf, &outl, maxOutLength);

  if (s != SECSuccess) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Error during NSS decrypt finalisation");

  }

  PK11_DestroyContext(mp_ctx, PR_TRUE);
  mp_ctx = NULL;
  m_initialised = false;

  return outl;
}

// --------------------------------------------------------------------------------
//           Encrypt initialisation
// --------------------------------------------------------------------------------

bool NSSCryptoSymmetricKey::encryptInit(bool doPad, 
											SymmetricKeyMode mode, 
											const unsigned char * iv) {

	if (m_initialised == true)
		return true;

	m_doPad = doPad;
	m_keyMode = mode;
	
	if (mp_k == 0) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Cannot initialise without key"); 

	}

	// Do some parameter initialisation
	m_initialised = true;

	// Set up the context according to the required cipher type

	const unsigned char * usedIV = NULL;
	unsigned char genIV[256];

	// Tell the library that the IV still has to be sent

	m_ivSent = false;

	switch (m_keyType) {

	case (XSECCryptoSymmetricKey::KEY_3DES_192) :

		// A 3DES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				
        SECStatus s = PK11_GenerateRandom(genIV, 8);

        if (s != SECSuccess) {

					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"NSS:SymmetricKey - Error generating random IV");

        }

				usedIV = genIV;

			}
			else
				usedIV = iv;

      SECItem ivItem;
      ivItem.data = (unsigned char*)usedIV;
      ivItem.len = 8;
	  int encryptAlg = (m_doPad == true ? CKM_DES3_CBC_PAD : CKM_DES3_CBC);

      SECItem * secParam = PK11_ParamFromIV(encryptAlg, &ivItem);
      mp_ctx = PK11_CreateContextBySymKey(encryptAlg, CKA_ENCRYPT, mp_k, secParam);

      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);

      m_ivSize = 8;
		}
		else {
      mp_ctx = PK11_CreateContextBySymKey(CKM_DES3_ECB, CKA_ENCRYPT, mp_k, NULL);

      m_ivSize = 0;
		}

		break;

	case (XSECCryptoSymmetricKey::KEY_AES_128) :
  case (XSECCryptoSymmetricKey::KEY_AES_192) :
  case (XSECCryptoSymmetricKey::KEY_AES_256) :

		// An AES key

		if (m_keyMode == MODE_CBC) {

			if (iv == NULL) {
				
				SECStatus s = PK11_GenerateRandom(genIV, 16);
				
        if (s != SECSuccess) {

					throw XSECCryptoException(XSECCryptoException::SymmetricError,
						"NSS:SymmetricKey - Error generating random IV");

        }

				usedIV = genIV;

			}
			else
				usedIV = iv;

			SECItem ivItem;
      ivItem.data = (unsigned char*)usedIV;
      ivItem.len = 16;

      SECItem * secParam = PK11_ParamFromIV(CKM_AES_CBC_PAD, &ivItem);
      mp_ctx = PK11_CreateContextBySymKey(CKM_AES_CBC_PAD, CKA_ENCRYPT, mp_k, secParam);

      if (secParam)
        SECITEM_FreeItem(secParam, PR_TRUE);

      m_ivSize = 16;
		}
		else {
			SECItem * secParam = PK11_ParamFromIV(CKM_AES_ECB, NULL);
			mp_ctx = PK11_CreateContextBySymKey(CKM_AES_ECB, CKA_ENCRYPT, mp_k, secParam);
			if (secParam)
				SECITEM_FreeItem(secParam, PR_TRUE);

      m_ivSize = 0;
		}

		break;

	default :

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Unknown key type"); 

	}

	// Add IV
	if (m_keyMode == MODE_CBC) {

		memcpy(m_lastBlock, usedIV, m_ivSize);

	}

	return true;

}

// --------------------------------------------------------------------------------
//           Encrypt
// --------------------------------------------------------------------------------

unsigned int NSSCryptoSymmetricKey::encrypt(const unsigned char * inBuf, 
								 unsigned char * cipherBuf, 
								 unsigned int inLength,
								 unsigned int maxOutLength) {

  if (m_initialised == false) {

		encryptInit();

	}

	// NOTE: This won't actually stop NSS blowing the buffer, so the onus is
	// on the caller.

	unsigned int offset = 0;
	if (m_ivSent == false && m_ivSize > 0) {

		memcpy(cipherBuf, m_lastBlock, m_ivSize);
		m_ivSent = true;

		offset = m_ivSize;

	}

	int outl = 0;

	if (inLength + offset > maxOutLength) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Not enough space in output buffer for encrypt"); 

	}

  SECStatus s = PK11_CipherOp(mp_ctx,
                              &cipherBuf[offset],
                              &outl,
                              maxOutLength - offset,
                              (unsigned char*)inBuf,
                              inLength);

	if (s != SECSuccess) {

		throw XSECCryptoException(XSECCryptoException::SymmetricError,
			"NSS:SymmetricKey - Error during NSS encrypt"); 

	}

	return outl + offset;

}

// --------------------------------------------------------------------------------
//           Encrypt finalisation
// --------------------------------------------------------------------------------

unsigned int NSSCryptoSymmetricKey::encryptFinish(unsigned char * cipherBuf,
													  unsigned int maxOutLength) {

	unsigned int outl = 0;

	SECStatus s = PK11_DigestFinal(mp_ctx, cipherBuf, &outl, maxOutLength);

		if (s != SECSuccess) {

			throw XSECCryptoException(XSECCryptoException::SymmetricError,
				"NSS:SymmetricKey - Error during NSS encrypt"); 

	}

	PK11_DestroyContext(mp_ctx, PR_TRUE);
	mp_ctx = NULL;

	// Setup so can be re-used
	m_initialised = false;

	return outl;

}

#endif /* XSEC_HAVE_NSS */
