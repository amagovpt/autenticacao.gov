#include "IcaoDg14.h"

#include "Log.h"
#include "aa_oids.h"
#include "PaceAuthentication.h"

#if defined(__WINCRYPT_H__)
#undef X509_NAME
#undef X509_EXTENSIONS
#undef X509_CERT_PAIR
#undef PKCS7_ISSUER_AND_SERIAL
#undef PKCS7_SIGNER_INFO
#undef OCSP_REQUEST
#undef OCSP_RESPONSE
#endif

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>

namespace eIDMW {

ASN1_SEQUENCE(SecurityInfo) = {ASN1_SIMPLE(SecurityInfo, protocol, ASN1_OBJECT),
							   ASN1_SIMPLE(SecurityInfo, requiredData, ASN1_ANY),
							   ASN1_OPT(SecurityInfo, optionalData, ASN1_ANY)} ASN1_SEQUENCE_END(SecurityInfo);

IMPLEMENT_ASN1_FUNCTIONS(SecurityInfo);

ASN1_SEQUENCE(SecurityInfos) = {ASN1_SET_OF(SecurityInfos, infos, SecurityInfo)} ASN1_SEQUENCE_END(SecurityInfos);

IMPLEMENT_ASN1_FUNCTIONS(SecurityInfos);

SecurityInfos *decodeDg14Data(const CByteArray &data) {
	if (data.Size() == 0) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Empty data array!", __FUNCTION__);
		return NULL;
	}

	// hack
	CByteArray data_copy = data;
	unsigned char *asn1_data = data_copy.GetBytes();
	if (asn1_data[0] != 0x6E) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Top-level tag is wrong in IDFILE (DG14): %2x!", __FUNCTION__, asn1_data[0]);
	}
	asn1_data[0] = 0x30;

	SecurityInfos *security_infos = d2i_SecurityInfos(nullptr, (const unsigned char **)&asn1_data, data.Size());
	if (security_infos == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Failed to decode DG14 Security Options structure!", __FUNCTION__);

		unsigned long errCode;
		const char *errString;
		const char *errFile;
		int errLine;
		errCode = ERR_get_error_line_data(&errFile, &errLine, NULL, NULL);
		errString = ERR_error_string(errCode, NULL);
		MWLOG(LEV_ERROR, MOD_APL, "%s: openssl error: %s generated in %s line %d", errString, errFile, errLine);
	} else {
		MWLOG(LEV_DEBUG, MOD_APL, "%s: The decoder consumed %ld bytes", __FUNCTION__, asn1_data - data_copy.GetBytes());
	}

	return security_infos;
}

ASN1_OBJECT *getSecurityOptionOidByOid(const CByteArray &dg14_file, const CByteArray &oid) {
	auto security_infos = decodeDg14Data(dg14_file);

	ASN1_OBJECT *oid_ret = nullptr;

	size_t security_infos_n = sk_SecurityInfo_num(security_infos->infos);
	for (size_t i = 0; i < security_infos_n; i++) {
		auto info = sk_SecurityInfo_value(security_infos->infos, i);

		// TODO: what is max OID size?
		char obj_buff[255];
		OBJ_obj2txt(obj_buff, sizeof(obj_buff), info->protocol, 1);

		if (memcmp(obj_buff, oid.GetBytes(), oid.Size()) == 0) {
			if (info->optionalData->type == V_ASN1_OBJECT) {
				oid_ret = OBJ_dup(info->optionalData->value.object);
			}
		}
	}

	SecurityInfos_free(security_infos);

	return oid_ret;
}

EVP_PKEY *getChipAuthenticationKey(const CByteArray &dg14_file) {
	EVP_PKEY *pkey = nullptr;

	auto security_infos = decodeDg14Data(dg14_file);

	ASN1_OBJECT *oid_ret = nullptr;
	CByteArray oid = {CHIP_AUTHENTICATION_PUBKEY_OID};

	size_t security_infos_n = sk_SecurityInfo_num(security_infos->infos);
	for (size_t i = 0; i < security_infos_n; i++) {
		auto info = sk_SecurityInfo_value(security_infos->infos, i);

		// TODO: what is max OID size?
		char obj_buff[255];
		OBJ_obj2txt(obj_buff, sizeof(obj_buff), info->protocol, 1);

		if (memcmp(obj_buff, oid.GetBytes(), oid.Size()) == 0) {
			// create evp key from required data sequence
			if (info->requiredData->type == V_ASN1_SEQUENCE) {
				size_t len = ASN1_STRING_length(info->requiredData->value.sequence);
				const unsigned char *p = ASN1_STRING_get0_data(info->requiredData->value.sequence);
				pkey = d2i_PUBKEY(NULL, &p, len);
			}
		}
	}

	SecurityInfos_free(security_infos);
	return pkey;
}

OID_INFO getChipAuthenticationOid(const CByteArray &dg14_file) {
	// List of supported NIDs for chip authentication
	static OID_INFO CA_OIDS[] = {get_id_CA_ECDH_3DES_CBC_CBC(),		get_id_CA_ECDH_AES_CBC_CMAC_128(),
								 get_id_CA_ECDH_AES_CBC_CMAC_192(), get_id_CA_ECDH_AES_CBC_CMAC_256(),
								 get_id_CA_DH_3DES_CBC_CBC(),		get_id_CA_DH_AES_CBC_CMAC_128(),
								 get_id_CA_DH_AES_CBC_CMAC_192(),	get_id_CA_DH_AES_CBC_CMAC_256()};

	auto security_infos = decodeDg14Data(dg14_file);
	if (!security_infos) {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Failed to decode DG14 Security Options structure!", __FUNCTION__);
		return {nullptr, 0, nullptr};
	}

	OID_INFO found_oid = {};

	size_t security_infos_n = sk_SecurityInfo_num(security_infos->infos);
	for (size_t i = 0; i < security_infos_n; i++) {
		auto info = sk_SecurityInfo_value(security_infos->infos, i);

		for (auto std_oid : CA_OIDS) {
			if (std_oid.object && OBJ_cmp(info->protocol, std_oid.object) == 0) {
				found_oid = std_oid;
				found_oid.object = OBJ_dup(info->protocol);

				ASN1_OBJECT_free(std_oid.object);
				break;
			}

			if (std_oid.object) {
				ASN1_OBJECT_free(std_oid.object);
			}
		}

		if (found_oid.object) {
			break;
		}
	}

	SecurityInfos_free(security_infos);
	return found_oid;
}

} // namespace eIDMW
