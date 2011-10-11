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
#include "ezinstaller.h"
#include "qpixmap.h"

#include "Platform.h"
#ifdef WIN32
#include "resource.h"
#include "diaglib.h"
#include "zrclib.h"
#else
#endif
#include <string>
#include "confirmdialogbox.h"
#include <QPrinter>
#include <QThread>
#include <QMessageBox>
#include <QDir>
#include "accessibleDialogbox.h"

#define LOGSTR(msg)
#define LOGINT(format, i)

using namespace std;

#ifndef WIN32
// Holds the credentials for the 'doAsAdmin' function.
AuthorizationRef ezInstaller::authRef=NULL;
#endif
string ezInstaller::currentLanguage="";

void ezInstaller::customEvent(QEvent * qe )
{
    verboseEvent * ve = (verboseEvent *)qe;
    if (ve->getAction() == "append") {appendString(ve->getText().toStdString(),ve->getWeight());}
    else  if (ve->getAction() == "insert") {insertString(ve->getText().toStdString(),ve->getWeight());}
    else  if (ve->getAction() == "appendReport") {appendStringReport(ve->getText().toStdString(),ve->getWeight());}
    else  if (ve->getAction() == "insertReport") {insertStringReport(ve->getText().toStdString(),ve->getWeight());}
    else  if (ve->getAction() == "initProgressBar") {ui.progressBar->setMaximum(ve->getint1());setPercentage(0);}
    else  if (ve->getAction() == "setPercentage") {setPercentage(ui.progressBar->value() + ve->getint1());}
    else  if (ve->getAction() == "restoreCursor") {
        qApp->restoreOverrideCursor();
    }
    else  if (ve->getAction() == "readCard") {

        showCardData(ve->getstr1().toStdString());

        qApp->restoreOverrideCursor();

        ui.clbBack->setEnabled(true);
        ui.clbCancel->setEnabled(true);

    } else  if (ve->getAction() == "addDriverParams") {
        driverParameters.push_back(ve->getstr1().toStdString());
    } else  if (ve->getAction() == "delayedAction") {
        if (ve->getstr1() == QString("goToPage")) {
            ui.stackedWidget->setCurrentIndex(ve->getint1());
            ui.clbBack->setEnabled(true);
            qApp->restoreOverrideCursor();
        }
    } else if (ve->getAction() == "done") {
        this->mdiagthread_active = false;
		Sleep(2000);
		previousPage = ui.stackedWidget->currentIndex();

		if (dt.fatalErrorOccurred) {
            buildSummaryPage("diagnosticError");
            ui.stackedWidget->setCurrentIndex(6); // goto warningpage.
        }
        else {
			mInstallSucceeded = true;
			ui.stackedWidget->setCurrentIndex(2);         
        }
    } else if (ve->getAction() == "disableCancel") {
        ui.clbCancel->setEnabled(false);
    } else if (ve->getAction() == "enableCancel") {
        ui.clbCancel->setEnabled(true);
    }
	else  if (ve->getAction() == "detectedReader") {
		ui.clbBack->setEnabled(false);
		ui.stackedWidget->setCurrentIndex(3);
	}
	else  if (ve->getAction() == "detectedCard") {
		ui.stackedWidget->setCurrentIndex(4);
	}
}

ezInstaller::ezInstaller(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags) {

    transparentstyle = "color: rgba(0, 0, 0, 0);background-color: rgb(0, 0, 0,0);alternate-background-color: rgb(0, 0, 0,0);border-color: rgba(0, 0, 0, 0);border-top-color: rgba(0, 0, 0, 0);border-right-color: rgba(0, 0, 0, 0);border-left-color: rgba(0, 0, 0, 0);border-bottom-color: rgba(0, 0, 0, 0);gridline-color: rgba(0, 0, 0, 0);selection-color: rgba(0, 0, 0, 0);selection-background-color: rgba(0, 0, 0, 0);";

    mhave_readers = false;
	mInstallSucceeded = false; 
    this->mdiagthread_active = false;
    this->mSignatureOK = false;

    ui.setupUi(this);
    this->initImages();
    ui.teProgress->setVisible(false);
    ui.clbBack->setEnabled(false);

    ui.stackedWidget->setCurrentIndex(0);
    previousPage = 0;
    delaythread.setobjectToUpdate(this);
    setMessages();

	//dt.doStartProcess("<InputParams><commandline>pwd</commandline><waitForTermination>YES</waitForTermination></InputParams>");
}

ezInstaller::~ezInstaller() {
}
void ezInstaller::appendString(string str, int weight) {
    QTextCharFormat fmt;
    fmt.setFontWeight(weight);
    ui.teProgress->setCurrentCharFormat(fmt);
    ui.teProgress->append(QString(str.c_str()));
    QTextCursor  tq = ui.teProgress->textCursor();
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    QString stringToDisplay (str.c_str());
    ui.leProgress->setText(stringToDisplay.trimmed());
	ui.leProgress->setAccessibleName(stringToDisplay.trimmed());
}
void ezInstaller::insertString(string str, int weight) {
    QTextCursor  tq = ui.teProgress->textCursor();
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    ui.teProgress->setTextCursor(tq);
    QTextCharFormat fmt;
    fmt.setFontWeight(weight);
    ui.teProgress->setCurrentCharFormat(fmt);
    ui.teProgress->insertPlainText(QString(str.c_str()));
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    QString stringToDisplay(ui.leProgress->text() + QString(str.c_str()));
    ui.leProgress->setText(stringToDisplay.trimmed());
	ui.leProgress->setAccessibleName(stringToDisplay.trimmed());
}

void ezInstaller::appendStringReport(string str, int weight) {
    QTextCharFormat fmt;
    fmt.setFontWeight(weight);
    ui.teReport->setCurrentCharFormat(fmt);
    ui.teReport->append(QString(str.c_str()));
    QTextCursor  tq = ui.teReport->textCursor();
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
}
void ezInstaller::insertStringReport(string str, int weight) {
    QTextCursor  tq = ui.teReport->textCursor();
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    ui.teReport->setTextCursor(tq);
    QTextCharFormat fmt;
    fmt.setFontWeight(weight);
    ui.teReport->setCurrentCharFormat(fmt);
    ui.teReport->insertPlainText(QString(str.c_str()));
    tq.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
}


void ezInstaller::setPercentage(int step) {
    ui.progressBar->setValue(step);
}


void ezInstaller::on_btnNext_clicked() {


    // If whe're on the diagnostic page, check if there where fatal errors to see if
    // Next should go to warningpage.

    if (ui.stackedWidget->currentIndex() > 1)
        ui.clbBack->setEnabled(true);

    QString pagename;
    pagename = ui.stackedWidget->currentWidget()->objectName();
    if (pagename.contains("pageProgress")) {
        if (dt.fatalErrorOccurred) {
            previousPage = ui.stackedWidget->currentIndex();
            ui.stackedWidget->setCurrentIndex(6); // goto warningpage.
        }
    }
    else  {
        ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);
    }
}

