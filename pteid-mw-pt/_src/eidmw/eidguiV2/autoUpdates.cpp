/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2023 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "autoUpdates.h"

#include <curl/curl.h>

#include <QObject>
#include <QCursor>
#include <QDebug>
#include <QtConcurrent>
#include <QStandardPaths>
#include <QList>
#include <QMap>
#include <QFile>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cstdio>
#include <time.h>

// MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "Util.h"
#include "Settings.h"
#include "Config.h"
#include "MiscUtil.h"

#include "appcontroller.h"
#include "gapi.h"

#ifdef WIN32
#include <windows.h>
#include <SoftPub.h>
#include <wintrust.h>
#include <QSysInfo>
#endif

#include "pteidversions.h"

#ifndef WIN32
#include <unistd.h>
#endif

// Authorization functions for Certificates update
#ifdef __APPLE__
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#endif

#include "proxyinfo.h"

#define N_RELEASE_NOTES 3

#define UPDATES_OK 200

#define CALLBACK_OK 0
#define CALLBACK_CANCEL 1

struct PteidVersion {
	int major;
	int minor;
	int release;
};

namespace eIDMW {

enum class UpdateStatus { ok, generic_error, proxy_auth_req, possible_bad_proxy, cancel };

using cJSON_ptr = std::unique_ptr<cJSON, decltype(&::cJSON_Delete)>;

static size_t curl_write_data(char *data, size_t size, size_t nmemb, void *read_string) {
	((std::string *)read_string)->append((char *)data, size * nmemb);
	return size * nmemb;
}

struct report_progress {
	AppController *controller; // report progress to this AppController
	GAPI::AutoUpdateType type; // about this update type
	bool *cancel;
};

static size_t progress_callback(void *data, curl_off_t total, curl_off_t current, curl_off_t, curl_off_t) {
	report_progress *prog = (report_progress *)data;
	if (*prog->cancel) {
		return CALLBACK_CANCEL; // return anything other than 0 to signal cancellation.
	}

	if (total) { // libcurl docs warns about possible unkown(0) total at first
		AppController *controller = (AppController *)prog->controller;

		double progress = ((double)current / total);
		controller->reportProgress(prog->type, progress);
	}

	return CALLBACK_OK;
}

static UpdateStatus perform_update_request(const std::string &url, std::string &out, CURL *curl = NULL, FILE *fp = NULL,
										   report_progress *prog = NULL) {

	char error_buffer[CURL_ERROR_SIZE] = {0};

	std::string cacerts_location =
		utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";

	curl_global_init(CURL_GLOBAL_DEFAULT);
	if (!curl && (curl = curl_easy_init()) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "autoupdate", "Failed to initialize curl");
		return UpdateStatus::generic_error;
	}

	curl_easy_setopt(curl, CURLOPT_USERAGENT, PTEID_USER_AGENT_VALUE);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
	if (fp) {
		// Writing directly to a file
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, prog);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	} else {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
	}

	curl_easy_setopt(curl, CURLOPT_CAINFO, cacerts_location.c_str());
	bool using_proxy = false; // applyProxyConfigToCurl(curl, url.c_str());

	UpdateStatus status;
	CURLcode ret = curl_easy_perform(curl);
	if (ret == CURLE_ABORTED_BY_CALLBACK) {
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "autoupdate", "Update request cancelled by user.");
		*prog->cancel = false;
		status = UpdateStatus::cancel;
	} else if (ret != CURLE_OK) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "autoupdate", "Error on request %s. Libcurl returned %s", url.c_str(),
				  error_buffer);

		long auth = 0;
		if (!curl_easy_getinfo(curl, CURLINFO_PROXYAUTH_AVAIL, &auth) && auth) {
			status = UpdateStatus::proxy_auth_req;
		} else if (using_proxy) {
			status = UpdateStatus::possible_bad_proxy;
		} else {
			status = UpdateStatus::generic_error;
		}
	} else {
		unsigned int response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		status = response_code == UPDATES_OK ? UpdateStatus::ok : UpdateStatus::generic_error;
	}

	if (fp) {
		//Download is only finished when the file is fully written
		fflush(fp);
	}

	// should i cleanup?
	// curl_easy_cleanup(curl);

	return status;
}

