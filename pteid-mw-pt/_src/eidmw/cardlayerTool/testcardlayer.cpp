/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#include "../cardlayer/CardLayer.h"
#include <stdio.h>
#include <memory>
#include <vector>

#define USE_OPENSSL

using namespace eIDMW;

extern CCardLayer g_oCardLayer;

static bool g_bVerifyFailed = false;

static long TestPteid(CReader & oReader, const std::string & csPin);

#define TEST_DATA_LEN   200

long Test(const std::string & csReaderName, const std::string & csPin)
{
	long lErrors = 0;
	bool bTestsDone = false;

	CReader &oReader = g_oCardLayer.getReader(csReaderName);

	if (oReader.Connect())
	{
		printf("Using card in reader \"%s\"\n", oReader.GetReaderName().c_str());
		bTestsDone = true;

		tCardType type = oReader.GetCardType();

		switch(type)
		{
		case CARD_PTEID:
			lErrors = TestPteid(oReader, csPin);
			break;
		default:
			printf("Unrecognized card type %d, no tests done\n", type);
			bTestsDone = false;
		}

		oReader.Disconnect();
	}
	else
		printf("  No card found in the reader, exiting\n");

	if (bTestsDone)
		printf("\nDone, %ld errors\n", lErrors);

	return lErrors;
}

////////////////////////////////////////////////////////////////////////////:

static inline void ERR_LOG(const char *msg, long & lErrors)
{
	printf(msg);
	lErrors++;
}

//////////////////////////

static long TestReadShortFile(CReader & oReader, const std::string & csPath)
{
	long lErrors = 0;

	printf("\nTesting CReader::ReadFile() for short files\n");

	try
	{
		CByteArray oFullData = oReader.ReadFile(csPath, 0, FULL_FILE);

		CByteArray oData1 = oReader.ReadFile(csPath, 2, FULL_FILE);
		if (oData1.Size() + 2 != oFullData.Size() ||
			memcmp(oData1.GetBytes(), oFullData.GetBytes() + 2, oData1.Size()) != 0)
		{
			ERR_LOG("ERR: CReader::ReadFile(offset != 0) failed\n", lErrors);
		}

		CByteArray oData2 = oReader.ReadFile(csPath, 2, oFullData.Size() - 3);
		if (oData2.Size() + 3 != oFullData.Size() ||
			memcmp(oData2.GetBytes(), oFullData.GetBytes() + 2, oData2.Size()) != 0)
		{
			ERR_LOG("ERR: CReader::ReadFile(length != FULL_FILE) failed\n", lErrors);
		}

		CByteArray oData3 = oReader.ReadFile(csPath, 0, oFullData.Size() + 1);
		if (!oData3.Equals(oFullData))
			ERR_LOG("ERR: CReader::ReadFile(length = too long) returned a wrong data\n", lErrors);

		bool bExceptionCaught = false;
		try
		{
			CByteArray oData4 = oReader.ReadFile(csPath, oFullData.Size() + 1, 1);
		}
		catch(CMWException & e) {
			bExceptionCaught = true;
			if ( (unsigned) e.GetError() != EIDMW_ERR_PARAM_RANGE)
			ERR_LOG("ERR: CReader::ReadFile(offset = too long) has thrown the wrong exception\n", lErrors);
		}
		if (!bExceptionCaught)
			ERR_LOG("ERR: CReader::ReadFile(offset = too long) didn't throw an exception\n", lErrors);

		CByteArray oData5 = oReader.ReadFile(csPath, 0, oFullData.Size());
		if (!oData5.Equals(oFullData))
			ERR_LOG("ERR: CReader::ReadFile(length = file size) failed\n", lErrors);
	}
	catch(const CMWException &e)
	{
		ERR_LOG("ERR: CReader::ReadFile() threw an MWException\n", lErrors);
		printf("   MWException code: 0x%0x\n", (unsigned) e.GetError());
	}
	catch (...)
	{
		ERR_LOG("ERR: CReader::ReadFile() threw an Exception\n", lErrors);
	}

	return lErrors;
}

//////////////////////////

