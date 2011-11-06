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
#include <QtGui/QApplication>
#include "systemClass.h"

#define LOGSTR(msg) 
#define LOGINT(format, i) 

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL IsDots(const TCHAR* str) 
{
	if(_tcscmp(str,_T(".")) && _tcscmp(str,_T(".."))) return FALSE;
    return TRUE;
}

BOOL DeleteDirectory(const TCHAR* sPath) {
    HANDLE hFind;    // file handle
    WIN32_FIND_DATA FindFileData;

    TCHAR DirPath[MAX_PATH];
    TCHAR FileName[MAX_PATH];

    _tcscpy_s(DirPath, MAX_PATH, sPath);
    _tcscat_s(DirPath, MAX_PATH, _T("\\*"));    // searching all files
    _tcscpy_s(FileName, MAX_PATH, sPath);
    _tcscat_s(FileName, MAX_PATH, _T("\\"));

    // find the first file
    hFind = FindFirstFile(DirPath,&FindFileData);
    if(hFind == INVALID_HANDLE_VALUE) return FALSE;
    _tcscpy_s(DirPath, MAX_PATH, FileName);

    bool bSearch = true;
    while(bSearch) {    // until we find an entry
        if(FindNextFile(hFind,&FindFileData)) {
            if(IsDots(FindFileData.cFileName)) continue;
            _tcscat_s(FileName, MAX_PATH, FindFileData.cFileName);
            if((FindFileData.dwFileAttributes &
                FILE_ATTRIBUTE_DIRECTORY)) {

                    // we have found a directory, recurse
                    if(!DeleteDirectory(FileName)) {
                        FindClose(hFind);
                        return FALSE;    // directory couldn't be deleted
                    }
                    // remove the empty directory
                    RemoveDirectory(FileName);
                    _tcscpy_s(FileName, MAX_PATH, DirPath);
            }
            else {
                if(FindFileData.dwFileAttributes &
                    FILE_ATTRIBUTE_READONLY)
                    // change read-only file mode
                    _tchmod(FileName, _S_IWRITE);
                if(!DeleteFile(FileName)) {    // delete the file
                    FindClose(hFind);
                    return FALSE;
                }
                _tcscpy_s(FileName, MAX_PATH, DirPath);
            }
        }
        else {
            // no more files there
            if(GetLastError() == ERROR_NO_MORE_FILES)
                bSearch = false;
            else {
                // some error occurred; close the handle and return FALSE
                FindClose(hFind);
                return FALSE;
            }

        }

    }
    FindClose(hFind);                  // close the file handle

    return RemoveDirectory(sPath);     // remove the empty directory

}
 #endif


systemClass::systemClass(void) {

};

systemClass::~systemClass(void) {};


// about system and hardware
string systemClass::getSystemInfo(string inputXml){

    ezw.xml_in.setContent(QString(inputXml.c_str()));	

    ezw.xml_out.setContent(QString("<Result></Result>"));

    Os_INFO osinfo = { 0, 0, 0, "", "", "", "", "", "", "", "", "" };
    this->_SD.getSystemInfo(osinfo);

    QDomNode ExtraInfo = ezw.xml_in.createElement("ExtraInfo");
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",ezw.inttostr(osinfo.OSMajorVersion),"InfoItemName","osMajorVersion"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",ezw.inttostr(osinfo.OSMinorVersion),"InfoItemName","osMinorVersion"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",ezw.inttostr(osinfo.OSBuildNumber),"InfoItemName","osBuildNo"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.OSServicePack,"InfoItemName","osServicePack"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.OSProductName,"InfoItemName","osProductName"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.OSDesc,"InfoItemName","osDescription"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.UserName,"InfoItemName","osUserName"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.TempFolder,"InfoItemName","osTempFolder"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.WindowsFolder,"InfoItemName","osWindowsFolder"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.DesktopFolder,"InfoItemName","osDesktopFolder"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.SystemFolder,"InfoItemName","osSystemFolder"));
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",osinfo.DefaultLanguage,"InfoItemName","osDefaultLanguage"));

#ifdef WIN32
    QString systemDrive(osinfo.SystemFolder.c_str());
    systemDrive = systemDrive.mid(0,systemDrive.indexOf(':')+1);
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",systemDrive.toStdString(),"InfoItemName","osSystemDrive"));
#else
    QString systemDrive = "/";
    ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",systemDrive.toStdString(),"InfoItemName","osSystemDrive"));
#endif


    string returnValue = "SUCCESS", errorText = "";

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error", errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult", returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

string systemClass::usbMainHubAvailable(string inputXml){

    ezw.xml_in.setContent(QString(inputXml.c_str()));	
    ezw.xml_out.setContent(QString("<Result></Result>"));


    string errorText = "";
    string returnValue = "NO";


    QDomNode Params = ezw.xml_in.documentElement().toElement();

    string guid = Params.namedItem(QString("HUB_CLASSGUID")).toElement().text().toStdString(); // cast to string
    if (!Params.isNull()) 
    {
        if (! this->_SD.usbMainHubAvailable(guid)) {
            returnValue = "NO";
        } else {
            returnValue = "YES";
        }
    } 
    else {errorText = "ERROR_IN_INPUTPARAMS";}

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();
};

string systemClass::isAdministrator(string inputXml){
    return "";
};


// about pcsc
string systemClass::pcscContextIsAvailable(string inputXml){

    ezw.xml_in.setContent(QString(inputXml.c_str()));	
    ezw.xml_out.setContent(QString("<Result></Result>"));
    string returnValue = "", errorText = "";

    if (_SD.pcscContextIsAvailable()) {
        returnValue = "LOADED";
    }
    else {
        returnValue = "NOT_LOADED";		
    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error", errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult", returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));
    return ezw.xml_out.toString().toStdString();
}; 
#ifdef __APPLE__
string systemClass::pcscWaitForCardReaders(string inputXml){
    ezw.xml_in.setContent(QString(inputXml.c_str()));	
	
    ezw.xml_out.setContent(QString("<Result></Result>"));
	
    string returnValue = "", errorText = "";
	
    QDomNode List = ezw.xml_in.createElement("List");
	
    vector<std::string> readers;
    if (this->_SD.pcscWaitForCardReaders(readers)) {
        for (vector<std::string>::iterator it = readers.begin() ; it != readers.end(); ++it) {
            string &readername =  *it;
            List.appendChild(ezw.CreateAttribTextNode("ListItem",readername,"ListItemName","smartCardReader"));
        }
		
    }
    QDomNode ExtraInfo = ezw.xml_out.createElement("ExtraInfo");
    ExtraInfo.appendChild(List);
	
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error", errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult", returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);
    return ezw.xml_out.toString().toStdString();
};
#endif
string systemClass::pcscEnumerateCardReaders(string inputXml){
    ezw.xml_in.setContent(QString(inputXml.c_str()));	

    ezw.xml_out.setContent(QString("<Result></Result>"));

    string returnValue = "", errorText = "";

    QDomNode List = ezw.xml_in.createElement("List");

    vector<std::string> readers;
    if (this->_SD.pcscEnumerateCardReaders(readers)) {
        for (vector<std::string>::iterator it = readers.begin() ; it != readers.end(); ++it) {
            string &readername =  *it;
            List.appendChild(ezw.CreateAttribTextNode("ListItem",readername,"ListItemName","smartCardReader"));
        }

    }
    QDomNode ExtraInfo = ezw.xml_out.createElement("ExtraInfo");
    ExtraInfo.appendChild(List);

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error", errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult", returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);
    return ezw.xml_out.toString().toStdString();
};

