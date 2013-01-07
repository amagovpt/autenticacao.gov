/*
 * cmp.h
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

#ifndef HEADER_CMP_H_
#define HEADER_CMP_H_

#ifdef __cplusplus
extern"C" {
#endif

#include "errorsext.h"

#include <openssl/asn1t.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/safestack.h>

DECLARE_ASN1_DUP_FUNCTION(GENERAL_NAME)


#define CMP_F_ERROR 100
#define CMP_R_ERROR 100

/*  PKIStatusInfo ::= SEQUENCE {
 *      status        PKIStatus,
 *      statusString  PKIFreeText     OPTIONAL,
 *      failInfo      PKIFailureInfo  OPTIONAL
 *  }
 */
/*     PKIFreeText ::= SEQUENCE SIZE (1..MAX) OF UTF8String
 *        -- text encoded as UTF-8 String (note:  each UTF8String SHOULD
 *         -- include an RFC 1766 language tag to indicate the language
 *         -- of the contained text)
 */

typedef struct CMP_PKISTATUS_INFO_st
{
    ASN1_INTEGER *status;
    STACK_OF(ASN1_UTF8STRING) *statusString; 
    ASN1_BIT_STRING *failInfo;

} CMP_PKISTATUS_INFO;



/* 
 *  PKIStatus ::= INTEGER {
 *      accepted                (0),
 *      -- you got exactly what you asked for
 *      grantedWithMods        (1),
 *      -- you got something like what you asked for; the
 *      -- requester is responsible for ascertaining the differences
 *      rejection              (2),
 *      -- you don't get it, more information elsewhere in the message
 *      waiting                (3),
 *      -- the request body part has not yet been processed,
 *      -- expect to hear more later
 *      revocationWarning      (4),
 *      -- this message contains a warning that a revocation is
 *      -- imminent
 *      revocationNotification (5),
 *      -- notification that a revocation has occurred
 *      keyUpdateWarning       (6)
 *      -- update already done for the oldCertId specified in
 *      -- CertReqMsg
 *  }
 */
#define V_CMP_PKISTATUS_INFO_ACCEPTED	0
#define V_CMP_PKISTATUS_INFO_GRANTED_WITH_MODS	1
#define V_CMP_PKISTATUS_INFO_REJECTION	2
#define V_CMP_PKISTATUS_INFO_WAITING	3
#define V_CMP_PKISTATUS_INFO_REVOCATION_WARNING	4
#define V_CMP_PKISTATUS_INFO_REVOCATION_NOTIFICATION	5
#define V_CMP_PKISTATUS_INFO_KEY_UPDATE_WARNING	6


/*  PKIFailureInfo ::= BIT STRING {
 *  -- since we can fail in more than one way!
 *  -- More codes may be added in the future if/when required.
 *      badAlg           (0),
 *      -- unrecognized or unsupported Algorithm Identifier
 *      badMessageCheck  (1),
 *      -- integrity check failed (e.g., signature did not verify)
 *      badRequest       (2),
 *      -- transaction not permitted or supported
 *      badTime          (3),
 *      -- messageTime was not sufficiently close to the system time,
 *      -- as defined by local policy
 *      badCertId        (4),
 *      -- no certificate could be found matching the provided criteria
 *      badDataFormat    (5),
 *      -- the data submitted has the wrong format
 *      wrongAuthority   (6),
 *      -- the authority indicated in the request is different from the
 *      -- one creating the response token
 *      incorrectData    (7),
 *      -- the requester's data is incorrect (for notary services)
 *      missingTimeStamp (8),
 *      -- when the timestamp is missing but should be there (by policy)
 *      badPOP           (9),
 *      -- the proof-of-possession failed
 *      certRevoked         (10),
 *         -- the certificate has already been revoked
 *      certConfirmed       (11),
 *         -- the certificate has already been confirmed
 *      wrongIntegrity      (12),
 *         -- invalid integrity, password based instead of signature or vice
 *         -- versa
 *      badRecipientNonce   (13),
 *         -- invalid recipient nonce, either missing or wrong value
 *      timeNotAvailable    (14),
 *         -- the TSA's time source is not available
 *      unacceptedPolicy    (15),
 *         -- the requested TSA policy is not supported by the TSA.
 *      unacceptedExtension (16),
 *         -- the requested extension is not supported by the TSA. 
 *      addInfoNotAvailable (17),
 *         -- the additional information requested could not be understood
 *         -- or is not available
 *      badSenderNonce      (18),
 *         -- invalid sender nonce, either missing or wrong size
 *      badCertTemplate     (19),
 *         -- invalid certificate template or missing mandatory information
 *      signerNotTrusted    (20),
 *         -- signer of the message unknown or not trusted
 *      transactionIdInUse  (21),
 *         -- the transaction identifier is already in use
 *      unsupportedVersion  (22),
 *         -- the version of the message is not supported  
 *      systemUnavail       (24),
 *         -- the request cannot be handled due to system unavailability
 *      systemFailure       (25),
 *        -- the request cannot be handled due to system failure
 *      duplicateCertReq    (26)
 *        -- certificate cannot be issued because a duplicate certificate
 *       -- already exists
 *  }
 */

