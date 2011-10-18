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
#include "EmulationPkiCard.h"
#include "bignum/rsa.h"

using namespace eIDMW;

static const unsigned char MD5_AID[] = {
	0x30, 0x20,
		0x30, 0x0c,
			0x06, 0x08, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05,
			0x05, 0x00,
		0x04, 0x10
};
static const unsigned char SHA1_AID[] = {
	0x30, 0x21,
		0x30, 0x09,
			0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
		0x05, 0x00,
	0x04, 0x14
};
static const unsigned char SHA256_AID[] = {
	0x30, 0x31,
		0x30, 0x0d,
			0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
		0x05, 0x00,
	0x04, 0x20
};

CEmuationPkiCard::CEmuationPkiCard()
{
	m_iKeyCount = 0;
}

void CEmuationPkiCard::PutRsaKey(const CByteArray & oPrivExp, const CByteArray & oMod,
	const std::string &csPath, unsigned long ulRef,
	unsigned long ulPinRef, bool bUserConsent,
	unsigned long ulAlgos)
{
	tCardKey key;	

	key.oRsaExp = oPrivExp;
	key.oRsaMod = oMod;
	key.csPath = csPath;
	key.ulRef = ulRef;
	key.ulPinRef = ulPinRef;
	key.bUserConsent = bUserConsent;
	key.ulAlgos = ulAlgos;

	m_CardKeys[m_iKeyCount++] = key;
}

CByteArray CEmuationPkiCard::Sign(const CByteArray & oData,
	const tCardKey *pokey, unsigned long ulAlgo)
{
	if (!(ulAlgo & pokey->ulAlgos))
		return CByteArray(tucSW12WrongParams, sizeof(tucSW12WrongParams));

	CByteArray oIn;
	CByteArray oBadSize(tucSW12WrongLengthP3, sizeof(tucSW12WrongLengthP3));
	switch(ulAlgo)
	{
	case ALGO_RSA_PKCS1_MD5:
		if (oData.Size() != 16)
			return oBadSize;
		oIn.Append(MD5_AID, sizeof(MD5_AID));
		break;
	case ALGO_RSA_PKCS1_SHA1:
		if (oData.Size() != 20)
			return oBadSize;
		oIn.Append(SHA1_AID, sizeof(SHA1_AID));
		break;
	case ALGO_RSA_PKCS1_SHA256:
		if (oData.Size() != 32)
			return oBadSize;
		oIn.Append(SHA256_AID, sizeof(SHA256_AID));
		break;
	case ALGO_RSA_PKCS1:
		break;
	case ALGO_RSA_PSS_SHA1:
		// TODO: implement PSS
		return CByteArray(tucSW12FunctionNotSupported, sizeof(tucSW12FunctionNotSupported));
	}
	oIn.Append(oData);

	unsigned char tucSig[4096];
	int iSigLen = sizeof(tucSig);
	int iRes = eay_rsa_pkcs1_sign(oIn.GetBytes(), (int) oIn.Size(),
		pokey->oRsaExp.GetBytes(), (int) pokey->oRsaExp.Size(),
		pokey->oRsaMod.GetBytes(), (int) pokey->oRsaMod.Size(),
		tucSig, &iSigLen);
	if (iRes == 0)
	{
		CByteArray oSig(tucSig, iSigLen);
		oSig.Append(tucSW12OK, sizeof(tucSW12OK));
		return oSig;
	}
	else
		return CByteArray(tucSW12Unknown, sizeof(tucSW12Unknown));
}

tCardKey *CEmuationPkiCard::FindKey(unsigned long ulKeyRef)
{
	tCardKey *poCardKey = NULL;
	std::string csPath;

	for (int i = 0; i < m_iKeyCount; i++)
	{
		tCardKey * key = &m_CardKeys[i];
		if ((key->ulRef == ulKeyRef) && IsSubDir(m_csCurrentDF, key->csPath))
		{
			if (csPath == "" || IsSubDir(key->csPath, csPath))
			{
				poCardKey = key;
				csPath = key->csPath;
			}
		}
	}

	return poCardKey;
}
