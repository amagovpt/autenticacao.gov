#ifndef SSLCONNECTION_H_
#define SSLCONNECTION_H_

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <curl/curl.h>

#include "APLCard.h"
#include "APLReader.h"
#include "SAM.h"
#include "EMV-Cap-Helper.h"
#include "Export.h"

namespace eIDMW
{

// This callback will get passed to OpenSSL and it can't be a member function of SSLConnection because
// it will be called by C code which is not expecting the additional "this" pointer 
// as first argument

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
	                unsigned char *sigret, unsigned int *siglen, const RSA * rsa);

enum SSLConnectionTarget
{
	OTP_SERVER,
	ADDRESS_CHANGE_SERVER
};

class SSLConnection
{
	public:

		EIDMW_APL_API SSLConnection(SSLConnectionTarget target_server)
		{
		InitConnection(target_server);
		};
		EIDMW_APL_API ~SSLConnection() 
		{
		CloseConnection();
		};

		/* The following functions implement the communication protocol for the Address Change process */

		SignedChallengeResponse *do_SAM_2ndpost(char *challenge, char *kicc);
		//Returns the session cookie
		DHParamsResponse *do_SAM_1stpost(DHParams *params, char *secretCode, char *process);

		StartWriteResponse *do_SAM_3rdpost(char * mse_resp, char *internal_auth_resp);
		bool do_SAM_4thpost(StartWriteResponse &resp);
		
		/* The following functions implement the communication protocol with the OTP server */
		char * do_OTP_1stpost();

		//Returns encrypted EMV-CAP changePin APDU
		char * do_OTP_2ndpost(char * cookie, OTPParams *params);
		void do_OTP_3rdpost(char *cookie, const char *change_pin_response);
		char * do_OTP_4thpost(char *cookie, OTPParams *params);
		void do_OTP_5thpost(char *cookie, const char *reset_scriptcounter_response);

	private:
		void ReadUserCert();
		//Generic POST routine that actually writes and reads from the SSL connection
		char * Post(char *cookie, char *url_path, char *body, bool chunked_expected=false);

		unsigned int write_to_stream(SSL* bio, char* req_string);
		unsigned int read_from_stream(SSL* bio, char* buffer, unsigned int length);
		unsigned int read_chunked_reply(SSL *bio, char* buffer, unsigned int length);
		SSL *connect_encrypted(char *host_and_port);
		bool InitConnection(SSLConnectionTarget target);
		void CloseConnection();
		void loadUserCert(SSL_CTX *ctx);
		void loadCertChain(X509_STORE *store);

		char *m_session_cookie;

		RSA *current_private_key;
		//BIO *m_bio;
		SSL * m_ssl_connection;
		//Hostname of our OTP server
		char * m_otp_host;
		//APL_Card card_handle;


};

}

#endif //Include guard
