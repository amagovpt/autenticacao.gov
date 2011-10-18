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
//basic certificate decoder
#include <stdio.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asn1.h"
#include "cert.h"


int cert_get_info(const unsigned char *pcert, unsigned int lcert, T_CERT_INFO *info)
{
int ret = 0;
ASN1_ITEM item;

memset(info, 0, sizeof(T_CERT_INFO));

//check size of cert
ret = asn1_get_item(pcert, lcert, "\1", &item);
if (ret)
   return(ret);
if (item.l_raw <= lcert)
   lcert = info->lcert = item.l_raw;
else
   return (E_X509_INCOMPLETE);

ret = asn1_get_item(pcert, lcert, X509_SUBJECT, &item);
if (ret)
   return(ret);

info->subject = (char*) malloc(item.l_raw);
if (info->subject == NULL)
   return(E_X509_ALLOC);
memcpy(info->subject, item.p_raw, item.l_raw);
info->l_subject = item.l_raw;

ret = asn1_get_item(pcert, lcert, X509_ISSUER, &item);
if (ret)
   return(ret);

info->issuer = (char*) malloc(item.l_raw);
if (info->issuer == NULL)
   return(E_X509_ALLOC);
memcpy(info->issuer, item.p_raw, item.l_raw);
info->l_issuer = item.l_raw;

ret = asn1_get_item(pcert, lcert, X509_SERIAL, &item);
if (ret)
   return(ret);
else if (item.tag != ASN_INTEGER)
   return(E_X509_DECODE);

info->serial = (char*) malloc (item.l_raw);
if (info->serial == NULL)
   return(E_X509_ALLOC);
memcpy(info->serial, item.p_raw, item.l_raw);
info->l_serial = item.l_raw;

ret = asn1_get_item(pcert, lcert, X509_VALID_FROM, &item);
if (ret)
   return(ret);

info->validfrom = (char*) malloc(item.l_data);
if (info->validfrom == NULL)
   return(E_X509_ALLOC);
memcpy(info->validfrom, item.p_data, item.l_data);
info->l_validfrom = item.l_data;

ret = asn1_get_item(pcert, lcert, X509_VALID_UNTIL, &item);
if (ret)
   return(ret);

info->validto = (char*) malloc(item.l_data);
if (info->validto == NULL)
   return(E_X509_ALLOC);
memcpy(info->validto, item.p_data, item.l_data);
info->l_validto = item.l_data;

/* check for RSA keytype */
/*ret = asn1_get_item(pcert, lcert, X509_KEYTYPE, &item);
if (ret)
   return(ret);
else if (item.tag != ASN_OID)
   return (E_X509_DECODE);
else if (item.l_data != sizeof(OID_RSA_ENCRYPTION)-1)
   return (E_X509_UNKNOWN_KEYTYPE);
else if (memcmp(item.p_data, OID_RSA_ENCRYPTION, item.l_data) != 0)
   return (E_X509_UNKNOWN_KEYTYPE);
*/

/* RSA: modulus and exponent */
ret = asn1_get_item(pcert, lcert, X509_RSA_MOD, &item);
if (ret)
   return(ret);
if (*(item.p_data) == 0)
   {
   /* first byte could be zero in ASN_INTEGER, skip this */
   item.p_data++;
   item.l_data--;
   }

info->mod = (char*) malloc(item.l_data);
if (info->mod == NULL)
   return(E_X509_ALLOC);

memcpy(info->mod, item.p_data, item.l_data);
info->l_mod = item.l_data;

ret = asn1_get_item(pcert, lcert, X509_RSA_EXP, &item);
if (ret)
   return(ret);
if (*(item.p_data) == 0)
   {
   /* first byte could be zero in ASN_INTEGER, skip this */
   item.p_data++;
   item.l_data--;
   }
info->exp = (char*) malloc(item.l_data);
if (info->exp == NULL)
   return(E_X509_ALLOC);
memcpy(info->exp, item.p_data, item.l_data);
info->l_exp = item.l_data;

/* PKINFO */
ret = asn1_get_item(pcert, lcert, X509_PKINFO, &item);
if (ret)
   return(ret);
info->pkinfo = malloc(item.l_raw);
if (info->pkinfo == NULL)
   return(E_X509_ALLOC);
memcpy(info->pkinfo, item.p_raw, item.l_raw);
info->l_pkinfo = item.l_raw;

return (0);
}



