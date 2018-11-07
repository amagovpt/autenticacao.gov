/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2011 - 2017
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *
 *  HTTPS Client with Client Certificate Authentication for PT-eID Middleware
 *  It provides the communication protocol to the Address Change WebServices
 *
*/
#include "SSLConnection.h"

#include "MWException.h"
#include "eidErrors.h"
#include "cryptoFramework.h"
#include "APLConfig.h"
#include "APLCertif.h"
#include "Log.h"
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
#include <openssl/ssl.h>
#include <openssl/err.h>


#ifdef WIN32
#define snprintf _snprintf
#endif

namespace eIDMW
{


/* Defines used in libcurl*/
#ifdef WIN32
#define SOCKERRNO         ((int)WSAGetLastError())
#else
#define SOCKERRNO         (errno)
#endif

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, const RSA * rsa)
{

	APL_Card *card = AppLayer.getReader().getCard();

	if (card == NULL) {
	    fprintf(stderr, "rsa_sign(): Failed to get card from global aplayer object\n");
	    return 0;
    }

	if (type != NID_md5_sha1)
	{
		fprintf(stderr, "rsa_sign(): Called with wrong input type, it should be NID_md5_sha1!\n");
		return 0;
	}

	CByteArray to_sign(m, m_len);
	CByteArray signed_data;

	try
	{
		//Sign with Authentication Key
		signed_data = card->Sign(to_sign, false, false);
	}
	catch (CMWException &e)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection->rsa_sign(): Exception caught in card.Sign. Aborting connection");
		fprintf(stderr, "Exception in card.sign() %s:%lu\n",e.GetFile().c_str(), e.GetLine());
		return 0;
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


APL_Certif * SSLConnection::loadCertsFromCard(SSL_CTX *ctx)
{

	APL_Certif *auth_cert = m_certs->getCert(APL_CERTIF_TYPE_AUTHENTICATION);

	MWLOG(LEV_DEBUG, MOD_APL, "Loading from APL_Certifs -> cert length= %ld", auth_cert->getData().Size());
	int ret = SSL_CTX_use_certificate_ASN1(ctx, auth_cert->getData().Size(), auth_cert->getData().GetBytes());

	if (ret != 1)
	{
		MWLOG(LEV_ERROR, MOD_APL, "Error loading Auth certificate for SSL handshake! Detail: %s",
			ERR_error_string(ERR_get_error(), NULL));
	}

	return auth_cert;
}

void SSLConnection::loadAllRootCerts(X509_STORE *store)
{
	X509 * pCert = NULL;
	for (int i=0; i != m_certs->countAll(); i++)
	{
		APL_Certif * cert = m_certs->getCert(i);
		if (cert != NULL && cert->isRoot())
		{
			MWLOG(LEV_DEBUG, MOD_APL, "loadAllRootCerts: Loading cert: %s", cert->getOwnerName());
			const unsigned char *cert_data = cert->getData().GetBytes();
		    pCert = d2i_X509(&pCert, &cert_data, cert->getData().Size());

		    if (pCert == NULL)
			{
				char *parsing_error = ERR_error_string(ERR_get_error(), NULL);
				MWLOG(LEV_ERROR, MOD_APL, "SSLConnection::loadAllRootCerts: Error parsing certificate #%d. Details: %s",
					i, parsing_error);
			}
			else
			{
				if (X509_STORE_add_cert(store, pCert) == 0)
				{
					char *loading_error = ERR_error_string(ERR_get_error(), NULL);
					MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection::loadAllRootCerts: error adding certificate #%d Details: %s",
					  i, loading_error);
				}
			}
			pCert = NULL;
		}

	}
}

void SSLConnection::loadCertChain(X509_STORE *store, APL_Certif * authentication_cert)
{

	APL_Certif * certif = authentication_cert;
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
		i++;
	}

}


#if 0
void get_ssl_state_callback(const SSL *s, int where, int ret)
{
	const char *str;
	int w;

	w=where& ~SSL_ST_MASK;

	if (w & SSL_ST_CONNECT) str="SSL_connect";
	else if (w & SSL_ST_ACCEPT) str="SSL_accept";
	else str="undefined";

	if (where & SSL_CB_LOOP)
	{
		fprintf(stderr,"%s:%s\n",str,SSL_state_string_long(s));
	}
	else if (where & SSL_CB_ALERT)
	{
		str=(where & SSL_CB_READ)?"read":"write";
		fprintf(stderr,"SSL3 alert %s: %s:%s\n",
				str,
				SSL_alert_type_string_long(ret),
				SSL_alert_desc_string_long(ret));
	}
	else if (where & SSL_CB_EXIT)
	{
		if (ret == 0)
			fprintf(stderr, "%s: failed in %s\n",
					str,SSL_state_string_long(s));
		else if (ret < 0)
		{
			fprintf(stderr, "%s: error in %s\n",
					str,SSL_state_string_long(s));
		}
	}
}
#endif


//Translate the string via specific OpenSSL error codes
//is not feasible AFAICT
unsigned int translate_openssl_error(unsigned int error)
{

	char *error_string = ERR_error_string(error, NULL);

	if (strstr(error_string, "Connection refused") != NULL
	|| strstr(error_string, "bad hostname lookup") != NULL)
		return EIDMW_OTP_CONNECTION_ERROR;

	else if (strstr(error_string, "certificate verify failed") != NULL)
		return EIDMW_OTP_CERTIFICATE_ERROR;
	else
		return EIDMW_OTP_UNKNOWN_ERROR;

}

