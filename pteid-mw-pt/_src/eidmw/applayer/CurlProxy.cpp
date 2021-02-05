
#include <string>
#include <curl/curl.h>

#include "APLConfig.h"
#include "Log.h"


//Apply MW proxy configuration to a curl request handle

namespace eIDMW {

    void curl_apply_proxy_settings(CURL * curl_handle) {

        std::string pac_proxy_host;
        std::string pac_proxy_port;
    #ifdef WIN32
        //Get system proxy configuration
        APL_Config conf_pac(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
        const char * proxy_pac = conf_pac.getString();
        if (proxy_pac != NULL && strlen(proxy_pac) > 0)
        {
            MWLOG(LEV_DEBUG, MOD_APL, "curl_apply_proxy using Proxy PAC");
            GetProxyFromPac(proxy_pac, TSA_URL, &pac_proxy_host, &pac_proxy_port);
            MWLOG(LEV_DEBUG, MOD_APL, "curl_apply_proxy using Proxy host: %s port: %s", pac_proxy_host.c_str(), pac_proxy_port.c_str());
        }
    #endif
                
        //Get Proxy configuration
        APL_Config config_proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
        APL_Config config_proxy_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
        APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
        APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);

        if (pac_proxy_host.size() > 0 && pac_proxy_port.size() > 0) {
            curl_easy_setopt(curl_handle, CURLOPT_PROXY, pac_proxy_host.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_PROXYPORT, atol(pac_proxy_port.c_str()));
            curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        }
        if (config_proxy_host.getString() != NULL && strlen(config_proxy_host.getString()) > 0) {
            curl_easy_setopt(curl_handle, CURLOPT_PROXY, config_proxy_host.getString());
            curl_easy_setopt(curl_handle, CURLOPT_PROXYPORT, config_proxy_port.getLong());

            const char * proxy_user_value = proxy_user.getString();

            if (proxy_user_value != NULL && strlen(proxy_user_value) > 0)
            {
                curl_easy_setopt(curl_handle, CURLOPT_PROXYUSERNAME, proxy_user_value);
                curl_easy_setopt(curl_handle, CURLOPT_PROXYPASSWORD, proxy_pwd.getString());
            }

            curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        }


    }


}