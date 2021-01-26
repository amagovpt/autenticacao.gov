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
#include <QNetworkProxy>
#endif

#include "pteidversions.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "gapi.h"
#include "autoUpdates.h"
#include "concurrent.h"

#define N_RELEASE_NOTES 3

struct PteidVersion
{
    int major;
    int minor;
    int release;
};

AppController::AppController(GUISettings& settings,QObject *parent) :
    QObject(parent)
  , m_Settings(settings)
{
    QString strVersion (PTEID_PRODUCT_VERSION);
    m_Settings.setGuiVersion(strVersion);
    qDebug() << "C++: AppController started. App version: " << m_Settings.getGuiVersion()
                + " - " + REVISION_NUM_STRING + " [ " + REVISION_HASH_STRING + " ]";;
    qDebug() << "C++: currentCpuArchitecture():" << QSysInfo::currentCpuArchitecture();
    qDebug() << "C++: prettyProductName():" << QSysInfo::prettyProductName();
    QByteArray ba = m_Settings.getGuiVersion().toLatin1();
    const char *c_str2 = ba.data();
    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui", "App started. %s - %s [%s]",
              c_str2, REVISION_NUM_STRING, REVISION_HASH_STRING);
    PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "eidgui",
              "CpuArch: %s ProductName: %s\n",
              QSysInfo::currentCpuArchitecture().toStdString().c_str(),
              QSysInfo::prettyProductName().toStdString().c_str());
    createCacheDir();
    checkUpdateCertslog();
    checkUpdateNewslog();
}

void AppController::restoreScreen(void){
    emit signalRestoreWindows();
}

bool AppController::getTestMode(void){
    return m_Settings.getTestMode();
}

QString AppController::getAppVersion(void){

    return m_Settings.getGuiVersion() + " - " + REVISION_NUM_STRING;
}

QString AppController::getAppRevision(void){

    return REVISION_HASH_STRING;
}

void AppController::checkUpdateCertslog(){

    QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
    QFile file(updateCertsLog);

    // Check if certs update log exist.
    if(!file.open(QIODevice::ReadOnly))
    {
        updateCertslog();
    }
    file.close();
}

void AppController::updateCertslog(){

    QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
    QFile file(updateCertsLog);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // We're going to streaming text to the file
        char date_string[STR_LOCALTIME_MAX_SIZE];
        QTextStream stream(&file);
        time_t _tm =time(nullptr);
        struct tm * t = localtime ( &_tm );
        strftime(date_string, STR_LOCALTIME_MAX_SIZE, "%H:%M ON_DAY %d-%m-%Y", t);
        stream << date_string;
        file.close();
    } else {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AutoUpdates::RunCertsPackage: Error Writing updateCertsLog.txt :%s",
                  updateCertsLog.toStdString().c_str());
    }
}

QString AppController::getCertsLog(void){

    QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
    QFile file(updateCertsLog);

    if(!file.open(QIODevice::ReadOnly))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AutoUpdates::getCertsLog: Error Reading updateCertsLog.txt :%s",
                  updateCertsLog.toStdString().c_str());
        return tr("STR_CERTS_UPDATE_LOG_ERROR");
    }

    QTextStream instream(&file);

    QString line = instream.readLine(STR_LOCALTIME_MAX_SIZE);

    line.replace("ON_DAY", tr("STR_ON_DAY"));
    file.close();

    return line;
}

void AppController::checkUpdateNewslog(){

    QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";

    // Create updateNewsLog.txt file if it does not exist.
    if(!QFile::exists(updateNewsLog))
    {
        QFile file(updateNewsLog);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "AppController::checkUpdateNewslog: Error Creating updateNewsLog.txt");
            return;
        }
        file.close();
    }
}

void AppController::updateNewsLog(){

    QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";
    QFile file(updateNewsLog);

    QString id = newsUpdate.getActiveNewsId();

    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << id << ",";
        file.close();
    } else {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AppController::updateNewsLog: Error Writing updateNewsLog.txt :%s",
                  updateNewsLog.toStdString().c_str());
    }
}

QString AppController::getNewsLog(void){

    QString updateNewsLog = m_Settings.getPteidCachedir() + "/updateNewsLog.txt";
    QFile file(updateNewsLog);

    if(!file.open(QIODevice::ReadOnly))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AppController::getNewsLog: Error Reading updateNewsLog.txt :%s",
                  updateNewsLog.toStdString().c_str());
        return "";
    }

    QTextStream instream(&file);
    QString line = instream.readLine();
    file.close();

    return line;
}