void AutoUpdates::checkForUpdate(GAPI::AutoUpdateType update_type) {
	std::string update_verify_url;
	if (update_type == GAPI::AutoUpdateApp) {
		PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui", "AutoUpdates::started");

		PTEID_Config config(PTEID_PARAM_AUTOUPDATES_VERIFY_URL);
		update_verify_url.append(config.getString());
		update_verify_url.append("version.json");
	} else if (update_type == GAPI::AutoUpdateCerts) {
		PTEID_Config config(PTEID_PARAM_AUTOUPDATES_CERTS_URL);
		update_verify_url.append(config.getString());
		update_verify_url.append("certs.json");
	} else {
		PTEID_Config config(PTEID_PARAM_AUTOUPDATES_NEWS_URL);
		update_verify_url.append(config.getString());
		update_verify_url.append("news.json");
	}

	std::string json_data;
	UpdateStatus status = perform_update_request(update_verify_url, json_data);
	// TODO: treat and report other error cases
	if (status != UpdateStatus::ok) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "checkForUpdate() perform_update_request failed reason: %s",
				  status == UpdateStatus::possible_bad_proxy ? "Proxy didn't work"
				  : status == UpdateStatus::proxy_auth_req	 ? "Proxy needs to authenticate"
				  : status == UpdateStatus::cancel			 ? "Update canceled by the user"
															 : "Generic error");

		if (update_type == GAPI::AutoUpdateApp || update_type == GAPI::AutoUpdateCerts) {
			m_app_controller->signalAutoUpdateFail(update_type,
												   status == UpdateStatus::generic_error ||
														   status == UpdateStatus::possible_bad_proxy ||
														   status == UpdateStatus::proxy_auth_req
													   ? GAPI::NetworkError
													   : GAPI::DownloadCancelled);
		}
		return;
	}

	if (update_type == GAPI::AutoUpdateApp) {
		verifyAppUpdates(json_data);
	} else if (update_type == GAPI::AutoUpdateCerts) {
		verifyCertsUpdates(json_data);
	} else {
		verifyNewsUpdates(json_data);
	}
}

void AutoUpdates::verifyCertsUpdates(const std::string &filedata) {
	qDebug() << "C++ AUTO UPDATES: verifyCertsUpdates";

	// certs.json parsing
	cJSON_ptr json(cJSON_Parse(filedata.c_str()), ::cJSON_Delete);
	if (!cJSON_IsObject(json.get())) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: may be a syntax error.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
		return;
	}

	cJSON *certs_json = cJSON_GetObjectItem(json.get(), "certs");
	if (!cJSON_IsObject(certs_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: Could not get certs object.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
		return;
	}

	chooseCertificates(certs_json);
}

void AutoUpdates::verifyNewsUpdates(const std::string &filedata) {
	qDebug() << "C++ AUTO UPDATES: verifyNewsUpdates";

	parseNews(filedata);
	m_news = chooseNews(); // Filter active news
	QVariantMap news_list;

	// should be only one
	if (m_news.size() > 0) {
		NewsEntry selectedEntry = m_news.at(0);
		// if there are more than one active news entry select highest id
		for (size_t i = 0; i < m_news.size(); i++) {
			NewsEntry entry = m_news.at(i);
			QVariantMap news;
			news.insert("id", QString::number(entry.id));
			news.insert("title", QString::fromStdString(entry.title));
			news.insert("text", QString::fromStdString(entry.text));
			news.insert("link", QString::fromStdString(entry.link));
			news.insert("read", !m_app_controller->isToShowNews(QString::number(entry.id)));
			news_list.insert(QString::number(entry.id), news);
		}
		m_app_controller->signalAutoUpdateNews(news_list);
		return;
	}
	m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateNews, GAPI::NoUpdatesAvailable);
}

time_t getTimeFromString(const std::string &stringTime) {
	// expects time string to be of format: YYYY-MM-DD
	std::stringstream stream(stringTime);
	std::string segment;
	std::vector<std::string> splitTime;

	int year, month, day;
	while (std::getline(stream, segment, '-')) {
		splitTime.push_back(segment);
	}

	if (splitTime.size() != 3) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Error parsing news.json: bad date format (expected YYYY-MM-DD) - maybe bad separator");
		return 0;
	}
	try {
		year = std::stoi(splitTime.at(0));
		month = std::stoi(splitTime.at(1));
		day = std::stoi(splitTime.at(2));
	} catch (const std::invalid_argument &) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Error parsing news.json: bad date format (expected YYYY-MM-DD) - bad year, month or day");
		return 0;
	}

	struct tm tm;
	time_t rawtime;
	time(&rawtime);
	tm = *localtime(&rawtime);
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	return mktime(&tm);
}

std::vector<NewsEntry> AutoUpdates::chooseNews() {
	qDebug() << "C++ AUTO UPDATES: chooseNews";

	std::vector<NewsEntry> filteredNews;
	NewsEntry entry;
	std::string firstString;
	std::string lastString;
	time_t firstTime;
	time_t lastTime;
	time_t currentTime = time(0);

	// Filter active news
	for (size_t i = 0; i < m_news.size(); i++) {
		entry = m_news.at(i);
		firstString = entry.first_day;
		lastString = entry.last_day;

		firstTime = getTimeFromString(firstString);
		lastTime = getTimeFromString(lastString);

		if (firstTime && lastTime && difftime(currentTime, firstTime) >= 0 && difftime(lastTime, currentTime) >= 0) {
			filteredNews.push_back(entry);
		}
	}

	return filteredNews;
}