// about smartcardreaders
string systemClass::smartCardReaderInstallFilesFound (string inputXml){
    ezw.xml_in.setContent(QString(inputXml.c_str()));	
    ezw.xml_out.setContent(QString("<Result></Result>"));


    string errorText = "";
    string returnValue = "";

    QDomNode Params = ezw.xml_in.documentElement().toElement();

    std::vector<std::string> params;
    params.push_back(Params.namedItem(QString("SCARDCLASSNAME")).toElement().text().toStdString()); // cast to string
    params.push_back(Params.namedItem(QString("SCARDCLASSGUID")).toElement().text().toStdString()); // cast to string
    params.push_back(Params.namedItem(QString("VID")).toElement().text().toStdString()); // cast to string
    params.push_back(Params.namedItem(QString("HWID")).toElement().text().toStdString()); // cast to string
    params.push_back(Params.namedItem(QString("DEVDESC")).toElement().text().toStdString()); // cast to string
    params.push_back(Params.namedItem(QString("MFG")).toElement().text().toStdString()); // cast to string


    if (! this->_SD.smartCardReaderInstallFilesFound(params)) {    
        returnValue = "NOT_INSTALLED";
        errorText = this->_SD.ErrorText();
    } else {
        returnValue = "INSTALLED";		
    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::canSeeSmartCardReaderAsUsbDevice (string inputXml){
    ezw.xml_in.setContent(QString(inputXml.c_str()));	
    ezw.xml_out.setContent(QString("<Result></Result>"));


    string errorText = "";
    string returnValue = "";

    QDomNode Params = ezw.xml_in.documentElement().toElement();

    std::vector<std::string> readerParams;
    readerParams.push_back(Params.namedItem(QString("SCARDCLASSGUID")).toElement().text().toStdString()); // cast to string
    readerParams.push_back(Params.namedItem(QString("HWID")).toElement().text().toStdString()); // cast to string

    if (! this->_SD.canSeeSmartCardReaderAsUsbDevice(readerParams)) {    
        returnValue = "NOT_CONNECTED";
        errorText = this->_SD.ErrorText();
    }
    else {
        returnValue = "CONNECTED";		
    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();
}; 

// about files
string systemClass::fileExists (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string fileName = ezw.GetNamedItem(Params,"fileName");

        bool returnValue = "NO";
        //DWORD attrib = GetFileAttributes(fileName.c_str());
        if (this->_SD.fileExists(fileName.c_str())) {
            returnValue = "YES";
        }
    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();
};


string systemClass::deleteFile (string inputXml){
    string errorText = "";
    string returnValue = "";
    returnValue = "SUCCESS";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {

        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();



        QDomNode pathToFile = Params.firstChild();
        if (!pathToFile.isNull()) {
            while (!pathToFile.isNull()) {
                if (!pathToFile.isComment()) {
                    string filename = ezw.TextFromNode(pathToFile);
                    if (! this->_SD.deleteFile(filename)) {
                        ExtraInfo.appendChild(ezw.CreateInfoTextNode("pathToFile",this->_SD.ErrorText()));
                    }
                }
                pathToFile = pathToFile.nextSibling();
            }
        }
    }
    catch (...){
        returnValue = "FAILURE";
        errorText = ezw.GetLastErrorText(GetLastError());
    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::fileIsLocked (string inputXml){
    
    #ifdef WIN32
    ifstream ifs;
    ifs.open("",ios::in, SH_DENYRW);
    #else
    this->_SD.fileIsLocked("");
    #endif
    return "";
}; 

string systemClass::versionInfo (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string filename = ezw.GetNamedItem(Params,"pathToFile");
        VersionINFO ver;
        if (this->_SD.versionInfo(filename, ver)) {
            returnValue = "SUCCESS";
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("MajorNumber",ver.MajorNumber));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("MinorNumber",ver.MinorNumber));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Release",ver.Release));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Build",ver.Build));
        }
        else {
            returnValue = "FAILURE";
            errorText = "Error : No version information found on file" + filename;			
        }
    }
    catch (...){
        returnValue = "FAILURE";
        errorText = "Error : No version information found on file.";	

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}; 

/*
#ifdef false
string systemClass::deleteFolder (string inputXml) {
    string errorText = "";
    string returnValue = "";
    try {

        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string foldername = ezw.GetNamedItem(Params,"pathToFolder");


        if (DeleteDirectory(foldername.c_str()) != 0 ) {
            returnValue = "SUCCESS";
        }
        else {
            returnValue = "FAILURE";
            errorText = ezw.GetLastErrorText(GetLastError());
        }
    }
    catch (...){
        returnValue = "FAILURE";
        errorText = ezw.GetLastErrorText(GetLastError());
    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();

}
#endif
*/
// about processes
string systemClass::findRunningProcess (string inputXml){

    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string processName = ezw.GetNamedItem(Params,"processName");

        std::vector<ProcINFO> pl;
        this->_SD.findRunningProcess(processName, pl);
        QDomNode ListNode = ezw.xml_in.createElement(QString("List"));

        for (std::vector<ProcINFO>::iterator it = pl.begin(); it != pl.end(); ++it) {
            QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","Process");
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",it->name,"ItemPropertyName","ProcessName"));
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",it->processId,"ItemPropertyName","ProcessId"));
            ListNode.appendChild(ListItemNode);
        }
        ExtraInfo.appendChild(ListNode);


    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();

};

string systemClass::libraryIsLoaded (string inputXml){
    return "";
}; 

string systemClass::killProcess (const char *ProcessName)
{
	std::string inputXml="";
	//inputXml+="<DescriptionNl>Stop '";
	//inputXml+=ProcessName;
	//inputXml+="'</DescriptionNl>\n";
	//inputXml+="<DescriptionFr>ArrÍter '";
	//inputXml+=ProcessName;
	//inputXml+="'</DescriptionFr>\n";
    inputXml+="<InputParams>\n";
    inputXml+="  <processName>";
 	inputXml+=ProcessName;
	inputXml+="</processName>\n";
    inputXml+="</InputParams>\n";

	return killProcess(inputXml);

}

string systemClass::killProcess (string inputXml)
{
LOGSTR(inputXml.c_str())

    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string processName = ezw.GetNamedItem(Params,"processName");
        string processID = ezw.GetNamedItem(Params,"processID");
        ProcINFO pi;
        if (processID.length() > 0) {
            pi.processId = processID;
        }
        else {
            if (processName.length() > 0) {
                pi.name = processName;
            }
        }
        returnValue = this->_SD.killProcess(pi);
    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("ExtraInfo",""));

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::startProcess (string inputXml){

LOGSTR(inputXml.c_str())

    string errorText = "";
    string returnValue = "";
    DWORD dwExitCode = 0;
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

		QDomNode Params = ezw.xml_in.documentElement().toElement();
		string commandLine = ezw.GetNamedItem(Params,"commandLine");
		string waitForTermination = ezw.GetNamedItem(Params,"waitForTermination");
		string bitness = ezw.GetNamedItem(Params,"bitness");

		if ( ((is64bitOS()== FALSE) && (bitness == "ONLY64")) ||
			 ((is64bitOS()== TRUE) && (bitness == "ONLY32"))	) {
			//We don't need to install this, so return success
			returnValue = "SUCCESS";
		}
		else
		{ 
			int waitTime = 0;
			if (waitForTermination == "YES") {
				waitTime = 10 * 60;          // 600 seconden = 10 minuten
			} else {
				try {
					int n = atoi(waitForTermination.c_str());
					if (n > 0)
						waitTime = n;
				} catch (...) {
					;
				}
			}

			DWORD exitCode = 0;

#ifdef WIN32
			if (this->_SD.startProcess(commandLine, exitCode, waitTime)) 
			{
				if (exitCode == STILL_ACTIVE) 
				{
					ExtraInfo.appendChild(ezw.CreateInfoTextNode("ExitCode", "Program still running... Can't wait anymore..."));
					returnValue = "UNKNOWN";
				} 
				else if (exitCode != 0) 
				{
					ExtraInfo.appendChild(ezw.CreateInfoTextNode("ExitCode","Program ended with code " + ezw.inttostr(dwExitCode)));
					returnValue = "FAILURE";
					LOGINT("FAILURE error=%ld",exitCode)
				} else {
					if (waitTime > 0)
						returnValue = "SUCCESS";
					else 
						returnValue = "STARTED";
				}
			}
#else
			//-----------------------
			// get the absolute path of where the application resides.
			// Use this to determine the path of the shell scripts for the installation
			//-----------------------
			QString currdirpath = QCoreApplication::applicationDirPath();
			std::string newCommandLine = "";
			newCommandLine += currdirpath.toStdString();
			newCommandLine += "/";
			newCommandLine += commandLine;
			commandLine = newCommandLine;
			LOGSTR(commandLine.c_str())
				if (this->_SD.doAsAdmin(commandLine, (waitForTermination == "YES"))) 
				{
					if (waitForTermination == "YES")
						returnValue = "SUCCESS";
					else 
						returnValue = "STARTED";
				} 
#endif
				else 
				{
					errorText = "StartProcess failure code: " + ezw.inttostr(exitCode);
					returnValue = "FAILURE";
					LOGINT("FAILURE error=%ld",exitCode)
				}

		}
    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::getLibraryDependencies (string inputXml){
    return "";
};

#ifdef WIN32
// about services or daemons
string systemClass::isServiceRunning (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string serviceName = ezw.GetNamedItem(Params,"serviceName");


		CServiceControl sc(serviceName.c_str());
        ServiceInfo info = sc.DoGetServiceInfo();


        if (sc.Succeeded()) {

            if (sc.CurrentState() == SERVICE_RUNNING) {
                returnValue = "YES";
            }
            else {
                returnValue = "NO";
            }

            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceName",sc.ServiceName()));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("DisplayName",info.DisplayName));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Description",info.Description));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Dependencies",info.Dependencies));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("PathName",info.Pathname));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("UserAccount",info.UserAccount));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("LoadOrderGroup",info.LoadOrderGroup));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceType",ezw.inttostr(info.ServiceType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("StartType",ezw.inttostr(info.StartType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("TagID",ezw.inttostr(info.TagID)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("CurrentStatus",sc.CurrentStateAsString()));
        }
        else {
            //errorText = ezw.GetLastErrorText( sc.LastError());
            returnValue = "SERVICE NOT_FOUND";
        }




    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::startService (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string serviceName = ezw.GetNamedItem(Params,"serviceName");

        CServiceControl sc(serviceName.c_str());
        ServiceInfo info = sc.DoGetServiceInfo();


        if (sc.Succeeded()) {

            if (sc.CurrentState() == SERVICE_RUNNING) {
                returnValue = "SUCCESS";
            }
            else {
                //if (sc.StartServiceA()) {
				if (sc.DoStartService())
				{
                    returnValue = "SUCCESS";
                }
                else {
                    returnValue = "FAILURE";	
                    errorText = ezw.GetLastErrorText(sc.LastError());
                }
            }
            info = sc.DoGetServiceInfo();
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceName",sc.ServiceName()));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("DisplayName",info.DisplayName));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Description",info.Description));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Dependencies",info.Dependencies));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("PathName",info.Pathname));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("UserAccount",info.UserAccount));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("LoadOrderGroup",info.LoadOrderGroup));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceType",ezw.inttostr(info.ServiceType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("StartType",ezw.inttostr(info.StartType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("TagID",ezw.inttostr(info.TagID)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("CurrentStatus",sc.CurrentStateAsString()));
        }
        else {
            errorText = ezw.GetLastErrorText( sc.LastError());
            returnValue = "NOT_FOUND";
        }
    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}; 

string systemClass::stopService (const char *ServiceName)
{
	std::string inputXml="";
	//inputXml+="<DescriptionNl>Stop de service '";
	//inputXml+=ServiceName;
	//inputXml+="'</DescriptionNl>\n";
	//inputXml+="<DescriptionFr>ArrÍter le service '";
	//inputXml+=ServiceName;
	//inputXml+="'</DescriptionFr>\n";
    inputXml+="<InputParams>\n";
    inputXml+="  <serviceName>";
 	inputXml+=ServiceName;
	inputXml+="</serviceName>\n";
    inputXml+="</InputParams>\n";

	return stopService(inputXml);
}

string systemClass::stopService (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string serviceName = ezw.GetNamedItem(Params,"serviceName");

        CServiceControl sc(serviceName.c_str());
        ServiceInfo info = sc.DoGetServiceInfo();


        if (sc.Succeeded()) {

            if (sc.CurrentState() == SERVICE_STOPPED) {
                returnValue = "SUCCESS";
            }
            else {
                if (sc.DoStopService()) {
                    returnValue = "SUCCESS";
                }
                else {
                    returnValue = "FAILURE";	
                    errorText = ezw.GetLastErrorText(sc.LastError());
                }
            }

            info = sc.DoGetServiceInfo();

            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceName",sc.ServiceName()));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("DisplayName",info.DisplayName));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Description",info.Description));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Dependencies",info.Dependencies));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("PathName",info.Pathname));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("UserAccount",info.UserAccount));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("LoadOrderGroup",info.LoadOrderGroup));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceType",ezw.inttostr(info.ServiceType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("StartType",ezw.inttostr(info.StartType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("TagID",ezw.inttostr(info.TagID)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("CurrentStatus",sc.CurrentStateAsString()));
        }
        else {
            errorText = ezw.GetLastErrorText( sc.LastError());
            returnValue = "NOT FOUND";
        }
    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

string systemClass::removeService (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string serviceName = ezw.GetNamedItem(Params,"serviceName");

        CServiceControl sc(serviceName.c_str());
        ServiceInfo info = sc.DoGetServiceInfo();


        if (sc.Succeeded()) {
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceName",sc.ServiceName()));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("DisplayName",info.DisplayName));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Description",info.Description));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("Dependencies",info.Dependencies));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("PathName",info.Pathname));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("UserAccount",info.UserAccount));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("LoadOrderGroup",info.LoadOrderGroup));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("ServiceType",ezw.inttostr(info.ServiceType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("StartType",ezw.inttostr(info.StartType)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("TagID",ezw.inttostr(info.TagID)));
            ExtraInfo.appendChild(ezw.CreateInfoTextNode("CurrentStatus",sc.CurrentStateAsString()));

            sc.DoStopService();
            sc.DoDisableService();
            if (sc.DoDeleteService()) {
                returnValue = "SUCCESS";						       
            }
            else {
                returnValue = "FAILURE";						    
                errorText = ezw.GetLastErrorText(sc.LastError());
            }					

        }
        else {
            errorText = ezw.GetLastErrorText( sc.LastError());
            returnValue = "NOT_FOUND";
        }
    }
    catch (...) {
        errorText = ezw.GetLastErrorText( GetLastError());
        returnValue = "FAILURE";
    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

// about Registry
string systemClass::readRegistryEntry(string inputXml){
    HKEY theRootKey;
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));


    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string rootName = ezw.GetNamedItem(Params,"rootName");
        string path = ezw.GetNamedItem(Params,"path");
        string keyName = ezw.GetNamedItem(Params,"keyName");

        if (rootName == "HKEY_LOCAL_MACHINE") {
            theRootKey = HKEY_LOCAL_MACHINE;  
        } 
        else {
            if (rootName == "HKEY_CLASSES_ROOT") {
                theRootKey = HKEY_CLASSES_ROOT;
            }
            else {
                if (rootName == "HKEY_CURRENT_USER") {
                    theRootKey = HKEY_CURRENT_USER;
                }
                else {
                    if (rootName == "HKEY_USERS") {
                        theRootKey = HKEY_USERS;
                    }
                }
            }
        }


        CRegistry reg(theRootKey,path.c_str(), KEY_QUERY_VALUE);
        if (reg.IsOpen()) {

            if (keyName != "") {
                if (reg.GetIntValue(keyName.c_str())) {
                    // is integer
                    returnValue = ezw.inttostr(reg.IntValue());
                }
                else {
                    if (reg.GetStringValue(keyName.c_str())) {
                        returnValue = reg.StringValue();
                    }
                    else {
                        if (reg.GetBinaryValue(keyName.c_str())) {
                            // is binary Value
                            returnValue = ezw.toHEX(reg.BinValue());
                        }
                        else {
                            if (reg.GetExpandStringValue(keyName.c_str())) {
                                // is ExpandStringValue
                                returnValue = reg.StringValue();
                            }
                            else {
                                if (reg.GetMultiStringValue(keyName.c_str())) {
                                    MSZArray msza =   reg.StringArrayValue();
                                    QDomNode ListNode = ezw.xml_in.createElement(QString("List"));

                                    MZSIterator last = msza.end();
                                    for (MZSIterator it = msza.begin(); it != last; ++it) {
                                        QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","String");
                                        ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",*it,"ItemPropertyName","Line"));
                                        ListNode.appendChild(ListItemNode);
                                    }
                                    ExtraInfo.appendChild(ListNode);
                                } 
                                else {
                                    // nu weten we 't niet meer. 't is een fout.
                                    errorText = "Key not found";									
                                }
                            }
                        }
                    }
                }
            }
            else {
                if (reg.GetNames()) {
                    RegNames rn = reg.Names();
                    QDomNode ListNode = ezw.xml_in.createElement(QString("List"));
                    RegNamesIterator last = rn.end();
                    for (RegNamesIterator it = rn.begin(); it != last; ++it) {
                        QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","keys");
                        ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",*it,"ItemPropertyName","key"));
                        ListNode.appendChild(ListItemNode);
                    }
                    ExtraInfo.appendChild(ListNode);

                    // 
                }
            }
        }
        else {

            errorText = reg.LastError();	
        }
    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

string systemClass::writeRegistryEntry (string inputXml){
    HKEY theRootKey;
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));


    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string rootName = ezw.GetNamedItem(Params,"rootName");
        string path = ezw.GetNamedItem(Params,"path");
        string keyName = ezw.GetNamedItem(Params,"keyName");
        string value = ezw.GetNamedItem(Params,"value");
        string keyType = ezw.GetNamedItem(Params,"keyType");

        if (rootName == "HKEY_LOCAL_MACHINE") {
            theRootKey = HKEY_LOCAL_MACHINE;  
        } 
        else {
            if (rootName == "HKEY_CLASSES_ROOT") {
                theRootKey = HKEY_CLASSES_ROOT;
            }
            else {
                if (rootName == "HKEY_CURRENT_USER") {
                    theRootKey = HKEY_CURRENT_USER;
                }
                else {
                    if (rootName == "HKEY_USERS") {
                        theRootKey = HKEY_USERS;
                    }
                }
            }
        }
        CRegistry reg(theRootKey,path.c_str(), KEY_ALL_ACCESS);
        if (reg.IsOpen()) {

            if (keyType == "DWORD") {
                DWORD _dwordvalue;

                try {
                    _dwordvalue = atoi(value.c_str());
                }
                catch (...) { 
                    _dwordvalue = 0;
                }


                if (reg.SetDWordValue(keyName.c_str(),_dwordvalue)) {
                    returnValue = "SUCCESS";
                }
                else {
                    returnValue = "FAILURE";
                    errorText = reg.LastError();				
                }
            }
            else {
                if (reg.SetExpandStringValue(keyName.c_str(),value.c_str())) {
                    returnValue = "SUCCESS";
                }
                else {
                    returnValue = "FAILURE";
                    errorText = reg.LastError();
                }

            }

        }


    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

string systemClass::deleteRegistryEntry (string inputXml){

    HKEY theRootKey;
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));


    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string rootName = ezw.GetNamedItem(Params,"rootName");
        string path = ezw.GetNamedItem(Params,"path");
        string keyName = ezw.GetNamedItem(Params,"keyName");

        if (rootName == "HKEY_LOCAL_MACHINE") {
            theRootKey = HKEY_LOCAL_MACHINE;  
        } 
        else {
            if (rootName == "HKEY_CLASSES_ROOT") {
                theRootKey = HKEY_CLASSES_ROOT;
            }
            else {
                if (rootName == "HKEY_CURRENT_USER") {
                    theRootKey = HKEY_CURRENT_USER;
                }
                else {
                    if (rootName == "HKEY_USERS") {
                        theRootKey = HKEY_USERS;
                    }
                }
            }
        }

        CRegistry reg(theRootKey,path.c_str(),KEY_ALL_ACCESS);
        if (reg.IsOpen())
        {
            if (reg.DeleteEntry(keyName.c_str())) {
                returnValue = "SUCCESS";
            }
            else {
                if (reg.DeleteKeyTree(keyName.c_str())) {
                    returnValue = "SUCCESS";
                }
                else {
                    returnValue = "FAILURE";
                    errorText = reg.LastError();
                }
            }
        }


    }
    catch (...) {

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();

};

