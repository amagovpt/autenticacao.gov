/*-****************************************************************************

 * Copyright (C) 2022-2023 José Pinto - <jose.pinto@caixamagica.pt>
 * Copyright (C) 2023 Nuno Romeu Lopes - <nuno.lopes@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <QUuid>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDateTime>
#include <QImage>

#include <regex>

#include "scapclient.h"
#include "scapservice.h"
#include "scapsettings.h"
#include "scaperrors.h"
#include "scapcache.h"

#include "pdfsignatureutils.h"
#include "Log.h"
#include "MiscUtil.h"
#include "Config.h"
#include "eidlibException.h"
#include "SSLConnection.h"
#include "PDFSignature.h"
#include "OAuthAttributes.h"
#include "eidErrors.h"

#define ABORT_ON_CRITICAL_ERROR(x)                                                                                     \
	if (x.is_critical())                                                                                               \
		return x;

namespace eIDMW {

struct SignatureDetails {
	QByteArray signature;
	QByteArray document_hash;
	QByteArray signing_certificate;
};

// implemented in pdfsignatureutils.h
bool get_citizen_signature_details(const QString &filepath, SignatureDetails &sig_details);

static std::string generate_process_id() { return QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString(); }

static std::string get_nic_from_subject_sn(const std::string &subject_serial_number) {
	// remove any textual prefix and checkdigit from NIC
	// Regex for 8-digit NIC number
	std::regex nic_pattern("\\d{8}");

	std::smatch match;

	if (std::regex_search(subject_serial_number, match, nic_pattern)) {
		return match[0];
	} else {
		MWLOG(LEV_ERROR, MOD_SCAP, "No NIC found in subject SN: %s", subject_serial_number.c_str());
		return "";
	}
}

static std::string get_nic_from_cert(PTEID_Certificate &certificate) {
	std::string subject_serial_number = certificate.getSubjectSerialNumber();
	return get_nic_from_subject_sn(subject_serial_number);
}

static std::string get_owner_from_cert(PTEID_Certificate &certificate) { return certificate.getOwnerName(); }

static void save_secret_key(const std::string &secret_key, const std::string &nic) {
	if (secret_key.empty() || nic.empty()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s Failed to save secret_key: empty secret key or NIC", __FUNCTION__);
		return;
	}

	ScapSettings settings;
#ifdef __APPLE__
	// With a secretKey already stored in MacOS keychain we can't overwrite it in setSecretKey()
	// TODO: remove just the matching key for nic
	settings.resetScapKeys();
#endif

	settings.setSecretKey(secret_key, nic);
}

static std::vector<CByteArray> parse_cert_chain(const std::string &cert_chain) {
	std::vector<std::string> certs = parsePEMCertSequence((char *)cert_chain.c_str(), cert_chain.size());
	std::vector<CByteArray> cert_chain_data;

	for (size_t i = 0; i < certs.size(); ++i) {
		CByteArray ba;
		unsigned char *der = NULL;
		int der_len = PEM_to_DER((char *)certs.at(i).c_str(), &der);

		if (der_len < 0) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s PEM -> DER conversion failed - len: %d", __FUNCTION__, der_len);
			return {};
		}

		ba.Append((const unsigned char *)der, (unsigned long)der_len);
		cert_chain_data.push_back(ba);
	}
	return cert_chain_data;
}

static bool set_extern_certificates(PDFSignature *sig_handler, const std::string &certificate_chain) {
	std::vector<CByteArray> cert_chain_data = parse_cert_chain(certificate_chain);
	if (cert_chain_data.empty()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Empty certificate chain in SCAP signature!");
		return false;
	}

	sig_handler->setExternCertificate(cert_chain_data.front());
	std::vector<CByteArray> ca_certs(cert_chain_data.begin() + 1, cert_chain_data.end());
	sig_handler->setExternCertificateCA(ca_certs);
	return true;
}

// returns a string with format: "{AttributeX, AttributeY e AttributeZ}"
static std::string join_attribute_strings(const std::vector<ScapAttribute> &attributes) {
	std::string result;

	for (auto it = attributes.begin(); it != attributes.end(); ++it) {
		result += it->description;
		if (std::next(it, 2) == attributes.end()) { // if second to last element
			result += " e ";
		} else if (std::next(it) != attributes.end()) {
			result += ", ";
		}
	}

	if (attributes.size() > 1) {
		result.insert(result.begin(), '{');
		result += "}";
	}

	return result;
}

std::string get_professional_name(const std::vector<ScapAttribute> &attributes) {
	std::string professional_name = "";
	bool checkProfessionalName = false;

	for (auto it = attributes.begin(); it != attributes.end(); ++it) {

		if (it == attributes.begin()) {

			for (const ScapSubAttribute &sub_attribute : it->sub_attributes) {
				if (sub_attribute.id == "NomeProfissional") {
					professional_name = sub_attribute.value;
				}
			}

			// if the first attribute doesn't have professional name
			if (professional_name.empty()) {
				return professional_name;
			}
		} else {
			checkProfessionalName = false;

			for (const ScapSubAttribute &sub_attribute : it->sub_attributes) {
				if (sub_attribute.id == "NomeProfissional" && sub_attribute.value == professional_name) {
					checkProfessionalName = true;
				}
			}

			// all atributes must have the same professional name
			if (checkProfessionalName == false) {
				professional_name = "";
				return professional_name;
			}
		}
	}

	return professional_name;
}

attributes_by_provider_map group_by_provider(const std::vector<ScapAttribute> &attributes) {
	attributes_by_provider_map grouped;
	for (const ScapAttribute &attribute : attributes) {
		std::vector<ScapAttribute> &attrs = grouped[attribute.provider];
		attrs.push_back(attribute);
	}

	return grouped;
}

std::pair<std::string, std::string> format_scap_seal_strings(const std::vector<ScapAttribute> &attributes) {
	std::string certified_by;
	std::string certified_attributes;

	std::set<std::string> provider_names;

	std::string separator = "";
	for (const auto &provider_attributes_pair : group_by_provider(attributes)) {
		const ScapProvider &provider = provider_attributes_pair.first;
		certified_attributes += separator + join_attribute_strings(provider_attributes_pair.second);

		if (provider.type == "ENTERPRISE") {
			provider_names.insert("SCAP");
			certified_attributes += " de " + provider.name + "\nVATPT-" + provider.nipc;
		} else {
			provider_names.insert(provider.name);
		}
		separator = " e ";
	}

	certified_attributes += "\n";

	separator = "";
	for (std::string provider_name : provider_names) {
		certified_by += separator + provider_name;
		separator = " e ";
	}

	return std::make_pair(certified_by, certified_attributes);
}

// returns string of format: "Entidade: X. Na qualidade de: Y. Subatributos: X1:Y1;(...) Xn:Yn. E na qualidade de (...)"
static std::string format_reason_field_string(const ScapTransaction &transaction,
	const std::vector<ScapAttribute> &attributes) {
	std::string provider_nipc;

	// Look for provider NIPC in the attributes, since the transaction's attributes don't have the NIPC
	for (const ScapAttribute &attribute : attributes) {
		if (attribute.provider.name == transaction.provider_name && attribute.provider.type == "ENTERPRISE") {
			provider_nipc = attribute.provider.nipc;
			break;
		}
	}

	std::string reason = "Entidade: " + transaction.provider_name + ". ";
	if (!provider_nipc.empty()) {
		reason += "VATPT-" + provider_nipc + ".";
	}

	reason += " Na qualidade de: ";

	std::string separator = "";

	ScapProvider current_provider;
	for (const ScapAttribute &attribute : transaction.attributes) {
		reason += separator + attribute.description + ". Subatributos: ";
		for (auto it = attribute.sub_attributes.begin(); it != attribute.sub_attributes.end(); ++it) {
			reason += it->description + ": " + it->value;
			reason += (std::next(it) != attribute.sub_attributes.end()) ? "; " : ". "; // end sentence with "."
		}

		separator = "E na qualidade de: ";
	}

	return reason;
}

static std::string open_scap_signature(const ScapTransaction &transaction, Document &document,
									   const PDFSignatureInfo &signature_info, const CitizenInfo &citizen_info,
									   const std::vector<ScapAttribute> &attributes, bool is_last_signature,
									   bool is_batch, bool is_cc) {
	document.sign_handle = new PTEID_PDFSignature(document.file_path.c_str());

	PTEID_PDFSignature *signature = document.sign_handle;
	PDFSignature *sig_handler = signature->getPdfSignature();

	if (!set_extern_certificates(sig_handler, transaction.certificate_chain)) {
		return "";
	}

	std::string reason;
	std::string output;
	if (is_last_signature) {

		if (signature_info.is_visible) {
			const SealGeometry &seal_geometry = signature_info.seal_geometry;
			if (seal_geometry.x >= 0 && seal_geometry.y >= 0) {
				unsigned int selected_page;
				const int page_count = sig_handler->getPageCount();
				if (signature_info.is_last_page || signature_info.selected_page < 1 ||
					signature_info.selected_page > page_count) {
					selected_page = page_count;
				} else {
					selected_page = signature_info.selected_page;
				}

				sig_handler->setVisibleCoordinates(selected_page, seal_geometry.x, seal_geometry.y);
				sig_handler->setCustomSealSize(seal_geometry.width, seal_geometry.height);
			}

			const std::pair<std::string, std::string> seal_strings = format_scap_seal_strings(attributes);

			std::string sig_name;
			std::string professional_name = get_professional_name(attributes);

			if (professional_name.empty()) {
				sig_name = citizen_info.name;
			} else {
				sig_name = professional_name;
			}

			sig_handler->setSCAPAttributes(_strdup(sig_name.c_str()), _strdup(citizen_info.cert_ssn.c_str()),
										   _strdup(seal_strings.first.c_str()), _strdup(seal_strings.second.c_str()));

			sig_handler->setIsCC(is_cc); // set correct default seal image
			if (signature_info.use_custom_image) {
				sig_handler->setCustomImage(signature_info.seal_image_data, signature_info.seal_image_length);
			} else {
				// if provider has custom logo, use that logo.
				// if more than one of the selected providers has a logo, use default CC/CMD image.
				QByteArray logo = get_scap_image_data(attributes);
				if (!logo.isEmpty()) {
					sig_handler->setCustomImage(reinterpret_cast<unsigned char *>(logo.data()), logo.size());
				}
			}
		}

		reason = signature_info.reason;

		if (is_batch) {
			QString tmp_out = signature_info.output.c_str();
			output = (QString::fromStdString(signature_info.output) + QDir::separator() + document.name.c_str())
						 .toStdString();
		} else {
			output = signature_info.output;
		}

		signature->setSignatureLevel(signature_info.level);
	} else {
		reason = format_reason_field_string(transaction, attributes);
		output = document.file_path;
	}

	try {
		sig_handler->signFiles(signature_info.location.c_str(), reason.c_str(), output.c_str(), false);
	} catch (CMWException &e) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s PDFSignature::signFiles() failed", __FUNCTION__);
		throw PTEID_Exception(e.GetError());
	}

	/* Calculate hash */
	CByteArray hash_ba = sig_handler->getHash();
	if (hash_ba.Size() == 0) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s PDFSignature::getHash() failed", __FUNCTION__);
	}

	unsigned char sha256_prefix[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
									 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};

	CByteArray signatureInput(sha256_prefix, sizeof(sha256_prefix));
	signatureInput.Append(hash_ba);

	return QByteArray((const char *)signatureInput.GetBytes(), signatureInput.Size()).toBase64().constData();
}

