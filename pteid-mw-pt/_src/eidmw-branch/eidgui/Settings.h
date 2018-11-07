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
#include "genpur.h"
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

#define STR_DEF_GUILANGUAGE		"EN"

#define PIN_MAX_LENGHT 8
#define PIN_MIN_LENGHT 4

#define SPECIALSTATUS_NA 0
#define SPECIALSTATUS_WHITECANE 1
#define SPECIALSTATUS_EXTMINORITY 2
#define SPECIALSTATUS_WCANE_EXTMIN 3
#define SPECIALSTATUS_YELLOWCANE 4
#define SPECIALSTATUS_YCANE_EXTMIN 5

// Resize Resolutions -> Common
enum eZOOMSTATUS
{
	 ZOOM_SMALL			// ( 800 x 600 )
	,ZOOM_MEDIUM		// ( 1024 x 768)
	,ZOOM_LARGE			// ( 1280 x 1024 )
	,ZOOM_HUGE			// ( 1600 x 1200 )
};

// Resize Resolutions -> Window width
#define WINDOW_WIDTH_SMALL 800.0
#define WINDOW_WIDTH_MEDIUM 1024.0
#define WINDOW_WIDTH_LARGE 1280.0
#define WINDOW_WIDTH_HUGE 1600.0

#define WINDOW_RATIO 1.58

// Resize Resolutions -> Window height -46 for the margin  -170 for the menu, toolbar,...
#define WINDOW_HEIGHT_SMALL  600.0
#define WINDOW_HEIGHT_MEDIUM 768.0
#define WINDOW_HEIGHT_LARGE  1024.0
#define WINDOW_HEIGHT_HUGE   1200.0
// #define WINDOW_HEIGHT_SMALL  (int) ( (WINDOW_WIDTH_SMALL - 46) / WINDOW_RATIO + 170  )
// #define WINDOW_HEIGHT_MEDIUM (int) ( (WINDOW_WIDTH_MEDIUM - 46) / WINDOW_RATIO + 170 )
// #define WINDOW_HEIGHT_LARGE  (int) ( (WINDOW_WIDTH_LARGE - 46) / WINDOW_RATIO + 170  )
// #define WINDOW_HEIGHT_HUGE   (int) ( (WINDOW_WIDTH_HUGE - 46) / WINDOW_RATIO + 170   )

// zoomfactor
// zoom1 -> zoom2
#define ZOOMFACTOR_1 (WINDOW_HEIGHT_MEDIUM/WINDOW_HEIGHT_SMALL)
#define ZOOMFACTOR_2 (WINDOW_HEIGHT_LARGE/WINDOW_HEIGHT_SMALL)
#define ZOOMFACTOR_3 (WINDOW_HEIGHT_HUGE/WINDOW_HEIGHT_SMALL)

// Resize Resolutions -> Button Widgets
#define BUTTON_POINTSIZE_SMALL 10
#define BUTTON_POINTSIZE_MEDIUM 13
#define BUTTON_POINTSIZE_LARGE 16
#define BUTTON_POINTSIZE_HUGE 18

// Resize Resolutions -> Line Edit Widgets
#define NORMALVALUE_POINTSIZE_SMALL 11
#define NORMALVALUE_POINTSIZE_MEDIUM 14
#define NORMALVALUE_POINTSIZE_LARGE 17
#define NORMALVALUE_POINTSIZE_HUGE 20

// Resize Resolutions -> Line Edit Widgets
#define BIGVALUE_POINTSIZE_SMALL 18
#define BIGVALUE_POINTSIZE_MEDIUM 22
#define BIGVALUE_POINTSIZE_LARGE 26
#define BIGVALUE_POINTSIZE_HUGE 30

// Resize Resolutions -> Line Edit Widgets
#define SISVALUE_POINTSIZE_SMALL 18
#define SISVALUE_POINTSIZE_MEDIUM 22
#define SISVALUE_POINTSIZE_LARGE 26
#define SISVALUE_POINTSIZE_HUGE 30

// Resize Resolutions -> Label Widgets
#define LABEL_POINTSIZE_SMALL 9
#define LABEL_POINTSIZE_MEDIUM 11
#define LABEL_POINTSIZE_LARGE 14
#define LABEL_POINTSIZE_HUGE 16

// Resize Resolutions -> Title1 Widgets
#define TITLE1_POINTSIZE_SMALL 18
#define TITLE1_POINTSIZE_MEDIUM 22
#define TITLE1_POINTSIZE_LARGE 26
#define TITLE1_POINTSIZE_HUGE 30

// Resize Resolutions -> Title2 Widgets
#define TITLE2_POINTSIZE_SMALL 10
#define TITLE2_POINTSIZE_MEDIUM 12
#define TITLE2_POINTSIZE_LARGE 16
#define TITLE2_POINTSIZE_HUGE 19

// Resize Resolutions -> Footer Widgets
#define FOOTER1_POINTSIZE_SMALL 8
#define FOOTER1_POINTSIZE_MEDIUM 10
#define FOOTER1_POINTSIZE_LARGE 12
#define FOOTER1_POINTSIZE_HUGE 14

