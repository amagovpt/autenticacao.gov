/*
 * sslcommon.cpp
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
#include <openssl/err.h>
#include <openssl/engine.h>

BIO *bio_err=0;
static int password_cb(char *buf,int num, int rwflag,void *userdata);
static void sigpipe_handle(int x);
static X509 *m_cert1;

/* A simple error and exit routine*/

int err_exit(char *str)
{
	fprintf(stderr,"%s\n",str);
	exit(0);
}

/* Print SSL errors and exit*/
int berr_exit(char *str)
{
	BIO_printf(bio_err,"%s\n",str);
	ERR_print_errors(bio_err);
	exit(0);
}

static void sigpipe_handle(int x){
}

SSL_CTX *initialize_ctx()
{
    	#ifdef WIN32
    	const SSL_METHOD *meth;
	#endif
	SSL_METHOD *meth;
	SSL_CTX *ctx;

	if(!bio_err){
		/* Global system initialization*/
		SSL_library_init();
		SSL_load_error_strings();

		/* An error write context */
		bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
	}

	//////////////PKCS111 ENGINE///////////////

	struct {
		const char * slot_id;
		X509 * cert;
	} parms = {"4500000000000000",NULL};

	//X509 * m_cert;
	EVP_PKEY *m_pkey;

	ENGINE *engine;

	ENGINE_load_builtin_engines();

	engine = ENGINE_by_id("dynamic");
	ENGINE_ctrl_cmd_string(engine,"SO_PATH", "/usr/lib/engines/engine_pkcs11.so" ,0);
	ENGINE_ctrl_cmd_string(engine,"ID","pkcs11",0);
	ENGINE_ctrl_cmd_string(engine,"LIST_ADD","0",0);
	ENGINE_ctrl_cmd_string(engine,"LOAD",NULL,0);
	ENGINE_ctrl_cmd_string(engine,"VERBOSE",0,0);
	ENGINE_ctrl_cmd_string(engine,"MODULE_PATH", "/usr/local/lib/libpteidpkcs11.so",0);
	ENGINE_init(engine);


	ENGINE_ctrl_cmd(engine, "LOAD_CERT_CTRL", 0, &parms, NULL, 1);

	m_cert1 = parms.cert;

	if (!(m_pkey = ENGINE_load_private_key(engine,parms.slot_id, NULL, NULL)))
		berr_exit((char *)"Error loading private key. Do you have a card ?");

	///////////////////////////////////////////


	/* Set up a SIGPIPE handler */
	signal(SIGPIPE,sigpipe_handle);

	/* Create our context*/
	meth=SSLv23_method();
	ctx=SSL_CTX_new(meth);

	/* Load our keys and certificates from a file*/
	/*if(!(SSL_CTX_use_certificate_chain_file(ctx,
      keyfile)))
      berr_exit("Can't read certificate file");*/

	/*load our certificates from the card*/
	if(!(SSL_CTX_use_certificate(ctx,m_cert1)))
		berr_exit((char *)"Can't read certificate file");

	/*load our private keys from the card*/
	if(!(SSL_CTX_use_PrivateKey(ctx, m_pkey)))
		berr_exit((char *)"Can't read key file");

	/* Load the CAs we trust*/
	if(!(SSL_CTX_load_verify_locations(ctx,
			CA_LIST, "/etc/ssl/certs")))
		berr_exit((char *)"Can't read CA list");
#if (OPENSSL_VERSION_NUMBER < 0x00905100L)
	SSL_CTX_set_verify_depth(ctx,1);
#endif

	return ctx;
}

void destroy_ctx(SSL_CTX *ctx)
{
	SSL_CTX_free(ctx);
}
