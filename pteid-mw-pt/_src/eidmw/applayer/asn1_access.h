/*
 * asn1_access.h
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

/* Note: the IMPLEMENt access function are not actually used and
   may be wrong, instead there is a perl script that expands them */


#include <openssl/err.h>

#define ASN1_OBJECT_dup(x) OBJ_dup(x)
#define ASN1_GENERALIZEDTIME_dup(x) M_ASN1_GENERALIZEDTIME_dup(x)

#define DECLARE_ASN1_DUP_FUNCTION(stname) \
stname * stname##_dup(stname *x) ;


#define IMPLEMENT_ASN1_access(module,type1,type2,field,FIELD) \
type2 * module##_##type1##_set_##field (module##_##type1 * object, type2 * field ) { \
	type2 *temp; \
	if (object->field == field) \
		return 1; \
	temp = type2##_dup(field); \
	if (temp == NULL) { \
		module##err(module##_F_##type1##_SET_##FIELD, ERR_R_MALLOC_FAILURE); \
		return 0; \
		} \
	type2##_free(object->field); \
	object->field = temp; \
	return 1; \
	} \
type2 * module##_##type1##_get_##field (module##_##type1 * object) { return object->field ; } ;


#define IMPLEMENT_ASN1_access_int(module,type1,field) \
int module##_##type1##_set_##field (module##_##type1 * object, long field ) { return ASN1_INTEGER_set(object->field,field); } \
long module##_##type1##_get_##field (module##_##type1 * object) { return ASN1_INTEGER_get(object->field) ; } 

#define IMPLEMENT_ASN1_access_enum(module,type1,field) \
int module##_##type1##_set_##field (module##_##type1 * object, long field ) { return ASN1_ENUMERATED_set(object->field,field); } \
long module##_##type1##_get_##field (module##_##type1 * object) { return ASN1_ENUMERATED_get(object->field) ; } 

#define IMPLEMENT_ASN1_access_bool(module,type1,field,value) \
void module##_##type1##_set_##field (module##_##type1 * object, long field ) { object->field = value?255:0 ;} ; \
int module##_##type1##_get_##field (module##_##type1 * object) { return object->field ; } 



#define DECLARE_ASN1_access(module,type1,type2,field) \
type2 * module##_##type1##_set_##field (module##_##type1 * object, type2 * field ) ;  \
type2 *module##_##type1##_get_##field (module##_##type1 * object) ; \
type2 *module##_##type1##_take_##field (module##_##type1 * object) ;

#define DECLARE_ASN1_access_int(module,type1,field) \
int module##_##type1##_set_##field (module##_##type1 * object, long field ) ; \
long module##_##type1##_get_##field (module##_##type1 * object) ;

#define DECLARE_ASN1_access_enum(module,type1,field) \
int module##_##type1##_set_##field (module##_##type1 * object, long field ) ; \
long module##_##type1##_get_##field (module##_##type1 * object) ;

#define DECLARE_ASN1_access_bool(module,type1,field) \
void module##_##type1##_set_##field (module##_##type1 * object, int value ) ; \
int module##_##type1##_get_##field (module##_##type1 * object) ; 



#define DECLARE_ASN1_access_exts(module,type1,field) \
STACK_OF(X509_EXTENSION) *module##_##type1##_get_exts(module##_##type1 *a) ; \
void module##_##type1##_ext_free(module##_##type1 *a) ; \
int	module##_##type1##_get_ext_count(module##_##type1 *x); \
int	module##_##type1##_get_ext_by_NID(module##_##type1 *x, int nid, int lastpos); \
int	module##_##type1##_get_ext_by_OBJ(module##_##type1 *x,ASN1_OBJECT *obj,int lastpos); \
int	module##_##type1##_get_ext_by_critical(module##_##type1 *x, int crit, int lastpos); \
X509_EXTENSION * module##_##type1##_get_ext(module##_##type1 *x, int loc); \
X509_EXTENSION * module##_##type1##_delete_ext(module##_##type1 *x, int loc); \
int	module##_##type1##_add_ext(module##_##type1 *x, X509_EXTENSION *ex, int loc); \
void *	module##_##type1##_get_ext_d2i(module##_##type1 *x, int nid, int *crit, int *idx); \
int	module##_##type1##_add1_ext_i2d(module##_##type1 *x, int nid, void *value, int crit, unsigned long flags);