static void get_citizen_info_from_signature(PTEID_PDFSignature &signature, CitizenInfo *citizen_info) {
	std::string ssn = signature.getCertificateCitizenID();

	citizen_info->name = signature.getCertificateCitizenName();

	if (ssn.find("BI") == 0) { // national citizen
		citizen_info->cert_ssn = get_nic_from_subject_sn(ssn);
		citizen_info->doc_type = "BI";
		citizen_info->doc_nationality = "PT";
		citizen_info->doc_number = get_nic_from_subject_sn(ssn);
	} else {
		// e.g.: PASAF-ABC123456
		citizen_info->cert_ssn = ssn;
		citizen_info->doc_type = ssn.substr(0, 3);		  // e.g.: PAS
		citizen_info->doc_nationality = ssn.substr(3, 2); // e.g.: AF
		citizen_info->doc_number = ssn.substr(6);		  // e.g.: ABC123456
	}
}

static ScapError interpret_exception_code(const long code, const char *call) {
	switch (code) {
	case EIDMW_TIMESTAMP_ERROR:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: failed to apply timestamp.", call);
		return ScapError::sign_timestamp;
	case EIDMW_LTV_ERROR:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: failed to apply validation data (LTV).", call);
		return ScapError::sign_ltv;
	case EIDMW_PDF_UNSUPPORTED_ERROR:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: unsupported pdf.", call);
		return ScapError::sign_unsupported_pdf;
	case EIDMW_PERMISSION_DENIED:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: permission denied.", call);
		return ScapError::sign_permission_denied;
	case EIDMW_ERR_PIN_CANCEL:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: pin insertion cancelled by user.", call);
		return ScapError::sign_pin_cancel;
	case EIDMW_ERR_PIN_BLOCKED:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: pin is blocked.", call);
		return ScapError::sign_pin_blocked;
	case EIDMW_ERR_OP_CANCEL:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: operation cancelled by user.", call);
		return ScapError::sign_cancel;
	case EIDMW_ERR_CMD_CONNECTION:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: cmd service connection error.", call);
		return ScapError::sign_cmd_connection;
	case EIDMW_ERR_CMD_INVALID_CODE:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: cmd service invalid otp.", call);
		return ScapError::sign_cmd_invalid_code;
	case EIDMW_ERR_CMD_INACTIVE_ACCOUNT:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: cmd service invalid account.", call);
		return ScapError::sign_cmd_invalid_account;
	case EIDMW_ERR_CMD_SERVICE:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: cmd service generic error.", call);
		return ScapError::sign_cmd_generic;
	default:
		MWLOG(LEV_ERROR, MOD_SCAP, "%s: caught exception: %08lx.", call, code);
		return ScapError::generic;
	}
}

