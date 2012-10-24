/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include <iostream>
#include <sstream>
#include <QThread>
#include <QtGui>
#include <QFileDialog>
#include <QApplication>
#include <QEvent>
#include <QPixmap>
#include <QImage>
#include <stdlib.h>
#ifndef _WIN32
//This has to be explicitly included in gcc4.7
#include <unistd.h>
#endif
#include <time.h>

#ifdef WIN32
#include <shlobj.h>
#endif
#include "mainwnd.h"
#include "dlgAbout.h"
#include "dlgprint.h"
#include "dlgverifysignature.h"
#include "PDFSignWindow.h"
#include "dlgsignature.h"
#include "dlgOptions.h"
#include "Settings.h"
#include "CardInformation.h"
#include "eidlib.h"
#include "eidlibException.h"
#include "picturepopup.h"
#include "AutoUpdates.h"
#include "eidErrors.h"
#include "dialogs.h"
#include "Util.h"
#include <wchar.h>
#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include "verinfo.h"
#else
#include "pteidversions.h"
#endif

static bool	g_cleaningCallback=false;
static int	g_runningCallback=0;
//State variables for tab data
static unsigned int pinactivate = 1, certdatastatus = 1, addressdatastatus = 1, persodatastatus = 1 ;
//State of Pin Notes 0->Right PIN 1->Not yet inserted or wrong PIN
static unsigned int pinNotes = 1 ;

#ifdef WIN32
void ImportECRaizCert()
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		L"C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\ECRaizEstado_novo_assinado_GTE.der",
		CERT_QUERY_CONTENT_FLAG_ALL,
		CERT_QUERY_FORMAT_FLAG_ALL,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(const void **)&pCertCtx) != 0)
	{
		
		HCERTSTORE hCertStore = CertOpenSystemStoreA (NULL, "CA");

		if (hCertStore != NULL)
		{
			CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_SERVER_AUTH);
			if (CertAddCertificateContextToStore (
				hCertStore,
				pCertCtx,
				CERT_STORE_ADD_ALWAYS,
				NULL))
			{
				std::cout << "Added certificate to store." << std::endl;
			}

			if (CertCloseStore (hCertStore, 0))
			{
				std::cout << "Cert. store handle closed." << std::endl;
			}
		}

		if (pCertCtx)
		{
			CertFreeCertificateContext (pCertCtx);
		}
	}
}

void ImportCCCert()
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		L"C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\CartaodeCidadao001.der",
		CERT_QUERY_CONTENT_FLAG_ALL,
		CERT_QUERY_FORMAT_FLAG_ALL,
		0,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		(const void **)&pCertCtx) != 0)
	   {
		HCERTSTORE hCertStore = CertOpenSystemStoreA (NULL, "CA");

		if (hCertStore != NULL)
		{
			CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertCtx, szOID_PKIX_KP_SERVER_AUTH);
			if (CertAddCertificateContextToStore (
				hCertStore,
				pCertCtx,
				CERT_STORE_ADD_ALWAYS,
				NULL))
			{
				std::cout << "Added certificate to store." << std::endl;
			}

			if (CertCloseStore (hCertStore, 0))
			{
				std::cout << "Cert. store handle closed." << std::endl;
			}
		}

		if (pCertCtx)
		{
			CertFreeCertificateContext (pCertCtx);
		}
	}
}

#endif


void MainWnd::createTrayMenu()
{
	m_pMinimizeAction = new QAction(tr("Mi&nimize"), this);
	connect(m_pMinimizeAction, SIGNAL(triggered()), this, SLOT(showMinimized()));

	m_pRestoreAction = new QAction(tr("&Restore"), this);
	connect(m_pRestoreAction, SIGNAL(triggered()), this, SLOT(restoreWindow()));

	m_pTrayIconMenu = new QMenu(this);
	m_pTrayIconMenu->addAction(m_pMinimizeAction);
	m_pTrayIconMenu->addAction(m_pRestoreAction);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_ui.actionOptions);
	m_pTrayIconMenu->addSeparator();
	m_pTrayIconMenu->addAction(m_ui.actionE_xit);

	m_pTrayIcon->setContextMenu( m_pTrayIconMenu );
	if (isHidden())
	{
		m_pMinimizeAction->setEnabled(false);
	}
	else
	{
		m_pRestoreAction->setEnabled(false);
	}
}

//*****************************************************
// certificate contexts per card reader
//*****************************************************
tCertPerReader	MainWnd::m_certContexts;		//!< certificate contexts per reader

//*****************************************************
// Constructor
//*****************************************************
MainWnd::MainWnd( GUISettings& settings, QWidget *parent ) 
: QMainWindow(parent)
, m_Zoom(ZOOM_SMALL)
, m_pPrinter(NULL)
, m_Pop(NULL)
, m_TypeCard(PTEID_CARDTYPE_UNKNOWN)
, m_Language(GenPur::LANG_EN)
, m_CurrReaderName("")
, m_virtReaderContext(NULL)
, m_UseKeyPad(false)
, m_Settings(settings)
, m_timerReaderList(NULL)
, m_STATUS_MSG_TIME(5000)
, m_ShowBalloon(false)
, m_msgBox(NULL)
, m_connectionStatus((PTEID_CertifStatus)-1)
{
	//------------------------------------
	// install the translator object and load the .qm file for
	// the given language.
	//------------------------------------
	qApp->installTranslator(&m_translator);

	GenPur::UI_LANGUAGE CurrLng   = m_Settings.getGuiLanguageCode();
	GenPur::UI_LANGUAGE LoadedLng = LoadTranslationFile(CurrLng);

	m_Language = LoadedLng;

	m_ui.setupUi(this);

	setFixedSize(830, 630);
	m_ui.wdg_submenu_card->setVisible(false);

	Qt::WindowFlags flags = windowFlags();
	flags ^= Qt::WindowMaximizeButtonHint;
	setWindowFlags( flags );

	/*** Setup progress Bar ***/
	m_progress = new QProgressDialog(this);
	m_progress->setWindowModality(Qt::WindowModal);
	m_progress->setWindowTitle(QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"));
	m_progress->setLabelText(tr("Reading card data..."));
	m_progress->setWindowFlags(m_progress->windowFlags() ^ Qt::WindowMinimizeButtonHint ^ Qt::WindowCloseButtonHint ^ Qt::CustomizeWindowHint);
	m_progress->setFixedSize(m_progress->size());
	//Disable cancel button
	m_progress->setCancelButton(NULL);
	//Configuring dialog as a "doing-stuff-type" progressBar
	m_progress->setMinimum(0);
	m_progress->setMaximum(0);


	connect(&this->FutureWatcher, SIGNAL(finished()), m_progress, SLOT(cancel()));
	//------------------------------------
	//
	// set the window Icon (as it appears in the left corner of the window)
	//------------------------------------
	const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
	this->setWindowIcon( Ico );

	m_pPrinter	= new QPrinter();

	Show_Splash();
	//------------------------------------
	//SysTray
	//------------------------------------
	m_pTrayIcon = new QSystemTrayIcon( this );
	m_pTrayIcon->setToolTip(QString(tr("Portuguese Citizen Card")));
	createTrayMenu();

	connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

	m_Pop = new PicturePopup(0);		// create the popup object for card insert/remove

	//----------------------------------------------------
	// in case of autoread the card at startup, we have to load the data
	// This will be handled in the callback functions.
	// At startup, a callback will be called and the card will be loaded
	// if necessary
	//----------------------------------------------------
	setEventCallbacks();

	enableFileMenu();
	enablePrintMenu();

	m_ui.actionShow_Toolbar->setChecked( m_Settings.getShowToolbar() );

	setEnabledCertifButtons( false );
	setEnabledPinButtons( false );

	//----------------------------------
	// some items are not available on other platforms than windows
	// so hide them, such that users can't click them.
	//----------------------------------
#ifndef WIN32
	m_ui.btnCert_Details->hide();
	m_ui.btnCert_Register->hide();

#endif

	//----------------------------------
	// set a timer to check if the number of card readers is changed
	//----------------------------------
	if (!m_timerReaderList)
	{
		m_timerReaderList = new QTimer(this);
		connect(m_timerReaderList, SIGNAL(timeout()), this, SLOT(updateReaderList()));
		m_timerReaderList->start(TIMERREADERLIST); 
	}

	//------------------------------------
	// set the tray Icon (as it appears in the traybar)
	//------------------------------------
	setCorrespondingTrayIcon(NULL);

	if ( m_Settings.getStartMinimized() )
	{
		Qt::WindowStates state = windowState();
		state ^= Qt::WindowMinimized;
		setWindowState( state );
	}
	else 
	{
		showNoReaderMsg();
	}

	m_ui.lbl_menuCard_Read->installEventFilter(this);
	m_ui.lbl_menuCard_Pdf->installEventFilter(this);
	m_ui.lbl_menuCard_Quit->installEventFilter(this);
	m_ui.lbl_menuTools_Parameters->installEventFilter(this);
	m_ui.lbl_menuTools_Signature->installEventFilter(this);
	m_ui.lbl_menuTools_PDFSignature->installEventFilter(this);
	m_ui.lbl_menuTools_VerifySignature->installEventFilter(this);
	m_ui.lbl_menuLanguage_Portuguese->installEventFilter(this);
	m_ui.lbl_menuLanguage_English->installEventFilter(this);
	m_ui.lbl_menuHelp_about->installEventFilter(this);
	m_ui.lbl_menuHelp_updates->installEventFilter(this);
	m_ui.wdg_submenu_card->installEventFilter(this);
	m_ui.wdg_submenu_tools->installEventFilter(this);
	m_ui.wdg_submenu_help->installEventFilter(this);
	m_ui.wdg_submenu_language->installEventFilter(this);

}


bool MainWnd::eventFilter(QObject *object, QEvent *event)
{

	if (event->type() == QEvent::MouseButtonRelease)
	{

		if (object == m_ui.lbl_menuCard_Read )
		{
			hide_submenus();
			pinactivate = 1;
			pinNotes = 1;
			certdatastatus = 1;
			m_connectionStatus = (PTEID_CertifStatus)-1;
			m_CI_Data.Reset();
			loadCardData();
		}

		if (object == m_ui.lbl_menuCard_Pdf )
		{
			on_actionPrint_eID_triggered();
		}

		if (object == m_ui.lbl_menuCard_Quit )
		{
			quit_application();
		}

		if (object == m_ui.lbl_menuTools_Parameters )
		{
			hide_submenus();
			show_window_parameters();
		}

		if (object == m_ui.lbl_menuTools_Signature )
		{
			hide_submenus();
			actionSignature_eID_triggered();
		}
		if (object == m_ui.lbl_menuTools_PDFSignature)
		{
			hide_submenus();
			actionPDFSignature_triggered();
		}

		if (object == m_ui.lbl_menuTools_VerifySignature )
		{
			hide_submenus();
			actionVerifySignature_eID_triggered();
		}

		if (object == m_ui.lbl_menuLanguage_Portuguese )
		{
			hide_submenus();
			setLanguageNl();
		}
		if (object == m_ui.lbl_menuLanguage_English )
		{
			hide_submenus();
			setLanguageEn();
		}
		if (object == m_ui.lbl_menuHelp_updates )
		{
			hide_submenus();
			on_actionUpdates_triggered();
		}
		if (object == m_ui.lbl_menuHelp_about )
		{
			hide_submenus();
			show_window_about();
		}
	}
	
	if (event->type() == QEvent::Leave)
	{
		if (object == m_ui.wdg_submenu_card || object == m_ui.wdg_submenu_tools || object == m_ui.wdg_submenu_language || object == m_ui.wdg_submenu_help )
		{
			hide_submenus();
		}
	}

	return false;
}

void MainWnd::hide_submenus()
{
	m_ui.wdg_submenu_card->setVisible(false);
	m_ui.wdg_submenu_tools->setVisible(false);
	m_ui.wdg_submenu_language->setVisible(false);
	m_ui.wdg_submenu_help->setVisible(false);
}

//******************************************************
// Buttons to control tabs
//******************************************************
void MainWnd::on_btnSelectTab_Identity_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(1);
}

void MainWnd::on_btnSelectTab_Identity_Extra_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(2);
}

void MainWnd::on_btnSelectTab_Address_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(3);
	if (addressdatastatus == 1)
		refreshTabAddress();
}

void MainWnd::on_btnSelectTab_Certificates_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(4);
	if (certdatastatus == 1)
		refreshTabCertificates();
}

void MainWnd::on_btnSelectTab_PinCodes_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(5);
}

void MainWnd::on_btnSelectTab_Notes_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(6);
	if (persodatastatus == 1)
		refreshTabPersoData();
}

void MainWnd::on_btn_menu_card_clicked()
{
	m_ui.wdg_submenu_card->setVisible(true);
	//If defined language is portuguese, then the dialog needs to be larger
	if (m_Settings.getGuiLanguageCode() == GenPur::LANG_NL)
		m_ui.wdg_submenu_card->setGeometry(0,4,136,110);
	else
		m_ui.wdg_submenu_card->setGeometry(0,4,126,110);
}

void MainWnd::on_btn_menu_tools_clicked()
{
	m_ui.wdg_submenu_tools->setVisible(true);
	//If defined language is portuguese, then the dialog needs to be larger
	if (m_Settings.getGuiLanguageCode() == GenPur::LANG_NL)
		m_ui.wdg_submenu_tools->setGeometry(128,4,155,130);
	else
		m_ui.wdg_submenu_tools->setGeometry(128,4,126,130);

}

void MainWnd::on_btn_menu_language_clicked()
{
	m_ui.wdg_submenu_language->setVisible(true);
	m_ui.wdg_submenu_language->setGeometry(254,4,126,90);
}

void MainWnd::on_btn_menu_help_clicked()
{
	m_ui.wdg_submenu_help->setVisible(true);
	m_ui.wdg_submenu_help->setGeometry(380,4,126,90);
}


//*****************************************************
// show the message if no reader are available
//*****************************************************
void MainWnd::showNoReaderMsg( void )
{
	if ( 0 == ReaderSet.readerCount() )
	{
		QString strCaption(tr("Checking card readers"));
		QString strMessage(tr("No card readers are detected.\n"));
		strMessage += tr("Please check the card readers are connected and/or verify the ");
#ifdef WIN32
		strMessage += tr("smart card service ");
#else
		strMessage += tr("pcsc daemon ");
#endif
		strMessage += tr("is running.");
		if(!m_msgBox)
		{
			m_msgBox =  new QMessageBox(QMessageBox::Warning,strCaption,strMessage,QMessageBox::Ok,this);
			m_msgBox->setModal(true);
			m_msgBox->show();
		}
	}
}

//*****************************************************
// show the windows
//*****************************************************
void MainWnd::showNormal( void )
{
	m_ShowBalloon=true;
	QMainWindow::showNormal();
}

//*****************************************************
// Respond to other instance
//*****************************************************
void MainWnd::messageRespond( const QString& message)
{
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "messageRespond - Receive message = %s",message.toLatin1().data());

	if(message.compare("Restore Windows")==0)
	{
		restoreWindow();
	}
}

//*****************************************************
// restore the window in its previous state
//*****************************************************
void MainWnd::restoreWindow( void )
{
	if( this->isHidden() )
	{
		showNoReaderMsg();
		//If the application was started as minimized we need to load it once
		if(!m_CI_Data.isDataLoaded())
		{
			loadCardData();
		}

	}
	this->showNormal();
	this->raise();
	this->setFocus();
}
//*****************************************************
// update the readerlist. In case a reader is added to the machine
// at runtime.
//*****************************************************
void MainWnd::updateReaderList( void )
{
	//----------------------------------------------------
	// check if the number of readers is changed
	//----------------------------------------------------
	try
	{
		if (ReaderSet.isReadersChanged())
		{
			stopAllEventCallbacks();
			ReaderSet.releaseReaders();
			m_CI_Data.Reset();
		}
		if ( 0 == m_callBackHandles.size() )
		{
			setEventCallbacks();
		}
		setCorrespondingTrayIcon(NULL);		
	}
	catch(...)
	{
		stopAllEventCallbacks();
		ReaderSet.releaseReaders();
		m_CI_Data.Reset();
		setCorrespondingTrayIcon(NULL);		
	}
}