#define IMPLEMENT_ASN1_access_exts(module,type1,field) \
STACK_OF(X509_EXTENSION) *module##_##type1##_get_exts(module##_##type1 *a) {return a->field; } \
void module##_##type1##_ext_free(module##_##type1 *a) { if (!a) return; sk_X509_EXTENSION_pop_free(a->field, X509_EXTENSION_free); a->field = NULL; } \
int	module##_##type1##_get_ext_count(module##_##type1 *x) { return X509v3_get_ext_count(x->field) ; } \
int	module##_##type1##_get_ext_by_NID(module##_##type1 *x, int nid, int lastpos) { return X509v3_get_ext_by_NID(x->field,nid,lastpos) ; } \
int	module##_##type1##_get_ext_by_OBJ(module##_##type1 *x, ASN1_OBJECT *obj,int lastpos) { return X509v3_get_ext_by_OBJ(x->field,obj,lastpos); } \
int	module##_##type1##_get_ext_by_critical(module##_##type1 *x, int crit, int lastpos) { return X509v3_get_ext_by_critical(x->field, crit, lastpos);} \
X509_EXTENSION *module##_##type1##_get_ext(module##_##type1 *x, int loc) { return X509v3_get_ext(x->field,loc) ; } \
X509_EXTENSION *module##_##type1##_delete_ext(module##_##type1 *x, int loc) { return X509v3_delete_ext(x->field,loc) ; } \
int	module##_##type1##_add_ext(module##_##type1 *x, X509_EXTENSION *ex, int loc) { return (X509v3_add_ext(&x->field,ex,loc) != NULL); } \
void *	module##_##type1##_get_ext_d2i(module##_##type1 *x, int nid, int *crit, int *idx) { return X509V3_get_d2i(x->field, nid, crit, idx) ; } \
int	module##_##type1##_add1_ext_i2d(module##_##type1 *x, int nid, void *value, int crit, unsigned long flags) { return X509V3_add1_i2d(&x->field, nid, value, crit, flags); } 


#define IMPLEMENT_ASN1_access_seq(module,type1,type2,field,FIELD) \
int module##_##type1##_add_##field (module##_##type1 * object, type2 * field ) { \
	type2 *temp = type2##_dup(field); \
	if (temp == NULL || (!object->field && !(object->field = sk_##type2##_new_null())) || !sk_##type2##_push(object->field,temp)) { \
		module##err(module##_F_##type1##_ADD_##FIELD, ERR_R_MALLOC_FAILURE); return 0; \
	} \
        return 1; \
} 

#define DECLARE_ASN1_access_seq(module,type1,type2,field) \
int module##_##type1##_add_##field (module##_##type1 * object, type2 * field ) ;  \
type2 * module##_##type1##_get_##field (module##_##type1 * object, int i) ;



#define IMPLEMENT_ASN1_access_choice(module,type1,type2,field,FIELD) \
int module##_##type1##_setchoice_##field (module##_##type1 * object, type2 * field ) { \
	type2 *temp; \
	if (object->d.field == field) return object->type; \
	temp = type2##_dup(field); \
	if (temp == NULL) { \
		module##err(module##_F_##type1##_SET_##FIELD, ERR_R_MALLOC_FAILURE); return 0; \
	} \
	type2##_free(object->d.field); \
	object->d.field = (void *) temp; \
	object->type = V_##module##_##type1##_is_##field ;\
	return V_##module##_##type1##_is_##field ; \
} \
type2 *module##_##type1##_getchoice_##field (module##_##type1 * object) { return object->d.field; } ; 

#define DECLARE_ASN1_access_choice(module,type1,type2,field) \
int module##_##type1##_setchoice_##field (module##_##type1 * object, type2 * field ) ;  \
type2 *module##_##type1##_getchoice_##field (module##_##type1 * object) ;


#if 0

	if (object->d.field == field) return object->type; \
	temp = type2##_dup(field); \
	if (temp == NULL) { \
		module##err(module##_F_##type1##_SET_##FIELD, ERR_R_MALLOC_FAILURE); return 0; \
	} \
	type2##_free(object->d.field); \
	object->d.field = (void *) temp; \
	object->type = V_##module##_##type1##_is_##field ;\

#endif
