#ifndef SSLCONNECTION_H_
#define SSLCONNECTION_H_

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>

#include "APLCard.h"
#include "APLReader.h"
#include "SAM.h"
#include "Export.h"

namespace eIDMW
{

// This callback will get passed to OpenSSL and it can't be a member function of SSLConnection because
// it will be called by C code which is not expecting the additional "this" pointer 
// as first argument
int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
	                unsigned char *sigret, unsigned int *siglen, const RSA * rsa);

#define REPLY_BUFSIZE 100000
#define SCAP_REPLY_BUFSIZE 600000

class APL_Certifs;

struct NetworkBuffer {
	char * buf;
	unsigned int buf_size;
};

class SSLConnection
{
	public:

		EIDMW_APL_API SSLConnection()
		{
			m_ssl_connection = NULL;
			m_host = NULL;
			m_session_cookie = NULL;
		};

		EIDMW_APL_API ~SSLConnection() 
		{
			CloseConnection();
		};

		bool InitSAMConnection();

		/* The following functions implement the communication protocol for the Address Change process */
		DHParamsResponse *do_SAM_1stpost(DHParams *params, char *secretCode, char *process, char *serialNumber);

		SignedChallengeResponse *do_SAM_2ndpost(char *challenge, char *kicc);

		//Returns the session cookie
		StartWriteResponse *do_SAM_3rdpost(char * mse_resp, char *internal_auth_resp);
		bool do_SAM_4thpost(StartWriteResponse &resp);

	protected:
		void ReadUserCert();
		static void init_openssl();
		//Generic POST routine that actually writes and reads from the SSL connection
		char * Post(char *cookie, char *url_path, char *body);

		unsigned int write_to_stream(SSL* bio, char* req_string);
		unsigned int read_from_stream(SSL* bio, NetworkBuffer * buffer);
		void read_chunked_reply(SSL *bio, NetworkBuffer * buffer, bool headersAlreadyRead=false);

		//Connect to the specified host and port, optionally using the deprecated TLSv1 protocol for compatibility with some servers
		void connect_encrypted(char *host_and_port);
		bool InitConnection();
		void CloseConnection();
		BIO * connectToProxyServer(const char * proxy_host, long proxy_port, char *ssl_host, char *proxy_user, char * proxy_pwd, char *ssl_host_andport);
		APL_Certif * loadCertsFromCard(SSL_CTX *ctx);
		void loadCertChain(X509_STORE *store, APL_Certif * authentication_cert);
		void loadAllRootCerts(X509_STORE *store);

		char *m_session_cookie;

		SSL * m_ssl_connection;

		APL_Certifs *m_certs;
		//Hostname of our OTP server
		char * m_host;
		//APL_Card card_handle;


};

}

#endif //Include guard
