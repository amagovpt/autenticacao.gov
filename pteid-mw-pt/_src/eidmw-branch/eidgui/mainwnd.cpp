/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 * Copyright (C) 2012-2017 Caixa Mágica Software
 * Andre Guerreiro <andre.guerreiro@caixamagica.pt>
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
#include <QMenu>
#include <QFileDialog>
#include <QApplication>
#include <QPrintDialog>
#include <QDir>
#include <QtConcurrent>
#include <QEvent>
#include <QPixmap>
#include <QImage>
#include <QFileInfo>
#include <QProcess>
#include <stdlib.h>
#ifndef _WIN32

#include <unistd.h>
#endif
#include <time.h>

#ifdef WIN32
#include <shlobj.h>
#endif
#include "mainwnd.h"
#include "svn_revision.h"
#include "dlgAbout.h"
#include "dlgprint.h"
#include "ChangeAddressDialog.h"
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
#include <QDesktopServices>
#include <QUrl>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif

#include "pteidversions.h"


static bool	g_cleaningCallback=false;
static int	g_runningCallback=0;
//State variables for tab data
static unsigned int pinactivate = 1, certdatastatus = 1, addressdatastatus = 1, persodatastatus = 1 ;
//State of Pin Notes 0->Right PIN 1->Not yet inserted or wrong PIN
static unsigned int pinNotes = 1 ;

#ifdef WIN32
void ImportCertFromDisk(void *cert_path)
{
	PCCERT_CONTEXT pCertCtx = NULL;

	if (CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,
		cert_path,
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
		//Don't register any self-signed cert (as it shouldn't be in Intermediate Certificates Store)
		if (0 == memcmp(pCertCtx->pCertInfo->Issuer.pbData,
			           pCertCtx->pCertInfo->Subject.pbData,
			           pCertCtx->pCertInfo->Subject.cbData))
			return;

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
	else
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
		 "ImportCertFromDisk: CryptQueryObject returned error %08x", GetLastError());
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
, error_sod(false)
, m_Settings(settings)
, m_timerReaderList(NULL)
, m_pdf_signature_dialog(NULL)
, m_STATUS_MSG_TIME(5000)
, m_ShowBalloon(false)
, m_addressFieldNeedsReset(false)
, m_msgBox(NULL)
{
	//------------------------------------
	// install the translator object and load the .qm file for
	// the given language.
	//------------------------------------
	qApp->installTranslator(&m_translator);

#if !defined(WIN32)
    memset( &m_WndGeometry, -1, sizeof(m_WndGeometry) );
#endif

	GenPur::UI_LANGUAGE CurrLng   = m_Settings.getGuiLanguageCode();
	GenPur::UI_LANGUAGE LoadedLng = LoadTranslationFile(CurrLng);

	m_Language = LoadedLng;

	m_ui.setupUi(this);

	if (m_Settings.getGuiLanguageCode() == GenPur::LANG_NL)
		setLanguageNl();
	else
		setLanguageEn();

	setFixedSize(830, 630);
	m_ui.wdg_submenu_card->setVisible(false);

	Qt::WindowFlags flags = windowFlags();
	flags ^= Qt::WindowMaximizeButtonHint;
	setWindowFlags( flags );

	// Hide DSS and XAdES Signature buttons
	if (!m_Settings.areJavaAppsEnabled()) {
        m_ui.btnShortcut_VerifSign->hide();
		m_ui.btnShortcut_UnivSign->hide();
	}

	// Show test mode label
	if (!m_Settings.getTestMode() ) {
        m_ui.lbl_testmode->hide();
	}

	isLinkToCertClicked = false;

	/*** Setup progress Bar ***/
	m_progress = new QProgressDialog(this);
	m_progress->setWindowModality(Qt::WindowModal);
	m_progress->setWindowTitle(QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"));

	//Set windows flags
	m_progress->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint );
	m_progress->setValue(0);

	//Set fixed size window
	//m_progress->setFixedSize(m_progress->size());

	//Disable cancel button
    m_progress->setCancelButtonText(tr("Cancel"));
	m_progress->setCancelButton(NULL);

	//Configuring dialog as a "doing-stuff-type" progressBar
	/*m_progress->setMinimum(0);
	m_progress->setMaximum(0);*/
	m_progress->setRange(0, 0);

	connect(m_ui.btnPersoDataSave, SIGNAL(clicked()), this, SLOT(PersoDataSaveButtonClicked()));
	connect(&this->FutureWatcher, SIGNAL(finished()), m_progress, SLOT(cancel()));

	//Workaround for the QT5 behaviour of QProgressDialog: now it displays itself even before show() or exec() was called:
	// Accoring to the bugtracker we shouldn't be reusing the QProgressDialog for multiple long operations
	// https://bugreports.qt.io/browse/QTBUG-47042
	m_progress->reset();

	//------------------------------------
	//
	// set the window Icon (as it appears in the left corner of the window)
	//------------------------------------
    //const QIcon Ico = QIcon( ":/images/Images/Icons/ICO_CARD_EID_PLAIN_16x16.png" );
    const QIcon Ico = QIcon( ":/images/Images/Icons/pteid.ico" );
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

	setEnabledCertifButtons( false );
	setEnabledPinButtons( false );
	enableTabPersoData(false);
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

	applyProxyConfiguration();

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

	m_ui.lbl_menuTools_Signature->installEventFilter(this);
	m_ui.lbl_menuTools_PDFSignature->installEventFilter(this);
	m_ui.lbl_menuTools_VerifySignature->installEventFilter(this);
	m_ui.lbl_menuTools_SCAPSignature->installEventFilter(this);

	m_ui.lbl_menuSettings_Parameters->installEventFilter(this);

	m_ui.lbl_menuHelp_about->installEventFilter(this);
	m_ui.lbl_menuHelp_updates->installEventFilter(this);
	m_ui.lbl_menuHelp_documentation->installEventFilter(this);

	m_ui.wdg_submenu_card->installEventFilter(this);
	m_ui.wdg_submenu_tools->installEventFilter(this);
	m_ui.wdg_submenu_settings->installEventFilter(this);;
	m_ui.wdg_submenu_help->installEventFilter(this);

#ifdef __APPLE__
	QCoreApplication::processEvents();
#endif
}


bool MainWnd::eventFilter(QObject *object, QEvent *event)
{

	if (event->type() == QEvent::MouseButtonRelease)
	{

		if (object == m_ui.lbl_menuCard_Read )
		{
			/* Fix more than 1 loadCardData reading */
			//if ( !m_mutex_ReadCard.tryLock() ) return false;

			hide_submenus();
			//refreshTabPersoData();

			pinactivate = 1;
			pinNotes = 1;
			certdatastatus = 1;
			persodatastatus = 1;

            /* m_CI_Data.Reset();   This operation will be done on loadCardData() */

			/* Allow new card data reading */
			//m_mutex_ReadCard.unlock();
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

		if (object == m_ui.lbl_menuTools_SCAPSignature )
		{
			hide_submenus();
			actionSCAPSignature_triggered();
		}

		if (object == m_ui.lbl_menuSettings_Parameters )
		{
			hide_submenus();
			show_window_parameters();
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
		if (object == m_ui.lbl_menuHelp_documentation )
		{
			hide_submenus();
			QDesktopServices::openUrl(QUrl("http://svn.gov.pt/projects/ccidadao/repository/middleware-offline/trunk/docs/Manual_de_Utilizacao.pdf"));
		}
	}

	if (event->type() == QEvent::Leave)
	{
		if (object == m_ui.wdg_submenu_card || object == m_ui.wdg_submenu_tools || object == m_ui.wdg_submenu_settings || object == m_ui.wdg_submenu_help )
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
	m_ui.wdg_submenu_settings->setVisible(false);
	m_ui.wdg_submenu_help->setVisible(false);
}

//******************************************************^M
// Buttons to control Shortcuts
//******************************************************^M

void MainWnd::on_btnShortcut_UnivSign_clicked()
{
	actionSignature_eID_triggered();
}

void MainWnd::on_btnShortcut_PdfSign_clicked()
{
	actionPDFSignature_triggered();
}

void MainWnd::on_btnShortcut_VerifSign_clicked()
{
	actionVerifySignature_eID_triggered();
}

void MainWnd::on_btnShortcut_SCAPSign_clicked(){
	actionSCAPSignature_triggered();
}

/*
void MainWnd::hideJavaAppButtons() {
	m_ui.btnShortcut_SCAPSign->hide();
	m_ui.btnShortcut_VerifSign->hide();
}

void MainWnd::hideUniversalSigButton() {
	m_ui.btnShortcut_UnivSign->hide();
}
*/

/*
// Change Address functionality triggered by a button in the Address tab
*/
void MainWnd::address_change_callback(void *instance, int value)
{
	MainWnd * window = (MainWnd*) (instance);
	window->addressProgressChanged(value);
}

void MainWnd::setAddressProgress(int value)
{
	if (m_progress_addr)
		m_progress_addr->setValue(value);
}

void MainWnd::showChangeAddressDialog(long code)
{
	//It shouldn't be needed but, oh well...
	m_progress_addr->hide();

	QString error_msg;
	long sam_error_code = 0;
	QString caption  = tr("Address Confirmation");
	QString support_string = tr("Please try again. If this error persists, please have your"
							    " process number and error code ready, and contact the"
							    " Citizen Card support line at telephone number +351 211 950 500 or e-mail cartaodecidadao@irn.mj.pt.");

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "AddressChange op finished with error code 0x%08x", code);
	QMessageBox::Icon icon = QMessageBox::NoIcon;
	switch(code)
	{
		case 0:
			error_msg = tr("Address Confirmed successfully.");
			icon = QMessageBox::Information;
			break;
		//The error code for connection error is common between SAM and OTP
		case EIDMW_OTP_CONNECTION_ERROR:
			icon = QMessageBox::Critical;
			error_msg = tr("Connection Error") + "\n\n" +
				tr("Please make sure you are connected to the Internet");
			break;

		case 1121:
		case 1122:
			error_msg = tr("Error in the Address Change operation!") + "\n\n" + tr("Please make sure you typed the correct process number and confirmation code");
			icon = QMessageBox::Critical;
			sam_error_code = code;
			break;
		case EIDMW_SAM_UNCONFIRMED_CHANGE:
			error_msg = tr("Address change process is incomplete!") + "\n\n" + tr("The address is changed in the card but not confirmed by the State central services");
			icon = QMessageBox::Critical;
			break;
		case EIDMW_SSL_PROTOCOL_ERROR:
			error_msg = tr("Error in the Address Change operation!") + "\n\n" + tr("Please make sure you have a valid authentication certificate");
			icon = QMessageBox::Critical;
			break;
		default:
			//Make sure we only show the user error codes from the SAM service and not some weird pteid exception error code
			if (code > 1100 && code < 3500)
				sam_error_code = code;
			error_msg = tr("Error in the Address Change operation!");
			icon = QMessageBox::Critical;
			break;
	}

	if (sam_error_code != 0)
	{
		error_msg += "\n\n" + tr("Error code = ") + QString::number(sam_error_code);
	}

	if (code != 0)
		error_msg += "\n\n" + support_string;

	QMessageBox msgBoxp(icon, caption, error_msg, 0, this);
  	msgBoxp.exec();

	//Reload card information in case of successful address change
	if (code == 0)
	{

		pinactivate = 1;
		pinNotes = 1;
		addressdatastatus = 1;
		certdatastatus = 1;
		persodatastatus = 1;

		loadCardData();
	}

}

void MainWnd::doChangeAddress(const char *process, const char *secret_code)
{
	PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

    PTEID_EIDCard& Card = ReaderContext.getEIDCard();
    try
    {
        Card.ChangeAddress((char *)secret_code, (char*)process, MainWnd::address_change_callback, (void*)this);

	}
	catch(PTEID_Exception & exception)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Caught exception in EidCard.ChangeAddress()... closing progressBar");
		this->addressProgressChanged(100);
		this->addressChangeFinished(exception.GetError());

		free((char *)process);
		free((char *)secret_code);
		return;
	}

	free((char *)process);
	free((char *)secret_code);
	this->addressChangeFinished(0);

}

void MainWnd::on_btnAddress_Change_clicked()
{
    if (!m_CI_Data.isDataLoaded()) return;

	PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());

    PTEID_EIDCard& Card = ReaderContext.getEIDCard();
	QString caption  = tr("Address Confirmation");

	if (Card.getType() == PTEID_CARDTYPE_IAS101)
	{
		QString error_msg = tr("Unfortunately the Address Change operation is unsupported for this card");
		QMessageBox::Icon icon = QMessageBox::Critical;
		QMessageBox msgBoxp(icon, caption, error_msg, 0, this);
  		msgBoxp.exec();

		return;
	}

	ChangeAddressDialog* dlgChangeAddr = new ChangeAddressDialog(this);

	if (dlgChangeAddr->exec() == QDialog::Rejected) {
		delete dlgChangeAddr;
		return;
	}

    //Remove whitespace the start and the end that the user may have
    //typed by mistake
    QString process = dlgChangeAddr->getProcess().trimmed();
    QString secret_code = dlgChangeAddr->getSecretCode().trimmed();

    setup_addressChange_progress_bar();
   	connect(this, SIGNAL(addressProgressChanged(int)),
   	 this, SLOT(setAddressProgress(int)), Qt::UniqueConnection);

   	connect(this, SIGNAL(addressChangeFinished(long)),
   	 this, SLOT(showChangeAddressDialog(long)), Qt::UniqueConnection);

	char *processUtf8 = strdup(process.toUtf8().constData());
	char *secret_codeUtf8 = strdup(secret_code.toUtf8().constData());
   	QtConcurrent::run(this, &MainWnd::doChangeAddress,
					  processUtf8, secret_codeUtf8);
   	m_progress_addr->open();

	delete dlgChangeAddr;
}

void MainWnd::showJavaLaunchError(QProcess::ProcessError error)
{
	fprintf(stderr, "showJavaLaunchError: %d\n", (int)error);

	if (error == QProcess::FailedToStart)
	{
		QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning,
			QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"), tr("Error launching Java application! Make sure you have a working JRE installed."),
			QMessageBox::Ok, this);
		msgBox->setModal(true);
		msgBox->show();
		delete msgBox;
	}

}

