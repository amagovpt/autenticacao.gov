/*-****************************************************************************

 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#pragma once

#include "string"

enum class ScapError {
	generic,
	connection,
	timeout,
	proxy_auth,
	possibly_proxy,
	bad_credentials,
	oauth_failure,
	oauth_cancelled,
	oauth_timeout,
	oauth_connection,
	cache_write_failure,
	cache_read_failure,
	cache_removed_legacy,
	temp_dir_write_failure,
	null_signing_device,
	empty_attribute_list,
	bad_secret_key,
	clock,
	sign_cmd_connection,
	sign_cmd_invalid_code,
	sign_cmd_invalid_account,
	sign_cmd_generic,
	sign_timestamp,
	sign_ltv,
	sign_unsupported_pdf,
	sign_permission_denied,
	sign_cancel,
	sign_pin_cancel,
	sign_pin_blocked,
	expired_attributes,
	no_attributes,
	incomplete_response,
};

template<class T>
class ScapResult {
public:
	ScapResult(const T &data) : m_data(data), m_is_error(false) {};
	ScapResult(const ScapError &error) : m_error(error), m_is_error(true) { set_critical(error); };
	ScapResult(const ScapError &error, const std::vector<std::string> &error_data, const T& data)
		: m_data(data), m_error(error), m_is_error(true), m_error_data(error_data) { set_critical(error); };

	bool is_error() const { return m_is_error; };
	bool is_critical() const { return m_is_error && m_is_critical; };
	ScapError error() const { return m_error; };
	T data() const { return m_data; };
	std::vector<std::string> error_data() const { return m_error_data; };

private:
	void set_critical(const ScapError &error) {
		m_is_critical = (error != ScapError::sign_timestamp && error != ScapError::sign_ltv);
	}

	T m_data;
	ScapError m_error;
	bool m_is_error;
	bool m_is_critical;
	std::vector<std::string> m_error_data;
};

template<>
class ScapResult<void> {
public:
	ScapResult() : m_is_error(false), m_is_critical(false) {};
	ScapResult(const ScapError &error) : m_error(error), m_is_error(true) { set_critical(error); };
	ScapResult(const ScapError &error, const std::vector<std::string> &error_data)
			: m_error(error), m_is_error(true), m_error_data(error_data) { set_critical(error); };

	bool is_error() const { return m_is_error; };
	bool is_critical() const { return m_is_error && m_is_critical; };
	ScapError error() const { return m_error; };
	std::vector<std::string> error_data() const { return m_error_data; };

private:
	void set_critical(const ScapError &error) {
		m_is_critical = (error != ScapError::sign_timestamp && error != ScapError::sign_ltv);
	}

	ScapError m_error;
	bool m_is_error;
	bool m_is_critical;
	std::vector<std::string> m_error_data;
};