/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2017-2024 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 * EF.SOD Parser
 */

#include <algorithm>

#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>

#include "SODParser.h"
#include "eidErrors.h"
#include "Log.h"
#include "MWException.h"

extern "C" {
/* ASN.1 structures and decoder functions for EF.SOD according to ICAO doc 9303 part 10, see appendix D */

typedef struct {
	ASN1_INTEGER *datagroupNumber;
	ASN1_OCTET_STRING *dataGroupHashValue;
} DataGroupHash;

typedef struct {
	ASN1_INTEGER *version;
	X509_ALGOR *hashAlgorithm; // AlgorithmIdentifier is defined as X509_ALGOR in OpenSSL
	STACK_OF(DataGroupHash) * datagroupHashValues;
} LDSSecurityObject;

DEFINE_STACK_OF(DataGroupHash)
ASN1_SEQUENCE(DataGroupHash) = {
	ASN1_SIMPLE(DataGroupHash, datagroupNumber, ASN1_INTEGER),
	ASN1_SIMPLE(DataGroupHash, dataGroupHashValue, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(DataGroupHash)

IMPLEMENT_ASN1_FUNCTIONS(DataGroupHash)

ASN1_SEQUENCE(LDSSecurityObject) = {
	ASN1_SIMPLE(LDSSecurityObject, version, ASN1_INTEGER),
	ASN1_SIMPLE(LDSSecurityObject, hashAlgorithm, X509_ALGOR),
	ASN1_SEQUENCE_OF(LDSSecurityObject, datagroupHashValues, DataGroupHash)
} ASN1_SEQUENCE_END(LDSSecurityObject)

IMPLEMENT_ASN1_FUNCTIONS(LDSSecurityObject)
}

namespace eIDMW {

SODParser::~SODParser() {
	if (attr)
		delete attr;
}

void SODParser::ParseSodEncapsulatedContent(const CByteArray &contents, const std::vector<int> &valid_tags) {

	const unsigned char *p = contents.GetBytes();
	int len = (int)contents.Size();

	LDSSecurityObject *decoded_obj = d2i_LDSSecurityObject(NULL, &p, len);
	if (!decoded_obj) {
		MWLOG(LEV_ERROR, MOD_APL, "Failed to decode SOD LDSSecurityObject!");
		throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ASN1_TAG);
	} else {
		attr = new SODAttributes();

		X509_ALGOR *algorithm_sha256 = X509_ALGOR_new();
		X509_ALGOR_set_md(algorithm_sha256, EVP_sha256());
		if (X509_ALGOR_cmp(decoded_obj->hashAlgorithm, algorithm_sha256) != 0) {
			MWLOG(LEV_ERROR, MOD_APL, "Unexpected digest algorithm in LDSSecurityObject!");
			throw CMWEXCEPTION(EIDMW_SOD_UNEXPECTED_ALGO_OID);
		}

		int num_hashes = sk_DataGroupHash_num(decoded_obj->datagroupHashValues);

		for (int i = 0; i < num_hashes; i++) {
			DataGroupHash *dg_hash = sk_DataGroupHash_value(decoded_obj->datagroupHashValues, i);
			int read_tag = ASN1_INTEGER_get(dg_hash->datagroupNumber);
			MWLOG(LEV_INFO, MOD_APL, "Found datagroupNumber: %d", read_tag);
			
			if (std::find(valid_tags.begin(), valid_tags.end(), read_tag) != valid_tags.end()) {
				attr->add(read_tag, {ASN1_STRING_get0_data(dg_hash->dataGroupHashValue),
									 (unsigned long)ASN1_STRING_length(dg_hash->dataGroupHashValue)});
			} else {
				MWLOG(LEV_INFO, MOD_APL, "SODParser: unexpected datagroup tag: %d", read_tag);
			}
		}
		X509_ALGOR_free(algorithm_sha256);
	}
	
}

SODAttributes &SODParser::getAttributes() { return *attr; }

void SODAttributes::add(unsigned short tag, CByteArray value) { m_hashes[tag] = value; }

const CByteArray &SODAttributes::get(unsigned short tag) { return m_hashes.at(tag); }

const std::unordered_map<unsigned short, CByteArray> &SODAttributes::getHashes() { return m_hashes; }

} /* namespace eIDMW */
