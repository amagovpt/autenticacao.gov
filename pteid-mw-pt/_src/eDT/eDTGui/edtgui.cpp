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
#include <fstream>
#include <iostream>
#include <QString>
#include "edtgui.h"
#include "util.h"
#include "diaglib.h"
#include "repository.h"
//------------------------------------------
// main window ctor
//------------------------------------------
eDTGui::eDTGui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_currTestNr(0)
	, m_resolved(false)
{

	m_engineThread.setMainWnd(this);			// set main window pointer in the engine thread to allow callbacks
	qApp->installTranslator(&m_translator);		// install the translator engine to switch language if necessary

	ui.setupUi(this);

	ui.progressBar_Test->setMinimum(0);
	ui.progressBar_Test->setMaximum(100);

	ui.stackedWidget->setCurrentIndex (SCR_PRESENTATION);	// position at startup screen
	
	m_processingTimer = new QTimer(this);			// create timer to verify progress of the tests
	m_heartbeatTimer  = new QTimer(this);			// create heartbeat timer
	connect(m_processingTimer, SIGNAL(timeout()), this, SLOT(checkProgress()));
	connect(m_heartbeatTimer, SIGNAL(timeout()), this, SLOT(checkHeartbeat()));

	setFixedSize( 600, 400 );						// make sure we have a fixed, unsizable window

	//------------------------------------
	// set the window Icon (as it appears in the left corner of the window)
	//------------------------------------
	const QIcon Ico = QIcon( ":/Resources/Resources/ICO_CARD_EID_TOOLS_16x16.png" );
	this->setWindowIcon( Ico );

	QString version;
	version = diaglibVersion();
	ui.lbl_version->setText(ui.lbl_version->text()+version);
}

//------------------------------------------
// main window dtor
//------------------------------------------
eDTGui::~eDTGui()
{
#ifdef WIN32	//On Mac, the wait hangs
	m_engineThread.wait();							// allwais wait until the test engine is stopped
#endif
}

//------------------------------------------
// Check the progress of the tests
// This method is triggered by a timer.
// When the engine thread is idle, we consider the tests are done.
// If the engine is running, get the progress status and set it to the
// progress bar.
//------------------------------------------
void eDTGui::checkProgress()
{
	if (EngineThread::ENGINETHREAD_IDLE == m_engineThread.getStatus())
	{
		ui.progressBar_Overall->setValue(100);
		ui.progressBar_Test->reset();
		ui.progressBar_Test->setMaximum(100);
		ui.progressBar_Test->setValue(100);
		ui.lbl_testName->setText(tr("Done"));
		m_processingTimer->stop();
		ui.pb_Continue->setDisabled(false);
		ui.pb_Cancel->setDisabled(true);
		ui.pb_Continue_3->setDisabled(false);
		int progress = m_engineThread.getDiagEngine()->getProgress();
		if (progress>0)
		{
			ui.progressBar_signing->setValue(100);
		}
		else
		{
			ui.progressBar_signing->setValue(progress);
		}
		
	}
	else
	{
		int progress = m_engineThread.getDiagEngine()->getProgress();
		QProgressBar* pProgressBar = ui.progressBar_Test;
		if( SCR_SIGNING == ui.stackedWidget->currentIndex ())
		{
			pProgressBar = ui.progressBar_signing;
		}
		pProgressBar->setValue(progress);
	}
}

