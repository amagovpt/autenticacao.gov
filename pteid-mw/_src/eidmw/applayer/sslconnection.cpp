/*
 * sslconnection.cpp
 *
 *  Created on: 2011/10/25
 *      Author: Luis Medinas <luis.medinas@caixamagica.pt>
 *      based on Eric Rescorla OpenSSL Example Programs 20020110
 */

/*
 *  pteid-mw-ng is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  pteid-mw-ng is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "sslcommon.h"
#include "sslclient.h"
#include "sslconnection.h"

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/engine.h>

/*static char *REQUEST_TEMPLATE=
   "GET / HTTP/1.0\r\nUser-Agent:"
   "EKRClient\r\nHost: %s:%d\r\n\r\n";*/

static char *REQUEST_TEMPLATE=
		"POST /CAPPINChange/connect HTTP/1.1\r\nHost: otp.cartaodecidadao.pt\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: 16\r\n\r\n{\"connect\":\"\"}\r\n";

static char *host="otp.cartaodecidadao.pt";
//static char *host="localhost";
static int port=443;
static int require_server_auth=1;
static char *ciphers=0;
static int reconnect=1;

static int http_request(SSL *ssl)
{
	char *request=0;
	char buf[BUFSIZZ];
	int r;
	int len, request_len;

	/* Now construct our HTTP request */
	request_len=strlen(REQUEST_TEMPLATE)+
			strlen(host)+6;
	if(!(request=(char *)malloc(request_len)))
		err_exit("Couldn't allocate request");
	snprintf(request,request_len,REQUEST_TEMPLATE,
			host,port);

	/* Find the exact request_len */
	request_len=strlen(request);

	r=SSL_write(ssl,request,request_len);
	switch(SSL_get_error(ssl,r)){
	case SSL_ERROR_NONE:
		if(request_len!=r)
			err_exit("Incomplete write!");
		break;
	default:
		berr_exit("SSL write problem");
	}

	/* Now read the server's response, assuming
       that it's terminated by a close */
	while(1){
		r=SSL_read(ssl,buf,BUFSIZZ);
		switch(SSL_get_error(ssl,r)){
		case SSL_ERROR_NONE:
			len=r;
			break;
		case SSL_ERROR_WANT_READ:
			continue;
		case SSL_ERROR_ZERO_RETURN:
			goto shutdown;
		case SSL_ERROR_SYSCALL:
			fprintf(stderr,
					"SSL Error: Premature close\n");
			goto done;
		default:
			berr_exit("SSL read problem wclient.c");
		}

		buf[r] = '\0';
		FILE *fp;
		fwrite(buf,1,len,stdout);
		fread (buf, 1, len, stdout);

		char token[190];

		strncpy (token, buf+180, 67);
		token[67] = '\0';
		printf ("%s", token);

	}

	shutdown:
	r=SSL_shutdown(ssl);
	switch(r){
	case 1:
		break; /* Success */
	case 0:
	case -1:
	default:
		berr_exit("Shutdown failed");
	}

	done:
	SSL_free(ssl);
	free(request);
	return(0);
}

int init ()
{
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *sbio;
	int sock;
	extern char *optarg;
	int c;

	SSL_SESSION *sess;
	/* Build our SSL context*/
	ctx=initialize_ctx(KEYFILE);

	if(ciphers){
		SSL_CTX_set_cipher_list(ctx,ciphers);
	}


	/* Connect the TCP socket*/
	sock=tcp_connect(host,port);

	/* Connect the SSL socket */
	ssl=SSL_new(ctx);
	sbio=BIO_new_socket(sock,BIO_NOCLOSE);
	SSL_set_bio(ssl,sbio,sbio);

	if(SSL_connect(ssl)<=0)
		berr_exit("SSL connect error");
	if(require_server_auth)
		check_cert(ssl,host);

	/* Now hang up and reconnect, if requested */
	if(reconnect) {
		sess=SSL_get1_session(ssl); /*Collect the session*/
		SSL_shutdown(ssl);
		SSL_free(ssl);
		close(sock);

		sock=tcp_connect(host,port);
		ssl=SSL_new(ctx);
		sbio=BIO_new_socket(sock,BIO_NOCLOSE);
		SSL_set_bio(ssl,sbio,sbio);
		SSL_set_session(ssl,sess); /*And resume it*/
		if(SSL_connect(ssl)<=0)
			berr_exit("SSL connect error (second connect)");
		check_cert(ssl,host);
	}

	/* Now make our HTTP request */
	http_request(ssl);

	/* Shutdown the socket */
	destroy_ctx(ctx);
	close(sock);

	exit(0);
}