#endif


#ifdef WIN32
// about devices
string systemClass::getdevicesList (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    QDomNode ListNode = ezw.xml_in.createElement(QString("List"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));


        if (lDevList.BuildDeviceList()){
            string lastEnumerator = "";

            DevInfoIterator last = lDevList.getLast();

            for (DevInfoIterator it = lDevList.getFirst(); it != last; ++it) {

                string lDevProperty;
                CDeviceInfo &di = *it;

                lDevProperty = di.getEnumerator();
                if (lDevProperty != lastEnumerator) {
                    lastEnumerator = lDevProperty;
                    // cout << lDevProperty << endl;
                }

                QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","device");
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getClassGUID(),"ItemPropertyName","GUID"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getClassType(),"ItemPropertyName","ClassType"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getDescription(),"ItemPropertyName","Description"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getDriverInfo(),"ItemPropertyName","DriverInfo"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getFriendlyName(),"ItemPropertyName","FriendlyName"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getHardwareID(),"ItemPropertyName","HardwareID"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getLocation(),"ItemPropertyName","Location"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getMfg(),"ItemPropertyName","MFG"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getPhysicalObjName(),"ItemPropertyName","PhysicalObjName"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getService(),"ItemPropertyName","Service"));
                ListNode.appendChild(ListItemNode);

            }
        }
        ExtraInfo.appendChild(ListNode);


    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

