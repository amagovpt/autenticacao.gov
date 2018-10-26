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

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "eidlibdefines.h"

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
    bool isAnimationsEnabled(void);
    bool getNotShowHelpStartUp(void);
    bool getTestMode(void);
    void setNotShowHelpStartUp(bool notshowhelpStartUp);
    void initTranslation(void);
    bool getAutoCardReadingValue(void);
    void setAutoCardReadingValue (bool bAutoCardReading );

    void autoUpdates(void);
    void startRequest(QUrl url);
    void startUpdateRequest(QUrl url);
    bool VerifyUpdates(std::string filedata);
    std::string VerifyOS(std::string param);
    void ChooseVersion(std::string distro, std::string arch);
    void updateWindows(std::string uri, std::string distro);
    void RunPackage(std::string pkg, std::string distro);
    void startUpdate(void);

    bool getStartAutoValue (void);
    void setStartAutoValue (bool bAutoStartup );

    bool getStartMinimizedValue(void);
    void setStartMinimizedValue(bool bStartMinimized );

    QString getGuiLanguageString(void);
    void setGuiLanguageString (QString language);

    bool getShowNotificationValue(void);
    bool getShowPictureValue(void);
    bool getShowAnimationsValue(void);

    void setShowNotificationValue(bool bShowNotification);
    void setShowPictureValue(bool bShowPicture);
    void setShowAnimationsValue(bool bShowAnimations);

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

    void cancelDownload();
    void httpError(QNetworkReply::NetworkError networkError);
    void httpUpdateError(QNetworkReply::NetworkError networkError);
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void cancelUpdateDownload();
    void userCancelledUpdateDownload();
    void httpUpdateFinished();
    void httpUpdateReadyRead();
    void updateUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void flushCache();
    void getPteidCacheSize();
    void getScapCacheSize();
    
private:
    GUISettings&    m_Settings;
    bool LoadTranslationFile(QString NewLanguage );
    void doFlushCache();
    bool removePteidCache();
    void doGetPteidCacheSize();
    void doGetScapCacheSize();
    qint64 dirSize(QString dirPath, QString nameFilter);
    QString formatSize(qint64 size);

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
protected:
    QTranslator m_translator;

signals:
    void signalRestoreWindows();
    void languageChanged();
    void signalLanguageChangedError();
    void signalAutoUpdateFail(int error_code);
    void signalAutoUpdateAvailable(QString release_notes, QString installed_version, QString remote_version);
    void signalAutoUpdateProgress(int value);
    void signalStartUpdate(QString filename);
    void signalRemovePteidCacheSuccess(); 
    void signalRemovePteidCacheFail();
    void signalCacheNotReadable();
    void signalCacheNotWritable();
    void signalAppCacheSize(QString cacheSize);
    void signalScapCacheSize(QString cacheSize);
};

#endif // APPCONTROLLER_H
