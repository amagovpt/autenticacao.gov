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

#ifndef MAINWND_H
#define MAINWND_H

#include <QtGui>
#include <QFutureWatcher>
#ifdef WIN32
#include <windows.h>
#include <Wincrypt.h>
#include <Cryptuiapi.h>
#else
#include "pteidversions.h"
typedef const void *PCX509CERT;
#define PCCERT_CONTEXT PCX509CERT
#endif

#include "ui_mainwnd.h"
#include "Settings.h"
#include "genpur.h"
#include "CardInformation.h"
#include "eidlib.h"
#include "eidlibException.h"
#include "picturepopup.h"

using namespace eIDMW;

class MainWnd;


class PinInfo
{

public:
	QString pin_name;
	QString pin_status;
	QString pin_id;

	PinInfo() {};
	PinInfo(int id, const char *pin_name, const char *pin_status=""): 
	  pin_name(QString::fromUtf8(pin_name)), pin_id(QString::number(id)), pin_status(QString::fromUtf8(pin_status))
	  { };
};

//***********************************************
// callback data class
// This class can be used to store whatever info we
// need from the callback function
//***********************************************
class CallBackData
{
public:
	CallBackData( void )
		: m_readerName()
		, m_mainWnd()
		, m_cardID(0)
	{
	}
	CallBackData( const char* pReaderName, MainWnd* pMainWnd )
		: m_readerName(pReaderName)
		, m_mainWnd(pMainWnd)
		, m_cardID(0)
	{
	}
	virtual ~CallBackData( void )
	{
	}
	QString const& getReaderName()
	{
		return m_readerName;
	}
	void setReaderName( QString const& readerName)
	{
		m_readerName = readerName;
	}
	void setReaderName( const char* readerName)
	{
		m_readerName = readerName;
	}
	MainWnd* getMainWnd( void )
	{
		return m_mainWnd;
	}
private:
	QString			m_readerName;
	MainWnd*		m_mainWnd;
public:
	unsigned long 		m_cardID;
};

//***********************************************
// QuitEvent
// This class is used as event to notify that the
// application has to close t down because the 
// application session ends (logoff or shutdown)
//***********************************************
class QuitEvent : public QEvent
{
public:
	QuitEvent(unsigned int msg)
		: QEvent(QEvent::Type(QEvent::User+1))
	{
		m_msg = msg;
	};
	~QuitEvent(){};
public:
	unsigned int m_msg;
};


typedef QMap<QString,QVector<PCCERT_CONTEXT> >	tCertPerReader;
typedef QMap<QString,unsigned long>				tCallBackHandles;
typedef QMap<QString,CallBackData*>				tCallBackData;

class PopupEvent : public QEvent
{
public:
	enum eCustomEventType
	{
		ET_UNKNOWN
		, ET_CARD_CHANGED
		, ET_CARD_REMOVED
	};
	PopupEvent(QString const& reader, eCustomEventType eType )
		: QEvent(QEvent::User)
		, m_cardReader(reader)
		, m_customEventType(eType)
	{
	}
	~PopupEvent()
	{
	}
	QString const& getReaderName( void )
	{
		return m_cardReader;
	}
	eCustomEventType getType( void )
	{
		return m_customEventType;
	}
private:
	QString m_cardReader;
	eCustomEventType m_customEventType;
};

class QTreeCertItem : public QTreeWidgetItem
{
public:
	QTreeCertItem(QTreeWidget *view, int type):QTreeWidgetItem(view,type) {}
	QTreeCertItem(QTreeCertItem *parent, int type):QTreeWidgetItem(parent,type){}

	void setIssuer(QString Issuer) {m_Issuer=Issuer;}
	QString const& getIssuer() {return m_Issuer;}

	void setOwner(QString Owner) {m_Owner=Owner;}
	QString const& getOwner() {return m_Owner;}

	void setValidityBegin(QString ValidityBegin) {m_ValidityBegin=ValidityBegin;}
	QString const& getValidityBegin() {return m_ValidityBegin;}

