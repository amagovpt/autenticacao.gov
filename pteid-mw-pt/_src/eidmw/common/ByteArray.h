/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
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
#pragma once

#include "Export.h"

#include <string>

namespace eIDMW {

class EIDMW_CMN_API CByteArray {
public:
	CByteArray(unsigned long ulCapacity = 0);
	CByteArray(const unsigned char *pucData, unsigned long ulSize, unsigned long ulCapacity = 0);
	CByteArray(const CByteArray &oByteArray);
	CByteArray(const std::string &csData, bool bIsHex = false);
	~CByteArray();

	CByteArray &operator=(const CByteArray &oByteArray);

	unsigned long Size() const;

	unsigned char GetByte(unsigned long ulIndex) const;
	unsigned long GetLong(unsigned long ulIndex) const;
	void SetByte(unsigned char ucByte, unsigned long ulIndex);

	/** If Size() == 0, then NULL is returned */
	unsigned char *GetBytes();
	const unsigned char *GetBytes() const;
	/** Create a new CByteArray with part of this */
	CByteArray GetBytes(unsigned long ulOffset, unsigned long ulLen = 0xFFFFFFFF) const;

	void Append(unsigned char ucByte);
	CByteArray &operator+=(const unsigned char ucByte);
	void AppendLong(unsigned long ulLong);
	void Append(const CByteArray &oByteArray);
	CByteArray &operator+=(const CByteArray &oByteArray);
	void Append(const unsigned char *pucData, unsigned long ulSize);
	void SafeAppend(const unsigned char *pucData, size_t size);
	void Append(const std::string scData);
	CByteArray &operator+=(const std::string scData);
	void AppendString(const std::string scData);

	/** Remove the ulSize last bytes; if less bytes are present
	 * then everything is removed. */
	void Chop(unsigned long ulSize);

	/** Remove all the bytes at the end (right) of the array
	 *   if they are equal to ucByte
	 */
	void TrimRight(unsigned char ucByte = 0);

	/**
	 * Replace all ucByteSrc bytes by ucByteDest
	 */
	void Replace(unsigned char ucByteSrc, unsigned char ucByteDest);

	void ClearContents();

	bool Equals(const CByteArray &oByteArray) const;

	/** Returns a hex string, either on 1 line and truncated if needed
	 * (e.g. "A5 35 51 00 67 .. 2C 93") or in lines of 16 hex chars
	 * preceeded by a tab.
	 * A value of 0xFFFFFFFF for ulLen means: till the end of the bytearray. */
	std::wstring ToWString(bool bAddSpace = true, bool bOneLine = true, unsigned long ulOffset = 0,
						   unsigned long ulLen = 0xFFFFFFFF) const;
	std::string ToString(bool bAddSpace = true, bool bOneLine = true, unsigned long ulOffset = 0,
						 unsigned long ulLen = 0xFFFFFFFF) const;
	std::string hexToString() const;

private:
	void MakeArray(const unsigned char *pucData, unsigned long ulSize, unsigned long ulCapacity = 0);

	unsigned char *m_pucData;
	unsigned long m_ulSize;
	unsigned long m_ulCapacity;
	bool m_bMallocError;
};

} // namespace eIDMW
