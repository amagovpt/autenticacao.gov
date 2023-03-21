/*-****************************************************************************

 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <QString>
#include <QDir>
#include <QFile>

#include <unordered_set>

#include <cjson/cJSON.h>

#include "scapcache.h"
#include "scapservice.h"
#include "scapsettings.h"

#include "MiscUtil.h"
#include "Log.h"

#ifdef WIN32
//QtCore global variable to enable and disable file permissions checking on Windows
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

namespace eIDMW {

static QString get_cache_dir()
{
	ScapSettings settings;
	QString cache_dir_string = settings.getCacheDir();

	return cache_dir_string;
}

static QString get_attribute_cache_dir()
{
	return get_cache_dir() +  "/scap_attributes/";
}

static QString get_logo_cache_dir()
{
	return get_cache_dir() +  "/scap_logos/";
}

static std::vector<ScapAttribute> deserialize_attributes(const std::string &response)
{
	std::vector<ScapAttribute> result;

	cJSON *json =  NULL;
	if ((json = cJSON_Parse(response.c_str())) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse response: %s", __FUNCTION__, response.c_str());
		return result;
	}

	cJSON *citizen_info = NULL;
	if ((citizen_info = cJSON_GetObjectItem(json, "citizenInfo")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse citizenInfo object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	char *zscitizen_name = cJSON_GetStringValue(cJSON_GetObjectItem(citizen_info, "name"));
	std::string citizen_name = zscitizen_name ? zscitizen_name : "";

	cJSON *citizen_attributes = NULL;
	if ((citizen_attributes = cJSON_GetObjectItem(json, "citizenAttributes")) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse citizenAttributes object", __FUNCTION__);
		cJSON_Delete(json);
		return result;
	}

	const size_t array_size = cJSON_GetArraySize(citizen_attributes);
	for (size_t i = 0; i < array_size; ++i) {
		cJSON *array_item = NULL;
		if ((array_item = cJSON_GetArrayItem(citizen_attributes, i)) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse citizenAttributes's item with index %d", __FUNCTION__, i);
			cJSON_Delete(json);
			return result;
		}

		cJSON *attributeProviderInfo = NULL;
		if ((attributeProviderInfo = cJSON_GetObjectItem(array_item, "attributeProviderInfo")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse citizenAttributes object", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		char *provider_uri = cJSON_GetStringValue(cJSON_GetObjectItem(attributeProviderInfo, "uriId"));
		char *provider_name = cJSON_GetStringValue(cJSON_GetObjectItem(attributeProviderInfo, "name"));
		char *provider_type = cJSON_GetStringValue(cJSON_GetObjectItem(attributeProviderInfo, "type"));
		char *provider_nipc = cJSON_GetStringValue(cJSON_GetObjectItem(attributeProviderInfo, "nipc"));
		char *provider_logo = cJSON_GetStringValue(cJSON_GetObjectItem(attributeProviderInfo, "logo"));

		cJSON *attributes = NULL;
		if ((attributes = cJSON_GetObjectItem(array_item, "attributes")) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributes array", __FUNCTION__);
			cJSON_Delete(json);
			return result;
		}

		const size_t attributes_array_size = cJSON_GetArraySize(attributes);
		for (size_t j = 0; j < attributes_array_size; ++j) {
			cJSON *attribute_json = NULL;
			if ((attribute_json = cJSON_GetArrayItem(attributes, j)) == NULL) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to parse attributes's item with index %d", __FUNCTION__, j);
				cJSON_Delete(json);
				return result;
			}

			char *id = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_json, "id"));
			char *description = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_json, "description"));
			char *validity = cJSON_GetStringValue(cJSON_GetObjectItem(attribute_json, "validity"));

			ScapAttribute attribute = {
				id ? id : "",
				description ? description : "",
				validity ? validity : "",
				{},
				{
					provider_uri ? provider_uri : "",
					provider_name ? provider_name : "",
					provider_type ? provider_type : "",
					provider_nipc ? provider_nipc : "",
					provider_logo ? provider_logo : "",
				},
				citizen_name
			};

			cJSON *sub_attributes = NULL;
			if ((sub_attributes = cJSON_GetObjectItem(attribute_json, "subAttributes")) == NULL) {
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

				char *sub_id = cJSON_GetStringValue(cJSON_GetObjectItem(sub_attribute_json, "id"));
				char *sub_description = cJSON_GetStringValue(cJSON_GetObjectItem(sub_attribute_json, "description"));
				char *sub_value = cJSON_GetStringValue(cJSON_GetObjectItem(sub_attribute_json, "value"));

				attribute.sub_attributes.push_back({
					sub_id ? sub_id : "",
					sub_description ? sub_description : "",
					sub_value ? sub_value : ""
				});
			}

			result.push_back(attribute);
		}
	}

	cJSON_Delete(json);
	return result;
}

static std::string serialize_attributes(const std::vector<ScapAttribute> &attributes)
{
	std::string result;

	if (attributes.size() == 0) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s no attributes to store in cache", __FUNCTION__);
		return "";
	}

	char *json_string = NULL;
	const char *name = attributes.front().citizen_name.c_str();
	cJSON *json = NULL;
	cJSON *provider_info = NULL;
	cJSON *citizen_info = NULL;
	cJSON *citizen_attributes = NULL;
	cJSON *citizen_attribute_obj = NULL;
	cJSON *attributes_array = NULL;
	cJSON *sub_attributes_array = NULL;
	cJSON *attribute_json = NULL;
	cJSON *sub_attribute_json = NULL;
	const auto attrs_grouped_by_provider = group_by_provider(attributes);

	if ((json = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
		goto clean_up;
	}

	if ((citizen_info = cJSON_CreateObject()) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed create citizen_info", __FUNCTION__);
		goto clean_up;
	}

	if (cJSON_AddStringToObject(citizen_info, "name", name) == NULL) {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add citizen name", __FUNCTION__);
		goto clean_up;
	}

	cJSON_AddItemToObject(json, "citizenInfo", citizen_info);

	citizen_attributes = cJSON_CreateArray();
	for (const auto &provider_attributes_pair: attrs_grouped_by_provider) {
		const ScapProvider& provider = provider_attributes_pair.first;

		if ((citizen_attribute_obj = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		if ((provider_info = cJSON_CreateObject()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s cJSON_CreateObject() failed", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(provider_info, "uriId", provider.uriId.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add uriId", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(provider_info, "name", provider.name.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add name", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(provider_info, "type", provider.type.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add type", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(provider_info, "nipc", provider.nipc.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add nipc", __FUNCTION__);
			goto clean_up;
		}

		if (cJSON_AddStringToObject(provider_info, "logo", provider.logo.c_str()) == NULL) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to add logo", __FUNCTION__);
			goto clean_up;
		}

		if (!provider.logo.empty()) {
			if (!save_scap_image(provider.nipc, provider.logo)) {
				MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to save logo to cache", __FUNCTION__);
			}
		}

		cJSON_AddItemToObject(citizen_attribute_obj, "attributeProviderInfo", provider_info);

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
		cJSON_AddItemToObject(citizen_attribute_obj, "attributes", attributes_array);
		cJSON_AddItemToArray(citizen_attributes, citizen_attribute_obj);
	}
	cJSON_AddItemToObject(json, "citizenAttributes", citizen_attributes);

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

static bool save_cache(const std::string &to_be_saved, const std::string &id)
{
	QString cache_file_string = get_attribute_cache_dir() + id.c_str() + ".json";
	QFile cache_file(cache_file_string);
	if (cache_file.open(QIODevice::WriteOnly)) {
		cache_file.write(to_be_saved.c_str(), to_be_saved.length());
		cache_file.close();
	} else {
		MWLOG(LEV_ERROR, MOD_SCAP, "%s Failed to create scap cache file", __FUNCTION__);
		return false;
	}

	return true;
}

static bool check_dir_readable(const QDir &dir)
{
#ifdef WIN32
	// necessary according to https://doc.qt.io/archives/qt-5.12/qfileinfo.html#ntfs-permissions
	qt_ntfs_permission_lookup++;
#endif
	const bool is_readable = dir.isReadable();

#ifdef WIN32
	qt_ntfs_permission_lookup--;
#endif

	return is_readable;
}

ScapResult<std::vector<ScapAttribute>> load_cache()
{
	std::vector<ScapAttribute> result;
	bool removed_legacy = false;

	QString cache_path = get_attribute_cache_dir();
	QDir cache_dir(cache_path);

	if (QFileInfo::exists(cache_path) && !check_dir_readable(cache_dir)) {
		std::string cache_path_str = cache_path.toStdString();
		MWLOG(LEV_ERROR, MOD_SCAP, "%s cache dir not readable: %s", __FUNCTION__, cache_path_str.c_str());
		return ScapError::cache_read_failure;
	}

	QStringList file_list = cache_dir.entryList(QStringList({"*.json", "*.xml"}), QDir::Files | QDir::NoSymLinks);
	foreach(QString cache_file_name, file_list) {
		QFile cache_file(get_attribute_cache_dir() + cache_file_name);
		if (!cache_file.open(QIODevice::ReadOnly)) {
			MWLOG(LEV_ERROR, MOD_SCAP, "%s failed to open cache file", __FUNCTION__);
			return ScapError::cache_read_failure;
		}

		QFileInfo file_info(cache_file);
		if (file_info.completeSuffix() == "json") {
			std::string cache_file_content = cache_file.readAll().constData();
			std::vector<ScapAttribute> attributes = deserialize_attributes(cache_file_content);
			result.insert(result.end(), attributes.begin(), attributes.end());
		} else {
			removed_legacy = cache_file.remove() || removed_legacy;
		}
	}

	if (removed_legacy) {
		return ScapError::cache_removed_legacy;
	}

	return result;
}

static std::vector<ScapAttribute> merge_attributes(const std::vector<ScapAttribute> &dirty, const std::vector<ScapAttribute> &fresh)
{
	std::unordered_set<ScapAttribute, ScapAttributeHasher> attribute_set;
	attribute_set.insert(dirty.begin(), dirty.end());
	attribute_set.insert(fresh.begin(), fresh.end());

	return std::vector<ScapAttribute>(attribute_set.begin(), attribute_set.end());
}

static std::vector<ScapAttribute> handle_cache(const std::string &response)
{
	std::vector<ScapAttribute> attributes = deserialize_attributes(response);

	// perform cache logic here -> merge attributes etc...
	std::vector<ScapAttribute> dirty_attributes = load_cache().data();
	std::vector<ScapAttribute> result = merge_attributes(dirty_attributes, attributes);

	return result;
}

bool cache_response(const std::string& response, const std::string &id, std::vector<ScapAttribute> &out_attributes)
{
	QDir cache_dir; //create cache dir if it does not exist
	QString cache_path = get_attribute_cache_dir();
	if (!cache_dir.mkpath(cache_path)) {
		std::string cache_path_str = cache_path.toStdString();
		MWLOG(LEV_ERROR, MOD_SCAP, "%s Failed to create scap cache directory, %s", __FUNCTION__,
			cache_path_str.c_str());
		return false;
	}

	std::vector<ScapAttribute> to_be_saved = handle_cache(response);
	out_attributes = to_be_saved;

	return save_cache(serialize_attributes(to_be_saved), id);
}

static bool clear_files_in_dir(QDir &dir, const std::string &file_filter)
{
	dir.setNameFilters(QStringList() << file_filter.c_str());
	dir.setFilter(QDir::Files);

	ScapSettings settings;
	settings.resetScapKeys();

	foreach(QString cache_file, dir.entryList()) {
		if (!dir.remove(cache_file)) {
			return false;
		}
	}

	return true;
}

bool clear_cache()
{
	QDir attributes_dir(get_attribute_cache_dir());
	QDir logos_dir(get_logo_cache_dir());

	return clear_files_in_dir(attributes_dir, "*.json") && clear_files_in_dir(logos_dir, "*.jpeg");
}

};