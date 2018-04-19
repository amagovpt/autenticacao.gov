#include "appcontroller.h"
#include <QObject>
#include <QCursor>
#include <QDebug>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "eidlib.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <QSysInfo>
#include <QNetworkProxy>
#endif

#include "pteidversions.h"

#ifndef WIN32
#include <unistd.h>
#endif

#include "gapi.h"

std::string serverurl = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/";
std::string remoteversion = "https://svn.gov.pt/projects/ccidadao/repository/middleware-offline/tags/builds/lastversion/version.txt";

std::string WINDOWS32 = "PteidMW-Basic.msi";
std::string WINDOWS64 = "PteidMW-Basic-x64.msi";
std::string MAC_OS = "pteid-mw.pkg";
std::string DEBIAN32 = "pteid-mw_debian_i386.deb";
std::string DEBIAN64 = "pteid-mw_debian_amd64.deb";
std::string UBUNTU32 = "pteid-mw_ubuntu_i386.deb";
std::string UBUNTU64 = "pteid-mw_ubuntu_amd64.deb";
std::string FEDORA32 = "pteid-mw-fedora.i386.rpm";
std::string FEDORA64 = "pteid-mw-fedora.x86_64.rpm";
std::string SUSE32 = "pteid-mw-suse.i586.rpm";
std::string SUSE64 = "pteid-mw-suse.x86_64.rpm";
std::string MANDRIVA32 = "pteid-mw-mandriva.i586.rpm";
std::string MANDRIVA64 = "pteid-mw-mandriva.x86_64.rpm";

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
    qDebug() << "C++: AppController started. App version: " << m_Settings.getGuiVersion() +" - "+ SVN_REVISION_STR;
}

void AppController::restoreScreen(void){
    emit signalRestoreWindows();
}

bool AppController::getTestMode(void){
    return m_Settings.getTestMode();
}

QString AppController::getAppVersion(void){

    return m_Settings.getGuiVersion() +" - "+ SVN_REVISION_STR;
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
void AppController::autoUpdates(){
    qDebug() << "C++: Starting autoUpdates";
    url = remoteversion.c_str();

    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();
    if (fileName.isEmpty())
    {
         emit signalAutoUpdateFail(GAPI::GenericError);
         return;
    }

    std::string tmpfile;
    tmpfile.append(QDir::tempPath().toStdString());
    tmpfile.append("/");
    tmpfile.append(fileName.toStdString());

    file = new QFile(QString::fromUtf8((tmpfile.c_str())));
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug() << "C++: Unable to save the file";
        delete file;
        file = 0;
        emit signalAutoUpdateFail(GAPI::UnableSaveFile);
        return;
    }
    // schedule the request
    httpRequestAborted = false;
    startRequest(url);
}

void AppController::startUpdate(){
    qDebug() << "C++: startUpdate";
    url = urli.c_str();

    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();

    if (fileName.isEmpty())
    {
         emit signalAutoUpdateFail(GAPI::GenericError);
    }
    QFile::remove(fileName);

    std::string tmpfile;
    tmpfile.append(QDir::tempPath().toStdString());
    tmpfile.append("/");
    tmpfile.append(fileName.toStdString());

    file = new QFile(QString::fromUtf8((tmpfile.c_str())));
    if (!file->open(QIODevice::WriteOnly)) {
       qDebug() << "C++: Unable to save the file";
        delete file;
        file = 0;
        emit signalAutoUpdateFail(GAPI::UnableSaveFile);
        return;
    }

    emit signalStartUpdate(fileName);

    // schedule the request
    httpUpdateRequestAborted = false;
    startUpdateRequest(url);
}

