/*
 * tsp.h
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

#ifndef HEADER_TSP_H_
#define HEADER_TSP_H_

#ifdef __cplusplus
extern"C" {
#endif

#include "errorsext.h"

#include <openssl/asn1t.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/safestack.h>


#include "tsproto.h"
#include "cmp.h"
//#include <oe/ess.h> 



#define TSP_F_ERROR 100
#define TSP_R_ERROR 100



/* TimeStampReq ::= SEQUENCE  {
 *     version                  INTEGER  { v1(1) },
 *     messageImprint           MessageImprint,
 *       --a hash algorithm OID and the hash value of the data to be
 *       --time stamped
 *     reqPolicy                TSAPolicyId                OPTIONAL,
 *     nonce                    INTEGER                    OPTIONAL,
 *     certReq                  BOOLEAN                    DEFAULT FALSE,
 *     extensions               [0] IMPLICIT Extensions    OPTIONAL
 * }
 */
/* TSAPolicyId ::= OBJECT IDENTIFIER
 */ 

typedef struct TSP_TIME_STAMP_REQ_st
{
    ASN1_INTEGER *version;
    OETS_MessageImprint *messageImprint;
    ASN1_OBJECT *reqPolicy;
    ASN1_INTEGER *nonce;
    int certReq; /* default 0 */
    STACK_OF(X509_EXTENSION) *extensions;
} TSP_TIME_STAMP_REQ;

/* TimeStampResp ::= SEQUENCE  {
 *          status                  PKIStatusInfo,
 *          timeStampToken          TimeStampToken     OPTIONAL
 * }
 */
typedef struct TSP_TIME_STAMP_RESP_st
{
    CMP_PKISTATUS_INFO *status;
    PKCS7 *timeStampToken;
} TSP_TIME_STAMP_RESP;


/*
 * Accuracy ::= SEQUENCE {
 *     seconds       INTEGER          OPTIONAL,
 *     millis    [0] INTEGER (1..999) OPTIONAL,
 *     micros    [1] INTEGER (1..999) OPTIONAL
 * }
 */
typedef struct TSP_ACCURACY_st {
	ASN1_INTEGER *seconds;
	ASN1_INTEGER *millis;
	ASN1_INTEGER *micros;
} TSP_ACCURACY;

DECLARE_ASN1_DUP_FUNCTION(TSP_ACCURACY)

/* TSTInfo ::= SEQUENCE  {
 *     version                      INTEGER  { v1(1) },
 *     policy                       TSAPolicyId,
 *     messageImprint               MessageImprint,
 *       -- MUST have the same value as the similar field in 
 *       -- TimeStampReq
 *     serialNumber                 INTEGER,
 *      -- Time Stamps users MUST be ready to accommodate integers 
 *      -- up to 160 bits.
 *     genTime                      GeneralizedTime,
 *     accuracy                     Accuracy                 OPTIONAL,
 *     ordering                     BOOLEAN             DEFAULT FALSE,
 *     nonce                        INTEGER                  OPTIONAL,
 *       -- MUST be present if the similar field was present 
 *       -- in TimeStampReq. In that case it MUST have the same value.
 *     tsa                          [0] GeneralName          OPTIONAL,
 *     extensions                   [1] IMPLICIT Extensions   OPTIONAL
 * }
 */
typedef struct TSP_TSTINFO_st
{
    ASN1_INTEGER *version;
    ASN1_OBJECT *policy;
    OETS_MessageImprint *messageImprint;
    ASN1_INTEGER *serialNumber;
    ASN1_GENERALIZEDTIME *genTime;
    TSP_ACCURACY *accuracy;
    int ordering; /* default 0 */
    ASN1_INTEGER *nonce;
    struct GENERAL_NAME_st *tsa;
    STACK_OF(X509_EXTENSION) *extensions;
} TSP_TSTINFO;

#define TSP_TSTINFO_VERSION_V1 1

DECLARE_ASN1_FUNCTIONS(TSP_TIME_STAMP_REQ)
DECLARE_ASN1_FUNCTIONS(OETS_MessageImprint)
DECLARE_ASN1_FUNCTIONS(TSP_TIME_STAMP_RESP)
DECLARE_ASN1_FUNCTIONS(TSP_TSTINFO)
DECLARE_ASN1_FUNCTIONS(TSP_ACCURACY)