// Resize Resolutions -> Small Widgets
#define SMALL_POINTSIZE_SMALL 5
#define SMALL_POINTSIZE_MEDIUM 6
#define SMALL_POINTSIZE_LARGE 7
#define SMALL_POINTSIZE_HUGE 9

// Resize Resolutions -> Toolbar Icon/Toolbutton Widgets
#define TOOLBAR_ICON_BASE 24
#define TOOLBAR_ICON_INCREMENT 2

#define PICTURE_RATIO 1.41

// Resize Resolutions -> Card Owner Picture
#define PICTURE_BASE_HEIGHT 250
#define PICTURE_BASE_WIDTH PICTURE_BASE_HEIGHT / PICTURE_RATIO
#define PICTURE_HEIGHT_INCREMENT 60
#define PICTURE_WIDTH_INCREMENT PICTURE_HEIGHT_INCREMENT / PICTURE_RATIO

#define IMG_SEX_SIZE_BASE 32
#define IMG_SEX_SIZE_INCREMENT 8

class GUISettings
{
public:
	//------------------------------------------------------
	// ctor
	//------------------------------------------------------
	GUISettings( void )
		: m_GuiLanguage("en")
		, m_bStartMinimized(false)
		, m_bShowPicture(false)
		, m_bShowNotification(false)
		, m_bAutoCardReading(false)
		, m_bAutoStartup(false)
		, m_bRegCert(false)
		, m_bRemoveCert(false)
		, m_strExePath("")
		, m_SelectedReader(-1)
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
			if ( GenPur::LANG_XX==GenPur::getLanguage(lng))
			{
				lng = GenPur::getLanguage(GenPur::LANG_DEF);
			}
			setGuiLanguage(lng);
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
		//----------------------------------------------------------
		// check file save path
		//----------------------------------------------------------
		{
			eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_FILESAVE);
			QString fileSave = config.getString();
			m_DefSavePath = fileSave;
		}

		//---------------------------------------------------------
		// Check proxy settings
		//---------------------------------------------------------
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
			QString m_pteid_cachedir = config.getString();
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
	GenPur::UI_LANGUAGE getGuiLanguageCode( void ) const
	{
		return GenPur::getLanguage(m_GuiLanguage);
	}
	void setGuiLanguage( QString const& GuiLanguage=STR_DEF_GUILANGUAGE )
	{
		m_GuiLanguage = GuiLanguage;
	}
	void setGuiLanguage( GenPur::UI_LANGUAGE language )
	{
		m_GuiLanguage = GenPur::getLanguage(language);
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
	void setExePath( QString const& strExePath )
	{
		m_strExePath = strExePath;
	}

	QString const& getExePath( void )
	{
		return m_strExePath;
	}
	
    void setSelectedReader( int Idx )
	{
		m_SelectedReader = Idx;
		eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_CARDREADNUMB);
		config.setLong(m_SelectedReader);
	}
	unsigned long getSelectedReader( void )
	{
		return m_SelectedReader;
    }

	void setGuiVersion( QString const& GUIVersion )
	{
		m_GUIVersion = GUIVersion;
	}
	QString const& getGuiVersion( void )
	{
		return m_GUIVersion;
	}
	void setDefSavePath( QString const& defSavePath )
	{
		m_DefSavePath = defSavePath;
		eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GUITOOL_FILESAVE);
		config.setString(m_DefSavePath.toLatin1());
	}

	QString const& getDefSavePath( void )
	{
		return m_DefSavePath;
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

	void setPteidCachedir(QString const& pteid_cachedir)
	{
		eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_PTEID_CACHEDIR);
		config.setString(pteid_cachedir.toUtf8());

		m_pteid_cachedir = pteid_cachedir;
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
	QString m_proxy_host;
	long m_proxy_port;
	QString m_proxy_username;
	QString m_proxy_pwd;
	QString m_pteid_cachedir;
	bool m_test_mode;


	QString	m_GuiLanguage;			//!< the GUI language
	bool	m_bStartMinimized;		//!< startup minimized (T/F)
	bool	m_bShowPicture;			//!< show the picture (T/F)
	bool	m_bShowNotification;	//!< show the notification (T/F)
	bool	m_bAutoCardReading;		//!< read the inserted card at startup (T/F)
	bool	m_bAutoStartup;			//!< start the app when windows starts (T/F)
	bool	m_bRegCert;				//!< register certificates on insert (T/F)
	bool	m_bRemoveCert;			//!< remove certificates on close (T/F)
	bool 	m_showJavaApps;     // wether we should show the SCAP/DSS buttons...
	QString m_strExePath;			//!< path to the executable
	unsigned long		m_SelectedReader;		//!< selected reader (-1=none)

	QString	m_GUIVersion;			//!! Full version of the GUI
	QString	m_DefSavePath;			//!< default save path for eid,xml,tlv files

};

#endif // SETTINGS_H
