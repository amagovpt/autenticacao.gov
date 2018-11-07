#include <curl/curl.h>

#include <cstdlib>
#include <string>
#include "CRLFetcher.h"
#include "APLConfig.h"
#include "MiscUtil.h"
#include "Util.h"
#include "Log.h"

namespace eIDMW
{

	CByteArray CRLFetcher::received_data = CByteArray();

	size_t CRLFetcher::curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream)
	{
		size_t realsize = size * nmemb;
		received_data.Append((const unsigned char*)ptr, realsize);

		return realsize;

	}
	
	CByteArray CRLFetcher::fetch_CRL_file(const char *url)
	{
		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];
		std::string pac_proxy_host;
		std::string pac_proxy_port;
		
		//Get Proxy configuration
		APL_Config proxy_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
		APL_Config proxy_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
		APL_Config proxy_user(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
		APL_Config proxy_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);

		if (strlen(url) == 0 || strstr(url, "http") != url)
		{
			fprintf(stderr, "Invalid URL for fetch_CRL_file()\n");
			return received_data;
		}

#ifdef WIN32
		//Get system proxy configuration
		APL_Config conf_pac(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
		const char * proxy_pac = conf_pac.getString();
		if (proxy_pac != NULL && strlen(proxy_pac) > 0)
		{
			MWLOG(LEV_DEBUG, MOD_APL, "timestamp_data using Proxy PAC");
			GetProxyFromPac(proxy_pac, url, &pac_proxy_host, &pac_proxy_port);
			MWLOG(LEV_DEBUG, MOD_APL, "timestamp_data using Proxy host: %s port: %s", pac_proxy_host.c_str(), pac_proxy_port.c_str());
		}
#endif

		MWLOG(LEV_DEBUG, MOD_APL, "Downloading CRL: %s", url);

		//Make sure the static array receiving the network reply 
		// is zero'd out before each request
		received_data.Chop(received_data.Size());

		#ifdef _WIN32
		curl_global_init(CURL_GLOBAL_WIN32);
#else
		curl_global_init(CURL_GLOBAL_NOTHING);
#endif
		curl = curl_easy_init();
		
		if (curl == NULL)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"curl_easy_init() failed!");
			return CByteArray();
		}
		
			struct curl_slist *headers= NULL;

			headers = curl_slist_append(headers, PTEID_USER_AGENT);
			headers = curl_slist_append(headers, "Accept: application/x-pkcs7-crl");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			curl_easy_setopt(curl, CURLOPT_URL, url);

			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

			if (pac_proxy_host.size() > 0 && pac_proxy_port.size() > 0)
			{
				curl_easy_setopt(curl, CURLOPT_PROXY, pac_proxy_host.c_str());
				curl_easy_setopt(curl, CURLOPT_PROXYPORT, atol(pac_proxy_port.c_str()));
				curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
			}
			if (proxy_host.getString() != NULL && strlen(proxy_host.getString()) > 0)
			{
				//Set Proxy options for request
				curl_easy_setopt(curl, CURLOPT_PROXY, proxy_host.getString());
				curl_easy_setopt(curl, CURLOPT_PROXYPORT, proxy_port.getLong());

				const char * proxy_user_value = proxy_user.getString();

				if (proxy_user_value != NULL && strlen(proxy_user_value) > 0)
				{
					curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxy_user_value);
					curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, proxy_pwd.getString());
				}
				
				curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
			}

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_data);

			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);

			if (res != 0)
			{
				MWLOG(LEV_ERROR, MOD_APL, L"Error downloading CRL. Libcurl returned %ls\n", 
						utilStringWiden(string(error_buf)).c_str());
			}

			curl_slist_free_all(headers);

			return received_data;

	}

}