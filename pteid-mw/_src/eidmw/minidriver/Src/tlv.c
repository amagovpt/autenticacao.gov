/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include "globmdrv.h"
#include "log.h"
#include "tlv.h"


#define EXPECT_TAG    0
#define EXPECT_LENGTH 1
#define EXPECT_VALUE  2

#define WHERE "TLVGetField"
/*
 * If pbData == NULL, then pdwDataLen will contain the required buffer size
 */
DWORD TLVGetField(PBYTE pbFile, DWORD dwFileLen, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, BYTE bField)
{
   DWORD    dwReturn       = 0;
	DWORD    dwIndex        = 0;
	BYTE     bFound         = 0;
	DWORD    dwLength;
	DWORD    dwValueIndex;
	BYTE     bExpect        = EXPECT_TAG;

   LogTrace(LOGTYPE_INFO, WHERE, "TLV Get Field: 0x%0x", bField);
	
	while (dwIndex < dwFileLen) 
	{
		if (bExpect == EXPECT_TAG) 
		{
			if (pbFile[dwIndex] == bField)
				bFound = 1;
			bExpect = EXPECT_LENGTH;  
			dwLength = 0;                              // reset length
		} 
		else if (bExpect == EXPECT_LENGTH) 
		{
			dwLength += pbFile[dwIndex];
			if (pbFile[dwIndex] != 0xFF) 
			{
				if (dwLength != 0)
					bExpect = EXPECT_VALUE;
				else 
					bExpect = EXPECT_TAG;
				if (bFound == 1) 
				{
					*pdwDataLen = dwLength;               // set returned length;
					if (pbData == NULL)                   // only buffer length is asked
						break;
					if (dwLength == 0)                    // no value
						break;
					if (dwLength > cbData)
						CLEANUP(ERROR_INSUFFICIENT_BUFFER);// cbData less then buffer to be returned
				}
			}
			dwValueIndex = 0;                           // reset valueindex
		}
		else if (bExpect == EXPECT_VALUE) 
		{
			if (bFound == 1 && pbData != NULL)
				pbData[dwValueIndex] = pbFile[dwIndex];  // fill data
			dwValueIndex++;
			if (dwValueIndex == dwLength) {
				bExpect = EXPECT_TAG;
				if (bFound == 1) 
					break;					                 // we are done
			}
		}
		dwIndex++;
	}
cleanup:
   return(dwReturn);
}
#undef WHERE