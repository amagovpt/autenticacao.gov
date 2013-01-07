/*
 * errorsext.h
 */

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

#ifndef HEADER_XXX_H
#define HEADER_XXX_H

#ifdef	__cplusplus
extern "C" {
#endif

#define ERR_LIB_CMP            50
#define ERR_LIB_ESS            51
#define ERR_LIB_TSP            52
#define ERR_LIB_DVCS           53
#define ERR_LIB_CMS            54

#define CMPerr(f,r) ERR_PUT_error(ERR_LIB_CMP,(f),(r),__FILE__,__LINE__)
#define ESSerr(f,r) ERR_PUT_error(ERR_LIB_ESS,(f),(r),__FILE__,__LINE__)
#define TSPerr(f,r) ERR_PUT_error(ERR_LIB_TSP,(f),(r),__FILE__,__LINE__)
#define DVCSerr(f,r) ERR_PUT_error(ERR_LIB_DVCS,(f),(r),__FILE__,__LINE__)
#define CMSerr(f,r) ERR_PUT_error(ERR_LIB_CMS,(f),(r),__FILE__,__LINE__)

#include "asn1_access.h"

#ifdef	__cplusplus
}
#endif

#endif

