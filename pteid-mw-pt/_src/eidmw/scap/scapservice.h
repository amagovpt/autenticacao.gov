/*-****************************************************************************

 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#pragma once

#include <string>
#include <vector>

#include <curl/curl.h>
#include <QTemporaryDir>

#include "eidlib.h"

namespace eIDMW {

#define CURL_GENERIC_ERROR                1
#define CURL_PERFORM_ERROR                2
#define SSL_PIN_CANCELED_ERROR            3
#define PROXY_AUTH_REQUIRED               4
#define POSSIBLE_PROXY_ERROR              5

#define SCAP_OK                           200
#define SCAP_PROCESSING_REQUEST           102
#define SCAP_EXPIRED_OR_NO_ATTRS          204
#define SCAP_EXPIRED_ATTRS                205
#define SCAP_INCOMPLETE_RESPONSE          206
#define SCAP_TRANSACTION_OR_TOTP_ERROR    401 //read message for details //possibly bad secret key
#define SCAP_INVALID_PROCESS_NUM          404
#define SCAP_INTERNAL_ERROR               500

#define SCAP_INVALID_TOTP_CODE_DESCR      "401_CLIENT_BAD_TOTP"
#define SCAP_MAX_CLOCK_DIFF               3*60    // 3 minutes of clock drift is allowed in SCAP services

struct ScapProvider;
struct ScapAttribute;
struct ScapSubAttribute;
struct ScapCredentials;

struct ScapResponse
{
	unsigned int status;
	std::string response;
	std::string server_time;
};

struct ScapRequest
{
	std::string endpoint;
	std::vector<std::string> headers;
	std::vector<std::string> get_parameters;
	std::string body;
};

struct Document
{
	std::string name;
	std::string hash;
	std::string original_signature;
	std::string file_path;
	QTemporaryDir *temp_dir;
	PTEID_PDFSignature *sign_handle;
};

struct ScapTransaction
{
	std::string id;
	std::string sad;
	std::string certificate_chain;
	std::string provider_name;
	std::vector<ScapAttribute> attributes;
};

struct CitizenInfo
{
	bool national_citizen;
	std::string name;
	std::string doc_type;
	std::string doc_nationality;
	std::string doc_number;
	std::string certificate;
	std::string cert_ssn;
};

ScapResponse perform_request(const ScapCredentials &credentials, const ScapRequest &request, CURL *curl = NULL);
ScapResponse perform_polling_request(const ScapCredentials &credentials, const ScapRequest &request);

/* Create requests and parse responses*/
unsigned int get_response_status(const std::string &response);
std::string get_response_code_description(const std::string &response);

std::vector<ScapProvider> parse_providers(const std::string &read_buffer);

std::string create_fetch_attributes_body(const std::string &processId);

std::string create_search_attributes_body(const CitizenInfo &citizen_info,
	const std::vector<ScapProvider> &providers,
	const std::string &processId,
	const ScapCredentials &credentials,
	bool allEnterprise,
	bool allEmployee);

std::vector<ScapAttribute> parse_attributes(const std::string& response);

std::string extract_secret_key(const std::string& response);

std::string create_authorization_body(const std::string &processId,
	const CitizenInfo &citizen_info,
	const ScapCredentials &credentials,
	const std::vector<ScapAttribute> &attributes,
	const std::vector<Document> &documents);

std::vector<ScapTransaction> parse_authorization(const std::string& response);

std::string create_sign_hash_body(const std::string &processId,
	const std::string &id,
	const ScapCredentials &credentials,
	const ScapTransaction &transaction,
	const std::vector<std::string> &hashes);

std::string create_get_signed_hash_body(const std::string &processId, const ScapTransaction &transaction);

std::vector<std::string> parse_signatures(const std::string &response);

std::vector<std::string> get_failed_providers_ids(const std::string &response);

};
