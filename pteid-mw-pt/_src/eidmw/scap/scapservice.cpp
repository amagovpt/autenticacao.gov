/*-****************************************************************************

 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "scapservice.h"

#include <chrono>
#include <thread>
#include <ctime>
#include <algorithm>
#include <sstream>

#include <cjson/cJSON.h>

#include "scapsettings.h"
#include "totp_gen.h"

#include "CurlUtil.h"
#include "MiscUtil.h"
#include "APLConfig.h"
#include "Config.h"
#include "Log.h"
#include "eidErrors.h"

#include "scapclient.h"

namespace eIDMW {

static size_t curl_write_data(char *data, size_t size, size_t nmemb, void *read_string)
{
	((std::string*)read_string)->append((char*)data, size * nmemb);
	return size * nmemb;
}

static std::string parse_date_header(const std::string &headers)
{
	const std::string to_find = "Date: ";
	std::istringstream date_header_stream(headers);

	std::string line;
	while (std::getline(date_header_stream, line)) {
		if (line.rfind(to_find, 0) == 0) {
			return line.substr(to_find.size());
		}
	}

	return "";
}

static CURLU *set_url(ScapSettings & settings, const std::string &endpoint, const std::vector<std::string> &queries)
{
	CURLU *url = curl_url();

	std::string host = "https://" + settings.getScapServerHost();

	if (curl_url_set(url, CURLUPART_URL, host.c_str(), 0) != CURLUE_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to set url: %s", __FUNCTION__, host.c_str());
		return NULL;
	}

	if (curl_url_set(url, CURLUPART_PORT, settings.getScapServerPort().c_str(), 0) != CURLUE_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to set port.", __FUNCTION__);
		return NULL;
	}

	if (curl_url_set(url, CURLUPART_PATH, endpoint.c_str(), 0) != CURLUE_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to set endpoint: %s", __FUNCTION__, endpoint.c_str());
		return NULL;
	}

	for (const std::string& query: queries) {
		if (curl_url_set(url, CURLUPART_QUERY, query.c_str(), CURLU_APPENDQUERY) != CURLUE_OK) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed: to set processid query string.", __FUNCTION__);
			return NULL;
		}
	}

	return url;
}


ScapResponse perform_request(const ScapCredentials &credentials, const ScapRequest &request, CURL *curl)
{
	ScapResponse response;
	response.status = 0;

	CURLcode ret;
	CURLU *url = NULL;
	char error_buffer[CURL_ERROR_SIZE] = {0};
	struct curl_slist *header_list = NULL;
	std::string received_header_data;
	ScapSettings settings;
	std::string partial_url = "https://" + settings.getScapServerHost();
	bool using_proxy = false;
	APL_Config conf_certsdir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	std::string cacerts_location = std::string(conf_certsdir.getString()) + "/cacerts.pem";

	// curl handle is always NULL here except when loading attributes with card
	// as it is the only request with client certificate authentication
	if (curl == NULL) {
		curl_global_init(CURL_GLOBAL_NOTHING);
		if ((curl = curl_easy_init()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s curl_easy_init() failed", __FUNCTION__);
			response.status = CURL_GENERIC_ERROR;
			goto clean_up;
		}
		/* For a request with client certificate authentication we can't set a low connect timeout
		   as the time for auth PIN introduction is included in the SSL "connect" phase */
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
	}

	if ((url = set_url(settings, request.endpoint, request.get_parameters)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to set url", __FUNCTION__);
		response.status = CURL_GENERIC_ERROR;
		goto clean_up;
	}

	if (!request.body.empty()) {
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request.body.size());
		header_list = curl_slist_append(header_list, "Content-Type: application/json");
	}

	for (const std::string &header: request.headers) {
		header_list = curl_slist_append(header_list, header.c_str());
	}

	if (header_list) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	}

	curl_easy_setopt(curl, CURLOPT_USERAGENT, PTEID_USER_AGENT_VALUE);

	curl_easy_setopt(curl, CURLOPT_CURLU, url);
	//Maximum time the transfer is allowed to complete
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

