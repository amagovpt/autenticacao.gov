/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2011-2014
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *
 *  HTTPS Client with Client Certificate Authentication for PT-eID Middleware
 *  It provides the communication protocol with the OTP and Address Change HTTPS WebServices
 *
*/
#include "SSLConnection.h"

#include "MWException.h"
#include "eidErrors.h"
#include "APLConfig.h"
#include "Log.h"
#include "static_pteid_certs.h"
#include "CardPteidDef.h"
#include "cJSON.h"

/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* OpenSSL headers */
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/opensslv.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace eIDMW
{

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, const RSA * rsa)
{
	
	APL_Card *card = AppLayer.getReader().getCard();

	if (card == NULL) {
	       	fprintf(stderr, "rsa_sign(): Failed to get card from global aplayer object\n"); 
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
		signed_data = card->Sign(to_sign, false); 
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


void SSLConnection::loadUserCert(SSL_CTX *ctx)
{

	APL_Card *card = AppLayer.getReader().getCard();
	CByteArray user_cert;
	card->readFile(PTEID_FILE_CERT_AUTHENTICATION, user_cert);

	//const unsigned char *p = user_cert.GetBytes();

	//X509 * cert = d2i_X509(NULL, &p, user_cert.Size());

	//if (cert != NULL) 
	//	SSL_CTX_use_certificate(ctx, cert);
	int ret = SSL_CTX_use_certificate_ASN1(ctx, user_cert.Size(), user_cert.GetBytes());

	if (ret != 1)
	{
		fprintf(stderr, "Error loading auth certificate for SSL handshake!\n");
		ERR_print_errors_fp(stderr);
	}

}

void SSLConnection::loadCertChain(X509_STORE *store)
{

	//Load all the CA certificates from our internal structures

	for (unsigned int i = 0; i != CERTS_N;
		i++)
	{
		X509 *pCert = NULL;
		unsigned char *cert_data = PTEID_CERTS[i].cert_data;
	        pCert = d2i_X509(&pCert, (const unsigned char **)&cert_data, 
			PTEID_CERTS[i].cert_len);

		if (pCert == NULL)
		{
			char *parsing_error = ERR_error_string(ERR_get_error(), NULL);
			MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection::loadCertChain: Error parsing certificate #%d. Details: %s",
				i, parsing_error);
		}
		else
		{
			if(X509_STORE_add_cert(store, pCert) == 0)
				MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection::loadCertChain: error adding certificate #%d\n",  i);
		}
	
	}


}


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


//Translate the string via specific OpenSSL error codes 
//is not feasible AFAICT
unsigned int translate_openssl_error(unsigned int error)
{

	char *error_string = ERR_error_string(error, NULL);

	if(strstr(error_string, "Connection refused") != NULL
	|| strstr(error_string, "bad hostname lookup") != NULL)
		return EIDMW_OTP_CONNECTION_ERROR;

	else if (strstr(error_string, "certificate verify failed") != NULL)
		return EIDMW_OTP_CERTIFICATE_ERROR;
	else 
		return EIDMW_OTP_UNKNOWN_ERROR;

}

void translate_ssl_error(SSL *ssl, int error_code)
{

	switch(SSL_get_error(ssl, error_code))
	{

		case SSL_ERROR_NONE:
		break;

		case SSL_ERROR_ZERO_RETURN:
		fprintf(stderr, "ssl_error: Connection closed!\n");
		break;

		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		fprintf(stderr, "ssl_error: Read/Write operation should be retried! or there's no more data in the buffers\n");

		case SSL_ERROR_WANT_CONNECT:
		fprintf(stderr, "ssl_error: Rad/Write operation should be retried!\n");

		case SSL_ERROR_SSL:
		fprintf(stderr, "ssl_error: SSL Protocol error, aborting!\n");

	}

	fprintf(stderr, "Complete error stack: \n");
	ERR_print_errors_fp(stderr);


}


/**
 * Print SSL error details
 */
void print_ssl_error(char* message, FILE* out) {

    fprintf(out, "DEBUG: %s", message);
    ERR_print_errors_fp(out);
}


/**
 * Initialise OpenSSL
 */
void init_openssl() {

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


#define REPLY_BUFSIZE 100000


char * SSLConnection::do_SAM_mutualAuthentication_IAS101(char *challenge)
{
	const char * endpoint = "/changeaddress101/mutualAuthentication";
	const char *mutual_format = 
	"{\"MutualAuthenticateInit\":{ \"challenge\" : \"%s\", \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } }";
	char *challenge_params = (char *) malloc(1024);

	sprintf(challenge_params, mutual_format, challenge);

	fprintf(stderr, "POSTing JSON %s\n", challenge_params);
	char *server_response = Post(this->m_session_cookie, (char *)endpoint, challenge_params);

	char *body = skipHTTPHeaders(server_response);

	fprintf(stderr, "DEBUG: Server reply: \n%s\n", server_response);

	cJSON *json = cJSON_Parse(body);
	if (!json)
	{
		fprintf(stderr, "JSON parsing error before: [%s]\n", cJSON_GetErrorPtr());
		return NULL;
	}
	else
	{
		cJSON *my_json = json->child;
		cJSON *child = cJSON_GetObjectItem(my_json, "MutualAuthenticateCommand");	
		return strdup(child->child->valuestring);
	}

	return NULL;
}

#define ENDPOINT_07 "/changeaddress/signChallenge"
#define ENDPOINT_101 "/changeaddress101/signChallenge"

//TODO: we need to make sure what MSE set commands we need to send before each GET RANDOM on IAS 0.7

SignedChallengeResponse * SSLConnection::do_SAM_2ndpost(char *challenge, char *kicc)
{
	cJSON *json = NULL;
	char *endpoint = NULL;

	SignedChallengeResponse *result = new SignedChallengeResponse();
	const char *challenge_format = "{\"Challenge\":{ \"challenge\" : \"%s\", \"kicc\" : \"%s\", \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";
	const char *challenge_format2 = "{\"Challenge\":{ \"challenge\" : \"%s\", \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	char *challenge_params = (char *) malloc(5*1024);

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_2ndpost()");

	if (kicc != NULL)
	{
		sprintf(challenge_params, challenge_format, challenge, kicc);
		endpoint = ENDPOINT_07;
	}
	else
	{
		sprintf(challenge_params, challenge_format2, challenge);
		endpoint = ENDPOINT_101;
	}

	fprintf(stderr, "POSTing JSON %s\n", challenge_params);
	char *server_response = Post(this->m_session_cookie, endpoint, challenge_params);

	char *body = skipHTTPHeaders(server_response);
	fprintf(stderr, "DEBUG: Server reply: \n%s\n", server_response);

	json = cJSON_Parse(body);
	if (!json)
	{
		fprintf(stderr, "JSON parsing error before: [%s]\n", cJSON_GetErrorPtr());
		return NULL;
	}
	else
	{
		cJSON *my_json = json->child;
		cJSON *child = cJSON_GetObjectItem(my_json, "signedChallenge");
		if (!child)
		{
			fprintf(stderr, "DEBUG: JSON does not contain signedChallenge element!\n");
			return NULL;
		}
		result->signed_challenge = strdup(child->valuestring);
		cJSON *elem = cJSON_GetObjectItem(my_json, "InternalAuthenticateCommand");
		if (!elem)
		{
			 if (strcmp(endpoint, ENDPOINT_07) == 0)
				fprintf(stderr, "DEBUG: JSON does not contain InternalAuthenticateCommand element!\n");
		}
		else
			result->internal_auth = strdup(elem->child->valuestring);
		cJSON *elem2 = cJSON_GetObjectItem(my_json, "SetSECommand");
		if (!elem2)
		{
			if (strcmp(endpoint, ENDPOINT_07) == 0)
				fprintf(stderr, "DEBUG: JSON does not contain SetSECommand element!\n");
		}
		else
			result->set_se_command = strdup(elem2->child->valuestring);

		return result;
	}

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

	for (int i=0; i!= resp.apdu_write_address.size(); i++)
		cJSON_AddItemToArray(arr1, cJSON_CreateString(resp.apdu_write_address.at(i)));

	for (int i=0; i!= resp.apdu_write_sod.size(); i++)
		cJSON_AddItemToArray(arr2, cJSON_CreateString(resp.apdu_write_sod.at(i)));

	cJSON_AddItemToObject(root, "WriteAddressResponse", arr1);
	cJSON_AddItemToObject(root, "WriteSODResponse", arr2);
	cJSON_AddItemToObject(root, "ErrorStatus", error_status);

	cJSON_AddItemToObject(real_root, "WriteResults", root);
	return cJSON_Print(real_root);
}


bool SSLConnection::do_SAM_4thpost(StartWriteResponse &resp)
{
	cJSON *json = NULL;
	char *json_request = build_json_object_sam(resp);

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_4thpost()");

	fprintf(stderr, "POSTing JSON %s\n", json_request);

	char *server_response = Post(this->m_session_cookie,
	  "/changeaddress/followUpWrite", json_request);

	fprintf(stderr, "DEBUG: Server reply: \n%s\n", server_response);

	char *body = skipHTTPHeaders(server_response);

	json=cJSON_Parse(body);
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
			return error_value == 0;
		}
		else
			return false;

	}

}


