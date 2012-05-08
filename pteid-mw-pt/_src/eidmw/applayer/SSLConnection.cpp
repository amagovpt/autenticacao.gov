/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2011-2012
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *
 *  HTTPS Client with Client Certificate Authentication for PT-eID Middleware
 *
*/
#include "SSLConnection.h"

#include "MWException.h"
#include "eidErrors.h"
#include "APLConfig.h"
#include "Log.h"
#include "static_pteid_certs.h"
#include "CardPteidDef.h"

/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* OpenSSL headers */
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bn.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace eIDMW
{

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, const RSA * rsa)
{
	
	APL_Card *card = AppLayer.getReader(0UL).getCard();

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

	APL_Card *card = AppLayer.getReader(0UL).getCard();
	CByteArray user_cert;
	card->readFile(PTEID_FILE_CERT_AUTHENTICATION, user_cert);
	const unsigned char *p = user_cert.GetBytes();

	X509 * cert = d2i_X509(NULL, &p, user_cert.Size());

	if (cert != NULL) 
		SSL_CTX_use_certificate(ctx, cert);

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
				MWLOG(LEV_ERROR, MOD_APL, L"XadesSignature::ValidateCert: error adding certificate #%d\n",  i);
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

    /* call the standard SSL init functions */
    SSL_library_init();
    SSL_load_error_strings();

    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

}

char *FormatOtpParameters (OTPParams *otp)
{
	char *params = (char *)malloc(1000);

	int ret = sprintf (params, "{\"PinPafUpdate\":{\"pan\":\"%s\",\"pin\":\"%s\",\"panseqnumber\":\"%s\",\"cdol1\":\"%s\",\"atc\":\"%s\",\"arqc\":\"%s\",\"counter\":%s,\"pintrycounter\":%s}}", otp->pan, otp->pin, otp->pan_seq_nr, otp->cdol1, otp->atc, otp->arqc, otp->counter, otp->pin_try_counter);
	
	return params;
}