void AutoUpdates::parseNews(const std::string &data) {
	// parses news.json file into vector of NewsEntry in m_news
	cJSON_ptr json(cJSON_Parse(data.c_str()), ::cJSON_Delete);
	if (!cJSON_IsObject(json.get())) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: may be a syntax error.");
		return;
	}

	cJSON *news_json = cJSON_GetObjectItem(json.get(), "news");
	if (!cJSON_IsArray(news_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: Could not get news array.");
		return;
	}

	for (int i = 0; i < cJSON_GetArraySize(news_json); i++) {
		NewsEntry newsEntry;

		cJSON *new_json = cJSON_GetArrayItem(news_json, i);
		if (!cJSON_IsObject(new_json)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: on news entry number %d", i);
			return;
		}

		// get id
		cJSON *new_jsonID = cJSON_GetObjectItem(new_json, "id");
		if (!cJSON_IsString(new_jsonID)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get id on news entry number %d", i);
			return;
		}

		// get title
		cJSON *new_jsonTitle = cJSON_GetObjectItem(new_json, "title");
		if (!cJSON_IsString(new_jsonTitle)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get title on news entry number %d", i);
			return;
		}

		// get begin date as string
		cJSON *new_jsonBegin = cJSON_GetObjectItem(new_json, "first_day");
		if (!cJSON_IsString(new_jsonBegin)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get begin date on news entry number %d", i);
			return;
		}

		// get end date as string
		cJSON *new_jsonEnd = cJSON_GetObjectItem(new_json, "last_day");
		if (!cJSON_IsString(new_jsonEnd)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get end date on news entry number %d", i);
			return;
		}

		// get text as string
		cJSON *new_jsonText = cJSON_GetObjectItem(new_json, "text");
		if (!cJSON_IsString(new_jsonText)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get text on news entry number %d", i);
			return;
		}

		// get link as string. Link is optional
		cJSON *new_jsonLink = cJSON_GetObjectItem(new_json, "link");
		if (new_jsonLink != NULL && !cJSON_IsString(new_jsonLink)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: Could not get link on news entry number %d", i);
			return;
		}

		try {
			newsEntry.id = std::stoi(new_jsonID->valuestring);
		} catch (const std::invalid_argument &) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing news.json: bad id. (expected 'id':'n', n being an integer)");
			return;
		}
		newsEntry.title = std::string(new_jsonTitle->valuestring);
		newsEntry.first_day = std::string(new_jsonBegin->valuestring);
		newsEntry.last_day = std::string(new_jsonEnd->valuestring);
		newsEntry.text = std::string(new_jsonText->valuestring);
		if (new_jsonLink)
			newsEntry.link = std::string(new_jsonLink->valuestring);
		else
			newsEntry.link = "";

		m_news.push_back(newsEntry);
	}
}

int compareVersions(PteidVersion v1, PteidVersion v2) {

	unsigned int ret = 0;
	ret = v2.major - v1.major;
	if (ret != 0)
		return ret;
	ret = v2.minor - v1.minor;
	if (ret != 0)
		return ret;
	ret = v2.release - v1.release;
	return ret;
}