#ifdef _WIN32
QString MainWnd::findJavaHomeOnWindows()
{
	QVariant current_version;
	const QString CURRENT_VERSION_KEY("CurrentVersion");
    QSettings javaSetting("HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment", QSettings::NativeFormat);
	current_version = javaSetting.value(CURRENT_VERSION_KEY);

	if (current_version == QVariant())
	{
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Couldn't find first Java registry key...");

		//Check WoW32Node values
		QSettings javaSetting2("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\JavaSoft\\Java Runtime Environment", QSettings::NativeFormat);
		current_version = javaSetting2.value(CURRENT_VERSION_KEY);
		if (current_version == QVariant())
		{
			PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Couldn't find second Java registry key, Java is not installed!");

			return QString();
		}
		else
			return javaSetting2.value(current_version.toString() + "/JavaHome").toString();
	}

	return javaSetting.value(current_version.toString() + "/JavaHome").toString();

}
#endif

void MainWnd::launchJavaProcess(const QString &application_jar, const QString jvm_args, const QString &classpath)
{
	QStringList arguments;

#ifdef __APPLE__

	//Using the current Oracle JRE this is always the path to the JRE dir
	QString program = QString("/Library/Internet Plug-Ins/JavaAppletPlugin.plugin/Contents/Home/bin/") +
					  "java";
#elif WIN32
	QString java_home = findJavaHomeOnWindows();
	if (java_home.isEmpty())
	{
		showJavaLaunchError(QProcess::FailedToStart);
		return;
	}

	QString program = java_home + "\\bin\\javaw";
#else
	//On Linux there's always a java binary in the PATH at least using proper packages...
	QString program = "java";
#endif
	QObject *parent = this;

	if (jvm_args.size() > 0)
		arguments << jvm_args;
	arguments << "-jar" << application_jar;

	if (classpath.length() > 0)
		arguments << "-cp" << classpath;

	QString language = m_Settings.getGuiLanguageString() == "nl" ? QString("pt") : m_Settings.getGuiLanguageString();
	//Add language code as args[0]
	arguments << language;

	QProcess *myProcess = new QProcess(parent);
	myProcess->setProcessChannelMode(QProcess::MergedChannels);
	QFileInfo fileinfo(application_jar);

	//Set working directory to where the jar file is located
	myProcess->setWorkingDirectory(fileinfo.dir().absolutePath());
	connect(myProcess, SIGNAL(error(QProcess::ProcessError)),
			 this, SLOT(showJavaLaunchError(QProcess::ProcessError)));

	myProcess->start(program, arguments);

	//delete myProcess;
}

void MainWnd::setup_addressChange_progress_bar()
{
	m_progress_addr = new QProgressDialog(this);
	m_progress_addr->setWindowModality(Qt::WindowModal);
	m_progress_addr->setWindowTitle(QString::fromUtf8("Cart\xc3\xa3o de Cidad\xc3\xa3o"));
	m_progress_addr->setLabelText(tr("Changing Address in card..."));
	m_progress_addr->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
	//m_progress_addr->setFixedSize(m_progress_addr->size());
	//Disable cancel button
	m_progress_addr->setCancelButton(NULL);
	m_progress_addr->setMinimum(0);
	m_progress_addr->setMaximum(100);
	QVBoxLayout *layout = new QVBoxLayout;
	foreach (QObject *obj, m_progress_addr->children()) {
	    QWidget *widget = qobject_cast<QWidget *>(obj);
    	if (widget)
        	layout->addWidget(widget);
	}
	m_progress_addr->setLayout(layout);
	m_progress_addr->reset();
	//Hack to force centered display of the popup dialog when running on "peculiar" window managers
	//m_progress_addr->move(geometry().center().x(), geometry().center().y());

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
	if (m_CI_Data.isDataLoaded())
	{
		bool addressLoaded = true;

		if (addressdatastatus == 1)
			addressLoaded = refreshTabAddress();

		if (addressLoaded)
			m_ui.stackedWidget->setCurrentIndex(3);
	}
}

void MainWnd::on_btnSelectTab_Certificates_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(4);
	m_ui.treeCert->setFocus( Qt::OtherFocusReason );
	if (certdatastatus == 1)
		refreshTabCertificates();
}

