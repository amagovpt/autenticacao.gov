
#include "SSLConnection.h"


/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* OpenSSL headers */
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bn.h>

namespace eIDMW
{

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
		unsigned char *sigret, unsigned int *siglen, const RSA * rsa)
{
	
	fprintf(stderr, "RSA_sign called with args type=%d input len=%d\n", type, m_len);

	APL_Card *card = AppLayer.getReader(0UL).getCard();

	if (card == NULL) {
	       	fprintf(stderr, "Failed to get card from global aplayer object\n"); 
       	}

	if (type != NID_md5_sha1)
	{
		fprintf(stderr, "rsa_sign called with wrong input type, it should be NID_md5_sha1!\n");
		return 0;
	}

	CByteArray to_sign(m, m_len);
	CByteArray signed_data;

	try
	{

		signed_data = card->Sign(to_sign, false); //Sign with Authentication Key

	}
	catch (...)
	{
		fprintf(stderr, "Exception caught in card.Sign()\n");
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


/**
 * Print SSL error details
 */
void print_ssl_error(char* message, FILE* out) {

    fprintf(out, "DEBUG: %s", message);
    //fprintf(out, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
    //fprintf(out, "%s\n", ERR_error_string(ERR_get_error(), NULL));
    ERR_print_errors_fp(out);
}

/**
 * Print SSL error details with inserted content
 */
void print_ssl_error_2(char* message, char* content, FILE* out) {

    fprintf(out, message, content);
    //fprintf(out, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
    //fprintf(out, "%s\n", ERR_error_string(ERR_get_error(), NULL));
    ERR_print_errors_fp(out);
}

/**
 * Initialise OpenSSL
 */
void init_openssl() {

    /* call the standard SSL init functions */
    SSL_load_error_strings();
    SSL_library_init();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    /* seed the random number system - only really nessecary for systems without '/dev/random' */
    /* RAND_add(?,?,?); need to work out a cryptographically significant way of generating the seed */
}

char *GetOtpParameters (const char *pan, const char *pin, const char *arqc, const char *cdol1, const char *atc,
	       	const char *panseqnr, const char *counter, const char *pintrycounter)
{
	char *params = (char *)malloc(1000);

	sprintf (params, "{\"PinPafUpdate\":{\"pan\":\"%s\",\"pin\":\"%s\",\"panseqnumber\":\"%s\",\"cdol1\":\"%s\",\"atc\":\"%s\",\"arqc\":\"%s\",\"counter\":\%s,\"pintrycounter\":%s}}", pan, pin, panseqnr, cdol1, atc, arqc, counter, pintrycounter);

	return params;
}

/**
 * Close an encrypted connection gracefully
 */
void SSLConnection::CloseConnection() {

    BIO_free_all(m_bio);

    fprintf(stderr, "DEBUG: SSL Connection closed.");

}

char *parseCookie(char *server_response)
{

	char *substr = NULL;
	substr = strstr(server_response, "JSESSIONID=");
	char * end_of_cookie = strchr(substr, '\n');
	size_t cookie_len = end_of_cookie - substr;

	char * cookie = (char *)malloc(cookie_len+1);
	strncpy (cookie, substr, cookie_len);
	cookie[cookie_len] = '\0';

	fprintf(stderr, "DEBUG: Captured Cookie: %s\n", cookie);

	return cookie;

}

#define REPLY_BUFSIZE 10000
/*
 * The return value of this method will be the cookie set by the server
 * if the connection is successfull
 */
char * SSLConnection::do_OTP_1stpost()
{
	char * cookie = NULL;

	char * server_request = 
    "POST /CAPPINChange/connect HTTP/1.1\nHost: otp.cartaodecidadao.pt\nContent-Type: text/plain; charset=UTF-8\nContent-Length: 14\r\n\r\n{\"connect\":\"\"}\r\n";
	char * server_response = (char *) malloc(REPLY_BUFSIZE);
	fprintf(stderr, "DEBUG: do_OTP_1stpost called() \n");

	write_to_stream(m_bio, server_request);

	//Read response
	ssize_t ret = read_from_stream(m_bio, server_response, REPLY_BUFSIZE);
	if (ret > 0)
		cookie = parseCookie(server_response);

	fprintf(stderr, "Server reply: \n%s\n", server_response);

	return cookie;

}

//TODO: Create a struct for the OTP Parameters and change the signature of the methods 
// accordingly
// TODO: Parse the APDU from the response
char * SSLConnection::do_OTP_2ndpost(char *cookie)
{
	//TODO: Test if the request is well-formed
	char * request_template= "POST /CAPPINChange/sendParameters HTTP/1.1\nHost: otp.teste.cartaodecidadao.pt\nContent-Type: text/plain; charset=UTF-8\nContent-Length: %d\nCookie: %s\r\n\r\n%s\r\n";
	char * server_response = (char *) malloc(REPLY_BUFSIZE);
	char * full_request = (char *) calloc(500, sizeof(char));
	
	fprintf(stderr, "DEBUG: do_OTP_2ndpost called() \n");

	char * otp_params = GetOtpParameters("0000000100277656", "1234", "CF9B15FF06FE7F7F",
               "0000000000000000000000000000800000000000000000000000000000", "0019", "00", "0", "3");
	//Build the full request
	snprintf (full_request, 500, request_template, strlen(otp_params)+4, cookie, otp_params);
	fprintf(stderr, "DEBUG: Sending 2nd Post: \n%s\n", full_request);

	write_to_stream(m_bio, full_request);

	//Read response
	ssize_t ret = read_from_stream(m_bio, server_response, REPLY_BUFSIZE);

	if (ret == 0)
	   ERR_print_errors_fp(stderr); //Connection aborted by server

	fprintf(stderr, "Server reply: \n%s\n", server_response);

	//TODO: parseChangePinAPDU()
	return server_response; 

}

/**
 * Connect to a host using an encrypted stream
 */
BIO* connect_encrypted(char* host_and_port, SSL_CTX** ctx, SSL** ssl) {

    BIO* bio = NULL;
    int r = 0;
    const char * chain_file = "/home/agrr/mychain.pem";
	const char * cert_file = "/home/agrr/my_auth_cert.pem";

    /* Set up the SSL pointers */
    *ctx = SSL_CTX_new(SSLv3_client_method());
    *ssl = NULL;

    RSA *rsa = RSA_new();

    rsa->flags |= RSA_FLAG_SIGN_VER;

    
    RSA_METHOD *current_method = (RSA_METHOD *)RSA_PKCS1_SSLeay();
    //ops.rsa_priv_enc = pkcs11_rsa_encrypt;
    //ops.rsa_priv_dec = pkcs11_rsa_decrypt;
    current_method->rsa_sign = eIDMW::rsa_sign;
    //ops.rsa_verify = pkcs11_rsa_verify;
    current_method->flags |= RSA_FLAG_SIGN_VER;
    current_method->flags |= RSA_METHOD_FLAG_NO_CHECK;

    RSA_set_method(rsa, current_method);
    
    //RSA_set_method(rsa,RSA_PKCS1_SSLeay());


    if (SSL_CTX_use_RSAPrivateKey(*ctx, rsa) != 1)
    {
        fprintf(stderr, "SSL_CTX_use_RSAPrivateKey failed!");
        return NULL;
    }

    r = SSL_CTX_use_certificate_chain_file(*ctx, chain_file);
    if (r == 0) {

        print_ssl_error_2("Unable to load the trust store from %s.\n", (char *)chain_file, stderr);
        return NULL;
    }
    if(SSL_CTX_use_certificate_file(*ctx, cert_file, SSL_FILETYPE_PEM) != 1)
    {
	    print_ssl_error_2("Unable to load the user authentication cert from %s.\n", (char *)cert_file, stderr);
	    return NULL;
    }
    if(!(SSL_CTX_load_verify_locations(*ctx,
			NULL, "/etc/ssl/certs")))
	fprintf(stderr, "Can't read CA list\n");


    /* Setting up the BIO SSL object */
    bio = BIO_new_ssl_connect(*ctx);
    BIO_get_ssl(bio, ssl);
    if (!(*ssl)) {

        print_ssl_error("Unable to allocate SSL pointer.\n", stderr);
        return NULL;
    }
    SSL_set_mode(*ssl, SSL_MODE_AUTO_RETRY);

    /* Attempt to connect */
    BIO_set_conn_hostname(bio, host_and_port);

    /* Verify the connection opened and perform the handshake */
    if (BIO_do_connect(bio) <= 0) {

        print_ssl_error_2("Unable to connect BIO.%s\n", host_and_port, stdout);
        return NULL;
    }
    if(BIO_do_handshake(bio) <= 0) {
	  print_ssl_error("Failed to perform SSL handshake!\n", stderr);
    }

    /*if (SSL_get_verify_result(*ssl) != X509_V_OK) {

        print_ssl_error("Unable to verify connection result.\n", stdout);
    }
    */

    return bio;
}

/**
 * Read a from a stream and handle restarts if nessecary
 */
ssize_t SSLConnection::read_from_stream(BIO* bio, char* buffer, ssize_t buffer_length) {

    ssize_t r = -1;
    unsigned int bytes_read = 0;
    while (bytes_read <= buffer_length) {
	
        r = BIO_read(bio, buffer+bytes_read, buffer_length - bytes_read);

        if (r <= 0) {
	  fprintf(stderr, "Finished reading or nothing left to read. BIO_Read() returned %d\n", 
			  r);
	  if (bytes_read == 0 && !BIO_should_retry(bio))
	  {
	     fprintf(stderr, "Connection closed don\'t even try to send any more data!\n");
             print_ssl_error("BIO_read should retry test failed.\n", stdout);

	  }
	  break;
        }

	bytes_read += r;

    }

    buffer[bytes_read] = '\0';

    return bytes_read;
}

/**
 * Write to a stream and handle restarts if nessecary
 */
int SSLConnection::write_to_stream(BIO* bio, char* request_string) {

    ssize_t r = -1;

    while (r < 0) {

        r = BIO_puts(bio, request_string);
        if (r <= 0) {

            if (!BIO_should_retry(bio)) {

                print_ssl_error("BIO_Write should retry test failed.\n", stdout);
                continue;
            }

            /* It would be prudent to check the reason for the retry and handle
             * it appropriately here */
        }
	else
	   fprintf(stderr, "Wrote %d bytes to the SSL Stream\n", r);

    }

    return r;
}

/**
 * Main SSL demonstration code entry point
 */
int SSLConnection::InitConnection()
{

    char buffer[4096];
    buffer[0] = 0;

    BIO* bio;
    SSL_CTX* ctx = NULL;
    SSL* ssl = NULL;
    char * host_and_port = "otp.cartaodecidadao.pt:443";

    /* initilise the OpenSSL library */
    init_openssl();


    if ((m_bio = connect_encrypted(host_and_port, &ctx, &ssl)) == NULL)
            return (EXIT_FAILURE);


    return (EXIT_SUCCESS);
}
}