static const char *SSL_ERROR_to_str(int err)
{
	switch (err) {
	case SSL_ERROR_NONE:
		return "SSL_ERROR_NONE";
	case SSL_ERROR_SSL:
		return "SSL_ERROR_SSL";
	case SSL_ERROR_WANT_READ:
		return "SSL_ERROR_WANT_READ";
	case SSL_ERROR_WANT_WRITE:
		return "SSL_ERROR_WANT_WRITE";
	case SSL_ERROR_WANT_X509_LOOKUP:
		return "SSL_ERROR_WANT_X509_LOOKUP";
	case SSL_ERROR_SYSCALL:
		return "SSL_ERROR_SYSCALL";
	case SSL_ERROR_ZERO_RETURN:
		return "SSL_ERROR_ZERO_RETURN";
	case SSL_ERROR_WANT_CONNECT:
		return "SSL_ERROR_WANT_CONNECT";
	case SSL_ERROR_WANT_ACCEPT:
		return "SSL_ERROR_WANT_ACCEPT";

	default:
		return "SSL_ERROR unknown";
	}
}


/**
 * Initialise OpenSSL
 */
void SSLConnection::init_openssl() {

    OpenSSL_add_all_algorithms();
    /* call the standard SSL init functions */
    SSL_library_init();
    SSL_load_error_strings();

    ERR_load_BIO_strings();


}

/**
 * Close an encrypted connection gracefully
 */
void SSLConnection::CloseConnection() {

	if (m_ssl_connection != NULL)
    	SSL_free(m_ssl_connection);

}

unsigned long parseContentLength(char * headers)
{
	unsigned long content_length = 0;
	char * line = NULL;

	line = strtok(headers, "\r\n");
	while (line != NULL)
	{
		int ret = sscanf(line, "Content-Length: %lu", &content_length);
		if (ret > 0 && ret != EOF)
			break;
		line = strtok(NULL, "\r\n");

	}

	return content_length;
}

char *skipHTTPHeaders(char *http_reply)
{
	char *needle = strstr(http_reply, "\r\n\r\n");
	if (needle)
	{
		return needle + 4;
	}
	else
	{
		fprintf(stderr, "Malformed HTTP reply!:\n%s\n", http_reply);
		return NULL;
	}
}

char *parseCookie(char *server_response)
{

	char *substr = NULL;
	substr = strstr(server_response, "JSESSIONID=");
	if (substr == NULL)
	{
	    MWLOG(LEV_ERROR, MOD_APL, L"parseCookie() failed!");
	    return NULL;
	}
	char * end_of_cookie = strchr(substr, '\r');
	size_t cookie_len = end_of_cookie - substr;

	char * cookie = (char *)malloc(cookie_len+1);
	strncpy (cookie, substr, cookie_len);
	cookie[cookie_len] = '\0';

	return cookie;

}

char *parseToken(char * server_response, const char * token)
{
	char *substr = strstr(server_response, token);
	if (substr == NULL)
	{
		MWLOG(LEV_ERROR, MOD_APL, L"Failed to parse token %s", token);
		return NULL;

	}
	char * start_apdu = substr + strlen(token);
	char * end_apdu = strchr(start_apdu, '"');

	size_t apdu_len = end_apdu - start_apdu;

	char * change_pin_apdu = (char *) malloc(apdu_len+1);
	strncpy(change_pin_apdu, start_apdu, apdu_len);

	return change_pin_apdu;
}

/*
	Checks for success or error in the response JSON message and throws MWException containing
	the server-sent error code
*/
void handleErrorCode(cJSON * json_obj, const char *caller_function)
{
	if (json_obj == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "JSON parsing error. Invalid input to handleErrorCode()");
		throw CMWEXCEPTION(EIDMW_SAM_UNKNOWN_ERROR);
	}
	cJSON *error_obj = cJSON_GetObjectItem(json_obj, "ErrorStatus");
	if (error_obj != NULL)
	{
		if (error_obj->type == cJSON_Object)
		{
			cJSON * errorcode_obj = cJSON_GetObjectItem(error_obj, "code");
			if (errorcode_obj != NULL)
			{
				long error_code = -1;
				//Error code can be integer or string
				if (errorcode_obj->type == cJSON_String)
					error_code = atol(errorcode_obj->valuestring);
				else if (errorcode_obj->type == cJSON_Number)
					error_code = errorcode_obj->valueint;
					
				if (error_code > 0)
				{
					error_obj = cJSON_GetObjectItem(error_obj, "description");

					MWLOG(LEV_ERROR, MOD_APL, "%s: Received non-zero error code: %ld description: %s", 
						caller_function, error_code, error_obj->valuestring);
					throw CMWEXCEPTION(error_code);
				}
			}
		}
	}
}

#define ENDPOINT_07 "/changeaddress/signChallenge"

