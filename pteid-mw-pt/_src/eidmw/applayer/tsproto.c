/* 
 * $Id: tsproto.c,v 1.1.1.1 2003/07/03 15:24:54 adminweb Exp $
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
//#include <oe.h>
#include "tsproto.h"
#include <openssl/asn1_mac.h>
#include <openssl/asn1t.h>

ASN1_SEQUENCE(OETS_MessageImprint) = {
	ASN1_SIMPLE(OETS_MessageImprint, hashAlgorithm, X509_ALGOR),
	ASN1_SIMPLE(OETS_MessageImprint, hashedMessage, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(OETS_MessageImprint)

IMPLEMENT_ASN1_FUNCTIONS(OETS_MessageImprint)
IMPLEMENT_ASN1_DUP_FUNCTION(OETS_MessageImprint)

ASN1_SEQUENCE(OETS_TimestampRequest) = {
	ASN1_SIMPLE(OETS_TimestampRequest, version, ASN1_INTEGER),
	ASN1_SIMPLE(OETS_TimestampRequest, messageImprint, OETS_MessageImprint),
	ASN1_SEQUENCE_OF_OPT(OETS_TimestampRequest, extensions, X509_EXTENSION)
} ASN1_SEQUENCE_END(OETS_TimestampRequest)

IMPLEMENT_ASN1_FUNCTIONS(OETS_TimestampRequest)

ASN1_SEQUENCE(OETS_TBSInformation) = {
	ASN1_SIMPLE(OETS_TBSInformation, dataHash, OETS_MessageImprint),
	ASN1_SIMPLE(OETS_TBSInformation, certHash, OETS_MessageImprint)
} ASN1_SEQUENCE_END(OETS_TBSInformation)

IMPLEMENT_ASN1_FUNCTIONS(OETS_TBSInformation)

ASN1_SEQUENCE(OETS_SignatureInfo) = {
	ASN1_SIMPLE(OETS_SignatureInfo, tbsInformation, OETS_TBSInformation),
	ASN1_SIMPLE(OETS_SignatureInfo, signatureAlgorithm, X509_ALGOR),
	ASN1_SIMPLE(OETS_SignatureInfo, signatureValue, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(OETS_SignatureInfo)

IMPLEMENT_ASN1_FUNCTIONS(OETS_SignatureInfo)

ASN1_SEQUENCE(OETS_Accuracy) = {
	ASN1_OPT(OETS_Accuracy, seconds, ASN1_INTEGER),
	ASN1_EXP_OPT(OETS_Accuracy, millis, ASN1_INTEGER, 0),
	ASN1_EXP_OPT(OETS_Accuracy, micros, ASN1_INTEGER, 1)
} ASN1_SEQUENCE_END(OETS_Accuracy)

IMPLEMENT_ASN1_FUNCTIONS(OETS_Accuracy)

ASN1_SEQUENCE(OETS_TimestampInfo) = {
	ASN1_SIMPLE(OETS_TimestampInfo, stampIdentifier, ASN1_OCTET_STRING),
	ASN1_SIMPLE(OETS_TimestampInfo, messageImprint, OETS_MessageImprint),
	ASN1_SIMPLE(OETS_TimestampInfo, genTime, ASN1_GENERALIZEDTIME),
	ASN1_EXP_OPT(OETS_TimestampInfo, accuracy, OETS_Accuracy, 0),
	ASN1_SEQUENCE_OF(OETS_TimestampInfo, historicalDigests,
					 OETS_MessageImprint),
	ASN1_SEQUENCE_OF_OPT(OETS_TimestampInfo, extensions, X509_EXTENSION)
} ASN1_SEQUENCE_END(OETS_TimestampInfo)

IMPLEMENT_ASN1_FUNCTIONS(OETS_TimestampInfo)

ASN1_SEQUENCE(OETS_LinkedTimestampToken) = {
	ASN1_SIMPLE(OETS_LinkedTimestampToken, version, ASN1_INTEGER),
	ASN1_SIMPLE(OETS_LinkedTimestampToken, aggregations, ASN1_OCTET_STRING),
	ASN1_SIMPLE(OETS_LinkedTimestampToken, timestampInfo, OETS_TimestampInfo),
	ASN1_SIMPLE(OETS_LinkedTimestampToken, linkingInfo, ASN1_OCTET_STRING),
	ASN1_SIMPLE(OETS_LinkedTimestampToken, signatureInfo, OETS_SignatureInfo),
	ASN1_SEQUENCE_OF_OPT(OETS_LinkedTimestampToken, extensions, X509_EXTENSION)
} ASN1_SEQUENCE_END(OETS_LinkedTimestampToken)

IMPLEMENT_ASN1_FUNCTIONS(OETS_LinkedTimestampToken)


ASN1_SEQUENCE(OETS_PublishedData) = {
	ASN1_SIMPLE(OETS_PublishedData, messageImprint, OETS_MessageImprint),
	ASN1_SIMPLE(OETS_PublishedData, stampIdentifier, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(OETS_PublishedData)

IMPLEMENT_ASN1_FUNCTIONS(OETS_PublishedData)

ASN1_SEQUENCE(OETS_ExtensionRequest) = {
	ASN1_SIMPLE(OETS_ExtensionRequest, oldStamp, OETS_LinkedTimestampToken),
	ASN1_OPT(OETS_ExtensionRequest, newStampIdentifier, ASN1_OCTET_STRING),
	ASN1_SEQUENCE_OF_OPT(OETS_ExtensionRequest, extensions, X509_EXTENSION)
} ASN1_SEQUENCE_END(OETS_ExtensionRequest)

IMPLEMENT_ASN1_FUNCTIONS(OETS_ExtensionRequest)