static long TestReadLongFile(CReader & oReader, const std::string & csPath)
{
	long lErrors = 0;

	printf("\nTesting CReader::ReadFile() for long files\n");

	try
	{
		CByteArray oFullData = oReader.ReadFile(csPath, 0, FULL_FILE);

		CByteArray oData1 = oReader.ReadFile(csPath, 0, 252);
		if (oData1.Size() != 252 || memcmp(oFullData.GetBytes(), oData1.GetBytes(), 252) != 0)
			ERR_LOG("ERR: CReader::ReadFile(length != 252) failed\n", lErrors);

		CByteArray oData2 = oReader.ReadFile(csPath, 0, 253);
		if (oData2.Size() != 253 || memcmp(oFullData.GetBytes(), oData2.GetBytes(), 253) != 0)
			ERR_LOG("ERR: CReader::ReadFile(length != 253) failed\n", lErrors);

		CByteArray oData3 = oReader.ReadFile(csPath, 100, 400);
		if (oData3.Size() != 400 || memcmp(oFullData.GetBytes() + 100, oData3.GetBytes(), 400) != 0)
			ERR_LOG("ERR: CReader::ReadFile(offs=100, len = 400) failed\n", lErrors);
	}
	catch(const CMWException &e)
	{
		ERR_LOG("ERR: CReader::ReadFile() threw an MWException\n", lErrors);
		printf("   MWException code: 0x%0x\n", (unsigned)e.GetError());
	}
	catch (...)
	{
		ERR_LOG("ERR: CReader::ReadFile() threw an MWException\n", lErrors);
	}

	return lErrors;
}

//////////////////////////

/**
 * csPath should be the path to a big file that can be cached
 * Best do this test first, because anything is cached.
 */
static long TestCache(CReader & oReader, const std::string & csPath)
{
	long lErrors = 0;

	printf("\nTesting caching functionality (using file %s)\n", csPath.c_str());

	CByteArray oMaybeFromDisk = oReader.ReadFile(csPath);

	bool bDeleted = g_oCardLayer.DeleteFromCache(oReader.GetSerialNr());
	if (!bDeleted)
	{
		ERR_LOG("ERR: CCardLayer::DeleteFromCache() seems to fail\n", lErrors);
		printf("  (File %s should have been cached, but isn't deleted)\n", csPath.c_str());
	}

	CByteArray oFromCard = oReader.ReadFile(csPath);
	//diffFromCard = fromCard2 - fromCard1;

	CByteArray oFromMem = oReader.ReadFile(csPath);
	//diffFromMem = fromMem2 - fromMem1;

	/*
	if (2 * diffFromMem > diffFromCard)
	{
		printf("WARNING: reading %d bytes from the card (%d msec) isn't much\n",
			oFromCard.Size(), diffFromCard);
		printf("  faster then reading it from cach (%d msec)\n", diffFromMem);
	}
	*/

	if (!oFromCard.Equals(oFromMem))
		ERR_LOG("ERR: cached file contents (from memory) differ from those on the card\n", lErrors);
	else if (!oFromCard.Equals(oMaybeFromDisk))
		ERR_LOG("ERR: cached file contents (from disk) differ from those on the card\n", lErrors);

	oFromCard = oReader.ReadFile(csPath, 10, 20);
	oFromMem = oReader.ReadFile(csPath, 10, 20);
	if (!oFromCard.Equals(oFromMem))
		ERR_LOG("ERR: reading part of the cached contents fails\n", lErrors);

	return lErrors;
}

//////////////////////////

static const char * SignAlgo2String(unsigned long ulAlgo)
{
	switch(ulAlgo)
	{
		case SIGN_ALGO_RSA_RAW: return "RSA_RAW ";
		case SIGN_ALGO_RSA_PKCS: return "RSA_PKCS ";
		case SIGN_ALGO_MD5_RSA_PKCS: return "RSA_MD5_PKCS ";
		case SIGN_ALGO_SHA1_RSA_PKCS: return "RSA_SHA1_PKCS ";
		case SIGN_ALGO_SHA256_RSA_PKCS: return "RSA_SHA256_PKCS ";
		case SIGN_ALGO_SHA384_RSA_PKCS: return "RSA_SHA384_PKCS ";
		case SIGN_ALGO_SHA512_RSA_PKCS: return "RSA_SHA512_PKCS ";
		case SIGN_ALGO_RIPEMD160_RSA_PKCS: return "RSA_RIPEMD160_PKCS ";
		case SIGN_ALGO_SHA1_RSA_PSS: return "RSA_SHA_PSS ";
		default: return "??? ";
	}
}

#ifdef USE_OPENSSL

#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/rsa.h"
#include "openssl/err.h"
#include "openssl/opensslv.h"

