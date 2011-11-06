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
// TLVBuffer.cpp: implementation of the CTLVBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "TLVBuffer.h"

namespace eIDMW
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char CTLVBuffer::hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

CTLVBuffer::CTLVBuffer()
{

}


CTLVBuffer::~CTLVBuffer()
{
    ITMap it;
    //free complete m_oMapTLV
    for (it = m_oMapTLV.begin(); it != m_oMapTLV.end(); ++it)
    {
        CTLV *pTemp = (*it).second;
        if(pTemp != NULL)
        {
            delete pTemp;
        }
    }
    m_oMapTLV.clear();
}

/** Unstream a multi-TLV-block into m_oMapTLV structure.
  User standard TLV syntax; extra length bytes when the MSB of a length-byte is set to 1.
  Each block should start with a 0-Tag, 0-tag can only be used as first TLV of a block.

 returns: false = invalid data
          true  = OK all done
*/
bool CTLVBuffer::ParseFileTLV(const unsigned char *pucData, unsigned long ulLen)
{
    bool bRet = false;
    
    if(pucData != NULL && ulLen > 0)
    {
        m_oMapTLV.clear();
        bRet = true;
        unsigned long ulIndex = 0;
        while ((ulIndex+1) < ulLen)      //at least 2 bytes; tag & length
        {
            //--- get & check TAG
            unsigned char ucTag      = pucData[ulIndex++];
            unsigned long ulFieldLen = 0;
            
            //the complete block should start with tag '0' and tag '0' may only be used as first TLV
            if((ucTag == 0x00 && ulIndex > 2) || (ucTag != 0x00 && ulIndex < 2))
            {
                bRet = false;
                break;
            }

            //--- get Length
//            int iNrBytes = ulLen - ulIndex - 1;     //calculate remaining bytes
            int iNrBytes = ulLen - ulIndex;     //calculate remaining bytes
            if (!TlvDecodeLen(pucData + ulIndex, &iNrBytes,  &ulFieldLen))            
            {
                bRet = false;
                break;
            }
            ulIndex += iNrBytes;

            if (((ulIndex == ulLen) && (ulFieldLen != 0)) || (ulIndex > ulLen))  //no data-bytes allowed when len=0           
            {
                bRet = false;
                break;
            }


            //--- add the decoded TLV 
            m_oMapTLV[ucTag] = new CTLV(ucTag, pucData + ulIndex, ulFieldLen);
            ulIndex += ulFieldLen;
        }
    }
    return bRet;
}

/** Unstream a multi-TLV-block into m_oMapTLV structure
 Use Fedict TLV syntax; extra length bytes when length-byte = FF
 0-tags can only be used as first TLV of a block
 returns: 0 = invalid data
          1 = OK all done

 */
int CTLVBuffer::ParseTLV(const unsigned char *pucData, unsigned long ulLen)
{
    int iRet = 0;
    
    if(pucData != NULL && ulLen > 0)
    {
        m_oMapTLV.clear();
        iRet = 1;
        unsigned long ulIndex = 0;
        while ((ulIndex+1) < ulLen)      //at least 2 bytes; tag & length
        {
            //--- get & check tag
            unsigned char ucTag = pucData[ulIndex++];

            // tag '0' may only be used as first TLV
            if(ucTag == 0x00 && ulIndex > 2)
            {
                iRet = 0;
                break;
            }

            //--- get Length
            unsigned long ulFieldLen = pucData[ulIndex];

            while(0xFF == pucData[ulIndex]) //add extra length-bytes
            {
                ulFieldLen += pucData[++ulIndex];
                if ((ulIndex+1) >= ulLen)
                {
                    iRet = 0;
                    break;
                }
            }
            ++ulIndex;

            //check if enough data available
            if ((ulIndex+ulFieldLen) > ulLen)
            {
                iRet = 0;
                break;
            }
            //get data
            m_oMapTLV[ucTag] = new CTLV(ucTag, pucData + ulIndex, ulFieldLen);
            ulIndex += ulFieldLen;
        }
    }
    return iRet;
}

CTLV *CTLVBuffer::GetTagData(unsigned char ucTag)
{
    ITMap it = m_oMapTLV.find(ucTag);
    if(it != m_oMapTLV.end())
    {
        return(*it).second;
    }	
    return NULL;
}

void CTLVBuffer::FillASCIIData(unsigned char ucTag, char *pData, unsigned long *pulLen)
{
    CTLV *pTagData = NULL;
    unsigned long ulLength;

    if((NULL != (pTagData = GetTagData(ucTag))) && (pData != NULL) && (pulLen != NULL) && (*pulLen >= (ulLength = pTagData->GetLength())))
    {
        memcpy(pData, pTagData->GetData(), ulLength);
        *pulLen = ulLength;
    }
}

