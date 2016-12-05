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
 * XSECCryptoUtils:= Helper crypo utilities that make life easier
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoUtils.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoUtils.hpp>
#include <xsec/enc/XSECCryptoKeyHMAC.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>

#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

#include "../utils/XSECAutoPtr.hpp"

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           XKMS Limited-Use Shared Secret handling
// --------------------------------------------------------------------------------

int CleanXKMSPassPhrase(unsigned char * input, int inputLen, safeBuffer &output) {

	// Now obsolete - use SASLCleanXKMSPassPhrase instead
	int j = 0;
	unsigned char c;
	for (int i = 0; i < inputLen; ++i) {

		c = input[i];

		if (c >= 'A' && c <= 'Z') {
			output[j++] = c - 'A' + 'a';
		}
		else if (c != '\n' && c != '\r' && c != '\t' && c != ' ') {
			output[j++] = c;
		}

	}

	return j;

}

int SASLCleanXKMSPassPhrase(unsigned char * input, int inputLen, safeBuffer &output) {

	// For now - this does *not* implement the full SASLPrep algorithm.
	// THe NFKC form is not trivial to implement - so this is kept very simple.
	// TODO - Fix this - it can be an interoperability issue as pass phrases from
	// different implementations could be treated differently.
	// Currently we only check for prohibited unput for chars < 0xFFFF and drop any
	// chars over 0xFFFF

	unsigned char * inp = new unsigned char[inputLen + 1];
	ArrayJanitor<unsigned char> j_inp(inp);
	memcpy(inp, input, inputLen);
	inp[inputLen] = '\0';

	XSECAutoPtrXMLCh uinput((char *) inp);
	xsecsize_t l = XMLString::stringLen(uinput.get());
	XMLCh* uoutput = new XMLCh[l + 1];
	ArrayJanitor<XMLCh> j_uoutput(uoutput);

	xsecsize_t i, j;
	j = 0;

	XMLCh ch1;

	for (i = 0; i < l; ++i) {

		ch1 = uinput.get()[i];
		// Case one - char is < 0x10000
		if (ch1 < 0xD800 || ch1 > 0xDFFF) {

			// OK - ch1 is "real" value - let's see if it is legal
			// The following switch tables are derived from
			// RFC 3454 - see http://www.ietf.org/rfc/rfc3454.txt

			// Non-ASCII Spaces - C.1.2
			switch (ch1) {

			case 0x00A0:		// NO-BREAK SPACE
			case 0x1680:		// OGHAM SPACE MARK
			case 0x2000:		// EN QUAD
			case 0x2001:		// EM QUAD
			case 0x2002:		// EN SPACE
			case 0x2003:		// EM SPACE
			case 0x2004:		// THREE-PER-EM SPACE
			case 0x2005:		// FOUR-PER-EM SPACE
			case 0x2006:		// SIX-PER-EM SPACE
			case 0x2007:		// FIGURE SPACE
			case 0x2008:		// PUNCTUATION SPACE
			case 0x2009:		// THIN SPACE
			case 0x200A:		// HAIR SPACE
			case 0x200B:		// ZERO WIDTH SPACE
			case 0x202F:		// NARROW NO-BREAK SPACE
			case 0x205F:		// MEDIUM MATHEMATICAL SPACE
			case 0x3000:		// IDEOGRAPHIC SPACE

				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - Non ASCII space character in XKMS pass phrase");
			default:

				break;

			}

			// ASCII Control characters
            // Note - us unsigned, so always >= 0)
			if ((ch1 <= 0x1F) || (ch1 == 0x7F)) {
				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - ASCII control character in XKMS pass phrase");
			}

			// Non-ASCII Control Characters
			if ((ch1 >= 0x80 && ch1 <= 0x9F) ||
				(ch1 >= 0x206A && ch1 <= 0x206F) ||
				(ch1 >= 0xFFF9 && ch1 <= 0xFFFC)) {

				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - NON ASCII control character in XKMS pass phrase");
			}

			switch (ch1) {


			case 0x06DD:		// ARABIC END OF AYAH
			case 0x070F:		// SYRIAC ABBREVIATION MARK
			case 0x180E:		// MONGOLIAN VOWEL SEPARATOR
			case 0x200C:		// ZERO WIDTH NON-JOINER
			case 0x200D:		// ZERO WIDTH JOINER
			case 0x2028:		// LINE SEPARATOR
			case 0x2029:		// PARAGRAPH SEPARATOR
			case 0x2060:		// WORD JOINER
			case 0x2061:		// FUNCTION APPLICATION
			case 0x2062:		// INVISIBLE TIMES
			case 0x2063:		// INVISIBLE SEPARATOR
			case 0xFEFF:		// ZERO WIDTH NO-BREAK SPACE
				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - NON ASCII control character in XKMS pass phrase");
			default:
				break;			}
		    // 1D173-1D17A; [MUSICAL CONTROL CHARACTERS] is not relevant as we are looking at
		    // ch1 at the moment

			// Private Use characters
			if ((ch1 >= 0xE000 && ch1 <= 0xF8FF)) {

				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - Private Use character in XKMS pass phrase");
			}

			// Non-character code points
			if ((ch1 >= 0xFDD0 && ch1 <= 0xFDEF) ||
				(ch1 >= 0xFFFE)) {

				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - Non-character code points in XKMS pass phrase");
			}

			// Inappropriate for plain text characters

			switch (ch1) {
			case 0xFFF9:		// INTERLINEAR ANNOTATION ANCHOR
			case 0xFFFA:		// INTERLINEAR ANNOTATION SEPARATOR
			case 0xFFFB:		// INTERLINEAR ANNOTATION TERMINATOR
			case 0xFFFC:		// OBJECT REPLACEMENT CHARACTER
			case 0xFFFD:		// REPLACEMENT CHARACTER
				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - Innappropriate for plain text chararcters in XKMS pass phrase");
			default:
				break;
			}

			// Inappripriate for canonical representation characters
			if (ch1 >= 0x2FF0 && ch1 <= 0x2FFB) {
				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - Innappropriate for canonicalisation chararcters in XKMS pass phrase");
			}

			// Change display properties or are deprecated
			switch (ch1) {
			case 0x0340:		// COMBINING GRAVE TONE MARK
			case 0x0341:		// COMBINING ACUTE TONE MARK
			case 0x200E:		// LEFT-TO-RIGHT MARK
			case 0x200F:		// RIGHT-TO-LEFT MARK
			case 0x202A:		// LEFT-TO-RIGHT EMBEDDING
			case 0x202B:		// RIGHT-TO-LEFT EMBEDDING
			case 0x202C:		// POP DIRECTIONAL FORMATTING
			case 0x202D:		// LEFT-TO-RIGHT OVERRIDE
			case 0x202E:		// RIGHT-TO-LEFT OVERRIDE
			case 0x206A:		// INHIBIT SYMMETRIC SWAPPING
			case 0x206B:		// ACTIVATE SYMMETRIC SWAPPING
			case 0x206C:		// INHIBIT ARABIC FORM SHAPING
			case 0x206D:		// ACTIVATE ARABIC FORM SHAPING
			case 0x206E:		// NATIONAL DIGIT SHAPES
			case 0x206F:		// NOMINAL DIGIT SHAPES
				throw XSECException(XSECException::XKMSError,
					"SASLCleanXKMSPassPhrase - change display or deprecated chararcters in XKMS pass phrase");
			default:
				break;
			}

			// We got this far = just run with it for now
			uoutput[j++] = ch1;
		}
		else {
			throw XSECException(XSECException::XKMSError,
				"SASLCleanXKMSPassPhrase - don't support XKMS pass phrase chars > 0xFFFF");
		}
	} /* for */
	uoutput[j++] = chNull;

	// Now transcode
	char * utf8output= transcodeToUTF8(uoutput);
	output.sbStrcpyIn(utf8output);

	int ret = (int)strlen(utf8output);
	XSEC_RELEASE_XMLCH(utf8output);
	return ret;
}

