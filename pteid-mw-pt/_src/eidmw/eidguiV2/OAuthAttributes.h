/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef OAUTH_ATTRIBUTES_H_
#define OAUTH_ATTRIBUTES_H_

#include <string>
#include <vector>
#include <map>
#include "Export.h"
#include "AttributeFactory.h"
#include <QTcpServer>
#include <QTcpSocket>

namespace eIDMW
{

    enum OAuthResult { 
        OAuthSuccess = 0,
        OAuthTimeoutError = 1,
        OAuthGenericError = 2,
        OAuthConnectionError = 3,
        OAuthCancelled = 4
    };

    class OAuthAttributes {
    public:
        OAuthAttributes();
        OAuthAttributes(std::vector<CitizenAttribute> attributes);
        ~OAuthAttributes();
        void setAttributes(std::vector<CitizenAttribute>);
        std::map<CitizenAttribute, std::string>* getAttributes();
        void clearAttributes();
        QTcpServer *getServer();
        std::string getToken();

        OAuthResult fetchAttributes();
        void closeListener();

    private:
        OAuthResult fetchToken();
        void openBrowser(unsigned short listeningPort);
        OAuthResult parseRequestToken(QByteArray httpRequest);
        void showReturnPage(QTcpSocket *connection, bool tokenSuccess);
        OAuthResult requestResources();
        void parseAttributes(const char * response);
        static size_t curl_write_data(char *dest, size_t size, size_t nmemb, void * buffer);

        std::map<CitizenAttribute, std::string> m_attributes;
        std::string m_secretToken;
        std::string m_redirectUri;
        std::string m_serviceEndpoint;  // default: autenticacao.gov.pt
        std::string m_oauthClientId;
        QTcpServer m_server;
        bool m_wasCancelled;
    };
}
#endif
