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
#include "Hash.h"
#include "MWException.h"
#include "eidErrors.h"

using namespace eIDMW;

CHash::CHash()
{
	m_bInitialized = false;
}

unsigned long CHash::GetHashLength(tHashAlgo algo)
{
	switch(algo)
	{
	case ALGO_MD5:
		return 16;
	case ALGO_SHA1:
		return 20;
	case ALGO_MD5_SHA1:
		return 36;
	case ALGO_SHA256:
		return 32;
	case ALGO_SHA384:
		return 48;
	case ALGO_SHA512:
		return 64;
	case ALGO_RIPEMD160:
		return 20;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

}

CByteArray CHash::Hash(tHashAlgo algo, const CByteArray & data)
{
	return Hash(algo, data, 0, data.Size());
}

CByteArray CHash::Hash(tHashAlgo algo, const CByteArray & data,
	unsigned long ulOffset, unsigned long ulLen)
{
	Init(algo);
	Update(data, ulOffset, ulLen);
	return GetHash();
}

void CHash::Init(tHashAlgo algo)
{
	switch(algo)
	{
	case ALGO_MD5:
		md5_init(&m_md1);
		break;
	case ALGO_SHA1:
		sha1_init(&m_md1);
		break;
	case ALGO_MD5_SHA1:
		md5_init(&m_md1);
		sha1_init(&m_md2);
		break;
	case ALGO_SHA256:
		sha256_init(&m_md1);
		break;
	case ALGO_SHA384:
		sha384_init(&m_md1);
		break;
	case ALGO_SHA512:
		sha512_init(&m_md1);
		break;
	case ALGO_RIPEMD160:
		rmd160_init(&m_md1);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	m_Algo = algo;
	m_bInitialized = true;
}

void CHash::Update(const CByteArray & data)
{
	Update(data, 0, data.Size());
}

void CHash::Update(const CByteArray & data, unsigned long ulOffset, unsigned long ulLen)
{
	if (!m_bInitialized)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	if (ulLen != 0)
	{
		const unsigned char *pucData = data.GetBytes() + ulOffset;

		switch(m_Algo)
		{
		case ALGO_MD5:
			md5_process(&m_md1, pucData, ulLen);
			break;
		case ALGO_SHA1:
			sha1_process(&m_md1, pucData, ulLen);
			break;
		case ALGO_MD5_SHA1:
			md5_process(&m_md1, pucData, ulLen);
			sha1_process(&m_md2, pucData, ulLen);
			break;
		case ALGO_SHA256:
			sha256_process(&m_md1, pucData, ulLen);
			break;
		case ALGO_SHA384:
			sha384_process(&m_md1, pucData, ulLen);
			break;
		case ALGO_SHA512:
			sha512_process(&m_md1, pucData, ulLen);
			break;
		case ALGO_RIPEMD160:
			rmd160_process(&m_md1, pucData, ulLen);
			break;
		default:
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		}
	}
}

CByteArray CHash::GetHash()
{
	if (!m_bInitialized)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	m_bInitialized = false;

	// hash result
	unsigned char tucHash[64]; // make sure this is enough if other hashes are added!!!

	switch(m_Algo)
	{
	case ALGO_MD5:
		md5_done(&m_md1, tucHash);
		break;
	case ALGO_SHA1:
		sha1_done(&m_md1, tucHash);
		break;
	case ALGO_MD5_SHA1:
		md5_done(&m_md1, tucHash);
		sha1_done(&m_md2, tucHash + 16);
		break;
	case ALGO_SHA256:
		sha256_done(&m_md1, tucHash);
		break;
	case ALGO_SHA384:
		sha384_done(&m_md1, tucHash);
		break;
	case ALGO_SHA512:
		sha512_done(&m_md1, tucHash);
		break;
	case ALGO_RIPEMD160:
		rmd160_done(&m_md1, tucHash);
		break;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	return CByteArray(tucHash, GetHashLength(m_Algo));
}