static ScapResult<void> perform_citizen_signatures(PTEID_SigningDevice *device, const PDFSignatureInfo &signature_info,
												   CitizenInfo *out_citizen_info,
												   std::vector<Document> &out_documents) {
	PTEID_PDFSignature signature;
	for (const std::string &filename : signature_info.filenames) {
		signature.addToBatchSigning((char *)filename.c_str());
	}

	// If a SCAP signature is LEVEL_TIMESTAMP or higher the 1st citizen signature must include timestamp
	signature.setSignatureLevel(signature_info.level >= PTEID_LEVEL_TIMESTAMP ? PTEID_LEVEL_TIMESTAMP
																			  : PTEID_LEVEL_BASIC);

	QTemporaryDir *tempDir = new QTemporaryDir();
	if (!tempDir->isValid()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s SCAP Signature error: Failed to create temporary directory", __FUNCTION__);
		return ScapError::temp_dir_write_failure;
	}

	std::string output_path = tempDir->path().toStdString();

	ScapResult<void> result;
	try {
		device->SignPDF(signature, 0, 0, 0, signature_info.location.c_str(), signature_info.reason.c_str(),
						output_path.c_str());
	} catch (const PTEID_ExBatchSignatureFailed &ex) {
		if (signature_info.filenames.size() > 1) {
			const std::string &failed_filename = signature_info.filenames.at(ex.GetFailedSignatureIndex());
			const ScapError error = interpret_exception_code(ex.GetError(), __FUNCTION__);

			result = {error, {failed_filename}};
		} else { // not really a batch signature - return simple error without filenames
			result = interpret_exception_code(ex.GetError(), __FUNCTION__);
		}
		ABORT_ON_CRITICAL_ERROR(result);
	} catch (const PTEID_Exception &ex) {
		result = interpret_exception_code(ex.GetError(), __FUNCTION__);
		ABORT_ON_CRITICAL_ERROR(result);
	}

	SignatureDetails details;
	QDir dir(output_path.c_str());
	QFileInfoList file_list = dir.entryInfoList(QDir::Files, QDir::Time | QDir::Reversed);
	for (const auto &fileinfo : file_list) {
		QString file_path = fileinfo.absoluteFilePath();

		if (!get_citizen_signature_details(file_path, details)) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s: Failed to get citizen signature details", __FUNCTION__);
			return ScapError::generic;
		}

		Document document;
		document.name = fileinfo.fileName().toStdString();
		document.hash = details.document_hash.toBase64().constData();
		document.original_signature = details.signature.toBase64().constData();
		document.file_path = file_path.toStdString();
		document.temp_dir = tempDir;
		document.sign_handle = NULL;

		out_documents.push_back(document);

		if (out_citizen_info->certificate.empty()) {
			out_citizen_info->certificate = details.signing_certificate.constData();
		}
	}

	get_citizen_info_from_signature(signature, out_citizen_info);
	return result;
}

