#ifndef SCAPSSL_H_
#define SCAPSSL_H_

#include "SSLConnection.h"

namespace eIDMW
{

class ScapSSLConnection: public SSLConnection
{
	public:
		EIDMW_APL_API ScapSSLConnection() { };
		EIDMW_APL_API ScapSSLConnection(char *host, char *port);
		EIDMW_APL_API ~ScapSSLConnection() { /* CloseConnection(); */ }

		EIDMW_APL_API char * postSoapRequest(char *endpoint, char *soapAction, char *soapBody);

};


}

#endif