void CTLVBuffer::FillUTF8Data(unsigned char ucTag, char *pData, unsigned long *pulLen)
{
    CTLV *pTagData = NULL;
    unsigned long ulLength;

    if((NULL != (pTagData = GetTagData(ucTag)))&& (pData != NULL) && (pulLen != NULL) && (*pulLen >= (ulLength = pTagData->GetLength())))
    {
        memcpy(pData, pTagData->GetData(), ulLength);
        *pulLen = ulLength;
    }
}

void CTLVBuffer::FillBinaryStringData(unsigned char ucTag, char *pData, unsigned long *pulLen)
{
    CTLV *pTagData = NULL;
    unsigned long ulLength;

    if((NULL != (pTagData = GetTagData(ucTag)))&& (pData != NULL) && (pulLen != NULL))
    {
        char *pszTemp = Hexify(pTagData->GetData(), pTagData->GetLength());
        if (*pulLen > (ulLength = (unsigned long)strlen(pszTemp)))
        {
            memcpy(pData, pszTemp,ulLength+1 );     //copy also terminating 0-char
            *pulLen = ulLength;                     //length without 0-char
        }else
            *pulLen = 0;
        delete [] pszTemp;
    }
}

void CTLVBuffer::FillLongData(unsigned char ucTag, long *piData)
{
    CTLV *pTagData = NULL;
    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        char *pszTemp = new char[pTagData->GetLength() + 1];
        memset(pszTemp, 0, pTagData->GetLength() + 1);
        memcpy(pszTemp, pTagData->GetData(), pTagData->GetLength());
        *piData = atol(pszTemp);
        delete []pszTemp;
    }
}

void CTLVBuffer::FillBinaryData(unsigned char ucTag, 
                                unsigned char *pData, 
                                unsigned long *pulLen       /**< in/out: In=max.length, Out=current length */
                                )
{
    CTLV *pTagData = NULL;

    if(NULL != (pTagData = GetTagData(ucTag)))
    {
        unsigned long ulLength = pTagData->GetLength();

        if(*pulLen >= ulLength)      //length data
            memcpy(pData, pTagData->GetData(), ulLength);
        else
            ulLength = 0;
        *pulLen = ulLength;
    }
}

bool CTLVBuffer::FillBinaryDataCheck(unsigned char ucTag, unsigned char *pData, unsigned long *pulLen)
{
   bool    bRet        = false;
    CTLV *  pTagData    = NULL;

    if(NULL != (pTagData = GetTagData(ucTag)))
    {
      if (*pulLen >= pTagData->GetLength())
      {
         *pulLen = pTagData->GetLength();
         bRet = true;
      }
        memcpy(pData, pTagData->GetData(), *pulLen);
    }
   else
      *pulLen = 0;
   return bRet;
}

//binair to ascii-hex
char *CTLVBuffer::Hexify(unsigned char * pData, unsigned long ulLen) 
{
    char *pszHex = new char[ulLen*2 + 1];
    //memset(pszHex, 0, ulLen*2 + 1);
    if(pData != NULL)
    {
        int j = 0;
        for(unsigned long i = 0; i < ulLen; i++) 
        {
            pszHex[j++] = hexChars[pData[i]>>4 & 0x0F];
            pszHex[j++] = hexChars[pData[i] & 0x0F];
         }
         pszHex[j] = 0;
    }
    return pszHex;
}

unsigned long CTLVBuffer::GetLengthNeeded()
{
    unsigned long   lRet            = 0;
    ITMap           it;
   unsigned char   ucLenBuffer[5];
   int             iBufLen;

    for (it = m_oMapTLV.begin(); it != m_oMapTLV.end(); ++it)
    {
        CTLV *pTemp = (*it).second;
        if(pTemp != NULL)
        {
         memset(ucLenBuffer, 0, sizeof(ucLenBuffer));
         iBufLen=sizeof(ucLenBuffer);
         if(TlvEncodeLen(pTemp->GetLength(), ucLenBuffer, &iBufLen))
         {
            lRet ++;					//Tag
            lRet +=iBufLen;				//Length
            lRet += pTemp->GetLength();	//Data
         }
        }
    }
    return lRet;
}

