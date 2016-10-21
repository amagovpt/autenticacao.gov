#ifndef SCAPSSL_H_
#define SCAPSSL_H_

#include "SSLConnection.h"

namespace eIDMW
{

class ScapSSLConnection: public SSLConnection
{
	public:
		ScapSSLConnection() { };
		ScapSSLConnection(char *host);
		~ScapSSLConnection() { /* CloseConnection(); */ }

		char * postSoapRequest(char *endpoint, char *soapAction, char *soapBody);

};


}

#endif