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
#include <exception>

#include "ByteArrayReader.h"
#include "ByteArray.h"

const static unsigned long EXTRA_INCREMENT_LEN = 10;

/***************** ByteArray **************************/

namespace eIDMW
{

// CByteArrayReader::CByteArrayReader()
CByteArrayReader::CByteArrayReader(CByteArray *inByteArray)
: m_ByteArray(inByteArray), m_ulIndex(0)
{
}

// CByteArrayReader::GetByte()
unsigned char CByteArrayReader::GetByte()  throw(CMWException)
{
  unsigned char theByte = m_ByteArray->GetByte(m_ulIndex);
	m_ulIndex += sizeof(char);
    return theByte;
}

// CByteArrayReader::GetLong()
unsigned long CByteArrayReader::GetLong()  throw(CMWException)
{
	unsigned long theLong = m_ByteArray->GetLong(m_ulIndex);
	m_ulIndex += sizeof(long);
    return theLong;
}

// CByteArrayReader::GetString()
std::string CByteArrayReader::GetString() throw(CMWException)
{
	std::string theString;
	unsigned long theLength = GetLong();
	while (theLength-- > 0)
	{
		unsigned char nextChar = GetByte();
		theString += nextChar;
	}
	return theString;
}

// unsigned char *CByteArrayReader::GetBytes()
unsigned char *CByteArrayReader::GetBytes()
{
	unsigned char *ptr = m_ByteArray->GetBytes();
	ptr += m_ulIndex;
    return ptr;
}

// const unsigned char *CByteArrayReader::GetBytes() const
const unsigned char *CByteArrayReader::GetBytes() const
{
	unsigned char *ptr = m_ByteArray->GetBytes();
	ptr += m_ulIndex;
    return ptr;
}

}