//*****************************************************
// Enable/disable the PIN buttons
//*****************************************************
void MainWnd::setEnabledPinButtons( bool bEnabled )
{
	if (bEnabled)
	{
		//----------------------------------------------------
		// only if an item is selected, the buttons are enabled
		//----------------------------------------------------
		QList<QTreeWidgetItem *> selectedItems =  m_ui.treePIN->selectedItems ();
		if (selectedItems.size()==0)
		{
			bEnabled = false;
		}
	}
	m_ui.btnPIN_Test->setEnabled(bEnabled);
	m_ui.btnPIN_Change->setEnabled(bEnabled);
}

//*****************************************************
// enable/disable the certificate buttons
//*****************************************************
void MainWnd::setEnabledCertifButtons( bool bEnabled )
{
	m_ui.btnCert_Register->setEnabled(bEnabled);
	m_ui.btnCert_Details->setEnabled(bEnabled);
}
//*****************************************************
// dtor
//*****************************************************
MainWnd::~MainWnd( void )
{
	cleanupCallbackData();
	if(m_pPrinter)
	{
		delete m_pPrinter;
		m_pPrinter = NULL;
	}
	if(m_Pop)
	{
		delete m_Pop;
		m_Pop = NULL;
	}

	releaseVirtualReader();
}
//*****************************************************
// cleanup the callback data
//*****************************************************
void MainWnd::cleanupCallbackData()
{

	while(g_runningCallback)
	{
#ifdef WIN32
		::Sleep(100);
#else
		::usleep(100000);
#endif
	}

	g_cleaningCallback = true;

	for (tCallBackData::iterator it = m_callBackData.begin()
			; it != m_callBackData.end()
			; it++
	)
	{
		CallBackData* pCallbackData = it.value();
		delete pCallbackData;
	}
	m_callBackData.clear();
	g_cleaningCallback = false;
}

//*****************************************************
// stop the event callbacks and delete the corresponding callback data
// objects.
//*****************************************************
void MainWnd::stopAllEventCallbacks( void )
{

	for (tCallBackHandles::iterator it = m_callBackHandles.begin()
			; it != m_callBackHandles.end()
			; it++
	)
	{
		PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName(it.key().toLatin1());
		unsigned long handle = it.value();
		readerContext.StopEventCallback(handle);
	}
	m_callBackHandles.clear();
	cleanupCallbackData();
}

//*****************************************************
// Enable the File save menu items depending on the fact we have a card loaded or not
//*****************************************************
void MainWnd::enableFileSave( bool bEnabled )
{
	m_ui.actionSave_eID->setEnabled(bEnabled);
	m_ui.actionSave_eID_as->setEnabled(bEnabled);;
}
void MainWnd::enableFileMenu( void )
{
	m_ui.actionSave_eID->setEnabled(false);
	m_ui.actionSave_eID_as->setEnabled(false);
}
//*****************************************************
// Enable the File print menu items depending on the fact we have a card loaded or not
// If we have a card loaded, then we can print
//*****************************************************
void MainWnd::enablePrintMenu( void )
{
	bool bEnable = false;
	if (m_CI_Data.isDataLoaded())
	{
		/*
		switch(m_CI_Data.m_CardInfo.getType())
		{
		case PTEID_CARDTYPE_IAS07:
		case PTEID_CARDTYPE_IAS101:
		default:
			break;
		}
		*/
	}
	m_ui.actionPrint_eID->setEnabled(bEnable);
	m_ui.actionPrinter_Settings->setEnabled(bEnable);;
}


//*****************************************************
// Load a translation file
// In case the new language can not be loaded, depending on the
// parameters, either the old language will be reloaded or the
// default language will be loaded.
//*****************************************************
GenPur::UI_LANGUAGE MainWnd::LoadTranslationFile(GenPur::UI_LANGUAGE NewLanguage)
{

	QString strTranslationFile;
	strTranslationFile = QString("eidmw_") + GenPur::getLanguage(NewLanguage);

	if (!m_translator.load(strTranslationFile,m_Settings.getExePath()+"/"))
	{
		// this should not happen, since we've built the menu with the translation filenames
	}
	return NewLanguage;
}

void MainWnd::showEvent( QShowEvent * event )
{
	m_pMinimizeAction->setEnabled(true);
	m_pRestoreAction->setEnabled(false);
	event->accept();
}
void MainWnd::resizeEvent( QResizeEvent * event )
{
	event->accept();
}


void MainWnd::closeEvent( QCloseEvent *event)
{
	printf("closeEvent() called");
#ifndef __APPLE__
    if ( m_pTrayIcon->isVisible() )
	{
		if (m_msgBox)
		{
			delete(m_msgBox);
			m_msgBox = NULL;
		}
        //clearGuiContent();
		hide();
		if(m_ShowBalloon)
		{
			QString title(tr("Portuguese ID Card Management is still running"));
			QString message(tr("The application will continue to run. To stop this application, select 'Exit' from the menu."));
			m_pTrayIcon->showMessage ( title, message, QSystemTrayIcon::Information) ;
			m_ShowBalloon = false;
		}
		m_pMinimizeAction->setEnabled(false);
		m_pRestoreAction->setEnabled(true);
		event->ignore();

        //To avoid problem when restoring the eidgui and clears data
        //loadCardData();
	}
#endif
}


//*****************************************************
// Tray icon activation detection
//*****************************************************
void MainWnd::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
	switch (reason) 
	{
	case QSystemTrayIcon::Trigger:
		break;

	case QSystemTrayIcon::DoubleClick:
		if( this->isHidden() )
		{
			this->restoreWindow();
			m_pMinimizeAction->setEnabled(true);
			m_pRestoreAction->setEnabled(false);
		}
		else
		{
			m_pMinimizeAction->setEnabled(false);
			m_pRestoreAction->setEnabled(true);
			this->hide();
		}
		break;

	case QSystemTrayIcon::MiddleClick:
		m_pTrayIcon->showMessage( "eID", "eID reader: Tray icon", QSystemTrayIcon::Information, 3 * 1000 ); // 3 sec
		break;

	default:
		break;
	}
}

//*****************************************************
// remove the certificates of a card in a specific reader
//*****************************************************
bool MainWnd::RemoveCertificates( QString const& readerName )
{
	return RemoveCertificates( readerName.toLatin1().data() );
}

bool MainWnd::RemoveCertificates( const char* readerName )
{
#ifdef WIN32

	if ( !readerName || 0 == strlen(readerName) )
	{
		return false;
	}

	PCCERT_CONTEXT	pCertContext = NULL;
	int nrCerts = m_certContexts[readerName].size();
	for (int CertIdx=0;CertIdx<nrCerts;CertIdx++)
	{

		// ----------------------------------------------------
		// create the certificate context with the certificate raw data
		// ----------------------------------------------------
		PCCERT_CONTEXT  pDesiredCert	= NULL;
		HCERTSTORE		hMyStore		= CertOpenSystemStore(NULL, "MY");

		pCertContext = m_certContexts[readerName][CertIdx];

		if ( NULL != hMyStore )
		{
			// ----------------------------------------------------
			// look if we already have the certificate in the store
			// If the certificate is not found --> NULL
			// ----------------------------------------------------
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
			)
			{
				CertDeleteCertificateFromStore( pDesiredCert );
			}
		}
	}
	if (nrCerts>0)
	{
		forgetCertificates( readerName );
	}
#endif
	return true;
}

//*****************************************************
// import the certificates from the card in a specific reader
//*****************************************************
bool MainWnd::ImportCertificates( QString const& readerName )
{
	return ImportCertificates( readerName.toLatin1().data() );
}

bool MainWnd::ImportCertificates( const char* readerName )
{
#ifdef WIN32
	if ( !readerName || 0 == strlen(readerName) )
	{
		return false;
	}

	PCCERT_CONTEXT	pCertContext = NULL;
	QString			strTip;
	bool			bImported	= false;

	PTEID_ReaderContext&  ReaderContext  = ReaderSet.getReaderByName(readerName);
	if (!ReaderContext.isCardPresent())
	{
		return false;
	}

	#ifdef WIN32
	//Register the 2 higher-level CA Certs from disk files
	ImportCCCert();
	ImportECRaizCert();
	#endif

	try
	{
		PTEID_EIDCard&		 Card			= ReaderContext.getEIDCard();
		PTEID_Certificates&	 certificates	= Card.getCertificates();

		for (size_t CertIdx=0;CertIdx<Card.certificateCount();CertIdx++)
		{
			PTEID_Certificate&	 cert			= certificates.getCertFromCard(CertIdx);
			const PTEID_ByteArray certData		= cert.getCertData();

			// ----------------------------------------------------
			// create the certificate context with the certificate raw data
			// ----------------------------------------------------
			pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

			if( pCertContext )
			{
				if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData,
				       pCertContext->pCertInfo->Subject.pbData,
					   pCertContext->pCertInfo->Subject.cbData
					   )
					)
					continue;
				unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
				CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

				// ----------------------------------------------------
				// Only store the context of the certificates with usages for an end-user 
				// i.e. no CA or root certificates
				// ----------------------------------------------------
				if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
				{
					if(StoreAuthorityCerts (pCertContext, KeyUsageBits, readerName))                     
					{
						bImported = true;
					}
				}
				else
				{
					if(StoreUserCerts (Card, pCertContext, KeyUsageBits, cert, readerName))
					{
						bImported = true;
					}
				}
				pCertContext = NULL;
			}
		}
	}
	catch (PTEID_Exception& e)
	{
		long err = e.GetError();
		err = err;
		QString strCaption(tr("Retrieving certificates"));
		QString strMessage(tr("Error retrieving certificates"));
		QMessageBox::information(NULL,strCaption,strMessage);
	}

	//	showCertImportMessage(bImported);

	return bImported;
#else
	return true;
#endif
}

//*****************************************************
// Show a messagebox that the certificate import was successful
//*****************************************************
void MainWnd::showCertImportMessage(bool bImported)
{
#ifdef WIN32
	QString caption(tr("Register certificate"));
	QString message(tr("Registration of certificate "));
	if (bImported)
	{
		message += tr("successful");
	}
	else
	{
		message += tr("failed");
	}
	m_pTrayIcon->showMessage(caption, message, QSystemTrayIcon::Information);
#endif
}

//*****************************************************
// Import the selected certificate
//*****************************************************
bool MainWnd::ImportSelectedCertificate( void )
{
#ifdef WIN32
	PCCERT_CONTEXT	pCertContext = NULL;
	QString			strTip;
	bool			bImported	= false;

	QList<QTreeWidgetItem *> listItem = m_ui.treeCert->selectedItems();
	if (!listItem.isEmpty()){

		QTreeCertItem *item = dynamic_cast<QTreeCertItem *>(listItem.first());

		const PTEID_ByteArray certData = item->getCert()->getCertData();
		PTEID_ReaderContext& ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		PTEID_EIDCard& Card	= ReaderContext.getEIDCard();

		// ----------------------------------------------------
		// create the certificate context with the certificate raw data
		// ----------------------------------------------------
		pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());

		if( pCertContext )
		{
			unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
			CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

			// ----------------------------------------------------
			// Only store the context of the certificates with usages for an end-user
			// ----------------------------------------------------
			if((KeyUsageBits & CERT_KEY_CERT_SIGN_KEY_USAGE) == CERT_KEY_CERT_SIGN_KEY_USAGE)
			{
				if(StoreAuthorityCerts (pCertContext, KeyUsageBits, m_CurrReaderName.toLatin1().data()))
				{
					bImported = true;
				}
			}
			else
			{
				if(StoreUserCerts (Card, pCertContext, KeyUsageBits, (*item->getCert()), m_CurrReaderName.toLatin1().data()))
				{

					//now store each time the issuer until we're done
					// an exception is thrown when there is no issuer
					bool bDone = false;
					PTEID_Certificate* currCert = item->getCert();
					while(!bDone)
					{
						try
						{
							PTEID_Certificate*	 issuer		= &currCert->getIssuer();
							const PTEID_ByteArray certData	= issuer->getCertData();

							pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
							StoreAuthorityCerts (pCertContext, KeyUsageBits, m_CurrReaderName.toLatin1().data());
							currCert	 = issuer;
						}
						catch (PTEID_ExCertNoIssuer& e)
						{
							long err = e.GetError();
							err = err;
							// ok, we're at the end of the tree
							bDone = true;
						}
					}

					bImported = true;
				}
			}
			pCertContext = NULL;
		}
		showCertImportMessage(bImported);
	}

	return bImported;
#else
	return true;
#endif
}