bool systemClass::IsDeviceInstallInprogress (DWORD dwTimeOutMillis){
	return ud.IsDeviceInstallInprogress(dwTimeOutMillis);
}

string systemClass::isDevicePresent (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    QDomNode ListNode = ezw.xml_in.createElement(QString("List"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string hardwareID = ezw.GetNamedItem(Params,"hardwareID");
        string strGUID = ezw.GetNamedItem(Params,"GUID");
        char *DontCare = NULL;

        DevicesFound& df = lDevList.FindDevices(
            strGUID.c_str(),
            DontCare,
            hardwareID.c_str(),        // vendor id
            DontCare,
            DontCare,
            DontCare,
            DontCare,
            DontCare,
            DontCare,
            DontCare,
            DontCare,
            DontCare);



        if (df.size() > 0){
            string lastEnumerator = "";

            DevInfoIterator last = df.end();

            for (DevInfoIterator it = df.begin(); it != last; ++it) {

                string lDevProperty;
                CDeviceInfo &di = *it;

                lDevProperty = di.getEnumerator();
                if (lDevProperty != lastEnumerator) {
                    lastEnumerator = lDevProperty;

                }

                QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","device");
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getClassGUID(),"ItemPropertyName","GUID"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getClassType(),"ItemPropertyName","ClassType"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getDescription(),"ItemPropertyName","Description"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getDriverInfo(),"ItemPropertyName","DriverInfo"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getFriendlyName(),"ItemPropertyName","FriendlyName"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getHardwareID(),"ItemPropertyName","HardwareID"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getLocation(),"ItemPropertyName","Location"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getMfg(),"ItemPropertyName","MFG"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getPhysicalObjName(),"ItemPropertyName","PhysicalObjName"));
                ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty",di.getService(),"ItemPropertyName","Service"));
                ListNode.appendChild(ListItemNode);

            }
            returnValue = "YES";
        }
        else {
            returnValue = "NO";
        }
        ExtraInfo.appendChild(ListNode);


    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};


