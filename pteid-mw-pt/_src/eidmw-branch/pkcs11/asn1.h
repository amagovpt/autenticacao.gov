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
#ifndef __ASN1_H__
#define __ASN1_H__

#ifdef __cplusplus
extern "C" {
#endif

/* return codes for ASN1 parsing */
#define E_ASN_BAD_TAG        -1           /* tag not expected or unknown */
#define E_ASN_TAG_LEN        -2           /* tag has length > 4 bytes */
#define E_ASN_BAD_LEN        -3           /* length encoding error */
#define E_ASN_INCOMPLETE     -4           /* unexpected end of ASN1 encoding */
#define E_ASN_ITEM_NOT_FOUND -5           /* item in path does not exist */
#define E_ASN_ALLOC          -6           /* memory allocation error */
#define E_ASN_LIST           -7           /* error in asn1 item list to encode */
//#define ASN1_ERR -1


typedef struct ASN1_ITEM 
{
   unsigned int         tag;          //tag
   const unsigned char* p_data;      //value data
   unsigned int	        l_data;       //value len
   unsigned int         nsubitems;    //number of sub-items (not yet implemented !, always 0)
   const unsigned char* p_raw;       //total TLV data
   unsigned int         l_raw;        //total TLV len
} ASN1_ITEM;


typedef struct ASN1_LIST
{
   unsigned int   size;
   unsigned int   nitems;
   ASN1_ITEM      *item;
} ASN1_LIST;



typedef struct T_OID
{
	char *dotid;
	char *str;
} T_OID;



/* BER ASN1 decoding masks */
#define TAG_MASK   0x1F  /* bits 1 - 5  */
#define TYPE_MASK  0x20  /* bit  6      */
#define CLASS_MASK 0xC0  /* bits 7,8    */

#define LEN_MASK   0x7F  /* bits 1 - 7  */
#define EXT_LEN    0x80  /* bit  8      */

/* For this implementation: class, type and tagnumber are converted to single unsigned int max 4 bytes (32-bit) */
/* 2 bits for class, 1 bit for type so 29 bits remain for tagnumber (32-bit system)    */
/* tag encoded as multiple of 7 bits cannot be bigger than 28 bits anyway */
/* following are the definitions for class, type and value */
/* ASN_CLASS: bit 1 and 2 */
#define ASN_UNIVERSAL    0x00
#define ASN_APPLICATION  0x01
#define ASN_CONTEXT      0x02
#define ASN_PRIVATE      0x03
#define ASN_CLASS        0x03

/* ASN_TYPE: bit 3 */
#define ASN_PRIMITIV     0x00
#define ASN_CONSTRUCTED  0x04
#define ASN_TYPE         0x04

/* COMMON ASN_TAGS */
//#define ASN_EOC               ( ASN_CONTEXT   | ASN_CONSTRUCTED | (0x00 << 3) )   /* end of contents octet */
#define ASN_BOOLEAN             ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x01 << 3) )      
#define ASN_INTEGER             ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x02 << 3) )      
#define ASN_BIT_STRING          ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x03 << 3) )
#define ASN_OCTET_STRING        ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x04 << 3) )
#define ASN_NULL                ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x05 << 3) )    
#define ASN_OID                 ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x06 << 3) )    
#define ASN_OBJDESCRIP          ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x07 << 3) )    
#define ASN_EXTERNAL            ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x08 << 3) )    
#define ASN_REAL                ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x09 << 3) )    
#define ASN_ENUM                ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x0A << 3) )    
#define ASN_EMBEDED_PDV         ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x0B << 3) )    
#define ASN_UTF8STRING          ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x0C << 3) )    

#define ASN_SEQUENCE            ( ASN_UNIVERSAL | ASN_CONSTRUCTED | (0x10 << 3) )  
#define ASN_SET                 ( ASN_UNIVERSAL | ASN_CONSTRUCTED | (0x11 << 3) )
#define ASN_NUMSTRING           ( ASN_UNIVERSAL | ASN_CONSTRUCTED | (0x12 << 3) )
#define ASN_PRINTABLE_STRING    ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x13 << 3) )
#define ASN_T61STRING           ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x14 << 3) )
#define ASN_VIDEOTXTSTRING      ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x15 << 3) )
#define ASN_IA5STRING           ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x16 << 3) )
#define ASN_UTCTIME             ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x17 << 3) )
#define ASN_GENERALIZEDTIME     ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x18 << 3) )
#define ASN_GRAPHICSTRING       ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x19 << 3) )
#define ASN_VISIBLESTRING       ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x1A << 3) )
#define ASN_GENERALSTRING       ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x1B << 3) )
#define ASN_UNIVERSALSTRING     ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x1C << 3) )
#define ASN_BMPSTRING           ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0x1E << 3) )
#define ASN_ANY                 ( ASN_UNIVERSAL | ASN_PRIMITIV    | (0xfffff << 3) )

#define ASNP15_DIRREC           ( ASN_APPLICATION | ASN_CONSTRUCTED | (01 << 3) )
#define ASNP15_DIRAID           ( ASN_APPLICATION | ASN_PRIMITIV    | (15 << 3) )
#define ASNP15_DIRLABEL         ( ASN_APPLICATION | ASN_PRIMITIV    | (16 << 3) )
#define ASNP15_DIRPATH          ( ASN_APPLICATION | ASN_PRIMITIV    | (17 << 3) )

/* ASN1 structured element */
#define ASN_SE(a)                    ( ASN_CONTEXT   | ASN_CONSTRUCTED   | (a << 3) )
#define ASN_CONSTRUCTED_CONTEXT_N(n) ( ASN_CONTEXT   | ASN_CONSTRUCTED   | (n << 3) )
#define ASN_PRIMITIVE_CONTEXT_N(n)   ( ASN_CONTEXT   | ASN_PRIMITIV      | (n << 3) )


#define OID_RSA_ENCRYPTION      "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01"
#define OID_SHA256_ALGORITHM	"\x60\x86\x48\x01\x65\x03\x04\x02\x01"
#define ASNPATH_FIRST           "\x01"

/* ASN1 decoding functions */
int asn1_get_item(const unsigned char *content, unsigned int len, const char *path, ASN1_ITEM *item);
int asn1_find_item(const unsigned char *content, unsigned int len, unsigned int findtag, ASN1_ITEM *item);
int dec_oid( unsigned char *p_data, int l_data, unsigned int *oid, int l_oid);
int asn1_next_item(ASN1_ITEM          *p_xLev0,       /**< In/out: object   */
                   ASN1_ITEM          *p_xLev1        /**< Out: object   */
                  );

#ifdef __cplusplus
   }
#endif


#endif