//*****************************************************
// store the authority certificates of the card in a specific reader
//*****************************************************
bool MainWnd::StoreAuthorityCerts(PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, const char* readerName)
{
#ifdef WIN32
	bool			bRet		 = false;
	HCERTSTORE		hMemoryStore = NULL;   // memory store handle
	PCCERT_CONTEXT  pDesiredCert = NULL;
	
	if ( 0 == memcmp ( pCertContext->pCertInfo->Issuer.pbData
			, pCertContext->pCertInfo->Subject.pbData
			, pCertContext->pCertInfo->Subject.cbData
	)
	)
	{
		hMemoryStore = CertOpenSystemStoreA (NULL, "ROOT");
	}
	else
	{
		hMemoryStore = CertOpenSystemStoreA (NULL, "CA");
	}

	if(NULL != hMemoryStore)
	{
		pDesiredCert = CertFindCertificateInStore( hMemoryStore
				, X509_ASN_ENCODING
				, 0
				, CERT_FIND_EXISTING
				, pCertContext
				, NULL
		);
		if( pDesiredCert )
		{
			CertFreeCertificateContext(pDesiredCert);
		}
		else
		{
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
			CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_SERVER_AUTH);
			if(CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
			{
				m_certContexts[readerName].push_back(pCertContext);
				bRet = true;
			}
		}
		CertCloseStore (hMemoryStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMemoryStore = NULL;
	}
	return bRet;
#else
	return true;
#endif
}

//*****************************************************
// store the user certificates of the card in a specific reader
//*****************************************************
bool MainWnd::StoreUserCerts (PTEID_EIDCard& Card, PCCERT_CONTEXT pCertContext, unsigned char KeyUsageBits, PTEID_Certificate& cert, const char* readerName)
{
#ifdef WIN32
	unsigned long	dwFlags			= CERT_STORE_NO_CRYPT_RELEASE_FLAG;
	PCCERT_CONTEXT  pDesiredCert	= NULL;
	PCCERT_CONTEXT  pPrevCert		= NULL;
	HCERTSTORE		hMyStore		= CertOpenSystemStore(NULL, "MY");

	if ( NULL != hMyStore )
	{
		// ----------------------------------------------------
		// look if we already have a certificate with the same 
		// subject (contains name and NNR) in the store
		// If the certificate is not found --> NULL
		// ----------------------------------------------------
		do
		{
			if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_SUBJECT_NAME, &(pCertContext->pCertInfo->Subject) , pPrevCert)))
			{
				// ----------------------------------------------------
				// If the certificates are identical and function 
				// succeeds, the return value is nonzero, or TRUE.
				// ----------------------------------------------------
				if(NULL == CertCompareCertificate(X509_ASN_ENCODING,pCertContext->pCertInfo,pDesiredCert->pCertInfo) ||
						!ProviderNameCorrect(pDesiredCert) )
				{
					// ----------------------------------------------------
					// certificates are not identical, but have the same 
					// subject (contains name and NNR),
					// so we remove the one that was already in the store
					// ----------------------------------------------------
					if(NULL == CertDeleteCertificateFromStore(pDesiredCert))
					{
						if (E_ACCESSDENIED == GetLastError())
						{
							QString strCaption(tr("Deleting former certificate"));
							QString strMessage(tr("Error deleting former certificate"));
							QMessageBox::information(NULL,strCaption,strMessage);
						}
					}
					pPrevCert = NULL;
					continue;
				}
			}
			pPrevCert = pDesiredCert;
		}while (NULL != pDesiredCert);


		if( NULL != (pDesiredCert = CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
		)
		{
			m_certContexts[readerName].push_back(pCertContext);
			// ----------------------------------------------------
			// certificate is already in the store, then just return
			// ----------------------------------------------------
			CertFreeCertificateContext(pDesiredCert);
			CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
			return true;
		}

		// ----------------------------------------------------
		// Initialize the CRYPT_KEY_PROV_INFO data structure.
		// Note: pwszContainerName and pwszProvName can be set to NULL 
		// to use the default container and provider.
		// ----------------------------------------------------
		CRYPT_KEY_PROV_INFO* pCryptKeyProvInfo	= new CRYPT_KEY_PROV_INFO;
		unsigned long		 dwPropId			= CERT_KEY_PROV_INFO_PROP_ID; 

		// ----------------------------------------------------
		// Get the serial number
		// ----------------------------------------------------
		PTEID_CardVersionInfo& versionInfo = Card.getVersionInfo();
		const char*			  pSerialKey  = versionInfo.getSerialNumber();

		QString				  strContainerName;

		if (UseMinidriver())
		{
			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				strContainerName = "NR_";
			}
			else
			{
				strContainerName = "DS_";
			}
			strContainerName += pSerialKey;
			pCryptKeyProvInfo->pwszProvName			= L"Microsoft Base Smart Card Crypto Provider";
			pCryptKeyProvInfo->dwKeySpec			= AT_SIGNATURE;
		}
		else
		{
			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				strContainerName = "Signature";
			}
			else
			{
				strContainerName = "Authentication";
			}

			strContainerName += "(";
			strContainerName += pSerialKey;
			strContainerName += ")";
			pCryptKeyProvInfo->pwszProvName		= L"Portugal Identity Card CSP";
			pCryptKeyProvInfo->dwKeySpec		= AT_KEYEXCHANGE;
		}
		pCryptKeyProvInfo->pwszContainerName	= (LPWSTR)strContainerName.utf16();
		pCryptKeyProvInfo->dwProvType			= PROV_RSA_FULL;
		pCryptKeyProvInfo->dwFlags				= 0;
		pCryptKeyProvInfo->cProvParam			= 0;
		pCryptKeyProvInfo->rgProvParam			= NULL;

		// Set the property.
		if (CertSetCertificateContextProperty(
				pCertContext,       // A pointer to the certificate
				// where the propertiy will be set.
				dwPropId,           // An identifier of the property to be set.
				// In this case, CERT_KEY_PROV_INFO_PROP_ID
				// is to be set to provide a pointer with the
				// certificate to its associated private key
				// container.
				dwFlags,            // The flag used in this case is
				// CERT_STORE_NO_CRYPT_RELEASE_FLAG
				// indicating that the cryptographic
				// context aquired should not
				// be released when the function finishes.
				pCryptKeyProvInfo   // A pointer to a data structure that holds
				// infomation on the private key container to
				// be associated with this certificate.
		))
		{
			if (NULL != pCryptKeyProvInfo)
			{
				delete pCryptKeyProvInfo;
				pCryptKeyProvInfo = NULL;
			}
			// Set friendly names for the certificates
			CRYPT_DATA_BLOB tpFriendlyName	= {0, 0};
			unsigned long	ulID			= 0;

			if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
			{
				ulID = 0x03;
			}
			else
			{
				ulID = 0x02;
			}

			QString			strFriendlyName;
			strFriendlyName = QString::fromUtf8(cert.getOwnerName());
			int iFriendLen = (strFriendlyName.length() + 1) * sizeof(QChar);

			tpFriendlyName.pbData = new unsigned char[iFriendLen];

			memset(tpFriendlyName.pbData, 0, iFriendLen);
			memcpy(tpFriendlyName.pbData, strFriendlyName.utf16(), iFriendLen - sizeof(QChar));

			tpFriendlyName.cbData = iFriendLen;

			if (CertSetCertificateContextProperty(
					pCertContext,       // A pointer to the certificate
					// where the propertiy will be set.
					CERT_FRIENDLY_NAME_PROP_ID,           // An identifier of the property to be set.
					// In this case, CERT_KEY_PROV_INFO_PROP_ID
					// is to be set to provide a pointer with the
					// certificate to its associated private key
					// container.
					dwFlags,            // The flag used in this case is
					// CERT_STORE_NO_CRYPT_RELEASE_FLAG
					// indicating that the cryptographic
					// context aquired should not
					// be released when the function finishes.
					&tpFriendlyName   // A pointer to a data structure that holds
					// infomation on the private key container to
					// be associated with this certificate.
			))
			{
				if (KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE)
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
				}
				else
				{
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION);
					CertAddEnhancedKeyUsageIdentifier (pCertContext, szOID_PKIX_KP_CLIENT_AUTH);
				}
				CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL);
				m_certContexts[readerName].push_back(pCertContext);
			}

			if (NULL != tpFriendlyName.pbData)
			{
				delete [] (tpFriendlyName.pbData);
				tpFriendlyName.pbData = NULL;
			}
		}
		CertCloseStore (hMyStore, CERT_CLOSE_STORE_FORCE_FLAG);
		hMyStore = NULL;
	}
#endif
	return true;
}

//*****************************************************
// button to register certificate clicked
//*****************************************************
void MainWnd::on_btnCert_Register_clicked( void )
{
#ifdef WIN32
	ImportSelectedCertificate();
#endif
}

//****************************************************
// Show certificate details (only for Windows)
// The certificate data is retrieved from the card and
// passed to the Windows dialog that displays the details
//****************************************************
void MainWnd::on_btnCert_Details_clicked( void )
{
#if WIN32
	QList<QTreeWidgetItem *> listItem = m_ui.treeCert->selectedItems();
	if (!listItem.isEmpty()){

		QTreeCertItem *item = dynamic_cast<QTreeCertItem *>(listItem.first());

		const PTEID_ByteArray certData = item->getCert()->getCertData();

		CRYPTUI_VIEWCERTIFICATE_STRUCT tCert = {0};
		tCert.dwSize		= sizeof(CRYPTUI_VIEWCERTIFICATE_STRUCT);
		tCert.hwndParent	= this->winId();
		tCert.dwFlags		= CRYPTUI_DISABLE_EDITPROPERTIES;
		tCert.pCertContext	= CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
		BOOL bChange		= FALSE;
		CryptUIDlgViewCertificate(&tCert, &bChange);
		CertFreeCertificateContext (tCert.pCertContext);
	} 
#endif
}

//*****************************************************
// a certificate has been selected
//*****************************************************
void MainWnd::on_treeCert_itemSelectionChanged ( void )
{
	QList<QTreeWidgetItem *> selectedItems = m_ui.treeCert->selectedItems();
	if (selectedItems.size()==1)
	{
		on_treeCert_itemClicked((QTreeCertItem*)selectedItems[0], 0);
	}

}
//*****************************************************
// a certificate has been clicked
//*****************************************************
void MainWnd::on_treeCert_itemClicked(QTreeWidgetItem* baseItem, int column)
{

	QTreeCertItem* item=(QTreeCertItem*)baseItem;

	if (!m_CI_Data.isDataLoaded())
		return;

	PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

	//------------------------------------
	// fill in the GUI
	//------------------------------------
	m_ui.txtCert_Issuer->setText( item->getIssuer());
	m_ui.txtCert_Issuer->setAccessibleName( item->getIssuer());
	m_ui.txtCert_Owner->setText( item->getOwner());
	m_ui.txtCert_Owner->setAccessibleName( item->getOwner());
	m_ui.txtCert_ValidFrom->setText(item->getValidityBegin());
	m_ui.txtCert_ValidFrom->setAccessibleName(item->getValidityBegin());
	m_ui.txtCert_ValidUntil->setText(item->getValidityEnd());
	m_ui.txtCert_ValidUntil->setAccessibleName(item->getValidityEnd());
	m_ui.txtCert_KeyLenght->setText(item->getKeyLen());
	m_ui.txtCert_KeyLenght->setAccessibleName(item->getKeyLen());

	if(!ReaderContext.isCardPresent())
	{
		m_ui.btnCert_Register->setEnabled(false);
		m_ui.btnCert_Details->setEnabled(false);
	}
	else if ( 0 < item->childCount())
	{
		m_ui.btnCert_Register->setEnabled(false);
		m_ui.btnCert_Details->setEnabled(true);
	}
	else
	{
		m_ui.btnCert_Register->setEnabled(true);
		m_ui.btnCert_Details->setEnabled(true);
	}
}


//*****************************************************
// PIN item selection changed
//*****************************************************
void MainWnd::on_treePIN_itemSelectionChanged ( void )
{
	QList<QTreeWidgetItem *> selectedItems = m_ui.treePIN->selectedItems();
	if (selectedItems.size()==1)
		setEnabledPinButtons(true);
	else
		setEnabledPinButtons(false);


	if (selectedItems.size()==1)
		on_treePIN_itemClicked((QTreeWidgetItem*)selectedItems[0], 0);
}

//*****************************************************
// A Pin is clicked
//*****************************************************
void MainWnd::on_treePIN_itemClicked(QTreeWidgetItem* item, int column)
{
	if (!m_CI_Data.isDataLoaded())
		return;

	int index = m_ui.treePIN->indexOfTopLevelItem(item);

	PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
	if (!ReaderContext.isCardPresent())
		return;


	/* BEGIN - the pin information have to be always sinchronized - quick fix
	 * this chunk of code can be removed if in the future the pin information sinchronization requirement is dropped
	 */
	unsigned int _pinRef = item->data(0,Qt::UserRole).value<uint>();
	PTEID_EIDCard&	Card	= ReaderContext.getEIDCard();
	PTEID_Pins&		Pins	= Card.getPins();

	PTEID_Pin& pin = Pins.getPinByPinRef(_pinRef);
	delete m_pinsInfo[_pinRef]; // doesn't make sense, but this way the quick fix can be removed with no harm
	m_pinsInfo[_pinRef] = new PinInfo(pin.getId(), pin.getLabel(), pin.getTriesLeft());
	/* END - the pin information have to be always sinchronized */


	unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
	PinInfo* pinfo = m_pinsInfo.find(pinRef)->second;

	QString status;
	if (1 == pinfo->triesLeft)
		status = tr("%1 try remaining").arg(pinfo->triesLeft);
	else
		status = tr("%1 tries remaining").arg(pinfo->triesLeft);
	m_ui.txtPIN_Name->setText(pinfo->pin_name);
	m_ui.txtPIN_Name->setAccessibleName(pinfo->pin_name);
	m_ui.txtPIN_ID->setText(pinfo->pin_id);
	m_ui.txtPIN_ID->setAccessibleName(pinfo->pin_id);
	m_ui.txtPIN_Status->setText(status);
	m_ui.txtPIN_Status->setAccessibleName(status);
	setEnabledPinButtons(pinfo->triesLeft > 0);
}

//****************************************************
// Callback function used by the Readercontext to notify insertion/removal
// of a card
// The callback comes at:
// - startup
// - insertion of a card
// - removal of a card
// - add/remove of a cardreader
// When a card is inserted we post a custom event to the GUI telling that
// a new card is inserted. A postEvent is called because this function is
// called from another thread than the main GUI thread.
//****************************************************
void cardEventCallback(long lRet, unsigned long ulState, CallBackData* pCallBackData)
{
	g_runningCallback++;

	try
	{
		PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName(pCallBackData->getReaderName().toLatin1());

		//------------------------------------
		// is card retracted from reader?
		//------------------------------------
		if (!readerContext.isCardPresent())
		{
			if ( pCallBackData->getMainWnd()->getSettings().getRemoveCert())
			{
				//------------------------------------
				// remove the certificates
				//------------------------------------
				MainWnd::RemoveCertificates(pCallBackData->getReaderName() );
			}
			PopupEvent* event = new PopupEvent(pCallBackData->getReaderName(),PopupEvent::ET_CARD_REMOVED);
			QCoreApplication::postEvent(pCallBackData->getMainWnd(),event);

			g_runningCallback--;
			return;
		}
		//------------------------------------
		// is card inserted ?
		//------------------------------------
		if (readerContext.isCardChanged(pCallBackData->m_cardID))
		{
			//------------------------------------
			// send an event to the main app to show the picture
			// we must use the postEvent since this callback function resides in a different thread than the
			// main thread.
			//------------------------------------

			PopupEvent* event = new PopupEvent(pCallBackData->getReaderName(),PopupEvent::ET_CARD_CHANGED);
			QCoreApplication::postEvent(pCallBackData->getMainWnd(),event);
		}
	}
	catch (PTEID_ExBadTransaction& e)
	{
		long err = e.GetError();
		err = err;
	}
	catch (...)
	{
		int x=0;
		x++;
		// we catch ALL exceptions. This is because otherwise the thread throwing the exception stops		
	}

	g_runningCallback--;
}

//*****************************************************
// forget all the certificates we kept for all readers
//*****************************************************
void MainWnd::forgetAllCertificates( void )
{
#ifdef WIN32
	bool bRefresh = true;;
	for (unsigned long readerIdx=0; readerIdx<ReaderSet.readerCount(bRefresh); readerIdx++)
	{
		const char* readerName = ReaderSet.getReaderByNum(readerIdx).getName();
		forgetCertificates(readerName);
	}
#endif
}

//*****************************************************
// forget all the certificates we kept for a specific reader
//*****************************************************
void MainWnd::forgetCertificates( QString const& reader )
{
	char readerName[256];
	readerName[0]=0;
	if (reader.length()>0)
	{
		strcpy(readerName,reader.toUtf8().data());
	}
#ifdef WIN32
	while ( 0 < m_certContexts[readerName].size() )
	{
		PCCERT_CONTEXT pContext = m_certContexts[readerName][m_certContexts[readerName].size()-1];
		CertFreeCertificateContext(pContext);
		m_certContexts[readerName].erase(m_certContexts[readerName].end()-1);
	}
#endif
}

//*****************************************************
// Ask if it is a test card
//*****************************************************
bool MainWnd::askAllowTestCard( void )
{
	//----------------------------------------------------------------
	// try to lock a mutex.
	// This is done for the case there are 2 or more cards inserted.
	// When 2 cards are inserted simultaneously, the card readers will post
	// each a custom event. The first event coming in will check if it is
	// a testcard and end up here. The messagebox will pop up and wait for either OK or CANCEL.
	// In the meantime, the second custum event will be processed by the main messageloop.
	// As a consequence, the card will also be tested to see if it is a testcard.
	// That means that we end up here again and a second messagebox is popped up.
	// To avoid 2 messageboxes to be popped up, we use a mutex.
	// The first request to allow a testcard will test-and-lock the mutex and popup the messagebox.
	// The second request will test the mutex, but will be unable to lock the mutex and return.
	// No second messagebox is popped up
	//----------------------------------------------------------------

	if ( !m_mutex.tryLock () )
	{	
		return false;
	}
	bool    bAllowTestCard = false;
	QString strCaption     = tr("Portuguese ID Card Management");
	QString strMessage     = tr("The Root Certificate is not correct.\nThis may be a test card.\n\nDo you want to accept it?");

	if (QMessageBox::Yes == QMessageBox::warning(this,strCaption,strMessage,QMessageBox::Yes|QMessageBox::No))
	{
		bAllowTestCard = true;
	}
	m_mutex.unlock();
	return bAllowTestCard;
}

