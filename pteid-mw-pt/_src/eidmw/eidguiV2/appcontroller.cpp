/*****************************************************************************

 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "appcontroller.h"
#include <QObject>
#include <QCursor>
#include <QDebug>
#include <QtConcurrent>
#include <QAccessible>
#include <QClipboard>
#include <QGuiApplication>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

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
    checkUpdateCertslog();
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
        char date_string[25];
        QTextStream stream(&file);
        time_t _tm =time(nullptr);
        struct tm * t = localtime ( &_tm );
        strftime(date_string, 25, "%H:%M do dia %d-%m-%Y", t);
        stream << date_string;
        file.close();
    } else {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AutoUpdates::RunCertsPackage: Error Writing updateCertsLog.txt :%s",
                  updateCertsLog.toStdString().c_str());
    }
}

QString AppController::getAppCertsUpdate(void){

    QString updateCertsLog = m_Settings.getPteidCachedir() + "/updateCertsLog.txt";
    QFile file(updateCertsLog);

    if(!file.open(QIODevice::ReadOnly))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AutoUpdates::getAppCertsUpdate: Error Reading updateCertsLog.txt :%s",
                  updateCertsLog.toStdString().c_str());
        return tr("STR_CERTS_UPDATE_LOG_ERROR");
    }

    QTextStream instream(&file);
    QString line = instream.readLine();

    file.close();

    return line;
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
    strTranslationFile = QString("eidmw_") + NewLanguage;

    qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << m_Settings.getExePath();

    if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
    {
        // this should not happen, since we've built the menu with the translation filenames
        strTranslationFile = QString("eidmw_") + STR_DEF_GUILANGUAGE;
        //try load default translation file
        qDebug() << "C++: AppController LoadTranslationFile" << strTranslationFile << m_Settings.getExePath();
        if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
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

void AppController::startUpdateApp(){
    qDebug() << "C++: startUpdateApp";
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
bool AppController::getGraphicsAccelValue(void){
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
void AppController::setGraphicsAccelValue(bool bGraphicsAccel){
    m_Settings.setAccelGraphics(bGraphicsAccel);
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

void AppController::flushCache(){
    QtConcurrent::run(this, &AppController::doFlushCache);
}

void AppController::doFlushCache(){
    if(removePteidCache()){
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
        std::cerr << "Error ocurred while removing ptEidCache from cache!";
        emit signalRemovePteidCacheFail();
        return false;
    }
}


void AppController::getPteidCacheSize() {
    QtConcurrent::run(this, &AppController::doGetPteidCacheSize);
}

QString AppController::getPteidCacheDir() {
    GUISettings settings;
    return settings.getPteidCachedir();
}

void AppController::doGetPteidCacheSize() {
    GUISettings settings;
    QString ptEidCacheDir = settings.getPteidCachedir();
    emit signalAppCacheSize(formatSize(dirSize(ptEidCacheDir, "*.bin")));
}

void AppController::getScapCacheSize() {
    QtConcurrent::run(this, &AppController::doGetScapCacheSize);
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
            PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui", "Outlook SupressNameChecks registry does not exist");
        }
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
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Could not set Outlook SupressNameChecks registry");
        }
        return;
    }
#endif
    (void)bDisabledMatching;
}

QString AppController::getFontFile(QString font) {
    std::string fontFile = "file:///";
#ifdef WIN32
    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_INSTALLDIR);
    fontFile.append(config.getString()).append("\\fonts\\");
#else
    fontFile.append("usr/local/share/fonts/");
#endif

    if (font.compare("lato") == 0)
    {
        fontFile.append("Lato-Regular.ttf");
    }
    else{
        return "";
    }

    return QString(fontFile.c_str());
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