StartWriteResponse *SSLConnection::do_SAM_3rdpost(char * mse_resp, char *internal_auth_resp)
{
	cJSON *json = NULL;
	char post_body[1024];
	StartWriteResponse * resp = new StartWriteResponse();
	const char *start_write_format = "{\"StartWriteRequest\":{ \"SetSEResponse\" : [\"%s\"], \"InternalAuthenticateResponse\" : [\"%s\"], \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_3rdpost()");

	snprintf(post_body, sizeof(post_body), start_write_format, mse_resp, internal_auth_resp);
	fprintf(stderr, "POSTing JSON %s\n", post_body);

	char *server_response = Post(this->m_session_cookie,
	  "/changeaddress/startWrite", post_body, true);

	fprintf(stderr, "DEBUG: Server reply: \n%s\n", server_response);

	char *body = skipHTTPHeaders(server_response);

	json=cJSON_Parse(body);
	if (!json)
	{
		fprintf(stderr, "JSON parsing error before: [%s]\n", cJSON_GetErrorPtr()); 
		return NULL;
	}
	else
	{
		cJSON *my_json = json->child;
		int i;

		cJSON *array_address = cJSON_GetObjectItem(my_json, "WriteAddressCommand");

		if (!array_address)
		{
			fprintf(stderr, "No WriteAddressCommand was returned!");
			return NULL;
		}
		int n_address = cJSON_GetArraySize(array_address);

		for (i=0; i!= n_address; i++)
			resp->apdu_write_address.push_back(strdup(cJSON_GetArrayItem(array_address, i)->valuestring));

		cJSON *array_sod= cJSON_GetObjectItem(my_json, "WriteSODCommand");
		int n_sod = cJSON_GetArraySize(array_sod);

		for (i=0; i!= n_sod; i++)
			resp->apdu_write_sod.push_back(strdup(cJSON_GetArrayItem(array_sod, i)->valuestring));

		return resp;
	}
	return NULL;
}