SignedChallengeResponse * SSLConnection::do_SAM_2ndpost(char *challenge, char *kicc)
{
	cJSON *json = NULL;
	char *endpoint = NULL;

	SignedChallengeResponse *result = new SignedChallengeResponse();
	const char *challenge_format = "{\"Challenge\":{ \"challenge\" : \"%s\", \"kicc\" : \"%s\", \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	char *challenge_params = NULL;

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_2ndpost()");

#ifdef _WIN32
	int buf_len = _scprintf(challenge_format, challenge, kicc);
	challenge_params = (char *) malloc(buf_len + 1);
#else
	//TODO: use snprintf to retrieve the needed buffer space
	challenge_params = (char *) malloc(5*1024);
#endif

	sprintf(challenge_params, challenge_format, challenge, kicc);
	endpoint = ENDPOINT_07;

	MWLOG(LEV_DEBUG, MOD_APL, "POSTing request: %s", challenge_params);
	char *server_response = Post(this->m_session_cookie, endpoint, challenge_params);

	free(challenge_params);
	char *body = skipHTTPHeaders(server_response);

	MWLOG(LEV_DEBUG, MOD_APL, "do_SAM_2ndpost server response: %s", server_response);

	free(server_response);

	json = cJSON_Parse(body);
	if (!json)
	{
		fprintf(stderr, "JSON parsing error before: [%s]\n", cJSON_GetErrorPtr());
		goto err;
	}
	else
	{
		cJSON *my_json = json->child;

		handleErrorCode(my_json, __FUNCTION__);

		cJSON *child = cJSON_GetObjectItem(my_json, "signedChallenge");
		if (!child)
		{
			fprintf(stderr, "DEBUG: JSON does not contain signedChallenge element!\n");
			goto err;
		}
		result->signed_challenge = _strdup(child->valuestring);
		cJSON *elem = cJSON_GetObjectItem(my_json, "InternalAuthenticateCommand");
		if (!elem)
		{
			fprintf(stderr, "DEBUG: JSON does not contain InternalAuthenticateCommand element!\n");
		}
		else
			result->internal_auth = _strdup(elem->child->valuestring);
		cJSON *elem2 = cJSON_GetObjectItem(my_json, "SetSECommand");

		if (!elem2)
		{
			fprintf(stderr, "DEBUG: JSON does not contain SetSECommand element!\n");
		}
		else
			result->set_se_command = _strdup(elem2->child->valuestring);

		cJSON_Delete(json);
		return result;
	}

err:
	delete result;
	cJSON_Delete(json);
	return NULL;
}


char *build_json_object_sam(StartWriteResponse &resp)
{
	cJSON *root, *real_root, *arr1, *arr2, *error_status;
	root=cJSON_CreateObject();
	real_root=cJSON_CreateObject();
	error_status = cJSON_CreateObject();

	arr1=cJSON_CreateArray();
	arr2=cJSON_CreateArray();
	cJSON_AddNumberToObject(error_status, "code", 0);
	cJSON_AddStringToObject(error_status, "description", "OK");

	for (unsigned int i=0; i!= resp.apdu_write_address.size(); i++)
		cJSON_AddItemToArray(arr1, cJSON_CreateString(resp.apdu_write_address.at(i)));

	for (unsigned int i=0; i!= resp.apdu_write_sod.size(); i++)
		cJSON_AddItemToArray(arr2, cJSON_CreateString(resp.apdu_write_sod.at(i)));

	cJSON_AddItemToObject(root, "WriteAddressResponse", arr1);
	cJSON_AddItemToObject(root, "WriteSODResponse", arr2);
	cJSON_AddItemToObject(root, "ErrorStatus", error_status);

	cJSON_AddItemToObject(real_root, "WriteResults", root);
	char * json_string = cJSON_Print(real_root);
	cJSON_Delete(real_root);

	return json_string;
}


bool SSLConnection::do_SAM_4thpost(StartWriteResponse &resp)
{
	cJSON *json = NULL;
	char *server_response = NULL;
	char *json_request = build_json_object_sam(resp);

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_4thpost()");

	MWLOG(LEV_DEBUG, MOD_APL, "POSTing request: %s", json_request);

	try {
		server_response = Post(this->m_session_cookie,
	     "/changeaddress/followUpWrite", json_request);
	
	}
	catch (CMWException &e) {

		free(json_request);
		MWLOG(LEV_ERROR, MOD_APL, 
			"Error caught in do_SAM_4thpost: Error code: %08x File: %s: line %ld", 
			e.GetError(), e.GetFile().c_str(), e.GetLine()); 

		MWLOG(LEV_ERROR, MOD_APL, "The Address Change process WAS ABORTED after successful card write!");

		throw CMWEXCEPTION(EIDMW_SAM_UNCONFIRMED_CHANGE);
	}

	MWLOG(LEV_DEBUG, MOD_APL, "do_SAM_4thpost: server response: %s", server_response);

	free(json_request);

	char *body = skipHTTPHeaders(server_response);

	json=cJSON_Parse(body);

	free(server_response);

	if (!json)
	{
		fprintf(stderr, "SSLConnection::do_SAM_4thpost - JSON parsing error before: [%s]\n", cJSON_GetErrorPtr());
		return false;
	}
	else
	{
		cJSON *my_json = json->child;

		cJSON *obj_error_status = cJSON_GetObjectItem(my_json, "ErrorStatus");
		cJSON * error_code = cJSON_GetObjectItem(obj_error_status, "code");

		if (error_code->type == cJSON_Number)
		{
			int error_value = error_code->valueint;
			MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection::do_SAM_4thpost - Server returned error code: %d\n", error_value);
			cJSON_Delete(json);
			return error_value == 0;
		}
		else
		{
			cJSON_Delete(json);
			return false;

		}

	}

}