void MainWnd::on_btnSelectTab_PinCodes_clicked()
{
	m_ui.stackedWidget->setCurrentIndex(5);
}

void MainWnd::on_btnSelectTab_Notes_clicked()
{
	if (persodatastatus == 1)
		refreshTabPersoData();
	m_ui.stackedWidget->setCurrentIndex(6);
}

void MainWnd::on_btnIdentityExtra_linkToCert_clicked()
{
    isLinkToCertClicked = true;
    unsigned int saved_certdatastatus = certdatastatus;
    certdatastatus = 1;

    on_btnSelectTab_Certificates_clicked();

    isLinkToCertClicked = false;
    certdatastatus = saved_certdatastatus;
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
	//Trick to hide the Java tools shortcuts in this overlay menu
	int tools_height = m_Settings.areJavaAppsEnabled() ? 130: 85;
	//int tools_height = m_Settings.areJavaAppsEnabled() ? 130: 65;
	m_ui.wdg_submenu_tools->setVisible(true);

	//If defined language is portuguese, then the dialog needs to be larger
	if (m_Settings.getGuiLanguageCode() == GenPur::LANG_NL)
		m_ui.wdg_submenu_tools->setGeometry(127,4,185, tools_height);
	else
		m_ui.wdg_submenu_tools->setGeometry(127,4,160, tools_height);

}

void MainWnd::on_btn_menu_settings_clicked()
{
	m_ui.wdg_submenu_settings->setVisible(true);
	m_ui.wdg_submenu_settings->setGeometry(260,4,151,75);
}

void MainWnd::on_btn_menu_language_clicked()
{
	//If defined language is portuguese, language should change to EN
	if (m_Settings.getGuiLanguageCode() == GenPur::LANG_NL)
		setLanguageEn();
	else
		setLanguageNl();
}

void MainWnd::on_btn_menu_help_clicked()
{
	m_ui.wdg_submenu_help->setVisible(true);
	m_ui.wdg_submenu_help->setGeometry(410,4,165,110);
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
		}
		m_msgBox->show();
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

/******************************************************************
// Respond to other instance of the pteidgui process: this way we can raise the
// existing window
********************************************************************/
void MainWnd::messageRespond()
{
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "messageRespond - Received message from another pteidgui instance: restoring the Window");

	restoreWindow();
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

                    if ( 0 == ReaderSet.readerCount() ){
                        clearGuiContent();
                        showNoReaderMsg();
                      }
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
#ifdef WIN32
	if ( bEnabled ){
		m_ui.btnCert_Register->show();
	} else{
		m_ui.btnCert_Register->hide();
	}
	m_ui.btnCert_Details->setEnabled(bEnabled);
#endif /* WIN32 */
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
	wchar_t * cert_filepath = NULL;

	PTEID_ReaderContext&  ReaderContext  = ReaderSet.getReaderByName(readerName);
	if (!ReaderContext.isCardPresent())
	{
		return false;
	}

	//Register the higher-level CA Cert from disk files
	//TODO: maybe we should do the same for test cards
	QString certsdir("C:\\Program Files\\Portugal Identity Card\\eidstore\\certs\\");
	QDir dir(certsdir);

	QStringList filter("*.der");
	QStringList flist = dir.entryList(QStringList(filter),
                                   QDir::Files | QDir::NoSymLinks);

	foreach (QString str, flist) {
		QString filename = QString("%1%2").arg(certsdir).arg(str);
		unsigned int alloc_len = filename.size()+1;
		cert_filepath = new wchar_t[alloc_len];
		//This way we ensure the UTF-16 string is terminated
		memset(cert_filepath, 0, sizeof(wchar_t)*alloc_len);
		filename.toWCharArray(cert_filepath);
		ImportCertFromDisk(cert_filepath);

		delete[] cert_filepath;
	}

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
					PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Successfully stored UserCert with subject: %s", item->getCert()->getOwnerName());

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
							PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "Successfully stored CA Cert with subject: %s", issuer->getOwnerName());
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
			, pCertContext->pCertInfo->Subject.cbData))
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
			if (CertAddCertificateContextToStore(hMemoryStore, pCertContext, CERT_STORE_ADD_NEWER, NULL))
			{
				m_certContexts[readerName].push_back(pCertContext);
				bRet = true;
			}
			else
			{
				if (GetLastError() == ERROR_CANCELLED)
					PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "User denied registration of root certificate! Certificate chain will be incomplete...");
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
		// subject (contains name and document number) in the store
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
					// subject (contains name and document number),
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
		}
		while (NULL != pDesiredCert);


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
		tCert.hwndParent	= (HWND) this->winId();
		tCert.dwFlags		= CRYPTUI_DISABLE_EDITPROPERTIES;
		tCert.pCertContext	= CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certData.GetBytes(), certData.Size());
		BOOL bChange		= FALSE;
		CryptUIDlgViewCertificate(&tCert, &bChange);
		CertFreeCertificateContext (tCert.pCertContext);
	}
#endif
}

//*****************************************************
// current selected cert in the tree changed.
// This event seems to be fired both with clicked event and on select.
//*****************************************************
void MainWnd::on_treeCert_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current)
	{
		syncTreeItemWithSideinfo(dynamic_cast<QTreeCertItem *>(current));
	}

}

void MainWnd::syncTreeItemWithSideinfo(QTreeCertItem *item)
{

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

	// Put some text while loading the status
	m_ui.txtCert_RevStatus->setText(QString(tr("Checking...")));
	m_ui.txtCert_RevStatus->setAccessibleName(QString(tr("Checking...")));

	// ask for the cert status
	item->askCertStatus();

#ifdef WIN32
	if(!ReaderContext.isCardPresent())
	{
		m_ui.btnCert_Register->hide();
		m_ui.btnCert_Details->setEnabled(false);
	}
	else if ( 0 < item->childCount())
	{
		m_ui.btnCert_Register->hide();
		m_ui.btnCert_Details->setEnabled(true);
	}
	else
	{
		m_ui.btnCert_Register->show();
		m_ui.btnCert_Details->setEnabled(true);
	}
#endif /* #ifdef WIN32 */
}

//*****************************************************
// Received the reply with the status of the certificate
//*****************************************************
void MainWnd::showCertStatusSideinfo(PTEID_CertifStatus certStatus)
{

	QString treeItemStatus;
	getCertStatusText(certStatus, treeItemStatus);

	m_ui.txtCert_RevStatus->setText(treeItemStatus);
	m_ui.txtCert_RevStatus->setWordWrap(true);
	m_ui.txtCert_RevStatus->setAccessibleName(treeItemStatus);
}

