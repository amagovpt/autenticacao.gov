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
/*

0000 0
0001 1
0010 2
0011 3
0100 4
0101 5
0110 6
0111 7
1000 8
1001 9
1010 A
1011 B
1100 C
1101 D
1110 E
1111 F


ASN1:
--- Tag
   Bin     Hex:    Bits:
00 0 11111   1F      1-5    tag-value 0-1E, 1F=multiple bytes until MSB=0; 
00 1 00000   20      6      type 0=primitive      contains primitive value, definite length encodeing 
                                                    1=boolean, 2=integer, 3=bitstring(padding), 4=octet string, 9=real, 10=enumerated, 12=IA5String

                                 1=constructed    contains other TLV's,     
                                                  definite or indefinite length encoding 
                                                  values:
                                                    16=sequence(of):ordered   set of items as prescribed in the definition, optional items posible
                                                    17=set(of)     :unordered set of items                                , optional items posible

                                                    ??=choice
                                                  "sequence" = same types, "sequence of" = different types
* Other types
  choice: collection of distinct type from which to choose one, denotes a union of one or more alternatives
  selection: select a component from a choice type
  any: enable a application to specify the type

* Implicit/explicit tagging: to distinguish component types within a structured type
      Implicitly tagged
      - changing the tag of the underlying type
      Explicitly tagged
      - adding an outer tag to the underlying type


11 0 00000   C0      7-8    Class:      Value:
                             universal    00     ASN.1 native types
                             application  01     valid for one application; application number in lower 6 bits ex. APL.2 = 01 1 00010
                             context      10     depends on the context,    lower-bits=index
                             private      11     custum defined

Universal Class Tags Name  P/C Number (decimal) Number (hex) 
EOC (End-of-Content)        P   0   0x00 
BOOLEAN                     P   1   0x01 
INTEGER                     P   2   0x02 
BIT STRING                  P/C 3   0x03 
OCTET STRING                P/C 4   0x04 
NULL                        P   5   0x05 
OBJECT IDENTIFIER           P   6   0x06 
Object Descriptor           P   7   0x07 
EXTERNAL                    C   8   0x28 
REAL (float)                P   9   0x09 
ENUMERATED                  P   10  0x0A 
EMBEDDED PDV                C   11  0x0B 
UTF8String                  P/C 12  0x0C 
RELATIVE-OID                P   13  0x0D 
SEQUENCE and SEQUENCE OF    C   16  0x30 
SET and SET OF              C   17  0x31 
NumericString               P/C 18  0x12 
PrintableString             P/C 19  0x13 
T61String                   P/C 20  0x14 
VideotexString              P/C 21  0x15 
IA5String                   P/C 22  0x16 
UTCTime                     P/C 23  0x17 
GraphicString               P/C 25  0x19 
VisibleString               P/C 26  0x1A 
GeneralString               P/C 27  0x1B 
UniversalString             P/C 28  0x1C 
CHARACTER STRING            P/C 29  0x1D 
BMPString                   P/C 30  0x1E 



--- Len
    00-7F       single length byte 00-7F
    81 xx       single length byte 00-FF
    82 xx xx    double length byte 0000-FFFF
    ...
    FF xx...    127 length bytes

--- Data


30      constructed sequence
3A      len
A1 34   optional tag.1 len 34
A4 32   tag.4 len 32

extra info: 
ftp.rsa.com/pub/pkcs/ascii/layman.asc               asn1
ftp.rsasecurity.com/pub/pkcs/pkcs-15/pkcs15v1.doc   P15
------------------------------------
*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "asn1.h"

//int get_item_length(ASN1_LIST *list, unsigned int n, unsigned int *l, unsigned int *nsubitems);

/** skips the specified number of items
 skips iPath-1 times the whole [Tag/length/data] block
     1 returns the first block
     2 returns the second block

   Return  0  : ok, found
           >0 : error
*/
int skip_item(const unsigned char *p_cInDat,      /**< In: ASN-1 data */
              unsigned int  iInLen,         /**< In: ASN-1 len */ 
              unsigned int  iPath,          /**< In: Path/Index; 1=first item, 2=second item */
              const unsigned char **pp_cOutDat,   /**< Out: result ASN-1 data */
              unsigned int  *l_out          /**< Out: result ASN-1 len */
              )
{
unsigned int    iNumTag =0;
unsigned int    iLengthLen = 0;     //iTypeTag, iClassTag, 
const unsigned char   *p_cDat = p_cInDat;
const unsigned char   *p_cEnd = p_cInDat + iInLen - 1;  //points to last byte from input stream
unsigned int    iIndex,j;
unsigned int    l_tag;
unsigned int    l_len;

for (iIndex=1; iIndex < iPath; iIndex++)
   {
//   iClassTag = *p_cDat & CLASS_MASK;
//   iTypeTag  = *p_cDat & TYPE_MASK;

	if((*p_cDat == 0)&&(*(p_cDat+1) == 0))
	{
		p_cDat += 2;
		while((*p_cDat == 0) && (p_cDat <= p_cEnd))
		{
			p_cDat++;
		}
	}
	else
	{
	   //--- get tag-value
	   iNumTag   = *p_cDat & TAG_MASK;          //tag-value  

	   if (iNumTag == TAG_MASK)
		  {
		  //decode multibyte tag; iNumTag = sequence of 7 bit values
		  iNumTag = 0;
		  l_tag = 0;
		  do 
			 {
			 p_cDat++;
			 l_tag++;
			 if (l_tag > 4)                                     /* iNumTag has max 4 bytes length; 4 times 7 bit */
				return (E_ASN_TAG_LEN);
			 iNumTag = (iNumTag << 7) | (*p_cDat & LEN_MASK);   //add extra length-byte
			 } while ((*p_cDat & EXT_LEN) && (p_cDat < p_cEnd));
		  }

	   if (p_cDat > p_cEnd) //???check this: if p_cDat==p_cEnd and length = 0, this could be the case for a NULL element, otherwise p_cDat should be < p_cEnd
		  return (E_ASN_INCOMPLETE);
	   
	   /*--- get length of asn1_item */
	   p_cDat++;
	   iLengthLen = l_len = (unsigned int) *p_cDat;
	   if (l_len & EXT_LEN)             //test if multi length-byte
		  {
		  if ((l_len &= LEN_MASK) > 4)  //max 4 length-bytes
			 return (E_ASN_BAD_LEN);
	/*      if (l_len == 0)
			 indefinite = 1; */
		  iLengthLen = 0;   
		  for (j=0; j < l_len; j++)     //add all length-bytes
			 {
			 p_cDat++;
			 if (p_cDat > p_cEnd)
				return (E_ASN_INCOMPLETE);
			 iLengthLen = (iLengthLen << 8) | *p_cDat;
			 }
		  }

	   /* skip iNumTag, length and data to get next item */
	   p_cDat = p_cDat + iLengthLen + 1;
	   if (p_cDat > p_cEnd)
		  return (E_ASN_ITEM_NOT_FOUND);
		}
	}
//p_cDat can never be bigger then p_cEnd (p_cEnd=p_cInDat + iInLen - 1;) 
//so p_cDat-p_cInDat < iInLen - 1
*pp_cOutDat = p_cDat;
*l_out = iInLen - (unsigned int)(*pp_cOutDat-p_cInDat);

return (0);
}


