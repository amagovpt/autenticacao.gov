/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "autoUpdates.h"

#include <QObject>
#include <QCursor>
#include <QDebug>
#include <QtConcurrent>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <time.h>

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "Util.h"
#include "Settings.h"

#include "appcontroller.h"
#include "gapi.h"

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

//Authorization functions for Certificates update
#ifdef __APPLE__
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#endif

#include "proxyinfo.h"

#define N_RELEASE_NOTES 3

struct PteidVersion
{
    int major;
    int minor;
    int release;
};

using namespace eIDMW;

/*
  AutoUpdates implementation for eidguiV2
*/

void AutoUpdates::initRequest(int updateType){
    qDebug() << "C++ AUTO UPDATES: initRequest updateType = " << updateType;

    m_updateType=updateType;

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ AUTO UPDATES: startRequest No Internet Connection";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::startRequest: No Internet Connection.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
        return;
    }

    // Auto update App
    std::string remoteversion;
    if(m_updateType == GAPI::AutoUpdateApp){
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_VERIFY_URL);
        remoteversion.append(config.getString());
        remoteversion.append("version.json");
    }
    else if(m_updateType == GAPI::AutoUpdateCerts){
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_CERTS_URL);
        remoteversion.append(config.getString());
        remoteversion.append("certs.json");
    } else {
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_NEWS_URL);
        remoteversion.append(config.getString());
        remoteversion.append("news.json");
    }

    url = remoteversion.c_str();

    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();
    if (fileName.isEmpty())
    {
         getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
         return;
    }
    startRequest(url);
}