void MainWnd::getCertStatusText(PTEID_CertifStatus certStatus, QString &strCertStatus)
{
	QString networkError(tr("Could not validate certificate. Please check your Internet connection"));
	switch(certStatus)
	{
	case PTEID_CERTIF_STATUS_REVOKED:
		strCertStatus = tr("Revoked");
		break;
	case PTEID_CERTIF_STATUS_SUSPENDED:
		strCertStatus = tr("Inactive or Suspended");
		break;
	case PTEID_CERTIF_STATUS_VALID:
		strCertStatus = tr("Valid");
		break;
	case PTEID_CERTIF_STATUS_EXPIRED:
		strCertStatus = tr("Expired");
		break;
	case PTEID_CERTIF_STATUS_CONNECT:
		strCertStatus = networkError;
		break;
	case PTEID_CERTIF_STATUS_UNKNOWN:
		strCertStatus = tr("Unknown");
		break;
	default:
		strCertStatus = networkError;
		break;
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

	if ( index < 0 ) return;

	PTEID_ReaderContext &ReaderContext = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
	if (!ReaderContext.isCardPresent()) {
		return;
	}


	/* BEGIN - the pin information have to be always synchronized - quick fix
	 * this chunk of code can be removed if in the future the pin information sinchronization requirement is dropped
	 */
	unsigned int _pinRef = item->data(0,Qt::UserRole).value<uint>();
	PTEID_EIDCard&	Card	= ReaderContext.getEIDCard();
	PTEID_Pins&		Pins	= Card.getPins();

	PTEID_Pin& pin = Pins.getPinByPinRef(_pinRef);
	delete m_pinsInfo[_pinRef]; // doesn't make sense, but this way the quick fix can be removed with no harm
	unsigned long pinId = pin.getId();
	m_pinsInfo[_pinRef] = new PinInfo( pinId, pin.getLabelById(pinId), pin.getTriesLeft() ); /*llemos*/



	unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
	PinInfo* pinfo = m_pinsInfo.find(pinRef)->second;

    setTabCardPin( item );/*llemos*/

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

void MainWnd::getReaderIndexes( unsigned long *p_Selected, unsigned long *p_Count, bool bRefresh ){
    unsigned long Selected = m_Settings.getSelectedReader();
    unsigned long Count   = ReaderSet.readerCount( bRefresh );

    if ( ( 0 == Count ) || ( ((unsigned long)-1) == Count ) ){
        Count = 0;
    }
    else {
        // Count > 0
        if ( ( ((unsigned long)-1) == Selected) || Selected == Count) {
            Selected = Count - 1;
        }
    }

    if ( p_Selected != NULL ) *p_Selected = Selected;
    if ( p_Count    != NULL ) *p_Count = Count;
}

void MainWnd::showInsertCardMsg()
{
	QString strCaption(tr("Reload eID"));
	strCaption = strCaption.remove(QChar('&'));
	QString strMessage(tr("No card found"));
	m_ui.statusBar->showMessage(strCaption+":"+strMessage,m_STATUS_MSG_TIME);

	QString caption  = tr("Warning");
	QString msg = tr("Please insert your card on the smart card reader");
	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	msgBoxp.exec();
}


void MainWnd::getCardForReading(PTEID_EIDCard * &new_card, bool clearData)
{
	try
	{
		unsigned long ReaderEndIdx = ReaderSet.readerCount();
		long ReaderIdx= 0;
		long selected_reader = -1;

        //getReaderIndexes( &ReaderStartIdx, &ReaderEndIdx, true );

        //We can safely clear previous data because we have successfully locked
        if (clearData)
        	m_CI_Data.Reset();

        if ( ReaderEndIdx == 0 ) {
            clearGuiContent();
            showNoReaderMsg();
        } else {
            bool bCardPresent = false;
            PTEID_CardType lastFoundCardType = PTEID_CARDTYPE_UNKNOWN;
            if (m_Settings.getSelectedReader() != -1)
            {
            	selected_reader = m_Settings.getSelectedReader();
            	std::cerr << "DEBUG: using selected reader index: " << selected_reader << std::endl;
            }

            for (ReaderIdx = selected_reader != -1 ? selected_reader: 0; ReaderIdx<ReaderEndIdx; ReaderIdx++)
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
                        
                        PTEID_EIDCard& Card = ReaderContext.getEIDCard();
                        new_card = &Card;

                        const char* readerName = ReaderSet.getReaderName(ReaderIdx);
                        m_CurrReaderName = readerName;

                        ReaderIdx=ReaderEndIdx;		// stop looping as soon as we found a card

                    }
                    break;
                    case PTEID_CARDTYPE_UNKNOWN:
                    default:
                        break;
                    }
                }
                else
                {
                    //clearGuiContent();

                }
                enablePrintMenu();

                //Only use selected reader so break out of the loop
                if (selected_reader != -1)
                	ReaderIdx=ReaderEndIdx;
            }
            if (!bCardPresent)
            {
               showInsertCardMsg();
            }
            else if (lastFoundCardType == PTEID_CARDTYPE_UNKNOWN)
            {
                QString msg(tr("Card read error or unknown card type"));
                ShowPTEIDError(msg );
                clearGuiContent();
            }
        }

		enableFileMenu();
	}
	catch (PTEID_ExParamRange e)
	{
		QString msg(tr("Index out of range"));
		ShowPTEIDError( msg );
	}
	catch (PTEID_ExNoCardPresent e)
	{
		QString msg(tr("No card present"));
		ShowPTEIDError( msg );
	}
	catch (PTEID_ExCardChanged e)
	{
		QString msg(tr("Card changed"));
		ShowPTEIDError( msg );

		/* Allow new card data reading */
		m_mutex_ReadCard.unlock();
		loadCardData();
	}
	catch (PTEID_ExBadTransaction& e)
	{
		QString msg(tr("Bad transaction"));
		ShowPTEIDError( msg );
	}
	catch (PTEID_ExCertNoRoot& e)
	{
		QString msg(tr("No root certificate found"));
		ShowPTEIDError( msg );
	}
	catch (PTEID_Exception e)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "Exception in getCardForReading(). Error code: 0x%08x", e.GetError());
		QString msg(tr("Error loading card data"));
		ShowPTEIDError( msg );
	}
}

//*****************************************************
// load the card data
//*****************************************************
void MainWnd::loadCardData( void )
{
	PTEID_EIDCard * new_card = NULL;
    //if( m_CI_Data.isDataLoaded() ) return;

	/* Fix more than 1 loadCardData reading */
	if ( !m_mutex_ReadCard.tryLock () )
		return;

	getCardForReading(new_card, true);

	if (new_card != NULL)
	{
		Show_Identity_Card(*new_card);
	}
	else
	{
		qDebug() << "loadCardData() Failed to getCardForReading() !";
	}

unlock:
	/* Unlock mutex to allow new card data reading */
	m_mutex_ReadCard.unlock();
}


//*****************************************************
// load the card data (Address Tab)
//*****************************************************
void MainWnd::loadCardDataAddress( void ) {
	PTEID_EIDCard * new_card = NULL;
    if (!m_CI_Data.isDataLoaded())
        return;

	getCardForReading(new_card);

	try
	{
		if (new_card != NULL)
		{
			LoadDataAddress(*new_card);
		}
		else
		{
			qDebug() << "loadCardDataAddress: Failed to getCardForReading() !" ;
		}
	}
	catch(PTEID_Exception &e) {
		long errorCode = e.GetError();

	    if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE &&	 	
	       errorCode <= EIDMW_SOD_ERR_VERIFY_SOD_SIGN)
	    {

	    	qDebug() << "SOD validation error " << errorCode;
			QString title = tr("SOD validation");
			QString msg = tr("SOD validation failed: card data consistency is compromised!");
			QMessageBox msgBoxcc(QMessageBox::Warning, title, msg, 0, this);
			msgBoxcc.setModal(true);
			msgBoxcc.exec();
	    }

		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui", "loadCardDataAddress: Error loading address data. Error code: 0x%08x", e.GetError()); 
	}

}

//*****************************************************
// load the card data (Personal Notes tab)
//*****************************************************
bool MainWnd::loadCardDataPersoData( void )
{
	PTEID_EIDCard * new_card = NULL;

    if (!m_CI_Data.isDataLoaded())
        return false;

	getCardForReading(new_card);

	if (new_card != NULL)
	{
		LoadDataPersoData(*new_card);
	}

	return true;
}

void MainWnd::SetValidCertificate(){

	QTreeWidgetItemIterator it(m_ui.treeCert);
	while (*it) {
		QTreeCertItem *item = dynamic_cast<QTreeCertItem *>(*it);

        if ( 0 == item->childCount() ) {
            QDate currentDate = QDate::currentDate();
            QDate ValidityBegin = QDate::fromString( item->getValidityBegin()
                                                    ,"dd/MM/yyyy");
            QDate ValidityEnd = QDate::fromString( item->getValidityEnd()
                                                    ,"dd/MM/yyyy");

            if ( ( currentDate >= ValidityBegin )
                && ( currentDate <= ValidityEnd)) {

                m_ui.treeCert->setCurrentItem( item );
                //m_ui.treeCert->setFocus( Qt::OtherFocusReason );
                break;
            }
        }

		++it;
	}
}