int DSIG_EXPORT CalculateXKMSAuthenticationKey(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen) {

	unsigned char keyVal[] = {XKMSAuthenticationValue};

	XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
	Janitor<XSECCryptoKeyHMAC> j_k(k);
	k->setKey(keyVal, 1);

	XSECCryptoHash *h = XSECPlatformUtils::g_cryptoProvider->hashHMACSHA1();
	Janitor<XSECCryptoHash> j_h(h);
	h->setKey(k);

	// Clean the input
	safeBuffer sb;
	int l = SASLCleanXKMSPassPhrase(input, inputLen, sb);

	h->hash((unsigned char *) sb.rawBuffer(), l);
	return h->finish(output, maxOutputLen);

}


int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding1(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen) {

	unsigned char keyVal[] = {XKMSRevocationCodeIdenfitierEncoding1};

	XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
	Janitor<XSECCryptoKeyHMAC> j_k(k);
	k->setKey(keyVal, 1);

	XSECCryptoHash *h = XSECPlatformUtils::g_cryptoProvider->hashHMACSHA1();
	Janitor<XSECCryptoHash> j_h(h);

	h->setKey(k);

	// Clean the input
	safeBuffer sb;
	int l = CleanXKMSPassPhrase(input, inputLen, sb);

	h->hash((unsigned char *) sb.rawBuffer(), l);
	return h->finish(output, maxOutputLen);

}

