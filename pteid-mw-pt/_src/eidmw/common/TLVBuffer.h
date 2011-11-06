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
// TLVBuffer.h: interface for the CTLVBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TLVBUFFER_H
#define TLVBUFFER_H

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <string.h>
#include <map>
#include "TLV.h"

namespace eIDMW
{

class CTLVBuffer  
{
public:
	EIDMW_CMN_API CTLVBuffer();
	EIDMW_CMN_API virtual ~CTLVBuffer();
    
	EIDMW_CMN_API void SetTagData(unsigned char ucTag, const unsigned char *pucData, unsigned long ulLen);
	EIDMW_CMN_API unsigned long Extract(unsigned char *pucData, unsigned long ulLen);
    EIDMW_CMN_API int ParseTLV(const unsigned char *pucData, unsigned long ulLen);
    EIDMW_CMN_API bool ParseFileTLV(const unsigned char *pucData, unsigned long ulLen);
    EIDMW_CMN_API CTLV *GetTagData(unsigned char ucTag);
    EIDMW_CMN_API void FillASCIIData(unsigned char ucTag, char *pData, unsigned long *pulLen);
    EIDMW_CMN_API void FillUTF8Data(unsigned char ucTag, char *pData, unsigned long *pulLen);
    EIDMW_CMN_API void FillBinaryStringData(unsigned char ucTag, char *pData, unsigned long *pulLen);
    EIDMW_CMN_API void FillLongData(unsigned char ucTag, long *piData);
    EIDMW_CMN_API void FillBinaryData(unsigned char ucTag, unsigned char *pData, unsigned long *pulLen);
    EIDMW_CMN_API bool FillBinaryDataCheck(unsigned char ucTag, unsigned char *pData, unsigned long *pulMaxLen);
    EIDMW_CMN_API unsigned long GetLengthNeeded();

private:
    static char *Hexify(unsigned char * pData, unsigned long ulLen);

    bool TlvEncodeLen(unsigned long ulLenVal, unsigned char *pucBufDest, int *piBufLen);
    bool TlvDecodeLen(const unsigned char *pucBufSrc, int *piBufLen,  unsigned long *pulLenVal);

    static char hexChars[];

    typedef std::map<unsigned char, CTLV *>  MapTLV; 
    typedef MapTLV::iterator                 ITMap;

	MapTLV m_oMapTLV;
};

}  // namespace eIDMW

#endif // TLVBUFFER_H