static CByteArray decode_base64(const std::string &encoded) {
	unsigned int decoded_len = 0;
	unsigned char *decoded_buffer;
	Base64Decode(encoded.c_str(), encoded.size(), decoded_buffer, decoded_len);

	CByteArray decoded(decoded_buffer, decoded_len);
	free(decoded_buffer);

	return decoded;
}

static ScapResult<void> close_scap_signature(std::vector<Document> &documents,
											 const std::vector<std::string> &signatures) {
	ScapResult<void> result;
	for (size_t i = 0; i < documents.size(); ++i) {
		Document &doc = documents.at(i);
		const std::string &sig_base64 = signatures.at(i);

		CByteArray signature = decode_base64(sig_base64);

		try {
			doc.sign_handle->getPdfSignature()->signClose(signature);
		} catch (const CMWException &ex) {
			if (documents.size() > 1) {
				result = {interpret_exception_code(ex.GetError(), __FUNCTION__), {doc.name}};
			} else { // not really a batch signature - return simple error without filenames
				result = interpret_exception_code(ex.GetError(), __FUNCTION__);
			}
			ABORT_ON_CRITICAL_ERROR(result);
		}

		delete doc.sign_handle;
		doc.sign_handle = NULL;
	}

	return result;
}

static bool missing_credentials(const ScapCredentials &credentials) {
	return credentials.basic_user.empty() || credentials.basic_pass.empty() || credentials.credential_id.empty();
}