int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding2(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen) {


	unsigned char tmpBuf[XSEC_MAX_HASH_SIZE];
	int tmpLen = CalculateXKMSRevocationCodeIdentifierEncoding1(input, inputLen, tmpBuf, XSEC_MAX_HASH_SIZE);
	return CalculateXKMSRevocationCodeIdentifierEncoding2From1(tmpBuf, tmpLen, output, maxOutputLen);

}

int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding2From1(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen) {

	unsigned char keyVal[] = {XKMSRevocationCodeIdenfitierEncoding2};

	XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
	Janitor<XSECCryptoKeyHMAC> j_k(k);
	k->setKey(keyVal, 1);

	XSECCryptoHash *h = XSECPlatformUtils::g_cryptoProvider->hashHMACSHA1();
	Janitor<XSECCryptoHash> j_h(h);

	h->setKey(k);

	h->hash(input, inputLen);
	return h->finish(output, maxOutputLen);

}

int DSIG_EXPORT CalculateXKMSKEK(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen) {

	unsigned char keyVal[] = {XKMSKeyEncryption};
	unsigned char shaOutput[22];	// SHA1 has 20 bytes of output

	// Clean the input
	safeBuffer sb;
	int l = SASLCleanXKMSPassPhrase(input, inputLen, sb);

	// Need to iterate through until we have enough data
	int bytesDone = 0, bytesToDo;;
	shaOutput[0] = keyVal[0];
	int keyLen = 1;
	while (bytesDone < maxOutputLen) {
		XSECCryptoKeyHMAC * k = XSECPlatformUtils::g_cryptoProvider->keyHMAC();
		k->setKey(shaOutput, keyLen);

		XSECCryptoHash *h = XSECPlatformUtils::g_cryptoProvider->hashHMACSHA1();
		Janitor<XSECCryptoHash> j_h(h);

		h->setKey(k);
		delete k;

		// Now hash next round of data
		h->hash((unsigned char *) sb.rawBuffer(), l);
		keyLen = h->finish(shaOutput, 22);

		// Copy into the output buffer
		bytesToDo = maxOutputLen - bytesDone;
		bytesToDo = bytesToDo > 20 ? 20 : bytesToDo;
		memcpy(&output[bytesDone], shaOutput, bytesToDo);
		bytesDone += bytesToDo;

		// Set up for next key
		shaOutput[0] ^= keyVal[0];
		keyLen = 20;

		j_h.release();
		delete h;
	}

	return bytesDone;

}

// --------------------------------------------------------------------------------
//           Some Base64 helpers
// --------------------------------------------------------------------------------

XMLCh DSIG_EXPORT * EncodeToBase64XMLCh(unsigned char * input, int inputLen) {

	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	Janitor<XSECCryptoBase64> j_b64(b64);
	unsigned char * output;
	int outputLen = ((4 * inputLen) / 3) + 5;
	XSECnew(output, unsigned char[outputLen]);
	ArrayJanitor<unsigned char> j_output(output);

	b64->encodeInit();
	int j = b64->encode(input, inputLen, output, outputLen - 1);
	j += b64->encodeFinish(&output[j], outputLen - j - 1);

	// Strip any trailing \n\r
	while (j > 0 && (output[j-1] == '\n' || output[j-1] == '\r'))
		j--;

	// Now transcode and get out of here
	output[j] = '\0';
	return XMLString::transcode((char *) output);

}

