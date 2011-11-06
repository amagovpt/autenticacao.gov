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
#include "diagnoseThread.h"
#ifdef WIN32
#include <windows.h>
#endif

#define LOGSTR(msg) 
#define LOGINT(format, i) 

void diagnoseThread::appendString(string str, int weight) {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("append",QString(str.c_str()),weight);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
} 
void diagnoseThread::insertString(string str, int weight) {

    verboseEvent * ve = new verboseEvent();
    ve->setParams("insert",QString(str.c_str()),weight);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
} 

void diagnoseThread::appendStringReport(string str, int weight) {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("appendReport",QString(str.c_str()),weight);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
} 
void diagnoseThread::insertStringReport(string str, int weight) {

    verboseEvent * ve = new verboseEvent();
    ve->setParams("insertReport",QString(str.c_str()),weight);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
} 
void diagnoseThread::translateOsString(string* myOS) {
    if ( *myOS == "Windows XP") {*myOS = "XP";}
    else if ( *myOS == "Windows Vista") {*myOS = "VISTA";}
    else if ( *myOS == "Windows 2000") {*myOS = "WIN2000";}
	else if ( *myOS == "Windows 7") {*myOS = "WIN7";}
}

void diagnoseThread::setPercentage(int int1) {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("setPercentage","","","",int1,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}
void diagnoseThread::initProgressBar(int int1) {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("initProgressBar","","","",int1,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}
void diagnoseThread::restoreCursor() {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("restoreCursor","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}
void diagnoseThread::addDriverParams(string str1) {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("addDriverParams",QString(str1.c_str()),"","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}

void diagnoseThread::signalCompletion() {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("done","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}


void diagnoseThread::disableCancel() {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("disableCancel","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}
void diagnoseThread::enableCancel() {
    verboseEvent * ve = new verboseEvent();
    ve->setParams("enableCancel","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}


void diagnoseThread::run()
{
    runScenario();
}

void diagnoseThread::UpdateWorstTestValue(const string Result, const string testLevel) {

    int CurrentLevel = 0;
    int itestLevel = 0;
    string sb, st;

    if (Result == "INFO") {itestLevel = 1;}
    else if (Result == "WARNING") {itestLevel = 2;}
    else if (Result == "ERROR") {itestLevel = 3;}



    if (testLevel == "CATEGORY") {
        if (CategoryWorstTestResult == "INFO") {CurrentLevel = 1;}
        else if (CategoryWorstTestResult == "WARNING") {CurrentLevel = 2;}
        else if ((CategoryWorstTestResult == "ERROR")||(CategoryWorstTestResult == "FATAL_ERROR")) {CurrentLevel = 3;};

        if (itestLevel > CurrentLevel) {CurrentLevel = itestLevel;};

        if (CurrentLevel == 0) {CategoryWorstTestResult = "SUCCESS";}
        else if (CurrentLevel == 1) {CategoryWorstTestResult = "INFO";}
        else if (CurrentLevel == 2) {CategoryWorstTestResult = "WARNING";}
        else if (CurrentLevel == 3) {CategoryWorstTestResult = "ERROR";};
        sb = GroupWorstTestResult;
        st = CategoryWorstTestResult + "/" + Result;

    }
    else {
        if (GroupWorstTestResult == "INFO") {CurrentLevel = 1;}
        else if (GroupWorstTestResult == "WARNING") {CurrentLevel = 2;}
        else if (GroupWorstTestResult == "ERROR") {CurrentLevel = 3;};

        if (itestLevel > CurrentLevel) {CurrentLevel = itestLevel;};

        if (CurrentLevel == 0) {GroupWorstTestResult = "SUCCESS";}
        else if (CurrentLevel == 1) {GroupWorstTestResult = "INFO";}
        else if (CurrentLevel == 2) {GroupWorstTestResult = "WARNING";}
        else if (CurrentLevel == 3) {GroupWorstTestResult = "ERROR";};
        sb = GroupWorstTestResult+ "/" + Result;
        st = CategoryWorstTestResult ;
    }
}
void diagnoseThread::runScenario() {

#pragma region Declare_variables
    fatalErrorOccurred = false;
    int numberOfNodes = 0;
    QDomElement docElem, e2, e_cat, e_property;
    QDomNode dn_cat, dn_group, dn_property, resultNode, resvarsNode;
    QDomNode paramnode;
    QDomText resvarstext;
    QDomAttr a, cacheAttr;
    string cacheAttrTxt;
    string propertyName;
    string externalfile = "scenario.xml";

    int nodecountcat, nodecountgroup;

    QDomDocument nodecounts;
    QDomElement root;


#pragma endregion Declare_variables

#ifdef WIN32
	this->thisOS = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osProductName");
	translateOsString(&(this->thisOS));
#endif

    statusList.insertBefore(statusList.createProcessingInstruction(QString("xml"),QString("version='1.0' encoding='UTF-8'")),statusList.firstChild());
    root = statusList.createElement("statusList");
    statusList.appendChild(root);

    resultVars.insertBefore(resultVars.createProcessingInstruction(QString("xml"),QString("version='1.0' encoding='UTF-8'")),resultVars.firstChild());
    root = resultVars.createElement("ResultVars");
    resultVars.appendChild(root);

    //  QDomDocument nodecounts;
    nodecounts.insertBefore(statusList.createProcessingInstruction(QString("xml"),QString("version='1.0' encoding='ISO-8859-1'")),nodecounts.firstChild());
    root = nodecounts.createElement("nodeCounts");
    nodecounts.appendChild(root);

    //	currentLanguage = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osDefaultLanguage");

    nodecounts.insertBefore(statusList.createProcessingInstruction(QString("xml"),QString("version='1.0' encoding='ISO-8859-1'")),nodecounts.firstChild());
    root = nodecounts.createElement("nodeCounts");
    nodecounts.appendChild(root);



#pragma region selectScenario_xml

    // Read in the input-xml into the QDomDocument
    // Select the file scenario.xml if it exists, otherwise use the embedded
    // xml-file.

    //externalfile = "C:\\Documents and Settings\\bca\\Mijn documenten\\Visual Studio 2005\\Projects\\eid-EZinstall\\debug\\scenario.xml";
    
    externalfile = "./scenario.xml";
    //QFile file(QString(externalfile.c_str()));
    
    const char* scenarioResrc = ":/scenario/theScenario";
    QFile file(scenarioResrc);
    if (!file.open(QIODevice::ReadOnly)) {
        
                          if (!xml_in.setContent(QString::fromUtf8(internal_inputxml.c_str()))) {
            return ;
        } else {
            appendStringReport(string("Scenario: embedded"), QFont::Normal);
            inputXmlSource = "Embedded";
        };
    } else {
        if (!xml_in.setContent(&file)) {
            appendStringReport(string("Scenario: ") + string(scenarioResrc), QFont::Normal);
            file.close();
            return ;
        } else {
            inputXmlSource = "External : " + externalfile;
        };
    };
    file.close();
#pragma endregion 

    //qApp->setOverrideCursor(Qt::WaitCursor);
    docElem = xml_in.documentElement();

#pragma region Calculate number of steps per category and group
    // Count the number of ChildNodes to set te totalsteps of the progressbar.

    dn_cat = docElem.firstChild();
    while (!dn_cat.isNull()) {
        nodecountcat = 0;
        e_cat = dn_cat.toElement();
        if (TestOnThisOS(dn_cat.namedItem("CheckOnOS"))) {
            if (!e_cat.isNull()) {
                dn_group = dn_cat.firstChild();
                while (!dn_group.isNull()) {
                    nodecountgroup = 0;
                    if (dn_group.toElement().childNodes().count() > 1) {
                        if (TestOnThisOS(dn_group.namedItem("CheckOnOS"))) {
                            dn_property = dn_group.firstChild();
                            while (!dn_property.isNull()) {
                                if (TestOnThisOS(dn_property.namedItem("CheckOnOS"))) {
                                    numberOfNodes++;
                                    nodecountcat++;
                                    nodecountgroup++;
                                }
                                dn_property = dn_property.nextSibling();
                            }
                        }
                    }
                    nodecounts.namedItem("nodeCounts").appendChild(ezw.CreateTextNode(ezw.AttribTextFromNode(dn_group,"TestID"),ezw.inttostr(nodecountgroup)));
                    dn_group = dn_group.nextSibling();
                }
            }
        }
        nodecounts.namedItem("nodeCounts").appendChild(ezw.CreateTextNode(ezw.AttribTextFromNode(dn_cat,"TestID"),ezw.inttostr(nodecountcat)));
        dn_cat = dn_cat.nextSibling();
    }
    nbrOfSteps = numberOfNodes;

    initProgressBar(nbrOfSteps);
#pragma endregion 


    dn_cat = docElem.firstChild();
    while (!dn_cat.isNull())
        // Catogorien overlopen
    {
        e_cat = dn_cat.toElement();
        if (e_cat.tagName() == QString("Category")) {
            if (TestOnThisOS(dn_cat.namedItem("CheckOnOS"))) {
                CategoryWorstTestResult = "SUCCESS";

                paramnode = dn_cat.namedItem(QString("InputParams"));
                if (!TestDependenciesOk(paramnode)) {
                    // hier de progresscount ophogen
                    if (ezw.TextFromNode(nodecounts.namedItem("nodeCounts").namedItem(ezw.AttribTextFromNode(dn_cat,"TestID").c_str()) ) != "") {
                        int val = atoi(ezw.TextFromNode(nodecounts.namedItem("nodeCounts").namedItem(ezw.AttribTextFromNode(dn_cat,"TestID").c_str())).c_str());
                        setPercentage(val);
                    }
                } else {
                    appendString( ezw.TextFromNode(dn_cat.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Bold);
                    if (!e_cat.isNull()) {
                        dn_group = dn_cat.firstChild();
                        while (!dn_group.isNull()) {
                            if (dn_group.toElement().childNodes().count() > 1) {
                                if (TestOnThisOS(dn_group.namedItem("CheckOnOS"))) {
                                    GroupWorstTestResult = "SUCCESS";
                                    paramnode = dn_group.namedItem(QString("InputParams"));
                                    if (!TestDependenciesOk(paramnode)) {
                                        // hier de progresscount ophogen
                                        if (ezw.TextFromNode(nodecounts.namedItem(QString("nodeCounts")).namedItem(ezw.AttribTextFromNode(dn_group,"TestID").c_str())) != "") {
                                            int val = atoi(ezw.TextFromNode(nodecounts.namedItem(QString("nodeCounts")).namedItem(ezw.AttribTextFromNode(dn_group,"TestID").c_str())).c_str());
                                            setPercentage( val);
                                        }
                                    } else {
                                        appendString("   " + ezw.TextFromNode(dn_group.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Bold);
                                        dn_property = dn_group.firstChild();

                                        while (!dn_property.isNull()) {
                                            if (TestOnThisOS(dn_property.namedItem("CheckOnOS"))) {

                                                appendString("      " + ezw.TextFromNode(dn_property.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Normal);
                                                propertyName = ezw.AttribTextFromNode(dn_property, "CheckPropertyName");
                                                if (propertyName != "") {

                                                    cacheAttrTxt = ezw.AttribTextFromNode(dn_property.namedItem(QString("RequiredValue")), "CacheAs").c_str();

                                                    paramnode = dn_property.namedItem(QString("InputParams"));

                                                    if (!TestDependenciesOk(paramnode)) {
                                                        resultNode = xml_in.createElement("Result");
                                                        resultNode.appendChild(ezw.CreateTextNode("TestSuccessFull", "YES"));
                                                        resultNode.appendChild(ezw.CreateTextNode("Error", "This test doesn't have to be done because of the result of previous tests."));
                                                        resultNode.appendChild(xml_in.createElement("QueriedResult"));
                                                        resultNode.appendChild(xml_in.createElement("ExtraInfo"));
                                                        statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(ezw.AttribTextFromNode(dn_property, "TestID"), "SUCCESS"));
                                                    } else {
                                                        resultNode = InvestigateElement(QString(propertyName.c_str()), paramnode);
                                                        resultNode = ValidateResult(resultNode, dn_property.namedItem(QString("RequiredValue")), ezw.AttribTextFromNode(dn_property, "Accuracy"), ezw.AttribTextFromNode(dn_property, "TestID"), ezw.AttribTextFromNode(dn_property, "SeverityWhenFailure"));
                                                        insertString(" : "+ezw.TextFromNode(resultNode.namedItem("QueriedResult")),QFont::Bold);
                                                        if (ezw.TextFromNode(statusList.namedItem("statusList").namedItem(ezw.AttribTextFromNode(dn_property, "TestID").c_str())) == QString("SUCCESS").toStdString()) {
                                                            insertString(" : "+ezw.TextFromNode(resultNode.namedItem("Error")),QFont::Bold);
                                                            verboseExtraInfoItems(ezw.NodeAsString(resultNode));
                                                        }
                                                    }
                                                    UpdateWorstTestValue(ezw.TextFromNode(statusList.namedItem(QString("statusList")).namedItem(QString(ezw.AttribTextFromNode(dn_property, "TestID").c_str()))), "GROUP");
                                                    dn_property.appendChild(resultNode.cloneNode());
                                                    if (resultNode.hasChildNodes()) {
                                                        if (cacheAttrTxt != "") {
                                                            resultVars.namedItem("ResultVars").appendChild(ezw.CreateTextNode(cacheAttrTxt, ezw.TextFromNode(resultNode.namedItem("QueriedResult"))));
                                                        }
                                                    };

                                                    // hier CacheInfoItem afhandelen.
                                                    cacheInfoItems(resultNode,dn_property);
                                                }
                                                setPercentage( 1);
                                            }
                                            dn_property = dn_property.nextSibling();
                                        }
                                    }
                                    statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(ezw.AttribTextFromNode(dn_group, "TestID"), GroupWorstTestResult));
                                    UpdateWorstTestValue(ezw.TextFromNode(statusList.namedItem(QString("statusList")).namedItem(QString(ezw.AttribTextFromNode(dn_group, "TestID").c_str()))), "CATEGORY");
                                }
                            }


                            dn_group = dn_group.nextSibling();
                        }
                    }
                }
                statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(ezw.AttribTextFromNode(dn_cat, "TestID"), CategoryWorstTestResult));

            }
        } 
        //BuildOutputXml(fullWarningList);


        dn_cat = dn_cat.nextSibling();
    }
    if (!fatalErrorOccurred) {
        appendString(eindeDiagWithoutError.toStdString(),QFont::Bold);
    }
    else {
        appendString(eindeDiagWithError.toStdString(),QFont::Bold);
    }


    string theResultXml = xml_in.toString().toStdString();
    buildReport(theResultXml);


    signalCompletion();

    restoreCursor();

}

bool diagnoseThread::TestOnThisOS(const QDomNode OSNode){
    bool returnValue = false;
    QDomNode innerNode ;
    innerNode = OSNode.firstChild();
    while (!innerNode.isNull()) {

        if ((ezw.TextFromNode(innerNode) == OS_ALL) || (ezw.TextFromNode(innerNode) == this->thisOS)) {
            returnValue = true;
            break;
        };
        innerNode = innerNode.nextSibling();
    }

    return returnValue;
}

bool diagnoseThread::TestDependenciesOk(const QDomNode Params){
    QDomNode Depends;
    QDomNode dn;
    bool result = true;
    bool isSuccess;
    string strStatus ;
    Depends = Params.namedItem("TestDependencies");
    if (!Depends.isNull()){    // If there are no dependencies, they're ok, of course
        dn = Depends.firstChild();
        while (!dn.isNull()) {
            if (!dn.isComment()){
                strStatus = ezw.TextFromNode(statusList.namedItem("statusList").namedItem(QString(ezw.TextFromNode(dn).c_str())));
                //				MessageBox(0,strStatus.c_str(),ezw.TextFromNode(dn).c_str(),MB_OK);
                //				MessageBox(0,ezw.NodeAsString(statusList.documentElement()).c_str(),ezw.TextFromNode(dn).c_str(),MB_OK);
                if ( strStatus == "") { // TestID does not exist or has not been tested yet, so dep's are not ok
                    result = false;
                    break;
                }
                else {
                    if ( strStatus == "SUCCESS") {
                        isSuccess = true;
                    }
                    else {
                        isSuccess = false;
                    }

                    if (isSuccess) {
                        if (!(ezw.AttribTextFromNode(dn,"SuccessFlagMustBe") == "YES")) {                   
                            result = false;
                            break;
                        }
                    }
                    else {
                        if (ezw.AttribTextFromNode(dn,"SuccessFlagMustBe") == "YES") {                   
                            result = false;
                            break;
                        }
                    }
                }
            }
            dn = dn.nextSibling();
        }
    }
    return result;
}

QDomNode diagnoseThread::ValidateResult( QDomNode ResultNode, QDomNode RequiredValue, string Accuracy, string TestID, string SeverityWhenFailure) {
    QDomNode returnNode;
    string returnValue;
    string typeOfResult, strQueriedResult ;
    bool IsSuccess = false;
    string strRequiredValue ;

    strRequiredValue = ezw.TextFromNode(RequiredValue);
    bool InvertStatus = false;

    if (strRequiredValue[0] == '!' ){
        strRequiredValue = strRequiredValue.substr(1,strRequiredValue.length()-1);
        InvertStatus = true;
    }

    returnNode = ResultNode;

    typeOfResult = ezw.AttribTextFromNode(RequiredValue,"RequiredValueType");
    strQueriedResult = ezw.TextFromNode(ResultNode.namedItem("QueriedResult"));

    if (Accuracy == "IDONTCARE") {
        IsSuccess = true;
    }
    else if (Accuracy == "EXACTLY") {
        if (typeOfResult == "STRING") {
            if (strQueriedResult == strRequiredValue) {IsSuccess = true;}else {IsSuccess = false;}
        } else
            if (typeOfResult == "INTEGER") {
                if (atoi(strQueriedResult.c_str()) == atoi(strRequiredValue.c_str())) {IsSuccess = true;}else {IsSuccess = false;}
            }
    }
    else if (Accuracy == "EQUAL_OR_GREATER") {
        if (typeOfResult == "INTEGER") {
            if (atoi(strQueriedResult.c_str()) >= atoi(strRequiredValue.c_str())) {IsSuccess = true;}else {IsSuccess = false;}
        }
    }
    else if (Accuracy == "EQUAL_OR_LESSER") {
        if (typeOfResult == "INTEGER") {
            if (atoi(strQueriedResult.c_str()) <= atoi(strRequiredValue.c_str())) {IsSuccess = true;}else {IsSuccess = false;}
        }
    }
    else if (Accuracy == "GREATER_THAN") {
        if (typeOfResult == "INTEGER") {
            if (atoi(strQueriedResult.c_str()) > atoi(strRequiredValue.c_str())) {IsSuccess = true;}else {IsSuccess = false;}
        }
    }
    else if (Accuracy == "LESSER_THAN") {
        if (typeOfResult == "INTEGER") {
            if (atoi(strQueriedResult.c_str()) < atoi(strRequiredValue.c_str())) {IsSuccess = true;}else {IsSuccess = false;}
        }
    }
    else if (Accuracy == "CONTAINS") {
        if (typeOfResult == "STRING") {
            if ( ezw.WildCardFit(strRequiredValue.c_str(), strQueriedResult.c_str()) ) {IsSuccess = true;}else {IsSuccess = false;}
            //if (strQueriedResult.find(strRequiredValue.c_str(),0) != strQueriedResult.npos) {IsSuccess = true;}else {IsSuccess = false;}
        };

    }
    if (IsSuccess) {
        if (!InvertStatus) {
            returnNode.insertBefore(ezw.CreateTextNode("TestSuccessFull","YES"),returnNode.firstChild());
            statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(TestID,"SUCCESS"));
        }
        else {
            returnNode.insertBefore(ezw.CreateTextNode("TestSuccessFull","NO"),returnNode.firstChild());
            statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(TestID,SeverityWhenFailure));
            if (SeverityWhenFailure == "FATAL_ERROR") 
            {

                fatalErrorOccurred = true;
            }
        }
    }
    else {
        if (!InvertStatus) {
            returnNode.insertBefore(ezw.CreateTextNode("TestSuccessFull","NO"),returnNode.firstChild());
            statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(TestID,SeverityWhenFailure));
            if (SeverityWhenFailure == "FATAL_ERROR") {
                fatalErrorOccurred = true;
            }
        }
        else {
            returnNode.insertBefore(ezw.CreateTextNode("TestSuccessFull","YES"),returnNode.firstChild());
            statusList.namedItem("statusList").appendChild(ezw.CreateTextNode(TestID,"SUCCESS"));
        }
    };

    return returnNode;
}

QDomNode diagnoseThread::InvestigateElement( const QString ElementName , const QDomNode Params )
{

    string paramsAsString;
    paramsAsString = ezw.NodeAsString(Params);
    paramsAsString = substituteResVars(paramsAsString);
    string returnString;

    QDomNode returnNode;
    if (ElementName == QString("CHECKOSPROPERTY"))
        {returnString = scl.getSystemInfo(""); }
    else if (ElementName == QString("CHECKINSTALLEDSOFTWARE")) 
        {returnString = scl.isSoftwareInstalled(paramsAsString); }
#ifdef WIN32
    else if (ElementName == QString("WAITFORUNINSTALL"))
		{returnString = scl.WaitForUninstall(paramsAsString); }
#endif
    else if (ElementName == QString("ENUMERATE_INSTALLED_SOFTWARE")) 
        {returnString = scl.getSoftwareList(paramsAsString); }
    else if (ElementName == QString("STARTPROCESS")) {
        if (currentLanguage == "Fr") {
            paramsAsString = QString(paramsAsString.c_str()).replace("%msi_language%","TRANSFORMS=:fr.mst").toStdString();
        }
        else {
            paramsAsString = QString(paramsAsString.c_str()).replace("%msi_language%","TRANSFORMS=:nl.mst").toStdString();
        }
        try {
			disableCancel();
LOGSTR(paramsAsString.c_str())
            returnString = scl.startProcess(paramsAsString); /*verboseExtraInfoItems(returnString);*/
        } catch(...) {
            ;
        }
        enableCancel();
    }
    else if (ElementName == QString("KILLPROCESS")) 
        {returnString = scl.killProcess(paramsAsString); }
    else if (ElementName == QString("FINDPROCESS")) 
        {returnString = scl.findRunningProcess(paramsAsString); }

#ifdef WIN32
    else if (ElementName == QString("READREGISTRY")) 
        {returnString = scl.readRegistryEntry(paramsAsString); }
    else if (ElementName == QString("WRITEREGISTRY")) 
        {returnString = scl.writeRegistryEntry(paramsAsString); }
    else if (ElementName == QString("DELETEREGISTRY_ENTRY")) 
        {returnString = scl.deleteRegistryEntry(paramsAsString); }

    else if (ElementName == QString("ISSERVICERUNNING")) 
        {returnString = scl.isServiceRunning(paramsAsString); }
    else if (ElementName == QString("STOPSERVICE")) 
        {returnString = scl.stopService(paramsAsString); }
    else if (ElementName == QString("STARTSERVICE")) 
        {returnString = scl.startService(paramsAsString); }
    else if (ElementName == QString("REMOVESERVICE")) 
        {returnString = scl.removeService(paramsAsString); }
#endif


    else if (ElementName == QString("DELETEFILE")) 
        {returnString = scl.deleteFile(paramsAsString); /*verboseExtraInfoItems(returnString);*/ }
    else if (ElementName == QString("VERSIONINFO"))
        {returnString = scl.versionInfo(paramsAsString); }


    else if (ElementName == QString("EXTRACTSCDRIVERS")) 
        {extractSCDrivers(QString(paramsAsString.c_str())); }
    else if (ElementName == QString("EXTRACTISSFILES")) 
        {extractISSFiles(QString(paramsAsString.c_str())); }
    else if (ElementName == QString("EXTRACTMIDDLEWARE")) 
        {extractMiddleWare(); }

#ifdef WIN32
	else if (ElementName == QString("EXTRACTPTEIDMW64")) 
		{
		if(scl.is64bitOS())
			extractPteidMW64(); }
    else if (ElementName == QString("INSTALLDEVICE")) 
        {string myOS = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osProductName");
		returnString = scl.installDevice(paramsAsString,myOS);
		if( myOS != "Windows 7")
			addDriverParams(paramsAsString);
		}
    else if (ElementName == QString("ENUMERATEDEVICES")) 
        {returnString = scl.getdevicesList(paramsAsString); }
    else if (ElementName == QString("IS_DEVICE_PRESENT")) 
        {returnString = scl.isDevicePresent(paramsAsString); }
#endif

    else if (ElementName == QString("SC_INSTALLFILESFOUND")) 
        {returnString = scl.smartCardReaderInstallFilesFound(paramsAsString); }
    else if (ElementName == QString("ENUMERATEREADERS")) 
        {returnString = scl.pcscEnumerateCardReaders(paramsAsString); }

    else if (ElementName == QString("USBMAINHUB_AVAILABLE")) 
        {returnString = scl.usbMainHubAvailable(paramsAsString); }
    else if (ElementName == QString("PCSCCONTEXT_AVAILABLE")) 
        {returnString = scl.pcscContextIsAvailable(paramsAsString); }
    else if (ElementName == QString("FIND_SC_AS_USBDEVICE")) 
        {returnString = scl.canSeeSmartCardReaderAsUsbDevice(paramsAsString); }

    else if (ElementName == QString("READCARD")) 
        {returnString = scl.readCard(paramsAsString); }


    QDomDocument qdd;
    qdd.setContent(QString(returnString.c_str()));

    returnNode = qdd.documentElement().toElement();

    return returnNode;
}
/*
bool diagnoseThread::doStartProcess(const QString theParams) {
	scl.startProcess(theParams.toStdString());
    
    return true;
}
*/
string diagnoseThread::substituteResVars(const string s)
{
    string returnString = s;
    string vkey = "", vvalue;

    QDomElement e, docElem = resultVars.documentElement();
    QDomNode dn;
    dn = docElem.firstChild();

    while (!dn.isNull())
    {
        e = dn.toElement();
        if (!e.isNull())
        {
            vkey = e.tagName().toStdString();
            vvalue = e.text().toStdString();
            returnString = QString(returnString.c_str()).replace(vkey.c_str (),vvalue.c_str()).toStdString();
            if (s.find("%") == s.npos ) {break;};
        }
        dn = dn.nextSibling();
    }
    return returnString;
}

void diagnoseThread::cacheInfoItems(QDomNode ResultNode, QDomNode dn) {
    QDomNode ciiList = dn.namedItem(QString("cacheInfoItems"));

    if (!ciiList.isNull()) {
        QDomNode cii ;
        cii = ciiList.firstChild();
        while (!cii.isNull()) {
            string CacheInfoItemWithName = ezw.AttribTextFromNode(cii,"CacheInfoItemWithName");
            string CacheInfoItemAs = ezw.AttribTextFromNode(cii,"CacheInfoItemAs");

            string tmp = ezw.GetExtraInfoItem(ezw.NodeAsString(ResultNode),CacheInfoItemWithName);
            resultVars.namedItem("ResultVars").appendChild(ezw.CreateTextNode(CacheInfoItemAs, tmp));
            cii = cii.nextSibling();
        }
    }
}


QByteArray getFileBytes (QString filename) {
    QFile qf(filename);
    qf.open (QIODevice::ReadOnly);
    QByteArray ba;
    QDataStream in(&qf);
    ba = qf.readAll();
    qf.close();
    return ba;
}

void saveByteArrayToFile(QByteArray ba, QString filename) {
    QFile outfile(filename);
    if (outfile.open(QIODevice::WriteOnly) == TRUE)
	{
		QDataStream dsout(&outfile);
		dsout.writeRawData(ba.data(),ba.length());
	    outfile.close();
	}
}

void diagnoseThread::extractSCDrivers(QString inputXml) {

#ifdef WIN32


    QDomDocument theDoc;
    theDoc.setContent(inputXml);	
    QDomNode Params = theDoc.documentElement().toElement();
    string extractToFolder = ezw.GetNamedItem(Params,"extractToFolder");
    extractToFolder = substituteResVars(extractToFolder);

    QDir qd(QString(extractToFolder.c_str()));
    qd.mkpath(QString(extractToFolder.c_str()));

    string myOS = ezw.GetExtraInfoItem(scl.getSystemInfo(""),"osProductName");

    QString result;
    QString unzipFolder;

    QString FileName(substituteResVars("%osTempFolder%driversresource.zip").c_str());
    appendString("Saving reader drivers archive to " + FileName.toStdString()+".", QFont::Normal);

    WinRes::SaveBinaryResource("SmartCarddrivers", IDR_SMARTCARDDRIVERS1, FileName.toStdString());

    QString firstInfFolder(extractToFolder.c_str());

    QString infFolderList(substituteResVars("%INF_FOLDER%").c_str());

    if (!infFolderList.contains(QString(firstInfFolder))) {
        string newInfList = infFolderList.toStdString() + ";";
        newInfList += firstInfFolder.toStdString();
        scl.writeRegistryEntry("<InputParams><rootName>HKEY_LOCAL_MACHINE</rootName><path>SOFTWARE\\Microsoft\\Windows\\CurrentVersion</path><keyName>DevicePath</keyName><value>"+newInfList+"</value><keyType>REG_EXPAND_SZ</keyType></InputParams>");
    }

    //QString firstInfFolder(substituteResVars("%INF_FOLDER%").c_str());
    //firstInfFolder = firstInfFolder.mid(0,firstInfFolder.indexOf(';'));
    //firstInfFolder = firstInfFolder.replace(QString("%SystemRoot%"),QString(substituteResVars("%osWindowsFolder%").c_str()));

    unzipFolder = firstInfFolder;

    string tmpUnzipfile = unzipFolder.toStdString()+"\\tmpUnzipfile";


    // open weer maar voor input
    ZrcArchive zarch(FileName.toStdString());
    zarch.OpenArchive();
    ZrcFileHeader *hdr = NULL;

    // lees de file headers
    while ((hdr = zarch.ReadFileHeader()) != NULL) {

        string theOS = hdr->get_OS();
        if ( myOS == "Windows XP") {myOS = "XP";}
        else if ( myOS == "Windows Vista") {myOS = "VISTA";}
        else if ( myOS == "Windows 2000") {myOS = "WIN2000";}
		else if ( myOS == "Windows 7") {myOS = "WIN7";}


        char* data = zarch.ReadFileData(hdr->get_Filesize());
        if (myOS == theOS) {

            ofstream ofs;
            string xfn = hdr->get_Filename();


            ofs.open(tmpUnzipfile.c_str(), ios::binary);
            ofs.write(data, hdr->get_Filesize());
            ofs.close();

            QString extractToFileName = unzipFolder+QString("\\")+   QString(xfn.c_str());
            QString tmpFolder = extractToFileName.mid(0,extractToFileName.lastIndexOf("\\"));

            appendString("Extracting ("+theOS+", "+hdr->get_Reader()+") "+extractToFileName.toStdString()+".",QFont::Normal);

            QByteArray ba = qUncompress(getFileBytes(QString(tmpUnzipfile.c_str())));
            saveByteArrayToFile(ba,extractToFileName);


            // schoonmaak
        }
        delete[] data;
        // schoonmaak

    }
    delete hdr;

    // sluit archive
    zarch.CloseArchive();
    QFile::remove(QString(tmpUnzipfile.c_str()));
    QFile::remove(FileName);
    appendString("DONE",QFont::Bold);

    resultVars.namedItem("ResultVars").appendChild(ezw.CreateTextNode("%FIRST_INF_FOLDER%", unzipFolder.toStdString()));
    
#endif

}
void diagnoseThread::extractMiddleWare() {
    
#ifdef WIN32

    QString result;

    QString FileName(substituteResVars("%osTempFolder%PteidMW.msi").c_str());

    WinRes::SaveBinaryResource("Msi",IDR_MSI1,FileName.toStdString());
    
#endif

}

void diagnoseThread::extractPteidMW64() {
#ifdef WIN32
	QString FileName(substituteResVars("%osTempFolder%PteidMW64.msi").c_str());
	WinRes::SaveBinaryResource("Msi",IDR_MSI2,FileName.toStdString());
#endif
}

void diagnoseThread::extractISSFiles(QString param) {
    
#ifdef WIN32


    QString result;

    const int NISS_FILES = 6;
    std::string issfiles[NISS_FILES] = {
        "uninstall24.iss",
        "uninstall25.iss",
        "uninstall26.iss",
        "uninstall30.iss",
        "uninstall35.iss",
        "uninstall35Pro.iss",
    };
    int ResIDs[NISS_FILES] = {
        IDR_ISSFILE1,
        IDR_ISSFILE2,
        IDR_ISSFILE3,
        IDR_ISSFILE4,
        IDR_ISSFILE5,
        IDR_ISSFILE6,
    };

    for (int i = 0; i < NISS_FILES; ++i)  {
        QString FileName(substituteResVars("%osTempFolder%" + issfiles[i]).c_str());
        WinRes::SaveBinaryResource("ISSFILE",ResIDs[i],FileName.toStdString());
    }

#endif

}





void diagnoseThread::verboseExtraInfoItems(string theXml) {
    QDomDocument theDoc;
    theDoc.setContent(QString(theXml.c_str()));	
    QDomNode Params = theDoc.documentElement().toElement();

    QDomElement e_elem;
    QDomElement e_elem2;


    if (Params.namedItem("ExtraInfo").namedItem("List").isNull()) {
        QDomNode elem = Params.namedItem(QString("ExtraInfo")).firstChild();

        while (!elem.isNull()) {
            e_elem = elem.toElement();
            appendString("         "+ezw.AttribTextFromNode(elem,"InfoItemName")+" -> "+e_elem.text().toStdString()+".",QFont::Normal);
            elem = elem.nextSibling();
        }
    } 
    else {
        QDomNode elem = Params.namedItem("ExtraInfo").namedItem(QString("List")).firstChild();

        while (!elem.isNull()) {
            e_elem = elem.toElement();
            appendString("         "+ezw.AttribTextFromNode(elem,"ListItemName"),QFont::Normal);

            QDomNode elem2;
            elem2 = elem.firstChild();
            while (!elem2.isNull()) {

                e_elem2 = elem2.toElement();
                appendString("            "+ezw.AttribTextFromNode(elem2,"ItemPropertyName")+" -> "+e_elem2.text().toStdString()+".",QFont::Normal);

                elem2 = elem2.nextSibling();
            }
            elem = elem.nextSibling();
        }

    }
}

void diagnoseThread::buildReport(string theXml) {

    QDomDocument xmldoc;
    QDomElement docElem, e_cat;
    QDomNode dn_cat, dn_group, dn_property, paramnode;
    string propertyName;

    xmldoc.setContent(QString(theXml.c_str()));
    docElem = xml_in.documentElement();


    dn_cat = docElem.firstChild();
    while (!dn_cat.isNull())
        // Catogorien overlopen
    {
        e_cat = dn_cat.toElement();
        if (e_cat.tagName() == QString("Category")) {
            if (TestOnThisOS(dn_cat.namedItem("CheckOnOS"))) {

                paramnode = dn_cat.namedItem(QString("InputParams"));
                if (TestDependenciesOk(paramnode))
                {
                    appendStringReport( ezw.TextFromNode(dn_cat.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Bold);
                    if (!e_cat.isNull()) {
                        dn_group = dn_cat.firstChild();
                        while (!dn_group.isNull()) {
                            if (dn_group.toElement().childNodes().count() > 1) {
                                if (TestOnThisOS(dn_group.namedItem("CheckOnOS"))) {
                                    paramnode = dn_group.namedItem(QString("InputParams"));
                                    if (TestDependenciesOk(paramnode))
                                    {
                                        appendStringReport("   " + ezw.TextFromNode(dn_group.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Bold);
                                        dn_property = dn_group.firstChild();

                                        while (!dn_property.isNull()) {
                                            if (TestOnThisOS(dn_property.namedItem("CheckOnOS"))) {
                                                appendStringReport("      " + ezw.TextFromNode(dn_property.namedItem(QString("Description") + QString(currentLanguage.c_str()))),QFont::Normal);
                                                propertyName = ezw.AttribTextFromNode(dn_property, "CheckPropertyName");
                                                if (propertyName != "") {
                                                    QDomNode resultNode;
                                                    resultNode = dn_property.namedItem("Result");
                                                    if (!resultNode.isNull())
                                                    {
                                                        string testIsSuccess = ezw.TextFromNode(resultNode.namedItem("TestSuccessFull"));
                                                        appendStringReport(tr("         Success ? : ").toStdString() + testIsSuccess,QFont::Normal);
                                                        appendStringReport(tr("         TestResult : ").toStdString() + ezw.TextFromNode(resultNode.namedItem("QueriedResult")),QFont::Normal);
                                                        appendStringReport(tr("         Error code : ").toStdString() + ezw.TextFromNode(resultNode.namedItem("Error")),QFont::Normal);
                                                        if (QString(testIsSuccess.c_str()).contains("NO")) {
                                                            appendStringReport(tr("         Action : ").toStdString() +ezw.TextFromNode(dn_property.namedItem(QString("ActionInfo") + QString(currentLanguage.c_str())) ),QFont::Normal);
                                                        }


                                                        QDomElement e_elem;
                                                        QDomElement e_elem2;


                                                        if (resultNode.namedItem("ExtraInfo").namedItem("List").isNull()) {
                                                            QDomNode elem = resultNode.namedItem(QString("ExtraInfo")).firstChild();

                                                            while (!elem.isNull()) {
                                                                e_elem = elem.toElement();
                                                                appendStringReport("         "+ezw.AttribTextFromNode(elem,"InfoItemName")+" -> "+e_elem.text().toStdString()+".",QFont::Normal);
                                                                elem = elem.nextSibling();
                                                            }
                                                        } 
                                                        else {
                                                            QDomNode elem = resultNode.namedItem("ExtraInfo").namedItem(QString("List")).firstChild();

                                                            while (!elem.isNull()) {
                                                                e_elem = elem.toElement();
                                                                appendStringReport("         "+ezw.AttribTextFromNode(elem,"ListItemName"),QFont::Normal);

                                                                QDomNode elem2;
                                                                elem2 = elem.firstChild();
                                                                while (!elem2.isNull()) {

                                                                    e_elem2 = elem2.toElement();
                                                                    appendStringReport("            "+ezw.AttribTextFromNode(elem2,"ItemPropertyName")+" -> "+e_elem2.text().toStdString()+".",QFont::Normal);

                                                                    elem2 = elem2.nextSibling();
                                                                }
                                                                elem = elem.nextSibling();
                                                            }

                                                        }

                                                    }
                                                }
                                            }
                                            dn_property = dn_property.nextSibling();
                                        }
                                    }
                                }
                            }
                            dn_group = dn_group.nextSibling();
                        }
                    }
                }
            }
        } 

        dn_cat = dn_cat.nextSibling();
    }
}


void readCardThread::run() {
    string result = scl.readCard(inputparameters);
#ifdef WIN32
	scl.regCerts(inputparameters);
#endif
    verboseEvent * ve = new verboseEvent();
    ve->setParams("readCard",QString(result.c_str()),"","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}


void delayThread::run() {
    sleep(int2);
    verboseEvent * ve = new verboseEvent();
    ve->setParams("delayedAction",QString(action.c_str()),QString(str2.c_str()),QString(str3.c_str()),int1,int2,int3);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}

void detectReaderThread::run() {
	string readersXml;
	QDomNode resultNode;
	QDomNode listItem; 
#ifdef __APPLE__
	signal(SIGPIPE,SIG_IGN);
	QString currdirpath = QCoreApplication::applicationDirPath();
	std::string commandLine = "";
	commandLine += currdirpath.toStdString();
	commandLine += "/../Resources/preparePcscd.sh";
	if(CSysDiagnost::doAsAdmin(commandLine, true))
	{
		readersXml = scl.pcscWaitForCardReaders("");
#else
		readersXml = scl.pcscEnumerateCardReaders("");
#endif	
	resultNode = ezw.xmlToNode(readersXml);
	listItem = resultNode.namedItem("ExtraInfo").namedItem("List").namedItem("ListItem");
#ifdef __APPLE__
		}
	else {
		listItem.clear();
	}

#endif		
			
    while (listItem.isNull()) {
		msleep(500);
#ifdef __APPLE__
		if(CSysDiagnost::doAsAdmin(commandLine, true))
		{
			readersXml = scl.pcscWaitForCardReaders("");
#else
			readersXml = scl.pcscEnumerateCardReaders("");
#endif			
			resultNode = ezw.xmlToNode(readersXml);
			listItem = resultNode.namedItem("ExtraInfo").namedItem("List").namedItem("ListItem");
#ifdef __APPLE__
		}
#endif	
	}

	verboseEvent * ve = new verboseEvent();
    ve->setParams("detectedReader","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();
}

void detectCardThread::run() {
    // Rebuild readerlist.
	string readersXml;
	string selectedReader = "";
	QDomNode resultNode;
	QDomNode listItem;
	string _readerName;
	string cardData;
	do
	{
		msleep(500);
		readersXml = scl.pcscEnumerateCardReaders("");
		resultNode = ezw.xmlToNode(readersXml);
		listItem = resultNode.namedItem("ExtraInfo").namedItem("List").namedItem("ListItem");

		while (!listItem.isNull()) {
			_readerName = ezw.TextFromNode(listItem);
			cardData = scl.readCard("<InputParams><method>PCSC</method><readerName>" + _readerName + "</readerName><fileName>TOKENINFO</fileName></InputParams>");
			if (ezw.GetNamedItem(cardData,"QueriedResult") ==  "SUCCESS") {
				selectedReader = _readerName;
				break;
			}
			listItem = listItem.nextSibling();
		}
	}while(selectedReader=="");

	verboseEvent * ve = new verboseEvent();
    ve->setParams("detectedCard","","","",0,0,0);
    QApplication::postEvent(objectToUpdate,ve);
    QApplication::sendPostedEvents();

};