//*****************************************************
// remove the virtual reader
//*****************************************************
void MainWnd::releaseVirtualReader( void )
{
	if (m_virtReaderContext)
	{
		delete m_virtReaderContext;
		m_virtReaderContext = NULL;
	}
}
//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardData( void )
{
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				PTEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS101:
				{
					try
					{
						PTEID_EIDCard& Card = ReaderContext.getEIDCard();
						if (Card.isTestCard()&&!Card.getAllowTestCard())
						{
							if (askAllowTestCard())
							{
								Card.setAllowTestCard(true);
							}
							else
							{
								on_actionClear_triggered();
								break;
							}
						}
						const char* readerName = ReaderSet.getReaderName(ReaderIdx);
						m_CurrReaderName = readerName;
						Show_Identity_Card(Card);
						//Show_Address_Card(Card);

						ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
					}
					catch (PTEID_ExCardBadType const& e)
					{
						QString errcode;
                        PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "loadCardData failed %s", e.GetError());
						errcode = errcode.setNum(e.GetError());
					}
				}
				break;
				case PTEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != PTEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowPTEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (PTEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardBadType e) 
	{
		QString msg(tr("Bad card type"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowPTEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (PTEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowPTEIDError( e.GetError(), msg );
	}
}


//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardDataAddress( void )
{
    if (!m_CI_Data.isDataLoaded())
        return;
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				PTEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS101:
				{
					try
					{
						PTEID_EIDCard& Card = ReaderContext.getEIDCard();
						if (Card.isTestCard()&&!Card.getAllowTestCard())
						{
							if (askAllowTestCard())
							{
								Card.setAllowTestCard(true);
							}
							else
							{
								on_actionClear_triggered();
								break;
							}
						}
						const char* readerName = ReaderSet.getReaderName(ReaderIdx);
						m_CurrReaderName = readerName;
						Show_Address_Card(Card);

						ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
					}
					catch (PTEID_ExCardBadType const& e)
					{
						QString errcode;
						errcode = errcode.setNum(e.GetError());
					}
				}
				break;
				case PTEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != PTEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowPTEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (PTEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowPTEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (PTEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowPTEIDError( e.GetError(), msg );
	}
}

//*****************************************************
// load the card data
//*****************************************************
bool MainWnd::loadCardDataPersoData( void )
{
    if (!m_CI_Data.isDataLoaded())
        return false;

	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				PTEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS101:
				{
					try
					{
						PTEID_EIDCard& Card = ReaderContext.getEIDCard();
						if (Card.isTestCard()&&!Card.getAllowTestCard())
						{
							if (askAllowTestCard())
							{
								Card.setAllowTestCard(true);
							}
							else
							{
								on_actionClear_triggered();
								break;
							}
						}
						const char* readerName = ReaderSet.getReaderName(ReaderIdx);
						m_CurrReaderName = readerName;
						Show_PersoData_Card(Card);

						ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
					}
					catch (PTEID_ExCardBadType const& e)
					{
						QString errcode;
						errcode = errcode.setNum(e.GetError());
					}
				}
				break;
				case PTEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowPTEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (PTEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowPTEIDError( e.GetError(), msg );
        m_CI_Data.Reset();
		loadCardData();
	}
	catch (PTEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowPTEIDError( e.GetError(), msg );
	}
	return true;
}

//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardDataCertificates( void )
{
  
	//----------------------------------------------------------------
	// if we load a new card, clear the certificate contexts we kept
	//----------------------------------------------------------------
	try
	{
		unsigned long	ReaderStartIdx = m_Settings.getSelectedReader();
		bool			bRefresh	   = false;
		unsigned long	ReaderEndIdx   = ReaderSet.readerCount(bRefresh);
		unsigned long	ReaderIdx	   = 0;

		if (ReaderStartIdx!=(unsigned long)-1)
		{
			ReaderEndIdx = ReaderStartIdx+1;
		}
		else
		{
			ReaderStartIdx=0;
		}

		bool bCardPresent = false;
		PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;
		for (ReaderIdx=ReaderStartIdx; ReaderIdx<ReaderEndIdx;ReaderIdx++)
		{
			PTEID_ReaderContext& ReaderContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (ReaderContext.isCardPresent())
			{
				bCardPresent = true;
				PTEID_CardType CardType = ReaderContext.getCardType();
				lastFoundCardType = CardType;
				switch (CardType)
				{
				case PTEID_CARDTYPE_IAS07:
				case PTEID_CARDTYPE_IAS101:
				{
					try
					{
						PTEID_EIDCard& Card = ReaderContext.getEIDCard();
						if (Card.isTestCard()&&!Card.getAllowTestCard())
						{
							if (askAllowTestCard())
							{
								Card.setAllowTestCard(true);
							}
							else
							{
								on_actionClear_triggered();
								break;
							}
						}
						const char* readerName = ReaderSet.getReaderName(ReaderIdx);
						m_CurrReaderName = readerName;
						Show_Certificates_Card(Card);

						ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card
					}
					catch (PTEID_ExCardBadType const& e)
					{
						QString errcode;
						errcode = errcode.setNum(e.GetError());
					}
				}
				break;
				case PTEID_CARDTYPE_UNKNOWN:
				default:
					break;
				}
			}
			else
			{
				clearGuiContent();
				if (lastFoundCardType != PTEID_CARDTYPE_UNKNOWN)
				{
					clearGuiContent();
				}
			}
			enablePrintMenu();
		}
		if (!m_CI_Data.isDataLoaded() || !bCardPresent)
		{
			QString strCaption(tr("Reload eID"));
			strCaption = strCaption.remove(QChar('&'));
			QString strMessage(tr("No card found"));
			m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);
		}
		else if (lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
		{
			QString msg(tr("Unknown card type"));
			ShowPTEIDError( 0, msg );
			clearGuiContent();
		}
		enableFileMenu();
	}
	catch (PTEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardBadType e)
	{
		QString msg(tr("Bad card type"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowPTEIDError( e.GetError(), msg );
		m_CI_Data.Reset();
		loadCardData();
	}
	catch (PTEID_ExReaderSetChanged e)
	{
		QString msg(tr("Readers changed"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExNotAllowByUser& e)
	{
		QString msg(tr("Not allowed by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExUserMustAnswer& e)
	{
		QString msg(tr("Not allowed yet by user"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (PTEID_Exception e)
	{
		QString msg(tr("Error loading card data"));
		ShowPTEIDError( e.GetError(), msg );
	}
}
//*****************************************************
// Test PIN clicked
//*****************************************************
void MainWnd::on_btnPIN_Test_clicked()
{
	on_actionPINRequest_triggered();
}

//*****************************************************
// Change PIN clicked
//*****************************************************
void MainWnd::on_btnPIN_Change_clicked()
{
	on_actionPINChange_triggered();
}

//*****************************************************
// Updates clicked
//*****************************************************
void MainWnd::on_actionUpdates_triggered( void )
{
	//Verify OS
	AutoUpdates r;
	r.show();
	r.exec();
}

//*****************************************************
// About clicked
//*****************************************************
void MainWnd::show_window_about(){
#ifdef WIN32 //version info for Windows
	QFileInfo	fileInfo(m_Settings.getExePath()) ;

	QString filename = QCoreApplication::arguments().at(0);
	CFileVersionInfo VerInfo;
	if(VerInfo.Open(filename.toLatin1()))
	{
		char version[256];
		VerInfo.QueryStringValue(VI_STR_FILEVERSION, version);
		m_Settings.setGuiVersion(version);
	}

#else //linux, apple
	QString strVersion (WIN_GUI_VERSION_STRING);
	m_Settings.setGuiVersion(strVersion);
#endif
	dlgAbout * dlg = new dlgAbout( m_Settings.getGuiVersion() , this);
	dlg->exec();
	delete dlg;
}


void MainWnd::on_actionAbout_triggered( void )
{
	show_window_about();
}

//*****************************************************
// Options clicked
//*****************************************************
void MainWnd::show_window_parameters(){
	if( this->isHidden() )
	{
		this->showNormal(); // Otherwise the application will end if the options dialog gets closed
	}

	dlgOptions* dlg = new dlgOptions( m_Settings, this );
	dlg->setShowToolbar( m_Settings.getShowToolbar() );
	dlg->setShowPicture( m_Settings.getShowPicture() );
	dlg->setShowNotification( m_Settings.getShowNotification() );

	m_ui.actionOptions->setEnabled(false);

	if( dlg->exec() )
	{
		m_ui.actionShow_Toolbar->setChecked( m_Settings.getShowToolbar() );

		if( !m_ui.txtIdentity_Name->text().isEmpty() )
			m_ui.lblIdentity_ImgPerson->setPixmap( m_imgPicture );
	}
	delete dlg;
	m_ui.actionOptions->setEnabled(true);
}

void MainWnd::on_actionOptions_triggered(void)
{
	show_window_parameters();
}

//*****************************************************
// Signature clicked
//*****************************************************
void MainWnd::actionSignature_eID_triggered()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();
	QString cardTypeText = GetCardTypeText(CardFields[CARD_TYPE]);
	if(m_CI_Data.isDataLoaded())
	{
		dlgSignature* dlgsig = new dlgSignature( this, m_CI_Data);
		dlgsig->exec();
		delete dlgsig;
	} else {
		QString caption  = tr("Warning");
	  	QString msg = tr("A problem has occurred while trying to read card. Please, try again.");
	  	//std::string Pmsgbody
	  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	  	msgBoxp.exec();
	}
}

void MainWnd::actionPDFSignature_triggered()
{

//	if(m_CI_Data.isDataLoaded())
//	{
		PDFSignWindow* dlgPDFSig = new PDFSignWindow(this, m_CI_Data);
		dlgPDFSig->exec();
//	}
       /*	else {
		QString caption  = tr("Warning");
	  	QString msg = tr("A problem has occurred while trying to read card. Please, try again.");
	  	//std::string Pmsgbody
	  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	  	msgBoxp.exec();
	}
*/

}

//*****************************************************
// VerifySignature clicked
//*****************************************************
void MainWnd::actionVerifySignature_eID_triggered()
{
    dlgVerifySignature* dlgversig = new dlgVerifySignature( this);
    dlgversig->exec();
    delete dlgversig;
}

//*****************************************************
// Print clicked
//*****************************************************
void MainWnd::on_actionPrint_eID_triggered()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();
	QString cardTypeText = GetCardTypeText(CardFields[CARD_TYPE]);
	if(m_CI_Data.isDataLoaded())
	{
	  	dlgPrint* dlg = new dlgPrint( this, m_CI_Data, m_Language, cardTypeText);
		dlg->exec();
		delete dlg;
	} else {
		QString caption  = tr("Warning");
		QString msg = tr("A problem has occurred while trying to read card. Please, try again.");
		QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
		msgBoxp.exec();
	}
}

void MainWnd::on_actionPrinter_Settings_triggered()
{
	QPrintDialog* pPrintDialog = new QPrintDialog( m_pPrinter, this );
	pPrintDialog->exec();
	delete pPrintDialog;
}

//*****************************************************
// Authentication Pin request
//*****************************************************
void MainWnd::authPINRequest_triggered()
{
	if (!m_CI_Data.isDataLoaded())
		return;

	try
	{
		PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		QString	caption(tr("Identity Card: PIN verification"));
		QString msg;

		if (ReaderContext.isCardPresent())
		{
			PTEID_Pin &pin = ReaderContext.getEIDCard().getPins().getPinByPinRef(PTEID_Pin::AUTH_PIN);

			if (pin.getTriesLeft() == 0)
			{
				QString msg = tr("Maximum number of"
						" PIN entry attempts has been reached! Couldn't save personal notes");

				QMessageBox::critical(this, caption,  msg, QMessageBox::Ok);
				return;
			}

			unsigned long triesLeft = -1;
			bool bResult   = pin.verifyPin("",triesLeft);

			if (!bResult && -1 == triesLeft)
				return;

			m_pinsInfo.find(PTEID_Pin::AUTH_PIN)->second->triesLeft = triesLeft;

			QString status;
			if (1 == triesLeft)
				status = tr("%1 try remaining").arg(triesLeft);
			else
				status = tr("%1 tries remaining").arg(triesLeft);

			if (bResult){
				pinNotes=0;
			} else {
				msg = tr("PIN verification failed");
				msg += "\n";
				msg += status;
			}
		}
		else
			msg = tr("No card present");

		if (!msg.isEmpty())
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
	}
	catch (PTEID_Exception &e)
	{
		QString msg;

		switch(e.GetError()){
		case EIDMW_ERR_PIN_CANCEL:
			// show nothing user pressed cancel.
			return;
		case EIDMW_ERR_TIMEOUT:
			msg = tr("Time for entering PIN expired (30 seconds)");
			break;
		default:
			msg = tr("General exception");
		}
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( 0, msg );
	}
	return;
}

//*****************************************************
// Address Pin request
//*****************************************************
bool MainWnd::addressPINRequest_triggered()
{
	try
	{
		PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

		QString caption(tr("Identity Card: PIN verification"));
		QString msg;

		if (ReaderContext.isCardPresent())
		{
			PTEID_Pin &pin = ReaderContext.getEIDCard().getPins().getPinByPinRef(PTEID_Pin::ADDR_PIN);

			if (pin.getTriesLeft() == 0)
			{
				QString msg = tr("The address cannot be accessed because the maximum number of"
					" PIN entry attempts has been reached");

				QMessageBox::critical(this, caption,  msg, QMessageBox::Ok);
				return false;
			}

			unsigned long triesLeft = -1;
			bool bResult   = pin.verifyPin("",triesLeft);

			if (!bResult && -1 == triesLeft)
				return false;

			m_pinsInfo.find(PTEID_Pin::ADDR_PIN)->second->triesLeft = triesLeft;

			QString status;
			if (1 == triesLeft)
				status = tr("%1 try remaining").arg(triesLeft);
			else
				status = tr("%1 tries remaining").arg(triesLeft);

			if (bResult){
				pinactivate = 0;
			} else {
				msg = tr("PIN verification failed");
				msg += "\n";
				msg += status;
				pinactivate = 1;
			}
		}
		else
			msg = tr("No card present");

		if (!msg.isEmpty())
			QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
		return pinactivate == 0;
	}
	catch (PTEID_Exception &e)
	{
		QString msg;

		switch(e.GetError()){
		case EIDMW_ERR_PIN_CANCEL:
			// show nothing user pressed cancel.
			return false;
		case EIDMW_ERR_TIMEOUT:
			msg = tr("Time for entering PIN expired (30 seconds)");
			break;
		default:
			msg = tr("General exception");
		}
		ShowPTEIDError( e.GetError(), msg );

		return false;
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( 0, msg );
		return false;
	}
}

//*****************************************************
// Pin test button clicked
//*****************************************************
void MainWnd::on_actionPINRequest_triggered()
{
	if (!m_CI_Data.isDataLoaded())
		return;

	try
	{
		PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		QString		 caption(tr("Identity Card: PIN verification"));
		QString msg;

		if (ReaderContext.isCardPresent())
		{
			QTreeWidgetItem *item = m_ui.treePIN->currentItem();

			if (!item)
				return;

			unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
			PTEID_Pin &pin = ReaderContext.getEIDCard().getPins().getPinByPinRef(pinRef);

			unsigned long triesLeft = -1;
			bool bResult   = pin.verifyPin("",triesLeft);

			if (!bResult && -1 == triesLeft)
				return;

			m_pinsInfo.find(pinRef)->second->triesLeft = triesLeft;

			QString status;
			if (1 == triesLeft)
				status = tr("%1 try remaining").arg(triesLeft);
			else
				status = tr("%1 tries remaining").arg(triesLeft);

			if (bResult){
				msg = tr("PIN verification passed");
			} else {
				msg = tr("PIN verification failed");
				msg += "\n";
				msg += status;
			}

			m_ui.txtPIN_Status->setText(status);
			m_ui.txtPIN_Status->setAccessibleName(status);
		}
		else
			msg = tr("No card present");

		QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
	}
	catch (PTEID_Exception &e)
	{
		QString msg;

		switch(e.GetError()){
		case EIDMW_ERR_PIN_CANCEL:
			// show nothing user pressed cancel.
			return;
		case EIDMW_ERR_TIMEOUT:
			msg = tr("Time for entering PIN expired (30 seconds)");
			break;
		default:
			msg = tr("General exception");
		}
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( 0, msg );
	}
	return;
}

void MainWnd::ChangeAuthPin(PTEID_ReaderContext &ReaderContext, unsigned int pin_ref)
{

	PTEID_EIDCard &card = ReaderContext.getEIDCard();
	//Show AskPins dialogs
	PTEID_Pin &pin = card.getPins().getPinByPinRef(pin_ref);
	bool bResult = false; 
	bool verify_ret = false; 
	wchar_t wsPin1[PIN_MAX_LENGTH+1];
	wchar_t wsPin2[PIN_MAX_LENGTH+1];
	char old_pin[PIN_MAX_LENGTH+1];
	char new_pin[PIN_MAX_LENGTH+1];
	QString dialog_title(tr("Change Authentication and OTP PIN"));

	//Some Way to detect pinpads to show different message
	QString msg(tr("Attention: your current authentication PIN will be requested twice "
				"in two different dialog boxes."));
	QMessageBox msgBoxcc(QMessageBox::Information, dialog_title, msg, 0, this);
	msgBoxcc.setModal(true);
	msgBoxcc.exec();

	DlgPinInfo pinInfo = {4, 8, PIN_FLAG_DIGITS};

	DlgRet ret = DlgAskPins(DLG_PIN_OP_CHANGE,
			DLG_PIN_AUTH, L"Pin de Autenticação",
			pinInfo, wsPin1,PIN_MAX_LENGTH+1, 
		pinInfo, wsPin2,PIN_MAX_LENGTH+1);

	if (ret == DLG_OK)
	{
		strcpy(old_pin, utilStringNarrow(std::wstring(wsPin1)).c_str());
		strcpy(new_pin, utilStringNarrow(std::wstring(wsPin2)).c_str());
	} 
	else
	   return;	
	
	// Perform a verifyPin with no interaction (to rule out pinpad readers like Xiring)
	// and abort if wrong/blocked/not supported
	unsigned long triesLeft = -1;
	try
	{
		verify_ret = pin.verifyPin(old_pin,
				triesLeft, false);
	}
	catch (PTEID_Exception &ex)
	{
		if (strstr(m_CurrReaderName.toLatin1().data(), "Xiring")!= NULL)
		{
			QString pinpad_msg(tr("Sorry, this operation is unsupported on this reader"));

			QMessageBox msgBoxcc(QMessageBox::Critical, dialog_title, pinpad_msg, 0, this);
			msgBoxcc.setModal(true) ;
			msgBoxcc.exec();
			return;
		}

	}

	if (!verify_ret)
	{
		
		//Some Way to detect pinpads to show different message
		QString msg(tr("Verification of your current PIN failed"));
		QMessageBox msgBoxcc(QMessageBox::Critical, dialog_title, msg, 0, this);
		msgBoxcc.setModal(true) ;
		msgBoxcc.exec();

		return;
	}
	
	//Call ChangeCapPin
	try
	{

	    card.ChangeCapPin(new_pin);

	}
	catch(PTEID_Exception &ex)
	{
		QString msg = "";
		switch(ex.GetError())
		{
			case EIDMW_OTP_CONNECTION_ERROR:
			msg = tr("Error connecting to the OTP Server. Please check your internet connection.");
			break;
			case EIDMW_OTP_PROTOCOL_ERROR:
			msg = tr("Unexpected error in the OTP Server results. Aborting Pin change operation");
			break;
			case EIDMW_OTP_CERTIFICATE_ERROR:
			msg = tr("Error connecting to the OTP Server. Your authentication certificate was rejected");
			case EIDMW_OTP_UNKNOWN_ERROR:
			msg = tr("Error connecting to the OTP Server.");
			break;
		default:
			msg = tr("General exception");
		}

		ShowPTEIDError( ex.GetError(), msg );
		return;
	}
	
	try
	{
		// Actually change the Auth Pin with no interaction
		bResult = pin.changePin(old_pin, new_pin, triesLeft, pin.getLabel());
	}
	catch(PTEID_Exception &ex)
	{

		QString msg(tr("Error ocurred changing the authentication PIN. "
				"Please try again to avoid out-of-sync PINs"));
		QMessageBox msgBoxcc(QMessageBox::Warning, dialog_title, msg, 0, this);
		msgBoxcc.setModal(true) ;
		msgBoxcc.exec();
		return;

	}
	if (!bResult && -1 == triesLeft)
	{
	//TODO: another error msg
	    return;

	}
	QString msg_tmp(tr("PIN change passed"));
	QMessageBox::information( this, dialog_title, msg_tmp, QMessageBox::Ok );


}

//*****************************************************
// PIN change button clicked
//*****************************************************
void MainWnd::on_actionPINChange_triggered()
{
	if (!m_CI_Data.isDataLoaded())
	{
		return;
	}
	try
	{
		PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		QString	caption(tr("Identity Card: PIN change"));
		QString msg;

		if (ReaderContext.isCardPresent())
		{
			QTreeWidgetItem *item = m_ui.treePIN->currentItem();

			if (!item)
				return;

			unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
			if (pinRef == 0x01 || pinRef == 0x81)
			{
				ChangeAuthPin(ReaderContext, pinRef);
				return;
			}
			PTEID_Pin &pin = ReaderContext.getEIDCard().getPins().getPinByPinRef(pinRef);

			unsigned long triesLeft = -1;
			bool bResult = pin.changePin("","",triesLeft, pin.getLabel());

			if (!bResult && -1 == triesLeft)
				return;

			m_pinsInfo.find(pinRef)->second->triesLeft = triesLeft;

			QString status;
			if (1 == triesLeft)
				status = tr("%1 try remaining").arg(triesLeft);
			else
				status = tr("%1 tries remaining").arg(triesLeft);

			if (bResult){
				msg = tr("PIN change passed");
			} else {
				msg = tr("PIN change failed");
				msg += "\n";
				msg += status;
			}

			m_ui.txtPIN_Status->setText(status);
			m_ui.txtPIN_Status->setAccessibleName(status);
		}
		else
			msg = tr("No card present");

		QMessageBox::information( this, caption,  msg, QMessageBox::Ok );
	}
	catch (PTEID_Exception &e)
	{
		QString msg;

		switch(e.GetError()){
		case EIDMW_ERR_PIN_CANCEL:
			// show nothing user pressed cancel.
			return;
		case EIDMW_ERR_TIMEOUT:
			msg = tr("Time for entering PIN expired (30 seconds)");
			break;
		default:
			msg = tr("General exception");
		}
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( 0, msg );
	}
}

//******************************************************
// Show the tabs 
//******************************************************
void MainWnd::showTabs()
{
	m_ui.stackedWidget->setCurrentIndex(0);

	if ( !m_CI_Data.isDataLoaded() )
	{
		return;
	}

	switch (m_TypeCard) 
	{
	case PTEID_CARDTYPE_IAS07:
	case PTEID_CARDTYPE_IAS101:

		if(PTEID_EIDCard::isApplicationAllowed())
		{
			refreshTabIdentity();
			refreshTabIdentityExtra();
		}

		refreshTabCardPin();
		refreshTabInfo();

		break;

	default:
		break;
	}

	setLanguage();
	m_ui.stackedWidget->setCurrentIndex(1);
	m_ui.btnSelectTab_Identity->setFocus();
}

//*****************************************************
// Set the First tab as Visible
//*****************************************************
void MainWnd::Show_Splash()
{
	m_TypeCard = PTEID_CARDTYPE_UNKNOWN;
	m_ui.stackedWidget->setCurrentIndex(0);
}


//*****************************************************
// show the tabs for an ID card
//*****************************************************
void MainWnd::Show_Identity_Card(PTEID_EIDCard& Card)
{
	LoadDataID(Card);
	showTabs();
	enableFileMenu();
}

void MainWnd::Show_Address_Card(PTEID_EIDCard& Card)
{
	LoadDataAddress(Card);
}

void MainWnd::Show_PersoData_Card(PTEID_EIDCard& Card)
{
	LoadDataPersoData(Card);
}

void MainWnd::Show_Certificates_Card(PTEID_EIDCard& Card)
{
	LoadDataCertificates(Card);
}

QTreeCertItem* MainWnd::buildTree(PTEID_Certificate &cert, bool &bEx){
	if (cert.isRoot())
		return new QTreeCertItem(m_ui.treeCert,0,cert);
	else {
		QList<QTreeWidgetItem *> listItem = m_ui.treeCert->findItems(QString::fromUtf8(cert.getOwnerName()), Qt::MatchContains | Qt::MatchRecursive);
		if (listItem.isEmpty() || dynamic_cast<QTreeCertItem *>(listItem.first())->getLabel().compare(QString::fromUtf8(cert.getLabel()))!=0){
			try {
				return new QTreeCertItem(buildTree(cert.getIssuer(),bEx),0,cert);
			} catch (PTEID_ExCertNoIssuer &ex){
				bEx = true;
				return new QTreeCertItem(m_ui.treeCert,0,cert);
			}
		}
		else
			return dynamic_cast<QTreeCertItem *>(listItem.first());
	}
}

void MainWnd::fillCertificateList( void )
{
	bool noIssuer = false;

	PTEID_Certificates* certificates = m_CI_Data.m_CertifInfo.getCertificates();

	if (!certificates)
		return;

	buildTree(certificates->getCert(PTEID_Certificate::CITIZEN_AUTH),noIssuer);
	buildTree(certificates->getCert(PTEID_Certificate::CITIZEN_SIGN),noIssuer);

	m_ui.treeCert->expandAll();
	m_ui.treeCert->setColumnCount(1);
	m_ui.treeCert->sortItems(0,Qt::DescendingOrder);
	if (m_ui.treeCert->topLevelItem(0))
		m_ui.treeCert->setCurrentItem (m_ui.treeCert->topLevelItem(0));

	if (noIssuer){
		QString title = tr("Certification path");
		QString msg = tr("The certificates could not be validated, the certification path is not complete");
		QMessageBox msgBoxcc(QMessageBox::Warning, title, msg, 0, this);
		msgBoxcc.setModal(true) ;
		msgBoxcc.exec();
	}
}

//**************************************************
// Load data of the ID card
//**************************************************
void MainWnd::LoadDataID(PTEID_EIDCard& Card)
{


	//Progress bar was already hidden so proceed updating the GUI
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();

	if(!m_CI_Data.isDataLoaded())
	{
		
		//Load data from card in a new thread
		CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName, this);
		QFuture<void> future = QtConcurrent::run(&loader, &CardDataLoader::Load);
		this->FutureWatcher.setFuture(future);
		m_progress->exec();

		//Load the picture in PNG format
		imgPicture = QImage();

		imgPicture.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);
		imgPicturescaled = imgPicture.scaled(150, 190);
		m_imgPicture = QPixmap::fromImage(imgPicturescaled);

		fillPinList();
	}
}


void MainWnd::LoadDataAddress(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();
	m_CI_Data.LoadDataAddress(Card,m_CurrReaderName);
	if(!m_CI_Data.isDataLoaded())
	{
		//clearTabCertificates();
		//clearTabAddress();

		//clearTabPins();
		//fillPinList( Card );
	}
}

void MainWnd::LoadDataPersoData(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
    m_ui.btnPersoDataSave->setEnabled(true);
	m_TypeCard = Card.getType();
	CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName);
	QFuture<void> future = QtConcurrent::run(loader, &CardDataLoader::LoadPersoData);
	this->FutureWatcher.setFuture(future);
	m_progress->exec();
	
}

void MainWnd::LoadDataCertificates(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();

	CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName);
	QFuture<void> future = QtConcurrent::run(loader, &CardDataLoader::LoadCertificateData);
	this->FutureWatcher.setFuture(future);
	m_progress->exec();

	clearTabCertificates();
	fillCertificateList();
	
}

#define TYPE_PINTREE_ITEM 0
#define COLUMN_PIN_NAME   0

void MainWnd::fillPinList()
{
	QTreeWidgetItem* pinTreeItem;

	clearTabPins();

	pinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );
	pinTreeItem->setText(COLUMN_PIN_NAME, m_pinsInfo[PTEID_Pin::AUTH_PIN]->pin_name);
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::AUTH_PIN));

	pinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );
	pinTreeItem->setText(COLUMN_PIN_NAME, m_pinsInfo[PTEID_Pin::SIGN_PIN]->pin_name);
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::SIGN_PIN));

	pinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );
	pinTreeItem->setText(COLUMN_PIN_NAME, m_pinsInfo[PTEID_Pin::ADDR_PIN]->pin_name);
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::ADDR_PIN));

	m_ui.treePIN->expandAll();

	m_ui.treePIN->topLevelItem(0)->setSelected(true);
	m_ui.treePIN->setCurrentItem (m_ui.treePIN->topLevelItem(0));
}