//*****************************************************
// load the card data (Certificates tab)
//*****************************************************
void MainWnd::loadCardDataCertificates( void )
{

	PTEID_EIDCard * new_card = NULL;

    if (!m_CI_Data.isDataLoaded())
        return;

	getCardForReading(new_card);

	if (new_card != NULL)
	{
		 LoadDataCertificates(*new_card);
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
	QString strVersion (PTEID_PRODUCT_VERSION);
	m_Settings.setGuiVersion(strVersion);

	dlgAbout * dlg = new dlgAbout(m_Settings.getGuiVersion() +" - "+ SVN_REVISION_STR, this);
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
void MainWnd::show_window_parameters() {
	if( this->isHidden() )
	{
		this->showNormal(); // Otherwise the application will end if the options dialog gets closed
	}

	dlgOptions* dlg = new dlgOptions(m_Settings, m_CurrReaderName, this);
	dlg->setShowPicture(m_Settings.getShowPicture());
	dlg->setShowNotification(m_Settings.getShowNotification());

	m_ui.actionOptions->setEnabled(false);

	if (dlg->exec())
	{
		//Reapply proxy settings
		applyProxyConfiguration();

		if ( !m_ui.txtIdentity_Name->text().isEmpty())
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
	if(m_CI_Data.isDataLoaded())
	{
		dlgSignature* dlgsig = new dlgSignature(this, m_Settings.getSelectedReader(), m_CI_Data);
		dlgsig->exec();
		delete dlgsig;
	} else {
		QString caption  = tr("Warning");
	  	QString msg = tr("Please insert your card on the smart card reader");
	  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	  	msgBoxp.exec();
	}
}

void MainWnd::actionPDFSignature_triggered()
{

	if(m_CI_Data.isDataLoaded())
	{

		m_pdf_signature_dialog = new PDFSignWindow(this, m_Settings.getSelectedReader(), m_CI_Data);
 		m_pdf_signature_dialog->exec();
 		delete m_pdf_signature_dialog;
		m_pdf_signature_dialog = NULL;
	}
	else
	{
		QString caption  = tr("Warning");
	  	QString msg = tr("Please insert your card on the smart card reader");
	  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	  	msgBoxp.exec();

	}

}


//*****************************************************
// VerifySignature clicked
//*****************************************************
void MainWnd::actionVerifySignature_eID_triggered()
{
	QString DSS_JAR("dss-standalone-app-3.0.4.jar");

	//Launch the exe wrapper on Windows, we need it for the file association
#ifdef _WIN32
	QStringList arguments;

	QString language = m_Settings.getGuiLanguageString() == "nl" ? QString("pt") : m_Settings.getGuiLanguageString();
	//Add language code as args[0]
	arguments << language;

	QProcess::startDetached(m_Settings.getExePath()+"/DSS/dss-standalone.exe", arguments);
	//launchJavaProcess(m_Settings.getExePath() + "/DSS/" + DSS_JAR, "", "");
#elif __APPLE__
	launchJavaProcess(QString::fromUtf8("/Applications/Validacao de assinaturas.app/Contents/Java/") + DSS_JAR,
			 QString::fromUtf8("-Xdock:name=Validação de assinatura"),  "");
#else
	launchJavaProcess(m_Settings.getExePath() + "/DSS/" + DSS_JAR , "", "");
#endif
}

//*****************************************************
// SCAPSignature clicked
//*****************************************************
void MainWnd::actionSCAPSignature_triggered()
{
	if(m_CI_Data.isDataLoaded())
	{

        QString SCAP_EXE("/ScapSignature");

        #ifdef _WIN32
            SCAP_EXE += ".exe";
        #endif
            QStringList args;

            QProcess::startDetached(m_Settings.getExePath()+SCAP_EXE, args);
	}
	else
	{
		QString caption  = tr("Warning");
	  	QString msg = tr("Please insert your card on the smart card reader");
	  	QMessageBox msgBoxp(QMessageBox::Warning, caption, msg, 0, this);
	  	msgBoxp.exec();

	}
}

//*****************************************************
// Print clicked
//*****************************************************
void MainWnd::on_actionPrint_eID_triggered()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	if(m_CI_Data.isDataLoaded())
	{
	  	dlgPrint* dlg = new dlgPrint( this, m_CI_Data, m_Language);
		dlg->exec();
		delete dlg;
	} else {
		QString caption  = tr("Warning");
		QString msg = tr("Please insert your card on the smart card reader");
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

#if !defined(WIN32)
//*****************************************************
// Set main window geometry
//*****************************************************
void MainWnd::setWndGeometry( const QRect &wndGeometry ){
    m_WndGeometry.x = wndGeometry.x();
    m_WndGeometry.y = wndGeometry.y();
    m_WndGeometry.width = wndGeometry.width();
    m_WndGeometry.height = wndGeometry.height();
}
#endif /* !defined(WIN32) */

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
#if !defined(WIN32)
            setWndGeometry( this->geometry() );
			bool bResult   = pin.verifyPin("",triesLeft, true, &m_WndGeometry );
#else
			bool bResult   = pin.verifyPin("",triesLeft );
#endif /* !defined(WIN32) */

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
		ShowPTEIDError( msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError(msg );
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
#if !defined(WIN32)
            setWndGeometry( this->geometry() );
			bool bResult   = pin.verifyPin("",triesLeft, true, &m_WndGeometry );
#else
			bool bResult   = pin.verifyPin("",triesLeft );
#endif /* !defined(WIN32) */

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
		ShowPTEIDError( msg );

		return false;
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( msg );
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

#if !defined(WIN32)
            setWndGeometry( this->geometry() );
			bool bResult = pin.verifyPin("",triesLeft, true, &m_WndGeometry );
#else
			bool bResult = pin.verifyPin("",triesLeft );
#endif /* !defined(WIN32) */

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

				//Reset the user notes flag
				if (pinRef == PTEID_Pin::AUTH_PIN)
					pinNotes = 1;
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
		ShowPTEIDError( msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( msg );
	}
	return;
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

			PTEID_Pin &pin = ReaderContext.getEIDCard().getPins().getPinByPinRef(pinRef);

			unsigned long triesLeft = -1;
#if !defined(WIN32)
            setWndGeometry( this->geometry() );
			bool bResult = pin.changePin("","",triesLeft, pin.getLabel(), true, &m_WndGeometry );
#else
			bool bResult = pin.changePin("","",triesLeft, pin.getLabel() );
#endif /* !defined(WIN32) */

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
				if (pinRef == 0x81 || pinRef == 0x01)
					pinNotes = 1;
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
		ShowPTEIDError( msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( msg );
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

			refreshTabIdentity();
			refreshTabIdentityExtra();
			refreshTabCardPin();

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

QTreeCertItem* MainWnd::buildTree(PTEID_Certificate &cert, bool &bEx)
{
	if (cert.isRoot())
	{
		// put the root cert as the tree top
		return new QTreeCertItem(m_ui.treeCert, cert);
	}
	else
	{
		// search for the cert in the widgettree
		QList<QTreeWidgetItem *> listItem = m_ui.treeCert->findItems(
											QString::fromUtf8(cert.getOwnerName()),
											Qt::MatchContains | Qt::MatchRecursive);

		if (listItem.isEmpty() || dynamic_cast<QTreeCertItem *>(listItem.first())->getLabel().compare(QString::fromUtf8(cert.getLabel())) != 0) {
			// when not in the tree add it

			try {
                return new QTreeCertItem(buildTree(cert.getIssuer(), bEx), cert);
			} catch (PTEID_ExCertNoIssuer &ex) {
				bEx = true;
				return new QTreeCertItem(m_ui.treeCert, cert);
			}
		}
		else
		{
			return dynamic_cast<QTreeCertItem *>(listItem.first());
		}
	}
}

void MainWnd::connectTreeCertItems(void)
{
	QTreeWidgetItemIterator it(m_ui.treeCert);
	while (*it)
	{
		QTreeCertItem *item = dynamic_cast<QTreeCertItem *>(*it);

		connect(item, SIGNAL(certStatusReady(PTEID_CertifStatus)),
				this, SLOT  (showCertStatusSideinfo(PTEID_CertifStatus)));

		++it;
	}
}

void MainWnd::fillCertificateList( void )
{
	bool noIssuer = false;

	PTEID_Certificates* certificates = m_CI_Data.m_CertifInfo.getCertificates();

	if (!certificates)
	{
		fprintf(stderr, "fillCertificateList() could not load certificates!\n");
		return;
	}

    buildTree(certificates->getCert(PTEID_Certificate::CITIZEN_AUTH), noIssuer);
    buildTree(certificates->getCert(PTEID_Certificate::CITIZEN_SIGN), noIssuer);

	connectTreeCertItems();

	m_ui.treeCert->expandAll();
	m_ui.treeCert->setColumnCount(1);
	m_ui.treeCert->sortItems(0, Qt::DescendingOrder);
	if (m_ui.treeCert->topLevelItem(0))
	{
		m_ui.treeCert->setCurrentItem(m_ui.treeCert->topLevelItem(0));
	}

    //m_ui.treeCert->setFocus( Qt::OtherFocusReason );
    if ( isLinkToCertClicked ) SetValidCertificate();

	if (noIssuer)
	{
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

	if (!m_CI_Data.isDataLoaded())
	{
	    //Load data from card in a new thread
		CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName, this);
		QFuture<LoadingErrorCode> future = QtConcurrent::run(&loader, &CardDataLoader::Load);
		this->FutureWatcher.setFuture(future);
		ProgressExec();

		LoadingErrorCode rc = future.result();
		if (rc == LOADING_ERROR_SOD)
		{
			QString title = tr("SOD validation");
			QString msg = tr("SOD validation failed: card data consistency is compromised!");
			QMessageBox msgBoxcc(QMessageBox::Warning, title, msg, 0, this);
			msgBoxcc.setModal(true);
			msgBoxcc.exec();
		}
		else if (rc == LOADING_ERROR_SOD_OUTDATED)
		{
			QString title = tr("SOD validation");
			QString msg = tr("SOD validation failed: card data consistency is compromised!") + "<br/><br/>";
			msg += tr("Please update your software at ")+"<a href=\'https://www.autenticacao.gov.pt/cc-aplicacao\'>Autenticação.Gov</a>" +"<br/><br/>";
			msg += tr("If this error persists, please contact the"
					" Citizen Card support line at telephone number +351 211 950 500 or e-mail cartaodecidadao@irn.mj.pt.");
			QMessageBox msgBoxcc(QMessageBox::Warning, title, msg, 0, this);
			msgBoxcc.setTextFormat(Qt::RichText);
			msgBoxcc.setModal(true);
			msgBoxcc.exec();
		}
		else if (rc == LOADING_ERROR_GENERIC)
		{
			QString	msg = tr("Error loading card data");
			
			ShowPTEIDError( msg );
		}
		else
		{
			//Load the picture in PNG format
			QPixmap pixmap_photo;
			const int display_height = 269;
			pixmap_photo.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData);

			QPixmap imgPicturescaled = pixmap_photo.scaledToHeight(display_height, Qt::SmoothTransformation);
			m_imgPicture = imgPicturescaled;
			fillPinList();
		}

	}
}


void MainWnd::LoadDataAddress(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();

	m_CI_Data.LoadDataAddress(Card, m_CurrReaderName);

	if(!m_CI_Data.isDataLoaded())
	{
		//clearTabCertificates();
		//clearTabAddress();

		//clearTabPins();
		//fillPinList( Card );
	}
}

void MainWnd::ProgressExec() {
    if ( NULL == m_progress ) return;

	QString labelText = tr("Reading card data...");
	m_progress->reset();
	m_progress->setLabelText((const QString)labelText);
	m_progress->adjustSize();

    m_progress->exec();
}

void MainWnd::LoadDataPersoData(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_ui.btnPersoDataSave->setEnabled(true);
	m_TypeCard = Card.getType();
	CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName);
	QFuture<LoadingErrorCode> future = QtConcurrent::run(loader, &CardDataLoader::LoadPersoData);
	this->FutureWatcher.setFuture(future);
	ProgressExec();
}

void MainWnd::LoadDataCertificates(PTEID_EIDCard& Card)
{
	setEnabledPinButtons(false);
	setEnabledCertifButtons(false);
	m_TypeCard = Card.getType();

	CardDataLoader loader(m_CI_Data, Card, m_CurrReaderName);
	//loader.LoadCertificateData();

	QFuture<LoadingErrorCode> future = QtConcurrent::run(loader, &CardDataLoader::LoadCertificateData);
	this->FutureWatcher.setFuture(future);
	//if(!m_CI_Data.isDataLoaded())
	ProgressExec();
	this->FutureWatcher.waitForFinished();

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
	pinTreeItem->setText(COLUMN_PIN_NAME, translateText( m_pinsInfo[PTEID_Pin::AUTH_PIN]->pin_name ) ); 
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::AUTH_PIN));

	pinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );
	pinTreeItem->setText(COLUMN_PIN_NAME, translateText( m_pinsInfo[PTEID_Pin::SIGN_PIN]->pin_name ) ); 
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::SIGN_PIN));

	pinTreeItem = new QTreeWidgetItem( TYPE_PINTREE_ITEM );
	pinTreeItem->setText(COLUMN_PIN_NAME, translateText( m_pinsInfo[PTEID_Pin::ADDR_PIN]->pin_name ) );
	m_ui.treePIN->addTopLevelItem ( pinTreeItem );
	pinTreeItem->setData(0, Qt::UserRole, QVariant((uint)PTEID_Pin::ADDR_PIN));

	m_ui.treePIN->expandAll();

	m_ui.treePIN->topLevelItem(0)->setSelected(true);
	m_ui.treePIN->setCurrentItem (m_ui.treePIN->topLevelItem(0));
}

