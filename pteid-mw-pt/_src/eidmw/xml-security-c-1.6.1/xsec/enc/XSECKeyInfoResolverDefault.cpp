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
 * XSECKeyInfoResolverDefault := Default (very basic) class for applications
 *						 to map KeyInfo to keys
 *
 * $Id: XSECKeyInfoResolverDefault.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/enc/XSECKeyInfoResolverDefault.hpp>
#include <xsec/dsig/DSIGKeyInfoX509.hpp>
#include <xsec/dsig/DSIGKeyInfoValue.hpp>
#include <xsec/dsig/DSIGKeyInfoDEREncoded.hpp>
#include <xsec/framework/XSECError.hpp>

#include "../utils/XSECAutoPtr.hpp"

#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Construct/Destruct
// --------------------------------------------------------------------------------
XSECKeyInfoResolverDefault::XSECKeyInfoResolverDefault() {

	// Create a UTF-8 formatter
	XSECnew(mp_formatter, XSECSafeBufferFormatter("UTF-8",XMLFormatter::NoEscapes, 
												XMLFormatter::UnRep_CharRef));
}


XSECKeyInfoResolverDefault::~XSECKeyInfoResolverDefault() {

	if (mp_formatter != NULL)
		delete mp_formatter;

}


// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------


XSECCryptoKey * XSECKeyInfoResolverDefault::resolveKey(DSIGKeyInfoList * lst) {

	// Try to find a key from the KeyInfo list as best we can
	// NOTE: No validation is performed (i.e. no cert/CRL checks etc.)

	XSECCryptoKey * ret = NULL;

	DSIGKeyInfoList::size_type sz = lst->getSize();

	for (DSIGKeyInfoList::size_type i = 0; i < sz; ++i) {

		switch (lst->item(i)->getKeyInfoType()) {

		case (DSIGKeyInfo::KEYINFO_X509) :
		{
			ret = NULL;
			const XMLCh * x509Str;
			XSECCryptoX509 * x509 = XSECPlatformUtils::g_cryptoProvider->X509();
			Janitor<XSECCryptoX509> j_x509(x509);

			x509Str = ((DSIGKeyInfoX509 *) lst->item(i))->getCertificateItem(0);
			
			if (x509Str != 0) {

				// The crypto interface classes work UTF-8
				safeBuffer transX509;

				transX509 << (*mp_formatter << x509Str);
				x509->loadX509Base64Bin(transX509.rawCharBuffer(), (unsigned int) strlen(transX509.rawCharBuffer()));
				ret = x509->clonePublicKey();
			}

			if (ret != NULL)
				return ret;
		
		}
			break;

		case (DSIGKeyInfo::KEYINFO_VALUE_DSA) :
		{

			XSECCryptoKeyDSA * dsa = XSECPlatformUtils::g_cryptoProvider->keyDSA();
			Janitor<XSECCryptoKeyDSA> j_dsa(dsa);

			safeBuffer value;

			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getDSAP());
			dsa->loadPBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));
			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getDSAQ());
			dsa->loadQBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));
			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getDSAG());
			dsa->loadGBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));
			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getDSAY());
			dsa->loadYBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));

			j_dsa.release();
			return dsa;
		}
			break;

		case (DSIGKeyInfo::KEYINFO_VALUE_RSA) :
		{

			XSECCryptoKeyRSA * rsa = XSECPlatformUtils::g_cryptoProvider->keyRSA();
			Janitor<XSECCryptoKeyRSA> j_rsa(rsa);

			safeBuffer value;

			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getRSAModulus());
			rsa->loadPublicModulusBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));
			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getRSAExponent());
			rsa->loadPublicExponentBase64BigNums(value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));

			j_rsa.release();
			return rsa;

		}
            break;

        case (DSIGKeyInfo::KEYINFO_VALUE_EC) :
        {

            XSECCryptoKeyEC* ec = XSECPlatformUtils::g_cryptoProvider->keyEC();
            Janitor<XSECCryptoKeyEC> j_ec(ec);

            safeBuffer value;
			value << (*mp_formatter << ((DSIGKeyInfoValue *) lst->item(i))->getECPublicKey());
            XSECAutoPtrChar curve(((DSIGKeyInfoValue *) lst->item(i))->getECNamedCurve());
            if (curve.get()) {
                ec->loadPublicKeyBase64(curve.get(), value.rawCharBuffer(), (unsigned int) strlen(value.rawCharBuffer()));
                j_ec.release();
                return ec;
            }
        }
            break;

        case (DSIGKeyInfo::KEYINFO_DERENCODED) :
        {
            safeBuffer value;
			value << (*mp_formatter << ((DSIGKeyInfoDEREncoded *) lst->item(i))->getData());
            return XSECPlatformUtils::g_cryptoProvider->keyDER(value.rawCharBuffer(), (unsigned int)strlen(value.rawCharBuffer()), true);
        }
            break;

		default :
			break;

		}
	}

	return NULL;

}


XSECKeyInfoResolver * XSECKeyInfoResolverDefault::clone(void) const {

	return new XSECKeyInfoResolverDefault();

}