void AutoUpdates::verifyAppUpdates(const std::string &filedata) {
	qDebug() << "C++ AUTO UPDATES: verifyAppUpdates";

	std::string distrover = verifyOS("distro");

	if (distrover == "unsupported") {
		qDebug() << "C++ AUTO UPDATES: Your OS is not supported by Auto-updates";
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::LinuxNotSupported);
		return;
	}

	std::string archver = verifyOS("arch");

	// installed version
	QString ver(WIN_GUI_VERSION_STRING);
	QStringList list1 = ver.split(",");
	installed_version = list1.at(0) + '.' + list1.at(1) + '.' + list1.at(2);

	// version.json parsing
	cJSON *json = cJSON_Parse(filedata.c_str());
	if (!cJSON_IsObject(json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: may be a syntax error.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}

	cJSON *dists_json = cJSON_GetObjectItem(json, "distributions");
	if (!cJSON_IsObject(dists_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get distributions object.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}

	cJSON *dist_json = cJSON_GetObjectItem(dists_json, distrover.c_str());
	if (!cJSON_IsObject(dist_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Error parsing version.json: Could not get object for this distribution (%s)", distrover.c_str());
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::LinuxNotSupported);
		return;
	}

	cJSON *latestVersion_json = cJSON_GetObjectItem(dist_json, "latestVersion");
	if (!cJSON_IsString(latestVersion_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Error parsing version.json: Could not get latestVersion for this distribution.", distrover.c_str());
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}
	remote_version = latestVersion_json->valuestring;

	QStringList list2 = remote_version.split(".");
	if (list2.size() < 3) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "AutoUpdates::verifyAppUpdates: Wrong data returned from server or Proxy HTML error!");
		qDebug() << "C++ AUTO UPDATES VerifyUpdates: Wrong data returned from server or Proxy HTML error!";
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}

	// Parse local version into PteidVersion
	PteidVersion local_version;
	local_version.major = list1.at(0).toInt();
	local_version.minor = list1.at(1).toInt();
	local_version.release = list1.at(2).toInt();

	// Parse remote version into PteidVersion
	PteidVersion remote_version;
	remote_version.major = list2.at(0).toInt();
	remote_version.minor = list2.at(1).toInt();
	remote_version.release = list2.at(2).toInt();

	qDebug() << "local_version:" << QString::number(local_version.major) << QString::number(local_version.minor)
			 << QString::number(local_version.release);
	qDebug() << "remote_version:" << QString::number(remote_version.major) << QString::number(remote_version.minor)
			 << QString::number(remote_version.release);

	if (compareVersions(local_version, remote_version) <= 0) {
		qDebug() << "C++ AUTO UPDATES: No App updates available at the moment";
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::NoUpdatesAvailable);
		return;
	}
	qDebug() << "C++ AUTO UPDATES: app updates available";

	cJSON *versions_array_json = cJSON_GetObjectItem(json, "versions");
	if (!cJSON_IsArray(versions_array_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get array of versions.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}
	int latestVerIdx;
	for (latestVerIdx = 0; latestVerIdx < cJSON_GetArraySize(versions_array_json); latestVerIdx++) {
		cJSON *version_json = cJSON_GetArrayItem(versions_array_json, latestVerIdx);
		if (!cJSON_IsObject(version_json)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing version.json: Could not get version object at index %d", latestVerIdx);
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
			return;
		}
		cJSON *version_number_json = cJSON_GetObjectItem(version_json, "version");
		if (!cJSON_IsString(version_number_json)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing version.json: Could not get version number from version at %d", latestVerIdx);
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
			return;
		}
		if (strcmp(version_number_json->valuestring, latestVersion_json->valuestring) == 0)
			break;
	}

	// build release notes string from the previous N_RELEASE_NOTES release notes
	release_notes = "<h2> Notas de Lançamento</h2>"; // init and clean previous content in case it failed
	for (int i = latestVerIdx; i < (std::min)(cJSON_GetArraySize(versions_array_json), latestVerIdx + N_RELEASE_NOTES);
		 i++) {
		cJSON *version_json = cJSON_GetArrayItem(versions_array_json, i);
		cJSON *version_number_json = cJSON_GetObjectItem(version_json, "version");
		cJSON *version_date_json = cJSON_GetObjectItem(version_json, "date");
		cJSON *version_release_notes_json = cJSON_GetObjectItem(version_json, "release_notes");
		if (!cJSON_IsObject(version_json) || !cJSON_IsString(version_number_json) ||
			!cJSON_IsString(version_date_json) || !cJSON_IsArray(version_release_notes_json)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Error parsing version.json: Could not parse some key/value of version object at %d", i);
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
			return;
		}
		release_notes += "<p>";
		release_notes += QString("<h3>Versão estável ") + version_number_json->valuestring + QString(":</h3>");
		release_notes += QString("- Data de lançamento: ") + version_date_json->valuestring + QString("<br/>");
		for (int rlsNoteIdx = 0; rlsNoteIdx < cJSON_GetArraySize(version_release_notes_json); rlsNoteIdx++) {
			cJSON *release_note_json = cJSON_GetArrayItem(version_release_notes_json, rlsNoteIdx);
			if (!cJSON_IsString(release_note_json)) {
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
						  "Error parsing version.json: Could not parse release note %d of version obj %d", rlsNoteIdx,
						  i);
				m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
				return;
			}
			release_notes += QString("- ") + release_note_json->valuestring + QString("<br/>");
		}
		release_notes += "</p>";
	}

	qDebug() << release_notes;

	buildPackageURL(dist_json);
}

std::string AutoUpdates::verifyOS(const std::string &param) {
	std::string distrostr;
	std::string archstr;

	if (QSysInfo::WordSize == 64)
		archstr = "x86_64";
	else
		archstr = "i386";

	if (param == "arch")
		return archstr;

#ifdef WIN32
	distrostr = "windows";
	distrostr += QSysInfo::WordSize == 64 ? "64" : "32";
#elif __APPLE__
	distrostr = "osx";
#elif __linux__
	distrostr = "kde";
	distrostr += QSysInfo::WordSize == 64 ? "64" : "32";
#else
	distrostr = "unsupported"
#endif
	return distrostr;
}

void AutoUpdates::buildPackageURL(cJSON *dist_json) {
	qDebug() << "C++ AUTO UPDATES: chooseAppVersion";
	
	std::string downloadurl;

	// Name of the msi/deb/rpm will be distro specific
	cJSON *package_json = cJSON_GetObjectItem(dist_json, "packageUrl");
	if (!cJSON_IsString(package_json)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "Error parsing version.json: Could not get package_json for this distribution.");
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}
	downloadurl.append(package_json->valuestring);
	urlList.clear();
	urlList.append(QString::fromStdString(downloadurl));
	reportAvailableUpdate(GAPI::AutoUpdateApp);
	return;
}