void MainWnd::loadPinData(PTEID_EIDCard& Card)
{
	unsigned long pinId;
	PTEID_Pins& Pins = Card.getPins();

	PTEID_Pin& pinAuth = Pins.getPinByPinRef(PTEID_Pin::AUTH_PIN);
	pinId = pinAuth.getId();
	m_pinsInfo[PTEID_Pin::AUTH_PIN] = new PinInfo(pinId, pinAuth.getLabelById(pinId), pinAuth.getTriesLeft());

	PTEID_Pin& pinSign = Pins.getPinByPinRef(PTEID_Pin::SIGN_PIN);
	pinId = pinSign.getId();
	m_pinsInfo[PTEID_Pin::SIGN_PIN] = new PinInfo(pinId, pinSign.getLabelById(pinId), pinSign.getTriesLeft());

	PTEID_Pin& pinAddr = Pins.getPinByPinRef(PTEID_Pin::ADDR_PIN);
	pinId = pinAddr.getId();
	m_pinsInfo[PTEID_Pin::ADDR_PIN] = new PinInfo(pinId, pinAddr.getLabelById(pinId), pinAddr.getTriesLeft());
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

	const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();

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

QString MainWnd::translateText(QString &qStr)
{
    return tr( qStr.toUtf8().constData() );
}

//*****************************************************
// refresh the tab with the ID extra info (card back side)
//*****************************************************
void MainWnd::refreshTabIdentityExtra()
{
	tFieldMap& CardFields = m_CI_Data.m_CardInfo.getFields();

	const tFieldMap PersonFields = m_CI_Data.m_PersonInfo.getFields();

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

	m_ui.txtIdentityExtra_LocalofRequest->setText( QString::fromUtf8(PersonFields[LOCALOFREQUEST].toStdString().c_str()) );
	m_ui.txtIdentityExtra_LocalofRequest->setAccessibleName	( QString::fromUtf8(PersonFields[LOCALOFREQUEST].toStdString().c_str()) );

	QString card_validation = QString::fromUtf8(PersonFields[VALIDATION].toStdString().c_str());
	card_validation = translateText(card_validation);
	m_ui.txtIdentityExtra_Validate->setText (card_validation);
	m_ui.txtIdentityExtra_Validate->setAccessibleName ( card_validation );

	if ( PersonFields[LINK_TO_CERT] != "" ){
        QString linkStr = QString::fromUtf8(PersonFields[LINK_TO_CERT].toStdString().c_str());
        linkStr = translateText( linkStr );

        linkStr.replace( ",", ",\n" );

        m_ui.btnIdentityExtra_linkToCert->setText( linkStr );
        m_ui.btnIdentityExtra_linkToCert->setAccessibleName ( linkStr );
        m_ui.btnIdentityExtra_linkToCert->setVisible(true);
        m_ui.btnIdentityExtra_linkToCert->resize(m_ui.btnIdentityExtra_linkToCert->sizeHint());
    } else {
        m_ui.btnIdentityExtra_linkToCert->setVisible(false);
    }

	m_ui.txtIdentityExtra_ValidFrom->setText( QString::fromUtf8(CardFields[CARD_VALIDFROM].toStdString().c_str()));
	m_ui.txtIdentityExtra_ValidFrom->setAccessibleName( QString::fromUtf8(CardFields[CARD_VALIDFROM].toStdString().c_str()));

}

void MainWnd::clearAddressData(void) {
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
bool MainWnd::refreshTabAddress( void )
{
    if (!m_CI_Data.isDataLoaded())
        return false;

	if (pinactivate == 1)
	{
		if (!addressPINRequest_triggered())
			return false;
	}

	loadCardDataAddress();

	tFieldMap& AddressFields = m_CI_Data.m_AddressInfo.getFields();

	const int streetName_offset = 7;
	if (m_CI_Data.m_AddressInfo.isForeign())
	{

		m_ui.page_Address->setStyleSheet("");
		m_ui.page_Address->setStyleSheet("background-image: url(:/images/Images/tab-backgrounds/bg_address_foreign.png);");

		m_ui.txtAddress_District->setText(QString::fromUtf8(AddressFields[FOREIGN_COUNTRY].toStdString().c_str()) );
		m_ui.txtAddress_Municipality->setText( QString::fromUtf8(AddressFields[FOREIGN_CITY].toStdString().c_str()) );
		m_ui.txtAddress_CivilParish->setText( QString::fromUtf8(AddressFields[FOREIGN_REGION].toStdString().c_str()) );
		m_ui.txtAddress_StreetType1->setText( QString::fromUtf8(AddressFields[FOREIGN_POSTALCODE].toStdString().c_str()) );
		m_ui.txtAddress_StreetType2->setText( QString::fromUtf8(AddressFields[FOREIGN_LOCALITY].toStdString().c_str()) );

		if (!m_addressFieldNeedsReset)
		{
			QRect geometry = m_ui.txtAddress_StreetName->geometry();
			//Nasty hack to overcome the non-matching backgrounds and label position
			QRect geometry2 = m_ui.txtAddress_StreetType1->geometry();
			m_ui.txtAddress_StreetType1->move(geometry2.x(), geometry2.y()+3);

			QRect geometry3 = m_ui.txtAddress_StreetName->geometry();
			m_ui.txtAddress_CivilParish->move(geometry3.x(), geometry3.y()+3);

			geometry3 = m_ui.txtAddress_StreetType2->geometry();
			m_ui.txtAddress_StreetType2->move(geometry3.x(), geometry3.y()+3);

			m_addressFieldNeedsReset = true;

			//Nasty hack...
			m_ui.txtAddress_StreetName->setGeometry(geometry.x(), geometry.y()+streetName_offset, 448, geometry.height()-2);

		}

		m_ui.txtAddress_StreetName->setText( QString::fromUtf8(AddressFields[FOREIGN_ADDRESS].toStdString().c_str()) );

		m_ui.txtAddress_BuildingType1->hide();
		m_ui.txtAddress_BuildingType2->hide();
		m_ui.txtAddress_DoorNo->hide();
		m_ui.txtAddress_Floor->hide();
		m_ui.txtAddress_Side->hide();
		m_ui.txtAddress_Locality->hide();
		m_ui.txtAddress_Zip4->hide();
		m_ui.txtAddress_Zip3->hide();
		m_ui.txtAddress_Place->hide();
		m_ui.txtAddress_PostalLocality->hide();

		m_ui.page_Address->update();

	}
	else
	{
		m_ui.page_Address->setStyleSheet("");
		m_ui.page_Address->setStyleSheet("background-image: url(:/images/Images/tab-backgrounds/bg_address.png);");
		unsigned int streetname_width = 190;
		unsigned int streetname_height = 3;

		if (m_addressFieldNeedsReset)
		{
			QRect geometry = m_ui.txtAddress_StreetName->geometry();
			m_ui.txtAddress_StreetName->setGeometry( geometry.x()
                                                    , geometry.y() -streetName_offset
                                                    , streetname_width
                                                    , geometry.height() + streetname_height );
			m_addressFieldNeedsReset = false;
		}

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

		m_ui.txtAddress_BuildingType1->show();
		m_ui.txtAddress_BuildingType2->show();
		m_ui.txtAddress_DoorNo->show();
		m_ui.txtAddress_Floor->show();
		m_ui.txtAddress_Side->show();
		m_ui.txtAddress_Locality->show();
		m_ui.txtAddress_Zip4->show();
		m_ui.txtAddress_Zip3->show();
		m_ui.txtAddress_Place->show();
		m_ui.txtAddress_PostalLocality->show();
	}

	addressdatastatus = 0;
	return true;
}

// Set Max of 1000Bytes for personalNotes Field
void MainWnd::updatetext()
{
	int strnr;
	int totalct;

	QString TxtPersoDataString = m_ui.txtPersoData->toPlainText().toUtf8();
	strnr = TxtPersoDataString.toStdString().size() + 1; // '\0' should be considered as a byte

	totalct = 1000 - strnr;

	QString TotalBytes = QString::number(totalct);
	TotalBytes.append(" / 1000");
	m_ui.txtPersoDataCount->setText(TotalBytes);

	if ( strnr > 1000)
        m_ui.txtPersoData->textCursor().deletePreviousChar();
}

void MainWnd::PersoDataSaveButtonClicked( void )
{
    try {
        QString TxtPersoDataString = m_ui.txtPersoData->toPlainText().toUtf8();
        m_ui.txtPersoData->setMaximumBlockCount(1000);

        PTEID_ReaderContext &ReaderContext  = ReaderSet.getReaderByName(m_CurrReaderName.toLatin1().data());
        PTEID_EIDCard	 &Card	= ReaderContext.getEIDCard();

        if (pinNotes == 1)
            authPINRequest_triggered();

        if (pinNotes == 0)
        {
            if ( TxtPersoDataString.toStdString().size() > 0 ){
                const PTEID_ByteArray oData(reinterpret_cast<const unsigned char*> (TxtPersoDataString.toStdString().c_str()), (TxtPersoDataString.toStdString().size() + 1) );
                Card.writePersonalNotes(oData);
            }
            else {
                unsigned long ulSize = 1000;
                unsigned char *pucData = (unsigned char *)calloc( ulSize, sizeof(char) );

                const PTEID_ByteArray oData( (const unsigned char *)pucData, ulSize);
                Card.writePersonalNotes(oData);
                free(pucData);
            }
            QMessageBox::information( this, tr("Personal Notes"),  tr("Personal notes successfully written!"), QMessageBox::Ok );

        }
    } catch (PTEID_Exception& e) {
        QMessageBox::critical(this, tr("Personal Notes"), tr("Error writing personal notes!"), QMessageBox::Ok );
    }
}

//*****************************************************
// refresh the tab with PinCodes Data
//*****************************************************
void MainWnd::refreshTabPinCodes( void )
{
    if (!m_CI_Data.isDataLoaded()) return;

    int index = m_ui.treePIN->indexOfTopLevelItem(m_ui.treePIN->currentItem());

	fillPinList();/*llemos*/

	if ( index != -1 ){
        m_ui.treePIN->topLevelItem(index)->setSelected(true);
        m_ui.treePIN->setCurrentItem (m_ui.treePIN->topLevelItem(index));
	}
}

void MainWnd::enableTabPersoData( bool isEnabled )
{
    m_ui.txtPersoData->setEnabled(isEnabled);
    m_ui.txtPersoDataCount->setEnabled(isEnabled);
    m_ui.btnPersoDataSave->setEnabled(isEnabled);
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
	m_ui.txtCert_RevStatus->setText("");
	m_ui.txtCert_RevStatus->setAccessibleName("");
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
    if (!m_CI_Data.isDataLoaded()) return;

	certdatastatus = 0;
	loadCardDataCertificates();

	//qDebug() << "columns: " << m_ui.treeCert->columnCount();
	//qDebug() << "top level count: " << m_ui.treeCert->topLevelItemCount();
	//qDebug() << "selected items: " << m_ui.treeCert->selectedItems();
	//qDebug() << "# seleteced: " << m_ui.treeCert->selectedItems().size();
	//qDebug() << "find 'Signature': " << m_ui.treeCert->findItems ( QString("Signature"), Qt::MatchContains|Qt::MatchRecursive );

    /* TODO Rever este comentario */
    // vasco.dias: este bloco de código parece nao estar a fazer nada.
    // no fillCertificateList que corre antes disto dentro do loadCardCertificates
    // o cert de root ´e selected, logo os selected items sao 1 sendo o root o selecionado.
    // o findItems para tentar selecionar o cert com o nome "Signature" retorna vazio
    // porque não existe nenhum item na tree com esse texto.

	//QList<QTreeWidgetItem *> selectedItems = m_ui.treeCert->selectedItems();
	/*if(selectedItems.size() == 0)
	{
		//If no item is selected, we select the signature certificate
		selectedItems = m_ui.treeCert->findItems ( QString("Signature"), Qt::MatchContains|Qt::MatchRecursive );
        if (selectedItems.size() > 0)
		{
			selectedItems[0]->setSelected(true);
		}
	}

	if (selectedItems.size() > 0)
	{
		on_treeCert_itemClicked((QTreeCertItem *)selectedItems[0], 0);
	}*/

}
//*****************************************************
// set the tab with the PIN info
//*****************************************************
void MainWnd::setTabCardPin( QTreeWidgetItem *item )
{
    if ( item == NULL ) return;

    unsigned int pinRef = item->data(0,Qt::UserRole).value<uint>();
	PinInfo* pinfo = m_pinsInfo.find(pinRef)->second;
    /*unsigned long triesLeft = m_pinsInfo[pinRef]->triesLeft;*/

	QString status;
	if (1 == pinfo->triesLeft)
		status = tr("%1 try remaining").arg(pinfo->triesLeft);
	else
		status = tr("%1 tries remaining").arg(pinfo->triesLeft);

    QString pin_name;
    pin_name = translateText( pinfo->pin_name );

	m_ui.txtPIN_Name->setText( pin_name );
	m_ui.txtPIN_Name->setAccessibleName( pin_name );

	m_ui.txtPIN_ID->setText(pinfo->pin_id);
	m_ui.txtPIN_ID->setAccessibleName(pinfo->pin_id);

	m_ui.txtPIN_Status->setText(status);
	m_ui.txtPIN_Status->setAccessibleName(status);
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
                setTabCardPin( m_ui.treePIN->currentItem() );
                break;
			default:
				break;
			}
		}
	}
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

	QPixmap pixmap(":/images/Images/flags/pt32.png");
	QIcon ButtonIcon(pixmap);
	m_ui.btn_menu_language->setIcon(ButtonIcon);
	m_ui.btn_menu_language->setIconSize(pixmap.rect().size());
	m_ui.btn_menu_language->setToolTip(tr("Change language", "to english"));
}