DHParamsResponse *SSLConnection::do_SAM_1stpost(DHParams *p, char *secretCode, char *process, char *serialNumber)
{
	int ret_channel = 0;
	cJSON *json = NULL;
	char *endpoint = NULL;
	DHParamsResponse *server_params = new DHParamsResponse();
	char *dh_params_template = "{\"DHParams\":{ \"secretCode\" : \"%s\", \"process\" : \"%s\", \"P\": \"%s\", \"Q\": \"%s\", \"G\":\"%s\", \"cvc_ca_public_key\": \"%s\",\"card_auth_public_key\": \"%s\", \"certificateChain\": \"%s\", \"version\": %d, \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";

	char *dh_params_template2 = "{\"DHParams\":{ \"secretCode\" : \"%s\", \"process\" : \"%s\", \"cvc_ca_public_key\": \"%s\",\"card_auth_public_key\": \"%s\", \"certificateChain\": \"%s\", \"serialNumber\": \"%s\", \"version\": %d, \"ErrorStatus\": { \"code\":0, \"description\":\"OK\" } } } ";
	char * post_dhparams = (char *) malloc(5*1024);
	char request_headers[1000];

	if (serialNumber == NULL)
	{
		sprintf(post_dhparams, dh_params_template,
			secretCode, process, p->dh_p, p->dh_q, p->dh_g, p->cvc_ca_public_key,
			p->card_auth_public_key, p->certificateChain, p->version);

		endpoint = "/changeaddress";
	}
	else
	{
		sprintf(post_dhparams, dh_params_template2,
			secretCode, process, p->cvc_ca_public_key, p->card_auth_public_key, p->certificateChain, serialNumber, p->version);
		endpoint = "/changeaddress101";
	}

	MWLOG(LEV_DEBUG, MOD_APL, L"SSLConnection: running do_SAM_1stpost()");

	snprintf(request_headers, sizeof(request_headers),	
    "POST %s/sendDHParams HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: %lu\r\n\r\n",
    	endpoint, m_host, strlen(post_dhparams));

	char * server_response = (char *) malloc(REPLY_BUFSIZE);

	fprintf(stderr, "POSTing JSON %s\n", post_dhparams);

	ret_channel = write_to_stream(m_ssl_connection, request_headers);
	fprintf(stderr, "Wrote to channel: %d bytes\n", ret_channel);
	ret_channel = write_to_stream(m_ssl_connection, post_dhparams);
	fprintf(stderr, "Wrote to channel: %d bytes\n", ret_channel);

	//Read response
	unsigned int ret = read_from_stream(m_ssl_connection, server_response, REPLY_BUFSIZE);

	fprintf(stderr, "DEBUG: Server reply: \n%s\n", server_response);

	if (ret > 0)
	{
		m_session_cookie = parseCookie(server_response);
		if (m_session_cookie == NULL)
			//Catch renegotiation errors (e.g. using test cards)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

	char *body = skipHTTPHeaders(server_response);

	json = cJSON_Parse(body);
	cJSON *my_json = json->child;
	if (my_json == NULL)
	{
		fprintf(stderr, "DEBUG: Server returned malformed JSON data: %s\n", body);
		return server_params;
	}

	cJSON *child = cJSON_GetObjectItem(my_json, "kifd");
	if (child)
		server_params->kifd = child->valuestring;
	child = cJSON_GetObjectItem(my_json, "c_cv_ifd_aut");
	if (child)
		server_params->cv_ifd_aut = child->valuestring;

	return server_params;

}

char * SSLConnection::Post(char *cookie, char *url_path, char *body, bool chunked_expected)
{
	int ret_channel = 0;

	char * request_template= "POST %s HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nCookie: %s\r\nContent-Length: %d\r\n\r\n";
	char * server_response = (char *) malloc(REPLY_BUFSIZE);
	char * request_headers = (char *) calloc(1000, sizeof(char));

	//Build the full request
	snprintf (request_headers, 1000, request_template, url_path, m_host,
			cookie, strlen(body));

//	fprintf(stderr, "DEBUG: Sending POST Headers: \n%s\n", request_headers);
	ret_channel = write_to_stream(m_ssl_connection, request_headers);

//	fprintf(stderr, "DEBUG: Sending POST Body: \n%s\n", body);

	ret_channel = write_to_stream(m_ssl_connection, body);

	//Read response
	//TODO: header and content reading should be split so we can dynamically decide if it uses chunked encoding or not
	//TODO: get rid of this ugly hack
	if (chunked_expected && strstr(m_host, "teste") == NULL)
		read_chunked_reply(m_ssl_connection, server_response, REPLY_BUFSIZE);
	else
		read_from_stream(m_ssl_connection, server_response, REPLY_BUFSIZE);

//	if (ret == 0)
//	   ERR_print_errors_fp(stderr); //Connection aborted by server

	//fprintf(stderr, "Server reply: \n%s\n", server_response);

	return server_response;

}

/**
 * Connect to a host using an encrypted stream
 */
SSL* SSLConnection::connect_encrypted(char* host_and_port)
{

    BIO* bio = NULL;

    /* Set up the SSL pointers */
    SSL_CTX *ctx = SSL_CTX_new(TLSv1_1_client_method());

    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);


    //NOTE: to get more debug output from the SSL layer uncomment this
    //SSL_CTX_set_info_callback(ctx, eIDMW::get_ssl_state_callback);

    loadUserCert(ctx);

    X509_STORE *store = SSL_CTX_get_cert_store(ctx);

    loadCertChain(store);

    if(!(SSL_CTX_load_verify_locations(ctx,
			NULL, "/etc/ssl/certs")))
	fprintf(stderr, "Can't read CA list\n");
	
    SSL_CTX_set_options(ctx, SSL_OP_NO_TICKET | SSL_OP_NO_SSLv2);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

//Version check for openssl >= 1.0.2 functions as described here: 
// https://www.openssl.org/docs/manmaster/ssl/SSL_set1_param.html

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
    X509_VERIFY_PARAM *vpm = SSL_CTX_get0_param(ctx);
    //Verify hostname in the server-provided certificate
    X509_VERIFY_PARAM_set1_host(vpm, m_host, 0);
#endif

    bio = BIO_new_connect(host_and_port);
    if (BIO_do_connect(bio) <= 0) {
	    fprintf(stderr, "Error connecting to OTP server\n");
	    ERR_print_errors_fp(stderr);
    }
    SSL *ssl_handle = SSL_new(ctx);
    SSL_set_bio (ssl_handle, bio, bio);

    RSA *rsa = RSA_new();

    rsa->flags |= RSA_FLAG_SIGN_VER;

    RSA_METHOD *current_method = (RSA_METHOD *)RSA_PKCS1_SSLeay(); 
    current_method->rsa_sign = eIDMW::rsa_sign;
    current_method->flags |= RSA_FLAG_SIGN_VER;
    current_method->flags |= RSA_METHOD_FLAG_NO_CHECK;

    RSA_set_method(rsa, current_method);

    if (SSL_use_RSAPrivateKey(ssl_handle, rsa) != 1)
    {
        fprintf(stderr, "SSL_CTX_use_RSAPrivateKey failed!");
        return NULL;
    }


    SSL_set_connect_state(ssl_handle);

    int ret_connect = SSL_connect(ssl_handle);

    if (ret_connect != 1)
    {
	//translate_ssl_error(ssl_handle, ret_connect);
	//fprintf(stderr, "Error returned by SSL_connect (cause): %s\n", ERR_error_string(ERR_get_error(), NULL));
	MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection: error establishing connection");

	throw CMWEXCEPTION(translate_openssl_error(ERR_get_error()));
    }

    return ssl_handle;
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

	//Further characters after number
	if (*endptr != '\0')
		return -1; 

	return val;
}

