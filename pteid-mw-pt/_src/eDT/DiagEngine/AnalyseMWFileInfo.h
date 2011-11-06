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
#ifndef _ANALYSEMWFILEINFO_H_
#define _ANALYSEMWFILEINFO_H_

#include <stdio.h>
#include <exception>
#include <sstream>
#include <vector>
#include <string>

#include "analysis.h"
#include "middleware.h"
#include "AnalysisError.h"
#include "folder.h"
#include "util.h"
#include "MD5Sum.h"
#ifdef __APPLE__
#include "mac_helper.h"
#endif

#ifdef WIN32
static wchar_t* FilesSystem[]=
{
	  L"pteid35applayer.dll"
	, L"pteid35cardlayer.dll"
	, L"pteid35common.dll"
	, L"pteid35DlgsWin32.dll"
//	, L"siscardplugins\\siscardplugin1_BE_EID_35__ACS_ACR38U__.dll"
	, L"libeay32_0_9_8g.dll"
	, L"ssleay32_0_9_8g.dll"
//	, L"xerces-c_2_8.dll"
	, L"xerces-c_3_1.dll"
	, L"pteidCSP.dll"
	, L"pteidCSPlib.dll"
	, L"pteidpkcs11.dll"
	, L"Portugal Identity Card PKCS11.dll"
};
static wchar_t* FilesApp []=
{
	  L"pteid35libCpp.dll"
	, L"pteid35gui.exe"
	, L"eidmw_en.qm"
	, L"eidmw_nl.qm"
	, L"eidmw_fr.qm"
	, L"eidmw_de.qm"
	, L"QtCore4.dll"
	, L"QtGui4.dll"
	, L"imageformats\\qjpeg4.dll"
};
#else
static wchar_t* FilesSystem[]=
{
	  L"libpteidapplayer.3.5.3.dylib"
	, L"libpteidapplayer.3.5.dylib"
	, L"libpteidapplayer.3.dylib"
	, L"libpteidapplayer.dylib"
	, L"libpteidcardlayer.3.5.3.dylib"
	, L"libpteidcardlayer.3.5.dylib"
	, L"libpteidcardlayer.3.dylib"
	, L"libpteidcardlayer.dylib"
	, L"libpteidcommon.3.5.3.dylib"
	, L"libpteidcommon.3.5.dylib"
	, L"libpteidcommon.3.dylib"
	, L"libpteidcommon.dylib"
	, L"libpteiddialogsQT.3.5.3.dylib"
	, L"libpteiddialogsQT.3.5.dylib"
	, L"libpteiddialogsQT.3.dylib"
	, L"libpteiddialogsQT.dylib"
//	, L"siscardplugins/libsiscardplugin1__ACS__.dylib"
//	, L"libeay32_0_9_8g.dll"
//	, L"ssleay32_0_9_8g.dll"
	, L"libxerces-c.28.0.dylib"
	, L"libxerces-c.28.dylib"
	, L"libpteidpkcs11.3.5.3.dylib"
	, L"libpteidpkcs11.3.5.dylib"
	, L"libpteidpkcs11.3.dylib"
	, L"libpteidpkcs11.dylib"
	, L"libpteidlib.3.5.3.dylib"
	, L"libpteidlib.3.5.dylib"
	, L"libpteidlib.3.dylib"
	, L"libpteidlib.dylib"
};
static wchar_t* FilesApp []=
{
	  L"pteidgui"
	, L"eidmw_en.qm"
	, L"eidmw_nl.qm"
	, L"eidmw_fr.qm"
	, L"eidmw_de.qm"
};
static wchar_t* FilesOther []=
{
	L"/usr/local/lib/pteidqt/QtCore"
	, L"/usr/local/lib/pteidqt/QtGui"
	, L"/usr/local/lib/pteidqt/plugins/imageformats/libpteidjpeg.dylib"

//	L"/Library/Frameworks/QtCore.framework/Versions/pteid_*/QtCore"
//	, L"/Library/Frameworks/QtGui.framework/Versions/pteid_*/QtGui"
//	, L"/Developer/Applications/Qt/plugins/imageformats/libpteidjpeg.dylib"
};
#endif

//******************************************
// Middleware File info
// Tries to detect that all necessary files are available
// Passed:
//		All files available
// Failed:
//		One or more files missing
//******************************************
class AnalyseMWFileInfo : public Analysis
{
public:
	AnalyseMWFileInfo()
	{
		m_testName		= "middleware_files";
		m_friendlyName	= "Middleware files";

	}
	virtual ~AnalyseMWFileInfo()
	{
	}

