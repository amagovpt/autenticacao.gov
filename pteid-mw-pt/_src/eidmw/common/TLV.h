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
// TLV.h: interface for the CTLV class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TLV_H
#define TLV_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ByteArray.h"

namespace eIDMW
{

class CTLV  
{
public:
	EIDMW_CMN_API CTLV();
	EIDMW_CMN_API CTLV(unsigned char ucTag, const unsigned char *pucData, unsigned long ulLen);
	EIDMW_CMN_API CTLV(const CTLV & oTlv);

	EIDMW_CMN_API virtual ~CTLV();

    EIDMW_CMN_API void SetTag(unsigned char ucTag) { m_ucTag = ucTag; }
    EIDMW_CMN_API void SetData(const CByteArray data) { m_Data.Append(data); }
    EIDMW_CMN_API void SetData(const unsigned char *pucData, unsigned long ulLen) { m_Data.Append(pucData, ulLen); }
    EIDMW_CMN_API void ReplaceData(const unsigned char *pucData, unsigned long ulLen) { m_Data.ClearContents();SetData(pucData, ulLen); }
    EIDMW_CMN_API unsigned char GetTag() { return m_ucTag; }
    EIDMW_CMN_API unsigned char *GetData() { return m_Data.GetBytes(); }
    EIDMW_CMN_API unsigned long GetLength() { return m_Data.Size(); }

private:
    unsigned char   m_ucTag;
    CByteArray      m_Data;
};

}  // namespace eIDMW

#endif // TLV_H
