#include "OAuthAttributes.h"

#include <iostream>
#include "Log.h"
#include "MiscUtil.h"
#include "cJSON.h"
#include <sstream>
#include <regex>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

#include "Util.h"
#include "Config.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <curl/curl.h>

#define FA_OAUTH_ASK_AUTH           "/OAuth/AskAuthorization"               // service to obtain token
#define FA_OAUTH_API_RESOURCE       "/OAuthResourceServer/Api/Resource"     // service to obtain citizen attributes with token

#define RESPONSE_TYPE       "token"

#define GET_TOKEN_TIMEOUT   10*60       // (10min) time limit to receive connection with new token (time to complete the get token procedure)
#define RECV_TOKEN_TIMEOUT  10*1000     // (10s)   time limit to receive token http request after connection 
#define SEND_TOKEN_TIMEOUT  10*1000     // (10s)   time limit to send return page to server

#define HTML_BATCH_SIZE     1024*1024

#define TOKEN_SIZE          36

namespace eIDMW
{
    OAuthAttributes::OAuthAttributes() {
        APL_Config oauth_host_config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_HOST);
        APL_Config oauth_port_config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_PORT);
        APL_Config oauth_clientid_config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_CLIENTID);

        m_serviceEndpoint
            .append(oauth_host_config.getString()).append(":")
            .append(oauth_port_config.getString());
        m_wasCancelled = false;
        m_secretToken = "";
        m_oauthClientId.assign(oauth_clientid_config.getString());
    }
    OAuthAttributes::OAuthAttributes(std::vector<CitizenAttribute> attributes) : OAuthAttributes() {
        setAttributes(attributes);
    }
    OAuthAttributes::~OAuthAttributes(){
        std::cout << "Closing server" << std::endl;
        m_server.close();
    }
    void OAuthAttributes::setAttributes(std::vector<CitizenAttribute> attributes) {
        m_attributes.clear();
        for (size_t i = 0; i < attributes.size(); i++)
        {
            m_attributes.insert(std::pair<CitizenAttribute, std::string>(attributes[i], std::string()));
        }
    }
    std::map<CitizenAttribute, std::string>* OAuthAttributes::getAttributes() {
        return &m_attributes;
    }
    void OAuthAttributes::clearAttributes() {
        m_attributes.clear();
    }
    QTcpServer* OAuthAttributes::getServer() {
        return &m_server;
    }
    std::string OAuthAttributes::getToken() {
        return m_secretToken;
    }

    OAuthResult OAuthAttributes::fetchAttributes(){
        OAuthResult result;
        
        result = fetchToken();
        if (result != OAuthSuccess) {
            return result;
        }

        result = requestResources();
        if (result != OAuthSuccess){
            return result;
        }
        
        //DEBUG
        std::cout << "Attributes fetched:" << std::endl;
        for (auto const& attr : m_attributes)
        {
            std::cout << "Attr: " << attributeToUri(attr.first) << " -> " << attr.second << std::endl;
        }

        return result;
    }

    void OAuthAttributes::openBrowser(unsigned short listeningPort) {

        std::string faAuthReq("https://");
        faAuthReq.append(m_serviceEndpoint).append(FA_OAUTH_ASK_AUTH);
        faAuthReq.append("?client_id=").append(m_oauthClientId);
        faAuthReq.append("&response_type=").append(RESPONSE_TYPE);
        faAuthReq.append("&redirect_uri=").append("http://127.0.0.1:").append(std::to_string(listeningPort));
        faAuthReq.append("&scope=");
        for (auto const& attr : m_attributes)
        {
            faAuthReq.append(attributeToUri(attr.first)).append("%20");
        }
        const char * url = faAuthReq.c_str();

        MWLOG(LEV_DEBUG, MOD_APL, "OAuthAttribues::openBrowser URL: %s", url);
        QDesktopServices::openUrl(QUrl(url));
    }

    OAuthResult OAuthAttributes::parseRequestToken(QByteArray httpRequest) {
        std::stringstream httpReq(httpRequest.toStdString());
        std::string token;
        std::getline(httpReq, token); // only care about first line
        
        size_t tokenPos = token.find("access_token=");
        if (tokenPos == std::string::npos)
        {
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues:parseRequestToken() token not received in query string");
            return OAuthGenericError;
        }
        token = token.substr(tokenPos + 13, TOKEN_SIZE);
        std::cout << "token parsed: " << token << std::endl;

        std::regex tokenValidation("[a-z0-9]{8}-([a-z0-9]{4}-){3}[a-z0-9]{12}");

        if (!std::regex_match(token, tokenValidation))
        {
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues:parseRequestToken() token received is not valid!");
            return OAuthGenericError;
        }
        m_secretToken.assign(token); 
        return OAuthSuccess;
    }

    void OAuthAttributes::showReturnPage(QTcpSocket *conn, bool tokenSuccess) {
        std::string pagePath = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_WEB_DIR));
        if (tokenSuccess) {
            pagePath.append("/AutenticacaoGov.html");
        }
        else {
            pagePath.append("/AutenticacaoGov_failed.html");
        }

