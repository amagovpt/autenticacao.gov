#ifndef ICAODG14_H
#define ICAODG14_H

#include "ByteArray.h"
#include <openssl/asn1.h>
#include <openssl/asn1t.h>
#include <openssl/objects.h>

namespace eIDMW {

struct SecurityInfo_ {
	ASN1_OBJECT *protocol;
	ASN1_TYPE *requiredData;
	ASN1_TYPE *optionalData;
};
typedef SecurityInfo_ SecurityInfo;
struct OID_INFO;

struct SecurityInfos_ {
	STACK_OF(SecurityInfo) * infos;
};
typedef SecurityInfos_ SecurityInfos;
typedef SecurityInfos_ IcaoDg14;

DECLARE_ASN1_FUNCTIONS(SecurityInfo);
DECLARE_ASN1_FUNCTIONS(SecurityInfos);
DEFINE_STACK_OF(SecurityInfo);

SecurityInfos *decodeDg14Data(const CByteArray &data);
ASN1_OBJECT *getSecurityOptionOidByOid(const SecurityInfos &security_infos, const CByteArray &oid);
EVP_PKEY *getChipAuthenticationKey(const SecurityInfos &security_infos);
OID_INFO getChipAuthenticationOid(const SecurityInfos &security_infos);

} // namespace eIDMW
#endif