char *FormatResetSCParameters(OTPParams* otp)
{

	char *params = (char *)malloc(1000);

	int ret = sprintf(params, "{\"OnlineTransactionParameters\" : {\"pan\":\"%s\",\"pin\":\"%s\",\"panseqnumber\":\"%s\",\"cdol1\":\"%s\",\"atc\":\"%s\",\"arqc\":\"%s\",\"counter\":%s,\"pintrycounter\":%s}}", otp->pan, otp->pin, otp->pan_seq_nr, otp->cdol1, otp->atc, otp->arqc, otp->counter, otp->pin_try_counter);

	return params;

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

char *parseCookie(char *server_response)
{

	char *substr = NULL;
	substr = strstr(server_response, "JSESSIONID=");
	if (substr == NULL)
	{
	    MWLOG(LEV_ERROR, MOD_APL, L"parseCookie() failed server returned unexpected content");
	    return NULL;
	}

	char * end_of_cookie = strchr(substr, '\n');
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


#define REPLY_BUFSIZE 10000
/*
 * The return value of this method will be the cookie set by the server
 * if the connection is successfull
 */
char * SSLConnection::do_OTP_1stpost()
{
	char * cookie = NULL;
	char * request_headers = (char *) calloc(1000, sizeof(char));

	snprintf(request_headers, 1000,	
    "POST /CAPPINChange/connect HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: 16\r\n\r\n",
    	m_otp_host);
    	char * request_body = "{\"Connect\" : {}}";
	char * server_response = (char *) malloc(REPLY_BUFSIZE);

	write_to_stream(m_ssl_connection, request_headers);
	write_to_stream(m_ssl_connection, request_body);

	//Read response
	unsigned int ret = read_from_stream(m_ssl_connection, server_response, REPLY_BUFSIZE);

	//fprintf(stderr, "Server reply: \n%s\n", server_response);

	if (ret > 0)
		cookie = parseCookie(server_response);

	return cookie;

}

char * SSLConnection::do_OTP_2ndpost(char *cookie, OTPParams *params)
{
	

	char * otp_params = FormatOtpParameters(params);

	char * server_response = Post(cookie, "/CAPPINChange/sendParameters", otp_params);
	//Build the full request
	return parseToken(server_response, "\"apdu\":\"");

}


void SSLConnection::do_OTP_3rdpost(char *cookie, const char *changepin_card_response)
{

	char * server_response = NULL;
	char * request_body = (char *) calloc(200, sizeof(char));
	char * body_template = "{ \"PinChangeUnlockResponse\" : { \"apdu\" : \"%s\" } }";
	snprintf (request_body, 200, body_template, 
			changepin_card_response);

	server_response = Post(cookie, "/CAPPINChange/sendChangePINResponse", request_body);
	//TODO: We should at least check for an HTTP 200 return code

}


char * SSLConnection::do_OTP_4thpost(char *cookie, OTPParams *params)
{

	char * otp_params = FormatResetSCParameters(params);

	char * server_response = Post(cookie, "/CAPPINChange/sendResetScriptCounterParameters", otp_params);

	return parseToken(server_response, "\"cdol2\":\"");

}

void SSLConnection::do_OTP_5thpost(char *cookie, const char *reset_scriptcounter_response)
{

	char * server_response = NULL;
	char * request_body = (char *) calloc(200, sizeof(char));
	char * body_template = "{ \"ResetScriptCounterResponse\" : { \"apdu\" : \"%s\" } }";
	snprintf (request_body, 200, body_template, reset_scriptcounter_response);

	server_response = Post(cookie, "/CAPPINChange/resetScriptCounterResponse", request_body);
	//TODO: We should at least check for an HTTP 200 return code

}

char * SSLConnection::Post(char *cookie, char *url_path, char *body)
{

	char * request_template= "POST %s HTTP/1.1\r\nHost: %s\r\nKeep-Alive: 300\r\nContent-Type: text/plain; charset=UTF-8\r\nCookie: %s\r\nContent-Length: %d\r\n\r\n";
	char * server_response = (char *) malloc(REPLY_BUFSIZE);
	char * request_headers = (char *) calloc(1000, sizeof(char));

	//Build the full request
	snprintf (request_headers, 1000, request_template, url_path, m_otp_host,
			cookie, strlen(body));

	//fprintf(stderr, "DEBUG: Sending POST Headers: \n%s\n", request_headers);
	write_to_stream(m_ssl_connection, request_headers);

	//fprintf(stderr, "DEBUG: Sending POST Body: \n%s\n", body);

	write_to_stream(m_ssl_connection, body);

	//Read response
	unsigned int ret = read_from_stream(m_ssl_connection, server_response, REPLY_BUFSIZE);

	if (ret == 0)
	   ERR_print_errors_fp(stderr); //Connection aborted by server

	//fprintf(stderr, "Server reply: \n%s\n", server_response);

	return server_response;

}

/**
 * Connect to a host using an encrypted stream
 */
SSL* SSLConnection::connect_encrypted(char* host_and_port) 
{

    BIO* bio = NULL;
    int r = 0;

    /* Set up the SSL pointers */
    SSL_CTX *ctx = SSL_CTX_new(SSLv3_client_method());
    SSL *ssl = NULL;

    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    RSA *rsa = RSA_new();

    rsa->flags |= RSA_FLAG_SIGN_VER;

    
    RSA_METHOD *current_method = (RSA_METHOD *)RSA_PKCS1_SSLeay();
    current_method->rsa_sign = eIDMW::rsa_sign;
    current_method->flags |= RSA_FLAG_SIGN_VER;
    current_method->flags |= RSA_METHOD_FLAG_NO_CHECK;

    RSA_set_method(rsa, current_method);

    //NOTE: to get more debug output from the SSL layer uncomment this
    //SSL_CTX_set_info_callback(ctx, eIDMW::get_ssl_state_callback);
    if (SSL_CTX_use_RSAPrivateKey(ctx, rsa) != 1)
    {
        fprintf(stderr, "SSL_CTX_use_RSAPrivateKey failed!");
        return NULL;
    }

    loadUserCert(ctx);	

    X509_STORE *store = SSL_CTX_get_cert_store(ctx);

    loadCertChain(store);

    if(!(SSL_CTX_load_verify_locations(ctx,
			NULL, "/etc/ssl/certs")))
	fprintf(stderr, "Can't read CA list\n");
	
    /* 
     * WARNING: the next line should be commented out to properly verify the server certificates      
     * We leave it like this because the current production server has one expired certificate
     * and we can't do much about it...
     */
    //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    bio = BIO_new_connect(host_and_port);

    SSL *ssl_handle = SSL_new(ctx);
    SSL_set_bio (ssl_handle, bio, bio);

    SSL_set_connect_state(ssl_handle);

    int ret_connect = SSL_connect(ssl_handle);

    if (ret_connect != 1)
    {
	translate_ssl_error(ssl_handle, ret_connect);
	MWLOG(LEV_ERROR, MOD_APL, L"SSLConnection: error establishing connection");

	throw CMWEXCEPTION(EIDMW_OTP_CONNECTION_ERROR);
    }

    return ssl_handle;
}

/**
 * Read from a stream and handle restarts and buffering if necessary
 */
unsigned int SSLConnection::read_from_stream(SSL* ssl, char* buffer, unsigned int buffer_length)
{

    unsigned int r = -1;
    unsigned int i = 0;
    unsigned int bytes_read = 0, header_len=0, content_length = 0;

    int err = 1;
    do
    {
	    // We're using blocking IO so SSL_Write either succeeds completely or not...
	    r = SSL_read(ssl, buffer+bytes_read, buffer_length-bytes_read);
	    if (bytes_read == 0)
	    {
		   
		    header_len = r;
		    char * buffer_tmp = (char*)calloc(strlen(buffer)+1, 1);
		    strcpy(buffer_tmp, buffer);
		    content_length = parseContentLength(buffer_tmp);
	    }

	    if (r <= 0) {

		    translate_ssl_error(ssl, r);

	    }
	    else
		    bytes_read += r;
    }
    while( bytes_read - header_len  < content_length );

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

    APL_Config otp_server(CConfig::EIDMW_CONFIG_PARAM_GENERAL_OTP_SERVER);
    std::string otp_server_str = otp_server.getString();
    char * host_and_port = (char *)otp_server_str.c_str();

    //Split hostname and port
    m_otp_host = (char *)malloc(strlen(host_and_port)+1);
    strcpy(m_otp_host, host_and_port);
    char * delim = strchr(m_otp_host, ':');
    *delim = '\0';

    /* initialise the OpenSSL library */
    init_openssl();


    if ((m_ssl_connection = connect_encrypted(host_and_port)) == NULL)
            return false;

    return true;
}
}