//**************************************************
// Switch UI language to Portuguese
//**************************************************
void MainWnd::setLanguageNl( void )
{
	setLanguage(GenPur::LANG_NL);

	QPixmap pixmap(":/images/Images/flags/uk32.png");
	QIcon ButtonIcon(pixmap);
	m_ui.btn_menu_language->setIcon(ButtonIcon);
	m_ui.btn_menu_language->setIconSize(pixmap.rect().size());
	m_ui.btn_menu_language->setToolTip(tr("Change language", "to portuguese"));
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

					m_imgPicture = QPixmap();
					m_pinsInfo.clear();
					/*m_CI_Data.Reset();*/
					clearGuiContent();

					refreshTabIdentity();
					refreshTabIdentityExtra();
					refreshTabPersoData();
					enableTabPersoData(false);
					refreshTabCardPin();
					refreshTabCertificates();

					clearAddressData();
					m_ui.btnSelectTab_Identity->setFocus();

					if (m_pdf_signature_dialog)
						m_pdf_signature_dialog->disableSignButton();

				}
				//----------------------------------------------------------
				// card has been changed in a reader
				//----------------------------------------------------------
				if (pPopupEvent->getType() ==  PopupEvent::ET_CARD_CHANGED)
				{
					QString	cardReader = pPopupEvent->getReaderName();

					//Check if the card event is in the currently selected reader
					if (!m_CurrReaderName.isEmpty() && cardReader != m_CurrReaderName)
					{
						pEvent->accept();
						return;
					}

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
					m_ui.statusBar->showMessage(statusMsg, m_STATUS_MSG_TIME);

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

					enableTabPersoData(true);

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
						PTEID_EIDCard& card = readerContext.getEIDCard();
						doPicturePopup( card );

						//------------------------------------
						// register certificates when needed
						//------------------------------------
						/*
						if (m_Settings.getRegCert())
						{
							//XXX: Move this from here
							/*
							bool bImported = ImportCertificates(cardReader);
							if (!isHidden())
							{
								showCertImportMessage(bImported);
							}

						}
						*/
						if (isHidden())
						{
							break;
						}

						if ( m_Settings.getAutoCardReading() )
						{
							/* Fix more than 1 loadCardData reading */
							if ( !m_mutex_ReadCard.tryLock() ){
								pEvent->accept();
								return;
							}

                            /* m_CI_Data.Reset();   This operation will be done on loadCardData() */


							/* Allow new card data reading */
							m_mutex_ReadCard.unlock();
							loadCardData();
							//Clear the Auth PIN verification flag
							pinNotes = 1;
						}


						if (m_pdf_signature_dialog)
						{
							m_pdf_signature_dialog->enableSignButton();
						}
					}
					break;
					case PTEID_CARDTYPE_UNKNOWN:
					{
						clearGuiContent();
						QString msg(tr("Card read error or unknown card type"));
						ShowPTEIDError( msg );
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

    QImage myImage, myImagescaled;
    QPixmap				  pixMap;

    loadCardData();

    if (pixMap.loadFromData(m_CI_Data.m_PersonInfo.m_BiometricInfo.m_pPictureData, "PNG"))
    {

		const int display_width = 150;
        pixMap = pixMap.scaledToWidth(display_width, Qt::SmoothTransformation);
        m_Pop->setPixmap(pixMap);
        m_Pop->popUp();
    }
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
			refreshTabPinCodes(); //moved from on_btn_menu_language_clicked()

			// Refresh current certificate data 
			if (m_ui.treeCert->currentItem()) {
				syncTreeItemWithSideinfo(
						dynamic_cast<QTreeCertItem *>(m_ui.treeCert->currentItem()) );
			}
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
		ShowPTEIDError( msg );
	}
	catch (...)
	{
		QString msg(tr("Unknown exception"));
		ShowPTEIDError( msg );
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
			PTEID_ReaderContext& readerContext  = ReaderSet.getReaderByNum(Ix);
			CallBackData*		 pCBData		= new CallBackData(readerName, this);

			fCallback = (void (*)(long,unsigned long,void *))&cardEventCallback;

			m_callBackHandles[readerName] = readerContext.SetEventCallback(fCallback, pCBData);
			m_callBackData[readerName]	  = pCBData;
		}
	}
	catch(PTEID_Exception& e)
	{
		QString msg(tr("setEventCallbacks"));
		ShowPTEIDError( msg );
	}

}

