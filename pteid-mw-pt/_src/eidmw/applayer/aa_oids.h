#pragma once

namespace eIDMW
{

static const char *SECURITY_OPTION_ALGORITHM_OID = "2.23.136.1.1.5";

// Security Options algorithms used in Active Authentication feature
const static unsigned int BSI_OID_LEN = 12;

static const unsigned char bsiSha1OID[] = {0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x04, 0x01, 0x01};
static const char* bsiSha1SN = "sha1";
static const char* bsiSha1LN = "bsiEcdsaWithSHA1";

static const unsigned char bsiSha224OID[] = {0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x04, 0x01, 0x02};
static const char* bsiSha224SN = "sha224";
static const char* bsiSha224LN = "bsiEcdsaWithSHA224";

static const unsigned char bsiSha256OID[] = {0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x04, 0x01, 0x03};
static const char* bsiSha256SN = "sha256";
static const char* bsiSha256LN = "bsiEcdsaWithSHA256";

static const unsigned char bsiSha384OID[] = {0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x04, 0x01, 0x04};
static const char* bsiSha384SN = "sha384";
static const char* bsiSha384LN = "bsiEcdsaWithSHA384";

static const unsigned char bsiSha512OID[] = {0x06, 0x0A, 0x04, 0x00, 0x7F, 0x00, 0x07, 0x01, 0x01, 0x04, 0x01, 0x05};
static const char* bsiSha512SN = "sha512";
static const char* bsiSha512LN = "bsiEcdsaWithSHA512";

}