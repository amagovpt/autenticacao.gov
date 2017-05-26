/*
 * OpenSSL OCSP client
 *
 */

#include "APLConfig.h"
#include "APLCertif.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/ocsp.h>
#include <openssl/err.h>

#include <cstdio>
#include <cstring>
#include <string>

#include "MiscUtil.h"
#include "ByteArray.h"

namespace eIDMW
{

//bool VerifyResponse(OCSP_REQUEST * req, OCSP_RESPONSE *resp);
//X509_STORE * setupStore();

X509_STORE * setupStore()
{
	X509_STORE *store = X509_STORE_new();

	X509 *pCert = NULL;
	const unsigned char * cert_data = NULL;
	APL_Certif *certif = NULL;
	APL_Card *card = AppLayer.getReader().getCard();

	APL_SmartCard * eid_card = static_cast<APL_SmartCard *> (card);
	APL_Certifs *certifs = eid_card->getCertificates();

	char *parsing_error = NULL;

	for (unsigned int i = 0; i != certifs->countAll(); i++)
	{
		pCert = NULL;
		certif = certifs->getCert(i);
		cert_data = certif->getData().GetBytes();
	    pCert = d2i_X509(&pCert, &cert_data, certif->getData().Size());

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

CByteArray sendOCSPRequest(X509 *cert, X509* issuer, char *ocsp_url)
{
	OCSP_CERTID *id;
	OCSP_REQ_CTX *ctx = NULL;
	char *host = "", *port, *path;
	int is_ssl = 0;
	BIO * ocsp_bio = NULL;

	APL_Config proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	APL_Config proxy_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
	APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
	APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);

	//TODO: We assume that if proxy_host has a value proxy_port also does which may not be true!!
	if (proxy_host.getString() != NULL && strlen(proxy_host.getString()) > 0)
	{
		host = (char *)proxy_host.getString();
		ocsp_bio = BIO_new_connect(host);
		BIO_set_conn_port(ocsp_bio, proxy_port.getLong());
		path = ocsp_url;
	}
	else
	{
		OCSP_parse_url(ocsp_url, &host, &port, &path, &is_ssl);
		ocsp_bio = BIO_new_connect(host);
		BIO_set_conn_port(ocsp_bio, port);

	}

		//TODO: we should introduce a network timeout to properly handle OCSP server downtime
	if (BIO_do_connect(ocsp_bio) <= 0)
	{
			fprintf(stderr, "Error connecting to OCSP host: %s\n", host);
			BIO_free(ocsp_bio);
			return CByteArray();
	}

	ctx = OCSP_sendreq_new(ocsp_bio, path, NULL, -1);
    if (!ctx)
        return CByteArray();

	OCSP_REQUEST *pRequest = OCSP_REQUEST_new();

	//NULL in first param means use default digest function SHA-1
	id = OCSP_cert_to_id(NULL, cert, issuer);
	OCSP_request_add0_id(pRequest, id);

	//XXX: Why do we need this??
	OCSP_request_add1_nonce(pRequest, 0, -1);

    const char * proxy_user_value = proxy_user.getString();

	if (proxy_user_value != NULL && strlen(proxy_user_value) > 0)
	{
		std::string proxy_cleartext = std::string(proxy_user_value) + ":" + proxy_pwd.getString();

        char *auth_token = Base64Encode((const unsigned char *)proxy_cleartext.c_str(), proxy_cleartext.size());
        OCSP_REQ_CTX_add1_header(ctx, "Proxy-Authorization", auth_token);
		free(auth_token);
	}

	if (!OCSP_REQ_CTX_set1_req(ctx, pRequest))
        return CByteArray();

	OCSP_RESPONSE * resp = NULL;
	/*
	Adapted from the OCSP_sendreq_bio() implementation just to use our custom OCSP_CONTEXT so we can add the
	Authorization header if needed
	*/
	int rv = 0;

	do {
        rv = OCSP_sendreq_nbio(&resp, ctx);
    }
    while ((rv == -1) && BIO_should_retry(ocsp_bio));

	if (resp == NULL)
	{
		fprintf(stderr, "Error in OCSP request??\n");
		BIO_free(ocsp_bio);
		OCSP_REQ_CTX_free(ctx);
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
		OCSP_REQ_CTX_free(ctx);
		OCSP_RESPONSE_free(resp);
		return CByteArray(s, n);
	}
}



}