bool AppController::isToShowNews(QString id){

    return !getNewsLog().split(",").contains(id);
}

QString AppController::getAppCopyright(void){

    return PTEID_COPYRIGHT;
}

bool AppController::isAnimationsEnabled(void){

    return m_Settings.getShowAnimations();
}
bool AppController::getNotShowHelpStartUp(void){

    return m_Settings.getNotShowHelpStartUp();
}
void AppController::setNotShowHelpStartUp(bool notshowhelpStartUp){

    m_Settings.setNotShowHelpStartUp(notshowhelpStartUp);
}
void AppController::initTranslation(){

    QString     appPath = QCoreApplication::applicationDirPath();
    m_Settings.setExePath(appPath);
    QString CurrLng   = m_Settings.getGuiLanguageString();

    if (LoadTranslationFile(CurrLng)==false){
        emit signalLanguageChangedError();
    }
}

bool AppController::LoadTranslationFile(QString NewLanguage)
{

    QString strTranslationFile;
    QString translations_dir;
    strTranslationFile = QString("eidmw_") + NewLanguage;

#ifdef __APPLE__
    translations_dir = m_Settings.getExePath()+"/../Resources/";
#else
    translations_dir = m_Settings.getExePath()+"/";
#endif

    qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << translations_dir;

    if (!m_translator.load(strTranslationFile,translations_dir))
    {
        // this should not happen, since we've built the menu with the translation filenames
        strTranslationFile = QString("eidmw_") + STR_DEF_GUILANGUAGE;
        //try load default translation file
        qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << translations_dir;
        if (!m_translator.load(strTranslationFile,translations_dir))
        {
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
void AppController::autoUpdateApp(){
    qDebug() << "C++: Starting autoUpdateApp";

    appUpdate.setAppController(this);
    appUpdate.initRequest(GAPI::AutoUpdateApp);
}

void AppController::autoUpdatesCerts(){
    qDebug() << "C++: Starting autoUpdates Certs";

    certsUpdate.setAppController(this);
    certsUpdate.initRequest(GAPI::AutoUpdateCerts);
}

void AppController::autoUpdatesNews(){
    qDebug() << "C++: Starting autoUpdates News";

    newsUpdate.setAppController(this);
    newsUpdate.initRequest(GAPI::AutoUpdateNews);
}

void AppController::startUpdateApp(){
    qDebug() << "C++: startUpdateApp";
    PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
                "AppController::startUpdateApp: App update started!");
    appUpdate.startUpdate();
}

void AppController::startUpdateCerts(){
    qDebug() << "C++: startUpdateCerts";
    certsUpdate.startUpdate();
}

void AppController::userCancelledUpdateCertsDownload(){
    qDebug() << "C++: userCancelledUpdateCertsDownload";
    certsUpdate.userCancelledUpdateDownload();
}

void AppController::userCancelledUpdateAppDownload(){
    qDebug() << "C++: userCancelledUpdateAppDownload";
    appUpdate.userCancelledUpdateDownload();
}

QVariant AppController::getCursorPos()
{
    return QVariant(QCursor::pos());
}
bool AppController::getAutoCardReadingValue (void){

    return m_Settings.getAutoCardReading();
}
void AppController::setAutoCardReadingValue (bool bAutoCardReading ){

    m_Settings.setAutoCardReading(bAutoCardReading);
}
bool AppController::getPinpadEnabledValue(void){

    return m_Settings.getPinpadEnabled();
}
void AppController::setPinpadEnabledValue(bool bPinpadEnabled){

    m_Settings.setPinpadEnabled(bPinpadEnabled);
}
bool AppController::getStartAutoValue (void){

    return m_Settings.getAutoStartup();
}
void AppController::setStartAutoValue (bool bAutoStartup ){

    m_Settings.setAutoStartup(bAutoStartup);
}
bool AppController::getStartAutoupdateValue(void){

    return m_Settings.getStartAutoupdate();
}
void AppController::setStartAutoupdateValue(bool bStartAutoupdate){

    m_Settings.setStartAutoupdate(bStartAutoupdate);
}
QString AppController::getGuiLanguageString (void){

    return m_Settings.getGuiLanguageString();
}
void AppController::setGuiLanguageString (QString language){

    if (LoadTranslationFile(language)){
        m_Settings.setLanguage(language);
        emit languageChanged();
    }else{
        emit signalLanguageChangedError();
    }
}
bool AppController::getStartMinimizedValue (void){

    return m_Settings.getStartMinimized();
}
void AppController::setStartMinimizedValue (bool bStartMinimized ){

    m_Settings.setStartMinimized(bStartMinimized);
}
bool AppController::getDebugModeValue (void){

    return m_Settings.getDebugMode();
}
QString AppController::setDebugModeValue (bool bDebugMode) {

    return m_Settings.setDebugMode(bDebugMode);
}
bool AppController::getShowNotificationValue (void){

    return m_Settings.getShowNotification();
}
bool AppController::getShowPictureValue (void){

    return m_Settings.getShowPicture();
}
bool AppController::getShowAnimationsValue(void){
    return m_Settings.getShowAnimations();
}
bool AppController::getUseSystemScaleValue(void) {
    return m_Settings.getUseSystemScale();
}
int AppController::getApplicationScaleValue(void) {
    return m_Settings.getApplicationScale();
}
int AppController::getGraphicsAccelValue(void){
    return m_Settings.getGraphicsAccel();
}
void AppController::setShowNotificationValue (bool bShowNotification){

    m_Settings.setShowNotification(bShowNotification);
}
void AppController::setShowPictureValue (bool bShowPicture){

    m_Settings.setShowPicture(bShowPicture);
}
void AppController::setShowAnimationsValue(bool bShowAnimations){
    m_Settings.setShowAnimations(bShowAnimations);
}
void AppController::setUseSystemScaleValue(bool bUseSystemScale) {
    m_Settings.setUseSystemScale(bUseSystemScale);
}
void AppController::setApplicationScaleValue(int iScale) {
    m_Settings.setApplicationScale(iScale);
}
void AppController::setGraphicsAccelValue(int iGraphicsAccel){
    m_Settings.setAccelGraphics(iGraphicsAccel);
}
QString AppController::getTimeStampHostValue (void){
    return m_Settings.getTimeStampHost();
}
void AppController::setTimeStampHostValue (QString const& timeStamp_host){
    m_Settings.setTimeStampHost(timeStamp_host);
}
bool AppController::getProxySystemValue (void){

    return m_Settings.getProxySystem();
}
void AppController::setProxySystemValue (bool bProxySystem){

    m_Settings.setProxySystem(bProxySystem);
}
QString AppController::getProxyHostValue (void){

    return m_Settings.getProxyHost();
}
void AppController::setProxyHostValue (QString const& proxy_host){
    m_Settings.setProxyHost(proxy_host);
}
long AppController::getProxyPortValue (void){

    return m_Settings.getProxyPort();
}
void AppController::setProxyPortValue (int proxy_port){
    m_Settings.setProxyPort(proxy_port);
}
QString AppController::getProxyUsernameValue (void){

    return m_Settings.getProxyUsername();
}
void AppController::setProxyUsernameValue (QString const& proxy_user){
    m_Settings.setProxyUsername(proxy_user);
}
QString AppController::getProxyPwdValue (void){

    return m_Settings.getProxyPwd();
}
void AppController::setProxyPwdValue (QString const& proxy_pwd){
    m_Settings.setProxyPwd(proxy_pwd);
}

bool AppController::isProxyConfigured() {
    return m_Settings.isProxyConfigured();
}

void AppController::setEnablePteidCache (bool bEnabled){
    m_Settings.setEnablePteidCache(bEnabled);
}

bool AppController::getEnablePteidCache (){
    return m_Settings.getEnablePteidCache();
}

void AppController::flushCache(){
    Concurrent::run(this, &AppController::doFlushCache);
}

void AppController::doFlushCache(){
    if(removePteidCache()){
        PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
                "PTEID cache removed success!");
        emit signalRemovePteidCacheSuccess();
    }
}

bool AppController::removePteidCache() {

    try
    {
        GUISettings settings;
        QString ptEidCacheDir = settings.getPteidCachedir();
        qDebug() << "C++: Removing ptEidCache files: " << ptEidCacheDir;

        QDir dir(ptEidCacheDir);
        bool has_all_permissions = true;
#ifdef WIN32
        extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
        qt_ntfs_permission_lookup++; // turn ntfs checking (allows isReadable and isWritable)
#endif
        if(!dir.isReadable())
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "No read permissions: PTEID cache directory!");
            qDebug() << "C++: Cache folder does not have read permissions! ";
            has_all_permissions = false;
            emit signalCacheNotReadable();
        }
#ifdef WIN32
        qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif

        dir.setNameFilters(QStringList() << "*.bin" << ".cache.csc");
        dir.setFilter(QDir::Files | QDir::Hidden);

        foreach(QString dirFile, dir.entryList())
        {
            dir.remove(dirFile);
        }
        return has_all_permissions;
    }
    catch(...) {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "Error removing PTEID cache directory from cache");
        emit signalRemovePteidCacheFail();
        return false;
    }
}


