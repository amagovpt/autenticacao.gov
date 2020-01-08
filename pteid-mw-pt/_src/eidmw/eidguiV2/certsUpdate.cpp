/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "certsupdate.h"

#include <QObject>
#include <QCursor>
#include <QDebug>
#include <QtConcurrent>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "Util.h"
#include "Settings.h"

#include "certsupdate.h"
#include "appcontroller.h"
#include "gapi.h"

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"

#include "proxyinfo.h"

using namespace eIDMW;

/*
  CertsUpdate implementation for eidguiV2
*/

void CertsUpdate::startRequest(QUrl url){
    qDebug() << "C++ CERTS UPDATE: startRequest: " << url;

    httpRequestAborted = false;

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ CERTS UPDATE: startRequest No Internet Connection";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "CertsUpdate::startRequest: No Internet Connection.");
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
        return;
    }

    //allow up to 1 minutes to fetch remote version
    int download_duration = 60000;

    ProxyInfo proxyinfo;
    proxy.setType(QNetworkProxy::HttpProxy);
    if (proxyinfo.isAutoConfig())
    {
        std::string proxy_host;
        long proxy_port;
        proxyinfo.getProxyForHost(url.toString().toUtf8().constData(), &proxy_host, &proxy_port);
        if (proxy_host.size() > 0)
        {
            proxy.setHostName(QString::fromStdString(proxy_host));
            proxy.setPort(proxy_port);
        }
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else if (proxyinfo.isManualConfig())
    {
        proxy.setHostName(proxyinfo.getProxyHost());
        proxy.setPort(proxyinfo.getProxyPort().toLong());
        if (proxyinfo.getProxyUser().size() > 0) {
            proxy.setUser(proxyinfo.getProxyUser());
            proxy.setPassword(proxyinfo.getProxyPwd());
        }
        QNetworkProxy::setApplicationProxy(proxy);
    }

    reply = qnam.get(QNetworkRequest(url));
    QTimer::singleShot(download_duration, this, SLOT(cancelDownload()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(httpError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
    return;
}

void CertsUpdate::cancelDownload()
{
    qDebug() << "C++ CERTS UPDATE: cancelDownload";
    httpRequestAborted = true;

    if (reply != NULL){
        reply->abort();
    }

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ CERTS UPDATE: cancelDownload No Internet Connection";

        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
    }
    return;
}
void CertsUpdate::httpError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "C++ CERTS UPDATE: httpError" << networkError;
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
        "CertsUpdate::httpError: QNetworkReply::NetworkError = %d", networkError);

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
void CertsUpdate::httpFinished()
{
    qDebug() << "C++ CERTS UPDATE: httpFinished httpRequestAborted = " << httpRequestAborted;
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
            "CertsUpdate::httpFinished: The update request was aborted.");
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        qDebug() << "C++ CERTS UPDATE: reply error";
        file->remove();
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
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
            file->open(QIODevice::WriteOnly);
            file->resize(0);
            startRequest(url);
            return;
        }
    }
    qDebug() << "C++ CERTS UPDATE: httpFinished";
    VerifyUpdates(filedata);
}

bool CertsUpdate::VerifyUpdates(std::string filedata)
{
    qDebug() << "C++ CERTS UPDATE: VerifyUpdates";

    qDebug() << QString::fromUtf8(filedata.c_str());;

    // certs.json parsing
    cJSON *json = cJSON_Parse(filedata.c_str());
    if (!cJSON_IsObject(json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: may be a syntax error.");
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
        return false;
    }

    cJSON *certs_json = cJSON_GetObjectItem(json, "certs");
    if (!cJSON_IsObject(certs_json))
    {
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Error parsing certs.json: Could not get certs object.");
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
        return false;
    }

    return ChooseVersion(certs_json);
}
bool CertsUpdate::ChooseVersion(cJSON *certs_json)
{
    qDebug() << "C++ CERTS UPDATE: ChooseVersion";

    std::string downloadurl;

    // TODO: Get certs.json from
    // https://github.com/amagovpt/autenticacao.gov/tree/master/pteid-mw-pt/_src/eidmw/misc/certs
    eIDMW::PTEID_Config configCert(eIDMW::PTEID_PARAM_AUTOUPDATES_CERTS_URL);
    std::string configurl = configCert.getString();

    int certCount = cJSON_GetArraySize(certs_json);
    qDebug() << certCount;

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
            getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
            return false;
        }

        std::string  file_name_temp;
        file_name_temp.append(certs_dir_str);
        file_name_temp.append(cert_json->valuestring);

        if(QFile::exists(QString::fromUtf8(file_name_temp.c_str()))){
            qDebug() << "Cert exists";
        } else{
            qDebug() << "Cert does not exists";
            QString cert = cert_json->valuestring;
            std::string  downloadurl_temp;
            downloadurl.append(configurl);
            downloadurl.append(cert_json->valuestring);
            qDebug() << "downloadurl : " << QString::fromUtf8(downloadurl.c_str());
            break; // TODO: Download more than one certificate
        }
    }
    updateWindows(downloadurl);
    return true;
}