#ifdef WIN32
        QFile inputFile(QString::fromLatin1(pagePath.c_str()));
#else
        QFile inputFile(pagePath.c_str());
#endif
        if (inputFile.open(QIODevice::ReadOnly))
        {
            conn->write("HTTP/1.0 200 OK\r\n");
            conn->write("Content-Type: text/html; charset=\"utf-8\"\r\n\r\n");
            QTextStream in(&inputFile);
            while (!in.atEnd())
            {
                QString batch = in.read(HTML_BATCH_SIZE);
                conn->write(batch.toLocal8Bit());
            }
            inputFile.close();
            if (!conn->waitForBytesWritten(SEND_TOKEN_TIMEOUT))
            {
                MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues: connection failed: could not return page!");
            }
        }
        else {
            MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues: open html file %s error: %d: %s", pagePath.c_str(), inputFile.error(), inputFile.errorString().toLocal8Bit().data());
        }
        conn->close();
    }

    OAuthResult OAuthAttributes::fetchToken() {
        // Open listener
        if (!m_server.listen(QHostAddress::LocalHost)) {
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues: open listener server failed");
            return OAuthConnectionError;
        }
        std::cout << "listening in port: " << std::to_string(m_server.serverPort()) << std::endl;

        openBrowser(m_server.serverPort());

        // POLLING
        bool hasConn = false;
        int seconds = 0;
        while ( seconds < GET_TOKEN_TIMEOUT && !hasConn && !m_wasCancelled) {
            hasConn = m_server.waitForNewConnection(1000);
            seconds++;
        }
        if (!hasConn || m_wasCancelled)
        {
            m_server.close();
            MWLOG(LEV_DEBUG, MOD_APL, L"OAuthAttribues:get token timed out or process was canceled");
            return (m_wasCancelled ? OAuthCancelled : OAuthTimeoutError);
        }

        QTcpSocket *conn = m_server.nextPendingConnection();
        QByteArray buffer;
        if (!conn || !conn->waitForReadyRead(RECV_TOKEN_TIMEOUT)){
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues: callback with token connection failed: can't establish new connection or not ready for read.");
            return OAuthConnectionError;
        }
        
        buffer = conn->readAll();
        if (buffer.isEmpty())
        {
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues: callback with token connection failed: Could not read to buffer.");
            return OAuthConnectionError;
        }
        /* We are going to send the "success" page now because it's the one containing 
         the logic to transform the fragment identifier in the url into a request with query string.
         The page won't be shown yet because there is no token in the query string
         (this logic is in embedded in the html of the page).
         */
        showReturnPage(conn, true);
        if (!m_server.hasPendingConnections())
        {
            m_server.waitForNewConnection(10 * 1000); // 10 seconds should be more than enough for localhost redirection
        }
        conn = m_server.nextPendingConnection();
        if (!conn || !conn->waitForReadyRead(RECV_TOKEN_TIMEOUT)){
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues: redirect with token connection failed");
            return OAuthConnectionError;
        }
        buffer = conn->readAll();
        OAuthResult tokenSucess = parseRequestToken(buffer);
        showReturnPage(conn, tokenSucess == OAuthSuccess);
        return tokenSucess;
    }

    size_t OAuthAttributes::curl_write_data(char *recv, size_t size, size_t nmemb, void * buffer)
    {
        MWLOG(LEV_DEBUG, MOD_APL, "OAuthAttribues::curl_read_data Read: %s", recv);
        size_t realsize = size * nmemb;
        ((std::string *)buffer)->append((char*)recv, realsize);
        return realsize;
    }

    void OAuthAttributes::parseAttributes(const char * response) {
        if (response == NULL || strlen(response) == 0)
        {
            MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues::parseAtributes() Can't parse null/empty string!");
            return;
        }
        cJSON *json_array = cJSON_Parse(response);

        for (int i = 0; i < cJSON_GetArraySize(json_array); i++)
        {
            cJSON *json_attribute = cJSON_GetArrayItem(json_array, i);
            cJSON *name = cJSON_GetObjectItem(json_attribute, "name");
            cJSON *value = cJSON_GetObjectItem(json_attribute, "value");
            CitizenAttribute attributeName = uriToAttribute(name->valuestring);

            if (attributeName == NONE || m_attributes.find(attributeName) == m_attributes.end())
            {
                MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues::parseAtributes received invalid attribute name: %s", name->valuestring);
                continue; // Parse all even if some citizen attributes are unrequested/malformed 
            }
            m_attributes[attributeName].assign(value->valuestring);
        }
    }

    OAuthResult OAuthAttributes::requestResources() {
        CURL *curl;
        /*curl_global_cleanup should be called for every curl_global_init
        and calling curl_global_cleanup makes libraries such as OpenSSL 
        unusable (at least until curl_global_init is called again).
        For this reason, we are relying on fail-safe initialization despite 
        not being thread-safe*/
#ifdef _WIN32
       // curl_global_init(CURL_GLOBAL_DEFAULT);
#else
       // curl_global_init(CURL_GLOBAL_SSL);
#endif
        curl = curl_easy_init();
        if (curl == NULL)
        {
            MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues: curl_easy_init() failed!");
            return OAuthGenericError;
        }

        std::string apiEndpoint("https://");
        apiEndpoint.append(m_serviceEndpoint).append(FA_OAUTH_API_RESOURCE);

        curl_easy_setopt(curl, CURLOPT_URL, apiEndpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, PTEID_USER_AGENT);
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string faResourceReq("{\"token\": \"");
        faResourceReq.append(m_secretToken);
        faResourceReq.append("\", \"attributesName\": [");
        for (auto const& attr : m_attributes)
        {
            faResourceReq.append("\"").append(attributeToUri(attr.first)).append("\",");
        }
        faResourceReq.append("]}");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, faResourceReq.c_str());

        std::string responseBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

#ifdef __linux__
        char * ca_path = "/etc/ssl/certs";
        curl_easy_setopt(curl, CURLOPT_CAPATH, ca_path);
#elif WIN32
        std::string cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR)) + "\\cacerts.pem";
        curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_file.c_str());
#elif __APPLE__
        std::string cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";
        curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_file.c_str());
#endif
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // for DEBUG

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            MWLOG(LEV_ERROR, MOD_APL, L"OAuthAttribues: curl_easy_perform() failed: %s", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return OAuthGenericError;
        }

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200)
        {
            MWLOG(LEV_ERROR, MOD_APL, "OAuthAttribues: Server response http code: %d with error: %s", http_code, responseBuffer.c_str());
            curl_easy_cleanup(curl);
            return OAuthGenericError;
        }
        parseAttributes(responseBuffer.c_str());
        curl_easy_cleanup(curl);
        return OAuthSuccess;
    }

    void OAuthAttributes::closeListener() {
        m_wasCancelled = true;
    }
}