int asn1_get_item(const unsigned char *p_cInDat,     /**< In: ASN-1 data */
                  unsigned int        iInLen,        /**< In: ASN-1 len   */
                  const char          *p_cInPath,    /**< In: path  e.g.  "\1\1\2" = 0x01 0x01 0x02 0x00 */
                  ASN1_ITEM           *p_xItem       /**< Out: object   */
                  )
{
unsigned int  iClassTag = 0;
unsigned int  iTypeTag  = 0;
unsigned int  iNumTag   = 0;
const unsigned char *p_cDat   = p_cInDat;
const unsigned char *p_cRawDat= NULL;
unsigned int  iRawLen   = 0;
unsigned int  iLen      = iInLen;
unsigned int  iLengthLen= 0;
const unsigned char *p_cEnd   = p_cInDat + iInLen - 1;
const char *p_cPath  = p_cInPath;
int           iRet      = 0;

memset(p_xItem, 0, sizeof (ASN1_ITEM));

for (; *p_cPath; p_cPath++)
   {
   iRet = skip_item(p_cDat, iLen, *p_cPath, &p_cDat, &iLen);   //goto required item
   if (iRet)
      return (iRet);

   /* check if we are decoding inside a BIT STRING: iNumTag == parent_tag */
   /* first octet of bit string is the number of unused bits at the end of the bitstring */
   /* in CER/DER: unused bits are always zero. And if they aren't zero, we still don't need to know the nr. of unused bits */
   if ((iNumTag == 0x03) )
      {
      p_cDat++;
      iLen--;
      }

   p_cRawDat = p_cDat;
   iRawLen = 0;

   //--- get tag-value
   iClassTag = *p_cDat & CLASS_MASK;
   iTypeTag  = *p_cDat & TYPE_MASK;
   iNumTag   = *p_cDat & TAG_MASK;

	if((*p_cDat == 0)&&(*(p_cDat+1) == 0))
	{
		iRawLen = 2;
		p_cDat += 2;
		iLen = 0;
		while((*p_cDat == 0) && (p_cDat <= p_cEnd))
		{
			p_cDat++;
			iLen++;
		}
	}
	else
	{
	   if (iNumTag == TAG_MASK)
		  {
		  //decode multibyte tag; iNumTag = sequence of 7 bit values
		  /* iNumTag = sequence of 7 bit values */
		  iNumTag = 0;
		  do 
			 {
			 p_cDat++; iRawLen++;
	//         if (p_cDat > p_cInDat + 4)     //multi-byte tag should be one of first 4 bytes ???????????
			 if (p_cDat > p_cRawDat + 4)     //multi-byte tag should be max 4 bytes with this implementation (ifnot overflow iNumTag)
				return (E_ASN_TAG_LEN);
			 iNumTag = (iNumTag << 7) | (*p_cDat & LEN_MASK);
			 } while ((*p_cDat & EXT_LEN) && (p_cDat < p_cEnd));
		  }			

	   if (p_cDat == p_cEnd)                    //check if length-byte present
		  return (E_ASN_INCOMPLETE);

	  //--- decode multi-byte length
	   p_cDat++; iRawLen++;
	   iLen = iLengthLen = (unsigned int) *p_cDat;

	   if (iLengthLen & EXT_LEN)
		  {
		  if ((iLengthLen &= LEN_MASK) > 4)     //max 4 length bytes supported
			 return (E_ASN_BAD_LEN);
	/*      if (iLengthLen == 0)
			 indefinite = 1; */
		  iLen = 0;
		  while (iLengthLen--)      
			 {
			 p_cDat++; iRawLen++;
			 if (p_cDat > p_cEnd)               //check if promissed extra length-bytes present
				return (E_ASN_INCOMPLETE);
			 iLen = (iLen << 8) | *p_cDat;
			 }
		  }
	   p_cDat++; iRawLen++;
	   }
	}
/* p_data points to element itself after length encoding, not the iNumTag */
p_xItem->p_data = p_cDat;
p_xItem->l_data = iLen;
/* construct tag: bit 1 and 2 for class, bit 3 for primitiv or structured and tagnum�ber from bit 4-32 = 29 bits for number encoding */
p_xItem->tag = (iClassTag >> 6) | (iTypeTag >> 3 ) | (iNumTag << 3);

p_xItem->p_raw = p_cRawDat;
p_xItem->l_raw = iRawLen + iLen;

return (0);
}


