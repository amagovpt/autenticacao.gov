/* cmp_asn1.c */
/* Written by Peter Sylvester <peter.sylvester@edelweb.fr> for the OpenEvidence
 * project. */

/*
 * Copyright (c) 2003 The OpenEvidence Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, the following disclaimer,
 *    and the original OpenSSL and SSLeay Licences below. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions, the following disclaimer 
 *    and the original OpenSSL and SSLeay Licences below in
 *    the documentation and/or other materials provided with the
 *    distribution. 
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgments:
 *    "This product includes software developed by the Openevidence Project
 *    for use in the OpenEvidence Toolkit. (http://www.openevidence.org/)"
 *    This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *    This product includes cryptographic software written by Eric Young
 *    (eay@cryptsoft.com).  This product includes software written by Tim
 *    Hudson (tjh@cryptsoft.com)."
 *
 * 4. The names "OpenEvidence Toolkit" and "OpenEvidence Project" must not be
 *    used to endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openevidence-core@openevidence.org.
 *
 * 5. Products derived from this software may not be called "OpenEvidence"
 *    nor may "OpenEvidence" appear in their names without prior written
 *    permission of the OpenEvidence Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgments:
 *    "This product includes software developed by the OpenEvidence Project
 *    for use in the OpenEvidence Toolkit (http://www.openevidence.org/)
 *    This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *    This product includes cryptographic software written by Eric Young
 *    (eay@cryptsoft.com).  This product includes software written by Tim
 *    Hudson (tjh@cryptsoft.com)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenEvidence PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenEvidence PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes software developed by the OpenSSL Project
 * for use in the OpenSSL Toolkit (http://www.openssl.org/)
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

//#include "oe.h"
#include <openssl/asn1t.h>
#include "cmp.h"

#include <string.h>


ASN1_SEQUENCE(CMP_PKISTATUS_INFO) = {

    ASN1_SIMPLE(CMP_PKISTATUS_INFO,status,ASN1_INTEGER),
    ASN1_SEQUENCE_OF_OPT(CMP_PKISTATUS_INFO,statusString,ASN1_UTF8STRING), 
    ASN1_OPT(CMP_PKISTATUS_INFO,failInfo,ASN1_BIT_STRING)

} ASN1_SEQUENCE_END(CMP_PKISTATUS_INFO) ;

IMPLEMENT_ASN1_FUNCTIONS(CMP_PKISTATUS_INFO) 
IMPLEMENT_ASN1_DUP_FUNCTION(CMP_PKISTATUS_INFO)

IMPLEMENT_STACK_OF(ASN1_UTF8STRING)

IMPLEMENT_ASN1_DUP_FUNCTION(GENERAL_NAME)


int CMP_PKISTATUS_INFO_add_fail_info(CMP_PKISTATUS_INFO *si, int code)
{
	if (!si->failInfo && !(si->failInfo = ASN1_BIT_STRING_new()))
		goto err;
	if (!ASN1_BIT_STRING_set_bit(si->failInfo, code, 1))
		goto err;
	return 1;
 err:
	CMPerr(CMP_F_PKISTATUS_INFO_ADD_FAILURE_INFO, ERR_R_MALLOC_FAILURE);
	return 0;
}

int CMP_PKISTATUS_INFO_set_status(CMP_PKISTATUS_INFO *si, int status)
{

	if (ASN1_INTEGER_set(si->status, status)) return 1;
	
	CMPerr(CMP_F_PKISTATUS_INFO_SET_STATUS, ERR_R_MALLOC_FAILURE);
	return 0;
}

int CMP_PKISTATUS_INFO_get_status(CMP_PKISTATUS_INFO *si)
{
	if (!si->status) return -1;
	return ASN1_INTEGER_get(si->status);
	
}

unsigned char * CMP_PKISTATUS_INFO_get_status_string (CMP_PKISTATUS_INFO *si, unsigned int i)
{
	char *text_to_be_returned = NULL;

	if (!si->statusString) return NULL;

	if (sk_ASN1_UTF8STRING_num(si->statusString) <= i) return NULL;

	{ 	int len; 
		ASN1_UTF8STRING * statusString = sk_ASN1_UTF8STRING_value(si->statusString,i) ;
		len = ASN1_STRING_length(statusString);
		if (!(text_to_be_returned = (char *) OPENSSL_malloc(len + 1)))
			{
			CMPerr(CMP_F_PKISTATUS_INFO_GET_STATUS_STRING, ERR_R_MALLOC_FAILURE);
			return 0;
			}
		strncpy(text_to_be_returned , ASN1_STRING_data(statusString), len);
		text_to_be_returned[len] = '\0';
		return text_to_be_returned ;
	}
}

int CMP_PKISTATUS_INFO_add_status_string (CMP_PKISTATUS_INFO *si, const char *text)
{	
	ASN1_UTF8STRING *statusString;
	if (!(si->statusString))
		 si->statusString = sk_ASN1_UTF8STRING_new_null();
 
	if (!(si->statusString)) {
		CMPerr(CMP_F_PKISTATUS_INFO_ADD_STATUS_STRING, ERR_R_MALLOC_FAILURE);
		return 0;
		}

	if (! ((statusString = ASN1_UTF8STRING_new())
              && ASN1_STRING_set(statusString, text, strlen(text)))) {
		CMPerr(CMP_F_PKISTATUS_INFO_ADD_STATUS_STRING, ERR_R_MALLOC_FAILURE);
		return 0;
		}

	if (sk_ASN1_UTF8STRING_push(si->statusString,statusString) <= 0) {
		CMPerr(CMP_F_PKISTATUS_INFO_ADD_STATUS_STRING, ERR_R_MALLOC_FAILURE);
		return 0;
	}
	return 1;

}