void ezInstaller::on_stackedWidget_currentChanged(int)
{
    QString pagename;
    pagename = ui.stackedWidget->currentWidget()->objectName();

    ui.clbSaveAsPdf->setVisible(false);
    mQLsaveAsPdf.setVisible(false);

    ui.clbClose->setVisible(false);
	ui.clbClose->setEnabled(false);
    mQLClose.setVisible(false);
	mQLClose.setEnabled(false);

	ui.clbOpenReport->setVisible(false);
	mQLOpenReport.setVisible(false);

    if (pagename.contains("pageProgress")) {  // 1

#ifdef WIN32

		bool bFirst = true;
		int QuietTry = 3;
		while(1)
		{
			//Stop service
			if(scl.stopService("pteidPrivacyFilter") == "SUCCESS") Sleep(500);		//2.3, 2.4
			if(scl.stopService("BELGIUM_ID_CARD_SERVICE") == "SUCCESS") Sleep(500);	//2.3, 2.4

			if(scl.stopService("eID CRL Service") == "SUCCESS") Sleep(500);			//2.5, 2.6
			if(scl.stopService("eID Privacy Service") == "SUCCESS") Sleep(500);		//2.5, 2.6

			//Kill process
			if(scl.killProcess("eidGUI.exe") == "KILLED") Sleep(500);			//2.3

			if(scl.killProcess("pteidgui.exe") == "KILLED") Sleep(500);			//2.4, 2.5, 2.6, 3.0
			if(scl.killProcess("pteidsystemtray.exe") == "KILLED") Sleep(500);	//2.4, 2.5, 2.6
			if(scl.killProcess("PTeidCertImport.exe") == "KILLED") Sleep(500);	//???

			if(scl.killProcess("xsign.exe") == "KILLED") Sleep(500);			//3.0

			if(scl.killProcess("pteid35gui.exe") == "KILLED") Sleep(500);		//3.5
			if(scl.killProcess("pteid35xsign.exe") == "KILLED") Sleep(500);		//3.5

			if(!QuietTry)
			{
			// Let the cleanuptool handle this
/*				if(scl.isMiddlewareLocked())
				{
					QString _title = msgbox_closeApplicationsTitle;
					QString _body = msgbox_closeApplicationsBody;
					if(!bFirst)
					{
						_body+="\n\n";
						_body+=msgbox_closeApplicationsBody2;
					}
					QString _retryButton = msgbox_retry;
					QString _closeButton = msgbox_close;

					if(1==QMessageBox::question(this,_title,_body,_retryButton,_closeButton,QString::null,0,1))
					{
						this->close();
						break;
					}
					bFirst = false;
				}
				else
				{*/
					break;
//				}
			}
			else
			{
				QuietTry--;
				Sleep(500);
			}
		}
#endif


        this->mdiagthread_active = true;

        ui.clbBack->setEnabled(false);
        ui.teProgress->clear();
        setStepButtons(true,false,false);

        qApp->setOverrideCursor(Qt::WaitCursor);
        driverParameters.clear();

        dt.setobjectToUpdate(this);
        dt.setCurrentLanguage(currentLanguage);
        dt.start();
    }
    else {
        if (pagename.contains("pageReaderImages")) {  // 2

            this->appendStringReport(dt.eindeDiagWithoutError.toStdString(), QFont::Bold);

            setStepButtons(false,true,false);

            ui.clbBack->setEnabled(false);  // ** was true

            ui.lblConnectReaders->setText(msglbl_ConnectReaders);
			ui.lblConnectReaders->setAccessibleName(msglbl_ConnectReadersAcc);
            ui.lblReadingCard->setText(msglbl_ReadingCard);
			ui.lblReadingCard->setAccessibleName(msglbl_ReadingCardAcc);
            ui.lbleIDInserted->setText(msglbl_EidInserted);
			ui.lbleIDInserted->setAccessibleName(msglbl_EidInsertedAcc);
            previousPage = 2;               // ** was 1
			drt.setobjectToUpdate(this);
			drt.start();
        }
        else {
            if (pagename.contains("welcomePage")) { // 0
                ui.clbBack->setEnabled(false);
                setStepButtons(true,true,true);
                previousPage = 0;
            }
            else {
                if (pagename.contains("pageConnectedReader")) {  //  3
					ui.clbBack->setEnabled(false);  // ** was true
					dct.setobjectToUpdate(this);
					dct.start();
                }
                else {
                    if (pagename.contains("pageVieweIDData")) {  // 5

                        setStepButtons(false,false,true);

						ui.lblTextSummary_4->setVisible(false);
						ui.lblIconSummary_3->setVisible(false);

						ui.clbOpenReport->setVisible(true);
						mQLOpenReport.setVisible(true);

                        ui.leName->setText(msglbl_name);
						ui.leName->setAccessibleName(msglbl_name);
                        ui.leFirstname->setText(msglbl_firstname);
						ui.leFirstname->setAccessibleName(msglbl_firstname);
                        ui.lePlaceOfBirth->setText(msglbl_placeOfBirth);
						ui.lePlaceOfBirth->setAccessibleName(msglbl_placeOfBirth);
                        ui.leDateOfBirth->setText(msglbl_BirthDate);
						ui.leDateOfBirth->setAccessibleName(msglbl_BirthDate);
                        ui.leNationalNumber->setText(msglbl_natNumber);
						ui.leNationalNumber->setAccessibleName(msglbl_natNumber);
                        // NN niet tonen
                        ui.leNationalNumber->setHidden(true);

                        ui.lblPhoto->setPixmap(QPixmap(""));

                        ui.clbCancel->setEnabled(false);
                        ui.clbCancel->update();
                        ui.clbCancel->repaint();
                        qApp->setOverrideCursor(Qt::WaitCursor);

						ui.clbBack->setEnabled(false);


                        // start de eid readcard thread
                        rct.setobjectToUpdate(this);
                        rct.inputparameters = "<InputParams><method>PCSC</method><readerName>" + selectedReader + "</readerName><fileName>ALL</fileName></InputParams>";
                        rct.start();
                        previousPage = 3;

                    }
                    else {
                        if (pagename.contains("pageeIDinserted")) { // 4

                            ui.clbCancel->setEnabled(false);
                            ui.clbCancel->update();
                            ui.clbCancel->repaint();

                            //QApplication::processEvents();

                            // Rebuild readerlist.
                            string readersXml = scl.pcscEnumerateCardReaders("");
                            QDomNode resultNode = ezw.xmlToNode(readersXml);


                            QDomNode listItem = resultNode.namedItem("ExtraInfo").namedItem("List").namedItem("ListItem");
                            readers.clear();
                            while (!listItem.isNull()) {
                                string _readerName = ezw.TextFromNode(listItem);
                                readers.push_back(_readerName);
                                listItem = listItem.nextSibling();
                                //ui.lblConnectedReader->setText(ui.lblConnectedReader->text()+ QString(" ") + QString(_readerName.c_str()));
                            }

                            ui.lbleIDInserted->setVisible(false);
                            setStepButtons(false,true,false);
                            previousPage = 3;

                            // als we een TokenInfo file vinden is het wel een eID denk ik (een bankkaart heeft ook een ATR)
                            selectedReader = "";
                            for (ReaderIterator it = readers.begin(); it != readers.end(); ++it) {
                                string cardData = scl.readCard("<InputParams><method>PCSC</method><readerName>" + *it + "</readerName><fileName>TOKENINFO</fileName></InputParams>");
                                if (ezw.GetNamedItem(cardData,"QueriedResult") ==  "SUCCESS") {
                                    selectedReader = *it;
                                    break;
                                }
                            }
                            if (selectedReader != "") {
                                this->appendStringReport(msginfo_EID.toStdString(), QFont::Bold);
                                ui.lbleIDInserted->setVisible(true);
                                previousPage = ui.stackedWidget->currentIndex();
                                ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);

                            }
                            else {
                                this->appendStringReport(msginfo_NOEID.toStdString(), QFont::Bold);
                                buildSummaryPage("eIDNotFound");
                                ui.clbBack->setEnabled(false);
                                ui.clbCancel->setEnabled(true);

                                // wait a second or two before going to warningpage
                                delaythread.action = "goToPage"; delaythread.int1 = 6;delaythread.int2 = 2;delaythread.start();
                                //ui.stackedWidget->setCurrentIndex(6); // goto warningpage.
                            }

                        }
                        else {
                            if (pagename.contains("pageReport")) { // 7

                                ui.clbSaveAsPdf->setVisible(true);
                                mQLsaveAsPdf.setVisible(true);

								ui.clbBack->setEnabled(true);
                            }
                            else {
                                if (pagename.contains("pageSummary")) { // 6

                                    ui.clbClose->setVisible(true);
                                    ui.clbClose->setEnabled(true);
                                    mQLClose.setVisible(true);
                                    mQLClose.setEnabled(true);

									ui.clbOpenReport->setVisible(true);
									mQLOpenReport.setVisible(true);

									if(!mInstallSucceeded)
										ui.clbBack->setEnabled(false);
									if (mhave_readers)
										previousPage = 3;
									else
										previousPage = 2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

bool ezInstaller::showCardData(string theXml) {

    string cardData = theXml;

    if (ezw.GetNamedItem(cardData,"QueriedResult") ==  "SUCCESS") {

        ui.leName->setText(msglbl_name+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"LastName").c_str())));
		if (QAccessible::isActive())
		{
			ui.leName->setAccessibleName(msglbl_name+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"LastName").c_str())));
			ui.leName->setFocus();
			QAccessible::updateAccessibility(ui.leName,0,QAccessible::NameChanged);
		}
		ui.leFirstname->setText(msglbl_firstname+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"FirstName").c_str())));
		ui.leFirstname->setAccessibleName(msglbl_firstname+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"FirstName").c_str())));
		ui.lePlaceOfBirth->setText(msglbl_placeOfBirth+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"BirthPlace").c_str())));
        ui.lePlaceOfBirth->setAccessibleName(msglbl_placeOfBirth+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"BirthPlace").c_str())));
		ui.leDateOfBirth->setText(msglbl_BirthDate+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"BirthDate").c_str())));
        ui.leDateOfBirth->setAccessibleName(msglbl_BirthDate+QString(QString::fromUtf8(ezw.GetExtraInfoItem(cardData,"BirthDate").c_str())));
		ui.leNationalNumber->setText(msglbl_natNumber+QString(ezw.GetExtraInfoItem(cardData,"NationalNumber").c_str()));
        ui.leNationalNumber->setAccessibleName(msglbl_natNumber+QString(ezw.GetExtraInfoItem(cardData,"NationalNumber").c_str()));
		string strPhoto = ezw.GetExtraInfoItem(cardData,"Photo");
        QByteArray ba = QByteArray::fromHex(QByteArray(strPhoto.c_str()));
        QPixmap fotopixmap;
        if (fotopixmap.loadFromData(ba)) {
            ui.lblPhoto->setPixmap(fotopixmap);
			ui.lblPhoto->setAccessibleName(QString("foto"));
        }
        else {
            ui.lblPhoto->setText(QString(" "));
        }

		ui.lblTextSummary_4->setText(msgOk_successfullInstall);
		ui.lblTextSummary_4->setAccessibleName(msgOk_successfullInstallAcc);
        ui.lblIconSummary_3->setPixmap(QPixmap(":/images/groene_vink"));
		ui.lblTextSummary_4->setVisible(true);
		ui.lblIconSummary_3->setVisible(true);

        ui.clbClose->setVisible(true);
        ui.clbClose->setEnabled(true);
        mQLClose.setVisible(true);
        mQLClose.setEnabled(true);

		appendStringReport(tr("         Read data : SUCCESS").toStdString(),QFont::Normal);
		return true;
    }
    else {
        //ui.pbTestSignature->setEnabled(false);
		buildSummaryPage("eIDNotFound");
		ui.stackedWidget->setCurrentIndex(6);
		appendStringReport(tr("         Read data : FAILS").toStdString(),QFont::Normal);
   }

	return false;
}

