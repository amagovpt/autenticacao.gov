/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QVariant>
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

#include "autoUpdates.h"

#define STR_LOCALTIME_MAX_SIZE   24

class AppController : public QObject
{
    Q_OBJECT
    //  Used to dynamic translation in QML (QTBUG-15602)
    Q_PROPERTY(QString autoTr READ getAutoTr NOTIFY languageChanged)
public:
    explicit AppController(GUISettings &settings, QObject *parent = 0);
    GUISettings&    getSettings( void )
    {
        return m_Settings;
    }
    QString getAutoTr() {
     return "";
    }

public slots:
    void restoreScreen(void);
    Q_INVOKABLE QVariant getCursorPos();
    QString getAppVersion(void);
    QString getAppRevision(void);
    void updateCertslog(void);
    QString getCertsLog(void);
    void updateNewsLog(void);
    QString getNewsLog(void);
    bool isToShowNews(QString id);
    QString getAppCopyright(void);
    bool isAnimationsEnabled(void);
    bool getNotShowHelpStartUp(void);
    bool getTestMode(void);
    void setNotShowHelpStartUp(bool notshowhelpStartUp);
    void initTranslation(void);
    bool getAutoCardReadingValue(void);
    void setAutoCardReadingValue (bool bAutoCardReading );


    // Auto updates certificates
    void autoUpdatesCerts(void);
    void startUpdateCerts(void);

    void autoUpdateApp(void);
    void startUpdateApp(void);

    void autoUpdatesNews(void);

    bool getStartAutoupdateValue(void);
    void setStartAutoupdateValue(bool bStartAutoupdate);

    bool getPinpadEnabledValue(void);
    void setPinpadEnabledValue(bool bPinpadEnabled);

    bool getStartAutoValue (void);
    void setStartAutoValue (bool bAutoStartup );

    bool getStartMinimizedValue(void);
    void setStartMinimizedValue(bool bStartMinimized );

    bool getDebugModeValue(void);
    QString setDebugModeValue(bool bDebugMode);

    bool getAskToRegisterCmdCertValue(void);
    void setAskToRegisterCmdCertValue(bool bAskToRegisterCmdCert);

    QString getGuiLanguageString(void);
    void setGuiLanguageString (QString language);

    bool getShowNotificationValue(void);
    bool getShowPictureValue(void);
    bool getShowAnimationsValue(void);
    bool getUseSystemScaleValue(void);
    int  getApplicationScaleValue(void);
    int  getGraphicsAccelValue(void);

    void setShowNotificationValue(bool bShowNotification);
    void setShowPictureValue(bool bShowPicture);
    void setShowAnimationsValue(bool bShowAnimations);
    void setUseSystemScaleValue(bool bUseSystemScale);
    void setApplicationScaleValue(int iScale);
    void setGraphicsAccelValue(int iGraphicsAccel);

    QString getTimeStampHostValue (void);
    void setTimeStampHostValue (QString const& timeStamp_host);

    bool getProxySystemValue (void);
    void setProxySystemValue (bool bProxySystem);
    QString getProxyHostValue (void);
    void setProxyHostValue (QString const& proxy_host);
    long getProxyPortValue (void);
    void setProxyPortValue (int proxy_port);
    QString getProxyUsernameValue (void);
    void setProxyUsernameValue (QString const& proxy_user);
    QString getProxyPwdValue (void);
    void setProxyPwdValue (QString const& proxy_pwd);

    bool isOutlookInstalled();
    bool getOutlookSuppressNameChecks(void);
    void setOutlookSuppressNameChecks(bool bDisabledMatching);

    void userCancelledUpdateCertsDownload();
    void userCancelledUpdateAppDownload();
    void setEnablePteidCache(bool bEnabled);
    bool getEnablePteidCache();
    void flushCache();
    void getPteidCacheSize();
    void getScapCacheSize();
    void forceAccessibilityUpdate(QObject *obj);
    bool isAccessibilityActive();

    QString getFontFile(QString font);
    QStringList getFilesFromClipboard();
    static void initApplicationScale();
    void openTransfersFolder();

private:
    GUISettings&    m_Settings;
    bool LoadTranslationFile(QString NewLanguage );
    void doFlushCache();
    bool removePteidCache();
    void doGetPteidCacheSize();
    void doGetScapCacheSize();
    QString getPteidCacheDir();
    void createCacheDir();
    void checkUpdateCertslog(void);
    void checkUpdateNewslog(void);
    qint64 dirSize(QString dirPath, QString nameFilter);
    QString formatSize(qint64 size);

    AutoUpdates certsUpdate;
    AutoUpdates appUpdate;
    AutoUpdates newsUpdate;

protected:
    QTranslator m_translator;

signals:
    void signalRestoreWindows();
    void languageChanged();
    void signalLanguageChangedError();

    // Autoupdates
    void signalAutoUpdateFail(int updateType, int error_code);
    void signalAutoUpdateAvailable(int updateType,
                                   QString arg1,
                                   QString arg2,
                                   QString arg3,
                                   QString url_list);
    void signalAutoUpdateProgress(int updateType/*, int value*/);
    void signalStartUpdate(int updateType, QString filename);
    void signalAutoUpdateSuccess(int updateType);

    void signalRemovePteidCacheSuccess();
    void signalRemovePteidCacheFail();
    void signalCacheNotReadable();
    void signalCacheNotWritable();
    void signalAppCacheSize(QString cacheSize);
    void signalScapCacheSize(QString cacheSize);
};

#endif // APPCONTROLLER_H