static const EVP_MD * GetOpensslHash(unsigned long ulSignAlgo)
{
	switch(ulSignAlgo)
	{
	case SIGN_ALGO_MD5_RSA_PKCS: return EVP_md5();
	case SIGN_ALGO_RSA_PKCS: // Why use SHA1 here? -> see below
	case SIGN_ALGO_SHA1_RSA_PSS:
	case SIGN_ALGO_SHA1_RSA_PKCS: return EVP_sha1();
#if (OPENSSL_VERSION_NUMBER > 0x009070ffL)
	case SIGN_ALGO_SHA256_RSA_PKCS: return EVP_sha256();
	case SIGN_ALGO_SHA384_RSA_PKCS: return EVP_sha384();
	case SIGN_ALGO_SHA512_RSA_PKCS: return EVP_sha512();
	case SIGN_ALGO_RIPEMD160_RSA_PKCS: return EVP_ripemd160();
#endif
	default: return NULL;
	}
}

static bool bOpensslInitialized = false;

static bool VerifySignature(const CByteArray &oData, const CByteArray & oSignature,
	const CByteArray & oCert, unsigned long ulSignAlgo)
{
	if (!bOpensslInitialized)
	{
		OpenSSL_add_all_algorithms();
		bOpensslInitialized = true;
	}

	unsigned long ulCertLen = oCert.Size();
	unsigned char *pucCert = new unsigned char[ulCertLen];
	memcpy(pucCert, oCert.GetBytes(), ulCertLen);
	BIO *pBio = BIO_new_mem_buf(pucCert, ulCertLen);
	X509 *pCert = d2i_X509_bio(pBio, NULL);
	EVP_PKEY *pKey = X509_get_pubkey(pCert);

	EVP_MD_CTX mdCtx;
	const EVP_MD *pEvpMdAlgo = GetOpensslHash(ulSignAlgo);

	int res = 0;
	if (ulSignAlgo == SIGN_ALGO_SHA1_RSA_PSS)
	{
		// Can't use the EVP_xxx() functions for PSS
		CByteArray oHash = CHash().Hash(ALGO_SHA1, oData);
		RSA *rsa = (RSA *) pKey->pkey.rsa;
		unsigned char tucDecrypted[512];
		RSA_public_decrypt(oSignature.Size(), oSignature.GetBytes(), 
			tucDecrypted, rsa, RSA_NO_PADDING);
		res = RSA_verify_PKCS1_PSS(rsa, oHash.GetBytes(),
			pEvpMdAlgo, tucDecrypted, -2);
	}
	else
	{
		EVP_VerifyInit(&mdCtx, pEvpMdAlgo);
		
		EVP_VerifyUpdate(&mdCtx, oData.GetBytes(), oData.Size());

#if (OPENSSL_VERSION_NUMBER > 0x009070ffL)
		res = EVP_VerifyFinal(&mdCtx, oSignature.GetBytes(), oSignature.Size(), pKey);
#else
		unsigned char* signatureBytes = const_cast<unsigned char*> (oSignature.GetBytes());
		res = EVP_VerifyFinal(&mdCtx, signatureBytes, oSignature.Size(), pKey);
#endif
	
	}

	
	X509_free(pCert);
	
	BIO_free(pBio);
	
	free(pucCert);

	// Result res: 1 = signature OK, 0 = bad signature, other = other err
	if (res == 1)
		return true;
	else
	{
		if (res == 0)
			printf("     ERR: verification of algo %s failed\n", SignAlgo2String(ulSignAlgo));
		else
			printf("     ERR: openssl verification of algo %s returned 0x%0x (%d)\n",
				SignAlgo2String(ulSignAlgo), res, res);
		return false;
	}
}

#else

static bool VerifySignature(const CByteArray &oData, const CByteArray & oSignature,
	const CByteArray & oCert, unsigned long ulSignAlgo)
{
	printf("        WARNING: signature (%s) not verified\n", SignAlgo2String(ulSignAlgo));
	return true;
}

#endif  // USE_OPENSSL

long sign2hashAlgo(unsigned long ulSignAlgo)
{
	switch(ulSignAlgo)
	{
	case SIGN_ALGO_MD5_RSA_PKCS: return ALGO_MD5;
	case SIGN_ALGO_RSA_PKCS: // Why use SHA1 here? -> see below
	case SIGN_ALGO_SHA1_RSA_PSS:
	case SIGN_ALGO_SHA1_RSA_PKCS: return ALGO_SHA1;
	case SIGN_ALGO_SHA256_RSA_PKCS: return ALGO_SHA256;
	case SIGN_ALGO_SHA384_RSA_PKCS: return ALGO_SHA384;
	case SIGN_ALGO_SHA512_RSA_PKCS: return ALGO_SHA512;
	case SIGN_ALGO_RIPEMD160_RSA_PKCS: return ALGO_RIPEMD160;
	default: return -1;
	}
}

