/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2022 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 João Pinheiro - <joao.pinheiro@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */


#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <Qt>
#include <QtCore>
#include <QSettings>
#include <QString>
#include "eidlib.h"

#define SECTION_CERTVALIDATION		"certificatevalidation"
#define SECTION_GENERAL				"general"
#define SECTION_CARDREADERS			"cardreaders"
#define SECTION_CERTIFICATES		"certificates"

#define STR_LANGUAGE			"language"
#define STR_STARTMINIMIZED		"start_minimized"
#define STR_USEVIRTUALKEYPAD	"use_virtual_keypad"

#define STR_SHOWPICTURE			"show_picture"
#define STR_AUTOCARDREADING		"automatic_cardreading"
#define STR_AUTOSTARTUP			"start_with_windows"
#define STR_REGCERT				"register_certificate"
#define STR_REMOVECERT			"remove_certificate"
#define STR_CARDREADER			"cardreader"

#define STR_DEF_GUILANGUAGE		"nl"

#define PIN_MAX_LENGHT 8
#define PIN_MIN_LENGHT 4

#define OPENGL_SOFTWARE     0
#define OPENGL_HARDWARE     1
#define OPENGL_DIRECT3D     2

#define SIGN_SEAL_NUM_ID_OPTION 1
#define SIGN_SEAL_DATE_OPTION 2


