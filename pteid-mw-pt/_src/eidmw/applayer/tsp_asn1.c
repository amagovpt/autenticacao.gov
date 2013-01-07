/*
 * tsp_asn1.c
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
//#include "oe.h"
#include "tsp.h"
//#include <oe/tasn1_xml.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/x509v3.h>

ASN1_SEQUENCE(TSP_TIME_STAMP_REQ) = {
	ASN1_SIMPLE(TSP_TIME_STAMP_REQ,version,ASN1_INTEGER), 
	ASN1_SIMPLE(TSP_TIME_STAMP_REQ,messageImprint,OETS_MessageImprint),
	ASN1_OPT(TSP_TIME_STAMP_REQ,reqPolicy,ASN1_OBJECT),
        ASN1_OPT(TSP_TIME_STAMP_REQ,nonce,ASN1_INTEGER),
	ASN1_OPT(TSP_TIME_STAMP_REQ,certReq,ASN1_BOOLEAN),
        ASN1_IMP_SEQUENCE_OF_OPT(TSP_TIME_STAMP_REQ,extensions,X509_EXTENSION,0)
} ASN1_SEQUENCE_END(TSP_TIME_STAMP_REQ) ;

IMPLEMENT_ASN1_FUNCTIONS(TSP_TIME_STAMP_REQ)


int TSP_TIME_STAMP_REQ_set_version (TSP_TIME_STAMP_REQ * object, long version ) {
	return ASN1_INTEGER_set(object->version,version); 
}
 
long TSP_TIME_STAMP_REQ_get_version (TSP_TIME_STAMP_REQ * object) {
	return ASN1_INTEGER_get(object->version) ; 
} 


OETS_MessageImprint * TSP_TIME_STAMP_REQ_set_messageImprint (TSP_TIME_STAMP_REQ * object, OETS_MessageImprint * messageImprint ) { 

	if (object->messageImprint == messageImprint) 
		return messageImprint; 

	OETS_MessageImprint_free(object->messageImprint); 
	object->messageImprint = messageImprint; 
	return messageImprint; 

	} 

OETS_MessageImprint *TSP_TIME_STAMP_REQ_get_messageImprint (TSP_TIME_STAMP_REQ * object) { 
	if (object) return object->messageImprint ; 
	return NULL;
} 
OETS_MessageImprint *TSP_TIME_STAMP_REQ_take_messageImprint (TSP_TIME_STAMP_REQ * object) { 
	if (object) { OETS_MessageImprint * x = object->messageImprint ;  object->messageImprint = NULL; return x;}
	return NULL;
} 



ASN1_OBJECT * TSP_TIME_STAMP_REQ_set_reqPolicy (TSP_TIME_STAMP_REQ * object, ASN1_OBJECT * reqPolicy ) { 

	if (object->reqPolicy == reqPolicy) 
		return reqPolicy; 

	ASN1_OBJECT_free(object->reqPolicy); 
	object->reqPolicy = reqPolicy; 
	return reqPolicy; 

	} 

ASN1_OBJECT *TSP_TIME_STAMP_REQ_get_reqPolicy (TSP_TIME_STAMP_REQ * object) { 
	if (object) return object->reqPolicy ; 
	return NULL;
} 
ASN1_OBJECT *TSP_TIME_STAMP_REQ_take_reqPolicy (TSP_TIME_STAMP_REQ * object) { 
	if (object) { ASN1_OBJECT * x = object->reqPolicy ;  object->reqPolicy = NULL; return x;}
	return NULL;
} 



ASN1_INTEGER * TSP_TIME_STAMP_REQ_set_nonce (TSP_TIME_STAMP_REQ * object, ASN1_INTEGER * nonce ) { 

	if (object->nonce == nonce) 
		return nonce; 

	ASN1_INTEGER_free(object->nonce); 
	object->nonce = nonce; 
	return nonce; 

	} 

ASN1_INTEGER *TSP_TIME_STAMP_REQ_get_nonce (TSP_TIME_STAMP_REQ * object) { 
	if (object) return object->nonce ; 
	return NULL;
} 
ASN1_INTEGER *TSP_TIME_STAMP_REQ_take_nonce (TSP_TIME_STAMP_REQ * object) { 
	if (object) { ASN1_INTEGER * x = object->nonce ;  object->nonce = NULL; return x;}
	return NULL;
} 



void TSP_TIME_STAMP_REQ_set_certReq (TSP_TIME_STAMP_REQ * object, int value ) {
	object->certReq = value?255:0 ;
} ;
 
int TSP_TIME_STAMP_REQ_get_certReq (TSP_TIME_STAMP_REQ * object) { 
	return object->certReq ;
} 


STACK_OF(X509_EXTENSION) *TSP_TIME_STAMP_REQ_get_exts(TSP_TIME_STAMP_REQ *a) {
	return a->extensions; 
} 
void TSP_TIME_STAMP_REQ_ext_free(TSP_TIME_STAMP_REQ *a) {

	if (!a) return;
	sk_X509_EXTENSION_pop_free(a->extensions, X509_EXTENSION_free);
	a->extensions = NULL; 

} 

int	TSP_TIME_STAMP_REQ_get_ext_count(TSP_TIME_STAMP_REQ *x) { 
	return X509v3_get_ext_count(x->extensions) ; 
}
 
int	TSP_TIME_STAMP_REQ_get_ext_by_NID(TSP_TIME_STAMP_REQ *x, int nid, int lastpos) { 
	return X509v3_get_ext_by_NID(x->extensions,nid,lastpos) ;
}
 
int	TSP_TIME_STAMP_REQ_get_ext_by_OBJ(TSP_TIME_STAMP_REQ *x, ASN1_OBJECT *obj,int lastpos) { 
	return X509v3_get_ext_by_OBJ(x->extensions,obj,lastpos); 
} 

int	TSP_TIME_STAMP_REQ_get_ext_by_critical(TSP_TIME_STAMP_REQ *x, int crit, int lastpos) { 
	return X509v3_get_ext_by_critical(x->extensions, crit, lastpos);
}
 
X509_EXTENSION *TSP_TIME_STAMP_REQ_get_ext(TSP_TIME_STAMP_REQ *x, int loc) { 
	return X509v3_get_ext(x->extensions,loc) ; 
}  

X509_EXTENSION *TSP_TIME_STAMP_REQ_delete_ext(TSP_TIME_STAMP_REQ *x, int loc) { 
	return X509v3_delete_ext(x->extensions,loc) ; 
} 

int	TSP_TIME_STAMP_REQ_add_ext(TSP_TIME_STAMP_REQ *x, X509_EXTENSION *ex, int loc) { 
	return (X509v3_add_ext(&x->extensions,ex,loc) != NULL); 
} 

void *	TSP_TIME_STAMP_REQ_get_ext_d2i(TSP_TIME_STAMP_REQ *x, int nid, int *crit, int *idx) { 
	return X509V3_get_d2i(x->extensions, nid, crit, idx) ; 
}
 
int	TSP_TIME_STAMP_REQ_add1_ext_i2d(TSP_TIME_STAMP_REQ *x, int nid, void *value, int crit, unsigned long flags) { 
	return X509V3_add1_i2d(&x->extensions, nid, value, crit, flags); 
}
 

ASN1_SEQUENCE(TSP_ACCURACY) = {
	ASN1_OPT(TSP_ACCURACY, seconds, ASN1_INTEGER),
	ASN1_IMP_OPT(TSP_ACCURACY, millis, ASN1_INTEGER, 0),
	ASN1_IMP_OPT(TSP_ACCURACY, micros, ASN1_INTEGER, 1)
} ASN1_SEQUENCE_END(TSP_ACCURACY)

IMPLEMENT_ASN1_FUNCTIONS(TSP_ACCURACY) ;
IMPLEMENT_ASN1_DUP_FUNCTION(TSP_ACCURACY) ;


int TSP_ACCURACY_set_seconds (TSP_ACCURACY * object, long seconds ) {
	return ASN1_INTEGER_set(object->seconds,seconds); 
}
 
long TSP_ACCURACY_get_seconds (TSP_ACCURACY * object) {
	return ASN1_INTEGER_get(object->seconds) ; 
} 


int TSP_ACCURACY_set_millis (TSP_ACCURACY * object, long millis ) {
	return ASN1_INTEGER_set(object->millis,millis); 
}
 
long TSP_ACCURACY_get_millis (TSP_ACCURACY * object) {
	return ASN1_INTEGER_get(object->millis) ; 
} 


int TSP_ACCURACY_set_micros (TSP_ACCURACY * object, long micros ) {
	return ASN1_INTEGER_set(object->micros,micros); 
}
 
long TSP_ACCURACY_get_micros (TSP_ACCURACY * object) {
	return ASN1_INTEGER_get(object->micros) ; 
} 




ASN1_SEQUENCE(TSP_TSTINFO) = {
	ASN1_SIMPLE(TSP_TSTINFO,version,ASN1_INTEGER),
	ASN1_SIMPLE(TSP_TSTINFO,policy,ASN1_OBJECT),
	ASN1_SIMPLE(TSP_TSTINFO,messageImprint,OETS_MessageImprint),
	ASN1_SIMPLE(TSP_TSTINFO,serialNumber,ASN1_INTEGER),
	ASN1_SIMPLE(TSP_TSTINFO,genTime,ASN1_GENERALIZEDTIME),
	ASN1_OPT(TSP_TSTINFO,accuracy,TSP_ACCURACY),
	ASN1_OPT(TSP_TSTINFO,ordering,ASN1_BOOLEAN),
	ASN1_OPT(TSP_TSTINFO,nonce,ASN1_INTEGER),
	ASN1_EXP_OPT(TSP_TSTINFO,tsa,GENERAL_NAME,0),
        ASN1_IMP_SEQUENCE_OF_OPT(TSP_TSTINFO,extensions,X509_EXTENSION,1)
} ASN1_SEQUENCE_END(TSP_TSTINFO) ;

IMPLEMENT_ASN1_FUNCTIONS(TSP_TSTINFO)



int TSP_TSTINFO_set_version (TSP_TSTINFO * object, long version ) {
	return ASN1_INTEGER_set(object->version,version); 
}
 
long TSP_TSTINFO_get_version (TSP_TSTINFO * object) {
	return ASN1_INTEGER_get(object->version) ; 
} 


ASN1_OBJECT * TSP_TSTINFO_set_policy (TSP_TSTINFO * object, ASN1_OBJECT * policy ) { 

	if (object->policy == policy) 
		return policy; 

	ASN1_OBJECT_free(object->policy); 
	object->policy = policy; 
	return policy; 

	} 

ASN1_OBJECT *TSP_TSTINFO_get_policy (TSP_TSTINFO * object) { 
	if (object) return object->policy ; 
	return NULL;
} 
ASN1_OBJECT *TSP_TSTINFO_take_policy (TSP_TSTINFO * object) { 
	if (object) { ASN1_OBJECT * x = object->policy ;  object->policy = NULL; return x;}
	return NULL;
} 



OETS_MessageImprint * TSP_TSTINFO_set_messageImprint (TSP_TSTINFO * object, OETS_MessageImprint * messageImprint ) { 

	if (object->messageImprint == messageImprint) 
		return messageImprint; 

	OETS_MessageImprint_free(object->messageImprint); 
	object->messageImprint = messageImprint; 
	return messageImprint; 

	} 

OETS_MessageImprint *TSP_TSTINFO_get_messageImprint (TSP_TSTINFO * object) { 
	if (object) return object->messageImprint ; 
	return NULL;
} 
OETS_MessageImprint *TSP_TSTINFO_take_messageImprint (TSP_TSTINFO * object) { 
	if (object) { OETS_MessageImprint * x = object->messageImprint ;  object->messageImprint = NULL; return x;}
	return NULL;
} 



ASN1_INTEGER * TSP_TSTINFO_set_serialNumber (TSP_TSTINFO * object, ASN1_INTEGER * serialNumber ) { 

	if (object->serialNumber == serialNumber) 
		return serialNumber; 

	ASN1_INTEGER_free(object->serialNumber); 
	object->serialNumber = serialNumber; 
	return serialNumber; 

	} 

ASN1_INTEGER *TSP_TSTINFO_get_serialNumber (TSP_TSTINFO * object) { 
	if (object) return object->serialNumber ; 
	return NULL;
} 
ASN1_INTEGER *TSP_TSTINFO_take_serialNumber (TSP_TSTINFO * object) { 
	if (object) { ASN1_INTEGER * x = object->serialNumber ;  object->serialNumber = NULL; return x;}
	return NULL;
} 



ASN1_GENERALIZEDTIME * TSP_TSTINFO_set_genTime (TSP_TSTINFO * object, ASN1_GENERALIZEDTIME * genTime ) { 

	if (object->genTime == genTime) 
		return genTime; 

	ASN1_GENERALIZEDTIME_free(object->genTime); 
	object->genTime = genTime; 
	return genTime; 

	} 

ASN1_GENERALIZEDTIME *TSP_TSTINFO_get_genTime (TSP_TSTINFO * object) { 
	if (object) return object->genTime ; 
	return NULL;
} 
ASN1_GENERALIZEDTIME *TSP_TSTINFO_take_genTime (TSP_TSTINFO * object) { 
	if (object) { ASN1_GENERALIZEDTIME * x = object->genTime ;  object->genTime = NULL; return x;}
	return NULL;
} 



TSP_ACCURACY * TSP_TSTINFO_set_accuracy (TSP_TSTINFO * object, TSP_ACCURACY * accuracy ) { 

	if (object->accuracy == accuracy) 
		return accuracy; 

	TSP_ACCURACY_free(object->accuracy); 
	object->accuracy = accuracy; 
	return accuracy; 

	} 

TSP_ACCURACY *TSP_TSTINFO_get_accuracy (TSP_TSTINFO * object) { 
	if (object) return object->accuracy ; 
	return NULL;
} 
TSP_ACCURACY *TSP_TSTINFO_take_accuracy (TSP_TSTINFO * object) { 
	if (object) { TSP_ACCURACY * x = object->accuracy ;  object->accuracy = NULL; return x;}
	return NULL;
} 



void TSP_TSTINFO_set_ordering (TSP_TSTINFO * object, int value ) {
	object->ordering = value?255:0 ;
} ;
 
int TSP_TSTINFO_get_ordering (TSP_TSTINFO * object) { 
	return object->ordering ;
} 


ASN1_INTEGER * TSP_TSTINFO_set_nonce (TSP_TSTINFO * object, ASN1_INTEGER * nonce ) { 

	if (object->nonce == nonce) 
		return nonce; 

	ASN1_INTEGER_free(object->nonce); 
	object->nonce = nonce; 
	return nonce; 

	} 

ASN1_INTEGER *TSP_TSTINFO_get_nonce (TSP_TSTINFO * object) { 
	if (object) return object->nonce ; 
	return NULL;
} 
ASN1_INTEGER *TSP_TSTINFO_take_nonce (TSP_TSTINFO * object) { 
	if (object) { ASN1_INTEGER * x = object->nonce ;  object->nonce = NULL; return x;}
	return NULL;
} 



GENERAL_NAME * TSP_TSTINFO_set_tsa (TSP_TSTINFO * object, GENERAL_NAME * tsa ) { 

	if (object->tsa == tsa) 
		return tsa; 

	GENERAL_NAME_free(object->tsa); 
	object->tsa = tsa; 
	return tsa; 

	} 

GENERAL_NAME *TSP_TSTINFO_get_tsa (TSP_TSTINFO * object) { 
	if (object) return object->tsa ; 
	return NULL;
} 
GENERAL_NAME *TSP_TSTINFO_take_tsa (TSP_TSTINFO * object) { 
	if (object) { GENERAL_NAME * x = object->tsa ;  object->tsa = NULL; return x;}
	return NULL;
} 



STACK_OF(X509_EXTENSION) *TSP_TSTINFO_get_exts(TSP_TSTINFO *a) {
	return a->extensions; 
} 
void TSP_TSTINFO_ext_free(TSP_TSTINFO *a) {

	if (!a) return;
	sk_X509_EXTENSION_pop_free(a->extensions, X509_EXTENSION_free);
	a->extensions = NULL; 

} 

int	TSP_TSTINFO_get_ext_count(TSP_TSTINFO *x) { 
	return X509v3_get_ext_count(x->extensions) ; 
}
 
int	TSP_TSTINFO_get_ext_by_NID(TSP_TSTINFO *x, int nid, int lastpos) { 
	return X509v3_get_ext_by_NID(x->extensions,nid,lastpos) ;
}
 
int	TSP_TSTINFO_get_ext_by_OBJ(TSP_TSTINFO *x, ASN1_OBJECT *obj,int lastpos) { 
	return X509v3_get_ext_by_OBJ(x->extensions,obj,lastpos); 
} 

int	TSP_TSTINFO_get_ext_by_critical(TSP_TSTINFO *x, int crit, int lastpos) { 
	return X509v3_get_ext_by_critical(x->extensions, crit, lastpos);
}
 
X509_EXTENSION *TSP_TSTINFO_get_ext(TSP_TSTINFO *x, int loc) { 
	return X509v3_get_ext(x->extensions,loc) ; 
}  

X509_EXTENSION *TSP_TSTINFO_delete_ext(TSP_TSTINFO *x, int loc) { 
	return X509v3_delete_ext(x->extensions,loc) ; 
} 

int	TSP_TSTINFO_add_ext(TSP_TSTINFO *x, X509_EXTENSION *ex, int loc) { 
	return (X509v3_add_ext(&x->extensions,ex,loc) != NULL); 
} 

void *	TSP_TSTINFO_get_ext_d2i(TSP_TSTINFO *x, int nid, int *crit, int *idx) { 
	return X509V3_get_d2i(x->extensions, nid, crit, idx) ; 
}
 
int	TSP_TSTINFO_add1_ext_i2d(TSP_TSTINFO *x, int nid, void *value, int crit, unsigned long flags) { 
	return X509V3_add1_i2d(&x->extensions, nid, value, crit, flags); 
}
 


ASN1_SEQUENCE(TSP_TIME_STAMP_RESP) = {
	ASN1_SIMPLE(TSP_TIME_STAMP_RESP,status,CMP_PKISTATUS_INFO) ,
	ASN1_OPT(TSP_TIME_STAMP_RESP,timeStampToken,PKCS7)
} ASN1_SEQUENCE_END(TSP_TIME_STAMP_RESP);

IMPLEMENT_ASN1_FUNCTIONS(TSP_TIME_STAMP_RESP)


CMP_PKISTATUS_INFO * TSP_TIME_STAMP_RESP_set_status (TSP_TIME_STAMP_RESP * object, CMP_PKISTATUS_INFO * status ) { 

	if (object->status == status) 
		return status; 

	CMP_PKISTATUS_INFO_free(object->status); 
	object->status = status; 
	return status; 

	} 

CMP_PKISTATUS_INFO *TSP_TIME_STAMP_RESP_get_status (TSP_TIME_STAMP_RESP * object) { 
	if (object) return object->status ; 
	return NULL;
} 
CMP_PKISTATUS_INFO *TSP_TIME_STAMP_RESP_take_status (TSP_TIME_STAMP_RESP * object) { 
	if (object) { CMP_PKISTATUS_INFO * x = object->status ;  object->status = NULL; return x;}
	return NULL;
} 



PKCS7 * TSP_TIME_STAMP_RESP_set_timeStampToken (TSP_TIME_STAMP_RESP * object, PKCS7 * timeStampToken ) { 

	if (object->timeStampToken == timeStampToken) 
		return timeStampToken; 

	PKCS7_free(object->timeStampToken); 
	object->timeStampToken = timeStampToken; 
	return timeStampToken; 

	} 

PKCS7 *TSP_TIME_STAMP_RESP_get_timeStampToken (TSP_TIME_STAMP_RESP * object) { 
	if (object) return object->timeStampToken ; 
	return NULL;
} 
PKCS7 *TSP_TIME_STAMP_RESP_take_timeStampToken (TSP_TIME_STAMP_RESP * object) { 
	if (object) { PKCS7 * x = object->timeStampToken ;  object->timeStampToken = NULL; return x;}
	return NULL;
} 

#define ASN1_ITEM_MAX_PKCS7 20

//Copied from one of the exclude header files: oe/tasn1_xml.h
typedef struct ASN1_item_nid_st
{
				    int nid;
						    ASN1_ITEM_EXP * iptr ;
} ASN1_item_nid;

ASN1_item_nid * encapspkcs7[ASN1_ITEM_MAX_PKCS7+1] = {NULL} ;

int ASN1_item_pkcs7_add(ASN1_item_nid * itemnid) {
  int i=0;
  while(encapspkcs7[i]) i++;

  if (i >= ASN1_ITEM_MAX_PKCS7) return 0;
  encapspkcs7[i++] = itemnid;
  encapspkcs7[i] = NULL ;
  return i ;
}



int TSP_init_library(void) {
        static int init=1;
 
	static ASN1_item_nid tstinfo = {NID_id_smime_ct_TSTInfo, ASN1_ITEM_ref(TSP_TSTINFO)} ;
     if (init) {
             init=0;
		
       if (!ASN1_item_pkcs7_add(&tstinfo)) return 0; 
 
	}
	return 1; 
}

 



