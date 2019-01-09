/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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


class GUISettings
{
public:
    //------------------------------------------------------
    // ctor
    //------------------------------------------------------
    GUISettings( void )
        : m_GuiLanguage("nl")
        , m_bShowAnimations(false)
        , m_bUseCustomSignature(false)
        , m_bStartMinimized(false)
        , m_bNotShowStartUpHelp(false)
        , m_bShowPicture(false)
        , m_bShowNotification(false)
        , m_bAutoCardReading(false)
        , m_bAutoStartup(false)
        , m_bRegCert(false)
        , m_bRemoveCert(false)
        , m_strExePath("")
        , m_test_mode(false)

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
        // check ShowPicture
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWPIC);
            long ShowPicture = config.getLong();
            if ( 0 != ShowPicture )
            {
                setShowPicture(true);
            }
        }
        //----------------------------------------------------------
        // check ShowNotification
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWNOTIFICATION);
            long ShowNotification = config.getLong();
            if ( 0 != ShowNotification )
            {
                setShowNotification(true);
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
            if ( 0 != AutoStartup )
            {
                setAutoStartup(true);
            }
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
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SHOW_JAVA_APPS);
            m_showJavaApps = config.getLong() == 1;

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

    bool getShowPicture( void )
    {
        return m_bShowPicture;
    }
    void setShowPicture( bool bShowPicture )
    {
        m_bShowPicture = bShowPicture;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWPIC);
        config.setLong(m_bShowPicture);
    }
    bool getShowNotification( void )
    {
        return m_bShowNotification;
    }
    bool getShowAnimations( void )
    {
        return m_bShowAnimations;
    }
    void setShowAnimations(  bool bShowAnimations )
    {
        m_bShowAnimations = bShowAnimations;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWANIMATIONS);
        config.setLong(m_bShowAnimations);
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
    void setShowNotification( bool bShowNotification )
    {
        m_bShowNotification = bShowNotification;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_SHOWNOTIFICATION);
        config.setLong(m_bShowNotification);
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
        m_bAutoStartup = bAutoStartup;
        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_STARTWIN);
        config.setLong(m_bAutoStartup);

#ifdef WIN32
        QSettings s("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
#endif

        if (m_bAutoStartup)
        {
#ifdef WIN32
            QString filePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
            s.setValue("pteid", filePath);
#endif
#ifdef __APPLE__
            //            QProcess::execute("sudo defaults write com.apple.loginwindow LoginHook" + m_strExePath);
#endif
        }
        else
        {
#ifdef WIN32
            s.remove("pteid");
#endif
#ifdef __APPLE__
            //            QProcess::execute("sudo defaults delete com.apple.loginwindow LoginHook" + m_strExePath);
#endif
        }

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

    bool areJavaAppsEnabled()
    {
        return m_showJavaApps;
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

    /* Test Mode */
    bool getTestMode()	{
        return m_test_mode;
    }

    void setTestMode( bool test_mode )	{
        m_test_mode = test_mode;
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
    bool    m_bUseCustomSignature;  //!< the GUI use custom signature image
    bool    m_bStartMinimized;      //!< startup minimized (T/F)
    bool    m_bNotShowStartUpHelp;  //!< the GUI Show Help	bool	m_bStartMinimized;		//!< startup minimized (T/F)
    bool    m_bShowPicture;         //!< show the picture (T/F)
    bool    m_bShowNotification;    //!< show the notification (T/F)
    bool    m_bAutoCardReading;     //!< read the inserted card at startup (T/F)
    bool    m_bAutoStartup;         //!< start the app when windows starts (T/F)
    bool    m_bRegCert;             //!< register certificates on insert (T/F)
    bool    m_bRemoveCert;          //!< remove certificates on close (T/F)
    bool    m_showJavaApps;         // wether we should show the SCAP/DSS buttons...
    QString m_strExePath;           //!< path to the executable
    bool m_test_mode;

    QString m_GUIVersion;           //!! Full version of the GUI
};

#endif // SETTINGS_H
