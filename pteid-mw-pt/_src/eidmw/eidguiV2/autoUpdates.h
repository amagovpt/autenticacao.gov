/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef AUTOUPDATES_H
#define AUTOUPDATES_H

#include <QObject>
#include <QVariant>
#include <QFile>
#include <QUrl>
#include "Settings.h"
#include "ScapSettings.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include "cJSON_1_7_12.h"

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "eidlibdefines.h"

class AppController;

class AutoUpdates : public QObject
{
    Q_OBJECT
public:
    AutoUpdates(){}
    AutoUpdates(AppController* AppController)
        : m_AppController(AppController)
    {
    }
    AppController* setAppController(AppController* AppController){
        m_AppController = AppController;
    }
    AppController* getAppController(void){
        return m_AppController;
    }

    void initRequest(int updateType);
    void startRequest(QUrl url);
    void startUpdate();
    void userCancelledUpdateDownload();

public slots:

    // Public slots to process the configuration file
    void cancelDownload();
    void httpError(QNetworkReply::NetworkError networkError);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    // Public slots to process the update file
    void cancelUpdateDownload();
    void httpUpdateError(QNetworkReply::NetworkError networkError);
    void httpUpdateFinished();
    void httpUpdateReadyRead();
    void updateUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    AppController*   m_AppController;
    int m_updateType;
    QUrl url;
    QNetworkProxy proxy;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;

    bool httpRequestAborted;
    bool httpUpdateRequestAborted;
    bool userCanceled;
    std::string filedata;
    std::string urli;
    std::string getdistro;
    QString fileName;
    QString release_notes;
    QString remote_version;
    QString installed_version;

    // Private methods to process the configuration file
    bool VerifyAppUpdates(std::string filedata);
    bool VerifyCertsUpdates(std::string filedata);

    bool ChooseCertificates(cJSON *certs_json);
    bool ChooseAppVersion(std::string distro, std::string arch, cJSON *dist_json);

    std::string VerifyOS(std::string param);


    void updateWindows(std::string uri, std::string distro);

    // Private methods to process the update file
    void startUpdateRequest(QUrl url);

    void RunAppPackage(std::string pkg, std::string distro);
    void RunCertsPackage(std::string pkg);
};

#endif //AUTOUPDATES_H