void AppController::startRequest(QUrl url){
    qDebug() << "C++: startRequest";

    eIDMW::PTEID_Config config_pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
    const char * pacfile_url = config_pacfile.getString();

    if (pacfile_url != NULL && strlen(pacfile_url) > 0)
    {
        m_pac_url = QString(pacfile_url);
    }

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
    eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);
    eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
    eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);

    std::string proxy_host = config.getString();
    std::string proxy_username = config_username.getString();
    std::string proxy_pwd = config_pwd.getString();
    long proxy_port = config2.getLong();

    //10 second timeout
    int network_timeout = 10000;

    if (!proxy_host.empty() && proxy_port != 0)
    {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "eidgui", "Autoupdates: using manual proxy config");
        qDebug() << "C++: Autoupdates: using manual proxy config";
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(QString::fromStdString(proxy_host));
        proxy.setPort(proxy_port);

        if (!proxy_username.empty())
        {
            proxy.setUser(QString::fromStdString(proxy_username));
            proxy.setPassword(QString::fromStdString(proxy_pwd));
        }

        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if (!m_pac_url.isEmpty())
    {
        std::string proxy_port_str;
        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Autoupdates: using system proxy config");
        qDebug() << "C++: Autoupdates: using system proxy config";
        PTEID_GetProxyFromPac(m_pac_url.toUtf8().constData(), url.toString().toUtf8().constData(), &proxy_host, &proxy_port_str);
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(QString::fromStdString(proxy_host));
        proxy.setPort(atol(proxy_port_str.c_str()));
        QNetworkProxy::setApplicationProxy(proxy);
    }

    reply = qnam.get(QNetworkRequest(url));
    QTimer::singleShot(network_timeout, this, SLOT(cancelDownload()));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));

}
void AppController::startUpdateRequest(QUrl url){
    qDebug() << "C++: startUpdateRequest";

    int network_timeout = 10000;
    reply = qnam.get(QNetworkRequest(url));
    QTimer::singleShot(network_timeout, this, SLOT(cancelUpdateDownload()));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpUpdateFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpUpdateReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateUpdateDataReadProgress(qint64,qint64)));
}

void AppController::httpUpdateFinished(){
    qDebug() << "C++: httpUpdateFinished";
    if (httpUpdateRequestAborted)
    {
        if (file)
        {
            file->close();
            file->remove();
            delete file;
            file = 0;
            qDebug() << "C++: httpUpdateRequestAborted";
        }
        reply->deleteLater();
        emit signalAutoUpdateFail(GAPI::DownloadFailed);
        return;
    }
    if (!file){
        reply->deleteLater();
        emit signalAutoUpdateFail(GAPI::DownloadFailed);
        return;
    }
    file->flush();
    file->close();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error())
    {
        file->remove();
        emit signalAutoUpdateFail(GAPI::DownloadFailed);
    }
    else if (!redirectionTarget.isNull())
    {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());

        qDebug() << "C++: Redirect";
        // TODO: ask about redirect.
            url = newUrl;
            reply->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
    }

    if (!reply->error())
        RunPackage(fileName.toStdString() , getdistro);

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void AppController::RunPackage(std::string pkg, std::string distro){
    qDebug() << "C++: RunPackage filename";

    std::string pkgpath;
    // TODO: test with Ubunto < 17

    pkgpath.append(QDir::tempPath().toStdString());
    pkgpath.append("/");
    pkgpath.append(pkg);
#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);

    std::string winpath;
    winpath.append("C:\\Windows\\system32\\msiexec.exe /i");

    QString s = QDir::toNativeSeparators(QString::fromStdString(pkgpath));
    winpath.append(s.toStdString());
    winpath.append(" /L*v ");
    winpath.append(QDir::tempPath().toStdString());
    winpath.append("\\Pteid-MSI.log");
    CreateProcess(NULL, LPTSTR(winpath.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	
	PTEID_ReleaseSDK();
	
    exit(0);

#elif __APPLE__
    // This launches the GUI installation process, the user has to follow the wizard to actually perform the installation
    execl("/usr/bin/open", pkgpath.c_str(), NULL);
#else

    //Normalize distro string to lowercase
    std::transform(distro.begin(), distro.end(), distro.begin(), ::tolower);

    std::cout << "pkgpath " << pkgpath << " distro " << distro << std::endl;

    if (distro == "debian" || distro == "ubuntu" || distro == "caixamagica")
    {
        // TODO: Ubunto < 17
        execl ("/usr/bin/software-center", "software-center", pkgpath.c_str(), NULL);
        pkgpath.insert(0,"--local-filename=");
        execl ("/usr/bin/ubuntu-software", "gnome-software", pkgpath.c_str(), NULL);
    }

    else if (distro == "fedora")
    {
        execl ("/usr/bin/gpk-install-local-file", "gpk-install-local-file", pkgpath.c_str(), NULL);
    }

    else if (distro == "suse")
    {
            execl ("/usr/bin/gpk-install-local-file", "gpk-install-local-file", pkgpath.c_str(), NULL);
    }
#endif
    qDebug() << "C++: RunPackage finish";
}

void AppController::httpUpdateReadyRead()
{
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());

}

void AppController::updateUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpUpdateRequestAborted)
        return;

    qint64 valueFloat = (quint64)((double) (bytesRead * 100 / totalBytes ));
    emit signalAutoUpdateProgress((int)valueFloat);

}
void AppController::httpFinished()
{
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }

        emit signalAutoUpdateFail(GAPI::DownloadFailed);
        reply->deleteLater();
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qDebug() << "C++: reply error";
        file->remove();
        emit signalAutoUpdateFail(GAPI::DownloadFailed);
        QString strLog = QString("AutoUpdates:: Download failed: ");
        strLog += reply->url().toString();
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", strLog.toStdString().c_str() );
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        // TODO: ask about redirect.
        {
            url = newUrl;
            reply->deleteLater();
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
        }
    }
    VerifyUpdates(filedata);
}

