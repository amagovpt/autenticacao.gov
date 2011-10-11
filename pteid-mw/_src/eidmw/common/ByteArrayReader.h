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
#pragma once

#include "Export.h"
#include "ByteArray.h"
#include "MWException.h"

#include <string>

namespace eIDMW
{
#ifdef WIN32
#pragma warning(disable:4290)			// Allow for 'throw()' specifications	
#endif

class CByteArrayReader 
{
public:
    EIDMW_CMN_API CByteArrayReader(CByteArray *inByteArray);

	EIDMW_CMN_API unsigned char GetByte(void) throw(CMWException);
	EIDMW_CMN_API unsigned long GetLong(void) throw(CMWException);

	EIDMW_CMN_API std::string GetString(void) throw(CMWException);

	EIDMW_CMN_API bool IsEmpty() {return m_ulIndex >= m_ByteArray->Size();} 

	/** If Size() == 0, then NULL is returned */
    EIDMW_CMN_API unsigned char *GetBytes();
    EIDMW_CMN_API const unsigned char *GetBytes() const;

private:
	CByteArray *m_ByteArray;
	unsigned long m_ulIndex;

};

}