/* returns next item, updates Level0 parameters too: pointing just after new object */
int asn1_next_item(ASN1_ITEM          *p_xLev0Item,      /**< In/out: object   */
                   ASN1_ITEM          *p_xLev1Item       /**< Out: object   */
                  )
{
    int iReturn = asn1_get_item(p_xLev0Item->p_data, p_xLev0Item->l_data, ASNPATH_FIRST, p_xLev1Item);
    if (iReturn == 0)
    {
        if (p_xLev1Item->l_raw > p_xLev0Item->l_data)  p_xLev1Item->l_raw = p_xLev0Item->l_data; //make sure not to pass the end
        p_xLev0Item->p_data += p_xLev1Item->l_raw;      //next item
        p_xLev0Item->l_data -= p_xLev1Item->l_raw;    
    }
    return(iReturn);
}


//preliminary version to find items with particular TAG in linear list of ASN1 items
int asn1_find_item(const unsigned char *p_cInDat, /**< In: ASN-1 data */
                   unsigned int iInLen,           /**< In: ASN-1 len   */
                   unsigned int findtag,          /**< In: tag to look for */
                   ASN1_ITEM *p_xItem             /**< Out: object   */
                   )
{
unsigned int  iClassTag = 0;
unsigned int  iTypeTag  = 0;
unsigned int  tagnum    = 0;
unsigned int  iNumTag   = 0;
unsigned char *p_cDat = (unsigned char*) p_cInDat;
unsigned int iLen = iInLen;
unsigned int iLengthLen = 0;
unsigned char *p_cEnd = (unsigned char*)p_cInDat + iInLen - 1;
int found = 0;

memset(p_xItem, 0, sizeof (ASN1_ITEM));

do
   {
   iClassTag = *p_cDat & CLASS_MASK;
   iTypeTag  = *p_cDat & TYPE_MASK;
   tagnum    = *p_cDat & TAG_MASK;

   if (tagnum == TAG_MASK)
      {
      /* tagnum = sequence of 7 bit values */
      tagnum = 0;
      do 
         {
         p_cDat++;
         if (p_cDat > p_cInDat + 4)
            return (E_ASN_TAG_LEN);
         tagnum = (tagnum << 7) | (*p_cDat & LEN_MASK);
         } while ((*p_cDat & EXT_LEN) && (p_cDat < p_cEnd));
      }			

   iNumTag = (iClassTag >> 6) | (iTypeTag >> 3 ) | (tagnum << 3);

   if (p_cDat == p_cEnd) //????
      return (E_ASN_INCOMPLETE);

   p_cDat++;
   iLen = iLengthLen = (unsigned int) *p_cDat;
   if (iLengthLen & EXT_LEN)
      {
      if ((iLengthLen &= LEN_MASK) > 4)
         return (E_ASN_BAD_LEN);
/*      if (iLengthLen == 0)
         indefinite = 1;    */
      iLen = 0;
      while (iLengthLen--)
         {
         p_cDat++;
         if (p_cDat > p_cEnd)
            return (E_ASN_INCOMPLETE);
         iLen = (iLen << 8) | *p_cDat;
         }
      }

   if (findtag == iNumTag)
      {
      //point to value
      p_cDat++;
      found = 1;
      }
   else
      //point to next item
      p_cDat = p_cDat + iLen + 1;

   if (p_cDat > p_cEnd)
      return (E_ASN_ITEM_NOT_FOUND);
   } while (found == 0);

if (!found)
   return (E_ASN_ITEM_NOT_FOUND);

p_xItem->p_data = p_cDat;
p_xItem->l_data = iLen;
p_xItem->tag = findtag;

return (0);
}