#ifdef DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

	curl_easy_setopt(curl, CURLOPT_USERNAME, credentials.basic_user.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, credentials.basic_pass.c_str());

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.response);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &curl_write_data);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &received_header_data);

	curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_location.c_str());

	using_proxy = applyProxyConfigToCurl(curl, partial_url.c_str());

	try {
		ret = curl_easy_perform(curl);
	}
	catch (CMWException &e) {
		if (e.GetError() == EIDMW_ERR_PIN_CANCEL) {
			response.status = SSL_PIN_CANCELED_ERROR;
		} else {
			response.status = CURL_GENERIC_ERROR;
		}
		goto clean_up;
	}

	if (ret != CURLE_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Error on request %s. Libcurl returned %s\n",
			request.endpoint.c_str(), error_buffer);

		long auth = 0;
		if (!curl_easy_getinfo(curl, CURLINFO_PROXYAUTH_AVAIL, &auth) && auth) {
			response.status = PROXY_AUTH_REQUIRED;
		} else if (using_proxy) {
			response.status = POSSIBLE_PROXY_ERROR;
		} else {
			response.status = CURL_PERFORM_ERROR;
		}
		goto clean_up;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);

	response.server_time = parse_date_header(received_header_data);

clean_up:
	curl_url_cleanup(url);
	curl_easy_cleanup(curl);
	return response;
}

static cJSON *get_status_json_object(cJSON *&out_json, const std::string &response)
{
	if ((out_json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		return NULL;
	}

	cJSON *status = NULL;
	if ((status = cJSON_GetObjectItem(out_json, "status")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse status object", __FUNCTION__);
		return NULL;
	}

	return status;
}

unsigned int get_response_status(const std::string &response)
{
	cJSON *json = NULL;
	cJSON *status = get_status_json_object(json, response);

	unsigned int status_code = 0;
	if (status) {
		status_code = atoi(cJSON_GetStringValue(cJSON_GetObjectItem(status, "code")));
		char *code_descr = cJSON_GetStringValue(cJSON_GetObjectItem(status, "codeDescription"));
		MWLOG(LEV_DEBUG, MOD_SCAP, "response with status code %d - %s", status_code, code_descr);
	}

	cJSON_Delete(json);
	return status_code;
}

std::string get_response_code_description(const std::string &response)
{
	cJSON *json = NULL;
	cJSON *status = get_status_json_object(json, response);

	std::string result;
	if (status) {
		char *code_descr = cJSON_GetStringValue(cJSON_GetObjectItem(status, "codeDescription"));
		result = code_descr ? code_descr : "";
	}

	cJSON_Delete(json);
	return result;
}

ScapResponse perform_polling_request(const ScapCredentials &credentials, const ScapRequest &request)
{
	// polling must start 5 second after first request. polling every 2 seconds, max of 50 seconds

	ScapResponse response;
	unsigned int inner_status = 0;
	time_t start = time(NULL);
	std::this_thread::sleep_for(std::chrono::seconds(3));

	do {
		std::this_thread::sleep_for(std::chrono::seconds(2));

		ScapResponse temp_response = perform_request(credentials, request);
		if (temp_response.status != 200) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s polling request failed: %d", __FUNCTION__, temp_response.status);
			inner_status = 0;
		} else {
			inner_status = get_response_status(temp_response.response);
			if (inner_status == 404) {
				MWLOG(LEV_DEBUG, MOD_SCAP, "Possibly reached polling time limit. Got incomplete response.");
				break;
			}
		}

		response = temp_response;

	} while((inner_status == 102 || inner_status == 206) && difftime(time(NULL), start) < 55);

	return response;
}

std::vector<ScapProvider> parse_providers(const std::string &read_buffer)
{
	std::vector<ScapProvider> result;
	cJSON *json =  NULL;
	if ((json = cJSON_Parse(read_buffer.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_Parse() failed", __FUNCTION__);
		return result;
	}

	cJSON *attributeProviders = NULL;
	if ((attributeProviders = cJSON_GetObjectItem(json, "attributeProviders")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviders object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	const size_t array_size = cJSON_GetArraySize(attributeProviders);
	for (size_t i = 0; i < array_size; ++i) {
		cJSON *array_item = NULL;
		if ((array_item = cJSON_GetArrayItem(attributeProviders, i)) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviders object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		char *uri = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "uriId"));
		char *name = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "name"));
		char *type = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "type"));
		char *nipc = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "nipc"));
		char *logo = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "logo"));

		result.push_back(ScapProvider{
			uri ? uri : "",
			name ? name : "",
			type ? type : "",
			nipc ? nipc : "",
			logo ? logo : ""
		});
	}

	MWLOG(LEV_DEBUG, MOD_SCAP, "%s attributeProviders object correctly parsed.", __FUNCTION__);
	cJSON_Delete(json);
	return result;
}