StartWriteResponse *SSLConnection::do_SAM_3rdpost(char * mse_resp, char *internal_auth_resp)
{
	cJSON *json = NULL;
	const int buf_len = 1024;
	char post_body[buf_len];

	StartWriteResponse * resp = new StartWriteResponse();
	const char *start_write_format = "{\"StartWriteRequest\":{ \"SetSEResponse\" : [\"%s\"], \"InternalAuthenticateResponse\" : [\"%s\"], \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_3rdpost()");

	snprintf(post_body, buf_len, start_write_format, mse_resp, internal_auth_resp);

	MWLOG(LEV_DEBUG, MOD_APL, "POSTing request: %s", post_body);
	char *server_response = Post(this->m_session_cookie, "/changeaddress/startWrite", post_body);

	MWLOG(LEV_DEBUG, MOD_APL, "do_SAM_3rdpost server response: %s", server_response);

	char *body = skipHTTPHeaders(server_response);

	json=cJSON_Parse(body);
	if (!json)
	{
		fprintf(stderr, "JSON parsing error before: [%s]\n", cJSON_GetErrorPtr());
		goto err;
	}
	else
	{
		cJSON *my_json = json->child;

		handleErrorCode(my_json, __FUNCTION__);

		cJSON *array_address = cJSON_GetObjectItem(my_json, "WriteAddressCommand");

		if (!array_address)
		{
			fprintf(stderr, "No WriteAddressCommand was returned!");
			goto err;
		}
		int n_address = cJSON_GetArraySize(array_address);

		for (int i=0; i!= n_address; i++)
			resp->apdu_write_address.push_back(_strdup(cJSON_GetArrayItem(array_address, i)->valuestring));

		cJSON *array_sod= cJSON_GetObjectItem(my_json, "WriteSODCommand");
		int n_sod = cJSON_GetArraySize(array_sod);

		for (int i=0; i != n_sod; i++)
			resp->apdu_write_sod.push_back(_strdup(cJSON_GetArrayItem(array_sod, i)->valuestring));

		cJSON_Delete(json);
		delete server_response;
		return resp;
	}

err:
	delete resp;
	delete server_response;
	cJSON_Delete(json);
	return NULL;
}

DHParamsResponse *SSLConnection::do_SAM_1stpost(DHParams *p, char *secretCode, char *process, char *serialNumber)
{
	int ret_channel = 0;
	cJSON *json = NULL;
	char *endpoint = NULL;
	DHParamsResponse *server_params = new DHParamsResponse();
	char *dh_params_template = "{\"DHParams\":{ \"secretCode\" : \"%s\", \"process\" : \"%s\", \"P\": \"%s\", \"Q\": \"%s\", \"G\":\"%s\", \"cvc_ca_public_key\": \"%s\",\"card_auth_public_key\": \"%s\", \"certificateChain\": \"%s\", \"version\": %d, \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	char * post_dhparams = NULL;
	char request_headers[1000];

	if (serialNumber == NULL)
	{
#ifdef _WIN32
		//we need to use this instead of the _snprintf() with first param as NULL
		int buf_len = _scprintf(dh_params_template,secretCode, process, p->dh_p, p->dh_q, p->dh_g, p->cvc_ca_public_key,
			p->card_auth_public_key, p->certificateChain, p->version);
		post_dhparams = (char *) malloc(buf_len +2);
#else
		//TODO: use snprintf to determine needed allocation;
		post_dhparams = (char *) malloc(5*1024);

#endif
		sprintf(post_dhparams, dh_params_template,
			secretCode, process, p->dh_p, p->dh_q, p->dh_g, p->cvc_ca_public_key,
			p->card_auth_public_key, p->certificateChain, p->version);

		endpoint = "/changeaddress";
	}

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_1stpost()");

	snprintf(request_headers, sizeof(request_headers),
    "POST %s/sendDHParams HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: %lu\r\n\r\n",
    	endpoint, m_host, strlen(post_dhparams));

	char * server_response = (char *) calloc(REPLY_BUFSIZE, sizeof(char));

	MWLOG(LEV_DEBUG, MOD_APL, "do_SAM_1stpost: POSTing request: %s", post_dhparams);

	ret_channel = write_to_stream(m_ssl_connection, request_headers);
	//fprintf(stderr, "Wrote to channel: %d bytes\n", ret_channel);
	ret_channel = write_to_stream(m_ssl_connection, post_dhparams);
	//fprintf(stderr, "Wrote to channel: %d bytes\n", ret_channel);

	//Read response
	NetworkBuffer buffer;
	buffer.buf = server_response;
	buffer.buf_size = REPLY_BUFSIZE;
	unsigned int ret = read_from_stream(m_ssl_connection, &buffer);
	
	if (ret > 0)
	{
		MWLOG(LEV_DEBUG, MOD_APL, "do_SAM_1stpost: Server reply: %s", buffer.buf);
		m_session_cookie = parseCookie(buffer.buf);
		if (m_session_cookie == NULL)
		{
			delete server_params;
			free(post_dhparams);
			free(buffer.buf);

			//Catch renegotiation errors (e.g. using test cards)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
		}
	}

	char *body = skipHTTPHeaders(buffer.buf);

	json = cJSON_Parse(body);
	cJSON *my_json = json->child;
	if (my_json == NULL)
	{
		fprintf(stderr, "DEBUG: Server returned malformed JSON data: %s\n", body);
		free(buffer.buf);
		free(post_dhparams);
        cJSON_Delete(json);
		return server_params;
	}

	handleErrorCode(my_json, __FUNCTION__);

	cJSON *child = cJSON_GetObjectItem(my_json, "kifd");
	if (child)
		server_params->kifd = _strdup(child->valuestring);
	child = cJSON_GetObjectItem(my_json, "c_cv_ifd_aut");
	if (child)
		server_params->cv_ifd_aut = _strdup(child->valuestring);

	free(buffer.buf);
	free(post_dhparams);
	cJSON_Delete(json);

	return server_params;

}