void MainWnd::loadPinData(PTEID_EIDCard& Card){

	PTEID_Pins& Pins = Card.getPins();

	PTEID_Pin& pinAuth = Pins.getPinByPinRef(PTEID_Pin::AUTH_PIN);
	m_pinsInfo[PTEID_Pin::AUTH_PIN] = new PinInfo(pinAuth.getId(), pinAuth.getLabel(), pinAuth.getTriesLeft());

	PTEID_Pin& pinSign = Pins.getPinByPinRef(PTEID_Pin::SIGN_PIN);
	m_pinsInfo[PTEID_Pin::SIGN_PIN] = new PinInfo(pinSign.getId(), pinSign.getLabel(), pinSign.getTriesLeft());

	PTEID_Pin& pinAddr = Pins.getPinByPinRef(PTEID_Pin::ADDR_PIN);
	m_pinsInfo[PTEID_Pin::ADDR_PIN] = new PinInfo(pinAddr.getId(), pinAddr.getLabel(), pinAddr.getTriesLeft());
}


QString MainWnd::getFinalLinkTarget(QString baseName)
{
	QFileInfo info(baseName);
	if (info.isSymLink() && info.exists())
	{
		baseName = info.symLinkTarget();
		baseName = getFinalLinkTarget(baseName);
	}

	return baseName;
}



