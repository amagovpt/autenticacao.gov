/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef CERTSUPDATE_H
#define CERTSUPDATE_H

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

class CertsUpdate : public QObject
{
    Q_OBJECT
public:
    CertsUpdate(){}
    CertsUpdate(AppController* AppController)
        : m_AppController(AppController)
    {
    }
    AppController* setAppController(AppController* AppController){
        m_AppController = AppController;
    }
    AppController* getAppController(void){
        return m_AppController;
    }

public slots:
    void startRequest(QUrl url);
    void startUpdate();
    void startUpdateRequest(QUrl url);

    void cancelDownload();
    void httpError(QNetworkReply::NetworkError networkError);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    void cancelUpdateDownload();
    void httpUpdateError(QNetworkReply::NetworkError networkError);
    void httpUpdateFinished();
    void httpUpdateReadyRead();
    void updateUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    bool VerifyUpdates(std::string filedata);
    bool ChooseVersion(cJSON *dist_json);
    void updateWindows(std::string uri);
    void RunPackage(std::string pkg);
    void userCancelledUpdateDownload();

private:
    AppController*   m_AppController;
    QUrl url;
    QNetworkProxy proxy;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    QFile *file;
    QString m_pac_url;
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


};

#endif //CERTSUPDATE_H
