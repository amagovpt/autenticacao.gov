#ifndef SSLCONNECTION_H_
#define SSLCONNECTION_H_

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ssl.h>
#include <curl/curl.h>

#include "APLCard.h"
#include "APLReader.h"
#include "Export.h"

namespace eIDMW
{

// This callback will get passed to OpenSSL and it can't be a member function of SSLConnection because
// it will be called by C code which is not expecting the additional "this" pointer 
// as first argument

int rsa_sign(int type, const unsigned char *m, unsigned int m_len,
	                unsigned char *sigret, unsigned int *siglen, const RSA * rsa);
class SSLConnection
{
	public:

		EIDMW_APL_API SSLConnection()
		{
		InitConnection();
		};
		EIDMW_APL_API ~SSLConnection() 
		{
		CloseConnection();
		};
		
		//Send HTTP POST requests using the connection through the BIO object stored in m_bio
		EIDMW_APL_API char * do_OTP_1stpost();
		EIDMW_APL_API char * do_OTP_2ndpost(char * cookie);
		//EIDMW_APL_API void do_OTP_3rdpost();

	private:
		void ReadUserCert();

		int write_to_stream(BIO* bio, char* req_string);
		ssize_t read_from_stream(BIO* bio, char* buffer, ssize_t length);
		int InitConnection();
		void CloseConnection();

		//static CURLcode SetConnectionParams(CURL *curl, void *sslctx, void *parm);
		//User auth certificate to be used in SSL Configuration
		static unsigned char *user_auth_cert;
		static unsigned int user_cert_len;

		RSA *current_private_key;
		CURL * m_curl_handle;
		BIO *m_bio;
		//APL_Card card_handle;


};

}

#endif //Include guard