class GUISettings
{
public:
    //------------------------------------------------------
    // ctor
    //------------------------------------------------------
    GUISettings( void )
        : m_bProxySystem(false)
        , m_proxy_port(0)
        , m_GuiLanguage("nl")
        , m_bShowAnimations(false)
        , m_bUseSystemScale(false)
        , m_iApplicationScale(0)
        , m_iGraphicsAccel(0)
        , m_bUseCustomSignature(false)
        , m_bPinpadEnabled(false)
        , m_bStartMinimized(false)
        , m_bStartAutoupdate(false)
        , m_bDebugMode(false)
        , m_bNotShowStartUpHelp(false)
        , m_bAskToRegisterCmdCert(false)
        , m_bAskToSetCache(false)
        , m_bShowSignatureOptions(false)
        , m_bShowSignatureHelp(false)
        , m_bAutoCardReading(false)
        , m_bAutoStartup(false)
        , m_bRegCert(false)
        , m_bRemoveCert(false)
        , m_strExePath("")
        , m_test_mode(false)
        , m_iSignSealOptions(0)
    {
        //----------------------------------------------------------
        // Check always what is set in the registry
        //----------------------------------------------------------

        //----------------------------------------------------------
        // check the GUI language
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_LANGUAGE);
            QString lng = config.getString();
            if(lng == "en" || lng == "nl"){
                setGuiLanguage(lng);
            }else{
                setGuiLanguage(STR_DEF_GUILANGUAGE);
            }
        }

        //----------------------------------------------------------
        // check use Pinpad functionality
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PINPAD_ENABLED);
            long pinpadEnabled = config.getLong();

            if (0 != pinpadEnabled)
            {
                setPinpadEnabled(true);
            }
        }

        //----------------------------------------------------------
        // check start minimized
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTMINI);
            long StartMinimized = config.getLong();

            if ( 0 != StartMinimized )
            {
                setStartMinimized(true);
            }
        }
        //----------------------------------------------------------
        // check autoupdates on app start
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTUPDATE);
            long startAutoUpdate = config.getLong();

            if (0 != startAutoUpdate)
            {
                setStartAutoupdate(true);
            }
        }

        //----------------------------------------------------------
        // check debug mode
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_ASKREGCMDCERT);
            long askRegCmdCert = config.getLong();

            if (0 != askRegCmdCert)
            {
                setAskToRegisterCmdCert(true);
            }
        }

        //----------------------------------------------------------
        // check ask register Cmd cert
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_LOGGING_LEVEL);
            QString logLevel = config.getString();

            // Debug mode means the log's log_level is "debug".
            if ("debug" == logLevel)
            {
                setDebugMode(true);
            }
        }

        //----------------------------------------------------------
        // check set cache
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_ASKSETCACHE);
            long askToSetCache = config.getLong();

            if (0 != askToSetCache)
            {
                setAskToSetCache(true);
            }
        }

        //----------------------------------------------------------
        // check ShowAnimations
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWANIMATIONS);
            long ShowAnimations = config.getLong();
            if ( 0 != ShowAnimations )
            {
                setShowAnimations(true);
            }
        }
        //----------------------------------------------------------
        // check UseSystemApplicationScale
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_USESYSTEMSCALE);
            long UseSystemScale = config.getLong();
            if ( 0 != UseSystemScale )
            {
                setUseSystemScale(UseSystemScale);
            }
        }
        //----------------------------------------------------------
        // check ApplicationScale
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_APPLICATIONSCALE);
            long Scale = config.getLong();
            if ( 0 != Scale )
            {
                setApplicationScale(Scale);
            }
        }
        //----------------------------------------------------------
        // check Acceleration Graphics
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_GRAPHICSACCEL);
            long AccelGraphics = config.getLong();
            setAccelGraphics(AccelGraphics);
        }
        //----------------------------------------------------------
        // check USECUSTOMSIGNATURE
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_USECUSTOMSIGN);
            long UseCustomSignature = config.getLong();
            if ( 0 != UseCustomSignature )
            {
                setUseCustomSignature(true);
            }
        }

        //----------------------------------------------------------
        // check SIGNSEALOPTIONS
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SIGNSEALOPTIONS);
            long UseSignSealOptions = config.getLong();
            setSignSealOptions(UseSignSealOptions);
        }

        //----------------------------------------------------------
        // check ShowSignatureOptions
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSIGNOPTIONS);
            long ShowSignOption = config.getLong();
            setShowSignatureOptions(ShowSignOption);

        }
        //----------------------------------------------------------
        // check ShowSignatureHelp
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSIGNHELP);
            long ShowSignatureHelp = config.getLong();
            setShowSignatureHelp(ShowSignatureHelp);
        }
        //----------------------------------------------------------
        // check ShowHelpStartUp
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP);
            long ShowHelpStartUp = config.getLong();
            if ( 0 != ShowHelpStartUp )
            {
                setNotShowHelpStartUp(true);
            }
        }
        //----------------------------------------------------------
        // check AutoCardReading
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_AUTOCARDREAD);
            long AutoCardReading = config.getLong();
            if ( 0 != AutoCardReading )
            {
                setAutoCardReading(true);
            }
        }
        //----------------------------------------------------------
        // check AutoStartup
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTWIN);
            long AutoStartup = config.getLong();
            setAutoStartup(0 != AutoStartup);
        }
        //----------------------------------------------------------
        // check card reader to use
        // We shouldn't load the selected reader from settings initially
        //----------------------------------------------------------
        /*	{

            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_CARDREADNUMB);
            long CardReader = config.getLong();
            setSelectedReader(CardReader);
        } */
        //----------------------------------------------------------
        // check RegCert
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_REGCERTIF);
            long RegCert = config.getLong();
            if ( 0 != RegCert )
            {
                setRegCert(true);
            }
        }
        //----------------------------------------------------------
        // check RemoveCert
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_REMOVECERTIF);
            long RemoveCert = config.getLong();
            if ( 0 != RemoveCert )
            {
                setRemoveCert(true);
            }
        }
        //---------------------------------------------------------
        // Check timestamp settings
        //---------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_XSIGN_TSAURL);
            QString timeStamp_url = config.getString();
            m_time_stamp_host = timeStamp_url;

        }

        //---------------------------------------------------------
        // Check proxy settings
        //---------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_USE_SYSTEM);
            long proxy_system = config.getLong();
            m_bProxySystem = proxy_system;
        }
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
            QString proxy_host = config.getString();
            m_proxy_host = proxy_host;
        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_PORT);
            long proxy_port = config.getLong();
            m_proxy_port = proxy_port;

        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_USERNAME);
            QString proxy_user = config.getString();
            m_proxy_username = proxy_user;

        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_PWD);
            QString pwd = config.getString();
            m_proxy_pwd = pwd;

        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PTEID_CACHEDIR);
#ifdef WIN32
			m_pteid_cachedir = QString::fromLatin1(config.getString());
#else
			m_pteid_cachedir = config.getString();