char * SSLConnection::Post(char *cookie, char *url_path, char *body)
{
	int ret_channel = 0;

	char * request_template= "POST %s HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nCookie: %s\r\nContent-Length: %d\r\n\r\n";
	char * server_response = (char *) calloc(REPLY_BUFSIZE, sizeof(char));
	char request_headers[1000];

	//Build the full request
	snprintf (request_headers, sizeof(request_headers), request_template, url_path, m_host,
			cookie, strlen(body));

//	fprintf(stderr, "DEBUG: Sending POST Headers: \n%s\n", request_headers);
	ret_channel = write_to_stream(m_ssl_connection, request_headers);

//	fprintf(stderr, "DEBUG: Sending POST Body: \n%s\n", body);

	ret_channel = write_to_stream(m_ssl_connection, body);

	NetworkBuffer buffer;
	buffer.buf = server_response;
	buffer.buf_size = REPLY_BUFSIZE;

	//Read response
	read_from_stream(m_ssl_connection, &buffer);

	//Hack for chunked replies
	if (strstr(server_response, "Transfer-Encoding: chunked") != NULL)
	{
		MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection:Post() server response is chunked, calling read_chunked_reply()");
		read_chunked_reply(m_ssl_connection, &buffer, true);
	}

	return buffer.buf;

}

bool isUnsupportedProxy(char *tmpbuf) {

	return strstr(tmpbuf, "Proxy-Authenticate: Negotiate") != NULL ||
	   strstr(tmpbuf, "Proxy-Authenticate: Kerberos") != NULL || 
	   strstr(tmpbuf, "Proxy-Authenticate: NTLM") != NULL;

}

BIO * SSLConnection::connectToProxyServer(const char * proxy_host, long proxy_port, char *ssl_host, char *proxy_user, char * proxy_pwd, char *ssl_host_andport)
{

		char tmpbuf[10*1024];
        char connect_request[1024];
        char proxy_host_str[512];
        int ret = 0;

		BIO * cbio = BIO_new(BIO_s_connect());
		int len = 0;

		const char * user_agent = PTEID_USER_AGENT;
        const char * no_cache = "Pragma: no-cache";
        const char * no_content = "Content-Length: 0";
        const char * keepAlive = "Proxy-Connection: Keep-Alive";

		MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection: Connecting to proxy Host: %s Port: %ld",
			proxy_host, proxy_port);

		snprintf(proxy_host_str, sizeof(proxy_host_str), "%s:%ld", proxy_host, proxy_port);
		//TODO: the BIO_connect, BIO_read, BIO_write calls are blocking (we may need to have a timeout mechanism...)
		BIO_set_conn_hostname(cbio, proxy_host_str);

        if (BIO_do_connect(cbio) <= 0) {
               MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection: TCP connection to proxy server failed!");
               return NULL;
        }

        //For proxy auth Basic
        if (proxy_user != NULL && proxy_pwd != NULL)
        {
        	std::string proxy_cleartext = std::string(proxy_user) + ":" + proxy_pwd;
        	const char *proxy_auth_header = "Proxy-Authorization: basic ";

        	char * auth_token = Base64Encode((const unsigned char *)proxy_cleartext.c_str(), proxy_cleartext.size());

        	ret = snprintf(connect_request, sizeof(connect_request), "CONNECT %s HTTP/1.1\r\n Host: %s\r\n%s%s\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n",
        		ssl_host_andport, ssl_host, proxy_auth_header, auth_token, user_agent, no_cache, no_content, keepAlive);

        	free(auth_token);
        }
        else
        {

	    	ret = snprintf(connect_request, sizeof(connect_request), "CONNECT %s HTTP/1.1\r\n Host: %s\r\n%s\r\n%s\r\n%s\r\n%s\r\n\r\n",
            	ssl_host_andport, ssl_host, user_agent, no_cache, no_content, keepAlive);
		}

	    BIO_puts(cbio, connect_request);

        len = BIO_read(cbio, tmpbuf, sizeof(tmpbuf));
        tmpbuf[len] = '\0';

        MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection: CONNECT reply: %s", tmpbuf);

        if (strstr(tmpbuf, "200 Connection established") == NULL)  {
        	long errorCode = 0;
        	MWLOG(LEV_DEBUG, MOD_APL, L"Error connecting to proxy!");
          	
          	if (strstr(tmpbuf, "407 Proxy Authentication Required")!= NULL) {

				if (isUnsupportedProxy(tmpbuf))
          			errorCode = EIDMW_SAM_PROXY_UNSUPPORTED;
          		else 
          			errorCode = EIDMW_SAM_PROXY_AUTH_FAILED;
          	}
          	else 
          		errorCode = EIDMW_OTP_CONNECTION_ERROR;
          
            throw CMWEXCEPTION(errorCode);
        }

        return cbio;
}

/**
 * Connect to a host using an encrypted stream
 */