#define F_CMP_PKIFAILURE_INFO_BAD_ALG	0
#define F_CMP_PKIFAILURE_INFO_BAD_MESSAGE_CHECK	1
#define F_CMP_PKIFAILURE_INFO_BAD_REQUEST	2
#define F_CMP_PKIFAILURE_INFO_BAD_TIME	3
#define F_CMP_PKIFAILURE_INFO_BAD_CERT_ID	4
#define F_CMP_PKIFAILURE_INFO_BAD_DATA_FORMAT	5
#define F_CMP_PKIFAILURE_INFO_WRONG_AUTHORITY	6
#define F_CMP_PKIFAILURE_INFO_INCORRECT_DATA	7
#define F_CMP_PKIFAILURE_INFO_MISSING_TIME_STAMP	8
#define F_CMP_PKIFAILURE_INFO_BAD_POP	9
#define F_CMP_PKIFAILURE_INFO_CERT_REVOKED	10
#define F_CMP_PKIFAILURE_INFO_CERT_CONFIRMED	11
#define F_CMP_PKIFAILURE_INFO_WRONG_INTEGRITY	12
#define F_CMP_PKIFAILURE_INFO_BAD_RECIPIENT_NONCE	13
#define F_CMP_PKIFAILURE_INFO_TIME_NOT_AVAILABLE	14
#define F_CMP_PKIFAILURE_INFO_UNACCEPTED_POLICY	15
#define F_CMP_PKIFAILURE_INFO_UNACCEPTED_EXTENSION	16
#define F_CMP_PKIFAILURE_INFO_ADD_INFO_NOT_AVAILABLE	17
#define F_CMP_PKIFAILURE_INFO_BAD_SENDER_NONCE	18
#define F_CMP_PKIFAILURE_INFO_BAD_CERT_TEMPLATE	19
#define F_CMP_PKIFAILURE_INFO_SIGNER_NOT_TRUSTED	20
#define F_CMP_PKIFAILURE_INFO_TRANSACTION_ID_IN_USE	21
#define F_CMP_PKIFAILURE_INFO_UNSUPPORTED_VERSION	22
#define F_CMP_PKIFAILURE_INFO_NOT_AUTHORISED	23
#define F_CMP_PKIFAILURE_INFO_SYSTEM_UNAVAIL	24
#define F_CMP_PKIFAILURE_INFO_SYSTEM_FAILURE	25
#define F_CMP_PKIFAILURE_INFO_DUPLICATE_CERT_REQ	26


DECLARE_ASN1_FUNCTIONS(CMP_PKISTATUS_INFO)
DECLARE_ASN1_DUP_FUNCTION(CMP_PKISTATUS_INFO)

DECLARE_STACK_OF(ASN1_UTF8STRING)