#pragma region MESSAGES

void ezInstaller::setMessages(){

    msgOk_successfullInstall = tr("<html><body><b>De installatie is succesvol afgelopen.</b> <br/>Je kunt de eID en kaartlezer nu gebruiken.</body></html>");
	msgOk_successfullInstallAcc = tr("De installatie is succesvol afgelopen. Je kunt de eID en kaartlezer nu gebruiken.");
	
	//    msgOK_successfullSignature = tr("<html><body><b>De Quick Installer heeft de elektronische authenticatie succesvol getest.</b><br/>De authenticatie werkt naar behoren. Je kunt de elektronische handtekening nu gebruiken.</body></html>");

//    msgError_PinBlocked = tr("<html><body><b>De authenticatie is niet getest.</b><br/>U heeft een verkeerde PIN ingegeven of u heeft de PIN operatie afgebroken. <br/>Indien u problemen blijft hebben met het gebruik van uw PIN-code, neem dan contact op met de Servicedesk</body></html>");
//    msgError_installMWFailed = tr("<html><body><b>De Quick Installer heeft een probleem gevonden bij het installeren van de eID software.</b></body></html>");
 //   msgError_installMWFailedAcc = tr("De Quick Installer heeft een probleem gevonden bij het installeren van de eID software.");
    msgReferenceToReport = tr("Je kunt een rapport openen waar je een gedetailleerde omschrijving van het probleem terugvindt.");
	msgContactDataHelpdesk = tr("Open het rapport en contacteer de Servicedesk om het probleem op te lossen.");
	msgError_smartcard = tr("<html><body><b>De Quick Installer kan geen kaartlezer detecteren.</b> <br/>Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt, kun je de servicedesk contacteren.</body></html>");
	msgError_smartcardAcc = tr("De Quick Installer kan geen kaartlezer detecteren. Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt, kun je de servicedesk contacteren.");   
	msgError_eIDnotfound = tr("<html><body><b>De Quick Installer kan de identiteitskaart niet vinden.</b> <br/>Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.</body></html>");
	msgError_eIDnotfoundAcc = tr("De Quick Installer kan de identiteitskaart niet vinden. Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.");
//    msgError_eIDnotReadable= tr("<html><body><b>De Quick Installer heeft een probleem met het lezen van de identiteitskaart.</b> <br/>Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.</body></html>");
//	msgError_eIDnotReadableAcc= tr("De Quick Installer heeft een probleem met het lezen van de identiteitskaart. Keer terug naar de vorige pagina en probeer het nogmaals. Als dat niet werkt kun je de servicedesk contacteren.");
    msgError_diagnosticError = tr("<html><body><b>De Quick Installer heeft een probleem gevonden met de configuratie van uw computer, of het installeren van de middleware.</b> </body></html>");
	msgError_diagnosticErrorAcc = tr("De Quick Installer heeft een probleem gevonden met de configuratie van uw computer, of het installeren van de middleware.");

    msglbl_InstallSoft = tr("eID software installeren");
    msglbl_ConnectCard =  tr("Kaartlezer verbinden");
    msglbl_ViewData = tr("Gegevens bekijken");

    msglbl_showDetails = tr("Toon details");
    msglbl_hideDetails = tr("Verberg details");

    msglbl_name = tr("Naam:   ");
    msglbl_firstname = tr("Voornamen:   ");
    msglbl_placeOfBirth = tr("Geboorteplaats:   ");
    msglbl_BirthDate = tr("Geboortedatum:   ");
    msglbl_natNumber = tr("Nationaal nummer:   ");

    msglbl_ReadCard = tr("Kaart lezen");

    msglbl_ConnectReaders = tr("<html><body>De eID software is succesvol geïnstalleerd.<br>De Quick Installer zal vervolgens controleren of de kaartlezer correct functioneert. Verbind nu je kaartlezer(s) met de computer.</html></body>");
    msglbl_ConnectReadersAcc = tr("De eID software is succesvol geïnstalleerd. De Quick Installer zal vervolgens controleren of de kaartlezer correct functioneert. Verbind nu je kaartlezer met de computer.");

	msglbl_ReaderConnected = tr("<html><body>De kaartlezer is met de computer verbonden.<br>Quick installer zal nu de goede werking controleren. Steek de elektronische identiteitskaart in een kaartlezer.</body></html>");
	msglbl_ReaderConnectedAcc = tr("De kaartlezer is met de computer verbonden. Quick installer zal nu de goede werking controleren. Steek de elektronische identiteitskaart in een kaartlezer.");
	msglbl_ReadingCard = tr("<html><body><b>De kaartlezer werkt!</b><br>De Quick Installer leest nu de belangrijkste gegevens van je identiteitskaart:</body></html>");
	msglbl_ReadingCardAcc = tr("De kaartlezer werkt! De Quick Installer leest nu de belangrijkste gegevens van je identiteitskaart:");
	//    msglbl_WarningTestSig = tr("<html><body>Om nu te weten of je elektronische authenticatie werkt, klik je hieronder op <b>Authenticatie testen</b>. De Quick Installer zal je vervolgens vragen om je pincode in te geven.</body></html>");
    msglbl_EidInserted = tr("<html><body>De Quick Installer heeft de elektronische identiteitskaart gevonden en is nu klaar om de gegevens te lezen.</body></html>");
	msglbl_EidInsertedAcc = tr("De Quick Installer heeft de elektronische identiteitskaart gevonden en is nu klaar om de gegevens te lezen.");

//    msgconf_title = tr("<html><body>Authenticatie testen</body></html>");
//    msgconf_Warning = tr("<html><body><b>Let op : </b>als je de verkeerde pincode ingeeft, kun je de elektronische identiteitskaart blokkeren.</body></html>");
//    msgconf_OnlyTest = tr("<html><body>Het authenticatie process wordt enkel getest, er wordt niets opgeslagen of gebruikt voor andere doeleinden.</body></html>");
//    msgconf_AreYouSure = tr("<html><body><b>Ben je zeker dat je de elektronische authenticatie wil testen ?</b></body></html>");
//    msgconf_btnTest = tr("Testen");
//    msgconf_btnNoTest = tr("Niet testen");

    msgbox_MW35 = tr("<html><body>Een andere versie van de eID Middleware 3.5 is reeds geïnstalleerd.<br><br>Om deze versie te kunnen installeren moet u eerst de andere versie verwijderen.</body></html>");
//    msgBox_SigNOK = tr("De test met de authenticatie is niet geslaagd.");
//    msgBox_SigCancelled = tr("De test met de handtekening is geannuleerd door de gebruiker.");

//    msgBox_SigOK = tr("De test met de authenticatie is geslaagd.");
//    msgbox_SigTitle = tr("Authenticatie testen");
    msginfo_NOEID = tr("Geen eID kaart gedetecteerd.");
    msginfo_EID = tr("eID kaart correct gedetecteerd.");
    msginfo_NoReaders = tr("Probleem: geen smartcardlezers aangetroffen.");

    msgbox_closeTitle = tr("Afsluiten ?");
    msgbox_closeTitle2talig = tr("Afsluiten ? / Fermer ?");

    msgbox_closeBody = tr("U wilt de eID Quick Installer afsluiten ?");
    msgbox_closeBody2talig = tr("U wilt de eID Quick Installer afsluiten? / Vous voulez fermer eID Quick Installer ?");

	msgbox_closeReboot = tr("<html><body>Het is aangeraden om de computer te herstarten.<br/><br/>Wilt u nu herstarten?</body></html>");

    msgbox_retryTitle = tr("Probeer opnieuw");
	msgbox_retryReader = tr("<html><body>Er werd geen kaartlezer aangetroffen.<br/><br/>Probeer opnieuw door:<ul><li>de kaartlezer los te koppelen</li><li>de kaartlezer terug te verbinden</li><li>op \"Ok\" te klikken</li></ul><br/><br/>Suggesties:<ul><li>probeer eventueel een andere USB-poort</li><li>vermijd het gebruik van een USB hub</li></ul></body></html>");
	msgbox_closeApplicationsTitle = tr("eID Middleware in gebruik!");
    msgbox_closeApplicationsBody = tr("Minstens één applicatie gebruikt de eID middleware. Sluit de applicatie af vooraleer verder te gaan.");
    msgbox_closeApplicationsBody2 = tr("Indien u de applicatie niet kan sluiten, klik dan op \"Afsluiten\", \nherstart uw systeem en probeer opnieuw.");

    msgbox_yes = tr("&Ja");
    msgbox_no = tr("&Nee");
    msgbox_ok = tr("&Ok");
    msgbox_cancel = tr("&Annuleren");
    msgbox_retry = tr("&Opnieuw");
    msgbox_close = tr("&Afsluiten");

    msgbox_yes2talig = tr("Ja / Oui");
    msgbox_no2talig = tr("Nee / Non");


    dt.eindeDiagWithError = tr("De installatie van de software is niet geslaagd.");
    dt.eindeDiagWithoutError = tr("De installatie van de software is geslaagd.");


}