string systemClass::installDevice (string inputXml, string myOS){

    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));


    try {

        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string hardwareID = ezw.GetNamedItem(Params,"hardwareID");
        string pathToDriverFiles = ezw.GetNamedItem(Params,"pathToDriverFiles");
        string infFile = ezw.GetNamedItem(Params,"infFile");
        string readerUnzipCode = ezw.GetNamedItem(Params,"readerUnzipCode");

        if (errorText == "") {
            bool bootreqd;
			bool retVal;

            string pathToInfFile = infFile;
			if (myOS == "Windows 7")
			{
				bootreqd = false;
				retVal = ud.PrepareDriver(pathToInfFile);
			}
			else
			{
				retVal = ud.UpdateDriver(NULL,hardwareID.c_str() , pathToInfFile, 0, &bootreqd);
			}

            if (!retVal)  {
                returnValue = "FAILURE";
                errorText = ud.get_LastError();
            } 
            else {
                returnValue = "SUCCESS";	
                if (bootreqd) {
                    ExtraInfo.appendChild(ezw.CreateInfoTextNode("BootRequired","YES"));
                }
                else {
                    ExtraInfo.appendChild(ezw.CreateInfoTextNode("BootRequired","NO"));				
                }
            }
        }
    }
    catch (...) {
        returnValue = "FAILURE";
        errorText = "";		
    }


    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