static ScapCredentials load_credentials_from_config() {
	std::wstring appid = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_APPID);
	std::wstring user = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_USERID);
	std::wstring passw = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_PASSWORD);

	return {std::string(user.begin(), user.end()), std::string(passw.begin(), passw.end()),
			std::string(appid.begin(), appid.end())};
}

ScapClient::ScapClient(const ScapCredentials &credentials) {
	const ScapCredentials credentials_from_config = load_credentials_from_config();
	m_scap_credentials = missing_credentials(credentials_from_config) ? credentials : credentials_from_config;

	m_oauth = NULL;
}

static ScapError map_perform_error(const unsigned long error) {
	switch (error) {
	case CURL_PERFORM_ERROR:
		return ScapError::connection;
	case PROXY_AUTH_REQUIRED:
		return ScapError::proxy_auth;
	case POSSIBLE_PROXY_ERROR:
		return ScapError::possibly_proxy;
	case SSL_PIN_CANCELED_ERROR:
		return ScapError::sign_pin_cancel;
	default:
		return ScapError::generic;
	}
}

ScapResult<std::vector<ScapProvider>> ScapClient::getAttributeProviders() {
	if (missing_credentials(m_scap_credentials)) {
		return ScapError::bad_credentials;
	}

	ScapRequest request;
	request.endpoint = "/SCAPAttributeService/searchAttributeProviderInstitutionsRequest";
	request.get_parameters = {"processId=" + generate_process_id()};

	ScapResponse response = perform_request(m_scap_credentials, request);
	if (response.status != SCAP_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "searchAttributeProviderInstitutionsRequest failed with error: %d", response.status);
		return map_perform_error(response.status);
	}
	std::vector<ScapProvider> providers = parse_providers(response.response);
	return providers;
}

static ScapResult<void> oauth_get_citizen_info(CitizenInfo &citizen_info, std::string &oauth_token,
											   OAuthAttributes *&m_oauth) {
	m_oauth = new OAuthAttributes({CitizenAttribute::NIC, CitizenAttribute::COMPLETENAME, CitizenAttribute::DOCTYPE,
								   CitizenAttribute::DOCNATIONALITY, CitizenAttribute::DOCNUMBER});

	OAuthResult status = m_oauth->fetchAttributes();
	if (status != OAuthSuccess) {
		if (status == OAuthCancelled) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s Oauth canceled by user.", __FUNCTION__);
			return ScapError::oauth_cancelled;
		} else if (status == OAuthTimeoutError) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s Oauth timed out.", __FUNCTION__);
			return ScapError::oauth_timeout;
		} else if (status == OAuthConnectionError) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s Oauth connection error.", __FUNCTION__);
			return ScapError::oauth_connection;
		}

		MWLOG(LEV_ERROR, MOD_SCAP, "%s Failed to get OAuth Attributes", __FUNCTION__);
		return ScapError::oauth_failure;
	}

	const auto oauth_attributes = m_oauth->getAttributes();
	citizen_info.national_citizen = !oauth_attributes->at(CitizenAttribute::NIC).empty();
	citizen_info.name = oauth_attributes->at(CitizenAttribute::COMPLETENAME);

	if (citizen_info.national_citizen) {
		citizen_info.doc_number = oauth_attributes->at(CitizenAttribute::NIC);
		citizen_info.doc_type = "BI";
		citizen_info.doc_nationality = "PT";
	} else {
		citizen_info.doc_number = oauth_attributes->at(CitizenAttribute::DOCNUMBER);
		citizen_info.doc_type = oauth_attributes->at(CitizenAttribute::DOCTYPE);
		citizen_info.doc_nationality = oauth_attributes->at(CitizenAttribute::DOCNATIONALITY);
	}

	oauth_token = m_oauth->getToken();

	if (m_oauth) {
		delete m_oauth;
		m_oauth = NULL;
	}

	return {};
}