void SSLConnection::connect_encrypted(char* host_and_port)
{

    BIO* bio = NULL;
	char * proxy_user_value = NULL;
	char * proxy_pwd_value = NULL;
	const char *proxy_host = NULL;
	std::string pac_proxy_host;
	std::string pac_proxy_port;
		
	long proxy_port = 0;

    /* Set up the SSL pointers */

    /* TLS v1.2 is not supported at the moment as it uses different hash functions for the client challenge */
    SSL_CTX *ctx = SSL_CTX_new(TLSv1_1_client_method());

    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    APL_Card *card = AppLayer.getReader().getCard();

    APL_SmartCard * eid_card = static_cast<APL_SmartCard *> (card);
    m_certs = eid_card->getCertificates();

    //NOTE: to get more debug output from the SSL layer uncomment this
    //SSL_CTX_set_info_callback(ctx, eIDMW::get_ssl_state_callback);

    APL_Certif * cert = loadCertsFromCard(ctx);
    X509_STORE *store = SSL_CTX_get_cert_store(ctx);

    //Load cert chain for the current card
    loadCertChain(store, cert);

    //Load any available root cert to validate the server certificate (needed for AddressChange test server)
    loadAllRootCerts(store);
	
	// TODO: Load windows root certificates with some validations
	//loadWindowsRootCertificates(store);

	SSL_CTX_set_default_verify_paths(ctx);

    SSL_CTX_set_options(ctx, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    //This needs OpenSSL 1.0.2
#if (OPENSSL_VERSION_NUMBER >= 0x10002000L)
	X509_VERIFY_PARAM *vpm = SSL_CTX_get0_param(ctx);
	//Verify hostname in the server-provided certificate
	X509_VERIFY_PARAM_set1_host(vpm, m_host, 0);
#endif

    //Get Proxy configuration
	APL_Config config_proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
	APL_Config config_proxy_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
	APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
	APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);
	
#ifdef WIN32
	//Get system proxy configuration
	APL_Config conf_pac(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
	const char * proxy_pac = conf_pac.getString();
	std::string SAM_URL = std::string("https://") + m_host;
	if (proxy_pac != NULL && strlen(proxy_pac) > 0)
	{
		MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection using Proxy PAC: %s to URL: %s", proxy_pac, SAM_URL.c_str());
		GetProxyFromPac(proxy_pac, SAM_URL.c_str(), &pac_proxy_host, &pac_proxy_port);
		proxy_host = pac_proxy_host.c_str();
		proxy_port = atol(pac_proxy_port.c_str());
	}
#endif

	if (config_proxy_host.getString() != NULL && strlen(config_proxy_host.getString()) > 0 && config_proxy_port.getLong() != 0)
	{
		
		if (proxy_user.getString() != NULL && strlen(proxy_user.getString()) > 0)
		{
			proxy_user_value = (char *)proxy_user.getString();
			proxy_pwd_value = (char *)proxy_pwd.getString();
		}
		proxy_host = config_proxy_host.getString();
		proxy_port = config_proxy_port.getLong();
				
	}

	if (proxy_host != NULL && strlen(proxy_host) > 0)
	{

		bio = connectToProxyServer(proxy_host, proxy_port,
			m_host, proxy_user_value, proxy_pwd_value, host_and_port);

		if (!bio) {
			throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);			
		}
	}
	else
	{
    	bio = BIO_new_connect(host_and_port);
    	if (BIO_do_connect(bio) <= 0) {
    	   MWLOG(LEV_ERROR, MOD_APL, "SSLConnection: BIO_do_connect failed: %s", 
    	   	 ERR_error_string(ERR_get_error(), NULL));

	 	   throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
    	}
	}

    m_ssl_connection = SSL_new(ctx);
    SSL_set_bio (m_ssl_connection, bio, bio);

    RSA *rsa = RSA_new();

    rsa->flags |= RSA_FLAG_SIGN_VER;

    RSA_METHOD *current_method = (RSA_METHOD *)RSA_PKCS1_SSLeay();
    current_method->rsa_sign = eIDMW::rsa_sign;
    current_method->flags |= RSA_FLAG_SIGN_VER;
    current_method->flags |= RSA_METHOD_FLAG_NO_CHECK;

    RSA_set_method(rsa, current_method);

    if (SSL_use_RSAPrivateKey(m_ssl_connection, rsa) != 1)
    {
        fprintf(stderr, "SSL_CTX_use_RSAPrivateKey failed!");
        return;
    }

    SSL_set_connect_state(m_ssl_connection);

    int ret_connect = SSL_connect(m_ssl_connection);

    if (ret_connect != 1)
    {
		long openssl_error = ERR_get_error();
		MWLOG(LEV_ERROR, MOD_APL, "SSLConnection: error establishing connection. Detail: %s", ERR_error_string(openssl_error, NULL));

		throw CMWEXCEPTION(translate_openssl_error(openssl_error));
    }

}

int validate_hex_number(char *str)
{
        int i = 0;
        while(str[i])
        {
            if (!isxdigit(str[i++]))
               return 0;
        }

        return 1;
}


long parseLong(char *str)
{
	long val;
	char *endptr = NULL;

	errno = 0;
	val = strtol(str, &endptr, 16);

    /* Check for various possible errors */

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
		|| (errno != 0 && val == 0)) {
		perror("strtol");
	return -1;

	}

	if (endptr == str) {
		fprintf(stderr, "No digits were found\n");
		return -1;
	}

	return val;
}


int waitForRWSocket(SSL *ssl, bool wantRead)
{
	int rv = 0;
	int fd = SSL_get_fd(ssl);
	fd_set confds;

	FD_ZERO(&confds);
	FD_SET(fd, &confds);
	struct timeval tv;
	tv.tv_usec = 0;
	//Timeout value in seconds
	tv.tv_sec = 10;

	if (wantRead)
		rv = select(fd + 1, &confds, NULL, NULL, &tv);
	else
		rv = select(fd + 1, NULL, &confds, NULL, &tv);

	return rv;
}