static long SignVerify(CReader & oReader, tPrivKey & key,
	const CByteArray & oCertData, unsigned long ulSignAlgo)
{
	CByteArray oData(1000);
	for (int i = 0; i < 300; i++)
		oData.Append((unsigned char) rand());

	long lHashAlgo = sign2hashAlgo(ulSignAlgo);

	if (lHashAlgo != -1)
	{
		CByteArray oSignature;
	
		CHash oHash;
		oHash.Init((tHashAlgo) lHashAlgo);
		oHash.Update(oData);

		if (ulSignAlgo == SIGN_ALGO_RSA_PKCS)
		{
			// To test SIGN_ALGO_RSA_PKCS, we take as input the SHA1 AID
			// plus the SHA1 hash of oData. This way, we can use OpenSSL's
			// SHA1 signature verification in VerifySignature().
			const unsigned char SHA1_AID[] = {0x30, 0x21, 0x30, 0x09,
				0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00,0x04, 0x14};
			CByteArray oTobeSigned(SHA1_AID, sizeof(SHA1_AID));
			oTobeSigned.Append(oHash.GetHash());
			oSignature = oReader.Sign(key, ulSignAlgo, oTobeSigned);
		}
		else
			oSignature = oReader.Sign(key, ulSignAlgo, oHash);

		bool bVerified = VerifySignature(oData, oSignature, oCertData, ulSignAlgo);

		return bVerified ? 0 : 1;
	}
	else
	{
		printf("      Signature algo %s can't be tested yet\n", SignAlgo2String(ulSignAlgo));
		return 0;
	}
}

static long TestSignaturesWithKey(CReader & oReader, tPrivKey & key, bool bFullTest)
{
	// Note tested for now: SIGN_ALGO_RSA_RAW
	unsigned long tulSignAlgos[] = {SIGN_ALGO_SHA1_RSA_PKCS, 
		SIGN_ALGO_MD5_RSA_PKCS,
#if (OPENSSL_VERSION_NUMBER > 0x009070ffL)
		SIGN_ALGO_SHA256_RSA_PKCS, 
		SIGN_ALGO_SHA384_RSA_PKCS, 
		SIGN_ALGO_SHA512_RSA_PKCS,
		SIGN_ALGO_RIPEMD160_RSA_PKCS,
#endif
		SIGN_ALGO_SHA1_RSA_PSS, 
		SIGN_ALGO_RSA_PKCS};

	long lErrors = 0;

	CByteArray oCertData;
	tCert cert = oReader.GetCertByID(key.ulID);
	if (cert.bValid)
		oCertData = oReader.ReadFile(cert.csPath);
	else
	  printf("  warning: no cert found for key \"%s\", can't verify the signatures!\n", key.csLabel.c_str() );

	unsigned long ulSupportedAlgos = oReader.GetSupportedAlgorithms();

	std::string csAlgos;
	int iAlgoCount = 0;
	// For each of the supported algos
	for (int i = 0; i < sizeof(tulSignAlgos) / sizeof(unsigned long); i++)
	{
		if (tulSignAlgos[i] & ulSupportedAlgos)
		{
			lErrors += SignVerify(oReader, key, oCertData, tulSignAlgos[i]);
			csAlgos += std::string(SignAlgo2String(tulSignAlgos[i]));
			iAlgoCount++;
			if (!bFullTest)
				break;
		}
	}
	if (iAlgoCount != 0)
	{
		printf("      used %d algo(s): %s\n", iAlgoCount, csAlgos.c_str());
		if (key.ulUserConsent != 0)
			printf("      (not all algos were used because each signature requires a PIN entry for this key)\n");
	}
	else
		printf("  ERR/warning: no signature algorithms are supported!\n");

	return lErrors;
}