static std::vector<std::string> filter_providers_by_ids(const std::vector<ScapProvider> &providers,
														const std::vector<std::string> &ids) {
	std::vector<std::string> result;
	for (const ScapProvider &p : providers) {
		if (std::find(ids.begin(), ids.end(), p.uriId) != ids.end()) {
			result.push_back(p.name);
		}
	}

	return result;
}

#ifdef _WIN32

// Implemented in sslconnection_helper.cpp
void setupSSLCallbackFunction(bool is_ecdsa);
void setThreadLocalCardInstance(PTEID_EIDCard *card);

#endif

ScapResult<std::vector<ScapAttribute>> ScapClient::getCitizenAttributes(PTEID_EIDCard *card,
																		const std::vector<ScapProvider> &providers,
																		bool allEnterprise, bool allEmployee) {
	std::vector<ScapAttribute> attributes;
	if (missing_credentials(m_scap_credentials)) {
		return ScapError::bad_credentials;
	}

	CURL *curl = NULL;
	std::unique_ptr<SSLConnection> connection;

	CitizenInfo citizen_info;
	std::vector<std::string> headers;
	if (card) {

#ifdef _WIN32
		setThreadLocalCardInstance(card);
		setupSSLCallbackFunction(card->getType() == PTEID_CARDTYPE_IAS5);
#endif

		MWLOG(LEV_DEBUG, MOD_SCAP, "Building SSLConnection for SCAPClient...");
		connection.reset(card->buildSSLConnection());
		curl = connection->connect_encrypted();

		citizen_info.national_citizen = true;
		citizen_info.doc_number = get_nic_from_cert(card->getCertificates().getCert(PTEID_Certificate::CITIZEN_AUTH));
		citizen_info.name = get_owner_from_cert(card->getCertificates().getCert(PTEID_Certificate::CITIZEN_AUTH));
		citizen_info.doc_type = "BI";
		citizen_info.doc_nationality = "PT";

	} else {
		std::string oauth_token;
		const auto oauth_result = oauth_get_citizen_info(citizen_info, oauth_token, m_oauth);
		if (oauth_result.is_error()) {
			return oauth_result.error();
		}

		headers.push_back(std::string("FAAuthorization: ") + oauth_token);
	}

	std::string process_id = generate_process_id();

	ScapRequest first_request;
	first_request.endpoint = "/SCAPAttributeService/searchCitizenAttributesRequest";
	first_request.headers = headers;
	first_request.body = create_search_attributes_body(citizen_info, providers, process_id, m_scap_credentials,
													   allEnterprise, allEmployee);

	ScapResponse first_response = perform_request(m_scap_credentials, first_request, curl);
	if (first_response.status != SCAP_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "searchCitizenAttributesRequest failed with error: %d", first_response.status);
		return map_perform_error(first_response.status);
	}

	MWLOG(LEV_DEBUG, MOD_SCAP, "searchCitizenAttributesRequest successfull");

	ScapRequest second_request;
	second_request.endpoint = "/SCAPAttributeService/getCitizenAttributesResponse";
	second_request.body = create_fetch_attributes_body(process_id);

	ScapResponse second_response = perform_polling_request(m_scap_credentials, second_request);
	if (second_response.status != SCAP_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "getCitizenAttributesResponse polling request failed: %d", second_response.status);
		return map_perform_error(second_response.status);
	}

	const std::string &response = second_response.response;
	unsigned int status = get_response_status(response);
	if (status != SCAP_OK && status != SCAP_INCOMPLETE_RESPONSE) {
		if (status == SCAP_PROCESSING_REQUEST) {
			MWLOG(LEV_ERROR, MOD_SCAP, "getCitizenAttributesResponse: Service timed out.");
			return ScapError::timeout;
		} else if (status == SCAP_EXPIRED_OR_NO_ATTRS) {
			MWLOG(LEV_ERROR, MOD_SCAP, "getCitizenAttributesResponse: no attributes or expired attributes.");
			const auto failed_providers_ids = get_failed_providers_ids(response);
			std::vector<std::string> failed_providers = filter_providers_by_ids(providers, failed_providers_ids);
			return {ScapError::no_attributes, failed_providers, attributes};
		} else {
			MWLOG(LEV_ERROR, MOD_SCAP, "getCitizenAttributesResponse: Unexpected status: %d.", status);
			return ScapError::generic;
		}
	}

	MWLOG(LEV_DEBUG, MOD_SCAP, "Got citizen attributes successfully");

	std::string secret_key = extract_secret_key(second_response.response);
	save_secret_key(secret_key, citizen_info.doc_number);
	std::fill(secret_key.begin(), secret_key.end(), 0);

	if (!cache_response(second_response.response, citizen_info.doc_number, attributes)) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to cache getCitizenAttributesResponse response");
		return ScapError::cache_write_failure;
	}

	if (status == SCAP_INCOMPLETE_RESPONSE) {
		// polling reached timeout limit but got some results
		const auto failed_providers_ids = get_failed_providers_ids(response);
		std::vector<std::string> failed_providers = filter_providers_by_ids(providers, failed_providers_ids);
		return {ScapError::incomplete_response, failed_providers, attributes};
	}

	return attributes;
}

