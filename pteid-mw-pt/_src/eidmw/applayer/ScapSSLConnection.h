/*-****************************************************************************

 * Copyright (C) 2016 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef SCAPSSL_H_
#define SCAPSSL_H_

#include "SSLConnection.h"

namespace eIDMW
{

//Forward-declare it for the pointer: no need to be a complete type at this point
class APL_Card;

void setThreadLocalCardInstance(APL_Card * card);

class ScapSSLConnection: public SSLConnection
{
	public:
		//EIDMW_APL_API ScapSSLConnection() { };
		EIDMW_APL_API ScapSSLConnection(APL_Card * card, char *host, char *port);
		EIDMW_APL_API ~ScapSSLConnection() { /* CloseConnection(); */ }

		EIDMW_APL_API char * postSoapRequest(char *endpoint, char *soapAction, char *soapBody);

};


}

#endif