	virtual int run()
	{
		m_bPassed = false;
		setProgress(0);
		setStartTime();

		int			retVal		= DIAGLIB_OK;
		Report_TYPE reportType	= REPORT_TYPE_RESULT;
		wchar_t		sepa		= L'~';

		reportPrintHeader2(reportType, L"Middleware files info", sepa);

		try
		{

			//------------------------------------------
			// write to the report what we're doing
			//------------------------------------------
			resultToReport(reportType,L"[Info ] Detecting installed Middleware files");

			Folder_ID system;
			folderGetPath(FOLDER_SYSTEM,&system);

			Folder_ID appfolder;
			folderGetPath(FOLDER_APP,&appfolder);
#ifdef __APPLE__
			appfolder += L"eID-Viewer.app/Contents/MacOS/";
#endif
			//------------------------------------------
			// Loop over all the files and check if they exist
			//------------------------------------------
			std::wstring msg(L"[Info ] Detecting ");
#ifdef WIN32			
			msg += L"system32 ";
#else
			msg += L"system ";
#endif
			msg += L"files";
			resultToReport(reportType,msg.c_str());
			bool bTmpPass = true;
			std::wstring fullPath;
			
			REP_PREFIX(L"system");
			for (size_t idx=0;idx<sizeof(FilesSystem)/sizeof(wchar_t*);idx++)
			{
				REP_PREFIX(FilesSystem[idx]);
				bool exist = false;
				fullPath = system;
				fullPath += FilesSystem[idx];
				retVal = fileExists(fullPath,&exist);
				if (DIAGLIB_OK != retVal)
				{
					// should not happen
				}
				std::wstringstream text;
				text << L"File ";
				if (!exist)
				{
					bTmpPass = false;
					text << L"not "; 
				}
				text << L"found: "; 
				text << FilesSystem[idx] << L"";

				REP_CONTRIBUTE(L"exists",exist?L"true":L"false");

				if(exist)
				{
					std::wstring	sumstr;
					MD5Sum			sum;
									sum.add_file(string_From_wstring(fullPath));
									sumstr=sum.get_sum();
					text << L" md5=[" << sumstr << L"]";
					REP_CONTRIBUTE(L"md5",sumstr);
				}

				resultToReport(reportType,text);
				REP_UNPREFIX();
			}

			REP_UNPREFIX();
			resultToReport(reportType,L"[Info ] Detecting application files");
			REP_PREFIX(L"application");
			for (size_t idx=0;idx<sizeof(FilesApp)/sizeof(wchar_t*);idx++)
			{
				REP_PREFIX(FilesApp[idx]);
				bool exist = false;
				fullPath = appfolder;
				fullPath += FilesApp[idx];
				retVal = fileExists(fullPath,&exist);
				if (DIAGLIB_OK != retVal)
				{
					// should not happen
				}
				std::wstringstream text;
				text << L"File ";
				if (!exist)
				{
					bTmpPass = false;
					text << L"not "; 
				}
				text << L"found: ";
				text << FilesApp[idx] << L"";

				REP_CONTRIBUTE(L"exists",exist?L"true":L"false");

				if(exist)
				{
					std::wstring	sumstr;
					MD5Sum			sum;
									sum.add_file(string_From_wstring(fullPath));
									sumstr=sum.get_sum();
					text << L" md5=[" << sumstr << L"]";
					REP_CONTRIBUTE(L"md5",sumstr);
				}

				resultToReport(reportType,text);
				REP_UNPREFIX();
			}
			REP_UNPREFIX();

#ifdef __APPLE__	
			
			resultToReport(reportType,L"[Info ] Detecting other files");

			if(!CheckQtFiles(reportType))
			{
				bTmpPass = false;
			}
		
#endif
			
			m_bPassed = bTmpPass;
			//------------------------------------------
			// it seems to be working ok, return
			//------------------------------------------
			processParamsToStop();
		}
		//------------------------------------------
		// exception from writing to the report
		//------------------------------------------
		catch (ExcReport& exc) 
		{
			processParamsToStop();
			retVal = exc.getErr();
		}
		resultToReport(reportType,m_bPassed);
		return retVal;
	}
private:
#ifdef __APPLE__	