//**************************************************
// fill the software info table
//**************************************************
void MainWnd::fillSoftwareInfo( void )
{
	/*
	QStringList libPaths = QProcess::systemEnvironment();
	QStringList searchPaths;
	QMap<QString,QString> softwareInfo;

#ifdef WIN32

	//--------------------------------
	// search paths are:
	// 1. path of executable
	// 2. current directory
	// 3. windows system directory
	// 4. windows directory
	// 5. PATH
	//--------------------------------

	//--------------------------------
	// 1. exe path
	//--------------------------------
	searchPaths.push_back(QCoreApplication::applicationDirPath());

	//--------------------------------
	// 2. current path
	//--------------------------------
	searchPaths.push_back(QDir::currentPath());

	//--------------------------------
	// 3. system path
	//--------------------------------
	char sysDir[MAX_PATH];
	QString systemPath = GetSystemDirectory(sysDir,MAX_PATH);
	searchPaths.push_back(sysDir);

	//--------------------------------
	// 4. system path
	//--------------------------------
	char winDir[MAX_PATH];
	GetWindowsDirectory(winDir,MAX_PATH);
	searchPaths.push_back(winDir);

	//--------------------------------
	// 5. PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^PATH=.+");

	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);
		QStringList subPaths;
		subPaths = strPATH.split(";");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QFileInfo fileInfo(QDir::toNativeSeparators(path+"/"+fileList[idx]));
				if (fileInfo.isFile())
				{
					CFileVersionInfo VerInfo;
					if(VerInfo.Open(fileInfo.filePath().toLatin1()))
					{
						char version[256];
						//VerInfo.QueryStringValue(VI_STR_PRODUCTVERSION, version);
						VerInfo.QueryStringValue(VI_STR_FILEVERSION, version);
						softwareInfo[QString(fileList[idx])] = QString(version);
					}
				}
			}
		}
	}
#elif defined __APPLE__
	//--------------------------------
	// search paths are:
	// 1. DYLD_LIBRARY_PATH
	// 2. /usr/local/lib    !!! assumes we're installing in /usr/local/lib !!!
	//--------------------------------
	//--------------------------------
	// 1. DYLD_LIBRARY_PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^DYLD_LIBRARY_PATH=.+");
	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);

		QStringList subPaths;
		subPaths = strPATH.split(":");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}
	//--------------------------------
	// 2. /usr/local/lib
	//--------------------------------
	QString exePath = QCoreApplication::applicationDirPath();
	searchPaths.push_back("/usr/local/lib");

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QString thisFile(fileList[idx]);
				//thisFile += ".*.*.*.dylib";
				//--------------------------------
				// we take the base filename and will follow the symbolic links until the last
				//--------------------------------
				thisFile += ".dylib";
				QDir fileInfo(path,thisFile);
				//QStringList theFiles = fileInfo.entryList();
				QFileInfoList theFiles = fileInfo.entryInfoList();

				if (theFiles.size()>0)
				{
					QString version;
					QString baseName(theFiles[0].absoluteFilePath());
					QString caption;
					caption ="debug_before_getFinalLinkTarget";
					baseName = getFinalLinkTarget(baseName);
					caption = "debug_after_getFinalLinkTarget";

					//--------------------------------
					// abcdefg.x.y.z.dylib
					// +-----+             == basename
					// therefore the x.y.z length can be calculated as:
					//    startpoint: baseName().size()+1  ( +1 for the dot after the basename)
					//    length: fileName().size()
					//            - (baseName().size()+1)   ( +1 for the dot after the basename)
					//            - ".dylib".size()
					//--------------------------------
					QFileInfo info(baseName);

					version = info.fileName();
					version = version.mid(info.baseName().size()+1,version.size()-(info.baseName().size()+1)-QString(".dylib").size());

					softwareInfo[thisFile]=QString(version);
				}
			}
		}
	}
#else
// Linux
	//--------------------------------
	// search paths are:
	// 1. LD_LIBRARY_PATH
	// 2. paths in file /etc/ld.so.conf
	//    The problem with this is that this file can include other .conf files
	//    This becomes too complicated for its purpose here, so we omit this.
	// 3. ../lib
	//--------------------------------
	//--------------------------------
	// 1. LD_LIBRARY_PATH variable
	//--------------------------------
	int idx = -1;
	QRegExp envPATH("^LD_LIBRARY_PATH=.+");
	if ( (idx=libPaths.indexOf(envPATH))>0)
	{
		QString strPATH = libPaths.at(idx);
		strPATH = strPATH.mid(strPATH.indexOf("=")+1);

		//printf("Splitting: %s\n",strPATH.toLatin1().data());

		QStringList subPaths;
		subPaths = strPATH.split(":");
		foreach(QString p, subPaths)
		{
			searchPaths.push_back(p);
		}
	}
	//--------------------------------
	// 3. ../lib (relative to the exe path
	//--------------------------------
	QString exePath = QCoreApplication::applicationDirPath();
	searchPaths.push_back(exePath+"/../lib");

	foreach (QString path, searchPaths)
	{
		for ( size_t idx=0; idx<sizeof(fileList)/sizeof(char*); idx++)
		{
			if (softwareInfo.end()==softwareInfo.find(fileList[idx]))
			{
				QFileInfo fileInfo(path+"/"+fileList[idx]);
				if (fileInfo.isFile())
				{
					QString thisFile;
					thisFile = fileList[idx];
					thisFile += ".?.?.?" ;
					QDir thisDir(path,thisFile);
					QStringList allFiles=thisDir.entryList();
					foreach(QString p,allFiles)
					{
						QString version = p.mid(p.indexOf(".so.")+4);
						softwareInfo[QString(fileList[idx])] = QString(version);
					}
				}
			}
		}
	}

#endif

	m_ui.tblInfo->setRowCount( softwareInfo.size() );
	m_ui.tblInfo->setColumnCount( 2 );

	QTableWidgetItem*	newItem = NULL;
	int					RowNr = 0;
	int					ColNr = 0;

	Qt::ItemFlags flags;
	flags &= !Qt::ItemIsEditable;

	for ( QMap<QString,QString>::iterator itData=softwareInfo.begin()
		; itData != softwareInfo.end()
		; itData++, ColNr=0, RowNr++
		)
	{
		newItem = new QTableWidgetItem( itData.key() );
		newItem->setFlags(flags);
		m_ui.tblInfo->setItem( RowNr, ColNr++, newItem );
		newItem = new QTableWidgetItem( itData.value() );
		newItem->setFlags(flags);
		m_ui.tblInfo->setItem( RowNr, ColNr, newItem );
	}

	 */
}

//**************************************************
// clear button clicked
// - back to the main screen
// - clear the data of the loaded card
// - make sure virtual reader is cleared
// - current reader name reset
//**************************************************
void MainWnd::on_actionClear_triggered()
{
	QString msg = tr("Clear");
	msg.remove(QChar('&'));
	m_ui.statusBar->showMessage(msg,m_STATUS_MSG_TIME);
	clearGuiContent();

}
//*****************************************************
// Clear the content of the GUI
//*****************************************************
void MainWnd::clearGuiContent( void )
{
	Show_Splash();

	m_CI_Data.Reset();
	releaseVirtualReader();
	m_CurrReaderName = "";

	clearTabCertificates();
	clearTabPins();

	enableFileMenu();
	enablePrintMenu();
}

//*****************************************************
// refresh the tab with the ID data (front of card)
//*****************************************************
void MainWnd::refreshTabIdentity( void )
{

	m_ui.lblIdentity_ImgPerson->setPixmap(m_imgPicture);

	m_ui.lblIdentity_ImgPerson->show();

	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtIdentity_Name->setText		 ( QString::fromUtf8(PersonFields[NAME].toStdString().c_str()) );
	m_ui.txtIdentity_Name->setAccessibleName ( QString::fromUtf8(PersonFields[NAME].toStdString().c_str()) );
	m_ui.txtIdentity_GivenNames->setText ( QString::fromUtf8(PersonFields[GIVENNAME].toStdString().c_str()) );
	m_ui.txtIdentity_GivenNames->setAccessibleName ( QString::fromUtf8(PersonFields[GIVENNAME].toStdString().c_str()) );
	m_ui.txtIdentity_Nationality->setText( QString::fromUtf8(PersonFields[NATIONALITY].toStdString().c_str()) );
	m_ui.txtIdentity_Nationality->setAccessibleName( QString::fromUtf8(PersonFields[NATIONALITY].toStdString().c_str()) );
	m_ui.txtIdentity_BirthDate->setText ( QString::fromUtf8(PersonFields[BIRTHDATE].toStdString().c_str()) );
	m_ui.txtIdentity_BirthDate->setAccessibleName ( QString::fromUtf8(PersonFields[BIRTHDATE].toStdString().c_str()) );
	m_ui.txtIdentity_Sex->setText        ( QString::fromUtf8(PersonFields[SEX].toStdString().c_str()) );
	m_ui.txtIdentity_Sex->setAccessibleName ( QString::fromUtf8(PersonFields[SEX].toStdString().c_str()) );

	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER]);
	m_ui.txtIdentity_Height->setText	 ( QString::fromUtf8(PersonFields[HEIGHT].toStdString().c_str()) );
	m_ui.txtIdentity_Height->setAccessibleName	 ( QString::fromUtf8(PersonFields[HEIGHT].toStdString().c_str()) );
	m_ui.txtIdentity_Country->setText	 ( QString::fromUtf8(PersonFields[COUNTRY].toStdString().c_str()) );
	m_ui.txtIdentity_Country->setAccessibleName	 ( QString::fromUtf8(PersonFields[COUNTRY].toStdString().c_str()) );
	m_ui.txtIdentity_DocumentNumber->setText	 ( QString::fromUtf8(PersonFields[DOCUMENTNUMBER].toStdString().c_str()) );
	m_ui.txtIdentity_DocumentNumber->setAccessibleName	 ( QString::fromUtf8(PersonFields[DOCUMENTNUMBER].toStdString().c_str()) );
	m_ui.txtIdentity_ValidUntil->setText( QString::fromUtf8(CardFields[CARD_VALIDUNTIL].toStdString().c_str()) );
	m_ui.txtIdentity_ValidUntil->setAccessibleName( QString::fromUtf8(CardFields[CARD_VALIDUNTIL].toStdString().c_str()) );
	m_ui.txtIdentity_Parents_Father->setText(  QString::fromUtf8(PersonFields[FATHER].toStdString().c_str()) );
	m_ui.txtIdentity_Parents_Father->setAccessibleName(QString::fromUtf8(PersonFields[FATHER].toStdString().c_str()));
	m_ui.txtIdentity_Parents_Mother->setText( QString::fromUtf8(PersonFields[MOTHER].toStdString().c_str()) );
	m_ui.txtIdentity_Parents_Mother->setAccessibleName( QString::fromUtf8(PersonFields[MOTHER].toStdString().c_str()) );
	m_ui.txtIdentity_AccidentalIndications->setText( QString::fromUtf8(PersonFields[ACCIDENTALINDICATIONS].toStdString().c_str()) );
	m_ui.txtIdentity_AccidentalIndications->setAccessibleName( QString::fromUtf8(PersonFields[ACCIDENTALINDICATIONS].toStdString().c_str()) );
}

//*****************************************************
// refresh the tab with the ID extra info (card back side)
//*****************************************************
void MainWnd::refreshTabIdentityExtra()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	tFieldMap& PersonFields = m_CI_Data.m_PersonInfo.getFields();

	m_ui.txtIdentityExtra_TaxNo->setText	( QString::fromUtf8(PersonFields[TAXNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_TaxNo->setAccessibleName	( QString::fromUtf8(PersonFields[TAXNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_SocialSecurityNo->setText	( QString::fromUtf8(PersonFields[SOCIALSECURITYNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_SocialSecurityNo->setAccessibleName	( QString::fromUtf8(PersonFields[SOCIALSECURITYNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_HealthNo->setText	( QString::fromUtf8(PersonFields[HEALTHNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_HealthNo->setAccessibleName	( QString::fromUtf8(PersonFields[HEALTHNO].toStdString().c_str()) );
	m_ui.txtIdentityExtra_CardVersion->setText	( QString::fromUtf8(PersonFields[CARDVERSION].toStdString().c_str()) );
	m_ui.txtIdentityExtra_CardVersion->setAccessibleName	( QString::fromUtf8(PersonFields[CARDVERSION].toStdString().c_str()) );
	m_ui.txtIdentityExtra_DocumentType->setText	( QString::fromUtf8(PersonFields[DOCUMENTTYPE].toStdString().c_str()) );
	m_ui.txtIdentityExtra_DocumentType->setAccessibleName	( QString::fromUtf8(PersonFields[DOCUMENTTYPE].toStdString().c_str()) );
	m_ui.txtIdentityExtra_IssuingEntity->setText	( QString::fromUtf8(PersonFields[ISSUINGENTITY].toStdString().c_str()) );
	m_ui.txtIdentityExtra_IssuingEntity->setAccessibleName	( QString::fromUtf8(PersonFields[ISSUINGENTITY].toStdString().c_str()) );

	m_ui.txtIdentityExtra_LocalofRequest->setText	( QString::fromUtf8(PersonFields[LOCALOFREQUEST].toStdString().c_str()) );
	m_ui.txtIdentityExtra_LocalofRequest->setAccessibleName	( QString::fromUtf8(PersonFields[LOCALOFREQUEST].toStdString().c_str()) );

	m_ui.txtIdentityExtra_Validate->setText ( QString::fromUtf8(PersonFields[VALIDATION].toStdString().c_str()) );
	m_ui.txtIdentityExtra_Validate->setAccessibleName ( QString::fromUtf8(PersonFields[VALIDATION].toStdString().c_str()) );

	QString cardNumber = m_CI_Data.m_CardInfo.formatCardNumber(CardFields[CARD_NUMBER]);

	QString nationalNumber = m_CI_Data.m_PersonInfo.formatNationalNumber( PersonFields[NATIONALNUMBER]);

	m_ui.txtIdentityExtra_ValidFrom->setText( QString::fromUtf8(CardFields[CARD_VALIDFROM].toStdString().c_str()));
	m_ui.txtIdentityExtra_ValidFrom->setAccessibleName( QString::fromUtf8(CardFields[CARD_VALIDFROM].toStdString().c_str()));

	tFieldMap& PersonExtraFields = m_CI_Data.m_PersonInfo.m_PersonExtraInfo.getFields();

	QMap<QString,QString> SpecialStatus;
	SpecialStatus["0"] = tr("none");
	SpecialStatus["1"] = tr("white cane");
	SpecialStatus["2"] = tr("extended minority");
	SpecialStatus["3"] = tr("white cane/extended minority");
	SpecialStatus["4"] = tr("yellow cane");
	SpecialStatus["5"] = tr("yellow cane/extended minority");

	tFieldMap& MiscFields = m_CI_Data.m_MiscInfo.getFields();

}

void MainWnd::clearAddressData(void){
	m_ui.txtAddress_Municipality->setText(QString());
	m_ui.txtAddress_Municipality->setAccessibleName(QString());
	m_ui.txtAddress_District->setText(QString());
	m_ui.txtAddress_District->setAccessibleName(QString());
	m_ui.txtAddress_CivilParish->setText(QString());
	m_ui.txtAddress_CivilParish->setAccessibleName(QString());
	m_ui.txtAddress_StreetType1->setText(QString());
	m_ui.txtAddress_StreetType1->setAccessibleName(QString());
	m_ui.txtAddress_StreetType2->setText(QString());
	m_ui.txtAddress_StreetType2->setAccessibleName(QString());
	m_ui.txtAddress_StreetName->setText(QString());
	m_ui.txtAddress_StreetName->setAccessibleName(QString());
	m_ui.txtAddress_BuildingType1->setText(QString());
	m_ui.txtAddress_BuildingType1->setAccessibleName(QString());
	m_ui.txtAddress_BuildingType2->setText(QString());
	m_ui.txtAddress_BuildingType2->setAccessibleName(QString());
	m_ui.txtAddress_DoorNo->setText(QString());
	m_ui.txtAddress_DoorNo->setAccessibleName(QString());
	m_ui.txtAddress_Floor->setText(QString());
	m_ui.txtAddress_Floor->setAccessibleName(QString());
	m_ui.txtAddress_Side->setText(QString());
	m_ui.txtAddress_Side->setAccessibleName(QString());
	m_ui.txtAddress_Locality->setText(QString());
	m_ui.txtAddress_Locality->setAccessibleName(QString());
	m_ui.txtAddress_Zip4->setText(QString());
	m_ui.txtAddress_Zip4->setAccessibleName(QString());
	m_ui.txtAddress_Zip3->setText(QString());
	m_ui.txtAddress_Zip3->setAccessibleName(QString());
	m_ui.txtAddress_Place->setText(QString());
	m_ui.txtAddress_Place->setAccessibleName(QString());
	m_ui.txtAddress_PostalLocality->setText(QString());
	m_ui.txtAddress_PostalLocality->setAccessibleName(QString());

}

//*****************************************************
// refresh the tab with the PTeid Address
//*****************************************************
void MainWnd::refreshTabAddress( void )
{
    if (!m_CI_Data.isDataLoaded())
        return;


	if (pinactivate == 1)
	{
		if (!addressPINRequest_triggered())
			return;
	}

	loadCardDataAddress();

	tFieldMap& AddressFields = m_CI_Data.m_AddressInfo.getFields();

	m_ui.txtAddress_Municipality->setText		 ( QString::fromUtf8(AddressFields[ADDRESS_MUNICIPALITY].toStdString().c_str()) );
	m_ui.txtAddress_Municipality->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_MUNICIPALITY].toStdString().c_str()) );
	m_ui.txtAddress_District->setText			( QString::fromUtf8(AddressFields[ADDRESS_DISTRICT].toStdString().c_str()) );
	m_ui.txtAddress_District->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_DISTRICT].toStdString().c_str()) );
	m_ui.txtAddress_CivilParish->setText			( QString::fromUtf8(AddressFields[ADDRESS_CIVILPARISH].toStdString().c_str()) );
	m_ui.txtAddress_CivilParish->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_CIVILPARISH].toStdString().c_str()) );
	m_ui.txtAddress_StreetType1->setText			( QString::fromUtf8(AddressFields[ADDRESS_ABBRSTREETTYPE].toStdString().c_str()) );
	m_ui.txtAddress_StreetType1->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_ABBRSTREETTYPE].toStdString().c_str()) );
	m_ui.txtAddress_StreetType2->setText			( QString::fromUtf8(AddressFields[ADDRESS_STREETTYPE].toStdString().c_str()) );
	m_ui.txtAddress_StreetType2->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_STREETTYPE].toStdString().c_str()) );
	m_ui.txtAddress_StreetName->setText			( QString::fromUtf8(AddressFields[ADDRESS_STREETNAME].toStdString().c_str()) );
	m_ui.txtAddress_StreetName->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_STREETNAME].toStdString().c_str()) );
	m_ui.txtAddress_BuildingType1->setText			( QString::fromUtf8(AddressFields[ADDRESS_ABBRBUILDINGTYPE].toStdString().c_str()) );
	m_ui.txtAddress_BuildingType1->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_ABBRBUILDINGTYPE].toStdString().c_str()) );
	m_ui.txtAddress_BuildingType2->setText			( QString::fromUtf8(AddressFields[ADDRESS_BUILDINGTYPE].toStdString().c_str()) );
	m_ui.txtAddress_BuildingType2->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_BUILDINGTYPE].toStdString().c_str()) );
	m_ui.txtAddress_DoorNo->setText			( QString::fromUtf8(AddressFields[ADDRESS_DOORNO].toStdString().c_str()) );
	m_ui.txtAddress_DoorNo->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_DOORNO].toStdString().c_str()) );
	m_ui.txtAddress_Floor->setText			( QString::fromUtf8(AddressFields[ADDRESS_FLOOR].toStdString().c_str()) );
	m_ui.txtAddress_Floor->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_FLOOR].toStdString().c_str()) );
	m_ui.txtAddress_Side->setText			( QString::fromUtf8(AddressFields[ADDRESS_SIDE].toStdString().c_str()) );
	m_ui.txtAddress_Side->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_SIDE].toStdString().c_str()) );
	m_ui.txtAddress_Locality->setText			( QString::fromUtf8(AddressFields[ADDRESS_LOCALITY].toStdString().c_str()) );
	m_ui.txtAddress_Locality->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_LOCALITY].toStdString().c_str()) );
	m_ui.txtAddress_Zip4->setText			( QString::fromUtf8(AddressFields[ADDRESS_ZIP4].toStdString().c_str()) );
	m_ui.txtAddress_Zip4->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_ZIP4].toStdString().c_str()) );
	m_ui.txtAddress_Zip3->setText			( QString::fromUtf8(AddressFields[ADDRESS_ZIP3].toStdString().c_str()) );
	m_ui.txtAddress_Zip3->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_ZIP3].toStdString().c_str()) );
	m_ui.txtAddress_Place->setText			( QString::fromUtf8(AddressFields[ADDRESS_PLACE].toStdString().c_str()) );
	m_ui.txtAddress_Place->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_PLACE].toStdString().c_str()) );
	m_ui.txtAddress_PostalLocality->setText			( QString::fromUtf8(AddressFields[ADDRESS_POSTALLOCALITY].toStdString().c_str()) );
	m_ui.txtAddress_PostalLocality->setAccessibleName ( QString::fromUtf8(AddressFields[ADDRESS_POSTALLOCALITY].toStdString().c_str()) );

	addressdatastatus = 0;
}

