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
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>

using namespace std;

#include "ByteArray.h"
#include "Util.h"
#include "MWException.h"
#include "eidErrors.h"

const static unsigned long EXTRA_INCREMENT_LEN = 10;

/***************** ByteArray **************************/

namespace eIDMW 
{

CByteArray::CByteArray(unsigned long ulCapacity)
: m_pucData(NULL), m_ulSize(0), m_ulCapacity(ulCapacity), m_bMallocError(false)
{
}

//copy mem into object
CByteArray::CByteArray(const unsigned char * pucData, unsigned long ulSize, unsigned long ulCapacity)
{
    MakeArray(pucData, ulSize, ulCapacity);
}

//copy object into new object
CByteArray::CByteArray(const CByteArray & oByteArray)
{
    MakeArray(oByteArray.GetBytes(), oByteArray.Size());
}

//assign data to object
//?! put move logic in makeArray
CByteArray & CByteArray::operator = (const CByteArray &oByteArray)
{
    if (&oByteArray != this)	//only action needed if both are not the same object
    {
        if (m_pucData == NULL)
            MakeArray(oByteArray.GetBytes(), oByteArray.Size());	//create new array with new data if nothing exist yet
        else if (m_ulCapacity >= oByteArray.Size())
        {
            m_ulSize = oByteArray.Size();							//array large enough; copy new data in existing array 
            memcpy(m_pucData, oByteArray.GetBytes(), m_ulSize);
            m_bMallocError = false;
        }
        else {
            m_ulCapacity = oByteArray.Size();						//array too small, create new one
			free(m_pucData);										//ip 13/08/07
            if (m_ulCapacity == 0)
                m_ulCapacity = EXTRA_INCREMENT_LEN;
            m_pucData = static_cast<unsigned char *>(malloc(m_ulCapacity));
            if (m_pucData == NULL)
                m_bMallocError = true;
            else
            {
                m_ulSize = m_ulCapacity;
                memcpy(m_pucData, oByteArray.GetBytes(), m_ulSize);
                m_bMallocError = false;
            }
        }
    }

    return *this;
}

static inline bool IsHexDigit(char c)
{
    return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

static inline unsigned char Hex2Byte(char cHex)
{
    if (cHex >= '0' && cHex <= '9')
        return (unsigned char) (cHex - '0');
    if (cHex >= 'A' && cHex <= 'F')
        return (unsigned char) (cHex - 'A' + 10);
    return (unsigned char) (cHex - 'a' + 10);
}

CByteArray::CByteArray(const std::string & csData, bool bIsHex)
{
	if (!bIsHex)
	{
		const unsigned char *data = reinterpret_cast<const unsigned char *>(csData.c_str()); 
		MakeArray(data, static_cast<unsigned int>(csData.length()));
	}
	else
	{
		const char *csHexData = csData.c_str();
		unsigned long ulHexLen = (int) csData.size();
		m_ulCapacity = ulHexLen / 2;
		MakeArray(NULL, 0, m_ulCapacity);
		if (!m_bMallocError)
		{
			unsigned char uc = 0;
			bool bSecondHexDigit = true;
			for (unsigned long i = 0; i < ulHexLen; i++)
			{
				if (IsHexDigit(csHexData[i]))
				{
					uc = 16 * uc + Hex2Byte(csHexData[i]);
					bSecondHexDigit = !bSecondHexDigit;
					if (bSecondHexDigit)
						m_pucData[m_ulSize++] = uc;
				}
			}
		}
	}
}

// CByteArray::~CByteArray()
CByteArray::~CByteArray()
{
    if (m_pucData)
        free(m_pucData);
}

unsigned long CByteArray::Size() const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    return m_ulSize;
}

unsigned char CByteArray::GetByte(unsigned long ulIndex) const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (ulIndex >= m_ulSize)
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 

    return m_pucData[ulIndex];
}

unsigned long CByteArray::GetLong(unsigned long ulIndex) const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (ulIndex + sizeof(unsigned long) > m_ulSize)
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 

	unsigned long ulLong = 0;

	for (unsigned int i = 0; i < sizeof(unsigned long); i++)
		ulLong = (ulLong * 256) + m_pucData[ulIndex++];

	return ulLong;
}

void CByteArray::SetByte(unsigned char ucByte, unsigned long ulIndex)
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (ulIndex >= m_ulSize)
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 

    m_pucData[ulIndex] = ucByte;
}

// unsigned char *CByteArray::GetBytes()
unsigned char *CByteArray::GetBytes()
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    return m_pucData;
}

// const unsigned char *CByteArray::GetBytes() const
const unsigned char *CByteArray::GetBytes() const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	return m_ulSize == 0 ? NULL : m_pucData;
}

