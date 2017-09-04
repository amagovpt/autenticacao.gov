#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QVariant>
#include "Settings.h"
#include "genpur.h"

//MW libraries
#include "eidlib.h"
#include "eidErrors.h"
#include "eidlibException.h"

class AppController;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(GUISettings &settings, QObject *parent = 0);
    GUISettings&    getSettings( void )
    {
        return m_Settings;
    }

public slots:
    Q_INVOKABLE QVariant getCursorPos();
    bool getAutoCardReadingValue(void);
    void setAutoCardReadingValue (bool bAutoCardReading );

    bool getStartAutoValue (void);
    void setStartAutoValue (bool bAutoStartup );

    bool getStartMinimizedValue(void);
    void setStartMinimizedValue(bool bStartMinimized );

    int getGuiLanguageCodeValue(void);
    void setGuiLanguageCodeValue (int language);

    bool getShowNotificationValue(void);
    bool getShowPictureValue(void);
    bool getShowAnimationsValue(void);
    bool getRegCertValue(void);
    bool getRemoveCertValue(void);

    void setShowNotificationValue(bool bShowNotification);
    void setShowPictureValue(bool bShowPicture);
    void setShowAnimationsValue(bool bShowAnimations);
    void setRegCertValue(bool bRegCert);
    void setRemoveCertValue(bool bRemoveCert);

    QString getTimeStampHostValue (void);
    void setTimeStampHostValue (QString const& timeStamp_host);

    QString getProxyHostValue (void);
    void setProxyHostValue (QString const& proxy_host);
    long getProxyPortValue (void);
    void setProxyPortValue (int proxy_port);
    QString getProxyUsernameValue (void);
    void setProxyUsernameValue (QString const& proxy_user);
    QString getProxyPwdValue (void);
    void setProxyPwdValue (QString const& proxy_pwd);

private:
    GUISettings&    m_Settings;
    GenPur::UI_LANGUAGE LoadTranslationFile( GenPur::UI_LANGUAGE NewLanguage );
    GenPur::UI_LANGUAGE m_Language;
protected:
    QTranslator m_translator;
signals:
    void signalSetReaderComboIndex(long selected_reader);
};

#endif // APPCONTROLLER_H