void AutoUpdates::startRequest(QUrl url) {
    qDebug() << "C++ AUTO UPDATES: startRequest: " << url;

    file = NULL;
    filedata.clear();
    httpRequestAborted = false;

    //allow up to 1 minutes to fetch remote version
    int download_duration = 60000;

    ProxyInfo proxyinfo;
    proxy = QNetworkProxy(); // reset Proxy

    if (proxyinfo.isAutoConfig())
    {
        std::string proxy_host;
        long proxy_port;
        proxyinfo.getProxyForHost(url.toString().toUtf8().constData(), &proxy_host, &proxy_port);
        if (proxy_host.size() > 0)
        {
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(QString::fromStdString(proxy_host));
            proxy.setPort(proxy_port);
        }
    }
    else if (proxyinfo.isManualConfig())
    {
        long proxyinfo_port;
        try {
            proxyinfo_port = std::stol(proxyinfo.getProxyPort());
        }
        catch (...) {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing proxy port to number value.");
        }
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(QString::fromStdString(proxyinfo.getProxyHost()));
        proxy.setPort(proxyinfo_port);
        if (proxyinfo.getProxyUser().size() > 0) {
            proxy.setUser(QString::fromStdString(proxyinfo.getProxyUser()));
            proxy.setPassword(QString::fromStdString(proxyinfo.getProxyPwd()));
        }
    }
    QNetworkProxy::setApplicationProxy(proxy);

    reply = qnam.get(QNetworkRequest(url));
    QTimer::singleShot(download_duration, this, SLOT(cancelDownload()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(httpError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress()));
    return;
}

void AutoUpdates::VerifyCertsUpdates(std::string filedata)
{
    qDebug() << "C++ AUTO UPDATES: VerifyCertsUpdates";

    // certs.json parsing
    cJSON *json = cJSON_Parse(filedata.c_str());
    if (!cJSON_IsObject(json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: may be a syntax error.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    cJSON *certs_json = cJSON_GetObjectItem(json, "certs");
    if (!cJSON_IsObject(certs_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: Could not get certs object.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    ChooseCertificates(certs_json);
}

void AutoUpdates::VerifyNewsUpdates(std::string filedata)
{
    qDebug() << "C++ AUTO UPDATES: VerifyNewsUpdates";

    parseNews(filedata);
    m_news = ChooseNews(); // Filter active news

    //should be only one
    if(m_news.size() > 0)
    {
        NewsEntry selectedEntry = m_news.at(0);
        // if there are more than one active news entry select highest id
        for(size_t i = 1; i < m_news.size(); i++)
        {
            NewsEntry entry = m_news.at(i);
            if(std::stoi(entry.id) == std::stoi(selectedEntry.id)){
                PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: ID duplicate.");
                getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
                return;
            }

            if(std::stoi(entry.id) > std::stoi(selectedEntry.id))
                selectedEntry = entry;
        }

        if(getAppController()->isToShowNews(QString::fromStdString(selectedEntry.id)))
        {
            qDebug() << "C++: AutoUpdates ChooseNews: active news entry with id: " << QString::fromStdString(selectedEntry.id);
            m_newsTitle = QString::fromStdString(selectedEntry.title);
            m_newsBody = QString::fromStdString(selectedEntry.text);
            m_newsUrl = QString::fromStdString(selectedEntry.link);
            m_newsId = QString::fromStdString(selectedEntry.id);
            updateWindows();
            return;
        }
    }
    getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NoUpdatesAvailable);
}

QString AutoUpdates::getActiveNewsId(void)
{
    return m_newsId;
}

time_t getTimeFromString(std::string stringTime){
    // expects time string to be of format: YYYY-MM-DD
    std::stringstream stream(stringTime);
    std::string segment;
    std::vector<std::string> splitTime;

    while(std::getline(stream, segment, '-'))
    {
       splitTime.push_back(segment);
    }

    int year = std::stoi(splitTime.at(0));
    int month = std::stoi(splitTime.at(1));
    int day = std::stoi(splitTime.at(2));

    struct tm  tm;
    time_t rawtime;
    time ( &rawtime );
    tm = *localtime ( &rawtime );
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    return mktime(&tm);
}

std::vector<NewsEntry> AutoUpdates::ChooseNews()
{
    qDebug() << "C++ AUTO UPDATES: ChooseNews";

    //TODO: check if already read news -> if entry.id in file of read news
    std::vector<NewsEntry> filteredNews;
    NewsEntry entry;
    std::string firstString;
    std::string lastString;
    time_t firstTime;
    time_t lastTime;
    time_t currentTime = time(0);

    // Filter active news
    for (size_t i = 0; i < m_news.size(); i++)
    {
        entry = m_news.at(i);
        firstString = entry.first_day;
        lastString = entry.last_day;

        firstTime = getTimeFromString(firstString);
        lastTime = getTimeFromString(lastString);

        if (difftime(currentTime,firstTime) >= 0 && difftime(lastTime,currentTime) >= 0)
        {
            filteredNews.push_back(entry);
        }
    }

    return filteredNews;
}

void AutoUpdates::parseNews(std::string data){
    // parses news.json file into vector of NewsEntry's
    cJSON *json = cJSON_Parse(data.c_str());
    if (!cJSON_IsObject(json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: may be a syntax error.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    cJSON *news_json = cJSON_GetObjectItem(json, "news");
    if (!cJSON_IsArray(news_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing news.json: Could not get news array.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    for (int i = 0; i < cJSON_GetArraySize(news_json); i++)
    {
        NewsEntry newsEntry;

        cJSON *new_json = cJSON_GetArrayItem(news_json, i);
        if (!cJSON_IsObject(new_json))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get id
        cJSON *new_jsonID = cJSON_GetObjectItem(new_json, "id");
        if (!cJSON_IsString(new_jsonID))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: Could not get id on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get title
        cJSON *new_jsonTitle = cJSON_GetObjectItem(new_json, "title");
        if (!cJSON_IsString(new_jsonTitle))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: Could not get title on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get begin date as string
        cJSON *new_jsonBegin = cJSON_GetObjectItem(new_json, "first_day");
        if (!cJSON_IsString(new_jsonBegin))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: Could not get begin date on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get end date as string
        cJSON *new_jsonEnd = cJSON_GetObjectItem(new_json, "last_day");
        if (!cJSON_IsString(new_jsonEnd))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: Could not get end date on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get text as string
        cJSON *new_jsonText = cJSON_GetObjectItem(new_json, "text");
        if (!cJSON_IsString(new_jsonText))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "Error parsing news.json: Could not get text on news entry number %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        //get link as string. Link is optional
        cJSON *new_jsonLink = cJSON_GetObjectItem(new_json, "link");
        if (new_jsonLink != NULL && !cJSON_IsString(new_jsonLink))
        {
          PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
              "Error parsing news.json: Could not get link on news entry number %d", i);
          getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
          return;
        }

        newsEntry.id = std::string(new_jsonID->valuestring);
        newsEntry.title = std::string(new_jsonTitle->valuestring);
        newsEntry.first_day = std::string(new_jsonBegin->valuestring);
        newsEntry.last_day = std::string(new_jsonEnd->valuestring);
        newsEntry.text = std::string(new_jsonText->valuestring);
        if(new_jsonLink)
            newsEntry.link = std::string(new_jsonLink->valuestring);
        else
            newsEntry.link = "";

        m_news.push_back(newsEntry);
    }
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

void AutoUpdates::VerifyAppUpdates(std::string filedata)
{
    qDebug() << "C++ AUTO UPDATES: VerifyAppUpdates";

    std::string distrover = VerifyOS("distro");
    std::string archver = VerifyOS("arch");

    // installed version
    QString ver (WIN_GUI_VERSION_STRING);
    QStringList list1 = ver.split(",");
    installed_version = list1.at(0) + '.' + list1.at(1) + '.' + list1.at(2);

    // version.json parsing
    cJSON *json = cJSON_Parse(filedata.c_str());
    if (!cJSON_IsObject(json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: may be a syntax error.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    cJSON *dists_json = cJSON_GetObjectItem(json, "distributions");
    if (!cJSON_IsObject(dists_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get distributions object.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    cJSON *dist_json = cJSON_GetObjectItem(dists_json, distrover.c_str());
    if (!cJSON_IsObject(dist_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get object for this distribution (%s)", distrover.c_str());
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }

    cJSON *latestVersion_json = cJSON_GetObjectItem(dist_json, "latestVersion");
    if (!cJSON_IsString(latestVersion_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get latestVersion for this distribution.", distrover.c_str());
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }
    remote_version = latestVersion_json->valuestring;

    QStringList list2 = remote_version.split(".");
    if (list2.size() < 3)
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::VerifyAppUpdates: Wrong data returned from server or Proxy HTML error!");
        qDebug() << "C++ AUTO UPDATES VerifyUpdates: Wrong data returned from server or Proxy HTML error!";
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
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

    if (compareVersions(local_version, remote_version) <= 0)
    {
        qDebug() << "C++ AUTO UPDATES: No App updates available at the moment";
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NoUpdatesAvailable);
        return;
    }
    qDebug() << "C++ AUTO UPDATES: app updates available";

    cJSON *versions_array_json = cJSON_GetObjectItem(json, "versions");
    if (!cJSON_IsArray(versions_array_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get array of versions.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }
    int latestVerIdx;
    for (latestVerIdx = 0; latestVerIdx < cJSON_GetArraySize(versions_array_json); latestVerIdx++)
    {
        cJSON *version_json = cJSON_GetArrayItem(versions_array_json, latestVerIdx);
        if (!cJSON_IsObject(version_json))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get version object at index %d", latestVerIdx);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }
        cJSON *version_number_json = cJSON_GetObjectItem(version_json, "version");
        if (!cJSON_IsString(version_number_json))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get version number from version at %d", latestVerIdx);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }
        if (strcmp(version_number_json->valuestring, latestVersion_json->valuestring) == 0)
            break;
    }

    // build release notes string from the previous N_RELEASE_NOTES release notes
    release_notes = "<h2> Notas de Lançamento</h2>"; //init and clean previous content in case it failed
    for (int i = latestVerIdx; i < (std::min)(cJSON_GetArraySize(versions_array_json),latestVerIdx + N_RELEASE_NOTES); i++)
    {
        cJSON *version_json = cJSON_GetArrayItem(versions_array_json, i);
        cJSON *version_number_json = cJSON_GetObjectItem(version_json, "version");
        cJSON *version_date_json = cJSON_GetObjectItem(version_json, "date");
        cJSON *version_release_notes_json = cJSON_GetObjectItem(version_json, "release_notes");
        if (!cJSON_IsObject(version_json)
         || !cJSON_IsString(version_number_json)
         || !cJSON_IsString(version_date_json)
         || !cJSON_IsArray(version_release_notes_json))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not parse some key/value of version object at %d", i);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }
        release_notes += "<p>";
        release_notes += QString("<h3>Versão estável ") + version_number_json->valuestring + QString(":</h3>");
        release_notes += QString("- Data de lançamento: ") + version_date_json->valuestring + QString("<br/>");
        for (int rlsNoteIdx = 0; rlsNoteIdx < cJSON_GetArraySize(version_release_notes_json); rlsNoteIdx++)
        {
            cJSON *release_note_json = cJSON_GetArrayItem(version_release_notes_json, rlsNoteIdx);
            if (!cJSON_IsString(release_note_json))
            {
                PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not parse release note %d of version obj %d", rlsNoteIdx, i);
                getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
                return;
            }
            release_notes += QString("- ") + release_note_json->valuestring + QString("<br/>");
        }
        release_notes += "</p>";
    }

    qDebug() << release_notes;

    ChooseAppVersion(distrover, archver, dist_json);
}

std::string AutoUpdates::VerifyOS(std::string param)
{
    std::string distrostr;
    std::string archstr;
    QRegExp rx;
    QStringList list;
    QString content;
    QFile osFile("/etc/os-release");

    if( QSysInfo::WordSize == 64 )
        archstr = "x86_64";
    else
        archstr = "i386";
    if (param == "arch")
    {
        goto done;
    }

#ifdef WIN32
    distrostr = "windows";
    distrostr += QSysInfo::WordSize == 64 ? "64" : "32";
#elif __APPLE__
    distrostr = "osx";
#else


    if (!osFile.exists())
    {
        qDebug() << "Not Linux or too old distro!";
        distrostr = "unsupported";
        goto done;
    }

    if (!osFile.open(QFile::ReadOnly | QFile::Text))
        goto done;

    rx = QRegExp("NAME=\"?(\\w+)\"?");
    content = osFile.readAll();
    rx.indexIn(content);
    list = rx.capturedTexts();
    if (list.size() > 1)
    {
        distrostr = list.at(1).toStdString();
    }

    //Normalize distro string to lowercase
    std::transform(distrostr.begin(), distrostr.end(), distrostr.begin(), ::tolower);

    if (distrostr == "ubuntu") // distinguish ubuntu16, ubuntu18, ...
    {
        rx = QRegExp("VERSION_ID=\"(\\d{2}.\\d{2})\"");
        rx.setMinimal(true);
        rx.indexIn(content);
        list = rx.capturedTexts();
        if (list.size() > 1)
        {
            std::string ubuntuVersion = list.at(1).toStdString();
            std::size_t pos = ubuntuVersion.find(".");
            if (pos != std::string::npos)
            {
                ubuntuVersion = ubuntuVersion.substr(0, pos);
            }
            distrostr += ubuntuVersion;
            qDebug() << "Ubuntu version: " << distrostr.c_str();
        }
    }
    else
    {
        // os-release name for openSUSE can have more characters such as version name
        if (distrostr.find("opensuse") != std::string::npos)
        {
            distrostr = "suse";
        }

        distrostr += QSysInfo::WordSize == 64 ? "64" : "32";
    }
#endif

done:
    if (param == "distro")
        return distrostr;
    else
        return archstr;
}

void AutoUpdates::ChooseAppVersion(std::string distro, std::string arch, cJSON *dist_json)
{
    qDebug() << "C++ AUTO UPDATES: ChooseAppVersion";

    std::string downloadurl;

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_AUTOUPDATES_URL);
    std::string configurl = config.getString();
    downloadurl.append(configurl);

#ifdef WIN32

#elif __APPLE__

#else

    if (distro == "unsupported")
    {
       qDebug() << "C++ AUTO UPDATES: Your Linux distribution is not supported by Auto-updates";
       getAppController()->signalAutoUpdateFail(m_updateType, GAPI::LinuxNotSupported);
       return;
    }
#endif
    //Name of the msi/deb/rpm will be distro specific
    cJSON *package_json = cJSON_GetObjectItem(dist_json, "package");
    if (!cJSON_IsString(package_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing version.json: Could not get package_json for this distribution.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
        return;
    }
    downloadurl.append(package_json->valuestring);
    urlList.clear();
    urlList.append(QString::fromStdString(downloadurl));
    getdistro = distro;
    updateWindows();
    return;
}

void AutoUpdates::ChooseCertificates(cJSON *certs_json)
{
    qDebug() << "C++ AUTO UPDATES: ChooseCertificates";

    urlList.clear();
    hashList.clear();
    fileNames.clear();
    fileIdx = 0;
    std::string downloadurl;

    eIDMW::PTEID_Config configCert(eIDMW::PTEID_PARAM_AUTOUPDATES_CERTS_URL);
    std::string configurl = configCert.getString();

    int certIdx;
    QString cert;

    eIDMW::PTEID_Config certs_dir(eIDMW::PTEID_PARAM_GENERAL_CERTS_DIR);
    std::string  certs_dir_str = certs_dir.getString();
    qDebug() << QString::fromUtf8(certs_dir_str.c_str());

    for (certIdx = 0; certIdx < cJSON_GetArraySize(certs_json); certIdx++)
    {
        cJSON *cert_json = cJSON_GetArrayItem(certs_json, certIdx);
        if (!cJSON_IsString(cert_json))
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: Could not get cert index = %d", certIdx);
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
            return;
        }

        std::string  file_name_temp;
        file_name_temp.append(certs_dir_str);
        file_name_temp.append("/");
        file_name_temp.append(cert_json->string);

#ifdef WIN32
        QDir dir(QString::fromLatin1(certs_dir_str.c_str()));
#else
        QDir dir(QString::fromStdString(certs_dir_str));
#endif
        if (!dir.exists())
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "AutoUpdates::RunCertsPackage: Certs dir do not exist! %s",certs_dir.getString());
            qDebug() << "C++: AutoUpdates::RunCertsPackage: Certs dir do not exist!";
            getAppController()->signalAutoUpdateFail(m_updateType,GAPI::InstallFailed);
            return;
        }
        if (!dir.isReadable())
        {
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "AutoUpdates::RunCertsPackage: Certs dir not have read permissions! %s",certs_dir.getString());
            qDebug() << "C++: AutoUpdates::RunCertsPackage: Certs dir not have read permissions!";
            getAppController()->signalAutoUpdateFail(m_updateType,GAPI::InstallFailed);
            return;
        }
#ifdef WIN32
        if(QFile::exists(QString::fromLatin1(file_name_temp.c_str()))
                && validateHash(QString::fromLatin1(file_name_temp.c_str()),
                                QString::fromStdString(cert_json->valuestring))){
#else
        if(QFile::exists(QString::fromStdString(file_name_temp))
                && validateHash(QString::fromStdString(file_name_temp),
                                QString::fromStdString(cert_json->valuestring))){
#endif
            qDebug() << "Cert exists: " << QString::fromUtf8(file_name_temp.c_str());
        } else{
            qDebug() << "Cert does not exist or invalid: " << QString::fromUtf8(file_name_temp.c_str());

            downloadurl.append(configurl);
            downloadurl.append(cert_json->string);

            qDebug() << "downloadurl : " << QString::fromUtf8(downloadurl.c_str());
            urlList.append(QString::fromStdString(downloadurl));
            hashList.append(QString::fromStdString(cert_json->valuestring));
            downloadurl.clear();
        }
    }
    if(urlList.length() > 0 && hashList.length() > 0){
        updateWindows();
    } else {
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NoUpdatesAvailable);
    }
}

void AutoUpdates::updateWindows()
{
    qDebug() << "C++ AUTO UPDATES: There are updates available";

    if(m_updateType == GAPI::AutoUpdateApp){
        // Show popup about app update
                getAppController()->signalAutoUpdateAvailable(
            m_updateType, release_notes, installed_version, remote_version, urlList.at(0));
    }
    else if(m_updateType == GAPI::AutoUpdateCerts){
#ifdef WIN32
        // Start update certs automatically
        getAppController()->startUpdateCerts();
#else
        // Show popup about certs update because the root password is needed.
        getAppController()->signalAutoUpdateAvailable(
            m_updateType, "", "", "", getCertListAsString());
#endif
    }
    else if(m_updateType == GAPI::AutoUpdateNews){
        // Show popup about news
        getAppController()->signalAutoUpdateAvailable(
            m_updateType, m_newsTitle, m_newsBody,"", m_newsUrl);
    }
}

void AutoUpdates::startUpdate()
{
    url = urlList.at(fileIdx);
    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();
    fileNames.append(fileName);

    if (fileName.isEmpty())
    {
         getAppController()->signalAutoUpdateFail(m_updateType, GAPI::GenericError);
    }
    QFile::remove(fileName);

    std::string tmpfile;
    tmpfile.append(QDir::tempPath().toStdString());
    tmpfile.append("/");
    tmpfile.append(fileName.toStdString());

    file = new QFile(QString::fromUtf8((tmpfile.c_str())));
    if (!file->open(QIODevice::WriteOnly)) {
       qDebug() << "C++ AUTO UPDATES: Unable to save the file";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
           "AutoUpdates::startUpdate: Unable to save the file.");
        delete file;
        file = nullptr;
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::UnableSaveFile);
        return;
    }

    getAppController()->signalStartUpdate(m_updateType, fileName);

    // schedule the request
    httpUpdateRequestAborted = false;
    startUpdateRequest(url);
}

void AutoUpdates::startUpdateRequest(QUrl url){
    qDebug() << "C++ AUTO UPDATES: startUpdateRequest";

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ AUTO UPDATES: startUpdateRequest No Internet Connection";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::startUpdateRequest: No Internet Connection.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
        return;
    }

    int download_duration = 300000;
    reply = qnam.get(QNetworkRequest(url));
    QTimer::singleShot(download_duration, this, SLOT(cancelUpdateDownload()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(httpUpdateError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpUpdateFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpUpdateReadyRead()));
    // TODO : For now the Autenticacao.gov server do not send content length header.
    //        So we are using a indeterminate progress bar.
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateUpdateDataReadProgress(/*qint64,qint64*/)));
}

void AutoUpdates::RunAppPackage(std::string pkg, std::string distro){
    qDebug() << "C++ AUTO UPDATES: RunAppPackage";

    std::string pkgpath;
    // TODO: test with Ubunto < 17

    pkgpath.append(QDir::tempPath().toStdString());
    pkgpath.append("/");
    pkgpath.append(pkg);

    qDebug() << QString::fromStdString("pkgpath " + pkgpath);

#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si,sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstring msi_path = QDir::toNativeSeparators(QString::fromStdString(pkgpath)).toStdWString();

    qDebug() << QString::fromStdString("msi package path");
    qDebug() << QString::fromStdWString(msi_path);

    std::wstring log_file = QDir::toNativeSeparators(QString::fromStdString(QDir::tempPath().toStdString() + "\\Pteid-MSI.log")).toStdWString();

    qDebug() << QString::fromStdString("log file path");
    qDebug() << QString::fromStdWString(log_file);

    //Prepare CreateProcess args
    std::wstring installer_app = L"C:\\Windows\\system32\\msiexec.exe";
    std::wstring logging_level = L" /L*v ";
    std::wstring msi_args = L" /i ";

    std::wstring path = installer_app;
    path.append(msi_args);

    //add double quotes for paths
    path.append(L"\"");
    path.append(msi_path);
    path.append(L"\"");

    path.append(logging_level);

    //add double quotes for paths
    path.append(L"\"");
    path.append(log_file);
    path.append(L"\"");

    //guarantees null-terminating string
    path.push_back(0);

    LPWSTR path_pointer = (wchar_t *) path.c_str();

    if (!CreateProcess(NULL, path_pointer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)){
        qDebug() << "autoUpdate process failed to start";
        qDebug() << QString::fromStdString("Error: " + GetLastError());
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AppController::RunPackage: Failed to start update process: %d.", GetLastError());
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::InstallFailed);
    } else {
        PTEID_ReleaseSDK();
        exit(0);
    }

#elif __APPLE__
    // This launches the GUI installation process, the user has to follow the wizard to actually perform the installation
    execl("/usr/bin/open", "open", pkgpath.c_str(), NULL);
#else

    //Normalize distro string to lowercase
    std::transform(distro.begin(), distro.end(), distro.begin(), ::tolower);

    std::cout << "pkgpath " << pkgpath << " distro " << distro << std::endl;

    if (distro.substr(0, 6) == "ubuntu")
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
    qDebug() << "C++ AUTO UPDATES: RunPackage finish";
}

#ifdef __APPLE__

/* Helper function for the helper function :) */
QByteArray readOriginalFile(QString &path) {
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

   //Create AuthorizationItem for the dialog

    OSStatus status = AuthorizationCreate(
      NULL,
      kAuthorizationEmptyEnvironment,
      kAuthorizationFlagDefaults,
      &authorization_ref);

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

   //Create the file if needed and open for writing
   snprintf(command_buf, sizeof(command_buf), "/usr/libexec/authopen -extauth -c -w %s", destination_full.toUtf8().data());

   FILE * destination_pipe = popen(command_buf, "w");

   if (destination_pipe == NULL) {
       PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "copyFileWithAuthorization: Failed to open pipe to 'authopen' (%d - %s)", errno, strerror(errno));
   }
   else {

        fwrite(form.bytes, kAuthorizationExternalFormLength, 1, destination_pipe);
        fflush(destination_pipe);

        QByteArray file_content = readOriginalFile(cert_filepath);

        if (file_content.size() == 0) {
            return false;
        }

        fwrite(file_content.data(), file_content.size(), 1, destination_pipe);
        fflush(destination_pipe);

        //Close the pipe and wait for the authopen process to finish
        int rc = pclose(destination_pipe);

        if (rc != 0) {
           PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "copyFileWithAuthorization: authopen process exited with error code %d", rc);
        }

        return rc == 0;

   }

}


#endif

void AutoUpdates::RunCertsPackage(QStringList certs) {
    qDebug() << "C++ AUTO UPDATES: RunCertsPackage filename";

    eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_CERTS_DIR);
    std::string certs_dir_std = config.getString();

#ifdef WIN32
    QString  certs_dir_str = QString::fromLatin1(certs_dir_std.c_str());
#else
    QString  certs_dir_str = QString::fromStdString(certs_dir_std);
#endif

    bool bUpdateCertsSuccess = false;
    bool bHaveFilesToCopy = false;

#ifdef WIN32
    certs_dir_str.append("\\");

    QString copySourceDir = QDir::tempPath() + "/";
    QString copyTargetDir = certs_dir_str;
    QString certificateFileName;
    if(certs.length() > 0){
        bUpdateCertsSuccess = true;
        for (int i = 0; i < certs.length(); i++){
            certificateFileName = certs.at(i);
            if (validateHash(copySourceDir + certificateFileName, hashList.at(i)) == false
                || QFile::copy(copySourceDir + certificateFileName, copyTargetDir + certificateFileName) == false)
            {
                bUpdateCertsSuccess = false; // Keep install another files but show popup with error message
                PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                    "AutoUpdates::RunCertsPackage: Cannot copy file: %s", certificateFileName.toStdString().c_str());
            }
            else {
                PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
                    "AutoUpdates::RunCertsPackage: Copy file success: %s", certificateFileName.toStdString().c_str());
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
           
            }
            else {
                bUpdateCertsSuccess = false;
                //Hash failure just abort because we don't want to support partially correct updates
                 PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                    "AutoUpdates::RunCertsPackage: invalid hash for downloaded file %s",
                                                 certificate_path.toStdString().c_str());
                
                break;
            }

        }
    }

