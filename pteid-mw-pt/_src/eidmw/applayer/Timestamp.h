// Needs OpenSSL 1.0 or a patched older version
// Patches available at http://opentsa.org 
#include <openssl/ts.h>
#include <openssl/asn1.h>

#ifndef TIMESTAMP_H_ 
#define TIMESTAMP_H_ 

int validate_ts_reply (char *timestamp_reply, size_t ts_reply_len, char * queryfile, size_t queryfile_len, char *ca_path);

TS_VERIFY_CTX *create_verify_ctx_from_query (char *queryfile, unsigned int queryfile_len, char *ca_path);
TS_VERIFY_CTX *create_verify_ctx_from_hash (char *hash_value, unsigned int hash_len, char *ca_path);

ASN1_GENERALIZEDTIME* get_Timestamp(const unsigned char * timestamp_reply, size_t timestamp_len);

//XMLCh *locateTimestamp(DOMDocument *doc);

X509_STORE *create_cert_store(char *ca_file);

int verify_cb(int ok, X509_STORE_CTX *ctx);

#endif //Include guard