void AppController::httpReadyRead()
{
    qDebug() << "C++: httpReadyRead";
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply

    QByteArray data = reply->readAll();
    QString qsdata(data);
    filedata = qsdata.toStdString();

    //Write to file
    if (file)
        file->write(data);

}

void AppController::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (httpRequestAborted)
        return;
}
int compareVersions(PteidVersion v1, PteidVersion v2)
{

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

bool AppController::VerifyUpdates(std::string filedata)
{
    qDebug() << "C++: VerifyUpdates";

    std::string distrover;
    std::string archver;

    QString ver (WIN_GUI_VERSION_STRING);

    //Only consider the first line of version.txt
    QString remote_data(filedata.c_str());
    remote_data = remote_data.left(remote_data.indexOf('\n'));

    QStringList list1 = ver.split(",");

    QStringList list2 = remote_data.split(",");

    if (list2.size() < 3)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::VerifyUpdates: Wrong data returned from server or Proxy HTML error!");
        qDebug() << "C++: AutoUpdates::VerifyUpdates: Wrong data returned from server or Proxy HTML error!";
        emit signalAutoUpdateFail(GAPI::DownloadFailed);
        return false;
    }

    //Parse local version into PteidVersion
    PteidVersion local_version;
    local_version.major = list1.at(0).toInt();
    local_version.minor = list1.at(1).toInt();
    local_version.release = list1.at(2).toInt();

    //Parse remote version into PteidVersion
    PteidVersion remote_version;
    remote_version.major = list2.at(0).toInt();
    remote_version.minor = list2.at(1).toInt();
    remote_version.release = list2.at(2).toInt();

    qDebug() << "local_version:" << QString::number(local_version.major) << QString::number(local_version.minor) << QString::number(local_version.release);
    qDebug() << "remote_version:" << QString::number(remote_version.major) << QString::number(remote_version.minor) << QString::number(remote_version.release);

    if (compareVersions(local_version, remote_version) > 0)
    {
        qDebug() << "C++: updates available";
        distrover = VerifyOS("distro");
        archver = VerifyOS("arch");
        ChooseVersion(distrover, archver);
        return true;
    }
    else {
        qDebug() << "C++: No updates available at the moment";
        emit signalAutoUpdateFail(GAPI::NoUpdatesAvailable);
        return false;
    }

}
std::string AppController::VerifyOS(std::string param)
{
    std::string distrostr;
    std::string archstr;
    QRegExp rx;
    QStringList list;
    QString content;

    if( QSysInfo::WordSize == 64 )
        archstr = "x86_64";
    else
        archstr = "i386";

#ifdef WIN32
    distrostr = "windows";

#elif __APPLE__
    distrostr = "osx";
#else

    QFile osFile("/etc/os-release");
    if (!osFile.exists())
    {
        qDebug() << "Not Linux or too old distro!";
        distrostr = "unsupported";
        goto done;
    }

    if (!osFile.open(QFile::ReadOnly | QFile::Text))
        goto done;

    rx = QRegExp("NAME=\"(.*)\"");
    content = osFile.readAll();

    rx.setMinimal(true);

    rx.indexIn(content);

    list = rx.capturedTexts();

    if (list.size() > 1)
    {
        distrostr = list.at(1).toStdString();
    }

#endif

done:
    if (param == "distro")
        return distrostr;
    else
        return archstr;
}
void AppController::ChooseVersion(std::string distro, std::string arch)
{
    qDebug() << "C++:ChooseVersion";

    std::string downloadurl;

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_URL);
    std::string configurl = config.getString();

    if (configurl.empty())
        downloadurl.append(serverurl);
    else
        downloadurl.append(configurl);

