/*-****************************************************************************

 * Copyright (C) 2013-2014, 2016-2017, 2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2013 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2018 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/


//cURL for Timestamping
#include <curl/curl.h>
#include <cstdlib>
#include <string>
#include "APLConfig.h"
#include "MiscUtil.h"

#include "APLReader.h"
#include "CurlUtil.h"
#include "Util.h"
#include "Log.h"
#include "TsaClient.h"

namespace eIDMW
{

#define SHA1_LEN 20
#define SHA256_LEN 32
#define TS_REQUEST_SHA1_LEN 43
#define TS_REQUEST_SHA256_LEN 59	
#define SHA1_OFFSET 20
#define SHA256_OFFSET 24

	//Implemented in CurlProxy.cpp
    extern void curl_apply_proxy_settings(CURL * curl_handle, const char * url);


	/* ASN1 "templates" for timestamp requests of SHA-1 and SHA-256 hashes  */

	static unsigned char timestamp_asn1_request[TS_REQUEST_SHA1_LEN] =
	{
		0x30, 0x29, 0x02, 0x01, 0x01, 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a,
		0x05, 0x00, 0x04, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xff
	};

	static unsigned char timestamp_asn1_sha256[TS_REQUEST_SHA256_LEN] =
	{
		0x30,0x39,0x02,0x01,0x01,0x30,0x31,0x30,0x0D,0x06,0x09,0x60,0x86,0x48,
		0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x01,0x01,0xFF
	};

	TSAClient::TSAClient()
	{

	}

	CByteArray TSAClient::received_data = CByteArray();

	size_t TSAClient::curl_write_data(char *ptr, size_t size, size_t nmemb, void * stream)
	{
		size_t realsize = size * nmemb;
		received_data.Append((const unsigned char*)ptr, realsize);

		return realsize;

	}

	CByteArray TSAClient::getResponse()
	{
		return received_data;

	}

	/* Fill the template with the supplied hash */
	void TSAClient::generate_asn1_request_struct(const unsigned char *hash, bool is_sha256)
	{
		int hash_length = SHA1_LEN;
		int hash_offset = SHA1_OFFSET;

		unsigned char * ts_request_buffer = timestamp_asn1_request;
		if (is_sha256)
		{
			hash_length = SHA256_LEN;
			hash_offset = SHA256_OFFSET;
			ts_request_buffer = timestamp_asn1_sha256;
		}

		for (unsigned int i=0; i != hash_length; i++)
		    ts_request_buffer[hash_offset +i] = hash[i];
	}

	void TSAClient::timestamp_data(const unsigned char *input, unsigned int data_len)
	{

		CURL *curl;
		CURLcode res;
		char error_buf[CURL_ERROR_SIZE];
		unsigned char *ts_request = timestamp_asn1_request;
		size_t post_size = sizeof(timestamp_asn1_request);

		//Make sure the static array receiving the network reply 
		// is zero'd out before each request
		received_data.Chop(received_data.Size());

		//Get Timestamping server URL from config
		APL_Config tsa_url(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL);
		const char * TSA_URL = tsa_url.getString();

		MWLOG(LEV_DEBUG, MOD_APL, "Requesting timestamp with TSA url: %s", TSA_URL);
	    if (data_len == SHA256_LEN)
		{
			generate_asn1_request_struct(input, true);
			ts_request = timestamp_asn1_sha256;
			post_size = sizeof(timestamp_asn1_sha256);

		}
		else
		{
			generate_asn1_request_struct(input, false);
		}

		curl = curl_easy_init();

		if (curl)
		{

			struct curl_slist *headers= NULL;

			headers = curl_slist_append(headers, "Content-Type: application/timestamp-query");
			headers = curl_slist_append(headers, "Content-Transfer-Encoding: binary");
			headers = curl_slist_append(headers, PTEID_USER_AGENT);

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_size);
			curl_easy_setopt(curl, CURLOPT_URL, TSA_URL);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

			/* Now specify the POST data */ 
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ts_request);

			curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buf);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &TSAClient::curl_write_data);

			applyProxyConfigToCurl(curl, TSA_URL);

			/* Perform the request, res will get the return code */ 
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK)
			{
				MWLOG(LEV_ERROR, MOD_APL, "Timestamp error in HTTP POST request. LibcURL returned %s",
					curl_easy_strerror(res));
			}
			else
			{
				long http_code = 0;
				curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
				if (http_code == 200)
				{
						MWLOG(LEV_DEBUG, MOD_APL, "Timestamp request Succeeded.");
				}
				else 
				{
					MWLOG(LEV_ERROR, MOD_APL, "Timestamp request Failed. HTTP error: %ld", http_code);
				}
			}

			curl_slist_free_all(headers);

			/* always cleanup */ 
			curl_easy_cleanup(curl);

		}

	}

}
