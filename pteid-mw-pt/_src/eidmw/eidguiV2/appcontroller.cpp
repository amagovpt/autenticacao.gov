/*****************************************************************************

 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2018-2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "appcontroller.h"
#include <QObject>
#include <QCursor>
#include <QDebug>
#include <QAccessible>
#include <QClipboard>
#include <QGuiApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QDirIterator>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <zip.h>

#include "eidlib.h"
#include "Util.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <QSysInfo>
#endif

#include "pteidversions.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include <cjson/cJSON.h>

#include "gapi.h"
#include "autoUpdates.h"
#include "concurrent.h"

#define N_RELEASE_NOTES 3

struct PteidVersion {
	int major;
	int minor;
	int release;
};

AppController::AppController(GUISettings &settings, QObject *parent)
	: QObject(parent), m_Settings(settings), certsUpdate(this), appUpdate(this), newsUpdate(this) {
	QString strVersion(PTEID_PRODUCT_VERSION);
	m_Settings.setGuiVersion(strVersion);
	qDebug() << "C++: AppController started. App version: "
			 << m_Settings.getGuiVersion() + " - " + REVISION_NUM_STRING + " [ " + REVISION_HASH_STRING + " ]";
	;
	qDebug() << "C++: currentCpuArchitecture():" << QSysInfo::currentCpuArchitecture();
	qDebug() << "C++: prettyProductName():" << QSysInfo::prettyProductName();
	QByteArray ba = m_Settings.getGuiVersion().toLatin1();
	const char *c_str2 = ba.data();
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "App started. %s - %s [%s]", c_str2, REVISION_NUM_STRING,
			  REVISION_HASH_STRING);
	PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "CpuArch: %s ProductName: %s\n",
			  QSysInfo::currentCpuArchitecture().toStdString().c_str(),
			  QSysInfo::prettyProductName().toStdString().c_str());
	createCacheDir();
	checkUpdateCertslog();
	checkUpdateNewslog();
}

void AppController::restoreScreen(void) { emit signalRestoreWindows(); }

bool AppController::getTestMode(void) { return m_Settings.getTestMode(); }

QString AppController::getAppVersion(void) { return m_Settings.getGuiVersion() + " - " + REVISION_NUM_STRING; }

QString AppController::getAppRevision(void) { return REVISION_HASH_STRING; }

void AppController::checkUpdateCertslog() {

	QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
	QFile file(updateCertsLog);

	// Check if certs update log exist.
	if (!file.open(QIODevice::ReadOnly)) {
		updateCertslog();
	}
	file.close();
}

void AppController::updateCertslog() {

	QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
	QFile file(updateCertsLog);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		// We're going to streaming text to the file
		char date_string[STR_LOCALTIME_MAX_SIZE];
		QTextStream stream(&file);
		time_t _tm = time(nullptr);
		struct tm *t = localtime(&_tm);
		strftime(date_string, STR_LOCALTIME_MAX_SIZE, "%H:%M ON_DAY %d-%m-%Y", t);
		stream << date_string;
		file.close();
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::RunCertsPackage: Error Writing updateCertsLog.txt :%s",
				  updateCertsLog.toStdString().c_str());
	}
}

QString AppController::getCertsLog(void) {

	QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
	QFile file(updateCertsLog);

	if (!file.open(QIODevice::ReadOnly)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AutoUpdates::getCertsLog: Error Reading updateCertsLog.txt :%s",
				  updateCertsLog.toStdString().c_str());
		return tr("STR_CERTS_UPDATE_LOG_ERROR");
	}

	QTextStream instream(&file);

	QString line = instream.readLine(STR_LOCALTIME_MAX_SIZE);

	line.replace("ON_DAY", tr("STR_ON_DAY"));
	file.close();

	return line;
}

void AppController::checkUpdateNewslog() {

	QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";

	// Create updateNewsLog.txt file if it does not exist.
	if (!QFile::exists(updateNewsLog)) {
		QFile file(updateNewsLog);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "AppController::checkUpdateNewslog: Error Creating updateNewsLog.txt");
			return;
		}
		file.close();
	}
}

void AppController::updateNewsLog(QString id) {

	QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";
	QFile file(updateNewsLog);

	if (!isToShowNews(id)) {
		return;
	}

	if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
		QTextStream stream(&file);
		stream << id << ",";
		file.close();
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::updateNewsLog: Error Writing updateNewsLog.txt :%s",
				  updateNewsLog.toStdString().c_str());
	}
}

QString AppController::getNewsLog(void) {

	QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";
	QFile file(updateNewsLog);

	if (!file.open(QIODevice::ReadOnly)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::getNewsLog: Error Reading updateNewsLog.txt :%s",
				  updateNewsLog.toStdString().c_str());
		return "";
	}

	QTextStream instream(&file);
	QString line = instream.readLine();
	file.close();

	return line;
}

bool AppController::isToShowNews(QString id) { return !getNewsLog().split(",").contains(id); }

QString AppController::getAppCopyright(void) { return PTEID_COPYRIGHT; }

bool AppController::isAnimationsEnabled(void) { return m_Settings.getShowAnimations(); }
bool AppController::getNotShowHelpStartUp(void) { return m_Settings.getNotShowHelpStartUp(); }
void AppController::setNotShowHelpStartUp(bool notshowhelpStartUp) {

	m_Settings.setNotShowHelpStartUp(notshowhelpStartUp);
}
void AppController::initTranslation() {

	QString appPath = QCoreApplication::applicationDirPath();
	m_Settings.setExePath(appPath);
	QString CurrLng = m_Settings.getGuiLanguageString();

	if (LoadTranslationFile(CurrLng) == false) {
		emit signalLanguageChangedError();
	}
}

bool AppController::LoadTranslationFile(QString NewLanguage) {

	QString strTranslationFile;
	QString translations_dir;
	strTranslationFile = QString("eidmw_") + NewLanguage;

#ifdef __APPLE__
	translations_dir = m_Settings.getExePath() + "/../Resources/";
#else
	translations_dir = m_Settings.getExePath() + "/";
#endif

	qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << translations_dir;

	if (!m_translator.load(strTranslationFile, translations_dir)) {
		// this should not happen, since we've built the menu with the translation filenames
		strTranslationFile = QString("eidmw_") + STR_DEF_GUILANGUAGE;
		// try load default translation file
		qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << translations_dir;
		if (!m_translator.load(strTranslationFile, translations_dir)) {
			// this should not happen too, since we've built the menu with the translation filenames
			qDebug() << "C++: AppController Load Default Translation File Error";
			return false;
		}
		qDebug() << "C++: AppController Loaded Default Translation File";
	}
	//------------------------------------
	// install the translator object and load the .qm file for
	// the given language.
	//------------------------------------
	qApp->installTranslator(&m_translator);
	return true;
}
void AppController::autoUpdateApp() {
	qDebug() << "C++: Starting autoUpdateApp";

	appUpdate.checkForUpdate(GAPI::AutoUpdateApp);
}

void AppController::autoUpdatesCerts() {
	qDebug() << "C++: Starting autoUpdates Certs";

	certsUpdate.checkForUpdate(GAPI::AutoUpdateCerts);
}

void AppController::autoUpdatesNews() {
	qDebug() << "C++: Starting autoUpdates News";

	newsUpdate.checkForUpdate(GAPI::AutoUpdateNews);
}

void AppController::reportProgress(GAPI::AutoUpdateType type, double value) {
	emit signalAutoUpdateProgress(type, value);
}

void AppController::startUpdateApp() {
	qDebug() << "C++: startUpdateApp";
	PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui", "AppController::startUpdateApp: App update started!");

	Concurrent::run(&appUpdate, &AutoUpdates::startUpdate, GAPI::AutoUpdateApp);
}

void AppController::startUpdateCerts() {
	qDebug() << "C++: startUpdateCerts";

	Concurrent::run(&certsUpdate, &AutoUpdates::startUpdate, GAPI::AutoUpdateCerts);
}

void AppController::userCancelledUpdateCertsDownload() {
	qDebug() << "C++: userCancelledUpdateCertsDownload";
	certsUpdate.cancelUpdate();
}

void AppController::userCancelledUpdateAppDownload() {
	qDebug() << "C++: userCancelledUpdateAppDownload";
	appUpdate.cancelUpdate();
}

QVariant AppController::getCursorPos() { return QVariant(QCursor::pos()); }
bool AppController::getAutoCardReadingValue(void) { return m_Settings.getAutoCardReading(); }
void AppController::setAutoCardReadingValue(bool bAutoCardReading) { m_Settings.setAutoCardReading(bAutoCardReading); }
bool AppController::getPinpadEnabledValue(void) { return m_Settings.getPinpadEnabled(); }
void AppController::setPinpadEnabledValue(bool bPinpadEnabled) { m_Settings.setPinpadEnabled(bPinpadEnabled); }
bool AppController::getStartAutoValue(void) { return m_Settings.getAutoStartup(); }
void AppController::setStartAutoValue(bool bAutoStartup) { m_Settings.setAutoStartup(bAutoStartup); }
bool AppController::getStartAutoupdateValue(void) { return m_Settings.getStartAutoupdate(); }
void AppController::setStartAutoupdateValue(bool bStartAutoupdate) { m_Settings.setStartAutoupdate(bStartAutoupdate); }
QString AppController::getGuiLanguageString(void) { return m_Settings.getGuiLanguageString(); }
void AppController::setGuiLanguageString(QString language) {

	if (LoadTranslationFile(language)) {
		m_Settings.setLanguage(language);
		emit languageChanged();
	} else {
		emit signalLanguageChangedError();
	}
}
bool AppController::getDebugModeValue(void) { return m_Settings.getDebugMode(); }
QString AppController::setDebugModeValue(bool bDebugMode) { return m_Settings.setDebugMode(bDebugMode); }
bool AppController::getShowAnimationsValue(void) { return m_Settings.getShowAnimations(); }
bool AppController::getUseSystemScaleValue(void) { return m_Settings.getUseSystemScale(); }
int AppController::getApplicationScaleValue(void) { return m_Settings.getApplicationScale(); }
int AppController::getGraphicsAccelValue(void) { return m_Settings.getGraphicsAccel(); }
void AppController::setShowAnimationsValue(bool bShowAnimations) { m_Settings.setShowAnimations(bShowAnimations); }
void AppController::setUseSystemScaleValue(bool bUseSystemScale) { m_Settings.setUseSystemScale(bUseSystemScale); }
void AppController::setApplicationScaleValue(int iScale) { m_Settings.setApplicationScale(iScale); }
void AppController::setGraphicsAccelValue(int iGraphicsAccel) { m_Settings.setAccelGraphics(iGraphicsAccel); }
QString AppController::getTimeStampHostValue(void) { return m_Settings.getTimeStampHost(); }
void AppController::setTimeStampHostValue(const QString &timeStamp_host) {
	m_Settings.setTimeStampHost(timeStamp_host);
}
void AppController::setSCAPOptions(int bSCAPOptions) { m_Settings.setSCAPOptions(bSCAPOptions); }
int AppController::getSCAPOptions(void) { return m_Settings.getSCAPOptions(); }

bool AppController::getProxySystemValue(void) { return m_Settings.getProxySystem(); }
void AppController::setProxySystemValue(bool bProxySystem) { m_Settings.setProxySystem(bProxySystem); }
QString AppController::getProxyHostValue(void) { return m_Settings.getProxyHost(); }
void AppController::setProxyHostValue(const QString &proxy_host) { m_Settings.setProxyHost(proxy_host); }
long AppController::getProxyPortValue(void) { return m_Settings.getProxyPort(); }
void AppController::setProxyPortValue(int proxy_port) { m_Settings.setProxyPort(proxy_port); }
QString AppController::getProxyUsernameValue(void) { return m_Settings.getProxyUsername(); }
void AppController::setProxyUsernameValue(const QString &proxy_user) { m_Settings.setProxyUsername(proxy_user); }
QString AppController::getProxyPwdValue(void) { return m_Settings.getProxyPwd(); }
void AppController::setProxyPwdValue(const QString &proxy_pwd) { m_Settings.setProxyPwd(proxy_pwd); }

bool AppController::isProxyConfigured() { return m_Settings.isProxyConfigured(); }

bool AppController::getShowSignatureOptions(void) { return m_Settings.getShowSignatureOptions(); }

void AppController::setShowSignatureOptions(bool bShowSignatureOptions) {
	m_Settings.setShowSignatureOptions(bShowSignatureOptions);
}

bool AppController::getShowSignatureHelp(void) { return m_Settings.getShowSignatureHelp(); }

void AppController::setShowSignatureHelp(bool bShowSignatureHelp) {
	m_Settings.setShowSignatureHelp(bShowSignatureHelp);
}

void AppController::setEnablePteidCache(bool bEnabled) { m_Settings.setEnablePteidCache(bEnabled); }

bool AppController::getEnablePteidCache() { return m_Settings.getEnablePteidCache(); }

void AppController::setEnablePteidCANCache(bool bEnabled) { m_Settings.setEnablePteidCANCache(bEnabled); }

bool AppController::getEnablePteidCANCache() { return m_Settings.getEnablePteidCANCache(); }

void AppController::flushCache() { Concurrent::run(this, &AppController::doFlushCache); }

void AppController::doFlushCache() {
	if (removePteidCache()) {
		PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui", "PTEID cache removed success!");
		emit signalRemovePteidCacheSuccess();
	}
}

bool AppController::removePteidCache() {

	try {
		GUISettings settings;
		QString ptEidCacheDir = settings.getPteidCachedir();
		qDebug() << "C++: Removing ptEidCache files: " << ptEidCacheDir;

		QDir dir(ptEidCacheDir);
		bool has_all_permissions = true;
#ifdef WIN32
		extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
		qt_ntfs_permission_lookup++; // turn ntfs checking (allows isReadable and isWritable)
#endif
		if (!dir.isReadable()) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "No read permissions: PTEID cache directory!");
			qDebug() << "C++: Cache folder does not have read permissions! ";
			has_all_permissions = false;
			emit signalCacheNotReadable();
		}
#ifdef WIN32
		qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif

		dir.setNameFilters(QStringList() << "*.ebin" << ".cache.csc");
		dir.setFilter(QDir::Files | QDir::Hidden);

		foreach (QString dirFile, dir.entryList()) {
			dir.remove(dirFile);
		}
		return has_all_permissions;
	} catch (...) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error removing PTEID cache directory from cache");
		emit signalRemovePteidCacheFail();
		return false;
	}
}

void AppController::getPteidCacheSize() { Concurrent::run(this, &AppController::doGetPteidCacheSize); }

QString AppController::getPteidCacheDir() {
	GUISettings settings;
	return settings.getPteidCachedir();
}

void AppController::createCacheDir() {
	QDir dir(m_Settings.getPteidCachedir());
	if (!dir.mkpath(".")) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::createCacheDir: Error creating cache directory :%s",
				  m_Settings.getPteidCachedir().toStdString().c_str());
	}
}

void AppController::doGetPteidCacheSize() {
	GUISettings settings;
	QString ptEidCacheDir = settings.getPteidCachedir();
	emit signalAppCacheSize(formatSize(dirSize(ptEidCacheDir, "*.ebin")));
}

void AppController::getScapCacheSize() { Concurrent::run(this, &AppController::doGetScapCacheSize); }

void AppController::doGetScapCacheSize() {
	ScapSettings settings;
	QString scapAttrDir = settings.getCacheDir() + "/scap_attributes/";
	QString scapLogoDir = settings.getCacheDir() + "/scap_logos/";
	const qint64 total_size = dirSize(scapAttrDir, "*.json") + dirSize(scapLogoDir, "*.jpeg");
	emit signalScapCacheSize(formatSize(total_size));
}

// borrowed from https://stackoverflow.com/a/47854799
qint64 AppController::dirSize(QString dirPath, QString nameFilter) {
	qint64 size = 0;
	QDir dir(dirPath);
	// calculate total size of current directories' files
	QDir::Filters fileFilters = QDir::Files | QDir::System | QDir::Hidden;
	dir.setNameFilters(QStringList() << nameFilter);
	for (QString filePath : dir.entryList(fileFilters)) {
		QFileInfo fi(dir, filePath);
		size += fi.size();
	}
	// add size of child directories recursively
	QDir::Filters dirFilters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden;
	for (QString childDirPath : dir.entryList(dirFilters))
		size += dirSize(dirPath + QDir::separator() + childDirPath, nameFilter);
	return size;
}
// borrowed from https://stackoverflow.com/a/47854799
QString AppController::formatSize(qint64 size) {
	QStringList units = {"Bytes", "KB", "MB", "GB", "TB", "PB"};
	int i;
	double outputSize = size;
	for (i = 0; i < units.size() - 1; i++) {
		if (outputSize < 1024)
			break;
		outputSize = outputSize / 1024;
	}
	return QString("%0 %1").arg(outputSize, 0, 'f', 0).arg(units[i]);
}

void getOutlookVersion(std::wstring &version) {
#ifdef WIN32
	version.assign(L"");
	DWORD dwType = REG_SZ;
	WCHAR abValueDat[23];
	DWORD dwValDatLen = sizeof(abValueDat);
	try {
		ReadReg(HKEY_CLASSES_ROOT, L"Outlook.Application\\CurVer", L"", &dwType, abValueDat, &dwValDatLen);
	} catch (...) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Outlook CurVer registry does not exist");
		return;
	}

	if (wcslen(abValueDat) <= 20) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Outlook CurVer registry format not expected");
		return;
	}
	version.assign(abValueDat, 20, 23);
#else
	Q_UNUSED(version);
#endif
}

bool AppController::isOutlookInstalled() {
#ifdef WIN32
	std::wstring version;
	getOutlookVersion(version);
	return version.compare(L"") != 0;
#endif

	return false;
}
bool AppController::getOutlookSuppressNameChecks(void) {
#ifdef WIN32
	std::wstring version;
	getOutlookVersion(version);
	if (version.compare(L"") != 0) {
		if (version.compare(L"9") == 0) {
			PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui",
					  "Can't write to Outlook 2000 (version 9) without admin privileges");
			return false;
		}
		std::wstring regName(L"Software\\Microsoft\\Office\\" + version + L".0\\Outlook\\Security");
		DWORD abValueDat(0);
		DWORD dwValDatLen(sizeof(abValueDat));
		DWORD dwType = REG_DWORD;
		try {
			ReadReg(HKEY_CURRENT_USER, regName.c_str(), L"SupressNameChecks", &dwType, &abValueDat, &dwValDatLen);
		} catch (...) {
			PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui",
					  "Outlook SupressNameChecks registry does not exist. RegName: %ls", regName.c_str());
			return false;
		}
		PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Get Outlook SupressNameChecks registry: %d RegName: %ls",
				  abValueDat, regName.c_str());
		return abValueDat == 1;
	}

	return false;
#endif

	return false;
}
void AppController::setOutlookSuppressNameChecks(bool bDisabledMatching) {
#ifdef WIN32
	std::wstring version;
	getOutlookVersion(version);
	if (version.compare(L"") != 0) {
		if (version.compare(L"9") == 0) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
					  "Can't write to Outlook 2000 (version 9) without admin privileges");
			return;
		}
		std::wstring regName(L"Software\\Microsoft\\Office\\" + version + L".0\\Outlook\\Security");
		DWORD abValueDat((bDisabledMatching ? 1 : 0));
		DWORD dwValDatLen(sizeof(abValueDat));
		DWORD dwType = REG_DWORD;
		try {
			WriteReg(HKEY_CURRENT_USER, regName.c_str(), L"SupressNameChecks", dwType, &abValueDat, dwValDatLen);
		} catch (...) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Could not set Outlook SupressNameChecks registry. RegName: %ls",
					  regName.c_str());
		}
		return;
	}
#endif
	(void)bDisabledMatching;
}

QString AppController::getFontFile(QString font) {
#ifdef WIN32
	std::string fontFile = "file:///";
	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_INSTALLDIR);
	fontFile.append(config.getString()).append("\\fonts\\");
#else
	std::string fontFile = "file://";
	QString executable_dir = m_Settings.getExePath() + "/";
#ifdef __APPLE__
	executable_dir.append("../Resources/");
#else
	executable_dir.append("../share/pteid-mw/fonts/");
#endif
	fontFile.append(executable_dir.toStdString());
#endif

	if (font.compare("lato") == 0) {
		fontFile.append("Lato-Regular.ttf");
	} else if (font.compare("myriad") == 0) {
		fontFile.append("MyriadPro-Regular.otf");
	} else {
		return "";
	}

	return QString::fromStdString(fontFile);
}

// This function should be called before instantiating a QApplication
void AppController::initApplicationScale() {
	double scale = 1;

	eIDMW::PTEID_Config configScale(eIDMW::PTEID_PARAM_GUITOOL_APPLICATIONSCALE);
	eIDMW::PTEID_Config configUseSystemScale(eIDMW::PTEID_PARAM_GUITOOL_USESYSTEMSCALE);

	int scaleSetting = configScale.getLong();
	bool useSystemScale = configUseSystemScale.getLong() != 0;

	if (!useSystemScale) {
		qDebug() << "C++: Using application scaling.";
		scale = 1 + 0.25 * scaleSetting; // scale levels are in 25% increments
	} else {
		qDebug() << "C++: Using system scaling.";
#ifdef WIN32
		// Get DPI from window with cursor where aplication window will be rendered
		POINT cursorPos;
		if (!GetPhysicalCursorPos(&cursorPos)) {
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error occurred getting cursor position: %d", GetLastError());
			return;
		}
		HWND hCursorWnd = WindowFromPoint(cursorPos);
		HDC hdc = GetDC(hCursorWnd);
		FLOAT horizontalDPI = static_cast<FLOAT>(GetDeviceCaps(hdc, LOGPIXELSX));
		ReleaseDC(0, hdc);

		scale = horizontalDPI / 96.0;

		// set scale setting to the system scale so it can be displayed in the application settings
		// convert to scale levels used in the application settings
		int scaleLevel = round((scale - 1) / 0.25);
		configScale.setLong(scaleLevel);
#else
		// System scale in application can only be configured in Windows

		// System scale is the default so set it to false (only relevant the first time the application runs)
		configUseSystemScale.setLong(0);
#endif
	}
	std::string scaleAsString = std::to_string(scale);
	QByteArray scaleAsQByteArray(scaleAsString.c_str(), (int)scaleAsString.length());
	qDebug() << "C++: Application scaling: " << scale;
	if (!qputenv("QT_SCALE_FACTOR", scaleAsQByteArray))
		qDebug() << "C++: Erro QT_SCALE_FACTOR";
}

bool AppController::getAskToRegisterCmdCertValue(void) { return m_Settings.getAskToRegisterCmdCert(); }
void AppController::setAskToRegisterCmdCertValue(bool bAskToRegisterCmdCert) {
	return m_Settings.setAskToRegisterCmdCert(bAskToRegisterCmdCert);
}

bool AppController::getAskToSetCacheValue(void) { return m_Settings.getAskToSetCache(); }
void AppController::setAskToSetCacheValue(bool bAskToSetCache) { return m_Settings.setAskToSetCache(bAskToSetCache); }

bool AppController::getAskToSetTelemetryValue(void) { return m_Settings.getAskToSetTelemetry(); }
void AppController::setAskToSetTelemetryValue(bool bAskToSetTelemetry) {
	return m_Settings.setAskToSetTelemetry(bAskToSetTelemetry);
}

void AppController::forceAccessibilityUpdate(QObject *obj) {
	/*
	Create a focus-gained event and notify the accessibility framework to force the active
	assistive tool (screen-reader) to reload the object's accessible properties.
	*/
	if (isAccessibilityActive()) {
		QAccessibleEvent ev(obj, QAccessible::Focus);
		QAccessible::updateAccessibility(&ev);
	}
	return;
}