#else
    QString filesToCopy;
    if (certs.length() > 0){
        bUpdateCertsSuccess = true;
        for (int i = 0; i < certs.length(); i++){
            QString certificateFileName;
            certificateFileName = QDir::tempPath() + "/" + certs.at(i);
            if(validateHash(certificateFileName, hashList.at(i))){
                    bHaveFilesToCopy= true;
                    filesToCopy.append(QDir::tempPath());
                    filesToCopy.append("/");
                    filesToCopy.append(certs.at(i));
                    filesToCopy.append(" ");
            } else {
                    bUpdateCertsSuccess = false;
            }
        }
    }

    if(bHaveFilesToCopy){
        QProcess *proc = new QProcess(this);
        proc->waitForFinished();

        QString cmd = "pkexec /bin/cp " + filesToCopy + certs_dir_str;
        proc->start(cmd);

        if(!proc->waitForStarted()) //default wait time 30 sec
        {
            qDebug() << "C++ AUTO UPDATES: cannot start process ";
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                      "AutoUpdates::RunCertsPackage: Cannot execute: %s",cmd.toStdString().c_str());
            bUpdateCertsSuccess = false;
        } else {

            proc->waitForFinished();
            proc->setProcessChannelMode(QProcess::MergedChannels);

            if(proc->exitStatus() == QProcess::NormalExit
                    && proc->exitCode() == QProcess::NormalExit){
                PTEID_LOG(PTEID_LOG_LEVEL_CRITICAL, "eidgui",
                          "AutoUpdates::RunCertsPackage: Copy file(s) success: %s",cmd.toStdString().c_str());
            } else {
                bUpdateCertsSuccess = false;
                PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                          "AutoUpdates::RunCertsPackage: Cannot copy file(s): %s",cmd.toStdString().c_str());
            }
        }
    }