typedef enum  {
	HEX, CRLF, CHUNK_DATA, TRAILER, PARSER_ERROR
} Chunky_State;

bool isxdigit_ascii(char digit)
{
  return (digit >= 0x30 && digit <= 0x39) /* 0-9 */
        || (digit >= 0x41 && digit <= 0x46) /* A-F */
        || (digit >= 0x61 && digit <= 0x66); /* a-f */
}

void parse_http_chunked_body(NetworkBuffer *net_buffer) {

	if (net_buffer->buf == NULL)
	{	
		MWLOG(LEV_ERROR, MOD_APL, "NULL buffer argument for parse_http_chunked_reply!");
		return;
	}

	unsigned int buffer_len = strlen(net_buffer->buf);
	char * out_buffer = (char *)calloc(buffer_len, sizeof(char));
	char * in_buffer = net_buffer->buf;

	char * body = skipHTTPHeaders(in_buffer);
	unsigned int offset = (unsigned int)(body - in_buffer);

	//Copy HTTP headers
	memcpy(out_buffer, in_buffer, offset);
	long out_offset = offset;

	//State machine variables
	Chunky_State state = HEX;
	long chunk_len = 0;
	int hex_len = 0;

	while(offset < buffer_len) {

		switch (state) {
			case HEX:
				hex_len = 0;
				while(isxdigit_ascii(in_buffer[offset+hex_len]))
					hex_len++;

				chunk_len = parseLong(in_buffer+offset);
				if (chunk_len == -1) {
					state = PARSER_ERROR;
					fprintf(stderr, "Error in parse_http_chunked_body, couldn't parse Chunk Length\n");
				}
				else
				{
					if (chunk_len == 0)
						fprintf(stderr, "Terminating chunk found.\n");
					state = CRLF;
					offset += hex_len;
				}
				break;

			case CRLF:
				if (in_buffer[offset] == 0x0D)
				{
					state = CHUNK_DATA;
					offset += 2;
				}
				else
				{
					fprintf(stderr, "Error in parse_http_chunked_body, expecting CRLF\n");
					state = PARSER_ERROR;
				}
				break;

			case CHUNK_DATA:
				memcpy(out_buffer+out_offset, in_buffer+offset, chunk_len);

				state = TRAILER;
				offset += chunk_len;
				out_offset += chunk_len;
				break;

			case TRAILER:
				if (in_buffer[offset] == 0x0D)
				{
					state = HEX;
					offset += 2;
				}
				else
				{
					state = PARSER_ERROR;
					fprintf(stderr, "Error in parse_http_chunked_body, expecting CRLF\n");
				}
				break;

			case PARSER_ERROR:
				goto parser_error;
		}
	}

	net_buffer->buf = out_buffer;
	return;

	parser_error:
		MWLOG(LEV_ERROR, MOD_APL, "http_chunked parser error giving up at offset: %u", offset);

}


void SSLConnection::read_chunked_reply(SSL *ssl, NetworkBuffer *net_buffer, bool headersAlreadyRead)
{
	int r;
	bool final_chunk_read = false;

	char *buffer = net_buffer->buf;
	unsigned int current_buf_length = net_buffer->buf_size;

	unsigned int bytes_read = headersAlreadyRead ? strlen(buffer) : 0;
	bool is_chunk_length = false;
	do
    {
    	is_chunk_length = false;

	    // We're using blocking IO so SSL_Write either succeeds completely or not...
    	r = SSL_read(ssl, buffer+bytes_read, current_buf_length-bytes_read);
    	//Read the chunk length
    	if (r > 0 && bytes_read > 0)
    	{
    		//XX: Can we assume that the final Chunk is always at the start of the last SSL packet??
    		if (memcmp(buffer+bytes_read, "\x30\x0d\x0a\x0d\x0a", 5) == 0)
    		{
    			final_chunk_read = true;
    			//fprintf(stderr, "DEBUG: Final chunk read. Ending read_chunked_reply() loop!\n");
    			//Discard the final chunk
    			*(buffer+bytes_read) = 0;
    		}

    	}
    	if (r > 0 && bytes_read == 0)
    	{
    		char * buffer_tmp = (char*)calloc(strlen(buffer)+1, 1);
    		strcpy(buffer_tmp, buffer);
    		if (!strstr(buffer, "Transfer-Encoding: chunked"))
				MWLOG(LEV_DEBUG, MOD_APL, "read_chunked_reply() Unexpected server reply: HTTP response is not chunked!\n");

			free(buffer_tmp);
    	}
		if (r <= 0)
		{
			int error_code = SSL_get_error(ssl, r);
			switch (error_code) {
			case SSL_ERROR_ZERO_RETURN:
				MWLOG(LEV_ERROR, MOD_APL, "read_chunked_reply() TLS Connection has been closed cleanly");
				throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
			case SSL_ERROR_WANT_READ:  /* there's data pending, re-invoke SSL_read() */
			case SSL_ERROR_WANT_WRITE:
				continue;
			default:
				unsigned long sslError = ERR_get_error();
				if ((r < 0) || sslError) {
					//SOCKERRNO can return here WSAETIMEDOUT (Windows socket error code)
					MWLOG(LEV_ERROR, MOD_APL, "read_chunked_reply() Aborted TLS Connection with error string %s errno: %d",
						(sslError ?
						ERR_error_string(sslError, NULL) :
						SSL_ERROR_to_str(error_code)),
						SOCKERRNO);

					throw CMWEXCEPTION(EIDMW_SSL_PROTOCOL_ERROR);
				}
			}
		}

		bytes_read += r;

    	//Check for buffer length
		if (bytes_read >= current_buf_length) {
			//Double the buffer length
			current_buf_length *= 2;

			net_buffer->buf = (char*)realloc(net_buffer->buf, current_buf_length);
			net_buffer->buf_size = current_buf_length;
			if (!net_buffer->buf) {
				fprintf(stderr, "Critical error: out of memory!\n");
				exit(1);
			}
			buffer = net_buffer->buf;
		}

    }
    while(bytes_read == 0 || !final_chunk_read);

    //Extract the HTTP body of the chunked message
    parse_http_chunked_body(net_buffer);

}