bool AppController::isAccessibilityActive() { return QAccessible::isActive(); }

QStringList AppController::getFilesFromClipboard() {
	const QClipboard *clipboard = QGuiApplication::clipboard();
	QStringList filenames = clipboard->text().split(QRegExp("\n"));

	return filenames;
}

void AppController::openTransfersFolder() {
	QStringList args(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first());
	QProcess process;
	process.setProgram("/usr/bin/xdg-open");
	process.setArguments(args);
	if (!process.startDetached()) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
				  "AppController::openTransfersFolder: Failed to open file manager at: %s",
				  args.first().toStdString().c_str());
	}
}

QString timestampString() {
	char timestamp[TIMESTAMP_MAX_SIZE]; // format YYYYMMDDHHMMSS

	time_t _tm = time(nullptr);
	struct tm *t = localtime(&_tm);

	strftime(timestamp, TIMESTAMP_MAX_SIZE, "%Y%m%d%H%M%S", t);

	return QString(timestamp);
}

void AppController::zipLogs() {
	/*
	 *   create a zip on Desktop with log files
	 *   with name of type: Autenticacao.gov_logs_TIMESTAMP.zip
	 */
	std::string logDir;
#ifdef WIN32
	eIDMW::PTEID_Config loggingDirname(eIDMW::PTEID_PARAM_GENERAL_INSTALLDIR);
	logDir.append(loggingDirname.getString()).append("\\log\\");
#else
	eIDMW::PTEID_Config loggingDirname(eIDMW::PTEID_PARAM_LOGGING_DIRNAME);
	logDir.append(loggingDirname.getString());
#endif

	int status = 0;
	zip_t *pZip = NULL;

	QString zipFileName = QString("/Autenticacao.gov_logs_%1.zip").arg(timestampString());
	QString outputDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
	QString outputFile(outputDir + zipFileName);

#ifdef WIN32
	extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
	qt_ntfs_permission_lookup++; // turn ntfs checking (allows isReadable and isWritable)
#endif
	QFileInfo outputDirInfo(outputDir);
	if (!outputDirInfo.isWritable()) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: no permissions to write on dir %s",
				  outputDir.toStdString().c_str());
		emit signalZipLogsFail();
		return;
	}