/**********************************************************************************************************
Encode the length in the TLV format    

    Default one length byte.
    The highest bit = 1 means one additional length-byte.
    The lowest 7 bits of each byte are 7 bits of the length-number.

    1 length byte  will encode max 7-bit  numbers, between     00 -       7F  
    2 length bytes will encode max 14-bit numbers, between     80 -     3FFF
    3 length bytes will encode max 21-bit numbers, between    400 -  1F FFFF
    4 length bytes will encode max 28-bit numbers, between 20 000 - FFF FFFF
    
examples:
  len=7B        -> encoding =          7B    
  len=CD        -> encoding =       81 4D
  len=2039      -> encoding =       C0 39 
  len=43ABDA    -> encoding = 82 8E D7 5A
    
 **********************************************************************************************************/
bool CTLVBuffer::TlvEncodeLen(unsigned long ulLenVal,  //In: length-value to encode in the TLV-format
                    unsigned char *pucBufDest,  //In/Out: Destination buffer with the encoded length-stream
                    int *piBufLen  //In/out: Max buffer length, Out:length of encoded stream
                    )
{
    int iStreamLen = 1;         //length of the stream
    unsigned long  ulLenValLoc = 0;        //length
    unsigned char ucExtraByte = 0x00;

    //--- parameter check   
    if (pucBufDest == NULL || piBufLen == NULL)
    {
        return false;
    }

    //--- determinate number of length-bytes
    ulLenValLoc = ulLenVal>>7;

    while(ulLenValLoc)
    {
        iStreamLen++;
        ulLenValLoc >>= 7;
    }

    if (*piBufLen < iStreamLen)
    {
        return false;
    }
    *piBufLen = iStreamLen;

    //--- write encoded length
    ulLenValLoc = ulLenVal;
    pucBufDest[0] = 0;
    ucExtraByte = 0;
    while(iStreamLen--)
    {
        pucBufDest[iStreamLen] = ((unsigned char)ulLenValLoc & 0x7F) + ucExtraByte;
        ucExtraByte = 0x80;
        ulLenValLoc >>= 7;
    }

    return true;
}

/**********************************************************************************************************
Decode the length from the TLV format    
   
 **********************************************************************************************************/
bool CTLVBuffer::TlvDecodeLen(
        const unsigned char *   pucBufSrc, //In: Source buffer with the encoded length-stream
        int *                   piBufLen,  //In/Out: Maximum length of encoded stream, Out:number of length-bytes
        unsigned long *         pulLenVal  //Out: length-value from the TLV stream
                    )
{
    int  iStreamLenMax = 0; //length of the stream

    //--- parameter check   
    if (pucBufSrc == NULL || piBufLen == NULL || (*piBufLen == 0) || pulLenVal == NULL)
    {
        return false;
    }

    //--- decode first byte   
    iStreamLenMax = *piBufLen;
    *piBufLen     = 1;
    *pulLenVal    = 0x7F & *pucBufSrc;

    //--- handle additional length bytes   
    while( *(pucBufSrc++) & 0x80)
    {
        if (++(*piBufLen) > iStreamLenMax)
        {
            return false;
        }

        *pulLenVal = (*pulLenVal<<7) + (0x7F & *pucBufSrc);
    }
    return true;
}

void CTLVBuffer::SetTagData(unsigned char ucTag, const unsigned char *pucData, unsigned long ulLen)
{

   if(pucData)
   {
      ITMap itr;

      itr = m_oMapTLV.find(ucTag);
      if(itr==m_oMapTLV.end())
      {
         m_oMapTLV[ucTag] = new CTLV(ucTag, pucData, ulLen);
      }
      else
      {
         itr->second->ReplaceData(pucData, ulLen);
      }
   }
}

unsigned long CTLVBuffer::Extract(unsigned char *pucData, unsigned long ulLen)
{

   if(!pucData)
      return 0;

   CByteArray      oTempArray;
   unsigned char   ucLenBuffer[4]  = {0};
   int             iBufLen         = sizeof(ucLenBuffer);
   ITMap           it;

    for (it = m_oMapTLV.begin(); it != m_oMapTLV.end(); ++it)
    {

      iBufLen=sizeof(ucLenBuffer);
      memset(ucLenBuffer, 0, sizeof(ucLenBuffer));

      if(TlvEncodeLen(it->second->GetLength(), ucLenBuffer, &iBufLen))
      {
         oTempArray.Append(it->second->GetTag());
         oTempArray.Append(ucLenBuffer, iBufLen);         
         oTempArray.Append(it->second->GetData(), it->second->GetLength());         
      }
   }

    unsigned long ulSize = oTempArray.Size();
    if(ulLen >= ulSize)
    {
        memcpy(pucData, oTempArray.GetBytes(), ulSize);
      return ulSize;
   }

   return 0;
}


}  // namespace eIDMW