//------------------------------------------
// Check if the program is still running.
// This method is triggered by a timer.
// We consider the callbacks from the diagnostic library as a heartbeat.
// When a progress callback comes through, a timestamp is set in the engine.
// This timestamp is compared to the current time and if the timedifference
// is too high, a messagebox is popped up, informing the user that the system
// seems to be hanging
//------------------------------------------
void eDTGui::checkHeartbeat()
{
	if (EngineThread::ENGINETHREAD_IDLE != m_engineThread.getStatus())
	{
		time_t* pHeartBeat = m_engineThread.getDiagEngine()->getHeartbeat();
		time_t  currDateTime = time(NULL);

		double diffTime = difftime ( currDateTime, *pHeartBeat ); 

		//--------------------------------------------
		// check if the tests still generated a heartbeat
		//--------------------------------------------
		if (diffTime>HEARTBEAT_TIMEOUT)
		{
			if (QMessageBox::Yes == QMessageBox::information(this,tr("Test status"),tr("The tests seem to have stopped responding. Quit the eDT application?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes))
			{
				try
				{
					m_engineThread.terminate();
				}
				catch (...)
				{
					int x=0;
					x++;
				}
				qApp->quit();
			}
			*pHeartBeat = currDateTime;
		}
	}
}

//------------------------------------------
// load the translation resource depending on the selected language
//------------------------------------------
bool eDTGui::loadTranslationResources(QString const& m_Language)
{
	QString strTranslationFile(":/TranslationFiles/");
	strTranslationFile += "edtgui_" + m_Language + ".qm";

	if (!m_translator.load(strTranslationFile))
	{
		QMessageBox::information(this,"Error","Translation resource file not found");
		return false;
	}

	ui.retranslateUi(this);
	return true;
}

//------------------------------------------
// change from one screen to another
//------------------------------------------
void eDTGui::on_pb_Analyse_nl_clicked()
{
	m_Language = "nl";
	loadTranslationResources(m_Language);
	ui.stackedWidget->setCurrentIndex (SCR_INFORMATION);
}

void eDTGui::on_pb_Analyse_fr_clicked()
{
	m_Language = "fr";
	loadTranslationResources(m_Language);
	ui.stackedWidget->setCurrentIndex (SCR_INFORMATION);
}

void eDTGui::on_pb_Continue_2_clicked()
{
	ui.pb_Continue->setDisabled(true);
	ui.stackedWidget->setCurrentIndex (SCR_PROGRESS);
	
	m_currTestNr = 0;							// make sure we reset the counting of the tests
	m_engineThread.setTestType(EngineThread::TESTS_DIAGNOSTIC);	// tell the engine which tests to run
	m_engineThread.start();						// start the test thread
	m_processingTimer->start(PROGRESS_TIMER);	// start the test processing timer for the progress indication
	m_heartbeatTimer->start(HEARTBEAT_TIMER);	// start the heart beat timer
}

void eDTGui::on_pb_Continue_clicked()
{
	if (EngineThread::ENGINETHREAD_IDLE!=m_engineThread.getStatus())
	{
		return;
	}
	ui.progressBar_signing->setValue(0);

	if (!allTestsPassed())
	{
		ui.stackedWidget->setCurrentIndex (SCR_SUMMARY);
	}
	else
	{
		ui.stackedWidget->setCurrentIndex (SCR_SIGNING);
	}
}

void eDTGui::on_pb_Continue_3_clicked()
{
	if (EngineThread::ENGINETHREAD_IDLE!=m_engineThread.getStatus())
	{
		return;
	}
	ui.stackedWidget->setCurrentIndex (SCR_SUMMARY);
}

void eDTGui::on_pb_Details_clicked()
{
	ui.stackedWidget->setCurrentIndex (SCR_DETAILS);

}

void eDTGui::on_pb_Summary_clicked()
{
	ui.stackedWidget->setCurrentIndex (SCR_SUMMARY);
}

void eDTGui::on_pb_Summary_2_clicked()
{
	on_pb_Summary_clicked();
}

//------------------------------------------
// Quit the application
//------------------------------------------
void eDTGui::on_pb_Diag_clicked()
{
	m_currDiag=0;
	ui.stackedWidget->setCurrentIndex(SCR_DIAGNOSTICS);
}

void eDTGui::on_pb_Diag2_clicked()
{
	on_pb_Diag_clicked();
}

void eDTGui::on_pb_Quit_clicked()
{
	qApp->quit();
}

void eDTGui::on_pb_solved_clicked()
{
	solved();
}

void eDTGui::on_pb_not_solved_clicked()
{
	if(m_currDiag<REP_VERDICT_COUNT()-1)
		nextVerdict();
	else
		not_solved();
}

void eDTGui::on_pb_previous_clicked()
{
	previousVerdict();
}

void eDTGui::on_pb_next_clicked()
{
	nextVerdict();
}

void eDTGui::on_pb_needhelp_clicked()
{
	gettingHelp();
}

void eDTGui::previousVerdict(void)
{
	if(m_currDiag>0)
	{
		m_currDiag--;
		updateVerdict();
	}
}

void eDTGui::nextVerdict(void)
{
	if(m_currDiag<REP_VERDICT_COUNT()-1)
	{
		m_currDiag++;
		updateVerdict();
	}
}

void eDTGui::gettingHelp(void)
{
	ui.lbl_remedy->setText(tr("To get help in resolving this problem, please: \n\n- Click the \"Save\" button below and save the eDT report.\n- Send the saved report via e-mail to the service desk (if you received eDT via email, reply to that email)\n- Contact the service desk."));
}

void eDTGui::solved(void)
{
	MetaRuleVerdict verdict=REP_VERDICT(m_currDiag);

	ui.lbl_remedy->setText(tr("We're delighted the eDT helped solve your problem\n\nIf you want to help us improve the eDT, please click the \"Send\" button below to share the good news."));
	m_resolved=true;
	ui.pb_previous->setEnabled(false);
	ui.pb_next->setEnabled(false);
	ui.pb_solved->setEnabled(false);
	ui.pb_not_solved->setEnabled(false);
	ui.pb_needhelp->setEnabled(false);
	ui.pb_SaveSend->setText(tr("Send"));

	REP_PREFIX(L"diagnostics");
		REP_PREFIX(verdict.rulename());
			REP_CONTRIBUTE(L"customer_solved",L"true");
		REP_UNPREFIX();
	REP_UNPREFIX();
}

void eDTGui::not_solved(void)
{
	MetaRuleVerdict verdict=REP_VERDICT(m_currDiag);

	ui.lbl_remedy->setText(tr("We're sorry the eDT couldn't help solve your problem rightaway.\n\nTo get help in resolving this problem, please: \n\n- Click the \"Save\" button below and save the eDT report.\n- Send the saved report via e-mail to the service desk (if you received eDT via email, reply to that email)\n- Contact the service desk."));
	m_resolved=false;
	ui.pb_previous->setEnabled(false);
	ui.pb_next->setEnabled(false);
	ui.pb_solved->setEnabled(false);
	ui.pb_not_solved->setEnabled(false);
	ui.pb_needhelp->setEnabled(false);

	REP_PREFIX(L"diagnostics");
		REP_PREFIX(verdict.rulename());
			REP_CONTRIBUTE(L"customer_solved",L"false");
		REP_UNPREFIX();
	REP_UNPREFIX();
}


void eDTGui::updateVerdict(void)
{
	MetaRuleVerdict		verdict=REP_VERDICT(m_currDiag);
	std::wstringstream	titleText;
						titleText << L"Diagnostic:" << verdict.verdict() << L" (click for details)";
						
	ui.toolBox->setItemText(0,w2qstr(titleText.str()));
	ui.lbl_details->	setText(w2qstr(verdict.details()));
	ui.lbl_remedy->		setText(w2qstr(verdict.corrective()));

	ui.pb_previous->	setEnabled(REP_VERDICT_COUNT()>1 && m_currDiag!=0);
	ui.pb_next->		setEnabled(REP_VERDICT_COUNT()>1 && m_currDiag!=(REP_VERDICT_COUNT()-1));
	ui.lbl_which_diag->	setText((tostring<size_t>(m_currDiag+1) + "/" + tostring<size_t>(REP_VERDICT_COUNT())).c_str());
}

void eDTGui::closeEvent(QCloseEvent* event)
{
	m_engineThread.wait();
	if ( EngineThread::ENGINETHREAD_IDLE == m_engineThread.getStatus()) 
	{
		event->accept();
	}
	else 
	{
		event->ignore();
	}
}

//------------------------------------------
// we changed to another screen
//------------------------------------------
void eDTGui::on_stackedWidget_currentChanged(int index)
{
	switch(index)
	{
	case SCR_PRESENTATION:
		break;
	case SCR_INFORMATION:
		{
			QString msg;
			msg =  tr("This tool will try to detect if the Portugal eID Middleware is properly installed and working.");
			msg += "\n";
			msg += tr("Make sure that:");
			msg += "\n";
			msg += tr("- at least one card reader is connected");
			msg += "\n";
			msg += tr("- one card is inserted in a reader");
			msg += "\n";
			msg += "\n";
			msg += tr("During the test, access permission to the eID card will be requested.");
			ui.lbl_instructions->setText(msg);

			ui.pb_Continue_2->setText(tr("Continue"));
		}
		break;
	case SCR_PROGRESS:
		ui.pb_Continue->setText(tr("Continue"));
		ui.pb_Cancel->setText(tr("Cancel"));
	    break;
	case SCR_SIGNING:
		{
			if (allTestsPassed())
			{
				ui.pb_testAuthentication->setDisabled(false);
			}
			else
			{
				ui.pb_testAuthentication->setDisabled(true);
			}
			QString msg(tr("Test Authentication "));
			msg += tr("Testing the authentication is done with both the PKCS11 modules and the");
#ifdef WIN32
			msg += tr("CSP");
#else
			msg += tr("tokend");
			msg += tr("module. ");
#endif
			msg += tr("For this test, the PIN code of the inserted eID card is required.");
			msg += tr("The PIN code is only used to verify the correct operation of the cryptographic modules and is never saved.");

			ui.lbl_signing->setText(msg);
		}
	    break;
	case SCR_SUMMARY:
		{
			ui.tbl_Summary->setRowCount(m_engineThread.getNrOfExecutedTests());
			ui.tbl_Summary->setColumnCount(SUMMARY_NRCOLUMNS);

			if (allTestsPassed())
			{
				ui.lbl_Status->setText(tr("Passed"));
			}
			else
			{
				ui.lbl_Status->setText(tr("Failed"));
			}

			fillTestTableGrid();
		}
		break;
	case SCR_DETAILS:
		{
			
			if (allTestsPassed())
			{
				ui.lbl_Status_2->setText(tr("Passed"));
			}
			else
			{
				ui.lbl_Status_2->setText(tr("Failed"));
			}

			if (!ui.te_Report->document()->isEmpty())
			{
				return;
			}
			loadReport();
		}

		break;
	case SCR_DIAGNOSTICS:
	{
		if(REP_VERDICT_COUNT()>0)
		{
			m_currDiag=0;
			updateVerdict();
			ui.pb_solved->		setEnabled(true);
			ui.pb_not_solved->	setEnabled(true);
			ui.pb_needhelp->	setEnabled(true);
		}
		else
		{
			ui.toolBox->setItemText(0,tr("Diagnostic: No Known Issues"));
			ui.lbl_details->	setText(tr("The Diagnostics Module found no obvious or known issues with your system and/or eID installation."));
			ui.lbl_remedy->		setText(tr("We're sorry the eDT couldn't help solve your problem rightaway.\n\nTo get help in resolving this problem, please: \n\n- Click the \"Save\" button below and save the eDT report.\n- Send the saved report via e-mail to the service desk (if you received eDT via email, reply to that email)\n- Contact the service desk."));
			ui.pb_solved->		setEnabled(false);
			ui.pb_not_solved->	setEnabled(false);
			ui.pb_needhelp->	setEnabled(false);
			ui.pb_previous->	setEnabled(false);
			ui.pb_next->		setEnabled(false);
			ui.lbl_which_diag->	setText(tr("0/0"));
		}
	}

	break;
		default:
	    break;
	}
}

//------------------------------------------
// fill the grid with the performed tests
//------------------------------------------
void eDTGui::fillTestTableGrid()
{
	QStringList labels;
	labels << tr("Controle") << tr("Result");
	ui.tbl_Summary->setHorizontalHeaderLabels ( labels );

	for (size_t row=0;row<m_engineThread.getNrOfExecutedTests();row++)
	{
		size_t column = 0;
		QTableWidgetItem* newItem = NULL;

		std::string testName		 = m_engineThread.getExecutedTestName(row);
		std::string testFriendlyName = m_engineThread.getTestFriendlyName(testName);

		newItem = new QTableWidgetItem(QString(testFriendlyName.c_str()),QTableWidgetItem::Type);
		ui.tbl_Summary->setItem(row, column, newItem);
		column++;
		QString msg(tr("Passed"));
		QFont font;
		int testErrorCode = m_engineThread.getTestErrorCode(row);
		QColor color(Qt::darkGreen);
		if ( ANALYSE_TEST_NOT_FOUND == testErrorCode )
		{
			msg = tr("Skipped");
			color = QColor(Qt::black);
		}
		else if( !m_engineThread.testPassed(testName) )
		{
			msg = tr("Failed");
			font.setBold(true);
			color = QColor(Qt::red);
		}
		// else { /* it's OK */ }
		newItem = new QTableWidgetItem(msg,QTableWidgetItem::Type);
		newItem->setFont(font);
		newItem->setTextColor(color);
		ui.tbl_Summary->setItem(row, column, newItem);
	}
	ui.tbl_Summary->resizeColumnToContents(0);
	ui.tbl_Summary->horizontalHeader()->setStretchLastSection( true );
}

//------------------------------------------
//------------------------------------------
void eDTGui::customEvent( QEvent* pEvent )
{
	if (pEvent)
	{
		std::string testName = ((ProcessEvent*)pEvent)->getTestName();
		std::string testFriendlyName = m_engineThread.getTestFriendlyName(testName);

		QLabel* pLblTestName = ui.lbl_testName;

		if (SCR_SIGNING == ui.stackedWidget->currentIndex())
		{
			pLblTestName = ui.lbl_testSignature;
		} 
		
		if (0<testFriendlyName.length())
		{
			pLblTestName->setText(testFriendlyName.c_str());
		}
		else
		{
			pLblTestName->setText(testName.c_str());
		}

		int progress = ((ProcessEvent*)pEvent)->getProgress();
		int overallProgress = 0;
		size_t nrTests = m_engineThread.getNrRequestedTests();

		if (EngineThread::ENGINETHREAD_IDLE != m_engineThread.getStatus())
		{
			overallProgress = (m_currTestNr*100)/nrTests;
			ui.progressBar_Overall->setValue(overallProgress);
			if (100 == overallProgress)
			{
				ui.progressBar_Test->reset();
				ui.progressBar_Test->setMaximum(100);
				ui.progressBar_Test->setValue(100);
				pLblTestName->setText(tr("Done"));
			}
		}
		// we start a new test?
		if (progress == 0)
		{
			m_currTestNr++;
		}
	}
}

//------------------------------------------
// start authentication tests
//------------------------------------------
void eDTGui::on_pb_testAuthentication_clicked()
{
	m_currTestNr = 0;
	m_engineThread.setTestType(EngineThread::TESTS_INTERACTIVE);	// tell the engine which tests to do
	m_engineThread.start();											// start the engine thread
	ui.pb_testAuthentication->setDisabled(true);
	ui.pb_Continue_3->setDisabled(true);
	m_processingTimer->start();
}

//------------------------------------------
// save the report
//------------------------------------------

void eDTGui::on_pb_SaveReport_clicked()
{
	if (ui.te_Report->document()->isEmpty())
	{
		loadReport();
	}
	QString caption(tr("Save report as"));
	QDateTime dateTime = QDateTime::currentDateTime();
	QString strDateTime=dateTime.toString("yyyyMMdd_hhmmss");
	QString targetFile;
	targetFile += "eDT_Report_";
	targetFile += strDateTime;
	targetFile += ".txt";

#ifdef WIN32
	QSettings settings(QSettings::UserScope, "Microsoft", "Windows");
	settings.beginGroup("CurrentVersion/Explorer/Shell Folders");
	QString dir = settings.value("Personal").toString();
#else
	QString dir = QDir::homePath();
#endif

	QString fullpath;
	fullpath += dir;
	fullpath += "/";
	fullpath += targetFile;
	
	QString	fileName = QFileDialog::getSaveFileName(  this
													, caption
													, fullpath
													); 
	if (0<fileName.length())
	{
		QFile reportFile(fileName);
		if (!reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QString caption(tr("Save report"));
			QString msg(tr("Error opening report file"));
			msg += ":";
			msg += fileName;
			QMessageBox::information(this,caption,msg);
			return;
		}
		QTextStream out(&reportFile);
		out << ui.te_Report->document()->toPlainText();
		reportFile.close();
	}
	else
	{
		QString caption(tr("Save report"));
		QString msg(tr("No file name given"));
		QMessageBox::information(this,caption,msg);
		return;
	}
}

void eDTGui::on_pb_SaveReport_2_clicked()
{
	on_pb_SaveReport_clicked();
}

void eDTGui::on_pb_SaveSend_clicked()
{
	if(!m_resolved)
		on_pb_SaveReport_clicked();
	else
		send_resolution();
}

void eDTGui::send_resolution(void)
{
		MetaRuleVerdict verdict=REP_VERDICT(m_currDiag);
		QString mailaddress("servicedesk@fedict.be");
		QString subject("eDT::Diagnostics Self-Resolution");
		QString body(w2qstr(L"rule: " + verdict.rulename() + L"\n\n"));
				body.append(w2qstr(L"verdict:\n" + verdict.verdict() + L"\n"));
				body.append(w2qstr(L"details:\n" + verdict.details() + L"\n"));
				body.append(w2qstr(L"suggestion:\n" + verdict.corrective() + L"\n"));
				body.append("The customer indicates that the above suggestion solved the problem.");

        QString mailCmd("mailto:");
        mailCmd += mailaddress;
        mailCmd += "?";
        mailCmd += "subject=";
        mailCmd += subject;

        QString encodedBody;
        encodedBody.clear();

        for (int idx=0;idx<body.length();idx++)
        {
                if(body.at(idx) == '&')
                {
                        encodedBody += "%26";
                }
                else if(body.at(idx) == '%')
                {
                        encodedBody += "%25";
                }
                else
                {
                        encodedBody += body.at(idx);
                }
        }

        QUrl url(mailCmd);
        url.addQueryItem("body",encodedBody);

        bool bOpen = QDesktopServices::openUrl(url);

        if(!bOpen)
        {
                QMessageBox::information(this,tr("Mail client"),tr("Default mail client cannot be opened"));
        }
} 

//------------------------------------------
// load the test report 
//------------------------------------------
void eDTGui::loadReport( void )
{
	int retVal = DiagEngine::ENG_OK;

	for (int row=0; row<ui.tbl_Summary->rowCount(); row++)
	{
		for (int col=0; col<ui.tbl_Summary->columnCount(); col++)
		{
			QTableWidgetItem* item = ui.tbl_Summary->item(row,col);
			QString text = item->text();
		}
	}

	std::wstring reportFile;
	retVal = m_engineThread.getReportFilename(reportFile);

	if(0==reportFile.length())
	{
		QMessageBox::information(this,tr("Error"),tr("Report generation failed"));
		return;
	}

	QString qstrReportFile;
	
	for (size_t idx=0;idx<reportFile.length();idx++)
	{
		qstrReportFile += reportFile.at(idx);
	}
	QFile inFile(qstrReportFile);
	inFile.open(QIODevice::ReadOnly|QIODevice::Text);
	QTextStream in(&inFile);
	in.setCodec("UTF-8");
	while (!in.atEnd()) 
	{
		QString line = in.readLine();
		ui.te_Report->append(line);
	}
	inFile.close();

	QTextCursor cursor = ui.te_Report->textCursor();
	cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
	ui.te_Report->setTextCursor(cursor);
}

void eDTGui::on_pb_Cancel_clicked()
{
	m_engineThread.setRequest(EngineThread::ENGINETHREAD_REQUEST_STOP);
	ui.pb_Cancel->setDisabled(true);
}

bool eDTGui::allTestsPassed()
{
	for (size_t idx=0;idx<m_engineThread.getNrOfExecutedTests();idx++)
	{
		int testErrorCode = m_engineThread.getTestErrorCode(idx);
		std::string testName = m_engineThread.getExecutedTestName(idx);
		if( !m_engineThread.testPassed(testName) )
		{
			return false;
		}
	}
	return true;
}

QString eDTGui::w2qstr(const std::wstring wstr)
{
	return QString(string_From_wstring(wstr).c_str());
}