#endif

    if(bUpdateCertsSuccess){
        getAppController()->signalAutoUpdateSuccess(m_updateType);
        getAppController()->updateCertslog();
    } else {
        getAppController()->signalAutoUpdateFail(m_updateType,GAPI::InstallFailed);
    }
    qDebug() << "C++: RunCertsPackage finish";
}

bool AutoUpdates::validateHash(QString certPath, QString hashString){

    QFile f(certPath);
    if (!f.open(QFile::ReadOnly)){
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                  "AutoUpdates::validateHash: Error Reading Certificate file :%s",
                  certPath.toStdString().c_str());
        return false;
    }

    QTextStream in(&f);

    QByteArray DataFile = f.readAll();

    QString hash = QString(QCryptographicHash::hash((DataFile),QCryptographicHash::Sha256).toHex());

    if (QString::compare(hash, hashString, Qt::CaseInsensitive)){
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::validateHash: Certificate invalid: %s %s %s ",
                  certPath.toStdString().c_str(),hashString.toStdString().c_str(),hash.toStdString().c_str());
        return false;
    }
    else {
        return true;
    }
}

void AutoUpdates::userCancelledUpdateDownload()
{
    qDebug() << "C++ AUTO UPDATES: userCanceledUpdateDownload";

    httpUpdateRequestAborted = true;
    userCanceled = true;

    if (reply != NULL){
        reply->abort();
    }

    getAppController()->signalAutoUpdateFail(m_updateType, GAPI::DownloadCancelled);
}

