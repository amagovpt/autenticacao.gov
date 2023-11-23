/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
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

#include "cert.h"

#include <openssl/x509.h>
#include <openssl/core_names.h>

#define WHERE "cert_get_info()"
int cert_get_info(const unsigned char *pcert, unsigned int lcert, T_CERT_INFO *info) {
    int res = 0;

    memset(info, 0, sizeof(T_CERT_INFO));

    X509 *pX509 = 0;
    pX509 = d2i_X509(&pX509, &pcert, lcert);

    X509_NAME *issuer = X509_get_issuer_name(pX509);
    if(i2d_X509_NAME(issuer, &info->issuer) < 0) {
        log_trace(WHERE, "i2d_X509_NAME() failed reading issuer");
        res = -1;
        goto cleanup;
    }
    
    X509_NAME *subject = X509_get_subject_name(pX509);
    if(i2d_X509_NAME(subject, &info->subject) < 0) {
        log_trace(WHERE, "i2d_X509_NAME() failed reading subject");
        res = -1;
        goto cleanup;
    }

    ASN1_INTEGER *serial = X509_get_serialNumber(pX509);
    if(i2d_ASN1_INTEGER(serial, &info->serial) < 0) {
        log_trace(WHERE, "i2d_ASN1_INTEGER() failed reading serial");
        res = -1;
        goto cleanup;
    }

cleanup:
    X509_free(pX509);
    return res;
}
#undef WHERE

#define WHERE "get_rsa_key_info()"
int get_rsa_key_info(const unsigned char *pcert, unsigned int lcert, T_RSA_KEY_INFO *rsa_keyinfo) {
    int res = 0;
    
    memset(rsa_keyinfo, 0, sizeof(T_RSA_KEY_INFO));

    X509 *pX509 = 0;
    pX509 = d2i_X509(&pX509, &pcert, lcert);

    EVP_PKEY *pubKey = X509_get_pubkey(pX509);

    BIGNUM *exponent = NULL;
    if (!EVP_PKEY_get_bn_param(pubKey, OSSL_PKEY_PARAM_RSA_E, &exponent)) {
		log_trace(WHERE, "Failed to get exponent from key");
		EVP_PKEY_free(pubKey);
        res = -1;
        goto cleanup;
	}

    rsa_keyinfo->l_exp = BN_num_bytes(exponent);
    rsa_keyinfo->exp = OPENSSL_malloc(rsa_keyinfo->l_exp);
    BN_bn2bin(exponent, rsa_keyinfo->exp);

	BIGNUM *n = NULL;
	if (!EVP_PKEY_get_bn_param(pubKey, OSSL_PKEY_PARAM_RSA_N, &n)) {
		log_trace(WHERE, "Failed to get modulus from key");
		EVP_PKEY_free(pubKey);
        res = -1;
        goto cleanup;
	}

	size_t mod_len_bytes = BN_num_bytes(n);
	unsigned char *mod = (unsigned char *) OPENSSL_malloc(mod_len_bytes);
	BN_bn2bin(n, mod);

	rsa_keyinfo->mod = mod;
    rsa_keyinfo->l_mod = mod_len_bytes;

cleanup:
    BN_free(exponent);
	BN_free(n);
	EVP_PKEY_free(pubKey);
    X509_free(pX509);

    return 0;
}
#undef WHERE

#define WHERE "parse_ec_params()"
unsigned char* parse_ec_params(unsigned char* pparams, long *len) {
    unsigned char *params_wrapped = pparams + 6;
    unsigned long params_size = *len - 6;
    params_wrapped[0] = 0x30; // little hack with SEQUENCE tag

    int tag = 0, xclass = 0;
    const unsigned char * op = params_wrapped;
    ASN1_get_object(&op, len, &tag, &xclass, params_size);
    if (tag != V_ASN1_SEQUENCE) {
        log_trace(WHERE, "Expected SEQUENCE tag, got %d", tag);
        return NULL;
    }

    return (unsigned char*)op;
}
#undef WHERE

#define WHERE "parse_ec_point()"
unsigned char* parse_ec_point(unsigned char* ppoint, long *len) {
    unsigned char *wrapped = ppoint + 6;
    unsigned long wrapped_size = *len - 6;
    wrapped[0] = 0x30; // little hack with SEQUENCE tag

    int tag = 0, xclass = 0;
    const unsigned char * op = wrapped;
    ASN1_get_object(&op, len, &tag, &xclass, wrapped_size);
    if (tag != V_ASN1_SEQUENCE) {
        log_trace(WHERE, "Expected SEQUENCE tag, got %d", tag);
        return NULL;
    }

    ASN1_get_object(&op, len, &tag, &xclass, *len);
    if (tag != V_ASN1_OBJECT) {
        log_trace(WHERE, "Expected V_ASN1_OBJECT tag, got %d", tag);
        return NULL;
    }

    return (unsigned char*)op;
}
#undef WHERE