	std::string getAppPath()
	{
		std::string strAppFolder;
		Folder_ID appfolder;
		folderGetPath(FOLDER_APP,&appfolder);
		
		for(size_t idx=0;idx<appfolder.size();idx++)
		{
			strAppFolder += appfolder.at(idx);
		}
		strAppFolder += "eID-Viewer.app/Contents/MacOS/pteidgui";
		return strAppFolder;
	}
/*	
	std::wstring getAppQtPath(std::string strAppFolder,std::string QtPart)
	{
	#define MAX_BUFFER_SIZE 1024

		std::string cmd(GetOtoolPath());
		cmd += " -L \"";
		cmd += strAppFolder;
		cmd += "\" | egrep -o 'pteid_([4-9]\\.[0-9]\\.[0-9]/";
		cmd += QtPart;
		cmd += ")'";
		
		std::string data;
		FILE* stream=NULL;
		char buffer[MAX_BUFFER_SIZE];
		buffer[0]='\0';
		
		stream = popen(cmd.c_str(), "r");
		
		if(!stream)
		{
			return L"";
		}
		
		int ToDo; // what if this fails??
		
		while(fgets(buffer,MAX_BUFFER_SIZE,stream)!=NULL)
		{
			data.append(buffer);
		}
		pclose(stream);
		
		if (0==strlen(buffer))
		{
			return L"";
		}
		
		std::string tmp;
		tmp = trim(data,std::string(" \n\r"));
		data = tmp;
		
		std::string fullPath;
		fullPath =  "/Library/Frameworks/";
		fullPath += QtPart;
		fullPath += ".framework/Versions/";
		
		std::wstring wfullPath = wstring_From_string(fullPath);
		std::wstring wdata = wstring_From_string(data);
		wfullPath += wdata;
		
		return wfullPath;
	}
*/
	bool CheckQtFiles(Report_TYPE reportType)
	{
		int retVal = DIAGLIB_OK;
		bool bTmpPass = true;
		
		REP_PREFIX(L"other");
		for (size_t idx=0;idx<sizeof(FilesOther)/sizeof(wchar_t*);idx++)
		{
			REP_PREFIX(FilesOther[idx]);
			bool exist = false;
			retVal = fileExists(FilesOther[idx],&exist);
			if (DIAGLIB_OK != retVal)
			{
				// should not happen
			}
			std::wstringstream text;
			text << L"File ";
			if (!exist)
			{
				bTmpPass = false;
				text << L"not "; 
			}
			text << L"found: "; 
			text << FilesOther[idx] << L"";
			
			REP_CONTRIBUTE(L"exists",exist?L"true":L"false");
			
			if(exist)
			{
				std::wstring	sumstr;
				MD5Sum			sum;
				sum.add_file(string_From_wstring(FilesOther[idx]));
				sumstr=sum.get_sum();
				text << L" md5=[" << sumstr << L"]";
				REP_CONTRIBUTE(L"md5",sumstr);
			}
			
			resultToReport(reportType,text);
			REP_UNPREFIX();
		}
		REP_UNPREFIX();
		
/*		
		std::wstring fullPath;
		//------------------------------------------
		// check the QtGui file exists
		//------------------------------------------			
		
		std::string strAppFolder;
		strAppFolder = getAppPath();
		
		fullPath = getAppQtPath(strAppFolder,std::string("QtGui"));
		
		if(0==fullPath.length())
		{
			return false;
		}
		
		bool exist = false;
		retVal = fileExists(fullPath,&exist);
		if (DIAGLIB_OK != retVal)
		{
			// should not happen
		}
		std::wstringstream text;
		text << L"File ";
		if (!exist)
		{
			bTmpPass = false;
			text << L"not "; 
		}
		text << L"found: "; 
		text << fullPath << L"";

		if(!bTmpPass)
		{
			return bTmpPass;
		}
		resultToReport(reportType,text);
		
		fullPath = getAppQtPath(strAppFolder,std::string("QtCore"));
		
		if(0==fullPath.length())
		{
			return false;
		}
		
		exist = false;
		retVal = fileExists(fullPath,&exist);
		if (DIAGLIB_OK != retVal)
		{
			// should not happen
		}
		text.str(L"");
		text << L"File ";
		if (!exist)
		{
			bTmpPass = false;
			text << L"not "; 
		}
		text << L"found: "; 
		text << fullPath << L"";
		
		resultToReport(reportType,text);
*/
		return bTmpPass;
	}
#endif	
};

#endif