#pragma endregion MESSAGES



void ezInstaller::initImages(void) {

    // palette voor labels.
    QBrush brushWhite(QColor(255, 255, 255, 255));
    QPalette palLabelsWhite;
    palLabelsWhite.setBrush(QPalette::Active, QPalette::WindowText, brushWhite);

    QBrush brushBlack(QColor(0, 0, 0, 255));
    QPalette palLabelsBlack;
    palLabelsBlack.setBrush(QPalette::Active, QPalette::WindowText, brushBlack);

    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QPixmap(":/images/fondinstaller"));
    this->setPalette(palette);

    ui.clbBack->setIcon(QIcon(":/images/terugknop"));ui.clbBack->setStyleSheet(transparentstyle);
    ui.clbCancel->setIcon(QIcon(":/images/xknop")); ui.clbCancel->setStyleSheet(transparentstyle);
    ui.clbNederlands->setIcon(QIcon(":/images/installeer_nl")); ui.clbNederlands->setStyleSheet(transparentstyle);
    ui.clbFrancais->setIcon(QIcon(":/images/installeer_fr")); ui.clbFrancais->setStyleSheet(transparentstyle);
    ui.lblPicUnconnectedReader->setPixmap(QPixmap(":/images/reader_disconnected"));
    ui.lblPicCardReaderConnected->setPixmap(QPixmap(":/images/reader_connected"));
    ui.lblPicCardReaderCardInserted->setPixmap(QPixmap(":/images/reader_and_eid"));
    ui.lblPicEID->setPixmap(QPixmap(":/images/eidSymbol"));
    ui.lblPicEidBackground->setPixmap(QPixmap(":/images/eidBackGround"));
    ui.clbViewDetails->setIcon(QIcon(":/images/knop_breed"));ui.clbViewDetails->setStyleSheet(transparentstyle);
    ui.clbOpenReport->setIcon(QIcon(":/images/knop_breed"));ui.clbOpenReport->setStyleSheet(transparentstyle);
    ui.clbSaveAsPdf->setIcon(QIcon(":/images/knop_breed"));ui.clbSaveAsPdf->setStyleSheet(transparentstyle);
    ui.clbClose->setIcon(QIcon(":/images/annuleren")); ui.clbClose->setStyleSheet(transparentstyle);mQLClose.setVisible(false);

    mQLClose.setIcons(QString(":/images/annuleren"),QString(":/images/annuleren_down"));
    mQLClose.setunderlyingButton(ui.clbClose);
    mQLClose.setPalette(palLabelsWhite);
    ui.clbClose->setVisible(false);
    mQLClose.setVisible(false);

    mQLShowDetail.setIcons(QString(":/images/knop_breed"),QString(":/images/knop_breed_down"));
    mQLShowDetail.setunderlyingButton(ui.clbViewDetails);
    mQLShowDetail.setPalette(palLabelsWhite);

    mQLChooseNederlands.setIcons(QString(":/images/installeer_nl"),QString(":/images/installeer_nl_down"));
    mQLChooseNederlands.setunderlyingButton(ui.clbNederlands);
    mQLChooseNederlands.setPalette(palLabelsBlack);

    mQLChooseFrancais.setIcons(QString(":/images/installeer_fr"),QString(":/images/installeer_fr_down"));
    mQLChooseFrancais.setunderlyingButton(ui.clbFrancais);
    mQLChooseFrancais.setPalette(palLabelsBlack);

    mQLOpenReport.setIcons(QString(":/images/knop_breed"),QString(":/images/knop_breed_down"));
    mQLOpenReport.setunderlyingButton(ui.clbOpenReport);
    mQLOpenReport.setPalette(palLabelsWhite);

    mQLsaveAsPdf.setIcons(QString(":/images/knop_breed"),QString(":/images/knop_breed_down"));
    mQLsaveAsPdf.setunderlyingButton(ui.clbSaveAsPdf);
    mQLsaveAsPdf.setPalette(palLabelsWhite);
    ui.clbSaveAsPdf->setVisible(false);
    mQLsaveAsPdf.setVisible(false);

    ui.lblRemoveReader->setText("<html><body>Alvorens te starten met de installatie, zorg ervoor dat de kaartlezer NIET verbonden is met de computer.<br>Avant de commencer l'installation, assurez-vous que le lecteur de cartes N'EST PAS connect&eacute; &agrave; l'ordinateur.</html></body>");
	ui.lblRemoveReader->setAccessibleName("Alvorens te starten met de installatie, zorg ervoor dat de kaartlezer NIET verbonden is met de computer. Avant de commencer l'installation, assurez-vous que le lecteur de cartes N'EST PAS connecté à l'ordinateur.");
	QBrush bgreen(QColor(50, 75, 50, 255));
	QPalette pal = ui.lblRemoveReader->palette();
	pal.setBrush(QPalette::Active, QPalette::WindowText, bgreen);
	ui.lblRemoveReader->setPalette(pal);

    //These properties we'll set at runtime, so we can see the widgets at designtime.
    ui.leName->setFrame(false);
    ui.leFirstname->setFrame(false);
    ui.leDateOfBirth->setFrame(false);
    ui.lePlaceOfBirth->setFrame(false);
    ui.leNationalNumber->setFrame(false);
    ui.lblPhoto->setFrameShape(QFrame::NoFrame);

    setStepButtons(true,true,true);
}