void AppController::getPteidCacheSize() {
    Concurrent::run(this, &AppController::doGetPteidCacheSize);
}

QString AppController::getPteidCacheDir() {
    GUISettings settings;
    return settings.getPteidCachedir();
}

void AppController::createCacheDir(){
    QDir dir(m_Settings.getPteidCachedir());
    if(!dir.mkpath(".")){
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AppController::createCacheDir: Error creating cache directory :%s",
                  m_Settings.getPteidCachedir().toStdString().c_str());
    }
}

void AppController::doGetPteidCacheSize() {
    GUISettings settings;
    QString ptEidCacheDir = settings.getPteidCachedir();
    emit signalAppCacheSize(formatSize(dirSize(ptEidCacheDir, "*.bin")));
}

void AppController::getScapCacheSize() {
    Concurrent::run(this, &AppController::doGetScapCacheSize);
}

void AppController::doGetScapCacheSize() {
    ScapSettings settings;
    QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
    emit signalScapCacheSize(formatSize(dirSize(scapCacheDir, "*.xml")));
}

//borrowed from https://stackoverflow.com/a/47854799
qint64 AppController::dirSize(QString dirPath, QString nameFilter) {
    qint64 size = 0;
    QDir dir(dirPath);
    //calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    dir.setNameFilters(QStringList() << nameFilter);
    for(QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size+= fi.size();
    }
    //add size of child directories recursively
    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for(QString childDirPath : dir.entryList(dirFilters))
        size+= dirSize(dirPath + QDir::separator() + childDirPath, nameFilter);
    return size;
}
//borrowed from https://stackoverflow.com/a/47854799
QString AppController::formatSize(qint64 size) {
    QStringList units = {"Bytes", "KB", "MB", "GB", "TB", "PB"};
    int i;
    double outputSize = size;
    for(i=0; i<units.size()-1; i++) {
        if(outputSize<1024) break;
        outputSize= outputSize/1024;
    }
    return QString("%0 %1").arg(outputSize, 0, 'f', 0).arg(units[i]);
}