CByteArray CByteArray::GetBytes(unsigned long ulOffset, unsigned long ulLen) const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (ulOffset >= m_ulSize)
        throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 

    if (ulLen == 0xFFFFFFFF || ulOffset + ulLen > m_ulSize)
        ulLen = m_ulSize - ulOffset;

    return CByteArray(&m_pucData[ulOffset],ulLen);
}

void CByteArray::Append(unsigned char ucByte)
{
    Append(&ucByte, sizeof(char));
}

//Other formalism for the Append(unsigned char ucByte)
CByteArray & CByteArray::operator += (const unsigned char ucByte)
{
	Append(ucByte);
	return *this;
}

void CByteArray::AppendLong(unsigned long ulLong)
{
	unsigned char tucLong[sizeof(unsigned long)];
	for (int i = sizeof(unsigned long) - 1; i >= 0; --i)
	{
		tucLong[i] = static_cast<unsigned char> (ulLong % 256);
		ulLong /= 256;
	}

	Append(tucLong, sizeof(tucLong));
}

void CByteArray::Append(const CByteArray & oByteArray)
{
    Append(oByteArray.GetBytes(), oByteArray.Size());
}
//Other formalism for the Append(const CByteArray & oByteArray)
CByteArray & CByteArray::operator += (const CByteArray & oByteArray)
{
	Append(oByteArray);
	return *this;
}

void CByteArray::Append(const unsigned char * pucData, unsigned long ulSize)
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (pucData != NULL && ulSize != 0)		//add only if object exist and is not empty, else ??
    {
        if (m_ulSize + ulSize > m_ulCapacity || m_pucData == NULL)
        {
            m_ulCapacity = m_ulSize + ulSize + EXTRA_INCREMENT_LEN;
            m_pucData = static_cast<unsigned char *>(realloc(m_pucData, m_ulCapacity));
            if (m_pucData == NULL)
            {
                m_bMallocError = true;
				throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
            }
        }

        memcpy(m_pucData + m_ulSize, pucData, ulSize);
        m_ulSize += ulSize;
    }
}

// void CByteArray::Append()
void CByteArray::Append(const std::string scData)
{
	Append((const unsigned char *)(scData.c_str()), static_cast<unsigned long>(scData.length()));
}
//Other formalism for the Append(const std::string scData)
CByteArray & CByteArray::operator += (const std::string scData)
{
	Append(scData);
	return *this;
}

// void CByteArray::Append()
void CByteArray::AppendString(const std::string scData)
{
	unsigned int length = static_cast<unsigned long>(scData.length());
	this->AppendLong(length);
	this->Append(scData);
}

// void CByteArray::Chop()
void CByteArray::Chop(unsigned long ulSize)
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	if (ulSize <= m_ulSize)
		m_ulSize -= ulSize;
	else
		m_ulSize = 0;
}

// void CByteArray::TrimRight
void CByteArray::TrimRight(unsigned char ucByte)
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	while (m_ulSize>0 && m_pucData[m_ulSize-1]==ucByte)
    {
        m_ulSize--;
    }
}

// void CByteArray::ClearContents()
void CByteArray::ClearContents()
{
    if (m_pucData)
    {
        free(m_pucData);
        m_pucData = NULL;
    }
    m_ulSize = 0;
    m_ulCapacity = 0;
}

bool CByteArray::Equals(const CByteArray & oByteArray) const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (m_ulSize == 0 && oByteArray.Size() == 0)
        return true;

    return m_ulSize == oByteArray.Size() &&
        memcmp(m_pucData, oByteArray.GetBytes(), m_ulSize) == 0;
}

static inline char ToHex(unsigned int uc)
{
    return static_cast<char>(uc <= 9 ? '0' + uc : 'A' - 10 + uc);
}

static inline void AddHexChar(char *csBuf, unsigned long & ulOffset, bool bAddSpace, unsigned char uc)
{
    csBuf[ulOffset++] = ToHex(uc / 16);
    csBuf[ulOffset++] = ToHex(uc % 16);
    if (bAddSpace)
        csBuf[ulOffset++] = ' ';
}

std::wstring CByteArray::ToWString(bool bAddSpace, bool bOneLine,
                                 unsigned long ulOffset, unsigned long ulLen) const
{
	return utilStringWiden(ToString(bAddSpace,bOneLine,ulOffset,ulLen));
}

