/*-****************************************************************************

 * Copyright (C) 2023 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

 ****************************************************************************-*/
/*
 *  PTeid v2 ID File ASN.1 parsing using libcrypto generated functions (macros)
 */

#include <openssl/asn1t.h>
#include <openssl/err.h>
#include <openssl/asn1.h>

#include "Log.h"

#include "asn1_idfile.h"

namespace eIDMW {

ASN1_SEQUENCE(ICC_PUBLICKEY) = {
	ASN1_SIMPLE(ICC_PUBLICKEY, curve_oid, ASN1_OBJECT),	   ASN1_IMP(ICC_PUBLICKEY, pk_p, ASN1_OCTET_STRING, 0x1),
	ASN1_IMP(ICC_PUBLICKEY, pk_a, ASN1_OCTET_STRING, 0x2), ASN1_IMP(ICC_PUBLICKEY, pk_b, ASN1_OCTET_STRING, 0x3),
	ASN1_IMP(ICC_PUBLICKEY, pk_g, ASN1_OCTET_STRING, 0x4), ASN1_IMP(ICC_PUBLICKEY, pk_n, ASN1_OCTET_STRING, 0x5),
	ASN1_IMP(ICC_PUBLICKEY, pk_q, ASN1_OCTET_STRING, 0x6), ASN1_IMP(ICC_PUBLICKEY, pk_cofactor, ASN1_OCTET_STRING, 0x7),

} ASN1_SEQUENCE_END(ICC_PUBLICKEY);

IMPLEMENT_ASN1_FUNCTIONS(ICC_PUBLICKEY);

ASN1_SEQUENCE(IDFILE) = {
	ASN1_IMP_EX(IDFILE, issuing_entity, ASN1_UTF8STRING, 0x0, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, issuing_state, ASN1_UTF8STRING, 0x1, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, document_label, ASN1_UTF8STRING, 0x2, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, document_number, ASN1_UTF8STRING, 0x3, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, document_version, ASN1_UTF8STRING, 0x4, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, issuing_date, ASN1_UTF8STRING, 0x5, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, place_order, ASN1_UTF8STRING, 0x6, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, expire_date, ASN1_UTF8STRING, 0x7, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, surname, ASN1_UTF8STRING, 0x8, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, givenname, ASN1_UTF8STRING, 0x9, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, gender, ASN1_UTF8STRING, 0xA, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, nationality_code, ASN1_UTF8STRING, 0xB, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, date_birth, ASN1_UTF8STRING, 0xC, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, height, ASN1_UTF8STRING, 0xD, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, civil_id, ASN1_UTF8STRING, 0x10, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, father_surname, ASN1_UTF8STRING, 0x11, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, father_givenname, ASN1_UTF8STRING, 0x12, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, mother_surname, ASN1_UTF8STRING, 0x13, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, mother_givenname, ASN1_UTF8STRING, 0x14, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, tax_id, ASN1_UTF8STRING, 0x15, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, socialsec_id, ASN1_UTF8STRING, 0x16, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, health_id, ASN1_UTF8STRING, 0x17, ASN1_TFLG_PRIVATE),
	ASN1_IMP_EX(IDFILE, possible_indications, ASN1_UTF8STRING, 0x18, ASN1_TFLG_PRIVATE | ASN1_TFLG_OPTIONAL),
	ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 73, IDFILE, icc_public_key, ICC_PUBLICKEY)

} ASN1_SEQUENCE_END(IDFILE);

IMPLEMENT_ASN1_FUNCTIONS(IDFILE);

IDFILE *decode_id_data(CByteArray &data) {

	if (data.Size() == 0) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Empty data array!", __FUNCTION__);
		return NULL;
	}

	unsigned char *asn1_data = data.GetBytes();
	if (asn1_data[0] != 0x6D) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Top-level tag is wrong in IDFILE (DG13): %2x!", __FUNCTION__, asn1_data[0]);
	}
	// XX: small hack. Force sequence as top-level tag
	asn1_data[0] = 0x30;

	IDFILE *id_file = d2i_IDFILE(NULL, (const unsigned char **)&asn1_data, data.Size());

	if (id_file == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Failed to decode IDFile structure!", __FUNCTION__);

		unsigned long errCode;
		const char *errString;
		const char *errFile;
		int errLine;
		errCode = ERR_get_error_line_data(&errFile, &errLine, NULL, NULL);
		errString = ERR_error_string(errCode, NULL);
		MWLOG(LEV_ERROR, MOD_APL, "%s: openssl error: %s generated in %s line %d", __FUNCTION__, errString, errFile, errLine);
	} else {
		MWLOG(LEV_DEBUG, MOD_APL, "%s: The decoder consumed %ld bytes", __FUNCTION__, asn1_data - data.GetBytes());

		/* Extract data from ASN1_STRINGs. ASN1_STRING_get0_data returns internal pointer so the returned string will
		not survive after a call to IDFILE_free() printf("Label: %s\n", ASN1_STRING_get0_data(id_file->document_label));
		printf("Doc number: %s\n", ASN1_STRING_get0_data(id_file->document_number));
		printf("Tax ID: %s\n", ASN1_STRING_get0_data(id_file->tax_id)); */
	}

	// undo force sequence as top-level tag so SOD verification can be done successfully
	data.SetByte(0x6D, 0);

	return id_file;
}

} // namespace eIDMW