void AutoUpdates::chooseCertificates(cJSON *certs_json) {
	qDebug() << "C++ AUTO UPDATES: chooseCertificates";

	urlList.clear();
	hashList.clear();
	fileNames.clear();
	std::string downloadurl;

	PTEID_Config configCert(PTEID_PARAM_AUTOUPDATES_CERTS_URL);
	std::string configurl = configCert.getString();

	int certIdx;
	QString cert;

	PTEID_Config certs_dir(PTEID_PARAM_GENERAL_CERTS_DIR);
	std::string certs_dir_str = certs_dir.getString();
	qDebug() << QString::fromUtf8(certs_dir_str.c_str());

	for (certIdx = 0; certIdx < cJSON_GetArraySize(certs_json); certIdx++) {
		cJSON *cert_json = cJSON_GetArrayItem(certs_json, certIdx);
		if (!cJSON_IsString(cert_json)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: Could not get cert index = %d",
					  certIdx);
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
			return;
		}

		std::string file_name_temp;
		file_name_temp.append(certs_dir_str);
		file_name_temp.append("/");
		file_name_temp.append(cert_json->string);


		QDir dir(QString::fromStdString(certs_dir_str));

		if (!dir.exists()) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::chooseCertificates: Certs dir does not exist! %s",
					  certs_dir.getString());
			qDebug() << "C++: AutoUpdates::chooseCertificates: Certs dir do not exist!";
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::InstallFailed);
			return;
		}
		if (!dir.isReadable()) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::chooseCertificates: Certs dir is not readable! %s",
					  certs_dir.getString());
			qDebug() << "C++: AutoUpdates::chooseCertificates: Certs dir is not readable!";
			m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::InstallFailed);
			return;
		}

		if (QFile::exists(QString::fromStdString(file_name_temp)) &&
			validateHash(QString::fromStdString(file_name_temp), QString::fromStdString(cert_json->valuestring))) {
			qDebug() << "Cert exists: " << QString::fromUtf8(file_name_temp.c_str());
		} else {
			PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
					  "AutoUpdates::chooseCertificates: Cert does not exist or invalid:! %s", file_name_temp.c_str());

			downloadurl.append(configurl);
			downloadurl.append(cert_json->string);

			qDebug() << "downloadurl : " << QString::fromUtf8(downloadurl.c_str());
			urlList.append(QString::fromStdString(downloadurl));
			hashList.append(QString::fromStdString(cert_json->valuestring));
			downloadurl.clear();
		}
	}
	if (urlList.length() > 0 && hashList.length() > 0) {
		reportAvailableUpdate(GAPI::AutoUpdateCerts);
	} else {
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NoUpdatesAvailable);
	}
}

void AutoUpdates::reportAvailableUpdate(GAPI::AutoUpdateType update_type) {
	PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
			  "AutoUpdates::reportAvailableUpdate: There are updates available! updateType = %d", update_type);

	if (update_type == GAPI::AutoUpdateApp) {
		// Show popup about app update
		m_app_controller->signalAutoUpdateAvailable(GAPI::AutoUpdateApp, release_notes, installed_version,
													remote_version, urlList.at(0));
	} else if (update_type == GAPI::AutoUpdateCerts) {
#ifdef WIN32
		// Start update certs automatically
		m_app_controller->startUpdateCerts();
#else
		// Show popup about certs update because the root password is needed.
		m_app_controller->signalAutoUpdateAvailable(GAPI::AutoUpdateCerts, "", "", "", getCertListAsString());
#endif
	}
}

void AutoUpdates::startUpdate(GAPI::AutoUpdateType update_type) {
	CURL *curl = NULL;
	QStringList downloaded_files;
	foreach (QUrl url, urlList) {
		QFileInfo fileInfo(url.path());
		QString file_name = fileInfo.fileName();

		QFile temp_file(QDir::tempPath() + "/" + file_name);

		if (!temp_file.open(QIODevice::WriteOnly)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "AutoUpdates::startUpdate: Unable to open temp file. Error code: %d", temp_file.error());
			m_app_controller->signalAutoUpdateFail(update_type, GAPI::UnableSaveFile);
			return;
		}
		int fd = temp_file.handle();

		FILE *fp_tmp_file = fdopen(fd, "wb");

		m_app_controller->signalStartUpdate(update_type, file_name);

		report_progress prog = {m_app_controller, update_type, &userCanceled};

		std::string empty; // TODO: stop doing this please; dont pass an empty string
		UpdateStatus status = perform_update_request(url.toString().toStdString(), empty, curl, fp_tmp_file, &prog);
		// TODO: report error cases
		if (status != UpdateStatus::ok) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "startUpdate() perform_update_request failed");
			temp_file.remove();
			if (status == UpdateStatus::cancel) {
				m_app_controller->signalAutoUpdateFail(update_type, GAPI::DownloadCancelled);
				return;
			}
		} else {
			downloaded_files.push_back(file_name);
		}
	}

	if (update_type == GAPI::AutoUpdateApp) {
		std::string downloaded_pkg = downloaded_files.at(0).toStdString();
		installAppUpdate(downloaded_pkg);
	}
	if (update_type == GAPI::AutoUpdateCerts) {
		installCertsUpdate(downloaded_files);
	} else {
		qDebug() << __FUNCTION__ << " shouldn't be called for AutoUpdateNews or other update_type!";
		return;
	}

	return;
}

/* Actually apply the app update package in a system-specific way
   Windows: install using msixec
   MacOS:   install .pkg file using the GUI Installer app
   Linux:   just move the package to the Downloads dir (the user will be informed that he needs to install the new
   package)

 */
