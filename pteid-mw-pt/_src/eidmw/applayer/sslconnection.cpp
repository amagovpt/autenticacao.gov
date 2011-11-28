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

#include <iostream>

#include <stdio.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/engine.h>

namespace eIDMW
{
static const char *FIRST_POST=
    "POST /CAPPINChange/connect HTTP/1.1\nHost: otp.cartaodecidadao.pt\nContent-Type: text/plain; charset=UTF-8\nContent-Length: 16\r\n\r\n{\"connect\":\"\"}\r\n";

/*
char *SECOND_POST=
    "POST /CAPPINChange/sendParameters HTTP/1.1\r\nHost: otp.cartaodecidadao.pt\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: 214\r\n";
*/
static char *host="otp.cartaodecidadao.pt";
static int port=443;
static int require_server_auth=1;
static char *ciphers=0;
static int reconnect=1;
static char token[80];

/* Read from the POST and write to the server
   Read from the server and write to the POST

   we use select() to multiplex
*/ 

char params[100000];

char *getparameters (const char *pin, const char *pan, const char *arqc, const char *cdol1, const char *atc, const char *panseqnr, const char *counter, const char *pintrycounter)
{
	//printf ("test pin: %s ; %s ; %s ; %s ; %s ; %s ; %s ; %s\n", pin, pan, arqc, cdol1, atc, panseqnr, counter, pintrycounter);

	//char params[100000];
	sprintf (params, "{\"PinPafUpdate\":{\"pan\":\"%s\",\"pin\":\"%s\",\"panseqnumber\":\"%s\",\"cdol1\":\"%s\",\"atc\":\"%s\",\"arqc\":\"%s\",\"counter\":\%s,\"pintrycounter\":%s}}", pan, pin, panseqnr, cdol1, atc, arqc, counter, pintrycounter);

	printf ("string final parametros %s\n", params);

	init();

	return params;
}

void read_write_buf(SSL * ssl, int sock, const char * buf, int len) 
{
	int width;
	int r,c2sl=0,c2s_offset=0;
	int read_blocked_on_write=0,write_blocked_on_read=0,read_blocked=0;
	fd_set readfds,writefds;
	int shutdown_wait=0;
	char c2s[BUFSIZZ],s2c[BUFSIZZ];
	int ofcmode;
	int rx_count = 0;

	/*First we make the socket nonblocking*/
	ofcmode=fcntl(sock,F_GETFL,0);
	ofcmode|=O_NDELAY;
	if(fcntl(sock,F_SETFL,ofcmode))
		err_exit("Couldn't make socket nonblocking");


	width=sock+1;

	/* Check for input on the console*/
	strcpy(c2s, buf);
	c2sl = strlen(c2s);
	c2s_offset=0;
	printf("Init str bytes (%d): \n---\n string %s\n---\n", c2sl, c2s);

	while(1){
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		FD_SET(sock,&readfds);

		/* If we're waiting for a read on the socket don't
         try to write to the server */
		if(!write_blocked_on_read){
			/* If we have data in the write queue don't try to
           read from stdin */
			if(c2sl || read_blocked_on_write) {
				printf("Using Write FDS only\n");
				FD_SET(sock,&writefds);
			}
		}

		r=select(width,&readfds,&writefds,0,0);
		if(r==0)
			continue;

		/* Now check if there's data to read */
		if((FD_ISSET(sock,&readfds) && !write_blocked_on_read) ||
				(read_blocked_on_write && FD_ISSET(sock,&writefds))){
			do {
				read_blocked_on_write=0;
				read_blocked=0;

				r=SSL_read(ssl,s2c,BUFSIZZ);

				switch(SSL_get_error(ssl,r)){
				case SSL_ERROR_NONE:
					/* Note: this call could block, which blocks the
					 entire application. It's arguable this is the
					 right behavior since this is essentially a terminal
					 client. However, in some other applications you
					 would have to prevent this condition */
					fwrite(s2c,1,r,stdout);
					//////////////
					char *substr;
					substr = strstr(s2c, "JSESSIONID=");
					strncpy (token , substr, 65);
					std::cout << "TOKEN: " <<  token << std::endl;
					/////////////////
					fflush(stdout);

					rx_count++;

					/* rx header and body */
					if ( rx_count == 2)
						return;


					/*
					if ( r == 14) {
						printf(">>>> Success: exiting read_write\n");
						goto end;
					}
					*/

					break;
				case SSL_ERROR_ZERO_RETURN:
					/* End of data */
					if(!shutdown_wait)
						SSL_shutdown(ssl);
					goto end;
					break;
				case SSL_ERROR_WANT_READ:
					printf("Read block WANT_READ on Write:  SSL_ERROR_WANT_READ\n");
					read_blocked=1;
					break;

					/* We get a WANT_WRITE if we're
					 trying to rehandshake and we block on
					 a write during that rehandshake.

					 We need to wait on the socket to be
					 writeable but reinitiate the read
					 when it is */
				case SSL_ERROR_WANT_WRITE:
					read_blocked_on_write=1;
					printf("Read block on Write:  SSL_ERROR_WANT_WRITE\n");
					break;
				default:
					berr_exit("SSL read problem");
				}

				/* We need a check for read_blocked here because
				 SSL_pending() doesn't work properly during the
				 handshake. This check prevents a busy-wait
				 loop around SSL_read() */
			} while (SSL_pending(ssl) && !read_blocked);
		}

		/* If the socket is writeable... */
		if((FD_ISSET(sock,&writefds) && c2sl) ||
				(write_blocked_on_read && FD_ISSET(sock,&readfds))) {
			write_blocked_on_read=0;

			/* Try to write */
			r=SSL_write(ssl,c2s+c2s_offset,c2sl);

			printf("SSL_write: Sent %d bytes\n", r);

			switch(SSL_get_error(ssl,r)){
			/* We wrote something*/
			case SSL_ERROR_NONE:
				c2sl-=r;
				c2s_offset+=r;

				if (c2sl == 0) {
					printf("Finished sending buf\n");
				}
				break;

				/* We would have blocked */
			case SSL_ERROR_WANT_WRITE:
				break;

				/* We get a WANT_READ if we're
               trying to rehandshake and we block on
               write during the current connection.

               We need to wait on the socket to be readable
               but reinitiate our write when it is */
			case SSL_ERROR_WANT_READ:
				write_blocked_on_read=1;
				printf("(SSL_WRITE) Write blocked on WANT_READ on Write\n");

				break;

				/* Some other error */
			default:
				berr_exit("SSL write problem");
			}
		}
	}

	end:
	return;
}



int init ()
{
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *sbio;
	int sock;
	extern char *optarg;

	SSL_SESSION *sess;
	/* Build our SSL context*/
	ctx=initialize_ctx();

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
		berr_exit((char *)"SSL connect error");
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
			berr_exit((char *)"SSL connect error (second connect)");
		check_cert(ssl,host);
	}

	/* Now make our HTTP request */

	printf("\nREQUEST - 1\n");
	char buf[30000];
	memset(buf, 0, 30000);
	int len = 0;
	strcpy (buf, FIRST_POST);
	len = strlen(buf);
	read_write_buf(ssl, sock, buf, len);

	printf ("FINAL %s", token);
	//token[80] = '\0';
	char *OLA;
	OLA = strdup(token);
	char SECOND_POST[3000];
	sprintf (SECOND_POST, "POST /CAPPINChange/sendParameters HTTP/1.1\nHost: otp.cartaodecidadao.pt\nContent-Type: text/plain; charset=UTF-8\nContent-Length: 215\nCookie: ");
	strcat(SECOND_POST, token);
	strcat(SECOND_POST, "\r\n\r\n");
	char *fpar = params;
	strcat(SECOND_POST, fpar);
	strcat(SECOND_POST, "\r\n");

	printf("\nREQUEST - 2\n");
	read_write_buf(ssl, sock, SECOND_POST, len);

	/* Shutdown the socket */
	SSL_free(ssl);
	destroy_ctx(ctx);
	close(sock);

	exit(0);
}
}