void ezInstaller::setStepButtons(bool ldown, bool mdown, bool rdown){

    if (ldown && mdown && rdown) {
        ui.lblInstallSoft->setText(tr(""));
		ui.lblInstallSoft->setAccessibleName(tr(""));
        ui.lblConnectReader->setText(tr(""));
		ui.lblConnectReader->setAccessibleName(tr(""));
        ui.lblViewData->setText("");
		ui.lblViewData->setAccessibleName("");
        ui.lblPicL->setPixmap(QPixmap(""));
        ui.lblPicM->setPixmap(QPixmap(""));
        ui.lblPicR->setPixmap(QPixmap(""));
        ui.lblPicEidBackground->setPixmap(QPixmap(":/images/eidBackGround"));
    }
    else {
        ui.lblPicEidBackground->setPixmap(QPixmap(""));
        QPalette pal;

        QBrush bwit(QColor(255, 255, 255, 255));

        QBrush bdonker(QColor(0, 0, 0, 255));

        if (ldown) {
            ui.lblPicL->setPixmap(QPixmap(":/images/tabL_down"));
            pal = ui.lblInstallSoft->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bwit);
            ui.lblInstallSoft->setPalette(pal);

        }
        else {
            ui.lblPicL->setPixmap(QPixmap(":/images/tabL"));
            pal = ui.lblInstallSoft->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bdonker);
            ui.lblInstallSoft->setPalette(pal);
        }

        if (mdown) {
            ui.lblPicM->setPixmap(QPixmap(":/images/tabM_down"));
            pal = ui.lblConnectReader->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bwit);
            ui.lblConnectReader->setPalette(pal);

        }
        else {
            ui.lblPicM->setPixmap(QPixmap(":/images/tabM"));
            pal = ui.lblConnectReader->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bdonker);
            ui.lblConnectReader->setPalette(pal);
        }

        if (rdown) {
            ui.lblPicR->setPixmap(QPixmap(":/images/tabR_down"));
            pal = ui.lblViewData->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bwit);
            ui.lblViewData->setPalette(pal);
        }
        else {
            ui.lblPicR->setPixmap(QPixmap(":/images/tabR"));
            pal = ui.lblViewData->palette();
            pal.setBrush(QPalette::Active, QPalette::WindowText, bdonker);
            ui.lblViewData->setPalette(pal);
        }

        ui.lblInstallSoft->setText(msglbl_InstallSoft);
		ui.lblInstallSoft->setAccessibleName(msglbl_InstallSoft);
        ui.lblConnectReader->setText(msglbl_ConnectCard);
		ui.lblConnectReader->setAccessibleName(msglbl_ConnectCard);
        ui.lblViewData->setText(msglbl_ViewData);
		ui.lblViewData->setAccessibleName(msglbl_ViewData);

        ui.lblTitleInstallSoftware_3->setText(msglbl_ReadCard);
		ui.lblTitleInstallSoftware_3->setAccessibleName(msglbl_ReadCard);

    }

}

bool ezInstaller::mCheckMW35(void) {


    bool cantproceed = false;

    string Result;

    // MW 3.0
    Result = scl.isSoftwareInstalled(string("<InputParams><productName>{82493A8F-7125-4EAD-8B6D-E9EA889ECD6A}</productName></InputParams>"));
    if (Result.find("<QueriedResult>YES</QueriedResult>") != std::string::npos) {
        cantproceed = true;
    }
    // MW 3.5 pro
    //Result = scl.isSoftwareInstalled(string("<InputParams><productName>{4C2FBD23-962C-450A-A578-7556BC79B8B2}</productName></InputParams>"));
    //if (Result.find("<QueriedResult>YES</QueriedResult>") != std::string::npos) {
    //    cantproceed = true;
    //}
    // MW 3.5 basic
    //Result = scl.isSoftwareInstalled(string("<InputParams><productName>{40420E84-2E4C-46B2-942C-F1249E40FDCB}</productName></InputParams>"));
    //if (Result.find("<QueriedResult>YES</QueriedResult>") != std::string::npos) {
    //    cantproceed = true;
    //}
    if (cantproceed) {
		if(QAccessible::isActive())
		{
			accessibleDialogbox::showDialogBox(QString("Quick-Installer"),msgbox_MW35,"OK",NULL,this);
		} else {
			QMessageBox::critical(this, "Quick-Installer", msgbox_MW35, QMessageBox::Ok);	
		}        
        this->close();
    }
    return cantproceed;

}



void ezInstaller::on_clbNederlands_clicked() {


    currentLanguage = "Nl";
    if (mCheckMW35())
        return;

    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);
}

void ezInstaller::on_clbNederlands_pressed() {
    ui.clbNederlands->setIcon(QIcon(":/images/installeer_nl_down"));
}

void ezInstaller::on_clbNederlands_released() {
    ui.clbNederlands->setIcon(QIcon(":/images/installeer_nl"));
}



void ezInstaller::on_clbFrancais_clicked(){

    currentLanguage = "Fr";

    QTranslator translator;
    translator.load(QString(":/TranslationFiles/transFrans"));
    qApp->installTranslator(&translator);
    ui.retranslateUi(this);

    setMessages();
    setStepButtons(true, true, true);

    if (mCheckMW35())
    return;

    mQLShowDetail.resetLabelText();
    mQLChooseNederlands.resetLabelText();
    mQLChooseFrancais.resetLabelText();

    mQLOpenReport.resetLabelText();
    mQLsaveAsPdf.resetLabelText();
    mQLClose.resetLabelText();

    ui.lblPicUnconnectedReader->setPixmap(QPixmap(":/images/reader_disconnected"));
    ui.lblPicCardReaderConnected->setPixmap(QPixmap(":/images/reader_connected"));
    ui.lblPicCardReaderCardInserted->setPixmap(QPixmap(":/images/reader_and_eid"));
    ui.lblPicEID->setPixmap(QPixmap(":/images/eidSymbol"));
    ui.lblPicEidBackground->setPixmap(QPixmap(":/images/eidBackGround"));

    mQLShowDetail.setText(msglbl_showDetails);
	mQLShowDetail.setAccessibleName(msglbl_showDetails);


    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);
}

void ezInstaller::on_clbFrancais_pressed() {
    ui.clbFrancais->setIcon(QIcon(":/images/installeer_fr_down"));
}

void ezInstaller::on_clbFrancais_released() {
    ui.clbFrancais->setIcon(QIcon(":/images/installeer_fr"));
}



void ezInstaller::on_textEdit_textChanged() {
    ui.teProgress->repaint();
}

void ezInstaller::on_lineEdit_textChanged(const QString &) {
    ui.leProgress->repaint();
}

void ezInstaller::on_clbPicBack_clicked() {
    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()-1);
}

void ezInstaller::on_btnCancel_clicked() {
    this->close();
}