void AutoUpdates::installAppUpdate(const std::string &pkg) {
	qDebug() << "C++ AUTO UPDATES: installAppUpdate";

	std::string pkgpath;

	pkgpath.append(QDir::tempPath().toStdString());
	pkgpath.append("/");
	pkgpath.append(pkg);

	qDebug() << QString::fromStdString("pkgpath " + pkgpath);

	if (!verifyPackageSignature(pkgpath)) {
		qDebug() << "Package signature invalid!";
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}

#ifdef WIN32
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::wstring msi_path = QDir::toNativeSeparators(QString::fromStdString(pkgpath)).toStdWString();

	qDebug() << QString::fromStdString("msi package path");
	qDebug() << QString::fromStdWString(msi_path);

	std::wstring log_file =
		QDir::toNativeSeparators(QString::fromStdString(QDir::tempPath().toStdString() + "\\Pteid-MSI.log"))
			.toStdWString();

	qDebug() << QString::fromStdString("log file path");
	qDebug() << QString::fromStdWString(log_file);

	// Prepare CreateProcess args
	std::wstring installer_app = L"C:\\Windows\\system32\\msiexec.exe";
	std::wstring logging_level = L" /L*v ";
	std::wstring msi_args = L" /i ";

	std::wstring path = installer_app;
	path.append(msi_args);

	// add double quotes for paths
	path.append(L"\"");
	path.append(msi_path);
	path.append(L"\"");

	path.append(logging_level);

	// add double quotes for paths
	path.append(L"\"");
	path.append(log_file);
	path.append(L"\"");

	// guarantees null-terminating string
	path.push_back(0);

	LPWSTR path_pointer = (wchar_t *)path.c_str();

	if (!CreateProcess(NULL, path_pointer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		qDebug() << "autoUpdate process failed to start";
		qDebug() << QString::fromStdString("Error: " + GetLastError());
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::RunPackage: Failed to start update process: %d.",
				  GetLastError());
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::InstallFailed);
	} else {
		PTEID_ReleaseSDK();
		exit(0);
	}

#elif __APPLE__
	// This launches the GUI installation process, the user has to follow the wizard to actually perform the
	// installation
	execl("/usr/bin/open", "open", pkgpath.c_str(), NULL);
#else

	QString pkgDestinationPath = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();
	pkgDestinationPath.append("/");
	pkgDestinationPath.append(pkg.c_str());

	if (QFile::exists(pkgDestinationPath))
		QFile::remove(pkgDestinationPath);

	if (!QFile::rename(QString::fromStdString(pkgpath), pkgDestinationPath)) {
		qDebug() << "C++ AUTO UPDATES: Failed to move package to " << pkgDestinationPath.toStdString().c_str();
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::RunPackage: Failed to move package to %s",
				  pkgDestinationPath.toStdString().c_str());
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateApp, GAPI::GenericError);
		return;
	}

	m_app_controller->signalAutoUpdateSuccess(GAPI::AutoUpdateApp);

#endif
	qDebug() << "C++ AUTO UPDATES: RunPackage finish";
}

#ifdef __APPLE__

/* Helper function for the helper function :) */
QByteArray readOriginalFile(const QString &path) {
	QFile origin_file(path);

	if (!origin_file.open(QIODevice::ReadOnly)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "readOriginalFile() Failed to open file %s!", path.toUtf8().data());
		return QByteArray();
	}

	return origin_file.readAll();
}

/* Helper function to elevate privileges in MacOS and copy a file to a root owned directory */

bool copyFileWithAuthorization(QString &cert_filepath, QString &destination_path) {
	AuthorizationRef authorization_ref = NULL;

	// Create AuthorizationItem for the dialog

	OSStatus status =
		AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorization_ref);

	if (status != errAuthorizationSuccess) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "copyFileWithAuthorization: AuthorizationCreate() failed with error code: %d", status);
		return false;
	}

	AuthorizationExternalForm form;
	status = AuthorizationMakeExternalForm(authorization_ref, &form);

	if (status != errAuthorizationSuccess)
		return false;

	QFileInfo fi(cert_filepath);
	QString destination_full = destination_path + fi.fileName();

	char command_buf[1024];

	// Create the file if needed and open for writing
	snprintf(command_buf, sizeof(command_buf), "/usr/libexec/authopen -extauth -c -w %s",
			 destination_full.toUtf8().data());

	FILE *destination_pipe = popen(command_buf, "w");

	if (destination_pipe == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "copyFileWithAuthorization: Failed to open pipe to 'authopen' (%d - %s)", errno, strerror(errno));
	} else {

		fwrite(form.bytes, kAuthorizationExternalFormLength, 1, destination_pipe);
		fflush(destination_pipe);

		QByteArray file_content = readOriginalFile(cert_filepath);

		if (file_content.size() == 0) {
			return false;
		}

		fwrite(file_content.data(), file_content.size(), 1, destination_pipe);
		fflush(destination_pipe);

		// Close the pipe and wait for the authopen process to finish
		int rc = pclose(destination_pipe);

		if (rc != 0) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "copyFileWithAuthorization: authopen process exited with error code %d", rc);
		}

		return rc == 0;
	}
}