unsigned int DSIG_EXPORT DecodeFromBase64XMLCh(const XMLCh * input, unsigned char * output, int maxOutputLen) {

	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	Janitor<XSECCryptoBase64> j_b64(b64);

	XSECAutoPtrChar tinput(input);

	b64->decodeInit();
	unsigned int j = b64->decode((unsigned char *) tinput.get(), (unsigned int) strlen(tinput.get()), output, maxOutputLen - 1);
	j += b64->decodeFinish(&output[j], maxOutputLen - j - 1);

	return j;
}

unsigned int DSIG_EXPORT DecodeFromBase64(const char * input, unsigned char * output, int maxOutputLen) {

	XSECCryptoBase64 * b64 = XSECPlatformUtils::g_cryptoProvider->base64();
	Janitor<XSECCryptoBase64> j_b64(b64);

	b64->decodeInit();
	unsigned int j = b64->decode((unsigned char *) input, (unsigned int) strlen(input), output, maxOutputLen - 1);
	j += b64->decodeFinish(&output[j], maxOutputLen - j - 1);

	return j;
}


// --------------------------------------------------------------------------------
//           Some stuff to help with wierd signatures
// --------------------------------------------------------------------------------

const unsigned char ASNDSAProlog[] = {0x30, 0x2c, 0x02, 0x14};
const unsigned char ASNDSAMiddle[] = {0x02, 0x14};

bool ASN2DSASig(const unsigned char * input, unsigned char * r, unsigned char * s) {

	if (memcmp(ASNDSAProlog, input, 4) != 0 ||
		memcmp(ASNDSAMiddle, &input[24], 2) != 0)

		return false;

	memcpy(r, &input[4], 20);
	memcpy(s, &input[26], 20);

	return true;

}


// --------------------------------------------------------------------------------
//           Calculate correct OIDs for an RSA sig
// --------------------------------------------------------------------------------

/* As per RSA's PKCS #1 v 2.1, Section 9.2 Note 1, the DER encodings for
 * the has types are as follows:
 *
 * MD2: (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 02 05 00 04 10 || H.
 * MD5: (0x)30 20 30 0c 06 08 2a 86 48 86 f7 0d 02 05 05 00 04 10 || H.
 * SHA-1: (0x)30 21 30 09 06 05 2b 0e 03 02 1a 05 00 04 14 || H.
 * SHA-256: (0x)30 31 30 0d 06 09 60 86 48 01 65 03 04 02 01 05 00 04 20 || H.
 * SHA-384: (0x)30 41 30 0d 06 09 60 86 48 01 65 03 04 02 02 05 00 04 30 || H.
 * SHA-512: (0x)30 51 30 0d 06 09 60 86 48 01 65 03 04 02 03 05 00 04 40 || H.
 *
 * More recently the following has been provided for SHA-224
 *
 * SHA-224: 30 2d 30 0d 06 09 60 86 48 01 65 03 04 02 04 05 00 04 1c
 *
 */

int MD5OIDLen = 18;
unsigned char MD5OID[] = {
	0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86,
	0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05, 0x05, 0x00,
	0x04, 0x10
};


int sha1OIDLen = 15;
unsigned char sha1OID[] = {
	0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E,
	0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14
};

int sha224OIDLen = 19;
unsigned char sha224OID[] = {
	0x30, 0x2d, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
	0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04, 0x05,
	0x00, 0x04, 0x1c
};

int sha256OIDLen = 19;
unsigned char sha256OID[] = {
	0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
	0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05,
	0x00, 0x04, 0x20
};

int sha384OIDLen = 19;
unsigned char sha384OID[] = {
	0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
	0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05,
	0x00, 0x04, 0x30
};

int sha512OIDLen = 19;
unsigned char sha512OID[] = {
	0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86,
	0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05,
	0x00, 0x04, 0x40
};


unsigned char * getRSASigOID(hashMethod hm, int &oidLen) {

	switch (hm) {

	case (HASH_MD5):
		oidLen = MD5OIDLen;
		return MD5OID;
	case (HASH_SHA1):
		oidLen = sha1OIDLen;
		return sha1OID;
	case (HASH_SHA224):
		oidLen = sha224OIDLen;
		return sha224OID;
	case (HASH_SHA256):
		oidLen = sha256OIDLen;
		return sha256OID;
	case (HASH_SHA384):
		oidLen = sha384OIDLen;
		return sha384OID;
	case (HASH_SHA512):
		oidLen = sha512OIDLen;
		return sha512OID;
	default:
		oidLen = 0;
		return NULL;

	}
}