/**
 * Read from a stream and handle restarts and buffering if necessary
 */
unsigned int SSLConnection::read_from_stream(SSL* ssl, NetworkBuffer *net_buffer)
{

	int r = -1;
	unsigned int bytes_read = 0, header_len = 0, content_length = 0;
	unsigned int current_buf_length = net_buffer->buf_size;

	do
	{
		// We're using blocking IO so SSL_Write either succeeds completely or not...
		r = SSL_read(ssl, net_buffer->buf + bytes_read, current_buf_length - bytes_read);
		if (r > 0)
		{
			if (bytes_read == 0) {
				header_len = r;
				char * buffer_tmp = (char*)calloc(strlen(net_buffer->buf) + 1, 1);
				strcpy(buffer_tmp, net_buffer->buf);
				content_length = parseContentLength(buffer_tmp);
				free(buffer_tmp);
			}

			bytes_read += r;
		}
		else
		{
			int error_code = SSL_get_error(ssl, r);
			switch (error_code) {
			case SSL_ERROR_ZERO_RETURN:
				MWLOG(LEV_ERROR, MOD_APL, "read_from_stream() TLS Connection has been closed cleanly");
				throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
			case SSL_ERROR_WANT_READ:  /* there's data pending, re-invoke SSL_read() */
			case SSL_ERROR_WANT_WRITE:
				continue;
			default:
				unsigned long sslError = ERR_get_error();

				if ((r < 0) || sslError) {
					//SOCKERRNO can return here WSAETIMEDOUT (Windows socket error code)
					MWLOG(LEV_ERROR, MOD_APL, "read_from_stream() Aborted TLS Connection with error string %s errno: %d",
						(sslError ?
						ERR_error_string(sslError, NULL) :
						SSL_ERROR_to_str(error_code)),
						SOCKERRNO);

					throw CMWEXCEPTION(EIDMW_SSL_PROTOCOL_ERROR);
				}
				
			}
		}
		//Check for buffer length
		if (bytes_read >= current_buf_length) {
			//Double the buffer length
			current_buf_length *= 2;
			net_buffer->buf = (char*)realloc(net_buffer->buf, current_buf_length);
			net_buffer->buf_size = current_buf_length;

			if (!net_buffer->buf) {
				fprintf(stderr, "Critical error: out of memory!\n");
				exit(1);
			}
		}
	} while (bytes_read == 0 || bytes_read - header_len < content_length);

	if (bytes_read > 0)
		net_buffer->buf[bytes_read] = '\0';

	return bytes_read;
}


/**
 * Write to a stream and handle restarts if necessary
 */
unsigned int SSLConnection::write_to_stream(SSL* ssl, char* request_string) 
{

    unsigned int r = -1;
	do {
		r = SSL_write(ssl, request_string, strlen(request_string));
		if (r <= 0)
		{
			int error_code = SSL_get_error(ssl, r);
			switch (error_code) {
				case SSL_ERROR_ZERO_RETURN:
					MWLOG(LEV_ERROR, MOD_APL, "write_to_stream() TLS Connection has been closed cleanly");
					throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
				case SSL_ERROR_WANT_READ:  /* there's data pending, re-invoke SSL_write() */
					if (waitForRWSocket(ssl, true) > 0)
						continue;
					else
						throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
				case SSL_ERROR_WANT_WRITE:
					if (waitForRWSocket(ssl, false) > 0)
						continue;
					else
						throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
				default:
					unsigned long sslError = ERR_get_error();
					if (sslError) {
						MWLOG(LEV_ERROR, MOD_APL, "write_to_stream: Aborted TLS Connection with error code %d and root cause %s",
							error_code, ERR_error_string(sslError, NULL));
					}
					else
					{
						MWLOG(LEV_ERROR, MOD_APL, "write_to_stream: Aborted TLS Connection with error code %d",
							error_code);
					}
					throw CMWEXCEPTION(EIDMW_SSL_PROTOCOL_ERROR);
			}
		}
	} while (r <= 0);

    return r;
}

/**
 * Perform TLS handshake to the SAM server: it may throw CMWException on connection error
 */
bool SSLConnection::InitSAMConnection() 
{

    APL_Config sam_server(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SAM_SERVER);

    std::string server_str = sam_server.getString();

    char * host_and_port = (char *)server_str.c_str();

    //Split hostname and port
    m_host = (char *)malloc(strlen(host_and_port)+1);
    strcpy(m_host, host_and_port);
    char * delim = strchr(m_host, ':');
    *delim = '\0';

    MWLOG(LEV_DEBUG, MOD_APL, "SSLConnection: connecting to SAM server: %s", host_and_port);

    /* initialise the OpenSSL library */
    init_openssl();

    connect_encrypted(host_and_port);

    if (m_ssl_connection == NULL)
            return false;

    return true;
}
}