void getOutlookVersion(std::wstring &version) {
#ifdef WIN32
    version.assign(L"");
    DWORD dwType = REG_SZ;
    WCHAR abValueDat[23];
    DWORD dwValDatLen = sizeof(abValueDat);
    try
    {
        ReadReg(HKEY_CLASSES_ROOT, L"Outlook.Application\\CurVer", L"", &dwType, abValueDat, &dwValDatLen);
    }
    catch (...)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Outlook CurVer registry does not exist");
        return;
    }

    if (wcslen(abValueDat) <= 20)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Outlook CurVer registry format not expected");
        return;
    }
    version.assign(abValueDat, 20, 23);
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
    if (version.compare(L"") != 0)
    {
        if (version.compare(L"9") == 0)
        {
            PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Can't write to Outlook 2000 (version 9) without admin privileges");
            return false;
        }
        std::wstring regName(L"Software\\Microsoft\\Office\\" + version + L".0\\Outlook\\Security");
        DWORD abValueDat(0);
        DWORD dwValDatLen(sizeof(abValueDat));
        DWORD dwType = REG_DWORD;
        try
        {
            ReadReg(HKEY_CURRENT_USER, regName.c_str(), L"SupressNameChecks", &dwType, &abValueDat, &dwValDatLen);
        }
        catch (...)
        {
            PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Outlook SupressNameChecks registry does not exist. RegName: %ls", regName.c_str());
            return false;
        }
        PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Get Outlook SupressNameChecks registry: %d RegName: %ls", abValueDat, regName.c_str());
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
    if (version.compare(L"") != 0)
    {
        if (version.compare(L"9") == 0)
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Can't write to Outlook 2000 (version 9) without admin privileges");
            return;
        }
        std::wstring regName(L"Software\\Microsoft\\Office\\" + version + L".0\\Outlook\\Security");
        DWORD abValueDat((bDisabledMatching ? 1 : 0));
        DWORD dwValDatLen(sizeof(abValueDat));
        DWORD dwType = REG_DWORD;
        try
        {
            WriteReg(HKEY_CURRENT_USER, regName.c_str(), L"SupressNameChecks", dwType, &abValueDat, dwValDatLen);
        }
        catch (...)
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Could not set Outlook SupressNameChecks registry. RegName: %ls", regName.c_str());
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
    QString executable_dir = m_Settings.getExePath()+"/";
#ifdef __APPLE__
    executable_dir.append("../Resources/");
#endif
    fontFile.append(executable_dir.toStdString());
#endif

    if (font.compare("lato") == 0)
    {
        fontFile.append("Lato-Regular.ttf");
    }
    else{
        return "";
    }

#ifdef WIN32
    return QString::fromLatin1(fontFile.c_str());
#else
    return QString::fromStdString(fontFile);
#endif

}