#endif
        }
    }
    //------------------------------------------------------
    // dtor
    //------------------------------------------------------
    ~GUISettings( void )
    {
    }
    //------------------------------------------------------
    // Read the setting
    //------------------------------------------------------
    QString const& getGuiLanguageString( void ) const
    {
        return m_GuiLanguage;
    }

    void setGuiLanguage( QString const& GuiLanguage=STR_DEF_GUILANGUAGE )
    {
        m_GuiLanguage = GuiLanguage;
    }
    void setLanguage( QString const& language )
    {
        m_GuiLanguage = language;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_LANGUAGE);
        config.setString(m_GuiLanguage.toLatin1());
    }
    bool getPinpadEnabled(void) const
    {
        return m_bPinpadEnabled;
    }
    void setPinpadEnabled(bool bPinpadEnabled)
    {
        m_bPinpadEnabled = bPinpadEnabled;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PINPAD_ENABLED);
        config.setLong(m_bPinpadEnabled);
    }
    bool getStartMinimized( void ) const
    {
        return m_bStartMinimized;
    }
    void setStartMinimized( bool bStartMinimized)
    {
        m_bStartMinimized = bStartMinimized;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTMINI);
        config.setLong(m_bStartMinimized);
    }
    bool getStartAutoupdate(void) const
    {
        return m_bStartAutoupdate;
    }
    void setStartAutoupdate(bool bStartAutoupdate)
    {
        m_bStartAutoupdate = bStartAutoupdate;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTUPDATE);
        config.setLong(bStartAutoupdate);
    }
    bool getDebugMode(void) const
    {
        return m_bDebugMode;
    }
    QString setDebugMode(bool bDebugMode)
    {
        m_bDebugMode = bDebugMode;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_LOGGING_LEVEL);
        if (bDebugMode) {
            config.setString("debug");
        }
        else {
            config.setString(""); // Default (see MapLevel)
            #ifdef WIN32
            QFile debugFileDesktop(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "\\pteid-debug.conf");
            if (debugFileDesktop.exists() && !debugFileDesktop.remove()) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "setDebugMode: Could not delete desktop debug file at:");
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", debugFileDesktop.fileName().toLocal8Bit().data());
                return debugFileDesktop.fileName();
            }

            QFile debugFileSystem(QString::fromWCharArray(_wgetenv(L"WINDIR")) + "\\pteid-debug.conf");
            if (debugFileDesktop.exists() && !debugFileSystem.remove()) {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", "setDebugMode: Could not delete system debug file at:");
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "eidgui", debugFileSystem.fileName().toLocal8Bit().data());
                return debugFileSystem.fileName();
            }
            #endif
        }
        return QString("");
    }
    bool getAskToRegisterCmdCert(void)
    {
        return m_bAskToRegisterCmdCert;
    }
    void setAskToRegisterCmdCert(bool bAskToRegisterCmdCert)
    {
        m_bAskToRegisterCmdCert = bAskToRegisterCmdCert;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_ASKREGCMDCERT);
        config.setLong(m_bAskToRegisterCmdCert);
    }

    bool getAskToSetCache(void)
    {
        return m_bAskToSetCache;
    }
    void setAskToSetCache(bool bAskToSetCacheValue)
    {
        m_bAskToSetCache = bAskToSetCacheValue;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_ASKSETCACHE);
        config.setLong(m_bAskToSetCache);
    }
    bool getShowAnimations( void )
    {
        return m_bShowAnimations;
    }
    bool getUseSystemScale( void )
    {
        return m_bUseSystemScale;
    }
    int getApplicationScale( void )
    {
        return m_iApplicationScale;
    }
    int getGraphicsAccel( void )
    {
        return m_iGraphicsAccel;
    }
    void setUseSystemScale(  bool bUseSystemScale )
    {
        m_bUseSystemScale = bUseSystemScale;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_USESYSTEMSCALE);
        config.setLong(bUseSystemScale);
    }
    void setApplicationScale(  int iApplicationScale )
    {
        m_iApplicationScale = iApplicationScale;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_APPLICATIONSCALE);
        config.setLong(iApplicationScale);
    }
    void setShowAnimations(  bool bShowAnimations )
    {
        m_bShowAnimations = bShowAnimations;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWANIMATIONS);
        config.setLong(m_bShowAnimations);
    }
    void setAccelGraphics(  int iGraphicsAccel )
    {
        m_iGraphicsAccel = iGraphicsAccel;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_GRAPHICSACCEL);
        config.setLong(m_iGraphicsAccel);
    }
    bool getUseCustomSignature( void )
    {
        return m_bUseCustomSignature;
    }
    void setUseCustomSignature(  bool bUseCustomSignature )
    {
        m_bUseCustomSignature = bUseCustomSignature;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_USECUSTOMSIGN);
        config.setLong(m_bUseCustomSignature);
    }
    bool getNotShowHelpStartUp( void )
    {
        return m_bNotShowStartUpHelp;
    }
    void setNotShowHelpStartUp(  bool bNotShowStartUpHelp )
    {
        m_bNotShowStartUpHelp = bNotShowStartUpHelp;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP);
        config.setLong(m_bNotShowStartUpHelp);
    }
    bool getAutoCardReading( void )
    {
        return m_bAutoCardReading;
    }
    void setAutoCardReading( bool bAutoCardReading )
    {
        m_bAutoCardReading = bAutoCardReading;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_AUTOCARDREAD);
        config.setLong(m_bAutoCardReading);
    }
    bool getAutoStartup( void )
    {
        return m_bAutoStartup;
    }
    void setAutoStartup( bool bAutoStartup )
    {
    #ifdef WIN32
        m_bAutoStartup = bAutoStartup;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTWIN);
        config.setLong(m_bAutoStartup);

        QSettings s("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

        if (m_bAutoStartup)
        {
            QString filePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            s.setValue("pteid", filePath);
        }
        else
        {
            s.remove("pteid");
        }

    #endif

    }
    bool getRegCertSetting( void )
    {
        return m_bRegCert;
    }
    bool getRegCert( void )
    {
        if (!m_bAutoCardReading)
        {
            return false;
        }
        return m_bRegCert;
    }

    void setRegCert( bool bRegCert )
    {
        m_bRegCert = bRegCert;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_REGCERTIF);
        config.setLong(m_bRegCert);
    }

    bool getRemoveCertSetting( void )
    {
        return m_bRemoveCert;
    }

    bool getRemoveCert( void )
    {
        if (!m_bAutoCardReading)
        {
            return false;
        }
        return m_bRemoveCert;
    }
    void setRemoveCert( bool bRemoveCert )
    {
        m_bRemoveCert = bRemoveCert;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_REMOVECERTIF);
        config.setLong(m_bRemoveCert);
    }

    QString getTimeStampHost(void){
        return m_time_stamp_host;
    }

    void setTimeStampHost(QString const& timeStamp_host)
    {
        m_time_stamp_host = timeStamp_host;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_XSIGN_TSAURL);
        config.setString(timeStamp_host.toUtf8());
    }

    void setExePath( QString const& strExePath )
    {
        m_strExePath = strExePath;
    }

    QString const& getExePath( void )
    {
        return m_strExePath;
    }

    void setGuiVersion( QString const& GUIVersion )
    {
        m_GUIVersion = GUIVersion;
    }
    QString const& getGuiVersion( void )
    {
        return m_GUIVersion;
    }

    bool getProxySystem( void )
    {
        return m_bProxySystem;
    }
    void setProxySystem( bool bProxySystem )
    {
        m_bProxySystem = bProxySystem;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_USE_SYSTEM);
        config.setLong(m_bProxySystem);
    }
    void setProxyHost(QString const& proxy_host)
    {
        m_proxy_host = proxy_host;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
        config.setString(proxy_host.toUtf8());
    }

    QString getProxyHost()
    {
        return m_proxy_host;
    }

    void setProxyUsername(QString const& proxy_user)
    {
        m_proxy_username = proxy_user;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_USERNAME);
        config.setString(proxy_user.toUtf8());
    }

    void setProxyPwd(QString const& proxy_pwd)
    {
        m_proxy_pwd = proxy_pwd;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_PWD);
        config.setString(proxy_pwd.toUtf8());
    }

    void setEnablePteidCache(bool bEnabled)
    {
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PTEID_CACHE_ENABLED);
        config.setLong(bEnabled);
    }

    bool getEnablePteidCache()
    {
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PTEID_CACHE_ENABLED);
        return config.getLong();
    }

    QString getProxyUsername()
    {
        return m_proxy_username;
    }

    QString getProxyPwd()
    {
        return m_proxy_pwd;
    }

    long getProxyPort()
    {
        return m_proxy_port;
    }

    void setProxyPort(int proxy_port)
    {
        m_proxy_port = proxy_port;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_PORT);
        config.setLong(proxy_port);
    }

    QString getPteidCachedir(){
        return m_pteid_cachedir;
    }

    /* Is proxy manually configured or using system? */
    bool isProxyConfigured() {
        if (!getProxyHost().isEmpty() && getProxyPort() != 0) {
            return true; //Manually configured
        }
        if (getProxySystem()) {
            return true; //Configured in system
        }
        return false;
    }

    /* Test Mode */
    bool getTestMode()	{
        return m_test_mode;
    }

    void setTestMode( bool test_mode )	{
        m_test_mode = test_mode;
    }

       /* GUI Signature Options */
    bool getShowSignatureOptions() {
        return m_bShowSignatureOptions;
    }

    void setShowSignatureOptions( bool bShowSignatureOptions ) {
        m_bShowSignatureOptions = bShowSignatureOptions;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSIGNOPTIONS);
        config.setLong(m_bShowSignatureOptions);
    }

       /* GUI Signature Help */
    bool getShowSignatureHelp() {
        return m_bShowSignatureHelp;
    }

    void setShowSignatureHelp( bool bShowSignatureHelp ) {
        m_bShowSignatureHelp = bShowSignatureHelp;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWSIGNHELP);
        config.setLong(m_bShowSignatureHelp);
    }

    bool getUseNumId(void) {
        return (m_iSignSealOptions & SIGN_SEAL_NUM_ID_OPTION);
    }
    
    bool getUseDate(void) {
        return (m_iSignSealOptions & SIGN_SEAL_DATE_OPTION);
    }

    void setUseNumId(bool bUseNumId) {
        if (bUseNumId)   
        {
            m_iSignSealOptions |= SIGN_SEAL_NUM_ID_OPTION;
        }
        else
        {
            m_iSignSealOptions &= SIGN_SEAL_DATE_OPTION;
        }

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SIGNSEALOPTIONS);
        config.setLong(m_iSignSealOptions);
    }

    void setUseDate(bool bUseDate) {
        if (bUseDate)  
        {
            m_iSignSealOptions |= SIGN_SEAL_DATE_OPTION;
        }
        else
        {
            m_iSignSealOptions &= SIGN_SEAL_NUM_ID_OPTION;
        }

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SIGNSEALOPTIONS);
        config.setLong(m_iSignSealOptions);
    }

    void setSignSealOptions(int iSignSealOptions) {
        m_iSignSealOptions = iSignSealOptions;
        
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SIGNSEALOPTIONS);
        config.setLong(m_iSignSealOptions);
    }