#ifdef WIN32
	qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "AppController::zipLog called with outputFile: %s",
			  outputFile.toStdString().c_str());

	pZip = zip_open(outputFile.toStdString().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &status);
	if (!pZip) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: zip_open() failed, error %d", status);
		emit signalZipLogsFail();
		return;
	}

	// Add log files to the zip
	QDirIterator it(logDir.c_str(), QStringList() << ".PTEID_*" << "pteidmdrv.log", QDir::AllEntries | QDir::Hidden);
	while (it.hasNext()) {
		zip_source_t *source = zip_source_file(pZip, it.next().toStdString().c_str(), 0, -1);
		if (source == NULL || zip_file_add(pZip, it.fileName().toStdString().c_str(), source, ZIP_FL_ENC_GUESS) < 0) {
			zip_source_free(source);
			free(pZip);
			PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: Failed to add logs to zip container");
			emit signalZipLogsFail();
			return;
		}
	}

	if (zip_close(pZip) < 0) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: zip_close failed with error %s",
				  zip_error_strerror(zip_get_error(pZip)));
		free(pZip);
		emit signalZipLogsFail();
		return;
	}

	QFile newZipFile(outputFile);
	bool largeZip = newZipFile.size() > 15728640; // 15MiB

	zipFileName.remove(0, 1); // remove slash
	emit signalZipLogsSuccess(largeZip, zipFileName);
}