#endif

// about installed software
string systemClass::getSoftwareList(string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    QDomNode ListNode = ezw.xml_in.createElement(QString("List"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));


        std::vector<Soft_INFO> sl;
        this->_SD.getSoftwareList(sl);

        for (std::vector<Soft_INFO>::iterator it = sl.begin(); it != sl.end(); ++it) {
            QDomNode ListItemNode = ezw.CreateAttribTextNode("ListItem","", "ListItemName","software");
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty", it->ProductName,"ItemPropertyName","ProductName"));
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty", it->DisplayName,"ItemPropertyName","DisplayName"));
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty", it->ProductVersion,"ItemPropertyName","DisplayVersion"));
            ListItemNode.appendChild(ezw.CreateAttribTextNode("ItemProperty", it->UninstallString,"ItemPropertyName","UninstallString"));
            ListNode.appendChild(ListItemNode);

        }

        ExtraInfo.appendChild(ListNode);


    }
    catch (...) {

    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};

#ifdef WIN32
string systemClass::WaitForUninstall(string inputXml) {
    string errorText = "";
    string returnValue = "NO";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string productName = ezw.GetNamedItem(Params,"productName");
        string time = ezw.GetNamedItem(Params, "waitSeconds");

        int waitTime = atoi(time.c_str()) / 5;

        for (int i = 0; i < waitTime; ++i) {
            std::vector<Soft_INFO> sl;
            this->_SD.isSoftwareInstalled(productName, sl);
            if (sl.size() > 0){
                // nog steeds aanwezig
                ::Sleep(5000);
            }
            else {
                returnValue = "YES";
                break;
            }
        }
    }
    catch (...) {
        ;
    }
    if (returnValue == "NO")
        errorText = "Time-Out";

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();

}
#endif