#define sk_ASN1_UTF8STRING_new(st) SKM_sk_new(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_new_null() SKM_sk_new_null(ASN1_UTF8STRING)
#define sk_ASN1_UTF8STRING_free(st) SKM_sk_free(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_num(st) SKM_sk_num(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_value(st, i) SKM_sk_value(ASN1_UTF8STRING, (st), (i))
#define sk_ASN1_UTF8STRING_set(st, i, val) SKM_sk_set(ASN1_UTF8STRING, (st), (i), (val))
#define sk_ASN1_UTF8STRING_zero(st) SKM_sk_zero(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_push(st, val) SKM_sk_push(ASN1_UTF8STRING, (st), (val))
#define sk_ASN1_UTF8STRING_unshift(st, val) SKM_sk_unshift(ASN1_UTF8STRING, (st), (val))
#define sk_ASN1_UTF8STRING_find(st, val) SKM_sk_find(ASN1_UTF8STRING, (st), (val))
#define sk_ASN1_UTF8STRING_delete(st, i) SKM_sk_delete(ASN1_UTF8STRING, (st), (i))
#define sk_ASN1_UTF8STRING_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_UTF8STRING, (st), (ptr))
#define sk_ASN1_UTF8STRING_insert(st, val, i) SKM_sk_insert(ASN1_UTF8STRING, (st), (val), (i))
#define sk_ASN1_UTF8STRING_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_UTF8STRING, (st), (cmp))
#define sk_ASN1_UTF8STRING_dup(st) SKM_sk_dup(ASN1_UTF8STRING, st)
#define sk_ASN1_UTF8STRING_pop_free(st, free_func) SKM_sk_pop_free(ASN1_UTF8STRING, (st), (free_func))
#define sk_ASN1_UTF8STRING_shift(st) SKM_sk_shift(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_pop(st) SKM_sk_pop(ASN1_UTF8STRING, (st))
#define sk_ASN1_UTF8STRING_sort(st) SKM_sk_sort(ASN1_UTF8STRING, (st))

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param bp \c (in|ou|inout) - your comments
 * \param x \c (in|ou|inout) - your comments
 * \param indent \c (in|ou|inout) - your comments
 * \return your  comments
 */
int CMP_PKISTATUS_INFO_print (BIO *bp,CMP_PKISTATUS_INFO * x,int indent) ;

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param si \c (in|ou|inout) - your comments
 * \param code \c (in|ou|inout) - your comments
 * \return your  comments
 */
int CMP_PKISTATUS_INFO_add_fail_info(CMP_PKISTATUS_INFO *si, int code) ;

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param si \c (in|ou|inout) - your comments
 * \param status \c (in|ou|inout) - your comments
 * \return your  comments
 */
int CMP_PKISTATUS_INFO_set_status(CMP_PKISTATUS_INFO *si, int status) ;

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param si \c (in|ou|inout) - your comments
 * \return your  comments
 */
int CMP_PKISTATUS_INFO_get_status(CMP_PKISTATUS_INFO *si) ;

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param si \c (in|ou|inout) - your comments
 * \param i \c (in|ou|inout) - your comments
 * \return your  comments
 */
unsigned char * CMP_PKISTATUS_INFO_get_status_string (CMP_PKISTATUS_INFO *si, unsigned int i);

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \param si \c (in|ou|inout) - your comments
 * \param text \c (in|ou|inout) - your comments
 * \return your  comments
 */
int CMP_PKISTATUS_INFO_add_status_string (CMP_PKISTATUS_INFO *si, const char *text);


/* Error codes for the CMP functions. */

/* Function codes. */
#define CMP_F_PKISTATUS_INFO_ADD_FAILURE_INFO	100
#define CMP_F_PKISTATUS_INFO_SET_STATUS		101
#define CMP_F_PKISTATUS_INFO_GET_STATUS_STRING	102
#define CMP_F_PKISTATUS_INFO_ADD_STATUS_STRING  103
/* Reason codes. */

/* Reason codes. */

/**
 * \brief The brief description of the function.
 * here all your comments.
 *
 * \return your  comments
 */
void ERR_load_CMP_strings();

#ifdef __cplusplus
}
#endif

#endif /* HEADER_CMP_H_ */