void SSLConnection::read_chunked_reply(SSL *ssl, char *buffer, unsigned int buffer_len)
{
	int r;
	bool final_chunk_read = false;
	unsigned int bytes_read = 0;
	long chunk_length = 0;
	do
    {	
    	chunk_length = 0;
	    // We're using blocking IO so SSL_Write either succeeds completely or not...
    	r = SSL_read(ssl, buffer+bytes_read, buffer_len-bytes_read);
    	//Read the chunk length
    	if (r > 0 && bytes_read > 0)
    	{
    		if (memcmp(buffer+bytes_read, "\x30\x0d\x0a\x0d\x0a", 5) == 0)
    		{
    			final_chunk_read = true;
    			fprintf(stderr, "DEBUG: Final chunk read. Ending read_chunked_reply() loop!\n");
    			//Discard the final chunk
    			*(buffer+bytes_read) = 0;
    		}
    		else
    		{

    			char * buffer_tmp = (char*)calloc(r+1, 1);
    			memcpy(buffer_tmp, buffer+bytes_read, r);
    			buffer_tmp[r] = '\0';

    			char * endline = strstr(buffer_tmp, "\r\n");
    			if (endline)
    				*endline = '\0';
    			long val = parseLong(buffer_tmp);
    			if (val != -1)
    			{
    				fprintf(stderr, "DEBUG: Parsed chunk length= %ld\n", val);
    				*(buffer+bytes_read) = 0;
    				chunk_length = val;
    			}

    		}
    	}
    	if (r > 0 && bytes_read == 0)
    	{
    		char * buffer_tmp = (char*)calloc(strlen(buffer)+1, 1);
    		strcpy(buffer_tmp, buffer);
    		if (!strstr(buffer, "Transfer-Encoding: chunked"))
    			fprintf(stderr, "DEBUG: Unexpected server reply: HTTP response is not chunked!\n");

    	}
    	if (r == -1)
    	{
			int error_code = SSL_get_error(ssl, r);
    		if (error_code == SSL_ERROR_WANT_READ)
    		{
    			fprintf(stderr, "SSL_ERROR_WANT_READ\n!");
    			continue;
    		}
			else if (error_code == SSL_ERROR_SSL)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Aborted SSL Connection in read_chunked_reply() with error string %s", 
					ERR_error_string(ERR_get_error(), NULL));

				throw CMWEXCEPTION(EIDMW_SSL_PROTOCOL_ERROR);
			}
			else
    			fprintf(stderr, "???\n");
    	}
    	else if (r == 0) {

    		translate_ssl_error(ssl, r);

    	}
    	//Only include the data if this string is not a chunk-length token
    	else if (chunk_length == 0)
    		bytes_read += r;
    }
    while(bytes_read == 0 || !final_chunk_read);

}