/*
 * Slots to process the configuration file
 */
void AutoUpdates::cancelDownload()
{
    qDebug() << "C++ AUTO UPDATES: cancelDownload";
    httpRequestAborted = true;

    if (reply != NULL){
        reply->abort();
    }

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ AUTO UPDATES: cancelDownload No Internet Connection";

        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
    }
    return;
}

void AutoUpdates::httpError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "C++ AUTO UPDATES type = " << m_updateType <<  "httpError " << networkError;
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
        "AutoUpdates type=%d httpError QNetworkReply::NetworkError = %d", m_updateType, networkError);

    switch(networkError){
        case QNetworkReply::NetworkError::NoError:
            //no error hence do nothing
            break;
        default:
            cancelDownload();
            break;
    }
    return;
}

void AutoUpdates::httpFinished()
{
    qDebug() << "C++ AUTO UPDATES: httpFinished httpRequestAborted = " << httpRequestAborted;
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        reply->deleteLater();
        reply = 0;
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "AutoUpdates::httpFinished: The update request was aborted.");
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qDebug() << "C++ AUTO UPDATES: reply error";
        file->remove();
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
        QString strLog = QString("AutoUpdates:: Download failed: ");
        strLog += reply->url().toString();
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", strLog.toStdString().c_str() );
        return;
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = url.resolved(redirectionTarget.toUrl());
        // TODO: ask about redirect.
        {
            url = newUrl;
            reply->deleteLater();
            filedata.clear();
            startRequest(url);
            return;
        }
    }
    qDebug() << "C++ AUTO UPDATES: httpFinished";
    if(m_updateType == GAPI::AutoUpdateApp){
        VerifyAppUpdates(filedata);
    }
    else if(m_updateType == GAPI::AutoUpdateCerts){
        VerifyCertsUpdates(filedata);
    } else {
        VerifyNewsUpdates(filedata);
    }
}