#define d2i_TSP_TIME_STAMP_REQ_bio(bp,p) (TSP_TIME_STAMP_REQ*)ASN1_d2i_bio((char*(*)()) \
		TSP_TIME_STAMP_REQ_new,(char *(*)())d2i_TSP_TIME_STAMP_REQ, (bp),\
		(unsigned char **)(p))

#define d2i_TSP_TIME_STAMP_RESP_bio(bp,p) (TSP_TIME_STAMP_RESP*)ASN1_d2i_bio((char*(*)())\
		TSP_TIME_STAMP_REQ_new,(char *(*)())d2i_TSP_TIME_STAMP_RESP, (bp),\
		(unsigned char **)(p))

#define d2i_TSP_TSTINFO_bio(bp,p) (TSP_TSTINFO*)ASN1_d2i_bio((char*(*)())\
		TSP_TSTINFO_new,(char *(*)())d2i_TSP_TSTINFO, (bp),\
		(unsigned char **)(p))

#define	PEM_read_bio_TSP_TIME_STAMP_REQ(bp,x,cb) (TSP_TIME_STAMP_REQ *)PEM_ASN1_read_bio( \
     (char *(*)())d2i_TSP_TIME_STAMP_REQ,PEM_STRING_TSP_TIME_STAMP_REQ,bp,(char **)x,cb,NULL)

#define	PEM_read_bio_TSP_TIME_STAMP_RESP(bp,x,cb)(TSP_TIME_STAMP_RESP *)PEM_ASN1_read_bio(\
     (char *(*)())d2i_TSP_TIME_STAMP_RESP,PEM_STRING_TSP_TIME_STAMP_RESP,bp,(char **)x,cb,NULL)

#define PEM_write_bio_TSP_TIME_STAMP_REQ(bp,o) \
    PEM_ASN1_write_bio((int (*)())i2d_TSP_TIME_STAMP_REQ,PEM_STRING_TSP_TIME_STAMP_REQ,\
			bp,(char *)o, NULL,NULL,0,NULL,NULL)

#define PEM_write_bio_TSP_TIME_STAMP_RESP(bp,o) \
    PEM_ASN1_write_bio((int (*)())i2d_TSP_TIME_STAMP_RESP,PEM_STRING_TSP_TIME_STAMP_RESP,\
			bp,(char *)o, NULL,NULL,0,NULL,NULL)

#define i2d_TSP_TIME_STAMP_RESP_bio(bp,o) ASN1_i2d_bio(i2d_TSP_TIME_STAMP_RESP,bp,\
		(unsigned char *)o)

#define i2d_TSP_TIME_STAMP_REQ_bio(bp,o) ASN1_i2d_bio(i2d_TSP_TIME_STAMP_REQ,bp,\
		(unsigned char *)o)

TSP_TIME_STAMP_REQ * TSP_request(char * digest, int digestlength, int nid) ;

#define TSP_INPUT_MIME_TYPE "application/timestamp-query"
#define TSP_OUTPUT_MIME_TYPE "application/timestamp-reply"

DECLARE_ASN1_access_int(TSP,TIME_STAMP_REQ,version) 
DECLARE_ASN1_access(TSP,TIME_STAMP_REQ,OETS_MessageImprint,messageImprint) 
DECLARE_ASN1_access(TSP,TIME_STAMP_REQ,ASN1_OBJECT,reqPolicy) 
DECLARE_ASN1_access(TSP,TIME_STAMP_REQ,ASN1_INTEGER,nonce) 
DECLARE_ASN1_access_bool(TSP,TIME_STAMP_REQ,certReq) 
DECLARE_ASN1_access_exts(TSP,TIME_STAMP_REQ,extensions) 


DECLARE_ASN1_access(TSP,TIME_STAMP_RESP,CMP_PKISTATUS_INFO,status) 
DECLARE_ASN1_access(TSP,TIME_STAMP_RESP,PKCS7,timeStampToken)

DECLARE_ASN1_access_int(TSP,TSTINFO,version) 
DECLARE_ASN1_access(TSP,TSTINFO,ASN1_OBJECT,policy) 
DECLARE_ASN1_access(TSP,TSTINFO,OETS_MessageImprint,messageImprint) 
DECLARE_ASN1_access(TSP,TSTINFO,ASN1_INTEGER,serialNumber) 
DECLARE_ASN1_access(TSP,TSTINFO,ASN1_GENERALIZEDTIME,genTime) 
DECLARE_ASN1_access(TSP,TSTINFO,TSP_ACCURACY,accuracy)
DECLARE_ASN1_access_bool(TSP,TSTINFO,ordering) 
DECLARE_ASN1_access(TSP,TSTINFO,ASN1_INTEGER,nonce) 
DECLARE_ASN1_access(TSP,TSTINFO,GENERAL_NAME,tsa) 
DECLARE_ASN1_access_exts(TSP,TSTINFO,extensions) 