std::string create_search_attributes_body(const CitizenInfo &citizen_info,
	const std::vector<ScapProvider> &providers, const std::string &processId,
	const ScapCredentials &credentials, bool allEnterprise, bool allEmployee)
{
	std::string result;

	char *json_string = NULL;
	cJSON *json = NULL;
	cJSON *citizen_info_json = NULL;
	cJSON *citizen_doc_info = NULL;
	cJSON *providers_json = NULL;
	std::vector<const char *> providers_strings;

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "processId", processId.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add processid", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "credentialId", credentials.credential_id.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add credentialId", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "clientName", credentials.basic_user.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add clientName", __FUNCTION__);
		goto clean_up;
	}

	if ((citizen_doc_info = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed create citizen_doc_info", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "type", citizen_info.doc_type.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add type", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "country", citizen_info.doc_nationality.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add country", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "id", citizen_info.doc_number.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add id", __FUNCTION__);
		goto clean_up;
	}

	if ((citizen_info_json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed create citizen_info", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_info_json, "name", citizen_info.name.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add citizen name", __FUNCTION__);
		goto clean_up;
	}

	cJSON_AddItemToObject(citizen_info_json, "citizenDocumentInfo", citizen_doc_info);
	cJSON_AddItemToObject(json, "citizenInfo", citizen_info_json);

	if (cJSON_AddBoolToObject(json, "searchAllEnterpriseAttributes", allEnterprise) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add searchAllEnterpriseAttributes", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddBoolToObject(json, "searchAllEmployeeAttributes", allEmployee) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add searchAllEmployeeAttributes", __FUNCTION__);
		goto clean_up;
	}

	if (providers.size() > 0) {
		for (const ScapProvider &provider : providers) {
			providers_strings.push_back(provider.uriId.c_str());
		}

		if ((providers_json = cJSON_CreateStringArray(&providers_strings[0], providers.size())) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to create provider id array", __FUNCTION__);
			goto clean_up;
		}

		cJSON_AddItemToObject(json, "attributeProviderUriIds", providers_json);
	}

	if ((json_string = cJSON_Print(json)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to print JSON", __FUNCTION__);
		goto clean_up;
	}

	result = json_string;

clean_up:
	cJSON_Delete(json);
	free(json_string);
	return result;
}

std::string create_fetch_attributes_body(const std::string &processId)
{
	std::string result;
	char *json_string = NULL;
	cJSON *json = NULL;

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject failed", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "processId", processId.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add processid", __FUNCTION__);
		goto clean_up;
	}

	if ((json_string = cJSON_Print(json)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to print JSON", __FUNCTION__);
		goto clean_up;
	}

	result = json_string;

clean_up:
	cJSON_Delete(json);
	free(json_string);
	return result;
}

std::string extract_secret_key(const std::string& response)
{
	std::string result;

	char *secret_key = NULL;
	cJSON *json = NULL;
	if ((json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		goto clean_up;
	}

	if ((secret_key = cJSON_GetStringValue(cJSON_GetObjectItem(json, "secretKey"))) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse secret key", __FUNCTION__);
		goto clean_up;
	}

	result = secret_key;

clean_up:
	cJSON_Delete(json);
	return result;
}

static std::string generate_totp(const std::string &id)
{
	ScapSettings settings;
	std::string secret_key = settings.getSecretKey(id);

	const int totp_digits = 6;
	const int totp_step_time = 60;
	std::string totp = generateTOTP(secret_key, totp_digits, totp_step_time, time(NULL));

	if (totp.length() != totp_digits) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s Error generating TOTP: possibly invalid key", __FUNCTION__);
		return "";
	}

	std::fill(secret_key.begin(), secret_key.end(), 0);

	return totp;
}