// Set Max of 1000Bytes for personalNotes Field
void MainWnd::updatetext()
{
	int strnr;
	int totalct;

	QString TxtPersoDataString = m_ui.txtPersoData->toPlainText().toUtf8();
	strnr = TxtPersoDataString.count();

	totalct = 1000-strnr;

	QString TotalBytes = QString::number(totalct);
	TotalBytes.append(" / 1000");
	m_ui.txtPersoDataCount->setText(TotalBytes);

	if (TxtPersoDataString.count()>1000)
		m_ui.txtPersoData->textCursor().deletePreviousChar();
}

void MainWnd::PersoDataSaveButtonClicked( void )
{
    try {
        QString TxtPersoDataString = m_ui.txtPersoData->toPlainText().toUtf8();
        m_ui.txtPersoData->setMaximumBlockCount(1000);

        PTEID_ReaderContext &ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
        PTEID_EIDCard	 &Card	= ReaderContext.getEIDCard();
        PTEID_Pins &Pins	= Card.getPins();
        PTEID_Pin &Pin	= Pins.getPinByNumber(1);

        if (pinNotes == 1)
            authPINRequest_triggered();

        if (pinNotes == 0)
        {
            const PTEID_ByteArray oData(reinterpret_cast<const unsigned char*> (TxtPersoDataString.toStdString().c_str()), TxtPersoDataString.toStdString().size());
            Card.writePersonalNotes(oData);
            QMessageBox::information( this, tr("Personal Notes"),  tr("Personal notes successfully written!"), QMessageBox::Ok );

        }
    } catch (PTEID_Exception& e) {
        QMessageBox::critical(this, tr("Personal Notes"), tr("Error writing personal notes!"), QMessageBox::Ok );
    }
}
//*****************************************************
// refresh the tab with the PTeid Personal Data
//*****************************************************
void MainWnd::refreshTabPersoData( void )
{
	persodatastatus = 0;

	loadCardDataPersoData();

	tFieldMap& PersoDataFields = m_CI_Data.m_PersoDataInfo.getFields();

	connect(m_ui.txtPersoData, SIGNAL(textChanged()), this, SLOT(updatetext()));

	m_ui.txtPersoData->clear();
	m_ui.txtPersoData->insertPlainText (QString::fromUtf8(PersoDataFields[PERSODATA_INFO].toStdString().c_str()));


	connect(m_ui.btnPersoDataSave, SIGNAL(clicked()), this, SLOT( PersoDataSaveButtonClicked()));
}
//*****************************************************
// get the text for the type of card 
//*****************************************************
QString MainWnd::GetCardTypeText(QString const& cardType)
{
	QString strCardType;
	int iDocType = cardType.toInt();
	switch (iDocType)
	{
	case 11:
		strCardType	= tr("A. Bewijs van inschrijving in het vreemdelingenregister - Tijdelijk verblijf");
		break;
	case 12:
		strCardType	= tr("B. Bewijs van inschrijving in het vreemdelingenregister");
		break;
	case 13:
		strCardType	= tr("C. Identiteitskaart voor vreemdeling");
		break;
	case 14:
		strCardType	= tr("D. EG - langdurig ingezetene");
		break;
	case 15:
		strCardType	= tr("E. Verklaring van inschrijving");
		break;
	case 16:
		strCardType	= tr("E+. Verklaring van inschrijving");
		break;
	case 17:
		strCardType	= tr("F. Verblijfskaart van een familielid van een burger van de Unie");
		break;
	case 18:
		strCardType	= tr("F+. Verblijfskaart van een familielid van een burger van de Unie");
		break;
	default:
		strCardType = tr("Unknown");
		break;
	}
	return strCardType;
}
QString MainWnd::getSpecialOrganizationText( QString const& code)
{
	QString trSpecialOrganization;
	if ( "1" == code)
	{
		trSpecialOrganization = tr("SHAPE");
	}
	else
	{
		trSpecialOrganization = tr("NATO");
	}
	return trSpecialOrganization;
}
QString MainWnd::getDuplicataText( void )
{
	return tr("Duplicate: ");
}
QString MainWnd::getFamilyMemberText( void )
{
	return tr("Family member");
}


//*****************************************************
// clear the tab with the certificates
//*****************************************************
void MainWnd::clearTabCertificates( void )
{
	m_ui.treeCert->clear();

	m_ui.txtCert_Owner->setText( "" );
	m_ui.txtCert_Owner->setAccessibleName( "" );
	m_ui.txtCert_Issuer->setText( "" );
	m_ui.txtCert_Issuer->setAccessibleName( "" );
	m_ui.txtCert_ValidFrom->setText( "" );
	m_ui.txtCert_ValidFrom->setAccessibleName( "" );
	m_ui.txtCert_ValidUntil->setText( "" );
	m_ui.txtCert_ValidUntil->setAccessibleName( "" );
	m_ui.txtCert_KeyLenght->setText( "" );
	m_ui.txtCert_KeyLenght->setAccessibleName( "" );
}

//*****************************************************
// clear the tab with the PIN info
//*****************************************************
void MainWnd::clearTabAddress( void )
{ 
	m_ui.txtAddress_Municipality->setText            ( "" );
	m_ui.txtAddress_Municipality->setAccessibleName ( "" );
	m_ui.txtAddress_District->setText                       ( "" );
	m_ui.txtAddress_District->setAccessibleName ( "" );
	m_ui.txtAddress_CivilParish->setText                    ( "" );
	m_ui.txtAddress_CivilParish->setAccessibleName ( "" );
	m_ui.txtAddress_StreetType1->setText                    ( "" );
	m_ui.txtAddress_StreetType1->setAccessibleName ( "" );
	m_ui.txtAddress_StreetType2->setText                    ( "" );
	m_ui.txtAddress_StreetType2->setAccessibleName ( "" );
	m_ui.txtAddress_StreetName->setText                     ( "" );
	m_ui.txtAddress_StreetName->setAccessibleName ( "" );
	m_ui.txtAddress_BuildingType1->setText                  ( "" );
	m_ui.txtAddress_BuildingType1->setAccessibleName ( "" );
	m_ui.txtAddress_BuildingType2->setText                  ( "" );
	m_ui.txtAddress_BuildingType2->setAccessibleName ( "" );
	m_ui.txtAddress_DoorNo->setText                 ( "" );
	m_ui.txtAddress_DoorNo->setAccessibleName ( "" );
	m_ui.txtAddress_Floor->setText                  ( "" );
	m_ui.txtAddress_Floor->setAccessibleName ( "" );
	m_ui.txtAddress_Side->setText                   ( "" );
	m_ui.txtAddress_Side->setAccessibleName ( "" );
	m_ui.txtAddress_Locality->setText                       ( "" );
	m_ui.txtAddress_Locality->setAccessibleName ( "" );
	m_ui.txtAddress_Zip4->setText                   ( "" );
	m_ui.txtAddress_Zip4->setAccessibleName ( "" );
	m_ui.txtAddress_Zip3->setText                   ( "" );
	m_ui.txtAddress_Zip3->setAccessibleName ( "" );
	m_ui.txtAddress_Place->setText                  ( "" );
	m_ui.txtAddress_Place->setAccessibleName ( "" );
	m_ui.txtAddress_PostalLocality->setText                 ( "" );
	m_ui.txtAddress_PostalLocality->setAccessibleName ( "" );
}


//*****************************************************
// clear the tab with the PIN info
//*****************************************************
void MainWnd::clearTabPins( void )
{
	m_ui.treePIN->clear();

	m_ui.txtPIN_Name->setText( "" );
	m_ui.txtPIN_Name->setAccessibleName( "" );
	m_ui.txtPIN_ID->setText( "" );
	m_ui.txtPIN_ID->setAccessibleName( "" );
	m_ui.txtPIN_Status->setText( "" );
	m_ui.txtPIN_Status->setAccessibleName( "" );
}


//*****************************************************
// refresh the tab with the certificates
//*****************************************************
void MainWnd::refreshTabCertificates( void )
{
	certdatastatus = 0;
	loadCardDataCertificates();

	QList<QTreeWidgetItem *> selectedItems = m_ui.treeCert->selectedItems();
	if(selectedItems.size()==0)
	{
		//If no item is selected, we select the signature certificate
		selectedItems = m_ui.treeCert->findItems ( QString("Signature"), Qt::MatchContains|Qt::MatchRecursive );
		if (selectedItems.size()>0)
		{
			selectedItems[0]->setSelected(true);
		}
	}

	if (selectedItems.size()>0)
	{
		on_treeCert_itemClicked((QTreeCertItem *)selectedItems[0], 0);
	}

}

//*****************************************************
// refresh the tab with the PIN info
//*****************************************************
void MainWnd::refreshTabCardPin( void )
{
	if ( 0 < m_CurrReaderName.length() )
	{
		PTEID_ReaderContext& ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
		if (ReaderContext.isCardPresent())
		{
			switch(ReaderContext.getCardType())
			{
			case PTEID_CARDTYPE_IAS07:
			case PTEID_CARDTYPE_IAS101:
                {
                	QTreeWidgetItem *item = m_ui.treePIN->currentItem();

                	unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
                	unsigned long triesLeft = m_pinsInfo[pinRef]->triesLeft;

                	QString status;
                	if (1 == m_pinsInfo.find(pinRef)->second->triesLeft)
                		status = tr("%1 try remaining").arg(m_pinsInfo.find(pinRef)->second->triesLeft);
                	else
                		status = tr("%1 tries remaining").arg(m_pinsInfo.find(pinRef)->second->triesLeft);
                	m_ui.txtPIN_Name->setText(m_pinsInfo.find(pinRef)->second->pin_name);
                	m_ui.txtPIN_Name->setAccessibleName(m_pinsInfo.find(pinRef)->second->pin_name);
                	m_ui.txtPIN_ID->setText(m_pinsInfo.find(pinRef)->second->pin_id);
                	m_ui.txtPIN_ID->setAccessibleName(m_pinsInfo.find(pinRef)->second->pin_id);
                	m_ui.txtPIN_Status->setText(status);
                	m_ui.txtPIN_Status->setAccessibleName(status);
                }
			default:
				break;
			}
		}
	}
}

//*****************************************************
// refresh the tab with the software info
//*****************************************************
void MainWnd::refreshTabInfo( void )
{
}

//**************************************************
// menu items to change the language
// Each menu item has its associated function. To make Qt to
// change the language, just load another translation file.
// This load will generate an event of type QEvent::LanguageChange.
// To capture the event, the function 'changeEvent(QEvent *event)' has to be 
// implemented and the event type has to be checked. If it is a change 
// of language, just call the function 'translateUi()' (that is generated 
// automatically by Qt) and that's it.
//**************************************************

//**************************************************
// Switch UI language to English
//**************************************************
void MainWnd::setLanguageEn( void )
{
	setLanguage(GenPur::LANG_EN);
}

//**************************************************
// Switch UI language to Portuguese
//**************************************************
void MainWnd::setLanguageNl( void )
{
	setLanguage(GenPur::LANG_NL);
}

//**************************************************
// set the language menu to the current language
//**************************************************
void MainWnd::setLanguage( void )
{
	setLanguage(m_Language);
}

//**************************************************
// change the language of the interface to the given language
// if it could not be loaded, don't change the language
//**************************************************
void MainWnd::setLanguage(GenPur::UI_LANGUAGE language)
{
	GenPur::UI_LANGUAGE lngLoaded = LoadTranslationFile(language);
	if ( lngLoaded == language)
	{
		m_Language = lngLoaded;					// keep what language we are in
	}
	m_Settings.setGuiLanguage(language);
}