private:
    //Proxy Settings
    bool m_bProxySystem;
    QString m_proxy_host;
    long m_proxy_port;
    QString m_proxy_username;
    QString m_proxy_pwd;
    QString m_pteid_cachedir;

    //Time Stamp Host Settings
    QString m_time_stamp_host;

    QString m_GuiLanguage;          //!< the GUI language
    bool    m_bShowAnimations;      //!< the GUI Animations
    int     m_bUseSystemScale;      //!< use the system scale
    int     m_iApplicationScale;    //!< the GUI scale
    int     m_iGraphicsAccel;       //!< the Graphics Acceleration 0=Software, 1=Hardware, 2=ANGLE
    bool    m_bUseCustomSignature;  //!< the GUI use custom signature image
    bool    m_bPinpadEnabled;       //!< use Pinpad functionality when supported by readers (T/F)
    bool    m_bStartMinimized;      //!< startup minimized (T/F)
    bool    m_bStartAutoupdate;     //!< check for updates when starting the app (T/F)
    bool    m_bDebugMode;           //!< debug mode enabled (T/F)
    bool    m_bNotShowStartUpHelp;  //!< the GUI Show Help	bool	m_bStartMinimized;              //!< startup minimized (T/F)
    bool    m_bAskToRegisterCmdCert;//!< the GUI will ask to register the CMD cert on start (T/F)
    bool    m_bAskToSetCache;        //!< the GUI will ask to set the cache (on/off) on start (T/F)
    bool    m_bShowSignatureOptions;//!< show signature options in GUI Signature page (T/F)
    bool    m_bShowSignatureHelp;   //!< show signature help in GUI Signature page (T/F)
    bool    m_bAutoCardReading;     //!< read the inserted card at startup (T/F)
    bool    m_bAutoStartup;         //!< start the app when windows starts (T/F)
    bool    m_bRegCert;             //!< register certificates on insert (T/F)
    bool    m_bRemoveCert;          //!< remove certificates on close (T/F)
    QString m_strExePath;           //!< path to the executable
    bool    m_test_mode;
    int     m_iSignSealOptions;

    QString m_GUIVersion;           //!! Full version of the GUI
};

#endif // SETTINGS_H
