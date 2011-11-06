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
#include <QThread>
#include <QEvent>
#include <string>
#include <QApplication>
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QDirIterator>
#include <QDir>
#include "ezWorker.h"
#include <QFont>
#include <QFileDialog>

#include "resource.h"
#include "systemClass.h"
#include "qpixmap.h"

#include "Platform.h"
#ifdef WIN32
#include "zrclib.h"
//#include "diaglib.h"
#define OS_ALL "WINALL"
#else
#define OS_ALL "MACALL"
#endif


using namespace std;

class verboseEvent : public QEvent {
private:

	QString _str1;
	QString _str2;
	QString _str3;

	int _int1;
	int _int2;
	int _int3;

	QString _txt;
	int _weight;
	QString _action;
public:
	verboseEvent() : QEvent(QEvent::User) {
		;
	}
	void setParams(QString Action, QString txt, int Weight) {
		_txt = txt;
		_weight = Weight;
		_action = Action;
	}
	void setParams(QString pAction, QString pstr1, QString pstr2, QString pstr3, int pint1, int pint2, int pint3) {
	    _action = pAction;
		_str1 = pstr1;
		_str2 = pstr2;
		_str3 = pstr3;
		_int1 = pint1;
		_int2 = pint2;
		_int3 = pint3;
	}

	QString getstr1() {return _str1;}
	QString getstr2() {return _str2;}
	QString getstr3() {return _str3;}

	int getint1() {return _int1;}
	int getint2() {return _int2;}
	int getint3() {return _int3;}

	QString getText() {return _txt;}
	QString getAction() {return _action;}
	int getWeight() {return _weight;}

};

class diagnoseThread : public QThread {

private:
	string currentLanguage;
    string currentLanguageForMsi;

    void appendString(string str, int weight);
    void insertString(string str, int weight);
    void appendStringReport(string str, int weight);
    void insertStringReport(string str, int weight);
	void setPercentage(int int1);
    void initProgressBar(int int1);
    void restoreCursor();
	void addDriverParams(string str1);
    void signalCompletion();
    void disableCancel();
    void enableCancel();
	void translateOsString(string* myOS);

public:

	ezWorker ezw;
	int nbrOfSteps;
	string selectedReader;
	bool fatalErrorOccurred;


	QString eindeDiagWithError;
	QString eindeDiagWithoutError;

    string thisOS;
	QDomDocument statusList;
	QDomDocument xml_in;
	QDomDocument resultVars;
	string internal_inputxml;
	string inputXmlSource;
	string alternativeInputXml;
	string CategoryWorstTestResult;
	string GroupWorstTestResult;

	systemClass scl;


	void setCurrentLanguage(string _language) {

	   currentLanguage = _language;
	   currentLanguageForMsi = QString(currentLanguage.c_str()).toLower().toStdString();
   	   
	}

	QObject * objectToUpdate;
	void setobjectToUpdate(QObject * qo ) {
		objectToUpdate = qo;
	}
	virtual void run();
	void UpdateWorstTestValue(const string Result, const string testLevel);
	void runScenario();
	bool TestOnThisOS(const QDomNode OSNode);
	bool TestDependenciesOk(const QDomNode Params);
	QDomNode ValidateResult( QDomNode ResultNode, QDomNode RequiredValue, string Accuracy, string TestID, string SeverityWhenFailure);
	QDomNode InvestigateElement( const QString ElementName , const QDomNode Params );
	//bool doStartProcess(const QString theParams);
	string substituteResVars(const string s);
	void cacheInfoItems(QDomNode ResultNode, QDomNode dn);
	void extractSCDrivers(QString OS);
    void extractISSFiles(QString param);
	void extractMiddleWare();
	void extractPteidMW64();
	void verboseExtraInfoItems(string theXml);
	void buildReport(string theXml);
};


class readCardThread : public QThread {
public:
	systemClass scl;
	QObject * objectToUpdate;
	string inputparameters;
	void setobjectToUpdate(QObject * qo ) {
		objectToUpdate = qo;
	}

	virtual void run();
};

class delayThread : public QThread {
public:
	systemClass scl;
	QObject * objectToUpdate;
	string action;
	int delayTime;
	string str2;
	string str3;
	int int1;
	int int2;
	int int3;
	void setobjectToUpdate(QObject * qo ) {
		objectToUpdate = qo;
	}

	virtual void run();
};

class detectReaderThread : public QThread {
private:
	systemClass scl;
	ezWorker ezw;
	QObject * objectToUpdate;
public:
	void setobjectToUpdate(QObject * qo ) {
		objectToUpdate = qo;
	}

	virtual void run();
};

class detectCardThread : public QThread {
private:
	systemClass scl;
	ezWorker ezw;
	QObject * objectToUpdate;
public:
	void setobjectToUpdate(QObject * qo ) {
		objectToUpdate = qo;
	}

	virtual void run();
};
