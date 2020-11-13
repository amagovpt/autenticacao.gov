/*-****************************************************************************

 * Copyright (C) 2016-2020 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <curl/curl.h>

#include <cstdlib>
#include <string>
#include "CRLFetcher.h"
#include "APLConfig.h"
#include "MiscUtil.h"
#include "Util.h"
#include "Log.h"

#ifdef WIN32
#include <wincrypt.h>
#endif

namespace eIDMW
{

	CByteArray CRLFetcher::received_data = CByteArray();

	size_t CRLFetcher::curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream)
	{
		size_t realsize = size * nmemb;
		received_data.Append((const unsigned char*)ptr, realsize);

		return realsize;

	}

#ifdef WIN32
	CByteArray CRLFetcher::fetch_CRL_file(const char *url) {
		CByteArray crl_data;

		DWORD dwRetrievalFlags = 0;
		//Timeout in milliseconds
		DWORD dwTimeout = 20 * 1000;
		CRL_CONTEXT * crl_ctx = NULL;
		MWLOG(LEV_DEBUG, MOD_APL, "Downloading CRL %s using CryptRetrieveObjectByUrl", url);

		BOOL ret = CryptRetrieveObjectByUrlA(url, CONTEXT_OID_CRL, dwRetrievalFlags, dwTimeout, (LPVOID *)&crl_ctx, NULL, NULL, NULL, NULL);

		if (!ret) {
			MWLOG(LEV_ERROR, MOD_APL, "Error fetching CRL! CryptRetrieveObjectByUrl error code: %08x", GetLastError());
		
		}
		else {
			if (crl_ctx->dwCertEncodingType & X509_ASN_ENCODING) {
				MWLOG(LEV_DEBUG, MOD_APL, "Successfully fetched %d bytes of CRL content", crl_ctx->cbCrlEncoded);
				crl_data.Append(crl_ctx->pbCrlEncoded, crl_ctx->cbCrlEncoded);
			}
			else {
				MWLOG(LEV_ERROR, MOD_APL, "CRLFetcher: unexpected encoding in CRL_CONTEXT object! This should never happen...");
			}
			CertFreeCRLContext(crl_ctx);
		}

		return crl_data;
	}

#else
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

		MWLOG(LEV_DEBUG, MOD_APL, "Downloading CRL: %s", url);

		//Make sure the static array receiving the network reply 
		// is zero'd out before each request
		received_data.Chop(received_data.Size());

		curl_global_init(CURL_GLOBAL_NOTHING);

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
			else
			{
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
				if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK)
				{
						MWLOG(LEV_DEBUG, MOD_APL, "CRL download succeeded.");
				}
				else
				{
					MWLOG(LEV_ERROR, MOD_APL, "CRL Download failed! HTTP status code: %ld", http_code);
				}
			}

			curl_slist_free_all(headers);

			return received_data;

	}
#endif

}