string systemClass::isSoftwareInstalled (string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string productName = ezw.GetNamedItem(Params,"productName");

        std::vector<Soft_INFO> sl;
        this->_SD.isSoftwareInstalled(productName, sl);
        if (sl.size() > 0){
            returnValue = "YES";
            for (std::vector<Soft_INFO>::iterator it = sl.begin(); it != sl.end(); ++it) {
                ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",it->ProductName,"InfoItemName","ProductName"));
                ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",it->DisplayName,"InfoItemName","DisplayName"));
                ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",it->ProductVersion,"InfoItemName","DisplayVersion"));
                ExtraInfo.appendChild(ezw.CreateAttribTextNode("InfoItem",it->UninstallString,"InfoItemName","UninstallString"));
            }
        }
        else {
            returnValue = "NO";
        }
    }
    catch (...) {
        ;
    }

    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();

};

#ifdef WIN32
string systemClass::regCerts(string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();
        string readerName = ezw.GetNamedItem(Params,"readerName");

       if (this->_SD.RegCerts(readerName))
            returnValue = "SUCCESS";
	}
    catch (...){
        returnValue = "FAILURE";
        errorText = ezw.GetLastErrorText(GetLastError());
    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
}
#endif

// about using cards
string systemClass::readCard(string inputXml){
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string method = ezw.GetNamedItem(Params,"method");
        string readerName = ezw.GetNamedItem(Params,"readerName");
        string fileName = ezw.GetNamedItem(Params,"fileName");

        CardDATA cd;
        if ((this->_SD.readCard(readerName, fileName, cd)) && (cd.TokenInfo.length() > 0)) { 

            if ((fileName == "ATR") || (fileName == "ALL")) {
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("ATR", cd.ATR));
            }
            if (fileName == "TOKENINFO") {
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("TokenInfo", cd.TokenInfo));		
            }
            if ((fileName == "ID") || (fileName == "ALL"))  {
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("CardNo",cd.swCardNr));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("NationalNumber",cd.swNationalNr));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("LastName",cd.swLastName));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("FirstName",cd.swFirstName));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("BirthDate",cd.swBirthDate));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("BirthPlace",cd.swBirthLocation));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("validFrom",cd.swCardValDateBegin));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("validUntil",cd.swCardValDateEnd));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("IssuingCommunity",cd.swCardDeliveryMunicip));
            }
            if ((fileName == "ADRESS") || (fileName == "ALL"))  {
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("Postalcode",cd.swZIPCode));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("Streetname",cd.swStreetNr));
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("Community",cd.swMunicipality));
            }
            if ((fileName == "PHOTO") || (fileName == "ALL"))  {
                ExtraInfo.appendChild(ezw.CreateInfoTextNode("Photo", cd.Photo));			
            }
            returnValue = "SUCCESS";
        }
        else {
            returnValue = "FAILURE";		
            errorText = "Could not read card in " + readerName + ", " + this->_SD.ErrorText();
        }
    }
    catch (...){
        returnValue = "FAILURE";
        errorText = ezw.GetLastErrorText(GetLastError());

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};
//string systemClass::AuthSign(string inputXml){