void AutoUpdates::httpReadyRead()
{
    qDebug() << "C++ AUTO UPDATES: httpReadyRead";
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply

    QByteArray data = reply->readAll();
    QString qsdata(data);
    //Write to memory
    filedata += qsdata.toStdString();
}

void AutoUpdates::updateDataReadProgress()
{
    if (httpRequestAborted)
        return;
}

 /*
 *  Slots to process the update file
 */

void AutoUpdates::cancelUpdateDownload()
{
    qDebug() << "C++ AUTO UPDATES: cancelUpdateDownload";

    httpUpdateRequestAborted = true;
    userCanceled = false;

    if (reply != NULL){
        reply->abort();
    }
}

void AutoUpdates::httpUpdateError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "C++ AUTO UPDATES: httpUpdateError";
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
        "AutoUpdates::httpUpdateError: QNetworkReply::NetworkError = %d", networkError);

    switch(networkError){
        case QNetworkReply::NetworkError::NoError:
            //no error hence do nothing
            break;
        default:
            cancelUpdateDownload();
            break;
    }
    return;
}

void AutoUpdates::httpUpdateFinished(){
    qDebug() << "C++ AUTO UPDATES: httpUpdateFinished";
    if (httpUpdateRequestAborted)
    {
        if (file)
        {
            file->close();
            file->remove();
            delete file;
            file = 0;
            qDebug() << "C++: httpUpdateRequestAborted";
            PTEID_LOG(PTEID_LOG_LEVEL_WARNING, "eidgui",
                "AutoUpdates::httpUpdateFinished: The update request was aborted.");
        }

        if (!userCanceled){
            //network failure occurred when downloading
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "AutoUpdates::httpUpdateFinished: Network failure occurred while downloading.");
            getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
        }

        reply->deleteLater();
        reply = 0;
        return;
    }
    if (!file){
        reply->deleteLater();
        reply = 0;
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::DownloadFailed);
        return;
    }
    file->flush();
    file->close();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error())
    {
        file->remove();
        getAppController()->signalAutoUpdateFail(m_updateType, GAPI::NetworkError);
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

    if (!reply->error()){
        if(m_updateType == GAPI::AutoUpdateApp){
            RunAppPackage(fileName.toStdString(), getdistro);
        } else{
            if(fileIdx == urlList.length() - 1){
                qDebug() << "Downloads complete";
                RunCertsPackage(fileNames);
            }
            else{
                fileIdx++;
                startUpdate();
                return;
            }
        }
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void AutoUpdates::httpUpdateReadyRead()
{
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());
}

void AutoUpdates::updateUpdateDataReadProgress(/*qint64 bytesRead, qint64 totalBytes*/)
{
    qDebug() << "C++ AUTO UPDATES: updateUpdateDataReadProgress";
    if (httpUpdateRequestAborted)
        return;

    // if(totalBytes > 0)
    //    valueFloat = (quint64)((double) (bytesRead * 100 / totalBytes ));

    getAppController()->signalAutoUpdateProgress(m_updateType/*,(int)valueFloat*/);
}

QString AutoUpdates::getCertListAsString(){
    // returns a string with the names of certificates to be downloaded
    QString stringURLs;
    QUrl url_tmp;
    QFileInfo fileInfo_tmp;
    QString fileName_tmp;
    for(int i = 0; i < urlList.length(); i++){
        url_tmp = urlList.at(i);
        fileInfo_tmp = QFileInfo(url_tmp.path());
        fileName_tmp = fileInfo_tmp.fileName();
        stringURLs.append(fileName_tmp);
        stringURLs.append(" ");
    }
    return stringURLs;
}