// This function should be called before instantiating a QApplication
void AppController::initApplicationScale() {
    double scale = 1;

    eIDMW::PTEID_Config configScale(eIDMW::PTEID_PARAM_GUITOOL_APPLICATIONSCALE);
    eIDMW::PTEID_Config configUseSystemScale(eIDMW::PTEID_PARAM_GUITOOL_USESYSTEMSCALE);

    int scaleSetting = configScale.getLong();
    bool useSystemScale = configUseSystemScale.getLong() != 0;

    if (!useSystemScale)
    {
        qDebug() << "C++: Using application scaling.";
        scale = 1 + 0.25 * scaleSetting; // scale levels are in 25% increments
    }
    else
    {
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

bool AppController::getAskToRegisterCmdCertValue(void) {
    return m_Settings.getAskToRegisterCmdCert();
}
void AppController::setAskToRegisterCmdCertValue(bool bAskToRegisterCmdCert) {
    return m_Settings.setAskToRegisterCmdCert(bAskToRegisterCmdCert);
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

bool AppController::isAccessibilityActive(){
	return QAccessible::isActive();
}

QStringList AppController::getFilesFromClipboard(){
    const QClipboard *clipboard = QGuiApplication::clipboard();
    QStringList filenames = clipboard->text().split(QRegExp("\n"));

    return filenames;
}

void AppController::openTransfersFolder(){
    QStringList args(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first());
    QProcess process;
    process.setProgram("/usr/bin/xdg-open");
    process.setArguments(args);
    if(!process.startDetached())
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AppController::openTransfersFolder: Failed to open file manager at: %s", args.first().toStdString().c_str());
    }
}

QString timestampString(){
    char timestamp[TIMESTAMP_MAX_SIZE]; // format YYYYMMDDHHMMSS

    time_t _tm = time(nullptr);
    struct tm * t = localtime(&_tm);

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
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: no permissions to write on dir %s", outputDir.toStdString().c_str());
        emit signalZipLogsFail();
        return;
    }
#ifdef WIN32
    qt_ntfs_permission_lookup--; // turn ntfs permissions lookup off for performance
#endif

    PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "AppController::zipLog called with outputFile: %s", outputFile.toStdString().c_str());

    pZip = zip_open(outputFile.toStdString().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &status);
    if(!pZip) {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: zip_open() failed, error %d", status);
        emit signalZipLogsFail();
        return;
    }

    // Add log files to the zip
    QDirIterator it(logDir.c_str(), QStringList() << ".PTEID_*" << "pteidmdrv.log", QDir::AllEntries | QDir::Hidden);
    while (it.hasNext()) {
        zip_source_t *source = zip_source_file(pZip, it.next().toStdString().c_str(), 0, -1);
        if (source == NULL || zip_file_add(pZip, it.fileName().toStdString().c_str(), source, ZIP_FL_ENC_GUESS) < 0)
        {
            zip_source_free(source);
            free(pZip);
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: Failed to add logs to zip container");
            emit signalZipLogsFail();
            return;
        }
    }

    if (zip_close(pZip) < 0)
    {
        free(pZip);
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "AppController::zipLog: zip_close failed with error %d", zip_error_strerror(zip_get_error(pZip)));
        emit signalZipLogsFail();
        return;
    }

    QFile newZipFile(outputFile);
    bool largeZip = newZipFile.size() > 15728640; //15MiB

    zipFileName.remove(0,1); //remove slash
    emit signalZipLogsSuccess(largeZip, zipFileName);
}