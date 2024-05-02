/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef SSLCONNECTION_H_
#define SSLCONNECTION_H_

#include <string>
#include "Export.h"

typedef void CURL;

namespace eIDMW {

class APL_Card;

class SSLConnection {
public:
	EIDMW_APL_API SSLConnection(APL_Card *card);
	EIDMW_APL_API ~SSLConnection(){};

	EIDMW_APL_API CURL *connect_encrypted();

private:
	CURL *m_curl;
	APL_Card *m_card;
};

} // namespace eIDMW

#endif // Include guard