#endif

void AutoUpdates::installCertsUpdate(const QStringList &certs) {
	qDebug() << "C++ AUTO UPDATES: installCertsUpdate filename";

	PTEID_Config config(PTEID_PARAM_GENERAL_CERTS_DIR);
	std::string certs_dir_std = config.getString();

	QString certs_dir_str = QString::fromStdString(certs_dir_std);

	bool bUpdateCertsSuccess = false;

#ifdef WIN32
	certs_dir_str.append("\\");

	QString copySourceDir = QDir::tempPath() + "/";
	QString copyTargetDir = certs_dir_str;
	QString certificateFileName;
	if (certs.length() > 0) {
		bUpdateCertsSuccess = true;
		for (int i = 0; i < certs.length(); i++) {
			certificateFileName = certs.at(i);
			if (validateHash(copySourceDir + certificateFileName, hashList.at(i)) == false) {
				bUpdateCertsSuccess = false; // Keep install another files but show popup with error message
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
						  "AutoUpdates::installCertsUpdate: failed to validate hash for file: %s",
						  certificateFileName.toStdString().c_str());
				continue;
			}

			if (QFile::exists(copyTargetDir + certificateFileName))
				QFile::remove(copyTargetDir + certificateFileName);

			if (QFile::copy(copySourceDir + certificateFileName, copyTargetDir + certificateFileName) == false) {
				bUpdateCertsSuccess = false; // Keep install another files but show popup with error message
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::installCertsUpdate: Cannot copy file: %s",
						  certificateFileName.toStdString().c_str());
			} else {
				PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui", "AutoUpdates::installCertsUpdate: Copy file success: %s",
						  certificateFileName.toStdString().c_str());
			}
		}
	}

#elif __APPLE__
	if (certs.length() > 0) {
		bUpdateCertsSuccess = true;
		for (int i = 0; i < certs.length(); i++) {
			QString certificate_path = QDir::tempPath() + "/" + certs.at(i);
			if (validateHash(certificate_path, hashList.at(i))) {
				bUpdateCertsSuccess = copyFileWithAuthorization(certificate_path, certs_dir_str);
				if (!bUpdateCertsSuccess)
					break;
			} else {
				bUpdateCertsSuccess = false;
				// Hash failure just abort because we don't want to support partially correct updates
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
						  "AutoUpdates::installCertsUpdate: invalid hash for downloaded file %s",
						  certificate_path.toStdString().c_str());
				break;
			}
		}
	}

#else
	QStringList filesToCopy;
	bool bHaveFilesToCopy = false;
	if (certs.length() > 0) {
		bUpdateCertsSuccess = true;
		for (int i = 0; i < certs.length(); i++) {
			QString certificateFileName;
			certificateFileName = QDir::tempPath() + "/" + certs.at(i);
			if (validateHash(certificateFileName, hashList.at(i))) {
				bHaveFilesToCopy = true;
				filesToCopy.append(certificateFileName);
			} else {
				bUpdateCertsSuccess = false;
			}
		}
	}

	if (bHaveFilesToCopy) {
		QProcess proc;
		proc.setProcessChannelMode(QProcess::MergedChannels);

		QString program = "pkexec";

		QStringList arguments;

		arguments.append("/bin/cp");

		for (QString file : filesToCopy) {
			arguments.append(file);
		}

		arguments.append(certs_dir_str);

		QString cmd = program;
		for (QString arg : arguments) {
			cmd += " " + arg;
		}

		proc.start(program, arguments);

		if (!proc.waitForStarted()) // default wait time 30 sec
		{
			qDebug() << "C++ AUTO UPDATES: cannot start process ";
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::installCertsUpdate: Cannot execute: %s",
					  cmd.toStdString().c_str());
			bUpdateCertsSuccess = false;
		} else {

			proc.waitForFinished();

			if (proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == QProcess::NormalExit) {
				PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
						  "AutoUpdates::installCertsUpdate: Copy file(s) success: %s", cmd.toStdString().c_str());
			} else {
				bUpdateCertsSuccess = false;
				if (proc.exitStatus() == QProcess::NormalExit) {
					PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Exit code of pkexec command: %d", proc.exitCode());
				}
				PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::installCertsUpdate: Cannot copy file(s): %s",
						  cmd.toStdString().c_str());
			}
		}
	}
#endif

	if (bUpdateCertsSuccess) {
		m_app_controller->signalAutoUpdateSuccess(GAPI::AutoUpdateCerts);
		m_app_controller->updateCertslog();
	} else {
		m_app_controller->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::InstallFailed);
	}
	qDebug() << "C++: installCertsUpdate finish";
}

bool AutoUpdates::validateHash(const QString &certPath, const QString &hashString) {

	QFile f(certPath);
	if (!f.open(QFile::ReadOnly)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::validateHash: Error Reading Certificate file :%s",
				  certPath.toStdString().c_str());
		return false;
	}

	QTextStream in(&f);

	QByteArray DataFile = f.readAll();

	QString hash = QString(QCryptographicHash::hash((DataFile), QCryptographicHash::Sha256).toHex());

	if (QString::compare(hash, hashString, Qt::CaseInsensitive)) {
		PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
				  "AutoUpdates::validateHash Certificate invalid or outdated: %s %s %s ",
				  certPath.toStdString().c_str(), hashString.toStdString().c_str(), hash.toStdString().c_str());
		return false;
	} else {
		return true;
	}
}