std::string create_authorization_body(const std::string &processId, const CitizenInfo &citizen_info,
	const ScapCredentials &credentials, const std::vector<ScapAttribute> &attributes,
	const std::vector<Document> &documents)
{
	std::string result;

	char *json_string = NULL;
	cJSON *json = NULL;
	cJSON *doc_info = NULL;
	cJSON *doc_hashes_info = NULL;
	cJSON *citizen_doc_info = NULL;
	cJSON *citizen_info_json = NULL;
	cJSON *attribute_provider_signatures = NULL;
	cJSON *attributes_to_sign = NULL;
	cJSON *attributes_aux_object = NULL;
	cJSON *attributes_provider_info = NULL;
	cJSON *attributes_array = NULL;
	cJSON *sub_attributes_array = NULL;
	cJSON *attribute_json = NULL;
	cJSON *sub_attribute_json = NULL;

	std::vector<const char *> providers_strings;
	const auto attrs_grouped_by_provider = group_by_provider(attributes);

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "processId", processId.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add processid", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "credentialId", credentials.credential_id.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add credentialId", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "clientName", credentials.basic_user.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add clientName", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "totp", generate_totp(citizen_info.doc_number).c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add totp", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "numSignatures", std::to_string(documents.size()).c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add numSignatures", __FUNCTION__);
		goto clean_up;
	}

	if ((citizen_doc_info = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed create citizen_doc_info", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "type", citizen_info.doc_type.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add type", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "country", citizen_info.doc_nationality.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add country", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_doc_info, "id", citizen_info.doc_number.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add id", __FUNCTION__);
		goto clean_up;
	}

	if ((citizen_info_json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed create citizen_info", __FUNCTION__);
		goto clean_up;
	}

	cJSON_AddItemToObject(citizen_info_json, "citizenDocumentInfo", citizen_doc_info);
	cJSON_AddItemToObject(json, "citizenInfo", citizen_info_json);

	doc_hashes_info = cJSON_CreateArray();
	for (const Document& doc: documents) {
		if ((doc_info = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(doc_info, "originalDocumentHash", doc.hash.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add originalDocumentHash", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(doc_info, "originalDocumentCitizenSignature", doc.original_signature.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add originalDocumentCitizenSignature", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(doc_info, "documentName", doc.name.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add documentName", __FUNCTION__);
			goto clean_up;
		}

		cJSON_AddItemToArray(doc_hashes_info, doc_info);
	}
	cJSON_AddItemToObject(json, "documentHashesInfo", doc_hashes_info);

	attribute_provider_signatures = cJSON_CreateArray();

	for (const auto &provider_attributes_pair: attrs_grouped_by_provider) {
		const ScapProvider& provider = provider_attributes_pair.first;

		if ((attributes_to_sign = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		if ((attributes_provider_info = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(attributes_provider_info, "uriId", provider.uriId.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add uriId", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(attributes_provider_info, "name", provider.name.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add name", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(attributes_provider_info, "type", provider.type.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add type", __FUNCTION__);
			goto clean_up;
		}

		cJSON_AddItemToObject(attributes_to_sign, "attributeProviderInfo", attributes_provider_info);

		attributes_array = cJSON_CreateArray();
		for (const auto &attr: provider_attributes_pair.second) {
			if ((attribute_json = cJSON_CreateObject()) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
				goto clean_up;
			}

			if (cJSON_AddStringToObject(attribute_json, "id", attr.id.c_str()) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add id", __FUNCTION__);
				goto clean_up;
			}

			if (cJSON_AddStringToObject(attribute_json, "description", attr.description.c_str()) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add description", __FUNCTION__);
				goto clean_up;
			}

			if (cJSON_AddStringToObject(attribute_json, "validity", attr.validity.c_str()) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add validity", __FUNCTION__);
				goto clean_up;
			}

			sub_attributes_array = cJSON_CreateArray();
			for (const ScapSubAttribute &sub_attribute: attr.sub_attributes) {
				if ((sub_attribute_json = cJSON_CreateObject()) == NULL) {
					MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
					goto clean_up;
				}

				if (cJSON_AddStringToObject(sub_attribute_json, "id", sub_attribute.id.c_str()) == NULL) {
					MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add id", __FUNCTION__);
					goto clean_up;
				}

				if (cJSON_AddStringToObject(sub_attribute_json, "description", sub_attribute.description.c_str()) == NULL) {
					MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add description", __FUNCTION__);
					goto clean_up;
				}

				if (cJSON_AddStringToObject(sub_attribute_json, "value", sub_attribute.value.c_str()) == NULL) {
					MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add value", __FUNCTION__);
					goto clean_up;
				}

				cJSON_AddItemToArray(sub_attributes_array, sub_attribute_json);
			}
			cJSON_AddItemToObject(attribute_json, "subAttributes", sub_attributes_array);
			cJSON_AddItemToArray(attributes_array, attribute_json);
		}
		cJSON_AddItemToObject(attributes_to_sign, "attributes", attributes_array);

		if ((attributes_aux_object = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		cJSON_AddItemToObject(attributes_aux_object, "attributesToSign", attributes_to_sign);
		cJSON_AddItemToArray(attribute_provider_signatures, attributes_aux_object);
	}
	cJSON_AddItemToObject(json, "attributeProviderSignatures", attribute_provider_signatures);

	if (cJSON_AddStringToObject(json, "citizenCertificate", citizen_info.certificate.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add citizenCertificate", __FUNCTION__);
		goto clean_up;
	}

	if ((json_string = cJSON_Print(json)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to print JSON", __FUNCTION__);
		goto clean_up;
	}

	result = json_string;

clean_up:
	cJSON_Delete(json);
	free(json_string);
	return result;
}

std::vector<ScapTransaction> parse_authorization(const std::string& response)
{
	std::vector<ScapTransaction> result;

	cJSON *json =  NULL;
	if ((json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		return result;
	}

	cJSON *transactions = NULL;
	if ((transactions = cJSON_GetObjectItem(json, "transactions")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse transactions object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	const size_t array_size = cJSON_GetArraySize(transactions);
	for (size_t i = 0; i < array_size; ++i) {
		cJSON *array_item = NULL;
		if ((array_item = cJSON_GetArrayItem(transactions, i)) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse transactions object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		char *sad = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "sad"));
		char *id = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "transactionId"));

		cJSON *attribute_provider_signatures = NULL;
		if ((attribute_provider_signatures = cJSON_GetObjectItem(array_item, "attributeProviderSignatures")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviderSignatures object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		cJSON *attributes_to_sign = NULL;
		if ((attributes_to_sign = cJSON_GetObjectItem(attribute_provider_signatures, "attributesToSign")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributesToSign object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		cJSON *attribute_provider_info = NULL;
		if ((attribute_provider_info = cJSON_GetObjectItem(attributes_to_sign, "attributeProviderInfo")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviderInfo object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		char *certificate_chain = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_provider_info, "certificates"));
		char *provider_name = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_provider_info, "name"));

		cJSON *attributes_array = NULL;
		if ((attributes_array = cJSON_GetObjectItem(attributes_to_sign, "attributes")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviderInfo object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		std::vector<ScapAttribute> attributes;
		const size_t array_size = cJSON_GetArraySize(attributes_array);
		for (size_t i = 0; i < array_size; ++i) {
			cJSON *array_item = NULL;
			if ((array_item = cJSON_GetArrayItem(attributes_array, i)) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributes object", __FUNCTION__);
				cJSON_Delete(json);
				return result;
			}

			char *attr_name = cJSON_GetStringValue(cJSON_GetObjectItem(array_item, "description"));
			ScapAttribute attribute;
			attribute.description = attr_name ? attr_name : "";

			cJSON *sub_attributes = NULL;
			if ((sub_attributes = cJSON_GetObjectItem(array_item, "subAttributes")) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse sub_attributes object", __FUNCTION__);
				cJSON_Delete(json);
				return result;
			}

			const size_t sub_attributes_array_size = cJSON_GetArraySize(sub_attributes);
			for (size_t k = 0; k < sub_attributes_array_size; ++k) {
				cJSON *sub_attribute_json = NULL;
				if ((sub_attribute_json = cJSON_GetArrayItem(sub_attributes, k)) == NULL) {
					MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse sub_attribute's item with index %d", __FUNCTION__, k);
					cJSON_Delete(json);
					return result;
				}

				char *sub_description = cJSON_GetStringValue(cJSON_GetObjectItem(sub_attribute_json, "description"));
				char *sub_value = cJSON_GetStringValue(cJSON_GetObjectItem(sub_attribute_json, "value"));

				attribute.sub_attributes.push_back({
					"",
					sub_description ? sub_description : "",
					sub_value ? sub_value : ""
				});
			}

			attributes.push_back(attribute);
		}

		ScapTransaction transaction  = {
			id ? id : "",
			sad ? sad : "",
			certificate_chain ? certificate_chain : "",
			provider_name ? provider_name : "",
			attributes
		};

		result.push_back(transaction);
	}

	MWLOG(LEV_DEBUG, MOD_SCAP, "Parsed sign authorization successfully");
	cJSON_Delete(json);
	return result;
}

std::string create_sign_hash_body(const std::string &processId, const std::string &id,
	const ScapCredentials &credentials, const ScapTransaction &transaction, const std::vector<std::string> &hashes)
{
	std::string result;

	char *json_string = NULL;
	cJSON *json = NULL;
	cJSON *hashes_array = NULL;
	std::vector<const char *> hashes_strings;

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "processId", processId.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add processid", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "credentialId", credentials.credential_id.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add credentialId", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "clientName", credentials.basic_user.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add clientName", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "totp", generate_totp(id).c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add totp", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "sad", transaction.sad.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add sad", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "transactionId", transaction.id.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add transactionId", __FUNCTION__);
		goto clean_up;
	}

	for (const std::string &hash: hashes) {
		hashes_strings.push_back(hash.c_str());
	}

	if ((hashes_array = cJSON_CreateStringArray(&hashes_strings[0], hashes.size())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to create hashes_strings array", __FUNCTION__);
		goto clean_up;
	}

	cJSON_AddItemToObject(json, "hashesToSign", hashes_array);

	if ((json_string = cJSON_Print(json)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to print JSON", __FUNCTION__);
		goto clean_up;
	}

	result = json_string;

clean_up:
	cJSON_Delete(json);
	free(json_string);
	return result;
}

std::string create_get_signed_hash_body(const std::string &processId, const ScapTransaction &transaction)
{
	std::string result;

	char *json_string = NULL;
	cJSON *json = NULL;

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "processId", processId.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add processid", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "sad", transaction.sad.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add sad", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(json, "transactionId", transaction.id.c_str()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add transactionId", __FUNCTION__);
		goto clean_up;
	}

	if ((json_string = cJSON_Print(json)) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to print JSON", __FUNCTION__);
		goto clean_up;
	}

	result = json_string;

clean_up:
	cJSON_Delete(json);
	free(json_string);
	return result;
}

std::vector<std::string> parse_signatures(const std::string &response)
{
	std::vector<std::string> result;

	cJSON *json =  NULL;
	if ((json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		return result;
	}

	cJSON *signatures = NULL;
	if ((signatures = cJSON_GetObjectItem(json, "signatures")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse transactions object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	const size_t array_size = cJSON_GetArraySize(signatures);
	for (size_t i = 0; i < array_size; ++i) {
		cJSON *array_item = NULL;
		if ((array_item = cJSON_GetArrayItem(signatures, i)) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse signature object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		char *signature = cJSON_GetStringValue(array_item);
		result.push_back(signature);
	}

	MWLOG(LEV_DEBUG, MOD_SCAP, "Parsed signatures successfully");
	cJSON_Delete(json);
	return result;
}

std::vector<std::string> get_failed_providers_ids(const std::string &response)
{
	std::vector<std::string> result;

	cJSON *json =  NULL;
	if ((json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		return result;
	}

	cJSON *citizenAttributes = NULL;
	if ((citizenAttributes = cJSON_GetObjectItem(json, "citizenAttributes")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse citizenAttributes object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	const size_t array_size = cJSON_GetArraySize(citizenAttributes);
	for (size_t i = 0; i < array_size; ++i) {
		cJSON *array_item = NULL;
		if ((array_item = cJSON_GetArrayItem(citizenAttributes, i)) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to get array item", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		cJSON *status = NULL;
		if ((status = cJSON_GetObjectItem(array_item, "status")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse status object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		unsigned int status_code = atoi(cJSON_GetStringValue(cJSON_GetObjectItem(status, "code")));
		if (status_code != SCAP_OK) {
			cJSON *attribute_provider_info = NULL;
			if ((attribute_provider_info = cJSON_GetObjectItem(array_item, "attributeProviderInfo")) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributeProviderInfo object", __FUNCTION__);
				cJSON_Delete(json);
				return result;
			}

			char *provider_uri = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_provider_info, "uriId"));
			result.push_back(provider_uri ? provider_uri : "");
		}
	}

	cJSON_Delete(json);
	return result;
}

};