	void setValidityEnd(QString ValidityEnd) {m_ValidityEnd=ValidityEnd;}
	QString const& getValidityEnd() {return m_ValidityEnd;}

	void setKeyLen(QString KeyLen) {m_KeyLen=KeyLen;}
	QString const& getKeyLen() {return m_KeyLen;}

	void setOcspStatus(PTEID_CertifStatus OcspStatus) {m_OcspStatus=OcspStatus;}
	PTEID_CertifStatus getOcspStatus() {return m_OcspStatus;}

private:
	QString m_Issuer;
	QString m_Owner;
	QString m_ValidityBegin;
	QString m_ValidityEnd;
	QString m_KeyLen;
	PTEID_CertifStatus m_OcspStatus;
};

class MainWnd : public QMainWindow
{
#define TIMERREADERLIST 5000

    Q_OBJECT

public:
    MainWnd( GUISettings& settings, QWidget *parent = 0 );
	~MainWnd( void );
	static bool ImportCertificates( const char* readerName );
	static bool ImportCertificates( QString const& readerName );
	static bool RemoveCertificates( const char* readerName );
	static bool RemoveCertificates( QString const& readerName );
	GUISettings&	getSettings( void )
	{
		return m_Settings;
	}


private slots:
	void on_actionAbout_triggered( void );
	void on_actionClear_triggered( void );
	void on_actionPrint_eID_triggered( void );
	void on_actionPrinter_Settings_triggered( void );
	void on_actionPINRequest_triggered( void );
	void on_actionPINChange_triggered( void );
	void on_actionOptions_triggered(void);
	void on_actionE_xit_triggered(void);
	void authPINRequest_triggered( void );
	bool addressPINRequest_triggered( void );
	void PersoDataSaveButtonClicked( void );
	void setLanguageEn( void );
	void setLanguageNl( void );
	void setLanguageFr( void );
	void setLanguageDe( void );
	void restoreWindow( void );
	void messageRespond( const QString& message);


	// eventFilter to Catch actions from "personalized toolbar"
	bool eventFilter(QObject *,QEvent *);

	// SystemTray
	void iconActivated(QSystemTrayIcon::ActivationReason reason);

	// Others
	void on_btnPIN_Test_clicked( void );
	void on_btnPIN_Change_clicked( void );
	void on_treePIN_itemClicked(QTreeWidgetItem* item, int column);
	void on_treeCert_itemClicked(QTreeWidgetItem* item, int column);
	void on_treeCert_itemSelectionChanged ( void );
	void on_btnCert_Details_clicked( void );
	void on_btnCert_Register_clicked( void );
	void on_treePIN_itemSelectionChanged ( void );


	void on_btnSelectTab_Identity_clicked ( void );
	void on_btnSelectTab_Identity_Extra_clicked ( void );
	void on_btnSelectTab_Address_clicked ( void );
	void on_btnSelectTab_Certificates_clicked ( void );
	void on_btnSelectTab_PinCodes_clicked ( void );
	void on_btnSelectTab_Notes_clicked ( void );
//TOOLBAR MENUS
	void on_btn_menu_card_clicked( void );
	void on_btn_menu_tools_clicked( void );
	void on_btn_menu_language_clicked( void );
	void on_btn_menu_help_clicked( void );




//SUBMENUS Toolbar

	void hide_submenus( void );


	void quit_application();
	void show_window_parameters();
	void show_window_about();

	void updateReaderList( void );
	void customEvent( QEvent * event );
	void changeEvent( QEvent *event );

protected:
	// Window Events
	void resizeEvent( QResizeEvent* pEvent );
	void showEvent	( QShowEvent*	pEvent );
	void closeEvent	( QCloseEvent*	pEvent );

	void showNormal( void );
	void showNoReaderMsg( void );


	// SystemTray
	QAction*			m_pMinimizeAction;
	QAction*			m_pMaximizeAction;
	QAction*			m_pRestoreAction;