ScapResult<std::vector<ScapAttribute>> ScapClient::readAttributeCache() { return load_cache(); }

bool ScapClient::clearAttributeCache() { return clear_cache(); }

static void clean_up_temp_documents(const std::vector<Document> &documents) {
	if (!documents.empty()) {
		delete documents.front().temp_dir;
	}
}

static bool validate_local_clock(const ScapResponse &response) {
	QString server_time_string = QString::fromStdString(response.server_time);
	QDateTime server_time = QDateTime::fromString(server_time_string, Qt::RFC2822Date);
	long local = time(nullptr);
	long server = server_time.toSecsSinceEpoch();

	MWLOG(LEV_DEBUG, MOD_SCAP, "signHashAuthorization: local time %ld - server time %ld.", local, server);

	return abs(difftime(local, server)) < SCAP_MAX_CLOCK_DIFF;
}

ScapResult<void> ScapClient::sign(PTEID_SigningDevice *device, const PDFSignatureInfo &signature_info,
								  const std::vector<ScapAttribute> &attributes) {
	ScapResult<void> result;
	if (missing_credentials(m_scap_credentials)) {
		return ScapError::bad_credentials;
	}

	if (!device) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s NULL pointer to signing device.", __FUNCTION__);
		return ScapError::null_signing_device;
	}

	if (attributes.empty()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s No attributes selected.", __FUNCTION__);
		return ScapError::empty_attribute_list;
	}

	if (!ScapSettings::secretKeyExists()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s No secret key found.", __FUNCTION__);
		return ScapError::bad_secret_key;
	}

	CitizenInfo citizen_info;
	std::vector<Document> documents;

	const auto citizen_sign_result = perform_citizen_signatures(device, signature_info, &citizen_info, documents);
	if (citizen_sign_result.is_error()) {
		if (citizen_sign_result.is_critical()) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s Citizen Signature failed.", __FUNCTION__);
			clean_up_temp_documents(documents);
			return citizen_sign_result;
		}

		result = citizen_sign_result; // save status to report later
	}

	std::string process_id = generate_process_id();

	ScapRequest request;
	request.endpoint = "/SCAPSignatureService/signHashAuthorization";
	request.body = create_authorization_body(process_id, citizen_info, m_scap_credentials, attributes, documents);

	if (request.body.empty()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s Failed to create signHashAuthorization request body.", __FUNCTION__);
		clean_up_temp_documents(documents);
		return ScapError::generic;
	}

	ScapResponse response = perform_request(m_scap_credentials, request);
	if (response.status != SCAP_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "signHashAuthorization failed with error: %d", response.status);
		clean_up_temp_documents(documents);
		return map_perform_error(response.status);
	}

	unsigned int status = get_response_status(response.response);

	if (status == SCAP_EXPIRED_OR_NO_ATTRS) {
		MWLOG(LEV_ERROR, MOD_SCAP, "signHashAuthorization: Citizen has no attributes.");
		clean_up_temp_documents(documents);
		return ScapError::no_attributes;
	} else if (status == SCAP_EXPIRED_ATTRS) {
		MWLOG(LEV_ERROR, MOD_SCAP, "signHashAuthorization: Citizen attributes were expired.");
		clean_up_temp_documents(documents);
		return ScapError::expired_attributes;
	} else if (status == SCAP_TRANSACTION_OR_TOTP_ERROR) {
		clean_up_temp_documents(documents);
		std::string code_descr = get_response_code_description(response.response);

		if (code_descr.compare(SCAP_INVALID_TOTP_CODE_DESCR) == 0) {
			if (!validate_local_clock(response)) {
				return ScapError::clock;
			}
			MWLOG(LEV_ERROR, MOD_SCAP, "signHashAuthorization: Invalid Secret Key.");
			return ScapError::bad_secret_key;
		}

		return ScapError::generic;
	} else if (status != SCAP_OK) {
		MWLOG(LEV_ERROR, MOD_SCAP, "signHashAuthorization: Received status %d", status);
		clean_up_temp_documents(documents);
		return ScapError::generic;
	}

	std::vector<ScapTransaction> transactions = parse_authorization(response.response);
	if (transactions.empty()) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Empty transaction list. Failed to parse signHashAuthorization response.");
		clean_up_temp_documents(documents);
		return ScapError::generic;
	}

	bool is_batch = documents.size() > 1;
	bool is_cc = device->getDeviceType() == PTEID_SigningDeviceType::CC;
	for (const ScapTransaction &transaction : transactions) {
		bool is_last_signature = stoul(transaction.id) == transactions.size() - 1;

		std::vector<std::string> hashes;
		for (Document &doc : documents) {
			std::string hash_ba = open_scap_signature(transaction, doc, signature_info, citizen_info, attributes,
													  is_last_signature, is_batch, is_cc);
			if (hash_ba.empty()) {
				// Abort the signature operation as the SCAP server didn't return any certificates
				return ScapError::generic;
			}
			hashes.push_back(hash_ba);
		}

		ScapRequest sign_hash_request;
		sign_hash_request.endpoint = "/SCAPSignatureService/signHash";
		sign_hash_request.body =
			create_sign_hash_body(process_id, citizen_info.doc_number, m_scap_credentials, transaction, hashes);

		ScapResponse sign_hash_response = perform_request(m_scap_credentials, sign_hash_request);
		if (sign_hash_response.status != SCAP_OK) {
			MWLOG(LEV_ERROR, MOD_SCAP, "signHash failed with error: %d", sign_hash_response.status);
			clean_up_temp_documents(documents);
			return map_perform_error(sign_hash_response.status);
		}

		ScapRequest get_signed_hash_request;
		get_signed_hash_request.endpoint = "/SCAPSignatureService/getSignHashResult";
		get_signed_hash_request.body = create_get_signed_hash_body(process_id, transaction);

		ScapResponse get_signed_hash_response = perform_polling_request(m_scap_credentials, get_signed_hash_request);
		if (get_signed_hash_response.status != SCAP_OK) {
			MWLOG(LEV_ERROR, MOD_SCAP, "getSignHashResult failed with error: %d", get_signed_hash_response.status);
			clean_up_temp_documents(documents);
			return map_perform_error(get_signed_hash_response.status);
		}

		unsigned int get_signed_hash_response_status = get_response_status(get_signed_hash_response.response);
		if (get_signed_hash_response_status != SCAP_OK) {
			MWLOG(LEV_ERROR, MOD_SCAP, "getSignHashResult: Received status %d", get_signed_hash_response_status);
			clean_up_temp_documents(documents);
			std::string code_descr = get_response_code_description(get_signed_hash_response.response);
			if (code_descr.compare(SCAP_INVALID_CITIZEN_ATTRIBUTES) == 0) {
				MWLOG(LEV_ERROR, MOD_SCAP,
					  "getSignHashResult: Invalid cached attributes - maybe the provider changed attribute IDs");
				return ScapError::invalid_attributes;
			}
			return ScapError::generic;
		}

		std::vector<std::string> signatures = parse_signatures(get_signed_hash_response.response);
		const auto close_sig_status = close_scap_signature(documents, signatures);
		if (close_sig_status.is_error()) {
			if (close_sig_status.is_critical()) {
				MWLOG(LEV_ERROR, MOD_SCAP, "Failed to close scap signature.");
				clean_up_temp_documents(documents);
				return close_sig_status;
			}

			result = close_sig_status;
		}
	}

	clean_up_temp_documents(documents);
	return result;
}

void ScapClient::cancelOAuth() {
	if (m_oauth)
		m_oauth->closeListener();
}
}; // namespace eIDMW