//**************************************************
// display a messagebox with the error code
//**************************************************
void MainWnd::ShowPTEIDError( QString const& msg )
{
	if (isHidden())
	{
		return;
	}
	QString strCaption(tr("Error"));
	QString strMessage(msg);
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


#define AUTOUPDATES_URL "https://www.autenticacao.gov.pt/documents/10179/11461/version.txt"


/**
 Apply application-wide proxy settings
*/
void MainWnd::applyProxyConfiguration() {

	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);
	eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
	eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
	QString pac_url;
	QNetworkProxy proxy;

	//Before trying any request configure the proxy autoconfig
	/* XX: it should only require this to use the system proxy but it does not fallback to 
	//the configured PAC script if there is WPAD config but it is somewhat broken
	 QNetworkProxyFactory::setUseSystemConfiguration(true); */

	eIDMW::PTEID_Config config_pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
	const char * pacfile_url = config_pacfile.getString();

	if (pacfile_url != NULL && strlen(pacfile_url) > 0)
	{
		pac_url = QString(pacfile_url);
	}

    std::string proxy_host = config.getString();
    std::string proxy_username = config_username.getString();
    std::string proxy_pwd = config_pwd.getString();
    long proxy_port = config2.getLong();

	if (!proxy_host.empty() && proxy_port != 0)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "applyProxyConfiguration: using manual proxy config");
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(QString::fromStdString(proxy_host));
		proxy.setPort(proxy_port);

		if (!proxy_username.empty())
		{
			proxy.setUser(QString::fromStdString(proxy_username));
			proxy.setPassword(QString::fromStdString(proxy_pwd));
		}

		QNetworkProxy::setApplicationProxy(proxy);
	}
	else if (!pac_url.isEmpty())
	{
		std::string proxy_port_str;
		PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "applyProxyConfiguration: using system proxy config");

		PTEID_GetProxyFromPac(pac_url.toUtf8().constData(), AUTOUPDATES_URL, &proxy_host, &proxy_port_str);

		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(QString::fromStdString(proxy_host));
		proxy.setPort(atol(proxy_port_str.c_str()));
		QNetworkProxy::setApplicationProxy(proxy);
	}
	else {
		//Reset proxy settings
		QNetworkProxy no_proxy(QNetworkProxy::NoProxy);
		QNetworkProxy::setApplicationProxy(no_proxy);
	}
}

bool CardDataLoader::isUpdateAvailable(QString const &remote_data) {
	
	QString ver(WIN_GUI_VERSION_STRING);

	//Only consider the first line of version.txt
	QString firstLine = remote_data.left(remote_data.indexOf('\n'));
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Local version: %s", __FUNCTION__, ver.toUtf8().constData());
	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Remote version: %s", __FUNCTION__, firstLine.toUtf8().constData());

	QStringList list1 = ver.split(",");

	QStringList list2 = firstLine.split(",");

	if (list2.size() < 3)
	{
		PTEID_LOG(PTEID_LOG_LEVEL_ERROR, "eidgui",
			"%s: Wrong data returned from server or Proxy HTML error!", __FUNCTION__);

		return false;
	}

	//Parse local version into PteidVersion
	PteidVersion local_version;
	local_version.major = list1.at(0).toInt();
	local_version.minor = list1.at(1).toInt();
	local_version.release = list1.at(2).toInt();

	//Parse remote version into PteidVersion
	PteidVersion remote_version;
	remote_version.major = list2.at(0).toInt();
	remote_version.minor = list2.at(1).toInt();
	remote_version.release = list2.at(2).toInt();

	PTEID_LOG(PTEID_LOG_LEVEL_DEBUG, "eidgui", "%s: Parsed versions: Remote= %d.%d.%d Local= %d.%d.%d", 
		__FUNCTION__,
		remote_version.major, remote_version.minor, remote_version.release, 
		local_version.major, local_version.minor, local_version.release);

	return compareVersions(local_version, remote_version) > 0 ;
}

LoadingErrorCode CardDataLoader::Load()
{
	this->mwnd->clearErrorSOD();
	try
	{
		this->information.LoadData(card, readerName);

	}

	catch (PTEID_Exception &e)
	{
		qDebug() << "Caught exception in CardDataLoader::Load()..." << e.GetError();
		long errorCode = e.GetError();

		if (errorCode == EIDMW_SOD_ERR_VERIFY_SOD_SIGN) {
			//Check for outdated MW which could indicate missing PKI certificates
			QNetworkAccessManager manager;

			reply = manager.get(QNetworkRequest(QUrl(AUTOUPDATES_URL)));

			//This is a way to make the QNetwork call synchronous
			QEventLoop loop;
			QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
			loop.exec();

			if (reply->error() == QNetworkReply::NoError) {
				QByteArray replyData = reply->readAll();
				QString qsReplyData(replyData);

				if (isUpdateAvailable(replyData)) {
					return LOADING_ERROR_SOD_OUTDATED;	
				}
				else
					return LOADING_ERROR_SOD;
			}
			else {
				return LOADING_ERROR_SOD;
			}
		}
	    else if (errorCode >= EIDMW_SOD_UNEXPECTED_VALUE &&	 	
	       errorCode <= EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY)	 	
	    {
	       return LOADING_ERROR_SOD;
	    }
		else
			return LOADING_ERROR_GENERIC;
	}
	catch(std::exception &ex) {
		qDebug() << "Caught std::exception in CardDataLoader::Load()..." << ex.what();
		return LOADING_ERROR_GENERIC;
	}

	if (this->mwnd)
		this->mwnd->loadPinData(this->card);

	//Import certificates after loading data in the same thread
	if (this->mwnd->m_Settings.getRegCert())
	{
		bool bImported = MainWnd::ImportCertificates(this->mwnd->m_CurrReaderName);

		if (!this->mwnd->isHidden())
		{
			this->mwnd->showCertImportMessage(bImported);
		}
	}
	return LOADING_OK;
}

LoadingErrorCode CardDataLoader::LoadPersoData()
{
	try
	{
		this->information.LoadDataPersoData(card, readerName);
	}
	catch(PTEID_Exception &e)
	{
		return LOADING_ERROR_GENERIC;
	}

	return LOADING_OK;
	
}

LoadingErrorCode CardDataLoader::LoadCertificateData()
{
	try {
	this->information.LoadDataCertificates(card, readerName);

	}
	catch(PTEID_Exception &e)
	{
		return LOADING_ERROR_GENERIC;
	}

	return LOADING_OK;
}


void QTreeCertItem::init(PTEID_Certificate &cert) {
	this->cert = &cert;
	m_Issuer = QString::fromUtf8(cert.getIssuerName());
	m_Owner = QString::fromUtf8(cert.getOwnerName());
	m_ValidityBegin = QString::fromUtf8(cert.getValidityBegin());
	m_ValidityEnd = QString::fromUtf8(cert.getValidityEnd());
	m_KeyLen = QString::number(cert.getKeyLength());
	m_Label = QString::fromUtf8(cert.getLabel());

	connect(&certStatusWatcher, SIGNAL(finished()), this, SLOT(handleFutureCertStatus()));
}

void QTreeCertItem::handleFutureCertStatus() {

	PTEID_CertifStatus status = certStatusWatcher.result();

	emit certStatusReady(status);
}

void QTreeCertItem::askCertStatus() {

	QFuture<PTEID_CertifStatus> future = QtConcurrent::run(this->cert, &PTEID_Certificate::getStatus);
	certStatusWatcher.setFuture(future);
}
