/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <QString>
#include <string>

#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <Wincrypt.h>
#endif

#ifdef __APPLE__
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFString.h>
#include <Security/SecItem.h>
#endif

#include "Log.h"
#include "scapsettings.h"

namespace eIDMW {

ScapSettings::ScapSettings() {
	{
		PTEID_Config config(PTEID_PARAM_GENERAL_CACHEDIR);
#ifdef WIN32
		QString cacheDir = QString::fromLatin1(config.getString());
#else
		QString cacheDir = config.getString();
#endif
		m_cache_dir = cacheDir;
	}
	{
		PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_HOST);
		m_scap_server_host = config.getString();
	}
	{
		PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_PORT);
		m_scap_server_port = config.getString();
	}
	{
		PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_APPID);
		m_appID = config.getString();
	}
}

void ScapSettings::setScapServerHost(const QString &host) {
	m_scap_server_host = host;

	PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_HOST);
	config.setString(host.toUtf8());
}

std::string ScapSettings::getScapServerHost() { return m_scap_server_host.toStdString(); }

void ScapSettings::setScapServerPort(int server_port) {
	m_scap_server_port = server_port;

	PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_PORT);
	config.setLong(server_port);
}

std::string ScapSettings::getScapServerPort() { return m_scap_server_port.toStdString(); }

static std::string removeCheckDigit(const std::string &nic) {
	// Remove NIC Check Digit
	const size_t nic_length = 8;
	return nic.substr(0, nic_length);
}

/* Native functions for saving and retrieving encrypted SCAP keys
 */

#ifdef _WIN32
void secureSaveSecretKey(const std::string &config_name, const std::string &secretKey) {
	assert(secretKey.length() <= ULONG_MAX);
	DATA_BLOB dataIn = {(DWORD) secretKey.length(), (BYTE *)secretKey.data()};
	DATA_BLOB dataOut = {0};
	DWORD dwFlags = 0;
	if (CryptProtectData(&dataIn, NULL, /* szDataDescription */
						 NULL,			/* pOptionalEntropy */
						 NULL,			/* pvReserved */
						 NULL,			/* pPromptStruct */
						 dwFlags, &dataOut)) {
		QByteArray array((const char *)dataOut.pbData, dataOut.cbData);
		PTEID_Config config(config_name.c_str(), L"general", L"");
		config.setString(array.toHex());
		LocalFree(dataOut.pbData);
	} else {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to protect secretKey, key is not stored! Error: %d", GetLastError());
	}
}

std::string secureGetSecretKey(QByteArray &secretKey_array) {
	std::string plaintext_key;
	assert(0 <= secretKey_array.length() <= ULONG_MAX);
	DATA_BLOB dataIn = {(DWORD) secretKey_array.length(), (BYTE *)secretKey_array.data()};
	DATA_BLOB dataOut = {0};
	DWORD dwFlags = 0;
	if (CryptUnprotectData(&dataIn, NULL, /* szDataDescription */
						   NULL,		  /* pOptionalEntropy */
						   NULL,		  /* pvReserved */
						   NULL,		  /* pPromptStruct */
						   dwFlags, &dataOut)) {
		plaintext_key.append((const char *)dataOut.pbData, dataOut.cbData);
		MWLOG(LEV_DEBUG, MOD_SCAP, "Fetched secretKey of %d bytes", plaintext_key.size());
		LocalFree(dataOut.pbData);
	} else {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to unprotect secretKey! Error: %d", GetLastError());
	}

	return plaintext_key;
}

#elif __APPLE__

#define KEYCHAIN_SERVICE_NAME "Autenticação.Gov SCAP key"

void secureSaveSecretKey(const std::string &config_name, const std::string &secretKey) {
	const int QUERY_ITEMS = 4;
	CFStringRef keys[QUERY_ITEMS];
	keys[0] = kSecClass;
	keys[1] = kSecAttrAccount;
	keys[2] = kSecAttrService;
	keys[3] = kSecValueData;

	CFTypeRef values[QUERY_ITEMS];
	values[0] = kSecClassGenericPassword;
	values[1] = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)config_name.data(), config_name.size(),
										kCFStringEncodingUTF8, FALSE);
	values[2] = CFSTR(KEYCHAIN_SERVICE_NAME);
	values[3] = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)secretKey.data(), secretKey.size(),
										kCFStringEncodingUTF8, FALSE);
	CFDictionaryRef secitem_attributes =
		CFDictionaryCreate((CFAllocatorRef)NULL, (const void **)keys, (const void **)values, QUERY_ITEMS,
						   &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	OSStatus status = SecItemAdd(secitem_attributes, NULL);

	if (status != 0) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s - SecItemAdd() failed with status: %d\n", __FUNCTION__, status);
	}

	CFRelease(secitem_attributes);
}