DECLARE_ASN1_access_int(TSP,ACCURACY,seconds) 
DECLARE_ASN1_access_int(TSP,ACCURACY,millis) 
DECLARE_ASN1_access_int(TSP,ACCURACY,micros) 


BIO * TSP_connect(BIO * error_bio, char * hostport,const int ssl,EVP_PKEY * pkey, X509 * cert, STACK_OF(X509) * ca ) ;

TSP_TIME_STAMP_RESP *TSP_sendreq_bio(BIO *b, char *path, TSP_TIME_STAMP_REQ *req) ;


int TSP_init_library(void) ;

TSP_TIME_STAMP_REQ *TSP_TIME_STAMP_REQ_create(TSP_TIME_STAMP_REQ * oldRequest, OETS_MessageImprint * messageImprint, ASN1_INTEGER * nonce, ASN1_OBJECT *policyOID, int certReq) ;

int TSP_TIME_STAMP_REQ_tst(TSP_TIME_STAMP_REQ * tspRequest, OETS_MessageImprint *messageImprint, ASN1_INTEGER *nonce, ASN1_OBJECT *policyOID, int certReq) ;

int TSP_TIME_STAMP_RESP_tst(TSP_TSTINFO * tspResponse, OETS_MessageImprint *messageImprint, ASN1_INTEGER * nonce, ASN1_OBJECT *policyOID) ;



void ERR_load_TSP_strings();

/* BEGIN ERROR CODES */
/* The following lines are auto generated by the script mkerr.pl. Any changes
 * made after this point may be overwritten when the script is next run.
 */

/* Error codes for the TSP functions. */

/* Function codes. */
#define TSP_F_TSP_CONNECT				101
#define TSP_F_TSP_SENDREQ_BIO				102
#define TSP_F_MESSAGE_IMPRINT_SET_HASH_ALGORITHM	103 
#define TSP_F_MESSAGE_IMPRINT_SET_HASHED_MESSAGE	104 
#define TSP_F_TIME_STAMP_REQ_SET_MESSAGE_IMPRINT	105
#define TSP_F_TIME_STAMP_REQ_SET_REQ_POLICY	106
#define TSP_F_TIME_STAMP_REQ_SET_NONCE	107
#define TSP_F_TSTINFO_SET_MESSAGE_IMPRINT	108

#define TSP_F_ACCURACY_SET_MICROS	109 
#define TSP_F_ACCURACY_SET_MILLIS	110 
#define TSP_F_ACCURACY_SET_SECONDS	111 
#define TSP_F_TSTINFO_SET_POLICY	112
#define TSP_F_TSTINFO_SET_SERIAL_NUMBER	113 
#define TSP_F_TSTINFO_SET_GEN_TIME	114 
#define TSP_F_TSTINFO_SET_ACCURACY	115 
#define TSP_F_TSTINFO_SET_NONCE	116 
#define TSP_F_TSTINFO_SET_TSA	117

#define TSP_F_TIME_STAMP_RESP_SET_STATUS	118
#define TSP_F_TIME_STAMP_RESP_SET_TIME_STAMP_TOKEN	119

/* Reason codes. */
#define TSP_R_NO_CONTENT                       		101
#define TSP_R_SERVER_READ_ERROR				102
#define TSP_R_SERVER_RESPONSE_ERROR			103           
#define TSP_R_SERVER_RESPONSE_PARSE_ERROR		104	
#define TSP_R_SERVER_WRITE_ERROR			105 
#define TSP_R_CONNECT_NO_BIO				106
#define TSP_R_CONNECT_BAD_USER_CERT           		107
#define TSP_R_CONNECT_BAD_USER_KEY            		108
#define TSP_R_CONNECT_INVALID_USER_KEY			109
#define TSP_R_CONNECT_INVALID_CERTSTORE			110
#define TSP_R_CONNECT_NO_TSA				111


#ifdef __cplusplus
}
#endif

#endif /* HEADER_TSP_H_ */