void CertsUpdate::updateWindows(std::string uri)
{
    qDebug() << "C++ CERTS UPDATE: There are updates available press Install do perform the updates.";
    urli = uri;
    getAppController()->signalAutoUpdateAvailable(
                GAPI::AutoUpdateCerts, "", "", "",QString::fromUtf8(uri.c_str()));
}

void CertsUpdate::httpReadyRead()
{
    qDebug() << "C++ CERTS UPDATE: httpReadyRead";
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply

    QByteArray data = reply->readAll();
    QString qsdata(data);
    //Write to memory
    filedata += qsdata.toStdString();
}
void CertsUpdate::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    qDebug() << "C++ CERTS UPDATE: updateDataReadProgress";
    if (httpRequestAborted)
        return;
}

void CertsUpdate::startUpdate()
{
    qDebug() << "C++ CERTS UPDATE: startUpdate";
    url = urli.c_str();
    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();

    if (fileName.isEmpty())
    {
         getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::GenericError);
    }
    QFile::remove(fileName);

    std::string tmpfile;
    tmpfile.append(QDir::tempPath().toStdString());
    tmpfile.append("/");
    tmpfile.append(fileName.toStdString());

    file = new QFile(QString::fromUtf8((tmpfile.c_str())));
    if (!file->open(QIODevice::WriteOnly)) {
       qDebug() << "C++ CERTS UPDATE: Unable to save the file";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
           "CertsUpdate::startUpdate: Unable to save the file.");
        delete file;
        file = nullptr;
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::UnableSaveFile);
        return;
    }

    getAppController()->signalStartUpdate(GAPI::AutoUpdateCerts, fileName);

    // schedule the request
    httpUpdateRequestAborted = false;
    startUpdateRequest(url);
}

void CertsUpdate::startUpdateRequest(QUrl url){
    qDebug() << "C++ CERTS UPDATE: startUpdate";

    if (qnam.networkAccessible() == QNetworkAccessManager::NotAccessible){
        qDebug() << "C++ CERTS UPDATE: startUpdateRequest No Internet Connection";
        PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
            "CertsUpdate::startUpdateRequest: No Internet Connection.");
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
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
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateUpdateDataReadProgress(qint64,qint64)));
}

void CertsUpdate::httpUpdateFinished(){
    qDebug() << "C++ CERTS UPDATE: httpUpdateFinished";
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
                "CertsUpdate::httpUpdateFinished: The update request was aborted.");
        }

        if (!userCanceled){
            //network failure occurred when downloading
            PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
                "CertsUpdate::httpUpdateFinished: Network failure occurred while downloading.");
            getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
        }

        reply->deleteLater();
        reply = 0;
        return;
    }
    if (!file){
        reply->deleteLater();
        reply = 0;
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::DownloadFailed);
        return;
    }
    file->flush();
    file->close();

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error())
    {
        file->remove();
        getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::NetworkError);
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
        RunPackage(fileName.toStdString());

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
}

void CertsUpdate::httpUpdateReadyRead()
{
    // this slot gets called everytime the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (file)
        file->write(reply->readAll());

}

void CertsUpdate::httpUpdateError(QNetworkReply::NetworkError networkError)
{
    qDebug() << "C++ CERTS UPDATE: httpUpdateError";
    PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
        "CertsUpdate::httpUpdateError: QNetworkReply::NetworkError = %d", networkError);

    switch(networkError){
        case QNetworkReply::NetworkError::NoError:
            //no error hence do nothing
            break;
        default:
            //cancelUpdateDownload();
            break;
    }
    return;
}
void CertsUpdate::updateUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    qDebug() << "C++ CERTS UPDATE: updateUpdateDataReadProgress";
    if (httpUpdateRequestAborted)
        return;

    qint64 valueFloat = (quint64)((double) (bytesRead * 100 / totalBytes ));
    getAppController()->signalAutoUpdateProgress(GAPI::AutoUpdateCerts,(int)valueFloat);

}
void CertsUpdate::cancelUpdateDownload()
{
    qDebug() << "C++ CERTS UPDATE: cancelUpdateDownload";

    httpUpdateRequestAborted = true;
    userCanceled = false;

    if (reply != NULL){
        reply->abort();
    }
}

void CertsUpdate::RunPackage(std::string pkg){
    qDebug() << "C++ CERTS UPDATE: RunPackage filename";

    std::string pkgpath;
    // TODO: test with Ubunto < 17

    pkgpath.append(QDir::tempPath().toStdString());
    pkgpath.append("/");
    pkgpath.append(pkg);

    qDebug() << QString::fromStdString("pkgpath " + pkgpath);

    qDebug() << "C++: RunPackage finish";
    getAppController()->signalAutoUpdateSuccess(GAPI::AutoUpdateCerts);
}

void CertsUpdate::userCancelledUpdateDownload()
{
    qDebug() << "C++ CERTS UPDATE: userCanceledUpdateDownload";

    httpUpdateRequestAborted = true;
    userCanceled = true;

    if (reply != NULL){
        reply->abort();
    }

    getAppController()->signalAutoUpdateFail(GAPI::AutoUpdateCerts, GAPI::DownloadCancelled);
}

