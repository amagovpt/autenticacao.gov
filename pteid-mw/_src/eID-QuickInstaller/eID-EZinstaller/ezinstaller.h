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
#ifndef EZINTALLER_H
#define EZINTALLER_H

#include <QtGui/QDialog>
#include <QDomDocument>
#include <QDomNode>
#include <QCursor>
#include "qdom.h"
#include "systemClass.h"
#include "ezWorker.h"
#include <QFile>
#include <QDirIterator>
#include <QDir>
#include "myQLabel.h"
#include <QTranslator>
#include <QAccessible>

#include "ui_ezinstaller.h"
#include "diagnoseThread.h"

#include "Platform.h"
#ifdef WIN32
#include "diaglib.h"
#endif


typedef vector<string> ReaderList;
typedef ReaderList::iterator ReaderIterator;

class ezInstaller : public QDialog
{
	Q_OBJECT

public:
	//ezInstaller(QWidget *parent = 0, Qt::WFlags flags = Qt::FramelessWindowHint);
	ezInstaller(QWidget *parent = 0, Qt::WFlags flags = Qt::CustomizeWindowHint);
	~ezInstaller();
	systemClass scl;
	ezWorker ezw;
	int nbrOfSteps;
	string selectedReader;
	void setPercentage(int step);
	void appendString(string str, int weight);
	void insertString(string str, int weight);
	void appendStringReport(string str, int weight);
	void insertStringReport(string str, int weight);
	void dummy(void);
    void initImages(void);
    void customEvent(QEvent * qe );
#ifdef __APPLE__
	// Holds the credentials for the 'doAsAdmin' function.
	static AuthorizationRef authRef;   
#endif
	static string currentLanguage;
	
private:
	
	myQLabel mQLShowDetail;
	myQLabel mQLChooseNederlands;
	myQLabel mQLChooseFrancais;
	myQLabel mQLOpenReport;
	myQLabel mQLsaveAsPdf;
	myQLabel mQLClose;

	Ui::ezInstallerClass ui;
	string thisOS;
    QDomDocument statusList;
    QDomDocument xml_in;
    QDomDocument resultVars;
    string internal_inputxml;
    string inputXmlSource;
    string alternativeInputXml;
    string CategoryWorstTestResult;
    string GroupWorstTestResult;
	diagnoseThread dt;
	readCardThread rct;
	detectReaderThread drt;
	detectCardThread dct;
	delayThread delaythread;
	ReaderList readers;
	QString transparentstyle;
	int previousPage;

    vector<string> driverParameters;

	void setStepButtons(bool l, bool m, bool r);
    bool showCardData(string theXml);

    void mdoAuthSign();
    bool mInstallReaders();
    bool mCheckMW35(void);

    bool mdiagthread_active;
    bool mhave_readers;
    bool mSignatureOK;
	bool mInstallSucceeded;

	QString msgOk_successfullInstall;
	QString msgOk_successfullInstallAcc;
	//QString msgOK_successfullSignature;
	//QString msgError_PinBlocked;
	//QString msgError_installMWFailed;
	//QString msgError_installMWFailedAcc;
	QString msgReferenceToReport;
	QString msgContactDataHelpdesk;
	QString msgError_smartcard;
	QString msgError_smartcardAcc;
	QString msgError_eIDnotfound;
	QString msgError_eIDnotfoundAcc;
	//QString msgError_eIDnotReadable;
	//QString msgError_eIDnotReadableAcc;
	QString msgError_diagnosticError;
	QString msgError_diagnosticErrorAcc;

	QString msglbl_InstallSoft;
	QString msglbl_ConnectCard;
    QString msglbl_ReadCard;
	QString msglbl_ViewData;
	QString msglbl_showDetails;
	QString msglbl_hideDetails;
	QString msglbl_name;
	QString msglbl_firstname;
	QString msglbl_placeOfBirth;
	QString msglbl_BirthDate;
	QString msglbl_natNumber;

    QString msglbl_ConnectReaders;
	QString msglbl_ConnectReadersAcc;
    QString msglbl_ReaderConnected;
	QString msglbl_ReaderConnectedAcc;
    QString msglbl_ReadingCard;
	QString msglbl_ReadingCardAcc;
    QString msglbl_WarningTestSig;
    QString msglbl_EidInserted;
	QString msglbl_EidInsertedAcc;

	QString msgconf_title;
	QString msgconf_Warning;
	QString msgconf_OnlyTest;
	QString msgconf_AreYouSure;
	QString msgconf_btnTest;
	QString msgconf_btnNoTest;

    QString msgbox_MW35;
    QString msgBox_SigNOK;
    QString msgBox_SigCancelled;
    QString msgBox_SigOK;
    QString msgbox_SigTitle;
    QString msginfo_NOEID;
    QString msginfo_EID;
    QString msginfo_NoReaders;
    QString msgbox_closeTitle;
    QString msgbox_closeBody;
    QString msgbox_closeReboot;
    QString msgbox_retryTitle;
    QString msgbox_retryReader;
    QString msgbox_closeTitle2talig;
    QString msgbox_closeBody2talig;
    QString msgbox_closeApplicationsTitle;
    QString msgbox_closeApplicationsBody;
    QString msgbox_closeApplicationsBody2;
	QString msgbox_yes;
    QString msgbox_no;
    QString msgbox_ok;
    QString msgbox_cancel;
    QString msgbox_retry;
    QString msgbox_close;
    QString msgbox_yes2talig;
    QString msgbox_no2talig;



	void setMessages();
	void buildSummaryPage(string pageType);

	



private slots:
    void on_clbClose_released();
    void on_clbClose_pressed();
    void on_clbOpenReport_released();
    void on_clbOpenReport_pressed();
    void on_clbSaveAsPdf_released();
    void on_clbSaveAsPdf_pressed();
	void on_clbClose_clicked();
	void on_clbSaveAsPdf_clicked();
	void on_clbOpenReport_clicked();
	void on_clbBack_released();
	void on_clbBack_pressed();
	void on_clbViewDetails_released();
	void on_clbViewDetails_pressed();
	void on_clbViewDetails_clicked();
	void on_lblNext_linkHovered(QString);
	void on_lblNext_linkActivated(QString);
	void on_clbBack_clicked();
	void on_clbFrancais_released();
	void on_clbFrancais_pressed();
	void on_clbNederlands_released();
	void on_clbNederlands_pressed();
	void on_clbCancel_released();
	void on_clbCancel_pressed();
	void on_clbCancel_clicked();
	void on_commandLinkButton_clicked();
	void on_commandLinkButton_released();
	void on_commandLinkButton_pressed();
	void on_pushButton_3_clicked();
	void on_btnCancel_clicked();
	void on_clbPicBack_clicked();
	void on_lineEdit_textChanged(const QString &);
	void on_textEdit_textChanged();
	void on_clbFrancais_clicked();
	void on_clbNederlands_clicked();
	void on_stackedWidget_currentChanged(int);
	void on_btnNext_clicked();
};

#endif // EZINTALLER_H
