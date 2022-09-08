/*-****************************************************************************

 * Copyright (C) 2016-2023 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2018 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 *  HTTPS Client with Client Certificate Authentication for PT-eID Middleware
 *  It provides a CURL handle to be able to perform HTTP requests using the client-authenticated TLS connection
 *
*/
#include "SSLConnection.h"

#include "MWException.h"
#include "eidErrors.h"
#include "cryptoFramework.h"
#include "APLConfig.h"
#include "APLCertif.h"
#include "Log.h"
#include "Config.h"
#include "MiscUtil.h"
#include "CardPteidDef.h"
#include "cJSON.h"

/* Standard headers */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

/* OpenSSL headers */
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rsa.h>

/* libcurl headers */
#include <curl/curl.h>

// select() socket function
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/select.h>
#include <arpa/inet.h>
#endif


namespace eIDMW
{

/* Defines used in libcurl*/
#ifdef WIN32
#define SOCKERRNO         ((int)WSAGetLastError())
#else
#define SOCKERRNO         (errno)
#endif

thread_local static APL_Card * sslconnection_card = NULL;

static void setThreadLocalCardInstance(APL_Card * card)
{
	sslconnection_card = card;
}

// This callback will get passed to OpenSSL and it can't be a member function of SSLConnection because
// it will be called by C code which is not expecting the additional "this" pointer 
// as first argument
static int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, const RSA * rsa)
{
	MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection.rsa_sign() called with type=%d", type);

	CByteArray to_sign(m, m_len);
	CByteArray signed_data;

	try
	{
		//Sign with Authentication Key
		signed_data = sslconnection_card->Sign(to_sign, false, m_len == SHA256_DIGEST_LENGTH);
	}
	catch (CMWException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection->rsa_sign(): Exception caught in card.Sign. Aborting connection");
		fprintf(stderr, "Exception in card.sign() %s:%lu\n",e.GetFile().c_str(), e.GetLine());
		throw e;
	}


	if (signed_data.Size() > 0)
	{
		memcpy(sigret, signed_data.GetBytes(), signed_data.Size());
		*siglen = (unsigned int)signed_data.Size();

		return 1;

	}
	else
		return 0;
}

static unsigned long getKeyLength(APL_Certifs *certs)
{
	APL_Certif *auth_cert = certs->getCert(APL_CERTIF_TYPE_AUTHENTICATION);
	return auth_cert->getKeyLength();
}

static APL_Certif * loadCertsFromCard(SSL_CTX *ctx, APL_Certifs *certs)
{
	APL_Certif *auth_cert = certs->getCert(APL_CERTIF_TYPE_AUTHENTICATION);
	const unsigned char *cert_data = auth_cert->getData().GetBytes();

	MWLOG(LEV_DEBUG, MOD_APL, "Loading from APL_Certifs: cert length= %ld", auth_cert->getData().Size());
	X509 * user_cert = d2i_X509(NULL, &cert_data, auth_cert->getData().Size());

	int ret = SSL_CTX_use_cert_and_key(ctx, user_cert, NULL, NULL, 0);

	if (ret != 1)
	{
		MWLOG(LEV_ERROR, MOD_APL, "SSL_CTX_use_cert_and_key: Error loading auth certificate for SSL handshake! Detail: %s",
			ERR_error_string(ERR_get_error(), NULL));
	}

	return auth_cert;
}

static void loadRootCertsFromCACerts(SSL_CTX *ctx)
{
	APL_Config conf_certsdir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	std::string cacerts_location = std::string(conf_certsdir.getString()) + "/cacerts.pem";

	if (SSL_CTX_load_verify_locations(ctx, cacerts_location.c_str(), NULL) == 0) {
		char *loading_error = ERR_error_string(ERR_get_error(), NULL);
		MWLOG(LEV_ERROR, MOD_APL, "Failed to load cacerts.pem certificates bundle! Error: %s", loading_error);
	}
}