#ifdef _TEST_DEBUG_
void ezInstaller::dummy(void) {
    nbrOfSteps = 21;
    qApp->setOverrideCursor(Qt::WaitCursor);

    /*	  ezWorker ezw;

    CServiceControl sc("ADSSv27");
    ServiceInfo info =   sc.GetServiceInfo();

    ui.textEdit->setText(QString(info.Pathname.c_str()) + "\n"+
    QString(info.UserAccount.c_str()) + "\n\t"+
    QString(ezw.inttostr(info.StartType).c_str()));

    ui.textEdit->insertPlainText(QString("Kiekeboe\n"));
    appendString("dit is een stringske",QFont::Bold);
    appendString("dit is een stringske",QFont::Normal);
    appendString("dit is een stringske\n",QFont::Bold);

    string s;

    CEikFiles eik;

    ReaderList& rl = eik.Readers();
    for (ReaderIterator it = rl.begin(); it != rl.end(); ++it) {

    string reader = *it;
    insertString("Found Reader : "+reader, QFont::Bold);
    if (eik.Connect(reader)) {
    if (eik.ReadID()) {
    insertString(" : Card found with nn "+eik.IDNN()+"\n",QFont::Normal);
    }
    }
    else {
    insertString(" : No card found\n",QFont::Normal);
    }
    }


    */
    string sysinfo = "";
    sysinfo = scl.getSystemInfo("");

    string taal = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osDefaultLanguage");
    insertString("taal: "+  taal,QFont::Normal);setPercentage(1);


    insertString("GetSystemInfo: "+  sysinfo,QFont::Normal);setPercentage(1);
    insertString("usbmainhubavailable : "+  scl.usbMainHubAvailable("<InputParams><HUB_CLASSGUID>{36FC9E60-C465-11CF-8056-444553540000}</HUB_CLASSGUID></InputParams>"),QFont::Normal);setPercentage(2);
    insertString("PCSCContext loaded : "+  scl.pcscContextIsAvailable(""),QFont::Normal);setPercentage(3);
    insertString("pcscEnumerateCardReaders : "+  scl.pcscEnumerateCardReaders(""),QFont::Normal);setPercentage(4);
    insertString("smartCardReaderInstallFilesFound : "+  scl.smartCardReaderInstallFilesFound("<InputParams><SCARDCLASSGUID>{50DD5230-BA8A-11D1-BF5D-0000F805F530}</SCARDCLASSGUID><SCARDCLASSNAME>SmartCardReader</SCARDCLASSNAME><HWID>PCMCIA\\GEMPLUS-GPR400-72B1</HWID><VID>PCMCIA\\GEMPLUS-GPR400-43E8</VID><DEVDESC>GPR400.DeviceDesc</DEVDESC><MFG>GEMPLUS</MFG></InputParams>"),QFont::Normal);setPercentage(5);
    insertString("canSeeSmartCardReaderAsUsbDevice : "+  scl.canSeeSmartCardReaderAsUsbDevice("<InputParams><SCARDCLASSGUID>{50DD5230-BA8A-11D1-BF5D-0000F805F530}</SCARDCLASSGUID><SCARDCLASSNAME>SmartCardReader</SCARDCLASSNAME><HWID>PCMCIA\\Gemplus-SerialPort-0D37</HWID><VID>PCMCIA\\GEMPLUS-GPR400-43E8</VID><DEVDESC>GPR400.DeviceDesc</DEVDESC><MFG>GEMPLUS</MFG></InputParams>"),QFont::Normal);setPercentage(6);
    insertString("delete : "+  scl.deleteFile("<InputParams><pathToFile>c:\\hhh.exe</pathToFile></InputParams>"),QFont::Normal);setPercentage(7);
    insertString("deletefolder : "+  scl.deleteFolder("<InputParams><pathToFolder>C:\\temp\\RockWerchterDonderdag2</pathToFolder></InputParams>"),QFont::Normal);setPercentage(7);
    insertString("VersionInfo : "+  scl.versionInfo("<InputParams><pathToFile>C:\\Development\\Delphi7\\CeviKsz\\CeviKsz.dll</pathToFile></InputParams>"),QFont::Normal);setPercentage(8);
    //insertString("startProcess : "+  scl.startProcess("<InputParams><commandLine>msiexec /i  c:\\temp\\PTeidMW35.msi XSIGN=1 /qb</commandLine><waitForTermination>YES</waitForTermination></InputParams>"),QFont::Normal);setPercentage(9);
    insertString("findRunningProcess : "+  scl.findRunningProcess("<InputParams><processName>Casis*</processName></InputParams>"),QFont::Normal);setPercentage(9);
    insertString("killProcess : "+  scl.killProcess("<InputParams><processName>cmd*</processName><processID></processID></InputParams>"),QFont::Normal);setPercentage(10);
    insertString("isServiceRunning : "+  scl.isServiceRunning("<InputParams><serviceName>CasisFolderWatcher</serviceName><processID></processID></InputParams>"),QFont::Normal);setPercentage(11);
    insertString("stopService : "+  scl.stopService("<InputParams><serviceName>CasisFolderWatcher</serviceName><processID></processID></InputParams>"),QFont::Normal);setPercentage(12);
    insertString("startService : "+  scl.startService("<InputParams><serviceName>CasisFolderWatcher</serviceName><processID></processID></InputParams>"),QFont::Normal);setPercentage(13);
    insertString("regread : "+  scl.readRegistryEntry("<InputParams><rootName>HKEY_LOCAL_MACHINE</rootName><path>SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup</path><keyName>Installation Sources</keyName></InputParams>"),QFont::Normal);setPercentage(14);
    insertString("deletekey : "+  scl.deleteRegistryEntry("<InputParams><rootName>HKEY_USERS</rootName><path>.DEFAULT\\Software\\Cevi\\CasisClient</path><keyName>toDelete</keyName></InputParams>"),QFont::Normal);setPercentage(15);
    insertString("getDevicesList : "+  scl.getdevicesList(""),QFont::Normal);setPercentage(16);
    insertString("isDevicePresent : "+  scl.isDevicePresent("<InputParams><GUID>{50DD5230-BA8A-11D1-BF5D-0000F805F530}</GUID><hardwareID>USB\\Vid_04e6&amp;Pid_200*</hardwareID></InputParams>"),QFont::Normal);setPercentage(17);
    //  insertString("InstallSTR391 : "+  scl.installDevice("<InputParams><hardwareID>USB\\VID_04E6&amp;PID_200D</hardwareID><pathToDriverFiles>C:\\temp\\STR391</pathToDriverFiles><infFile>STR391.inf</infFile></InputParams>"),QFont::Normal);setPercentage(18);
    insertString("getSoftwareList : "+  scl.getSoftwareList(""),QFont::Normal);setPercentage(19);
    insertString("isSoftwareInstalled : "+  scl.isSoftwareInstalled("<InputParams><productName>{824563DE-75AD-4166-9DC0-B6482F2DED5A}</productName></InputParams>"),QFont::Normal);setPercentage(20);
    insertString("readCard : "+  scl.readCard("<InputParams><method>PCSC</method><readerName>SCM Microsystems STR391 0</readerName><fileName>ALL</fileName></InputParams>"),QFont::Normal);setPercentage(21);

    qApp->restoreOverrideCursor();


}
#endif

