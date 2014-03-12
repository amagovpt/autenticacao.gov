/*
 * OpenSSL OCSP client
 *
 */

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/ocsp.h>
#include <openssl/err.h>

#include <stdio.h>
#include <string.h>
#include "ByteArray.h"
#include "static_pteid_certs.h"

namespace eIDMW
{

bool VerifyResponse(OCSP_REQUEST * req, OCSP_RESPONSE *resp);
X509_STORE * setupStore();

CByteArray sendOCSPRequest(X509 *cert, X509* issuer, char *ocsp_url)
{
	OCSP_CERTID *id;
	char *host, *port, *path;
	int is_ssl = 0; 

	OCSP_REQUEST *pRequest = OCSP_REQUEST_new();

	//NULL in first param means use default digest function SHA-1
	id = OCSP_cert_to_id(NULL, cert, issuer);
	OCSP_request_add0_id(pRequest, id);

	//XXX: Why do we need this??
	OCSP_request_add1_nonce(pRequest, 0, -1);

	//Parse url into host, port and path
	OCSP_parse_url(ocsp_url, &host, &port, &path, &is_ssl);

	BIO * ocsp_bio = BIO_new_connect(host);

	BIO_set_conn_port(ocsp_bio, port);

	//TODO: we should introduce a network timeout to properly handle OCSP server downtime
	if (BIO_do_connect(ocsp_bio) <= 0)
	{
			fprintf(stderr, "Error connecting to OCSP host: %s\n", host);
			BIO_free(ocsp_bio);
			return CByteArray();
	}

	OCSP_RESPONSE * resp = OCSP_sendreq_bio(ocsp_bio, path, pRequest);

	if (resp == NULL)
	{
		fprintf(stderr, "Error in OCSP request??\n");
		BIO_free(ocsp_bio);
		return CByteArray();
	}
	else
	{
		if (!VerifyResponse(pRequest, resp))
			return CByteArray();

		int n = i2d_OCSP_RESPONSE(resp, NULL);
		unsigned char *der_tmp = (unsigned char *)OPENSSL_malloc(n);
		//Backup the pointer value because the i2d function modifies its value!
		unsigned char * s = der_tmp;
		i2d_OCSP_RESPONSE(resp, &der_tmp);

		BIO_free(ocsp_bio);
		OCSP_RESPONSE_free(resp);
		return CByteArray(s, n);
	}
}
X509_STORE * setupStore()
{
	X509_STORE *store = X509_STORE_new();
	
	X509 *pCert = NULL;
	unsigned char * cert_data = NULL;
	char *parsing_error = NULL;

	for (unsigned int i = 0; i != CERTS_N; i++)
	{
		pCert = NULL;
		cert_data = PTEID_CERTS[i].cert_data;
	    pCert = d2i_X509(&pCert, (const unsigned char **)&cert_data, 
			PTEID_CERTS[i].cert_len);

		if (pCert == NULL)
		{
			parsing_error = ERR_error_string(ERR_get_error(), NULL);
			fprintf(stderr, "OCSP: Error parsing certificate #%d. Details: %s",
				i, parsing_error);
		}
		else
		{
			if(X509_STORE_add_cert(store, pCert) == 0)
				fprintf(stderr, "OCSP: error adding certificate #%d\n",  i);
		}
	
	}
	return store;
}

bool VerifyResponse(OCSP_REQUEST * req, OCSP_RESPONSE *resp)
{
	int i = 0;
	OCSP_BASICRESP *bs = NULL;

	bs = OCSP_response_get1_basic(resp);

	if (!bs)
	{
		fprintf(stderr, "Error parsing OCSP response\n");
		return false;
	}

	if (req && ((i = OCSP_check_nonce(req, bs)) <= 0))
	{
		if (i == -1)
			fprintf(stderr, "WARNING: no nonce in response\n");
		else
		{
			fprintf(stderr, "Nonce Verify error\n");
			return false; 
		}
	}
	X509_STORE * store = setupStore();

	i = OCSP_basic_verify(bs, NULL, store, 0);
	X509_STORE_free(store);

	if (i <= 0)
	{
		fprintf(stderr,  "OCSP Response Verify Failure\n");
		ERR_print_errors_fp(stderr);
		return false;
	}
	else
	{
		fprintf(stderr, "DEBUG: OCSP Response verify OK\n");
		return true;
	}

}
            
}