void AppController::getSignatureOptions() {
	std::string content;

	QString sign_options_file_path = m_Settings.getPteidCachedir() + "/SignOptions.json";
	QFile file(sign_options_file_path);
	if (!file.exists()) {
		return;
	}
	if (file.open(QIODevice::ReadOnly)) {
		content = file.readAll().toStdString();
		file.close();
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s Error reading SignOptions file: %s", __FUNCTION__,
				  sign_options_file_path.toStdString().c_str());
		return;
	}

	cJSON *json = NULL;
	if ((json = cJSON_Parse(content.c_str())) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to parse options file content: %s", __FUNCTION__,
				  content.c_str());
		return;
	}

	cJSON *pades_obj = NULL;
	if ((pades_obj = cJSON_GetObjectItem(json, "pades")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get pades field", __FUNCTION__);
	}
	bool pades = cJSON_IsTrue(pades_obj);

	cJSON *timestamp_obj = NULL;
	if ((timestamp_obj = cJSON_GetObjectItem(json, "timestamp")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get timestamp field", __FUNCTION__);
	}
	bool timestamp = cJSON_IsTrue(timestamp_obj);

	cJSON *lta_obj = NULL;
	if ((lta_obj = cJSON_GetObjectItem(json, "lta")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get lta field", __FUNCTION__);
	}
	bool lta = cJSON_IsTrue(lta_obj);

	cJSON *prof_name_obj = NULL;
	if ((prof_name_obj = cJSON_GetObjectItem(json, "professional_name")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get professional_name field", __FUNCTION__);
	}
	bool prof_name = cJSON_IsTrue(prof_name_obj);

	cJSON *visible_obj = NULL;
	if ((visible_obj = cJSON_GetObjectItem(json, "visible")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get visible field", __FUNCTION__);
	}
	bool visible = cJSON_IsTrue(visible_obj);

	cJSON *reduced_obj = NULL;
	if ((reduced_obj = cJSON_GetObjectItem(json, "reduced")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get reduced field", __FUNCTION__);
	}
	bool reduced = cJSON_IsTrue(reduced_obj);

	char *reason = NULL;
	if ((reason = cJSON_GetStringValue(cJSON_GetObjectItem(json, "reason"))) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get reason field", __FUNCTION__);
	}

	char *location = NULL;
	if ((location = cJSON_GetStringValue(cJSON_GetObjectItem(json, "location"))) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get location field", __FUNCTION__);
	}

	cJSON *seal_width_obj = NULL;
	double seal_width = 0;
	if ((seal_width_obj = cJSON_GetObjectItem(json, "seal_width")) == NULL || !cJSON_IsNumber(seal_width_obj)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get seal_width field", __FUNCTION__);
	} else {
		seal_width = cJSON_GetNumberValue(seal_width_obj);
	}

	cJSON *seal_height_obj = NULL;
	double seal_height = 0;
	if ((seal_height_obj = cJSON_GetObjectItem(json, "seal_height")) == NULL || !cJSON_IsNumber(seal_height_obj)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get seal_height field", __FUNCTION__);
	} else {
		seal_height = cJSON_GetNumberValue(seal_height_obj);
	}

	cJSON *scap_obj = NULL;
	if ((scap_obj = cJSON_GetObjectItem(json, "scap")) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to get scap field", __FUNCTION__);
	}
	bool scap = cJSON_IsTrue(scap_obj);

	QStringList attribute_ids;
    cJSON *attributes = NULL;
    // Verifies if the option is turned on
    eIDMW::PTEID_Config scapOptions(eIDMW::PTEID_PARAM_GUITOOL_SCAPOPTIONS);
    long scapOptionsEnabled = scapOptions.getLong();
	// 2 means that the option is enabled
    if (scapOptionsEnabled == 2){
        if ((attributes = cJSON_GetObjectItem(json, "attributes")) == NULL) {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to parse attributes array", __FUNCTION__);
        } else {
            const int array_size = cJSON_GetArraySize(attributes);
            for (int i = 0; i < array_size; ++i) {
                cJSON *array_item = NULL;
                if ((array_item = cJSON_GetArrayItem(attributes, i)) == NULL) {
                    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to parse attributes item", __FUNCTION__);
                }

                char *id = cJSON_GetStringValue(array_item);
                if (id) {
                    attribute_ids.push_back(id);
                }
            }
        }
    }

	QVariantList options;
	options.append({pades, timestamp, lta, prof_name, visible, reduced, reason, location, seal_width, seal_height, scap,
					attribute_ids});
	emit signalRetrieveStoredSignOptions(options);
}

static void add_to_options_object(cJSON *obj, const char *name, const char *value) {
	if (cJSON_AddStringToObject(obj, name, value) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to add field: %s, with value: %s", __FUNCTION__, name,
				  value);
	}
}

static void add_to_options_object(cJSON *obj, const char *name, bool value) {
	if (cJSON_AddBoolToObject(obj, name, value) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to add field: %s, with value: %s", __FUNCTION__, name,
				  value);
	}
}

static void add_to_options_object(cJSON *obj, const char *name, double value) {
	if (cJSON_AddNumberToObject(obj, name, value) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to add field: %s, with value: %s", __FUNCTION__, name,
				  value);
	}
}

static void add_to_options_object(cJSON *obj, const char *name, const std::vector<std::string> &strings) {
	std::vector<const char *> string_vector;
	for (const auto &s : strings) {
		string_vector.push_back(s.c_str());
	}

	cJSON *json_array = NULL;
	const char *empty = NULL;
	assert(string_vector.size() <= INT_MAX);
	if ((json_array = cJSON_CreateStringArray(string_vector.size() > 0 ? &string_vector[0] : &empty,
											  (int) string_vector.size())) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to create strings array", __FUNCTION__);
		return;
	}

	if (!cJSON_AddItemToObject(obj, name, json_array)) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s failed to add array field: %s", __FUNCTION__, name);
	}
}

void AppController::setSignatureOptions(const QVariantList &options) {
	bool pades = options.at(0).toBool();
	bool timestamp = options.at(1).toBool();
	bool lta = options.at(2).toBool();
	bool prof_name = options.at(3).toBool();
	bool visible = options.at(4).toBool();
	bool reduced = options.at(5).toBool();
	const QString &reason = options.at(6).toString();
	const QString &location = options.at(7).toString();
	const double seal_width = options.at(8).toDouble();
	const double seal_height = options.at(9).toDouble();
	const bool scap_signature = options.at(10).toBool();
	const QStringList &attribute_ids = options.at(11).toStringList();

	cJSON *json = NULL;
	if ((json = cJSON_CreateObject()) == NULL) {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s cJSON_CreateObject() failed", __FUNCTION__);
	}

	add_to_options_object(json, "pades", pades);
	add_to_options_object(json, "timestamp", timestamp);
	add_to_options_object(json, "lta", lta);
	add_to_options_object(json, "professional_name", prof_name);
	add_to_options_object(json, "visible", visible);
	add_to_options_object(json, "reduced", reduced);

	std::string s_reason = reason.toStdString();
	add_to_options_object(json, "reason", s_reason.c_str());

	std::string s_location = location.toStdString();
	add_to_options_object(json, "location", s_location.c_str());

	add_to_options_object(json, "seal_width", seal_width);
	add_to_options_object(json, "seal_height", seal_height);

	add_to_options_object(json, "scap", scap_signature);

	std::vector<std::string> attributes;
	foreach (const QString &id, attribute_ids) {
		attributes.push_back(id.toStdString());
	}

	add_to_options_object(json, "attributes", attributes);

	std::string options_json = cJSON_Print(json);

	cJSON_Delete(json);

	QString sign_options_file_path = m_Settings.getPteidCachedir() + "/SignOptions.json";
	QFile file(sign_options_file_path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		file.write(options_json.c_str());
		file.close();
	} else {
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "%s Error writing options file: %s", __FUNCTION__,
				  sign_options_file_path.toStdString().c_str());
	}

	return;
}

bool AppController::getAdminConfiguration() { return m_Settings.getAdminConfiguration(); }
