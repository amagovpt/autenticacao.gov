/* RemoteAddress helper functions for HTTP API requests using libcurl

 * Copyright (C) 2020-2021 André Guerreiro - <aguerreiro1985@gmail.com>
 */


#include <curl/curl.h>
#include <cstring>
#include <string>
#include <iostream>
#include <regex>

#include "Log.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#include "RemoteAddressRequest.h"

namespace eIDMW
{


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

PostResponse post_json_remoteaddress(const char *endpoint_url, char *json_data, const char *cookie) {

    PostResponse resp;

    CURLcode res;

    MWLOG(LEV_DEBUG, MOD_APL, "post_json_remoteaddress called for endpoint_url: %s", endpoint_url);

    CURLcode rc = curl_global_init(CURL_GLOBAL_ALL);
    if (rc != 0) {
        MWLOG(LEV_ERROR, MOD_APL, "Fatal error: curl_global_init failed! Error code: %d", rc);
        return resp;
    }

    /* get a curl handle */
    CURL * curl = curl_easy_init();

    std::string received_data;
    std::string received_headers;

    if (curl) {
        struct curl_slist *headers = NULL;

        //TODO: apply_proxy_config(curl, pInfo);

        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (cookie != NULL) {
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
        
        /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data);

        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_write_data);

        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &received_headers);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &received_data);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, PTEID_USER_AGENT_VALUE);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK) {
            MWLOG(LEV_ERROR, MOD_APL, "RemoteAddress call to %s failed! curl error msg: %s", endpoint_url, curl_easy_strerror(res));
            goto cleanup;
        }
        else
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            resp.http_code = http_code;

            if (http_code >= 200 && http_code < 300)
            {
                resp.http_response.append(received_data);
                resp.http_headers.append(received_headers);
            }
            else
            {
                MWLOG(LEV_ERROR, MOD_APL, "RemoteAddress call to %s returned HTTP error %ld Message: %s", endpoint_url, http_code,
                        (char *)received_data.c_str());
            }


        }

    }

    cleanup:

    /* always cleanup */
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    
    return resp;

}


} //End of namespace decl 

