/*
 * $Id: tsproto.h,v 1.1.1.1 2003/07/03 15:24:54 adminweb Exp $
 * 
 * ASN1 definitions used in OETS timestamping protocol.
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

#ifndef TSPROTO_H_INCLUDED
# define TSPROTO_H_INCLUDED

# include <openssl/asn1.h>
# include <openssl/x509.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * MessageImprint ::= SEQUENCE {
 *     hashAlgorithm    AlgorithmIdentifier,
 *     hashedMessage    OCTET STRING
 * }
 */
typedef struct OETS_MessageImprint_st {
	X509_ALGOR *hashAlgorithm;
	ASN1_OCTET_STRING *hashedMessage;
} OETS_MessageImprint;

DECLARE_ASN1_FUNCTIONS(OETS_MessageImprint)
OETS_MessageImprint* OETS_MessageImprint_dup(OETS_MessageImprint *str);

/*
 * TimestampRequest ::= SEQUENCE {
 *     version           INTEGER { v1(1) },
 *     messageImprint    MessageImprint,
 *     extensions        Extensions OPTIONAL
 * }
 */
typedef struct OETS_TimestampRequest_st {
	ASN1_INTEGER *version;
	OETS_MessageImprint *messageImprint;
	STACK_OF(X509_EXTENSION) *extensions;
} OETS_TimestampRequest;

DECLARE_ASN1_FUNCTIONS(OETS_TimestampRequest)

/*
 * TBSInformation ::= SEQUENCE {
 *     -- Result of hash chain calculation in the linkingInformation field.
 *     dataHash    MessageImprint,
 *     -- Hash of the signing certificate of the TSA.
 *     certHash    MessageImprint
 * }
 */
typedef struct OETS_TBSInformation_st {
	OETS_MessageImprint *dataHash;
	OETS_MessageImprint *certHash;
} OETS_TBSInformation;

DECLARE_ASN1_FUNCTIONS(OETS_TBSInformation)

/*
 * SignatureInfo ::= SEQUENCE {
 *     tbsInformation        TBSInformation,
 *     signatureAlgorithm    AlgorithmIdentifier,
 *     signatureValue        OCTET STRING
 * }
 */
typedef struct OETS_SignatureInfo_st {
	OETS_TBSInformation *tbsInformation;
	X509_ALGOR *signatureAlgorithm;
	ASN1_OCTET_STRING *signatureValue;
} OETS_SignatureInfo;

DECLARE_ASN1_FUNCTIONS(OETS_SignatureInfo)

/*
 * Accuracy ::= SEQUENCE {
 *     seconds       INTEGER          OPTIONAL,
 *     millis    [0] INTEGER (1..999) OPTIONAL,
 *     micros    [1] INTEGER (1..999) OPTIONAL
 * }
 */
typedef struct OETS_Accuracy_st {
	ASN1_INTEGER *seconds;
	ASN1_INTEGER *millis;
	ASN1_INTEGER *micros;
} OETS_Accuracy;

DECLARE_ASN1_FUNCTIONS(OETS_Accuracy)

/*
 * TimestampInfo ::= SEQUENCE {
 *     stampIdentifier      OCTET STRING,
 *     messageImprint       MessageImprint,
 *     genTime              GeneralizedTime,
 *     accuracy         [0] Accuracy OPTIONAL,
 *     historicalDigests    SEQUENCE OF MessageImprint,
 *     extensions           Extensions OPTIONAL
 * }
 */
typedef struct OETS_TimestampInfo_st {
	ASN1_OCTET_STRING *stampIdentifier;
	OETS_MessageImprint *messageImprint;
	ASN1_GENERALIZEDTIME *genTime;
	OETS_Accuracy *accuracy;
	STACK_OF(OETS_MessageImprint) *historicalDigests;
	STACK_OF(X509_EXTENSION) *extensions;
} OETS_TimestampInfo;

DECLARE_ASN1_FUNCTIONS(OETS_TimestampInfo)

/*
 * HashChain ::= OCTET STRING
 * 
 * LinkedTImestampToken ::= SEQUENCE {
 *     version          INTEGER { v1(1) },
 *     aggregations     HashChain,
 *     timestampInfo    TimestampInfo,
 *     linkingInfo      HashChain,
 *     signatureInfo    SignatureInfo,
 *     extensions       Extensions OPTIONAL
 * }
 */
typedef struct OETS_LinkedTimestampToken {
	ASN1_INTEGER *version;
	ASN1_OCTET_STRING *aggregations;
	OETS_TimestampInfo *timestampInfo;
	ASN1_OCTET_STRING *linkingInfo;
	OETS_SignatureInfo *signatureInfo;
	STACK_OF(X509_EXTENSION) *extensions;
} OETS_LinkedTimestampToken;

DECLARE_ASN1_FUNCTIONS(OETS_LinkedTimestampToken)

/*
 * PublishedData ::= SEQUENCE {
 *     messageImprint   MessageImprint,
 *     stampIdentifier  OCTET STRING
 * }
 */
typedef struct OETS_PublishedData {
	OETS_MessageImprint *messageImprint;
	ASN1_OCTET_STRING *stampIdentifier;
} OETS_PublishedData;

DECLARE_ASN1_FUNCTIONS(OETS_PublishedData);

/*
 * ExtensionRequest ::= SEQUENCE {
 *     oldStamp               LinkedTimestampToken,
 *     newStampIdentifier     OCTET STRING OPTIONAL,
 *     extensions Extensions  OPTIONAL
 * }
 */
typedef struct OETS_ExtensionRequest {
	OETS_LinkedTimestampToken *oldStamp;
	ASN1_OCTET_STRING *newStampIdentifier;
	STACK_OF(X509_EXTENSION) *extensions;
} OETS_ExtensionRequest;

DECLARE_ASN1_FUNCTIONS(OETS_ExtensionRequest);

#ifdef  __cplusplus
}
#endif

#endif /* ! TSPROTO_H_INCLUDED */