std::string CByteArray::ToString(bool bAddSpace, bool bOneLine,
                                 unsigned long ulOffset, unsigned long ulLen) const
{
    if (m_bMallocError)
        throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

    if (ulLen == 0xFFFFFFFF || ulOffset + ulLen > m_ulSize)
        ulLen = m_ulSize - ulOffset;

    if (ulOffset >= m_ulSize || ulLen == 0)
        return std::string("");

    char csBuf[3 * 16 + 10];
    unsigned long ulBufOffset = 0;
    unsigned long i;

    unsigned char *pTmp = m_pucData + ulOffset;

    if (bOneLine)
    {
        unsigned long ulEnd = (ulLen >= 10 ? 10 : ulLen);

        for (i = 0; i < ulEnd; i++)
            AddHexChar(csBuf, ulBufOffset, bAddSpace, pTmp[i]);
        if (ulLen > 16)
        {
            csBuf[ulBufOffset++] = '.';
            csBuf[ulBufOffset++] = '.';
            if (bAddSpace)
                csBuf[ulBufOffset++] = ' ';
            i = ulLen - 5;
        }
        for ( ; i < ulLen; i++)
            AddHexChar(csBuf, ulBufOffset, bAddSpace, pTmp[i]);
        if (bAddSpace)
            csBuf[ulBufOffset - 1] = '\0';
        else
            csBuf[ulBufOffset] = '\0';

        return std::string(csBuf);
    }
    else
    {
        std::string csRet("\n");

        for (i = 0; i < ulLen; i+= 16)
        {
            unsigned long ulEnd = ulLen - i > 16 ? 16 : ulLen - i;
            ulBufOffset = 0;
            csBuf[ulBufOffset++] = '\t';
            for (unsigned long j = 0; j < ulEnd; j++)
                AddHexChar(csBuf, ulBufOffset, bAddSpace, pTmp[j]);
            pTmp += ulEnd;
            csBuf[ulBufOffset++] = '\n';
            csBuf[ulBufOffset] = '\0';
            csRet.append(csBuf);
        }

        return csRet;
    }
}
//copy supplied memory into new allocated memory
//?? capacity is not in steps of 10, but takes new data-len
void CByteArray::MakeArray(const unsigned char * pucData,		//returns allocated memory
						   unsigned long ulSize,				
						   unsigned long ulCapacity)
{
    m_ulCapacity	= ulCapacity < ulSize ? ulSize : ulCapacity;	//take largest value of both: available memory
    m_ulSize		= ulSize;										//effictively used memory
    m_bMallocError	= false;

    if (m_ulCapacity == 0)
        m_ulCapacity = EXTRA_INCREMENT_LEN;
    m_pucData = static_cast<unsigned char *>(malloc(m_ulCapacity));
    if (m_pucData == NULL)
    {
        m_ulSize = 0;
        m_bMallocError = true;
    }
    else if (pucData != NULL)
	{
        memcpy(m_pucData, pucData, m_ulSize);
	}
}

// CByteArray::HideNewLineCharsA()
void CByteArray::HideNewLineCharsA()
{

	unsigned long noOfSpecials = 0;
	for (unsigned long i = 0; i < m_ulSize; ++i)
	{
		if ((m_pucData[i] == 0x0a) || (m_pucData[i] == 0x27))
		{
			noOfSpecials += 1;
		}
	}

	if (noOfSpecials == 0) return;

	unsigned long theNewSize = m_ulSize + noOfSpecials;
	if (theNewSize > m_ulCapacity)
	{
		m_pucData = static_cast<unsigned char *>(realloc(m_pucData, theNewSize));
	}

	unsigned char *src = m_pucData + m_ulSize;
	unsigned char *dst = m_pucData + theNewSize;
	for (unsigned long i = 0; i < m_ulSize; ++i)
	{
		*--dst = *--src;
		if ((*dst == 0x0a) || (*dst == 0x27))
		{
			*dst += 0x10;
			*--dst = 0x27;
		}
	}

	m_ulSize = theNewSize;

}

// CByteArray::RevealNewLineCharsA()
void CByteArray::RevealNewLineCharsA()
{

	unsigned char *src = m_pucData;
	unsigned char *dst = m_pucData;
	unsigned long noOfSpecials = 0;
	for (unsigned long i = 0; i < m_ulSize; ++i)
	{
		if (*src == 0x27)
		{
			src += 1;
			*src -= 0x10;
			noOfSpecials += 1;
		}
		*dst++ = *src++;
	}
	m_ulSize -= noOfSpecials;

}

void CByteArray::Replace(unsigned char ucByteSrc,unsigned char ucByteDest)
{

	unsigned char *src = m_pucData;
	for (unsigned long i = 0; i < m_ulSize; ++i)
	{
		if(*src == ucByteSrc)
			*src = ucByteDest;
		src++;
	}
}

} // namespace eidMW