#ifdef WIN32
    if (arch == "i386")
    {
        downloadurl.append(WINDOWS32);
        updateWindows(downloadurl, distro);
    } else {
        downloadurl.append(WINDOWS64);
        updateWindows(downloadurl, distro);
    }
#elif __APPLE__

    downloadurl.append(MAC_OS);
    updateWindows(downloadurl, distro);
#else

    if (distro == "unsupported")
    {
       qDebug() << "C++: Your Linux distribution is not supported by Auto-updates";
       emit signalAutoUpdateFail(GAPI::LinuxNotSupported);
    }

    //Name of the deb/rpm will be distro specific

    if (arch == "x86_64")
    {
        if (distro == "debian")
        {
            downloadurl.append(DEBIAN64);
            updateWindows(downloadurl, distro);
        }
        if (distro == "Ubuntu" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU64);
            updateWindows(downloadurl, distro);
        }
        else if (distro == "fedora")
        {
            downloadurl.append(FEDORA64);
            updateWindows(downloadurl, distro);
        }
        else if (distro == "suse")
        {
                downloadurl.append(SUSE64);
                updateWindows(downloadurl, distro);
        }
        else if (distro == "mandriva")
        {
                downloadurl.append(MANDRIVA64);
                updateWindows(downloadurl, distro);
        }

    } else {
        //32bits
        if (distro == "debian")
        {
            downloadurl.append(DEBIAN32);
            updateWindows(downloadurl, distro);
        }
        if (distro == "Ubuntu" || distro == "CaixaMagica")
        {
            downloadurl.append(UBUNTU32);
            updateWindows(downloadurl, distro);
        }
        else if (distro == "fedora")
        {
            downloadurl.append(FEDORA32);
            updateWindows(downloadurl, distro);
        }
        else if (distro == "suse")
        {
                downloadurl.append(SUSE32);
                updateWindows(downloadurl, distro);
        }
        else if (distro == "mandriva")
        {
                downloadurl.append(MANDRIVA32);
                updateWindows(downloadurl, distro);
        }
    }
#endif
}

void AppController::updateWindows(std::string uri, std::string distro)
{
    qDebug() << "C++: There are updates available press Install do perform the updates.";
    urli = uri;
    getdistro = distro;

    emit signalAutoUpdateAvailable();
}

void AppController::cancelDownload()
{
    qDebug() << "C++: cancelDownload";
    httpRequestAborted = true;
}

void AppController::cancelUpdateDownload()
{
    qDebug() << "C++: cancelUpdateDownload";
    httpUpdateRequestAborted = true;
    reply->abort();
    emit signalAutoUpdateFail(GAPI::DownloadFailed);
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
bool AppController::getStartAutoValue (void){

    return m_Settings.getAutoStartup();
}
void AppController::setStartAutoValue (bool bAutoStartup ){

    m_Settings.setAutoStartup(bAutoStartup);
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
bool AppController::getShowNotificationValue (void){

    return m_Settings.getShowNotification();
}
bool AppController::getShowPictureValue (void){

    return m_Settings.getShowPicture();
}
bool AppController::getShowAnimationsValue(void){
    return m_Settings.getShowAnimations();
}
bool AppController::getRegCertValue (void){

    return m_Settings.getRegCert();
}
bool AppController::getRemoveCertValue (void){

    return m_Settings.getRemoveCert();
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
void AppController::setRegCertValue (bool bRegCert){

    m_Settings.setRegCert(bRegCert);
}
void AppController::setRemoveCertValue (bool bRemoveCert){

    m_Settings.setRemoveCert(bRemoveCert);
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
