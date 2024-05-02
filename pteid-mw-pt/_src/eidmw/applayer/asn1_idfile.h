/*-****************************************************************************

 * Copyright (C) 2023 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

 ****************************************************************************-*/
/*
 *  PTeid v2 ID File ASN.1 parsing using libcrypto generated functions (macros)
 */

#pragma once

#include <openssl/asn1t.h>
#include <openssl/err.h>
#include <openssl/asn1.h>
#include "ByteArray.h"

namespace eIDMW {

struct ICCPublicKey_st {
	ASN1_OBJECT *curve_oid;
	ASN1_STRING *pk_p;
	ASN1_STRING *pk_a;
	ASN1_STRING *pk_b;
	ASN1_STRING *pk_g;
	ASN1_STRING *pk_n;
	ASN1_STRING *pk_q;
	ASN1_STRING *pk_cofactor;
};

typedef struct ICCPublicKey_st ICC_PUBLICKEY;

struct IDFile_st {
	ASN1_UTF8STRING *issuing_entity;
	ASN1_UTF8STRING *issuing_state;
	ASN1_UTF8STRING *document_label;
	ASN1_UTF8STRING *document_number;
	ASN1_UTF8STRING *document_version;
	ASN1_UTF8STRING *issuing_date;
	ASN1_UTF8STRING *place_order;
	ASN1_UTF8STRING *expire_date;
	ASN1_UTF8STRING *surname;
	ASN1_UTF8STRING *givenname;
	ASN1_UTF8STRING *gender;
	ASN1_UTF8STRING *nationality_code;
	ASN1_UTF8STRING *date_birth;
	ASN1_UTF8STRING *height;
	ASN1_UTF8STRING *civil_id;
	ASN1_UTF8STRING *father_surname;
	ASN1_UTF8STRING *father_givenname;
	ASN1_UTF8STRING *mother_surname;
	ASN1_UTF8STRING *mother_givenname;
	ASN1_UTF8STRING *tax_id;
	ASN1_UTF8STRING *socialsec_id;
	ASN1_UTF8STRING *health_id;
	ASN1_UTF8STRING *possible_indications;
	ICC_PUBLICKEY *icc_public_key;
	// ASN1_OCTET_STRING * public_key;
};

typedef struct IDFile_st IDFILE;

DECLARE_ASN1_FUNCTIONS(ICC_PUBLICKEY)

/* Declare ASN_1 handling functions for IDFILE */
DECLARE_ASN1_FUNCTIONS(IDFILE)

// Return a structure containing all fields of the PTeID V2 ID File (DG.13)
IDFILE *decode_id_data(CByteArray &idfile_data);

} // namespace eIDMW
