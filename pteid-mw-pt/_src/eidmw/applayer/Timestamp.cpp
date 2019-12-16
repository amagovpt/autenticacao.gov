#include <stdio.h>
#include <string.h>

#include "Timestamp.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/ts.h>
#include <openssl/asn1.h>

// Code inspired by the 'ts' client in OpenSSL

/* 
 * @ca_file is the path to the PEM-encoded Certificate Chain for the Timestamping Authority
 *
 */
int validate_ts_reply(char *timestamp_reply, size_t ts_reply_len, char * queryfile, size_t queryfile_len,
		char *ca_path)
{
	BIO *in_bio = NULL;
	TS_RESP *response = NULL;
	TS_VERIFY_CTX *verify_ctx = NULL;
	int ret = 0;

	/* Decode the token (PKCS7) or response (TS_RESP) files. */
	if (!(in_bio = BIO_new_mem_buf(timestamp_reply, ts_reply_len))) goto end;

	if (!(response = d2i_TS_RESP_bio(in_bio, NULL))) goto end;

	if (!(verify_ctx = create_verify_ctx_from_query(queryfile, queryfile_len,
					ca_path)))
		goto end;

	/* Checking the token or response against the request. */
	ret = TS_RESP_verify_response(verify_ctx, response);

end:
	fprintf (stderr, "Timestamp Verification: ");
	if (ret)
		fprintf(stderr, "OK\n");
	else
	{
		fprintf(stderr, "FAILED\n");
		/* Print errors, if there are any. */
		ERR_print_errors_fp(stderr);
	}

	/* Clean up. */
	BIO_free_all(in_bio);
	TS_RESP_free(response);
	TS_VERIFY_CTX_free(verify_ctx);
	return ret;
}

TS_VERIFY_CTX *create_verify_ctx_from_query(char *queryfile, unsigned int queryfile_len,
		char *ca_file)
{
	TS_VERIFY_CTX *ctx = NULL;
	BIO *input = NULL;
	TS_REQ *request = NULL;
	int ret = 0;

	if (queryfile != NULL)
	{
		/* The request has just to be read, decoded and converted to
		   a verify context object. */
		if (!(input = BIO_new_mem_buf(queryfile, queryfile_len))) goto err;
		if (!(request = d2i_TS_REQ_bio(input, NULL))) goto err;
		if (!(ctx = TS_REQ_to_TS_VERIFY_CTX(request, NULL))) goto err;
	}
	else
		return NULL;

	/* Add the signature verification flag and arguments. */
	ctx->flags |= TS_VFY_SIGNATURE;

	/* Initialising the X509_STORE object. */
	if (!(ctx->store = create_cert_store(ca_file))) goto err;

	ret = 1;
err:
	if (!ret)
	{
		TS_VERIFY_CTX_free(ctx);
		ctx = NULL;
	}
	BIO_free_all(input);
	TS_REQ_free(request);
	return ctx;
}

/*char* get_Timestamp(const unsigned char *timestamp_reply, size_t timestamp_len)
{

	TS_RESP *ts_ptr = NULL;
	struct tm tempo;
	ASN1_GENERALIZEDTIME *time = NULL;
	BUF_MEM **pp;
	//Quick Hack
	BIO *mem_bio = BIO_new(BIO_s_mem());

	if (d2i_TS_RESP(&ts_ptr, &timestamp_reply, timestamp_len) != NULL)
	{

		time = TS_RESP_get_tst_info(ts_ptr)->time;

		//fprintf(stderr, "Timestamp value:\n");
		ASN1_GENERALIZEDTIME_print(mem_bio, time);
		BIO_get_mem_ptr(mem_bio, pp);
	}
	
	//return strdup(*pp);
	return *pp;

}
*/


X509_STORE *create_cert_store(char *ca_file)
{
	X509_STORE *cert_ctx = NULL;
	X509_LOOKUP *lookup = NULL;
	int i;

	/* Creating the X509_STORE object. */
	cert_ctx = X509_STORE_new();

	/* Setting the callback for certificate chain verification. */
	X509_STORE_set_verify_cb(cert_ctx, verify_cb);

	/* Adding a trusted certificate file source. */
	if (ca_file)
	{
		lookup = X509_STORE_add_lookup(cert_ctx, X509_LOOKUP_file());
		if (lookup == NULL)
		{
			fprintf(stderr, "memory allocation failure\n");
			goto err;
		}
		i = X509_LOOKUP_load_file(lookup, ca_file, X509_FILETYPE_PEM);
		if (!i)
		{
			fprintf(stderr, "Error loading file %s\n", ca_file);
			goto err;
		}
	}

	return cert_ctx;
err:
	X509_STORE_free(cert_ctx);
	return NULL;
}

int verify_cb(int ok, X509_STORE_CTX *ctx)
{    
        /*   
	XXX: Should this be commented out?
        char buf[256];

        if (!ok)
                {
                X509_NAME_oneline(X509_get_subject_name(ctx->current_cert),
                                  buf, sizeof(buf));
                printf("%s\n", buf);
                printf("error %d at %d depth lookup: %s\n",
                       ctx->error, ctx->error_depth,
                        X509_verify_cert_error_string(ctx->error));
                }
        */

        return ok;
} 

