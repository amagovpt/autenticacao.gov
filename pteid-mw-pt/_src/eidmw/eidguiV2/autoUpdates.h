/*-****************************************************************************

 * Copyright (C) 2019-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.2

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

struct NewsEntry
{
    int id;
    std::string title;
    std::string first_day;
    std::string last_day;
    std::string text;
    std::string link;
};

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
    ~AutoUpdates() {
        delete qnam;
    }
    void setAppController(AppController* AppController){
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
    void updateDataReadProgress();

    // Public slots to process the update file
    void cancelUpdateDownload();
    void httpUpdateError(QNetworkReply::NetworkError networkError);
    void httpUpdateFinished();
    void httpUpdateReadyRead();
    void updateUpdateDataReadProgress(/*qint64 bytesRead, qint64 totalBytes*/);

private:
    AppController*   m_AppController;
    int m_updateType;
    QUrl url;
    QNetworkProxy proxy;
    QNetworkAccessManager *qnam = NULL;
    QNetworkReply *reply;
    QFile *file;
    int fileIdx = 0;
    bool httpRequestAborted;
    bool httpUpdateRequestAborted;
    bool userCanceled;
    std::string filedata;
    QStringList urlList;
    QStringList hashList;
    std::string getdistro;
    QStringList fileNames;
    QString fileName;
    QString release_notes;
    QString remote_version;
    QString installed_version;
    std::vector<NewsEntry> m_news;
    QString m_newsTitle;
    QString m_newsBody;
    QString m_newsUrl;
    QString m_newsId;

    // Private methods to process the configuration file
    void VerifyAppUpdates(std::string filedata);
    void VerifyCertsUpdates(std::string filedata);
    void VerifyNewsUpdates(std::string filedata);

    void parseNews(std::string filedata);
    std::vector<NewsEntry> ChooseNews();
    void ChooseCertificates(cJSON *certs_json);
    void ChooseAppVersion(std::string distro, std::string arch, cJSON *dist_json);

    std::string VerifyOS(std::string param);

    QString getCertListAsString();

    void updateWindows();

    // Private methods to process the update file
    void startUpdateRequest(QUrl url);

    void RunAppPackage(std::string pkg, std::string distro);
    void RunCertsPackage(QStringList certs);
    bool validateHash(QString certPath, QString hash);
    bool verifyPackageSignature(std::string &pkg);
};

#endif //AUTOUPDATES_H