void AutoUpdates::cancelUpdate() {
	qDebug() << "C++ AUTO UPDATES: cancelUpdate";
	userCanceled = true;
}

QString AutoUpdates::getCertListAsString() {
	// returns a string with the names of certificates to be downloaded
	QString stringURLs;
	QUrl url_tmp;
	QFileInfo fileInfo_tmp;
	QString fileName_tmp;
	for (int i = 0; i < urlList.length(); i++) {
		url_tmp = urlList.at(i);
		fileInfo_tmp = QFileInfo(url_tmp.path());
		fileName_tmp = fileInfo_tmp.fileName();
		stringURLs.append(fileName_tmp);
		stringURLs.append(" ");
	}
	return stringURLs;
}

bool AutoUpdates::verifyPackageSignature(const std::string &pkg) {
#ifdef WIN32
	bool result = false;
	LONG lStatus;
	DWORD dwLastError;

	std::wstring pkgW = utilStringWiden(pkg.c_str());

	// Initialize the WINTRUST_FILE_INFO structure.
	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = pkgW.c_str();
	FileData.hFile = NULL;
	FileData.pgKnownSubject = NULL;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;

	// Initialize the WinVerifyTrust input data structure.

	// Default all fields to 0.
	memset(&WinTrustData, 0, sizeof(WinTrustData));
	WinTrustData.cbStruct = sizeof(WinTrustData);

	// Use default code signing EKU.
	WinTrustData.pPolicyCallbackData = NULL;

	// No data to pass to SIP.
	WinTrustData.pSIPClientData = NULL;

	// Disable WVT UI.
	WinTrustData.dwUIChoice = WTD_UI_NONE;

	// No revocation checking.
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;

	// Verify an embedded signature on a file.
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

	// Verify action.
	WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;

	// Verification sets this value.
	WinTrustData.hWVTStateData = NULL;

	// Not used.
	WinTrustData.pwszURLReference = NULL;

	// This is not applicable if there is no UI because it changes
	// the UI to accommodate running applications instead of
	// installing applications.
	WinTrustData.dwUIContext = 0;

	// Set pFile.
	WinTrustData.pFile = &FileData;

	// WinVerifyTrust verifies signatures as specified by the GUID
	// and Wintrust_Data.
	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	switch (lStatus) {
	case ERROR_SUCCESS:
		/*
		Signed file:
			- Hash that represents the subject is trusted.

			- Trusted publisher without any verification errors.

			- UI was disabled in dwUIChoice. No publisher or
				time stamp chain errors.

			- UI was enabled in dwUIChoice and the user clicked
				"Yes" when asked to install and run the signed
				subject.
		*/
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "The file \"%S\" is signed and the signature was verified.\n",
				  pkgW.c_str());
		result = true;
		break;

	case TRUST_E_NOSIGNATURE:
		// The file was not signed or had a signature
		// that was not valid.

		// Get the reason for no signature.
		dwLastError = GetLastError();
		if (TRUST_E_NOSIGNATURE == dwLastError || TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
			TRUST_E_PROVIDER_UNKNOWN == dwLastError) {
			// The file was not signed.
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "The file \"%S\" is not signed.\n", pkgW.c_str());
		} else {
			// The signature was not valid or there was an error
			// opening the file.
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "An unknown error occurred trying to verify the signature of the \"%S\" file.\n", pkgW.c_str());
		}

		break;

	case TRUST_E_EXPLICIT_DISTRUST:
		// The hash that represents the subject or the publisher
		// is not allowed by the admin or user.
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "The signature is present, but specifically disallowed.\n");
		break;

	case TRUST_E_SUBJECT_NOT_TRUSTED:
		// The user clicked "No" when asked to install and run.
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "The signature is present, but not trusted.\n");
		break;

	case CRYPT_E_SECURITY_SETTINGS:
		/*
		The hash that represents the subject or the publisher
		was not explicitly trusted by the admin and the
		admin policy has disabled user trust. No signature,
		publisher or time stamp errors.
		*/
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "CRYPT_E_SECURITY_SETTINGS - The hash "
				  "representing the subject or the publisher wasn't "
				  "explicitly trusted by the admin and admin policy "
				  "has disabled user trust. No signature, publisher "
				  "or timestamp errors.\n");
		break;

	default:
		// The UI was disabled in dwUIChoice or the admin policy
		// has disabled user trust. lStatus contains the
		// publisher or time stamp chain error.
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error in verifying file %S is: 0x%x.\n", pkgW.c_str(), lStatus);
		break;
	}

	// Any hWVTStateData must be released by a call with close.
	WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

	lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

	return result;
#else
	Q_UNUSED(pkg);
	return true;
#endif
}

}; // namespace eIDMW