bool ezInstaller::mInstallReaders() {

    ui.clbCancel->setEnabled(false);
    ui.clbCancel->update();
    ui.clbCancel->repaint();

	mhave_readers = false;

	setStepButtons(false,true,false);
	ui.lblPicCardReaderConnected->setText("");

    ui.clbBack->setEnabled(true);

    ui.lblConnectedReader->setVisible(false);

#ifdef WIN32
	string myOS = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osProductName");
#endif WIN32

    // try to update all drivers found in the scenario
    vector<string>::iterator it;
    for (it = driverParameters.begin(); it != driverParameters.end(); ++it) {
        string params = *it;
#ifdef WIN32
		if( myOS != "Windows 7")
			scl.installDevice(params,myOS);
#endif WIN32
    }
    Sleep(2000);

#ifdef WIN32
if( myOS == "Windows 7")
	{
		// Wait for maximum 30 seconds for Windows device installers to complete
		scl.IsDeviceInstallInprogress(30000);
	}
#endif WIN32

	bool bContinue=true;
	do
	{
#ifdef __APPLE__
		qApp->setOverrideCursor(Qt::WaitCursor);
		//We had to check if ACR38 (non-CCID) dirver is plug in
		{
			QString currdirpath = QCoreApplication::applicationDirPath();
			std::string commandLine = "";
			commandLine += currdirpath.toStdString();
			commandLine += "/../Resources/preparePcscd.sh";
			if (!CSysDiagnost::doAsAdmin(commandLine, true))
			{
				qApp->restoreOverrideCursor();
				return false;
			}
		}
#endif

		// now we try to detect the pcscreaders.

		QDomNode Params;
		string readersXml = scl.pcscEnumerateCardReaders("");
		QDomNode resultNode = ezw.xmlToNode(readersXml);
LOGSTR(readersXml.c_str());

		QDomNode listItem = resultNode.namedItem("ExtraInfo").namedItem("List").namedItem("ListItem");
		readers.clear();
		this->appendStringReport("Detecting smartcard readers:", QFont::Bold);
		while (!listItem.isNull()) {
			string _readerName = ezw.TextFromNode(listItem);
			readers.push_back(_readerName);
			listItem = listItem.nextSibling();
			this->appendStringReport(_readerName.c_str(), QFont::Normal);

			//ui.lblConnectedReader->setText(ui.lblConnectedReader->text()+ QString(" ") + QString(_readerName.c_str()));
		}
#ifdef WIN32
		// als we een acr 38 tegenkomen moeten we ook de sis dll uit de qresources installeren
//		if (_readerName.find("ACR38U") != std::string::npos)
//		{
//			string sispath = this->dt.substituteResVars(SISPATH);
//			QDir sisdir(QString(sispath.c_str()));
//			if (sisdir.mkpath(QString(sispath.c_str()))) {
//				string filename = sisdir.absoluteFilePath(SISLIB).toStdString();
//				QFile sisdll_res(SISRES);
//				if (! sisdll_res.open(QIODevice::ReadOnly))
//					continue;
//				QByteArray qsis = sisdll_res.readAll();
//				sisdll_res.close();
//				QFile sisdll(filename.c_str());
//				if (! sisdll.open(QIODevice::Truncate | QIODevice::WriteOnly))
//					continue;
//				sisdll.write(qsis);
//				sisdll.close();
//				this->appendStringReport(" -> SIS/ACR38U: installed " + filename, QFont::Normal);
//			}
//		}
#endif

  		if(readers.size() > 0)
		{
			qApp->restoreOverrideCursor();
			bContinue=false;
		}
		else
		{
			QString _title = msgbox_retryTitle;
			QString _body = msgbox_retryReader;
			QString _okButton = msgbox_ok;
			QString _cancelButton = msgbox_cancel;
			int respons = 0;

			qApp->restoreOverrideCursor();

			if(QAccessible::isActive())
			{
				if (0 == accessibleDialogbox::showDialogBox(_title,_body,_okButton,_cancelButton,this))
				{
					bContinue=false;
				}
			} else {
				if (0 != QMessageBox::question(this,_title,_body,_okButton,_cancelButton,QString::null,0,1))
				{
					bContinue=false;
				}
			}  
		}
	} while(bContinue);

	if (readers.size() > 0) {
        ui.clbCancel->setEnabled(true);
        ui.lblConnectedReader->setVisible(true);
        ui.lblConnectedReader->setText(msglbl_ReaderConnected);
		ui.lblConnectedReader->setAccessibleName(msglbl_ReaderConnectedAcc);

        mhave_readers = true;
        return true;
    } else {
        buildSummaryPage("noReaderFound");
        this->appendStringReport(msginfo_NoReaders.toStdString(), QFont::Bold);
        previousPage = 2;
        // wait a second or two before going to warningpage
        delaythread.action = "goToPage"; delaythread.int1 = 6;delaythread.int2 = 0;delaythread.start();
        ui.clbCancel->setEnabled(true);

        return false;
    }
}

void ezInstaller::on_pushButton_3_clicked(){
    //confirmDialogBox c;
    //if (c.exec() == QDialog::Accepted) {
    //    scl.startProcess("<InputParams><commandLine>C:\\Program Files\\Portugal Identity Card\\signwithCSP.exe</commandLine><waitForTermination>YES</waitForTermination></InputParams>");
    //    //std::cout << "aanvaard";
    //}
    //else {
    //    //std::cout << "geweigerd";
    //}
    //scl.AuthSign("");
}

void ezInstaller::on_commandLinkButton_pressed()
{
    ui.clbBack->setStyleSheet(transparentstyle);
    ui.clbBack->setIcon(QIcon(":/images/terugknop_down"));
    ui.clbBack->repaint();
}

void ezInstaller::on_commandLinkButton_released()
{
    ui.clbBack->setStyleSheet(transparentstyle);
    ui.clbBack->setIcon(QIcon(":/images/terugknop"));
    ui.clbBack->repaint();
}

void ezInstaller::on_commandLinkButton_clicked()
{
    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()-1);
}

void ezInstaller::on_clbCancel_clicked()
{

    QString _title;
    QString _body;
    QString _yesButton;
    QString _noButton;

    if (ui.stackedWidget->currentIndex() == 0) {
        _title = msgbox_closeTitle2talig;
        _body = msgbox_closeBody2talig;
        _yesButton = msgbox_yes2talig;
        _noButton = msgbox_no2talig;

    }
    else {
        _title = msgbox_closeTitle;
        _body = msgbox_closeBody;
        _yesButton = msgbox_yes;
        _noButton = msgbox_no;
    }

	//int answer = QMessageBox::question(this,_title,_body,_yesButton,_noButton,QString::null,QMessageBox::Yes,QMessageBox::No);
	if(QAccessible::isActive())
	{
		//returnvalues: accepted = 1, rejected = 0
		if (0 != accessibleDialogbox::showDialogBox(_title,_body,_yesButton,_noButton,this))
		{
			this->close();
		}
	} else 
	{
		if (0 == QMessageBox::question(this,_title,_body,_yesButton,_noButton,QString::null,QMessageBox::Yes,QMessageBox::No))
		{
			this->close();
		}  	
	}
}

void ezInstaller::on_clbCancel_pressed()
{
    ui.clbCancel->setIcon(QIcon(":/images/xknop_down"));
}

void ezInstaller::on_clbCancel_released()
{
    ui.clbCancel->setIcon(QIcon(":/images/xknop"));

}
/*
#pragma region NEXTBUTTON

void ezInstaller::on_clbNext_pressed()
{
    ui.clbNext->setIcon(QIcon(":/images/annuleren_down"));
}

void ezInstaller::on_clbNext_released()
{
    ui.clbNext->setIcon(QIcon(":/images/annuleren"));
}

void ezInstaller::on_clbNext_clicked()
{


    if (ui.stackedWidget->currentIndex() == 2) { // about to connect readers
        previousPage = 2;
        if (this->mInstallReaders()) {
            ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);
        } else {
            ui.stackedWidget->setCurrentIndex(6); // summary
        }
        return;
    }

    // If we're on the diagnostic page, check if there were fatal errors to see if
    // Next should go to warningpage.

    QString pagename;
    pagename = ui.stackedWidget->currentWidget()->objectName();
    if (pagename.contains("pageProgress")) {
        if (dt.fatalErrorOccurred) {
            buildSummaryPage("diagnosticError");
            previousPage = ui.stackedWidget->currentIndex();
            ui.stackedWidget->setCurrentIndex(6); // goto warningpage.
        }
        else {
            previousPage = ui.stackedWidget->currentIndex();
            ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);

        }
    }
    else {
        if (ui.stackedWidget->currentWidget()->objectName() == "pageVieweIDData") {
            buildSummaryPage("endPage");
        }
        previousPage = ui.stackedWidget->currentIndex();
        ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);

    }
}

#pragma endregion NEXTBUTTON
*/
#pragma region BACKBUTTON

void ezInstaller::on_clbBack_clicked()
{


    if (previousPage != ui.stackedWidget->currentIndex()) {
        ui.stackedWidget->setCurrentIndex(previousPage);
    }
    else {
        ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()-1);
    }
    if (ui.stackedWidget->currentIndex() == 1)
        ui.clbBack->setEnabled(false);
}

void ezInstaller::on_clbBack_pressed()
{
    ui.clbBack->setIcon(QIcon(":/images/terugknop_down"));
}

void ezInstaller::on_clbBack_released()
{
    ui.clbBack->setIcon(QIcon(":/images/terugknop"));
}

#pragma endregion BACKBUTTON



void ezInstaller::on_lblNext_linkActivated(QString)
{
    //ui.stackedWidget->setCurrentIndex(ui.stackedWidget->currentIndex()+1);
}

void ezInstaller::on_lblNext_linkHovered(QString)
{

}