static void loadCertChain(X509_STORE *store, APL_Certif * authentication_cert)
{
	APL_Certif * certif = authentication_cert;
	MWLOG(LEV_DEBUG, MOD_APL, "Establishing TLS connection with auth certificate: %s", certif->getSerialNumber());
	X509 *pCert = NULL;
	
	int i = 0;
	while (!certif->isRoot())
	{
		APL_Certif * issuer = certif->getIssuer();

		if (issuer == NULL)
		{
			break;
			MWLOG(LEV_DEBUG, MOD_APL, "loadCertChain exited early without finding root: incomplete chain");
		}

		MWLOG(LEV_DEBUG, MOD_APL, "loadCertChain: Loading cert: %s", issuer->getOwnerName());
		const unsigned char *cert_data = issuer->getData().GetBytes();
		pCert = d2i_X509(&pCert, &cert_data, issuer->getData().Size());

		if (pCert == NULL)
		{
			char *parsing_error = ERR_error_string(ERR_get_error(), NULL);
			MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection::loadCertChain: Error parsing certificate #%d. Details: %s",
				i, parsing_error);
		}
		else
		{
			if (X509_STORE_add_cert(store, pCert) == 0)
			{
				char *loading_error = ERR_error_string(ERR_get_error(), NULL);
				MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection::loadCertChain: error adding certificate #%d Details: %s",
					i, loading_error);
			}
		}
		pCert = NULL;
		certif = issuer;
		MWLOG(LEV_DEBUG, MOD_APL, "Added certificate with subject: %s", certif->getLabel());
		i++;
	}
}


/**
 * Initialise OpenSSL
 */
static void init_openssl()
{
	OpenSSL_add_all_algorithms();
	/* call the standard SSL init functions */
	SSL_library_init();
	SSL_load_error_strings();

	ERR_load_BIO_strings();
}

SSLConnection::SSLConnection(APL_Card *card)
{
	if (card == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "Failed to create SSLConnection object: null APL_Card pointer");
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

	m_card = card;
	init_openssl();
}

static bool setupInternalSSL(SSL_CTX *ctx, APL_Card *card)
{
	setThreadLocalCardInstance(card);

	APL_SmartCard * eid_card = static_cast<APL_SmartCard *> (card);
	APL_Certifs *certs = eid_card->getCertificates();

    //NOTE: to get more debug output from the SSL layer uncomment this
    //SSL_CTX_set_info_callback(ctx, eIDMW::get_ssl_state_callback);

	
	X509_STORE *store = SSL_CTX_get_cert_store(ctx);

	loadRootCertsFromCACerts(ctx);

    //Change the default RSA_METHOD to use our function for signing
	RSA_METHOD * current_method = (RSA_METHOD *)RSA_get_default_method();

	RSA_meth_set_sign(current_method, eIDMW::rsa_sign);
	RSA_meth_set_flags(current_method, RSA_METHOD_FLAG_NO_CHECK);

	RSA_set_default_method(current_method);


	APL_Certif * cert = loadCertsFromCard(ctx, certs);

	if (!cert) {
		return false;
	}

	//Load cert chain for the current card
	loadCertChain(store, cert);

	return true;
}

static CURLcode sslctxfun(CURL *curl, void *sslctx, void *param) {
	APL_Card *card = (APL_Card *)param;
	SSL_CTX *ctx = (SSL_CTX *)sslctx;

	//Signature algorithms available on all cards
	SSL_CTX_set1_client_sigalgs_list(ctx, "RSA+SHA256:RSA+SHA1");

	return setupInternalSSL(ctx, card) ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;

}


/**
 * Connect to a TLS host using libcurl and our custom openssl code for client authentication
 */
CURL *SSLConnection::connect_encrypted() {
	CURLcode res;
	m_curl = curl_easy_init();

	res = curl_easy_setopt(m_curl, CURLOPT_SSL_CTX_FUNCTION, sslctxfun);

	if (res != CURLE_OK) {
		MWLOG(LEV_ERROR, MOD_APL, "CURLOPT_SSL_CTX_FUNCTION error code: %d\n",
			CURLOPT_SSL_CTX_FUNCTION, res);
	}
	//Card pointer passed to the SSL_CTX setup function
	curl_easy_setopt(m_curl, CURLOPT_SSL_CTX_DATA, m_card);

	return m_curl;
}

}