//#pragma region oldstuff
    /*	try
    {
    unsigned long ulRemaining=0xFFFF;

    PTEID_ReaderContext &reader = ReaderSet.getReader();
    PTEID_EIDCard &card = reader.getEIDCard();

    if(card.getPins().getPinByNumber(0).verifyPin("",ulRemaining))
    {
    std::cout << "verify pin succeeded" << std::endl;
    }
    else
    {
    if(ulRemaining==0xFFFF)
    std::cout << "verify pin canceled" << std::endl;
    else
    std::cout << "verify pin failed (" << ulRemaining << " tries left)" << std::endl;
    }
    }
    catch(PTEID_ExCardBadType &ex)
    {
    std::cout << "This is not an eid card" << std::endl;
    }
    catch(PTEID_ExNoCardPresent &ex)
    {
    std::cout << "No card present" << std::endl;
    }
    catch(PTEID_ExNoReader &ex)
    {
    std::cout << "No reader found" << std::endl;
    }
    catch(PTEID_Exception &ex)
    {
    std::cout << "PTEID_Exception exception" << std::endl;
    }
    catch(...)
    {
    std::cout << "Other exception" << std::endl;
    }
    */
//#pragma endregion oldstuff

/*
    string errorText = "";
    string returnValue = "";
    QDomNode ExtraInfo = ezw.xml_out.createElement(QString("ExtraInfo"));
    try {
        ezw.xml_in.setContent(QString(inputXml.c_str()));	
        ezw.xml_out.setContent(QString("<Result></Result>"));

        QDomNode Params = ezw.xml_in.documentElement().toElement();

        string readerName = ezw.GetNamedItem(Params,"readerName");
		std::string ret = this->_SD.AuthSign(readerName);
		if (ret == "") {
            returnValue = "SUCCESS";
        }
		else if (QString(ret.c_str()).contains("CKR_FUNCTION_CANCELED"))
		{
			returnValue = "CANCELLED";
            errorText = "CANCELLED";
            this->_errorMsg = errorText;
		}
		else
		{
            returnValue = "FAILURE";
            errorText = this->_SD.ErrorText();
            this->_errorMsg = errorText;
        }
    }
    catch (...){
        returnValue = "FAILURE";
        errorText = ezw.GetLastErrorText(GetLastError());

    }
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("Error",errorText));
    ezw.xml_out.documentElement().appendChild(ezw.CreateTextNode("QueriedResult",returnValue));
    ezw.xml_out.documentElement().appendChild(ExtraInfo);

    return ezw.xml_out.toString().toStdString();
};*/

bool systemClass::is64bitOS() {
#ifdef WIN32
	LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
#endif
	bool bIsOS64 = FALSE;
// not used by mac as universal libs are used there
#ifdef WIN64
	bIsOS64 = TRUE;
#elif WIN32
    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
		if(bIsWow64)
		{
			//"32 bit application running on 64 bit Windows"
			bIsOS64 = TRUE;
		}
		//else "32 bit application running on 32 bit Windows"
    }
	//else "32 bit application running on 32 bit Windows"
  
#endif
	return bIsOS64;
};
