/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2023 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef AUTOUPDATES_H
#define AUTOUPDATES_H

#include <QVariant>
#include <QUrl>
#include <QString>
#include <QStringList>
#include "Settings.h"

#include <cjson/cJSON.h>

// MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "eidlibdefines.h"

#include "gapi.h"

class AppController;

namespace eIDMW {

struct NewsEntry {
	int id;
	std::string title;
	std::string first_day;
	std::string last_day;
	std::string text;
	std::string link;
};

class AutoUpdates {
public:
	AutoUpdates(AppController *AppController) : m_app_controller(AppController) {};

	void checkForUpdate(GAPI::AutoUpdateType type);
	void startUpdate(GAPI::AutoUpdateType type);
	void cancelUpdate();

private:
	void verifyAppUpdates(const std::string &filedata);
	void verifyCertsUpdates(const std::string &filedata);
	void verifyNewsUpdates(const std::string &filedata);

	void parseNews(const std::string &filedata);
	std::vector<NewsEntry> chooseNews();
	void chooseCertificates(cJSON *certs_json);
	void chooseAppVersion(const std::string &distro, cJSON *dist_json);

	std::string verifyOS(const std::string &param);

	QString getCertListAsString();

	void reportAvailableUpdate(GAPI::AutoUpdateType update_type);
	void startUpdateRequest(QUrl url);
	void installAppUpdate(const std::string &pkg);
	void installCertsUpdate(const QStringList &certs);

	bool validateHash(const QString &certPath, const QString &hash);
	bool verifyPackageSignature(const std::string &pkg);

	AppController *m_app_controller;

	bool userCanceled = false;
	QStringList urlList;
	QStringList hashList;
	QStringList fileNames;
	QString release_notes;
	QString remote_version;
	QString installed_version;
	std::vector<NewsEntry> m_news;
};

}; // namespace eIDMW
#endif // AUTOUPDATES_H