#pragma region DETAIL_VIEW

void ezInstaller::on_clbViewDetails_clicked() {
    if (!ui.teProgress->isVisible()) {
        mQLShowDetail.setText(msglbl_hideDetails);
		mQLShowDetail.setAccessibleName(msglbl_hideDetails);
        ui.teProgress->setVisible(true);
    }
    else {
        mQLShowDetail.setText(msglbl_showDetails);
		mQLShowDetail.setAccessibleName(msglbl_showDetails);
        ui.teProgress->setVisible(false);
    }
}
void ezInstaller::on_clbViewDetails_pressed() {
    ui.clbViewDetails->setIcon(QIcon(":/images/knop_breed_down"));
}
void ezInstaller::on_clbViewDetails_released() {
    ui.clbViewDetails->setIcon(QIcon(":/images/knop_breed"));
}

#pragma endregion DETAIL_VIEW


#pragma region SUMMARYPAGE

void ezInstaller::buildSummaryPage(string pageType) {
    if (pageType == "eIDNotFound") {
        ui.lblTextSummary_1->setText(msgError_eIDnotfound);
		ui.lblTextSummary_1->setAccessibleName(msgError_eIDnotfoundAcc);
        ui.lblTextSummary_2->setText(msgReferenceToReport);
		ui.lblTextSummary_2->setAccessibleName(msgReferenceToReport);
        ui.lblTextSummary_3->setText(msgContactDataHelpdesk);
		ui.lblTextSummary_3->setAccessibleName(msgContactDataHelpdesk);

        ui.lblIconSummary_1->setPixmap(QPixmap(":/images/foutmelding"));
        ui.lblIconSummary_2->setPixmap(QPixmap(""));
    }
    else
        if (pageType == "noReaderFound") {
            ui.lblTextSummary_1->setText(msgError_smartcard);
			ui.lblTextSummary_1->setAccessibleName(msgError_smartcardAcc);
            ui.lblTextSummary_2->setText(msgReferenceToReport);
			ui.lblTextSummary_2->setAccessibleName(msgReferenceToReport);
            ui.lblTextSummary_3->setText(msgContactDataHelpdesk);
			ui.lblTextSummary_3->setAccessibleName(msgContactDataHelpdesk);

            ui.lblIconSummary_1->setPixmap(QPixmap(":/images/foutmelding"));
            ui.lblIconSummary_2->setPixmap(QPixmap(""));
        }
        else
            if (pageType == "endPage") {
                // alles ok.
                ui.lblTextSummary_1->setText(msgOk_successfullInstall);
				ui.lblTextSummary_1->setAccessibleName(msgOk_successfullInstallAcc);
                ui.lblIconSummary_1->setPixmap(QPixmap(":/images/groene_vink"));
                ui.lblTextSummary_3->setText("");
				ui.lblTextSummary_3->setAccessibleName("");
            }
            else
                if (pageType == "diagnosticError") {
                    ui.lblTextSummary_1->setText(msgError_diagnosticError);
					ui.lblTextSummary_1->setAccessibleName(msgError_diagnosticErrorAcc);
                    ui.lblTextSummary_2->setText(msgReferenceToReport);
					ui.lblTextSummary_2->setAccessibleName(msgReferenceToReport);
                    ui.lblTextSummary_3->setText(msgContactDataHelpdesk);
					ui.lblTextSummary_3->setAccessibleName(msgContactDataHelpdesk);

                    ui.lblIconSummary_1->setPixmap(QPixmap("images/foutmelding"));
                    ui.lblIconSummary_2->setPixmap(QPixmap(""));
                }

}

#pragma endregion SUMMARYPAGE

#pragma region REPORT

void ezInstaller::on_clbOpenReport_pressed() {
    ui.clbOpenReport->setIcon(QIcon(":/images/knop_breed_down"));
}

void ezInstaller::on_clbOpenReport_released() {
    ui.clbOpenReport->setIcon(QIcon(":/images/knop_breed"));
}

void ezInstaller::on_clbOpenReport_clicked()
{
    previousPage = ui.stackedWidget->currentIndex();
    ui.stackedWidget->setCurrentIndex(7);

    ui.clbClose->setEnabled(true);
    ui.clbClose->setVisible(true);
    mQLClose.setEnabled(true);
    mQLClose.setVisible(true);

}

void ezInstaller::on_clbSaveAsPdf_pressed()
{
    ui.clbSaveAsPdf->setIcon(QIcon(":/images/knop_breed_down"));
}

void ezInstaller::on_clbSaveAsPdf_released()
{
    ui.clbSaveAsPdf->setIcon(QIcon(":/images/knop_breed"));
}

void ezInstaller::on_clbSaveAsPdf_clicked()
{
    ui.clbSaveAsPdf->setEnabled(false);

    string _desktopFolder = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osDesktopFolder");
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString pdfFolder = QFileDialog::getExistingDirectory(this,tr("Kies een map om het rapport in op te slaan."),QString(_desktopFolder.c_str()),options);
    if (!pdfFolder.isNull()) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(pdfFolder+QString("/eID-QuickInstaller.report.pdf"));
        ui.teReport->document()->print(&printer);

    }
    ui.clbSaveAsPdf->setEnabled(true);

    ui.clbClose->setEnabled(true);
    ui.clbClose->setVisible(true);
    mQLClose.setEnabled(true);
    mQLClose.setVisible(true);


}

#pragma endregion REPORT

#pragma region CLOSE

void ezInstaller::on_clbClose_clicked() {

#ifdef WIN32
bool bReboot=scl.isRebootNeeded();
if(bReboot)
{
	QString _title = msgbox_closeTitle;
	QString _body = msgbox_closeReboot;
	QString _yesButton = msgbox_yes;
	QString _noButton = msgbox_no;

	if(QAccessible::isActive())
	{
		//returnvalues: accepted = 1, rejected = 0
		if (0 != accessibleDialogbox::showDialogBox(_title,_body,_yesButton,_noButton,this))
		{
			scl.reboot();
		}  	
		else
		{
			bReboot=false;
		}
	} else 
	{	//returnvalues : _yesButton=0
		if(0==QMessageBox::question(this,_title,_body,_yesButton,_noButton,QString::null,0,1))
		{
			scl.reboot();
		}  	
		else
		{
			bReboot=false;
		}
	}

}

/*
	if(!bReboot)
	{
		// start pteidgui, do not wait, return immediately
		string paramstring = dt.substituteResVars("<InputParams><commandLine>%PTEID_INSTALL_DIRNAME%\\pteid35gui.exe</commandLine><waitForTermination>0</waitForTermination></InputParams>");
		scl.startProcess(paramstring);
	}
*/
#endif

#ifndef WIN32
	if(scl.isRebootNeeded())
	{
		QString _title = msgbox_closeTitle;
		QString _body = msgbox_closeReboot;
		QString _yesButton = msgbox_yes;
		QString _noButton = msgbox_no;
		bool bReboot;

		if(QAccessible::isActive())
		{
			//returnvalues: accepted = 1, rejected = 0
			if (0 != accessibleDialogbox::showDialogBox(_title,_body,_yesButton,_noButton,this))
			{
				bReboot=true;
			}  	
			else
			{
				bReboot=false;
			}
		} else 
		{	//returnvalues : _yesButton=0
			if(0==QMessageBox::question(this,_title,_body,_yesButton,_noButton,QString::null,0,1))
			{
				bReboot=true;
			}  	
			else
			{
				bReboot=false;
			}
		}

		if(bReboot==true)
		{
			LOGSTR("Restarting")
			QString currdirpath = QCoreApplication::applicationDirPath();
			std::string commandLine = "";
			commandLine += currdirpath.toStdString();
			commandLine += "/../Resources/reboot.sh";
			CSysDiagnost::doAsAdmin(commandLine, true);
		}
	}
LOGSTR("Before close")
#endif

    this->close();

}
void ezInstaller::on_clbClose_pressed() {
    ui.clbClose->setIcon(QIcon(":/images/annuleren_down"));
}

void ezInstaller::on_clbClose_released() {
    ui.clbClose->setIcon(QIcon(":/images/annuleren"));
}

#pragma endregion CLOSE