/* decode asn1 oid to array of numbers */
int dec_oid( unsigned char *p_data, 
             int           l_data, 
             unsigned int  *oid, 
             int           l_oid)
{
int           len = 0;
unsigned int  id = 0;

while(l_data > 0 && l_oid > 0)
  {
  id = (id << 7) + (*p_data & 0x7f);
  if((*p_data & 0x80) == 0)
    {
    if(len == 0)
      {
      *oid++ = id / 40;
      id %= 40;
      l_oid--;
      len++;
      }
    *oid++ = id;
    l_oid--;
    len++;
    id = 0;
    }
  p_data++;
  l_data--;
  }

return(len);
}


void asn_ui2bitstring(unsigned int in, unsigned char *out, unsigned int *l_out)
{
unsigned int i;
unsigned char *p = out;
unsigned char unused_bits = 0;

for (i=0; i<sizeof(in)*8; i++, in >>= 1)
  {
  if (i%8 == 0)
    *(++p) = 0;   //start from byte 2 and clear each new byte first

  if (in & 0x01)
    {
    unused_bits = 0;
    *p |= 1 << (7-i%8);
    }
  else
    unused_bits++;
  }

//unused bits
*out   = unused_bits % 8;
//first byte is length of bitstring in octets + 1 byte for unused bits
*l_out = sizeof(unsigned int) + 1 - unused_bits / 8; 

return;
}
