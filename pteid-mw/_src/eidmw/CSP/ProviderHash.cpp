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
#include "ProviderHash.h"
#include "csputil.h"

using namespace eIDMW;

CProviderHash::CProviderHash(ALG_ID Algid, tHashAlgo algo)
{
	m_Algid = Algid;
	m_oHash.Init(algo);
}

CProviderHash::CProviderHash(const CProviderHash & oProvHash)
{
	m_Algid = oProvHash.m_Algid;
	m_oHash = oProvHash.m_oHash;
	m_oValue = oProvHash.m_oValue;
}

ALG_ID CProviderHash::GetAlgid() const
{
	return m_Algid;
}

unsigned long CProviderHash::GetLengthBytes() const
{
	return m_oHash.GetHashLength(GetHashAlgo(m_Algid));
}

void CProviderHash::Update(const CByteArray &oData)
{
	m_oHash.Update(oData);
}

void CProviderHash::SetHashValue(const CByteArray &oHashValue)
{
	m_oValue = oHashValue;
}

CByteArray & CProviderHash::GetHashValue()
{
	if (m_oValue.Size() == 0)
		m_oValue = m_oHash.GetHash();

	return m_oValue;
}

CHash & CProviderHash::GetHashObject()
{
	return m_oHash;
}