	QPixmap				m_imgBackground_Front;
	QPixmap				m_imgBackground_Back;
	QPixmap				m_imgPicture;
	QImage				imgPicture;
	QImage				imgPicturescaled;
	QPixmap				m_imgSex;
	QSystemTrayIcon*	m_pTrayIcon;
	QMenu*				m_pTrayIconMenu;
	QTranslator			m_translator;

	QProgressDialog *m_progress;
	QFutureWatcher<void> FutureWatcher;

	PinInfo list_of_pins[3]; 


private:
#define TRANSLATION_FILE_PREFIX	"eidmw_"

	void loadCardData( void );
	void loadCardDataAddress ();
	bool loadCardDataPersoData ();
	void loadCardDataCertificates ();
	void setStatus( unsigned int Status );

	// Refresh Data
	void showTabs( void );

	void zoomAllWidgets(QList<QWidget *>& allWidgets);
	void setWidgetPosition(QList<QWidget *>& allWidgets);

	void refreshTabIdentity( void );
	void refreshTabIdentityExtra( void );
	void refreshTabAddress( void );
	void refreshTabPersoData( void );
	void refreshTabCertificates( void );
	void refreshTabCardPin( void );
	void refreshTabInfo( void );

	void Show_Splash( void );
	void Show_Identity_Card(PTEID_EIDCard& Card);
	void Show_Address_Card(PTEID_EIDCard& Card);
	void Show_PersoData_Card(PTEID_EIDCard& Card);
	void Show_Certificates_Card(PTEID_EIDCard& Card);
	void Show_Memory_Card(PTEID_MemoryCard& Card);
	void LoadDataID(PTEID_EIDCard& Card);
	void LoadDataAddress(PTEID_EIDCard& Card);
	void LoadDataPersoData(PTEID_EIDCard& Card);
	void LoadDataCertificates(PTEID_EIDCard& Card);
	void LoadDataMC(PTEID_MemoryCard& Card);

	void InitLanguageMenu( void );
	void setLanguage( void );
	void setLanguage( GenPur::UI_LANGUAGE language );
	void fillCertTree( PTEID_Certificate *cert, short level, QTreeCertItem* item );
	void _getCertStatusText( PTEID_CertifStatus certStatus, QString& strCertStatus )
	{
		switch(certStatus)
		{
		case PTEID_CERTIF_STATUS_REVOKED:
			strCertStatus = tr("Revoked");
			break;
		case PTEID_CERTIF_STATUS_TEST:
			strCertStatus = tr("Test");
			break;
		case PTEID_CERTIF_STATUS_DATE:
			strCertStatus = tr("Date");
			break;
		case PTEID_CERTIF_STATUS_VALID:
			strCertStatus = tr("Valid");
			break;
		case PTEID_CERTIF_STATUS_VALID_CRL:
			strCertStatus = tr("Valid CRL");
			break;
		case PTEID_CERTIF_STATUS_VALID_OCSP:
			strCertStatus = tr("Valid OCSP");
			break;
		case PTEID_CERTIF_STATUS_CONNECT:
		case PTEID_CERTIF_STATUS_UNKNOWN:
		default:
			strCertStatus = tr("Unknown");
			break;
		}
	}
	void getCertStatusText( PTEID_CertifStatus certStatus, QString& strCertStatus )
	{
		switch(certStatus)
		{
		case PTEID_CERTIF_STATUS_OCSP_NOT_CHECKED:
			strCertStatus = tr("OCSP not checked");
			break;
		case PTEID_CERTIF_STATUS_REVOKED:
			strCertStatus = tr("Revoked");
			break;
		case PTEID_CERTIF_STATUS_VALID:
		case PTEID_CERTIF_STATUS_VALID_OCSP:
			strCertStatus = tr("Good");
			break;
		default:
			strCertStatus = tr("Unknown");
			break;
		}
	}
	void fillSoftwareInfo( void );
	void setLanguageMenu( GenPur::UI_LANGUAGE language );
	void setLanguageMenu( QString const& uiLang );
	void writeSettings( void );
	void readSettings( void );
	GenPur::UI_LANGUAGE LoadTranslationFile( GenPur::UI_LANGUAGE NewLanguage );
	void ShowPTEIDError( unsigned long ErrCode, QString const& msg="" );
	void enableFileMenu( void );
	void enableFileSave( bool bEnabled );
	void enablePrintMenu( void );
	bool ImportSelectedCertificate( void );
	unsigned long getCertificateIndex( QString const& label );
	void fillPinList(PTEID_EIDCard& Card);
	void fillCertificateList( void );
	void stopAllEventCallbacks( void );
	void setEventCallbacks( void );
	bool askAllowTestCard( void );