static long TestSignatures(CReader & oReader, const std::string & csPin)
{
	long lErrors = 0;

	printf("\nTesting CReader::Sign()\n");

	if (g_bVerifyFailed)
	{
		printf("PIN verification has failed, skipping signature tests\n");
		return lErrors;
	}

	// For all keys
	unsigned long ulPrivKeys = oReader.PrivKeyCount();
	for (unsigned long i = 0; i < ulPrivKeys && !g_bVerifyFailed; i++)
	{
		tPrivKey key = oReader.GetPrivKey(i);
		printf("  - for key \"%s\"\n", key.csLabel.c_str());

		// Verify if PIN is specified and needed
		tPin pin = oReader.GetPinByID(key.ulAuthID);
		if (csPin != "" && pin.bValid && key.ulUserConsent == 0)
		{
			unsigned long ulRemaining;
			if (!oReader.PinCmd(PIN_OP_VERIFY, pin, csPin, "", ulRemaining))
			{
				printf("ERR: PIN verification failed, quiting signature tests\n");
				g_bVerifyFailed = true;
			}
		}

		if (!g_bVerifyFailed)
			lErrors += TestSignaturesWithKey(oReader, key, i == 0);
	}

	return lErrors;
}

//////////////////////////

static long TestRandom(CReader & oReader)
{
	long lErrors = 0;

	printf("\nTesting CReader::GetRandom()\n");

	try
	{
		for (unsigned long i = 0; i < 4; i++)
			CByteArray oData4 = oReader.GetRandom(5 * i);
	}
	catch(CMWException & e) {
		ERR_LOG("ERR: CReader::GetRandom() returned an exception\n", lErrors);
		e;
	}

	for (unsigned long i = 6; i < 10; i++)
	{
		if (oReader.GetRandom(i).Equals(oReader.GetRandom(i)))
			ERR_LOG("ERR: CReader::GetRandom() returned twice the same value\n", lErrors);
	}

	return lErrors;
}

//////////////////////////

static long TestSelectApplication(CReader & oReader, const std::vector <CByteArray> & vAIDs)
{
	long lErrors = 0;

	printf("\nTesting CReader::SelectApplication()\n");

	for (size_t i = 0; i < vAIDs.size(); i++)
	{
		try
		{
			oReader.SelectApplication(vAIDs[i]);
		}
		catch(const CMWException & e) 
		{
			ERR_LOG("ERR: CReader::SelectApplication() failed\n", lErrors);
			printf("  AID = %s, err = 0x%0x\n", vAIDs[i].ToString().c_str(), (unsigned) e.GetError());
		}
	}

	return lErrors;
}

//////////////////////////

static bool ComparePins(tPin p1, tPin p2)
{
	return p1.bValid == p2.bValid &&
	  p1.csLabel == p2.csLabel &&
	  p1.csPath == p2.csPath &&
	  p1.ulID == p2.ulID && 
	  p1.ulAuthID == p2.ulAuthID &&
	  p1.ulMinLen == p2.ulMinLen && 
	  p1.ulMaxLen == p2.ulMaxLen &&
	  p1.ulStoredLen == p2.ulStoredLen && 
	  p1.encoding == p2.encoding &&
	  p1.ulPinRef == p2.ulPinRef && 
	  p1.ulFlags == p2.ulFlags && 
	  p1.ucPadChar == p2.ucPadChar;
}

static bool ComparePrivKeys(tPrivKey p1, tPrivKey p2)
{
	return p1.bValid && p2.bValid &&
	  p1.csLabel == p2.csLabel && p1.csPath == p2.csPath &&
	  p1.ulID == p2.ulID && p1.ulAuthID == p2.ulAuthID &&
	  p1.ulKeyLenBytes == p2.ulKeyLenBytes &&
	  p1.ulUserConsent == p2.ulUserConsent && 
	  p1.bUsedInP11 == p2.bUsedInP11 &&
	  p1.ulKeyRef == p2.ulKeyRef &&
	  p1.ulKeyAccessFlags == p2.ulKeyAccessFlags &&
	  p1.ulKeyUsageFlags == p2.ulKeyUsageFlags;
}

static bool CompareCerts(tCert c1, tCert c2)
{
	return c1.bValid == c2.bValid &&
		c1.csLabel == c2.csLabel && c1.csPath == c2.csPath &&
		c1.ulID == c2.ulID && c1.ulAuthID == c2.ulAuthID;
}

////////////////////////////////////////////////////////////////////////////:

static long TestPteidP15(CReader & oReader, unsigned char ucVersion)
{
	long lErrors = 0;

	printf("\nTesting PKCS15 functions\n");

	unsigned long ulPinCount = ucVersion < 0x20 ? 1 : 2;
	if (oReader.PinCount() != ulPinCount)
	{
		ERR_LOG("ERR: CReader::PinCount() didn't return a wrong value\n", lErrors);
	}
	else
	{
		if (ucVersion < 0x20)
		{
		  const tPin PinPteidV1 = {true, "Basic PIN",3,0,0,1,48,NO_ID, 4, 8, 12, 1, 0xFF,PIN_ENC_GP,"","3F00"};	
			if (!ComparePins(oReader.GetPin(0), PinPteidV1))
				ERR_LOG("ERR: CReader::GetPin(0) returned incorrect PIN info\n", lErrors);
			if (!ComparePins(oReader.GetPinByID(1), PinPteidV1))
				ERR_LOG("ERR: CReader::GetPinByID(1) returned incorrect PIN info\n", lErrors);
		}
		else
		{
		  const tPin pinPteidAuthV2 = {true, "Authentication",0x3,0,0,1,0x30,0, 4, 8, 12, 0x85, 0xFF,PIN_ENC_GP,"","3F00DF00"};
		  const tPin pinPteidSignV2 = {true, "Signature",     0x3,0,0,2,0x30,0, 4, 8, 12, 0x86, 0xFF,PIN_ENC_GP,"","3F00DF00"};
			if (!ComparePins(oReader.GetPin(0), pinPteidAuthV2))
				ERR_LOG("ERR: CReader::GetPin(0) returned incorrect PIN info\n", lErrors);
			if (!ComparePins(oReader.GetPin(1), pinPteidSignV2))
				ERR_LOG("ERR: CReader::GetPin(1) returned incorrect PIN info\n", lErrors);
			if (!ComparePins(oReader.GetPinByID(1), pinPteidAuthV2))
				ERR_LOG("ERR: CReader::GetPinByID(1) returned incorrect PIN info\n", lErrors);
			if (!ComparePins(oReader.GetPinByID(2), pinPteidSignV2))
				ERR_LOG("ERR: CReader::GetPinByID(2) returned incorrect PIN info\n", lErrors);
		}
	}

	if (oReader.PrivKeyCount() != 2)
	{
		ERR_LOG("ERR: CReader::PrivKeyCount() didn't return 2\n", lErrors);
	}
	else
	  {
	    if (ucVersion < 0x20)
	      {
		  
		const tPrivKey KeyAuthPteidV1 = {true, "Authentication", 0x3,1,0,2,4,0x1D,0x82,"3F00DF00", 128,true};
		const tPrivKey KeySignPteidV1 = {true, "Signature", 0x3,1,1,3,0x200,0x1D,0x83,"3F00DF00", 128,true};
		if (!ComparePrivKeys(oReader.GetPrivKey(0), KeyAuthPteidV1))
		  ERR_LOG("ERR: CReader::GetPrivKey(0) returned incorrect key info\n", lErrors);
		if (!ComparePrivKeys(oReader.GetPrivKey(1), KeySignPteidV1))
		  ERR_LOG("ERR: CReader::GetPrivKey(1) returned incorrect key info\n", lErrors);
		if (!ComparePrivKeys(oReader.GetPrivKeyByID(2), KeyAuthPteidV1))
		  ERR_LOG("ERR: CReader::GetPrivKeyByID(2) returned incorrect key info\n", lErrors);
		if (!ComparePrivKeys(oReader.GetPrivKeyByID(3), KeySignPteidV1))
		  ERR_LOG("ERR: CReader::GetPrivKeyByID(3) returned incorrect key info\n", lErrors);
	      }
	    else
	      {  
		const tPrivKey KeyAuthPteidV2 = {true, "Authentication", 0x3,1,0,2,4,0x1D,0x8A,"3F00DF00", 128,true};
		const tPrivKey KeySignPteidV2 = {true, "Signature", 0x3,2,1,3,0x200,0x1D,0x89,"3F00DF00", 128,true};
		if (!ComparePrivKeys(oReader.GetPrivKey(0), KeyAuthPteidV2))
		  ERR_LOG("ERR: CReader::GetPrivKey(0) returned incorrect key info\n", lErrors);
		

		if (!ComparePrivKeys(oReader.GetPrivKey(1), KeySignPteidV2))
		  ERR_LOG("ERR: CReader::GetPrivKey(1) returned incorrect key info\n", lErrors);
		if (!ComparePrivKeys(oReader.GetPrivKeyByID(2), KeyAuthPteidV2))
		  ERR_LOG("ERR: CReader::GetPrivKeyByID(2) returned incorrect key info\n", lErrors);
		if (!ComparePrivKeys(oReader.GetPrivKeyByID(3), KeySignPteidV2))
		  ERR_LOG("ERR: CReader::GetPrivKeyByID(3) returned incorrect key info\n", lErrors);
	      }
	}

	if (oReader.CertCount() != 4)
	{
		ERR_LOG("ERR: CReader::CertCount() didn't return 4\n", lErrors);
	}
	else
	{
	  const tCert CertAuthPteid = {true,"Authentication",3,1,0,2,false,false,"3F00DF005038"};
	  const tCert CertSignPteid = {true,"Signature",3,1,0,3,false,false,"3F00DF005039"};
	  const tCert CertCaPteid = {true,"CA",3,1,0,4,true,false,"3F00DF00503A"};
	  const tCert CertRootPteid = {true,"Root",3,1,0,6,true,false,"3F00DF00503B"};
		if (!CompareCerts(oReader.GetCert(0), CertAuthPteid))
			ERR_LOG("ERR: CReader::GetCert(0) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCert(1), CertSignPteid))
			ERR_LOG("ERR: CReader::GetCert(1) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCert(2), CertCaPteid))
			ERR_LOG("ERR: CReader::GetCert(2) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCert(3), CertRootPteid))
			ERR_LOG("ERR: CReader::GetCert(3) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCertByID(2), CertAuthPteid))
			ERR_LOG("ERR: CReader::GetCertByID(2) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCertByID(3), CertSignPteid))
			ERR_LOG("ERR: CReader::GetCertByID(3) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCertByID(4), CertCaPteid))
			ERR_LOG("ERR: CReader::GetCertByID(4) returned incorrect cert info\n", lErrors);
		if (!CompareCerts(oReader.GetCertByID(6), CertRootPteid))
			ERR_LOG("ERR: CReader::GetCertByID(6) returned incorrect cert info\n", lErrors);
	}

	return lErrors;
}

static long TestPteidSendApdu(CReader & oReader)
{
	long lErrors = 0;

	printf("\nTesting SendAPDU()\n");

	// Send a Get Card Data APDU
	unsigned char tucGetCardData[] = {0x80, 0xE4, 0x00, 0x00, 0x1C};
	CByteArray oGetCardData(tucGetCardData, sizeof(tucGetCardData));
	CByteArray oData = oReader.SendAPDU(oGetCardData);
	// Serial nr. are the first 16 bytes of the Get Card Data APDU
	std::string csSerialNr = oReader.GetSerialNr();
	if (csSerialNr != oData.ToString(false, true, 0, 16))
		ERR_LOG("SendAPDU(GetCardData) returns wrong data (bad serial nr.)\n", lErrors);

	// Send a Select MF APDU
	unsigned char tucSelectMF[] = {0x00, 0xa4, 0x02, 0x0C, 0x02, 0x3f, 0x00};
	CByteArray oSelectMF(tucSelectMF, sizeof(tucSelectMF));
	oData = oReader.SendAPDU(oSelectMF);
	// Result should be 90 00
	if (oData.Size() != 2 || oData.GetByte(0) != 0x90 || oData.GetByte(1) != 0x00)
	{
		ERR_LOG("SendAPDU(GetCardData) returns wrong data (bad serial nr.)\n", lErrors);
		printf("    (result = %s\n", oData.ToString().c_str());
	}

	return lErrors;
}

static long TestPteidPinStatus(CReader & oReader, unsigned char ucVersion)
{
	long lErrors = 0;

	printf("\nTesting PinStatus()\n");

	tPin pin = oReader.GetPin(0);
	unsigned long ulTriesLeft = oReader.PinStatus(pin);

	if (ucVersion < 0x20)
	{
		if (ulTriesLeft != PIN_STATUS_UNKNOWN)
		{
			ERR_LOG("PinStatus() for a V1 card doesn't return PIN_STATUS_UNKNOWN\n", lErrors);
			printf("    (remaining attempts = %ld)\n", ulTriesLeft);
		}
	}
	else
	{
		if (ulTriesLeft > 3)
		{
			ERR_LOG("PinStatus() for a V2 card doesn't return 0, 1, 2 or 3\n", lErrors);
			printf("    (remaining attempts = %ld)\n", ulTriesLeft);
		}
		else
		if (ulTriesLeft != 3)
		  printf("    (warning: remaining tries for the \"%s\" PIN: %ld)\n", pin.csLabel.c_str(), ulTriesLeft);
	}

	return lErrors;
}

static long TestPteidCtrl(CReader & oReader, unsigned char ucVersion)
{
	long lErrors = 0;

	printf("\nTesting Ctrl()\n");

	// CTRL_PTEID_GETCARDDATA
	CByteArray oCardInfo = oReader.GetInfo();
	CByteArray oUnsignedCardData = oReader.Ctrl(CTRL_PTEID_GETCARDDATA, CByteArray());
	if (!oCardInfo.Equals(oUnsignedCardData))
		ERR_LOG("ERR: Ctrl(CTRL_PTEID_GETCARDDATA) != GetInfo()\n", lErrors);

	// CTRL_PTEID_GETSIGNEDCARDDATA
	if (ucVersion >= 0x20)
	{
		CByteArray oSignedCardData = oReader.Ctrl(CTRL_PTEID_GETSIGNEDCARDDATA, CByteArray());

		if (oSignedCardData.Size() != oUnsignedCardData.Size() + 128)
			ERR_LOG("ERR: signed and unsigned card data should differ in size by 128 bytes\n", lErrors);

		oSignedCardData.Chop(128);
		if (!oUnsignedCardData.Equals(oSignedCardData))
			ERR_LOG("ERR: start of unsigned card data should be the same as for unsigned card data\n", lErrors);
	}

	// CTRL_PTEID_GETSIGNEDPINSTATUS
	if (ucVersion >= 0x20)
	{
		tPin pin = oReader.GetPin(0);

		CByteArray oData(1);
		oData.Append((unsigned char) pin.ulPinRef);
		CByteArray oSignedPinStatus = oReader.Ctrl(CTRL_PTEID_GETSIGNEDPINSTATUS, oData);

		if (oSignedPinStatus.Size() != 129)
			ERR_LOG("ERR: signed pin status response should be (1 + 128) bytes\n", lErrors);
		else if (oSignedPinStatus.GetByte(0) != oReader.PinStatus(pin))
			ERR_LOG("ERR: signed pin status differs from unsigned PIN status\n", lErrors);
	}

	// CTRL_PTEID_INTERNAL_AUTH
	CByteArray oData(21); // Key ref (1 byte) + challenge(20 bytes)
	oData.Append(0x81);
	for (int i = 0; i < 20; i++)
		oData.Append((unsigned char) rand());
	CByteArray oResp1 = oReader.Ctrl(CTRL_PTEID_INTERNAL_AUTH, oData);
	if (oResp1.Size() != 128)
		ERR_LOG("ERR: Internal Auth. didn't return 128 bytes\n", lErrors);
	CByteArray oResp2 = oReader.Ctrl(CTRL_PTEID_INTERNAL_AUTH, oData);
	if (!oResp1.Equals(oResp2))
		ERR_LOG("ERR: Internal Auth. on the same data returns a different result\n", lErrors);
	oData.SetByte(oData.GetByte(5) + 0x01, 5);
	CByteArray oResp3 = oReader.Ctrl(CTRL_PTEID_INTERNAL_AUTH, oData);
	if (oResp1.Equals(oResp3))
		ERR_LOG("ERR: Internal Auth. on the different data returns the same result\n", lErrors);

	return lErrors;
}

static long TestPteid(CReader & oReader, const std::string & csPin)
{
	long lErrors = 0;

	CByteArray oCardData = oReader.GetInfo();
	unsigned char ucVersion = oCardData.GetByte(21);

	printf("Card: Belgian eID V%d.%d\n", ucVersion / 16, ucVersion % 16);

	lErrors += TestPteidSendApdu(oReader);

	lErrors += TestPteidCtrl(oReader, ucVersion);

	lErrors += TestCache(oReader, "3F00DF014035");
	lErrors += TestReadShortFile(oReader, "3F002F00");

	lErrors += TestReadLongFile(oReader, "3F00DF005038");

	std::vector <CByteArray> vAIDs;
	vAIDs.push_back(CByteArray("A000000177504B43532D3135", true));
	if (ucVersion >= 0x20)
		vAIDs.push_back(CByteArray("A000000177496446696C6573", true));
	lErrors += TestSelectApplication(oReader, vAIDs);

	lErrors += TestRandom(oReader);

	lErrors += TestPteidP15(oReader, ucVersion);

	lErrors += TestPteidPinStatus(oReader, ucVersion);

	lErrors += TestSignatures(oReader, csPin);

	return lErrors;
}
