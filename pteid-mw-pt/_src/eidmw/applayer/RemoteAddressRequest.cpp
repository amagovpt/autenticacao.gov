/*-****************************************************************************
 *
 *  Copyright (C) 2020-2021 André Guerreiro - <aguerreiro1985@gmail.com>
 *  Licensed under the EUPL V.1.2
 *
 *  RemoteAddress helper functions for HTTP API requests using libcurl
 */


#include <curl/curl.h>
#include <cstring>
#include <string>
#include <iostream>
#include <regex>

#include "Log.h"
#include "eidErrors.h"
#include "CurlUtil.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#include "RemoteAddressRequest.h"

namespace eIDMW
{


    //Implemented in RemoteAddress.cpp
    int parseRemoteAddressErrorCode(const char * json_str);


static size_t curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream) {
    std::string *receive_buffer = (std::string *)stream;
    size_t realsize = size * nmemb;
    receive_buffer->append(std::string(ptr, realsize));

    return realsize;
}

#define DEFAULT_NETWORK_TIMEOUT 30

std::string parseCookieFromHeaders(std::string headers) {
    std::smatch match;
    std::regex expression("Set-Cookie: (.+)\r?\n");

    if (std::regex_search(headers, match, expression)) {
       return match[1];
    }
    else {
        MWLOG(LEV_ERROR, MOD_APL, "%s, Failed to find Set-Cookie header in returned headers!", __FUNCTION__);
        return "";
    }
 }

 char *get_url_endpoint(const char *full_url) {
    char * str = (char *)full_url;

    return strrchr(str, '/');

 }

void handle_curl_error(CURLcode rc) {

    switch (rc) {
        case CURLE_OPERATION_TIMEDOUT:
            throw CMWEXCEPTION(EIDMW_REMOTEADDR_CONNECTION_TIMEOUT);
        case CURLE_COULDNT_RESOLVE_HOST:
        case CURLE_COULDNT_CONNECT:
        case CURLE_SEND_ERROR:
        case CURLE_RECV_ERROR:
            throw CMWEXCEPTION(EIDMW_REMOTEADDR_CONNECTION_ERROR);
        case CURLE_PEER_FAILED_VERIFICATION:
            throw CMWEXCEPTION(EIDMW_REMOTEADDR_CERTIFICATE_ERROR);
        default:
            throw CMWEXCEPTION(EIDMW_REMOTEADDR_UNKNOWN_ERROR);
    }
 }

 void handle_http_error(long http_code, std::string &received_data) {
    //Application error codes returned by SCM (Serviço de consulta de morada)
    //Some errors that we need to handle differently to notify the user
    const int EXPIRED_CERT_ERR = 1009;
    const int EXPIRED_DOCUMENT_ERR = 1006;
    const int REVOKED_CERT_ERR  = 1007;
    const int INVALID_STATE_ERR = 1015;
    const int NO_TOP_ERROR_STRUCT = -1;

    if (http_code >= 400 && http_code < 500) {

        MWLOG(LEV_DEBUG, MOD_APL, "Received HTTP error %ld and data: %s", http_code, received_data.c_str());
        
        int scm_error_code = parseRemoteAddressErrorCode(received_data.data());

        switch (scm_error_code) {

            case EXPIRED_CERT_ERR:
            case EXPIRED_DOCUMENT_ERR:
                throw CMWEXCEPTION(EIDMW_REMOTEADDR_EXPIRED);
            case REVOKED_CERT_ERR:
                throw CMWEXCEPTION(EIDMW_REMOTEADDR_REVOKED);
            case INVALID_STATE_ERR:
                throw CMWEXCEPTION(EIDMW_REMOTEADDR_INVALID_STATE);
            // If no top-level error code is present we need to check the ErrorStatus struct in validateReadAddressResponse()
            case NO_TOP_ERROR_STRUCT:
                break;
            default:
                throw CMWEXCEPTION(EIDMW_REMOTEADDR_UNKNOWN_ERROR);
        }

    }
    else if (http_code >= 500 && http_code < 600) {
        throw CMWEXCEPTION(EIDMW_REMOTEADDR_SERVER_ERROR);
    }

 }

PostResponse post_json_remoteaddress(const char *endpoint_url, char *json_data, const char *cookie) {

    PostResponse resp;

    CURLcode res;
    char errbuf[CURL_ERROR_SIZE];

    MWLOG(LEV_DEBUG, MOD_APL, "post_json_remoteaddress called for endpoint_url: %s", endpoint_url);

    CURLcode rc = curl_global_init(CURL_GLOBAL_ALL);
    if (rc != 0) {
        MWLOG(LEV_ERROR, MOD_APL, "Fatal error: curl_global_init failed! Error code: %d", rc);
        throw CMWEXCEPTION(EIDMW_REMOTEADDR_UNKNOWN_ERROR);
    }

    /* get a curl handle */
    CURL * curl = curl_easy_init();

    std::string received_data;
    std::string received_headers;

    if (curl) {
        struct curl_slist *headers = NULL;

        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (cookie != NULL) {
            MWLOG(LEV_DEBUG, MOD_APL, "Performing authenticated request which means adding a Cookie header");
            std::string cookie_header("Cookie: ");
            cookie_header.append(cookie);
            headers = curl_slist_append(headers, cookie_header.c_str());

        }

        curl_easy_setopt(curl, CURLOPT_URL, endpoint_url);

        /* Now specify the headers data */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        APL_Config conf_certsdir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);

        /* Specify the CA certs PEM file - necessary for libcurl OpenSSL backend on Windows and MacOS */
        std::string cacerts_file = std::string(conf_certsdir.getString()) + "/cacerts.pem";
        curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_file.c_str());

        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(json_data));


        curl_easy_setopt(curl, CURLOPT_TIMEOUT, DEFAULT_NETWORK_TIMEOUT);

		bool using_proxy = applyProxyConfigToCurl(curl, endpoint_url);

        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);

        //set buffer for human readable error message
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_write_data);

        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &received_headers);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &received_data);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, PTEID_USER_AGENT_VALUE);

         /* set the error buffer as empty before performing a request */
        errbuf[0] = 0;

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {

            MWLOG(LEV_ERROR, MOD_APL, "RemoteAddress call to %s failed! Using proxy: %d curl error code: %d curl message: %s",
                    get_url_endpoint(endpoint_url), using_proxy, res, strlen(errbuf) > 0 ? errbuf: curl_easy_strerror(res));
            goto cleanup;
        }
        else {
            long http_code = 0;
            double request_time = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            resp.http_code = http_code;

            if (http_code >= 200 && http_code < 500)
            {
                resp.http_response.append(received_data);
                resp.http_headers.append(received_headers);
            }
            else
            {
                MWLOG(LEV_ERROR, MOD_APL, "RemoteAddress server returned HTTP error on endpoint %s: %ld", get_url_endpoint(endpoint_url), http_code);
                MWLOG(LEV_DEBUG, MOD_APL, "Received data: %s", (char *)received_data.c_str());
            }
            res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &request_time);
            if (res == CURLE_OK) {
                MWLOG(LEV_DEBUG, MOD_APL, "Request total time: %.3f seconds", request_time);
            }

        }

    }
    else {
        MWLOG(LEV_ERROR, MOD_APL, "curl initialization failed: curl_easy_init!");
        curl_global_cleanup();
        throw CMWEXCEPTION(EIDMW_REMOTEADDR_UNKNOWN_ERROR);
    }

    cleanup:

    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK) {
        handle_curl_error(res);
    }
    handle_http_error(resp.http_code, received_data);
    return resp;

}


} //End of namespace decl