	static bool StoreUserCerts (PTEID_EIDCard& Card, PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, PTEID_Certificate& cert, const char* readerName);
	static bool StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* readerName);
	static void forgetCertificates( QString const& readerName );
	static void forgetAllCertificates( void );
#ifdef WIN32
	static BOOL UseMinidriver( void );
	static bool ProviderNameCorrect (PCCERT_CONTEXT pCertContext );
#endif
	void showCertImportMessage(bool bImported);
	bool saveCardDataToFile(QString const& fileName, PTEID_EIDCard& card );
	void setEnabledPinButtons( bool bEnabled );
	void setEnabledCertifButtons( bool bEnabled );
	QString GetCardTypeText(QString const& cardType);
	void clearTabCertificates( void );
	void clearTabAddress( void );
	void clearTabPins( void );
	void releaseVirtualReader( void );
	void doPicturePopup( PTEID_Card& card );
	void setWidgetsPointSize(QList<QWidget *> &allWidgets);
	void setCorrespondingTrayIcon( PopupEvent* callbackEvent );
	void clearGuiContent( void );
	QString getSpecialOrganizationText( QString const& code);
	QString getDuplicataText( void );
	QString getFamilyMemberText( void );
	void createTrayMenu();
	QString getFinalLinkTarget(QString baseName);	
	void cleanupCallbackData();

	eZOOMSTATUS				m_Zoom;
	QPrinter*				m_pPrinter;				//!< the 'Selected' Printer
	Ui_MainWnd				m_ui;
	PicturePopup*			m_Pop;
	PTEID_CardType			m_TypeCard;				//!< current card type being used
	GenPur::UI_LANGUAGE		m_Language;				//!< current UI language
	CardInformation			m_CI_Data;				//!< card information being retrieved

	QMap<GenPur::UI_LANGUAGE,QAction*>	m_LanguageActions;		//!< array with actions for each language item in the menu
	tCallBackHandles		m_callBackHandles;
	tCallBackData			m_callBackData;
	QString					m_CurrReaderName;		//!< the current reader we're using
	PTEID_ReaderContext*		m_virtReaderContext;	//!< virtual reader context we're using
	bool					m_UseKeyPad;
	GUISettings&			m_Settings;				//!< settings of the app
	QTimer*					m_timerReaderList;
	QMutex					m_mutex;				//!< mutex to be used when multiple cards are inserted simultaneously
	int						m_STATUS_MSG_TIME;		//!< timeout that status message is displayed

	bool					m_ShowBalloon;			//!< To avoid the message eID still running when the gui start minimize
	QMessageBox*			m_msgBox;
	PTEID_CertifStatus		m_connectionStatus;
	
public:
	static tCertPerReader			m_certContexts;			//!< certificate contexts of each reader

};


class CardDataLoader
{
private:
	CardInformation& information;
	PTEID_EIDCard &card;
	QString &readerName;

	
	public:
	CardDataLoader(CardInformation& info, PTEID_EIDCard& Card, QString& ReaderName): 
		information(info), card(Card), readerName(ReaderName)
	{ }

	void Load()
	{

		this->information.LoadData(card, readerName);
	}
};





#endif