std::string secureGetSecretKey(const std::string &config_name) {
	std::string secret_key;
	const int QUERY_ITEMS = 4;

	CFStringRef keys[QUERY_ITEMS];
	keys[0] = kSecClass;
	keys[1] = kSecAttrAccount;
	keys[2] = kSecAttrService;
	keys[3] = kSecReturnData;

	CFTypeRef values[QUERY_ITEMS];
	values[0] = kSecClassGenericPassword;
	values[1] = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)config_name.data(), config_name.size(),
										kCFStringEncodingUTF8, FALSE);
	values[2] = CFSTR(KEYCHAIN_SERVICE_NAME);
	values[3] = kCFBooleanTrue;
	CFDictionaryRef query =
		CFDictionaryCreate((CFAllocatorRef)NULL, (const void **)keys, (const void **)values, QUERY_ITEMS,
						   &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	CFDataRef valueData = NULL;

	OSStatus status = SecItemCopyMatching(query, (CFTypeRef *)&valueData);

	if (status == errSecSuccess) {

		if (valueData != NULL) {
			secret_key.append((const char *)CFDataGetBytePtr(valueData), CFDataGetLength(valueData));
		}
	} else if (status == errSecItemNotFound) {
		MWLOG(LEV_INFO, MOD_SCAP, "Failed to find item in KeyChain errSecItemNotFound");
	} else {
		MWLOG(LEV_INFO, MOD_SCAP, "Failed to find item in KeyChain: %d\n", status);
	}
	CFRelease(query);

	return secret_key;
}

void keychain_delete_keys() {
	const int QUERY_ITEMS = 3;
	CFStringRef keys[QUERY_ITEMS];
	keys[0] = kSecClass;
	keys[1] = kSecAttrService;
	keys[2] = kSecMatchLimit;

	CFTypeRef values[QUERY_ITEMS];
	values[0] = kSecClassGenericPassword;
	values[1] = CFSTR(KEYCHAIN_SERVICE_NAME);
	values[2] = kSecMatchLimitAll;
	CFDictionaryRef query =
		CFDictionaryCreate((CFAllocatorRef)NULL, (const void **)keys, (const void **)values, QUERY_ITEMS,
						   &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	OSStatus status = SecItemDelete(query);

	MWLOG(LEV_INFO, MOD_SCAP, "SecItemDelete returned: %d\n", status);

	CFRelease(query);
}

bool keychain_keys_present() {
	const int QUERY_ITEMS = 2;
	CFStringRef keys[QUERY_ITEMS];
	keys[0] = kSecClass;
	keys[1] = kSecAttrService;

	CFTypeRef values[QUERY_ITEMS];
	values[0] = kSecClassGenericPassword;
	values[1] = CFSTR(KEYCHAIN_SERVICE_NAME);
	CFDictionaryRef query =
		CFDictionaryCreate((CFAllocatorRef)NULL, (const void **)keys, (const void **)values, QUERY_ITEMS,
						   &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	CFDataRef valueData = NULL;

	OSStatus status = SecItemCopyMatching(query, (CFTypeRef *)&valueData);

	CFRelease(query);

	return status == errSecSuccess;
}

#endif

void ScapSettings::setSecretKey(const std::string &secretKey, const std::string &nic) {
	std::string config_name = "scap_apikey_" + removeCheckDigit(nic);

#ifdef __linux__

	QByteArray array(secretKey.data(), secretKey.length());
	PTEID_Config config(config_name.c_str(), L"general", L"");
	config.setString(array.toHex());

#else
	secureSaveSecretKey(config_name, secretKey);
#endif
}

std::string ScapSettings::getSecretKey(const std::string &nic) {
	std::string config_name = "scap_apikey_" + removeCheckDigit(nic);

	PTEID_Config config(config_name.c_str(), L"general", L"");
	std::string temp_secretKey = config.getString();
	assert(temp_secretKey.length() <= INT_MAX);
	QByteArray temp_array(temp_secretKey.data(), (int) temp_secretKey.length());
	QByteArray secretKey_array = QByteArray::fromHex(temp_array);

#ifdef _WIN32
	return secureGetSecretKey(secretKey_array);
#elif __APPLE__
	return secureGetSecretKey(config_name);
#else
	return secretKey_array.constData();
#endif
}

bool ScapSettings::secretKeyExists() {

#ifdef __APPLE__
	return keychain_keys_present();
#else
	bool secretKeyExists = false;
	std::string config_name = "scap_apikey_";
	PTEID_Config config(config_name.c_str(), L"general", L"");

	try {
		secretKeyExists = config.CountKeysByPrefix() > 0;
	} catch (...) {
		MWLOG(LEV_INFO, MOD_SCAP, "CountKeysByPrefix failed");
	}

	return secretKeyExists;
#endif
}

void ScapSettings::resetScapKeys() {
	std::string config_name = "scap_apikey_";
	PTEID_Config config(config_name.c_str(), L"general", L"");

	try {
		config.DeleteKeysByPrefix();
	} catch (...) {
		MWLOG(LEV_INFO, MOD_SCAP, "DeleteKeysByPrefix failed");
	}
#ifdef __APPLE__
	keychain_delete_keys();
#endif
}

void ScapSettings::setAppID(const QString &appID) {
	m_appID = appID;

	PTEID_Config config(PTEID_PARAM_GENERAL_SCAP_APPID);
	config.setString(m_appID.toUtf8());
}

const QString ScapSettings::getAppID() { return m_appID; }

QString ScapSettings::getCacheDir() { return m_cache_dir; }

}; // namespace eIDMW