/**
 * Read from a stream and handle restarts and buffering if necessary
 */
unsigned int SSLConnection::read_from_stream(SSL* ssl, char* buffer, unsigned int buffer_length)
{

    int r = -1;
    unsigned int bytes_read = 0, header_len=0, content_length = 0;

    do
    {
	    // We're using blocking IO so SSL_Write either succeeds completely or not...
	    r = SSL_read(ssl, buffer+bytes_read, buffer_length-bytes_read);
	    if (r > 0 && bytes_read == 0)
	    {
		   
		    header_len = r;
		    char * buffer_tmp = (char*)calloc(strlen(buffer)+1, 1);
		    strcpy(buffer_tmp, buffer);
		    content_length = parseContentLength(buffer_tmp);
	    }
		if (r == -1)
		{
			int error_code = SSL_get_error(ssl, r);

			if (error_code == SSL_ERROR_WANT_READ)
			{
				fprintf(stderr, "SSL_ERROR_WANT_READ\n!");
				continue;
			}
			else if (error_code == SSL_ERROR_SSL)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Aborted SSL Connection with error string %s", 
					ERR_error_string(ERR_get_error(), NULL));

				throw CMWEXCEPTION(EIDMW_SSL_PROTOCOL_ERROR);
			}
			else
				fprintf(stderr, "???\n");
		}

		else if (r == 0) {

			translate_ssl_error(ssl, r);

		}
	    else
		    bytes_read += r;
    }
    while(bytes_read == 0 || bytes_read - header_len  < content_length );

    if (bytes_read > 0)
	    buffer[bytes_read] = '\0';

    return bytes_read;
}

/**
 * Write to a stream and handle restarts if necessary
 */
unsigned int SSLConnection::write_to_stream(SSL* ssl, char* request_string) {

    unsigned int r = -1;

    r = SSL_write(ssl, request_string, strlen(request_string));
    if (r <= 0) {

          print_ssl_error("BIO_Write should retry test failed.\n", stdout);
          r = 0;
     }

    return r;
}

/**
 * Main SSL demonstration code entry point
 */
bool SSLConnection::InitConnection()
{

    APL_Config sam_server(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SAM_SERVER);

    std::string server_str = sam_server.getString();

    char * host_and_port = (char *)server_str.c_str();

    //Split hostname and port
    m_host = (char *)malloc(strlen(host_and_port)+1);
    strcpy(m_host, host_and_port);
    char * delim = strchr(m_host, ':');
    *delim = '\0';

    /* initialise the OpenSSL library */
    init_openssl();

    if ((m_ssl_connection = connect_encrypted(host_and_port)) == NULL)
            return false;

    return true;
}
}