//**************************************************
// set tray icon corresponding to card(s) in the reader(s)
// no card reader selected:
//   display card inserted when a card is in any reader
// a card reader selected:
//   display card inserted if the card is in the selected reader
//**************************************************
void MainWnd::setCorrespondingTrayIcon( PopupEvent* pPopupEvent )
{

	QIcon TrayIco;
	TrayIco = QIcon( ":/images/Images/Icons/reader_nocard.png" );
	if(!m_pTrayIcon->isVisible())
	{
		m_pTrayIcon->setIcon(TrayIco);
		m_pTrayIcon->show();
	}

	if ( NULL==pPopupEvent  )
	{
		if ( 0==ReaderSet.readerCount() )
		{
			TrayIco = QIcon( ":/images/Images/Icons/reader_error.png" );
			m_pTrayIcon->setIcon(TrayIco);
			m_pTrayIcon->show();
		}
		return;
	}
	else if ( PopupEvent::ET_CARD_CHANGED == pPopupEvent->getType() )
	{
		QString readerName;

		//----------------------------------------------------------
		// if no card reader selected, find the first card reader with a card
		// present. If this is the card reader that gave the event, check
		// if it is an unknown card and adjust the icon accordingly.
		//----------------------------------------------------------
		if ( (unsigned long)-1 == m_Settings.getSelectedReader() )
		{
			TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
			for (unsigned long nrReaders=0; nrReaders<ReaderSet.readerCount();nrReaders++)
			{
				PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(nrReaders);
				readerName = readerContext.getName();
				if (readerContext.isCardPresent() && readerName==pPopupEvent->getReaderName())
				{
					PTEID_CardType	    cardType	  = readerContext.getCardType();
					if ( PTEID_CARDTYPE_UNKNOWN==cardType )
					{
						TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
					}
					nrReaders = ReaderSet.readerCount();	// stop looping
				}
			}
		}
		else
		{
			unsigned long ReaderIdx		 = m_Settings.getSelectedReader();
			readerName = ReaderSet.getReaderName(ReaderIdx);

			if (pPopupEvent->getReaderName() == readerName)
			{
				PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(ReaderIdx);
				PTEID_CardType	    cardType	  = readerContext.getCardType();
				if ( PTEID_CARDTYPE_UNKNOWN==cardType )
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
				}
				else
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
				}
			}
			else
			{
				return;
			}
		}
	}
	else if ( PopupEvent::ET_CARD_REMOVED == pPopupEvent->getType() )
	{
		if ( (unsigned long)-1 == m_Settings.getSelectedReader() )
		{
			for (unsigned long nrReaders=0; nrReaders<ReaderSet.readerCount();nrReaders++)
			{
				PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(nrReaders);
				QString readerName = readerContext.getName();
				if (readerContext.isCardPresent())
				{
					TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
					PTEID_CardType	    cardType	  = readerContext.getCardType();
					if ( PTEID_CARDTYPE_UNKNOWN==cardType )
					{
						TrayIco = QIcon( ":/images/Images/Icons/reader_question.png" );
					}
					nrReaders = ReaderSet.readerCount();	// stop looping
				}
			}
		}
		else
		{
			unsigned long		ReaderIdx	  = m_Settings.getSelectedReader();
			PTEID_ReaderContext& readerContext = ReaderSet.getReaderByNum(ReaderIdx);
			if (readerContext.isCardPresent())
			{
				TrayIco = QIcon( ":/images/Images/Icons/reader_card.png" );
			}
		}
	}
	m_pTrayIcon->setIcon(TrayIco);
	m_pTrayIcon->show();
}

//**************************************************
// custom event from the callback function
// When a card is inserted, we must determine if we have to reload
// the data.
// - don't reload in case the app is set as trayApplet.
// - at startup, we receive an event if a card was inserted
// - Add/remove of a card reader will make the card reader reset. Therefore
//   we also clean the data we might have in memory.
//   The fact that our internal data has a card pointer == NULL, tells
//   us that we either start up, or have cardreaders added/removed.
//**************************************************
void MainWnd::customEvent( QEvent* pEvent )
{
#ifdef WIN32
	if( pEvent->type() == QEvent::User+1 )
	{
		QuitEvent* qEvent = (QuitEvent*)pEvent;
		//----------------------------------------------------------
		// so we have to close down the application
		//----------------------------------------------------------
		if ( qEvent->m_msg == WM_QUERYENDSESSION)
		{
			on_actionE_xit_triggered();
		}
	}
	else
#endif 
		if( pEvent->type() == QEvent::User )
		{
			try
			{

				if (!m_Pop)
				{
					pEvent->accept();
					return;
				}

				//----------------------------------------------------------
				// look what card reader sent the event and get the picture from
				// the card, and show the picture
				//----------------------------------------------------------
				PopupEvent*	pPopupEvent	= (PopupEvent*)pEvent;

				setCorrespondingTrayIcon(pPopupEvent);

				if (pPopupEvent->getType() ==  PopupEvent::ET_CARD_REMOVED && m_CI_Data.m_cardReader==pPopupEvent->getReaderName())
				{
					setEnabledPinButtons(false);
					setEnabledCertifButtons(false);
					enableFileSave(false);

					m_imgPicture = NULL;
					m_pinsInfo.clear();
					m_CI_Data.Reset();
					refreshTabIdentity();
					refreshTabIdentityExtra();
					refreshTabPersoData();
					refreshTabCardPin();
					refreshTabCertificates();
					clearAddressData();
					m_ui.btnSelectTab_Identity->setFocus();

				}
				//----------------------------------------------------------
				// card has been changed in a reader
				//----------------------------------------------------------
				if (pPopupEvent->getType() ==  PopupEvent::ET_CARD_CHANGED)
				{
					QString	cardReader = pPopupEvent->getReaderName();

					//Toggle the Address PIN flag to "false"
					pinactivate = 1;
					//Force refresh of Certificates, Address and PersonalNotes tabs
					persodatastatus = 1;
					addressdatastatus = 1;
					certdatastatus = 1;

					//----------------------------------------------------------
					// show a message in the status bar that a card has been inserted
					//----------------------------------------------------------
					QString statusMsg;
					statusMsg += tr("Card Reader: ");
					statusMsg += pPopupEvent->getReaderName();
					m_ui.statusBar->showMessage(statusMsg,m_STATUS_MSG_TIME);

					PTEID_ReaderContext& readerContext	= ReaderSet.getReaderByName(cardReader.toLatin1());

					if (m_Settings.getShowNotification())
					{
						QString title(tr("Card Inserted"));
						m_pTrayIcon->showMessage ( title, statusMsg, QSystemTrayIcon::Information) ;
					}

					//----------------------------------------------------------
					// if no card is present, then just enable the reload button
					//----------------------------------------------------------
					if (!readerContext.isCardPresent())
					{
						pEvent->accept();
						return;
					}

					//----------------------------------------------------------
					// This custom event is arrived since a card is changed in a reader
					// check in which reader it is and disable the PIN/CERT buttons
					// to avoid inconsistencies.
					// We check on the card data pointer to verify if this is the first time
					// we have an event for 'card-changed'. The first time the pCard is NULL
					// meaning that no card was read yet, and thus no inconsistencies can exist.
					//----------------------------------------------------------
					PTEID_CardType cardType = readerContext.getCardType();

					switch( cardType )
					{
					case PTEID_CARDTYPE_IAS07:
					case PTEID_CARDTYPE_IAS101:
					{
						try
						{
							PTEID_EIDCard& card = readerContext.getEIDCard();
							doPicturePopup( card );

						}
						catch (PTEID_ExNotAllowByUser& e)
						{
							long err = e.GetError();
							err = err;
						}
						//------------------------------------
						// register certificates when needed
						//------------------------------------
						if (m_Settings.getRegCert())
						{
							bool bImported = ImportCertificates(cardReader);
							if (!isHidden())
							{
								showCertImportMessage(bImported);
							}
						}
						if (isHidden())
						{
							break;
						}
						//------------------------------------------------
						// first load the data if necessary, because this will check the test cards as well
						// and will ask if test cards are allowed
						//------------------------------------------------
						if ( m_Settings.getAutoCardReading() )
						{
							m_CI_Data.Reset(); 
							loadCardData();
						}
					}
					break;
					case PTEID_CARDTYPE_UNKNOWN:
					{
						clearGuiContent();
						QString msg(tr("Unknown card type"));
						ShowPTEIDError( 0, msg );
					}
					default:
						break;
					}
				}
				pEvent->accept();
			}
			catch (PTEID_Exception& e)
			{
				long err = e.GetError();
				err = err;
			}
		}
}
//**************************************************
// show the picture on the Card
//**************************************************
void MainWnd::doPicturePopup( PTEID_Card& card )
{
    //------------------------------------------------
    // just return, we don't show the picture when the card is inserted
    // The setting to show the picture is used for the textballoon
    //------------------------------------------------

    if (!m_Settings.getShowPicture())
    {
        return;
    }

    //------------------------------------------------
    // To show the picture we must:
    // - keep the status if test cards were allowed or not
    // - allways allow a testcard
    // - load the picture for the popup
    // - reset the allowTestCard like the user has set it
    //------------------------------------------------
    PTEID_EIDCard& eidCard		 = static_cast<PTEID_EIDCard&>(card);
    bool		  bAllowTestCard = eidCard.getAllowTestCard();

    if (!bAllowTestCard)
    {
        eidCard.setAllowTestCard(true);
    }

    QImage myImage, myImagescaled;
    QPixmap				  pixMap;

    m_CI_Data.Reset();
    loadCardData();

    if (pixMap.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData, "PNG"))
    {
        pixMap = pixMap.scaledToWidth(150);
        m_Pop->setPixmap(pixMap);
        m_Pop->popUp();
    }
    eidCard.setAllowTestCard(bAllowTestCard);
}

//**************************************************
// This function overloads the default QMainWindow::changeEvent(..) function
// Now we can check for the event of type QEvent::LanguageChange
// that is generated by the QTranslator::load() function, called when
// the menu item to change the language is selected.
// If we have to change the UI language, then call the retranslateUi()
// function on the UI and the interface will be set accordingly.
// Important to know is that the function 'retranslateUi()' seems to reset
// properties of the LineEdit-fields. e.g.: the background transparency is reset
// and the font size. Therefore, we have to initialize all the tabs and set the 
// zoom factor again.
//**************************************************
void MainWnd::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) 
	{
		m_ui.retranslateUi(this);
		//initAllTabs();
		try
		{
			refreshTabCardPin();
			refreshTabIdentityExtra();
		}
		catch (PTEID_ExNoCardPresent& e)
		{
			e=e;
		}
		catch (PTEID_Exception& e)
		{
			e=e;
		}
		createTrayMenu();
		m_ui.lblIdentity_ImgPerson->setPixmap( m_imgPicture );

	} 
	else if (event->type() == QEvent::WindowStateChange )
	{
		QWindowStateChangeEvent* ev = (QWindowStateChangeEvent*)event;	
		if (ev->oldState()== Qt::WindowNoState)
		{

			m_pMinimizeAction->setEnabled(false);
			m_pRestoreAction->setEnabled(true);
			event->ignore();
			//QApplication::postEvent(this, new QCloseEvent());
		}
		else if (ev->oldState()==Qt::WindowMinimized)
		{
			m_pMinimizeAction->setEnabled(true);
			m_pRestoreAction->setEnabled(false);
			event->ignore();
		}
	}
	else
	{
		QWidget::changeEvent(event);
	}
}

//**************************************************
// Quit the application.
// Release the SDK before stopping.
//**************************************************
void MainWnd::quit_application(){

	try
	{
		hide();
		if (m_Settings.getRemoveCert())
		{
			for (unsigned long readerCount=0;readerCount<ReaderSet.readerCount();readerCount++)
			{
				QString readerName = ReaderSet.getReaderName(readerCount);
				RemoveCertificates( readerName );
			}
		}

		//-------------------------------------------------------------------
		// we must release all the certificate contexts before releasing the SDK.
		// After Release, no more calls should be done to the SDK and as such
		// noting should be done in the dtor
		//-------------------------------------------------------------------
		forgetAllCertificates();
		stopAllEventCallbacks();

		qApp->quit();
	}
	catch (PTEID_Exception &e)
	{
		QString msg(tr("General exception"));
		ShowPTEIDError( e.GetError(), msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( 0, msg );
	}
}




void MainWnd::on_actionE_xit_triggered(void)
{
	quit_application();
}
//**************************************************
// set the event callback functions
//**************************************************
void MainWnd::setEventCallbacks( void )
{
	//----------------------------------------
	// for all the readers, create a callback such we can know
	// afterwards, which reader called us
	//----------------------------------------
	try
	{
		size_t maxcount=ReaderSet.readerCount(true);
		for (size_t Ix=0; Ix<maxcount; Ix++)
		{
			void (*fCallback)(long lRet, unsigned long ulState, void* pCBData);

			const char*			 readerName		= ReaderSet.getReaderName(Ix);
			PTEID_ReaderContext&  readerContext  = ReaderSet.getReaderByNum(Ix);
			CallBackData*		 pCBData		= new CallBackData(readerName,this);

			fCallback = (void (*)(long,unsigned long,void *))&cardEventCallback;

			m_callBackHandles[readerName] = readerContext.SetEventCallback(fCallback,pCBData);
			m_callBackData[readerName]	  = pCBData;
		}
	}
	catch(PTEID_Exception& e)
	{
		QString msg(tr("setEventCallbacks"));
		ShowPTEIDError( e.GetError(), msg );
	}

}

//**************************************************
// display a messagebox with the error code
//**************************************************
void MainWnd::ShowPTEIDError( unsigned long ErrCode, QString const& msg )
{
	if (isHidden())
	{
		return;
	}
	QString strCaption(tr("Error"));
	QString strMessage;
	strMessage = strMessage.setNum(ErrCode,16);
	strMessage += ": ";
	strMessage += msg;
	QMessageBox::warning( this, strCaption,  strMessage, QMessageBox::Ok );
}

#ifdef WIN32
//**************************************************
// Use Minidriver if OS is Vista or later
//**************************************************
BOOL MainWnd::UseMinidriver( void )
{
	OSVERSIONINFO osvi;
	BOOL bIsWindowsVistaorLater;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	bIsWindowsVistaorLater = (osvi.dwMajorVersion >= 6);

	return bIsWindowsVistaorLater;
}

//**************************************************
// Checks of older registered certificates are not
// still bound to the CSP when the minidriver is used
//**************************************************
bool MainWnd::ProviderNameCorrect (PCCERT_CONTEXT pCertContext )
{
	unsigned long dwPropId= CERT_KEY_PROV_INFO_PROP_ID; 
	DWORD cbData = 0;
	CRYPT_KEY_PROV_INFO * pCryptKeyProvInfo;

	if (!UseMinidriver())
		return true;

	if(!(CertGetCertificateContextProperty(
			pCertContext,		// A pointer to the certificate where the property will be set.
			dwPropId,           // An identifier of the property to get.
			NULL,               // NULL on the first call to get the length.
			&cbData)))          // The number of bytes that must be allocated for the structure.
	{
		if (GetLastError() != CRYPT_E_NOT_FOUND) // The certificate does not have the specified property.
			return false;
	}
	if(!(pCryptKeyProvInfo = (CRYPT_KEY_PROV_INFO *)malloc(cbData)))
	{
		return true;
	}
	if(CertGetCertificateContextProperty(pCertContext, dwPropId, pCryptKeyProvInfo, &cbData))
	{
		if (!wcscmp(pCryptKeyProvInfo->pwszProvName, L"Portugal Identity Card CSP"))
			return false;
	}
	return true;
}
#endif

void CardDataLoader::Load()
{
	this->information.LoadData(card, readerName);
	if (this->mwnd)
		this->mwnd->loadPinData(this->card);
}

void CardDataLoader::LoadPersoData()
{
	this->information.LoadDataPersoData(card, readerName);
}

void CardDataLoader::LoadCertificateData()
{
	this->information.LoadDataCertificates(card, readerName);
}




