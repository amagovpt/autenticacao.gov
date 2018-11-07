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
#include "LogBase.h"
#include "eidErrors.h"
#include "MWException.h"

#include <time.h>
#include <errno.h>
#include "Thread.h"
#include "Config.h"
#include "Util.h"

#ifdef WIN32
#include <windows.h>
#include <wchar.h>
#include <Shlobj.h>
#endif

#ifndef WIN32
#include "PCSC/wintypes.h"
#include "sys/stat.h"
#include "Util.h"

#define fwprintf_s fwprintf 
#define vfwprintf_s vfwprintf 
#define swprintf_s swprintf
#define _stat stat

#define LOG_DIRECTORY_DEFAULT  L"/tmp"
#endif

#define LOG_OPENFAILED_MAXALLOWED  10

namespace eIDMW
{

tLOG_Level MapLevel(const wchar_t *level)
{
	if(wcscmp(L"critical",level)==0)
		return LOG_LEVEL_CRITICAL;
	else if(wcscmp(L"error",level)==0)
		return LOG_LEVEL_ERROR;
	else if(wcscmp(L"warning",level)==0)
		return LOG_LEVEL_WARNING;
	else if(wcscmp(L"info",level)==0)
		return LOG_LEVEL_INFO;
	else if(wcscmp(L"debug",level)==0)
		return LOG_LEVEL_DEBUG;
	else if(wcscmp(L"none",level)==0)
		return LEV_LEVEL_NOLOG;
	else
		return LOG_LEVEL_DEFAULT;
}
/* ******************
*** CLogger Class ***
******************* */
//-----------------------------------------------
// The logger needs a systemwide mutex to control the access to the
// logfile. A systemwide mutex on Windows is a named mutex.
// For Linux/Mac we don't change the current code.
//-----------------------------------------------
#ifdef WIN32
HANDLE LogMutex;		// named mutex for Windows
#endif

static CMutex m_mutex;	// mutex for:
						//   - non-windows
						//   - used as automutex for creating a logger instance

std::auto_ptr<CLogger> CLogger::m_instance;
bool CLogger::m_bApplicationLeaving=false;

//Default constructor
CLogger::CLogger() 
{ 
	m_directory=L".";
	m_prefix=L"ZS";
	m_filesize=100000;
	m_filenr=2;
	m_groupinnewfile=false;
	m_maxlevel=LOG_LEVEL_DEFAULT;

	initFromConfig();
}

//Copy constructor
CLogger::CLogger(const CLogger &logger)				
{ 
	*this=logger;
}

CLogger &CLogger::operator= (const CLogger &logger)
{
	if(this!=&logger)
	{
		m_directory=logger.m_directory;
		m_prefix=logger.m_prefix;
		m_filesize=logger.m_filesize;
		m_filenr=logger.m_filenr; 
		m_maxlevel=logger.m_maxlevel;
		m_groupinnewfile=logger.m_groupinnewfile;
	}
	return *this;
}

//Destructor
CLogger::~CLogger()
{
	m_bApplicationLeaving=true;

	while(m_logStore.size()>0)
	{
		delete m_logStore[m_logStore.size()-1];
		m_logStore.pop_back();
	}
#ifdef WIN32
	//--------------------------------
	// Close the mutex handle. The last instance using the named mutex will
	// destroy the mutex
	//--------------------------------
	CloseHandle(LogMutex);
#endif
}

//Get the singleton instance of the logger
CLogger &CLogger::instance()
{
	if(m_bApplicationLeaving)
		throw CMWEXCEPTION(EIDMW_ERR_LOGGER_APPLEAVING);


    if (m_instance.get() == 0)
	{
#ifdef WIN32
		//----------------------------------------------
		// always create the logger mutex. Only the first time the named mutex is
		// used, it is created. After that it is equivalent to opening the mutex
		//----------------------------------------------
		LogMutex = CreateMutex(0, FALSE, L"LogMutex");
#endif
		CAutoMutex autoMutex(&m_mutex);
		m_instance.reset(new CLogger);
	}
    return *m_instance;
}

//Set the default values
void CLogger::init(const wchar_t *directory,const wchar_t *prefix,long filesize,long filenr,tLOG_Level maxlevel,bool groupinnewfile) 
{ 
	m_directory=directory;
	m_prefix=prefix;
	m_filesize=filesize;
	m_filenr=filenr; 
	m_maxlevel=maxlevel;
	m_groupinnewfile=groupinnewfile;
}

void CLogger::init(const char *directory,const char *prefix,long filesize,long filenr,tLOG_Level maxlevel,bool groupinnewfile) 
{ 
	init(utilStringWiden(directory).c_str(),utilStringWiden(prefix).c_str(),filesize,filenr,maxlevel,groupinnewfile);
}

#ifdef WIN32

/*
 Check for the presence of a pteid-debug.conf file in current user Desktop folder or %WINDIR%
*/
bool isWindowsDebugActive() {
	DWORD dwError = 0;
	PWSTR pszPath = NULL;
	DWORD dwAttr = 0;
	
	SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &pszPath);

	std::wstring debugFlag(pszPath);

	debugFlag += L"\\pteid-debug.conf";

	dwAttr = GetFileAttributes(debugFlag.c_str());
	bool is_windows_debug = dwAttr != INVALID_FILE_ATTRIBUTES;

	std::wstring debugFlag2(_wgetenv(L"WINDIR"));
	debugFlag2 += L"\\pteid-debug.conf";

	dwAttr = GetFileAttributes(debugFlag2.c_str());

	return (dwAttr != INVALID_FILE_ATTRIBUTES) || is_windows_debug;
}
#endif

//Set the default values
void CLogger::initFromConfig()
{
	CConfig config;

	std::wstring wcsLogDir = config.GetString(CConfig::EIDMW_CONFIG_PARAM_LOGGING_DIRNAME);
	std::wstring wcsPrefix = config.GetString(CConfig::EIDMW_CONFIG_PARAM_LOGGING_PREFIX);
	long lFileNbr = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER);
	long lFileSize = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILESIZE);
	std::wstring wcsMaxLevel = config.GetString(CConfig::EIDMW_CONFIG_PARAM_LOGGING_LEVEL);
	tLOG_Level maxLevel = MapLevel(wcsMaxLevel.c_str());
#ifdef WIN32
	if (isWindowsDebugActive())
	{
		maxLevel = LOG_LEVEL_DEBUG;
	}
#endif	
	long lGroup = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_LOGGING_GROUP);

	init(wcsLogDir.c_str(), wcsPrefix.c_str(), lFileSize, lFileNbr, maxLevel, (lGroup?true:false));
}

//Retrieve a CLog object by is group name
CLog &CLogger::getLogW(const wchar_t *group)
{
	bool find=false;
	unsigned int i;

	for(i=0;i<m_logStore.size();i++)
	{
		if(m_logStore[i]->m_group.compare(group)==0)
		{
			find=true;
			break;
		}
	}

	if(!find)
	{
		CLog *log = new CLog(m_directory.c_str(),m_prefix.c_str(),group,m_filesize,m_filenr,m_maxlevel,m_groupinnewfile);
		m_logStore.push_back(log);
		return *log;
	}

	return *m_logStore[i];
}

CLog &CLogger::getLogA(const char *group)
{
	return getLogW(utilStringWiden(group).c_str());
}

//Write into the log of the group
void CLogger::writeToGroup(const wchar_t *group,tLOG_Level level,const wchar_t *format, ...)
{
	CLog &log=getLogW(group);

	if(log.writeLineHeaderW(level))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLogger::writeToGroup(const char *group,tLOG_Level level,const char *format, ...)
{
	CLog &log=getLogA(group);

	if(log.writeLineHeaderA(level))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write into the log of the group with filename and line number
//use with __LINE__,__WFILE__
void CLogger::writeToGroup(const wchar_t *group,tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...)
{
	CLog &log=getLogW(group);

	if(log.writeLineHeaderW(level,line,file))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLogger::writeToGroup(const char *group,tLOG_Level level,const int line,const char *file,const char *format, ...)
{
	CLog &log=getLogA(group);

	if(log.writeLineHeaderA(level,line,file))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write into the default log (no group)
void CLogger::write(tLOG_Level level,const wchar_t *format, ...)
{
	CLog &log=getLogW(L"");

	if(log.writeLineHeaderW(level))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLogger::write(tLOG_Level level,const char *format, ...)
{
	CLog &log=getLogA("");

	if(log.writeLineHeaderA(level))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write into the default log (no group) with filename and line number
//use with  __LINE__,__WFILE__
void CLogger::write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...)
{
	CLog &log=getLogW(L"");

	if(log.writeLineHeaderW(level,line,file))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLogger::write(tLOG_Level level,const int line,const char *file,const char *format, ...)
{
	CLog &log=getLogA("");

	if(log.writeLineHeaderA(level,line,file))
	{
		va_list args;
		va_start(args, format);
		log.writeLineMessageA(format, args);
		va_end(args);
	}
}

/* ***************
*** CLog Class ***
**************** */
long CLog::m_sopenfailed = 0;

//PRIVATE: Default constructor
CLog::CLog(const wchar_t *directory,const wchar_t *prefix,const wchar_t *group,long filesize,long filenr,tLOG_Level maxlevel,bool groupinnewfile)	
{ 
	m_f=NULL;
	m_directory=directory;
	m_prefix=prefix;
	m_group=group;
	m_filesize=filesize;
	m_filenr=filenr; 
	m_maxlevel=maxlevel;
	m_groupinnewfile=groupinnewfile;
	m_openfailed=0;
}

//Copy constructor
CLog::CLog(const CLog &log)				
{ 
	*this=log;
}

CLog &CLog::operator= (const CLog &log)
{
	if(this!=&log)
	{
		m_f=log.m_f;
		m_directory=log.m_directory;
		m_prefix=log.m_prefix;
		m_group=log.m_group;
		m_filesize=log.m_filesize;
		m_filenr=log.m_filenr; 
		m_maxlevel=log.m_maxlevel;
		m_groupinnewfile=log.m_groupinnewfile;
		m_openfailed=log.m_openfailed;
	}
	return *this;
}

//Destructor
CLog::~CLog()
{
}

//PRIVATE: Return the name of to file to write into
void CLog::getFilename(std::wstring &filename)
{
	//Test if the directory exist
	std::wstring directory;

#ifdef WIN32
	DWORD dwError=0;
	directory=m_directory + L"\\";
	DWORD dwAttr = GetFileAttributes(directory.c_str());
    if(dwAttr == INVALID_FILE_ATTRIBUTES) dwError = GetLastError();
	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
	{
		m_directory=L".";
		directory=m_directory + L"\\";
	}
#else
	//	--> TODO : Test if the directory exist
	directory = m_directory;
	struct stat buffer;
	if ( stat(utilStringNarrow(directory).c_str(),&buffer)){
	  // check error code
	  m_directory=LOG_DIRECTORY_DEFAULT;
	} 
	directory=m_directory + L"/";
#endif

	//Initialize the root filename
	std::wstring root_filename;
	root_filename=directory + m_prefix + L"_";
	if(m_groupinnewfile && m_group.size()>0)
		root_filename+=m_group + L"_";

	wchar_t index[5];

	swprintf_s(index,5,L"%d",0);

	//If there is a maximal file size, 
	//  we parse the file from index 0 to m_filenr-1 
	//  until we find one -that doesn't exist 
	//                    or
	//                    -with a size smaller than m_filesize
	//  If we don't, we have to rename the files
	//
	//Else If there is only one file its index is 0
	if(m_filesize>0)
	{
		//There must be at least 2 files
		if(m_filenr<2)
			m_filenr=2;

		std::wstring file;
		struct _stat results;
		bool find=false;

		for(int i=0;i<m_filenr;i++)
		{

			swprintf_s(index,5,L"%d",i);

			file=root_filename + index + L".log";
#ifdef WIN32
			if (_wstat(file.c_str(), &results) != 0 || results.st_size<m_filesize)
#else
			if (stat(utilStringNarrow(file).c_str(), &results) != 0 || results.st_size<m_filesize)
#endif			
			{
				find=true;
				break;
			}
		}
		if(!find)
		{
			renameFiles(root_filename.c_str());
			swprintf_s(index,5,L"%d",m_filenr-1);
		}
	}

	filename=root_filename + index + L".log";

}

//PRIVATE: Delete file with index 0 et rename all file i to i-1 until m_filenr-1
//After this function, the file with index m_filenr-1 is free (it doesn't exist)
void CLog::renameFiles(const wchar_t *root_filename)
{
	//We remove the file 0
	std::wstring src;
	src=root_filename;
	src+=L"0.log";

#ifdef WIN32
	_wremove(src.c_str());
#else
	remove(utilStringNarrow(src).c_str());
#endif

	std::wstring dest;
	wchar_t isrc[5];
	wchar_t idest[5];
	struct _stat results;

	//For all file until m_filenr-1
	// 1 become 0
	// 2 become 1
	//i+1 become i
	//m_filenr-1 become m_filenr-2
	for(int i=0;i<m_filenr;i++)
	{
		swprintf_s(isrc,5,L"%d",i+1);
		swprintf_s(idest,5,L"%d",i);

		//if the source does not exist, we stop
		src=root_filename;
		src+=isrc;
		src+=L".log";

#ifdef WIN32
		if (_wstat(src.c_str(), &results) != 0)
			break;
#else
		if (stat(utilStringNarrow(src).c_str(), &results) != 0)
			break;
#endif
		dest=root_filename;
		dest+=idest;
		dest+=L".log";

		//Rename of the file
#ifdef WIN32
		_wrename(src.c_str(),dest.c_str());
#else
		rename(utilStringNarrow(src).c_str(),utilStringNarrow(dest).c_str());
#endif
	}
}

//PRIVATE: Open the file with the correct name
bool CLog::open(bool bWchar)
{
	if(!canWeTryToOpen())
	{
		incrementOpenFailed();
		return false;		
	}

#ifdef WIN32
	WaitForSingleObject(LogMutex, INFINITE);
#else
	m_mutex.Lock();
#endif

	if(m_f)	//Should not happend
	{
		close();
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
	}

#ifndef WIN32
	m_flock.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	m_flock.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	m_flock.l_start  = 0;        /* Offset from l_whence         */
	m_flock.l_len    = 0;        /* length, 0 = to EOF           */
	m_flock.l_pid    = getpid(); /* our PID                      */
#endif

	int err=0;
	std::wstring filename;

	int iLoop=0;
	do	//If the file is locked by another process, we wait
	{
		getFilename(filename); //We get the file name in the loop because other process may rename the files

#ifdef WIN32
		if(bWchar)
			err  = _wfopen_s(&m_f,filename.c_str(),L"a, ccs=UTF-8");
		else
			err  = fopen_s(&m_f,utilStringNarrow(filename).c_str(),"a");
#else
		m_f = fopen(utilStringNarrow(filename).c_str(),"a, ccs=UTF-8");
		if (m_f == NULL) err=errno;	
#endif

		if (err != 0 && err != EACCES) 
			m_f=NULL;

		if(err==EACCES) 
			CThread::SleepMillisecs(20);

		iLoop++;

	} while(err==EACCES && iLoop<100);

#ifndef WIN32
	// on Linux/Mac we set an advisory lock, i.e. it prevents
	// other processes from using the file only if they are collaborative 
	// and check for the lock, otherwise they can do whatever they like ..
	if(m_f!=NULL)
	{
		if( fcntl(fileno(m_f), F_SETLK, &m_flock) == -1)  /* set the lock */
		{
		  fclose(m_f);
		  m_f=NULL;
		}
	}
#endif

	if(!m_f)
	{
#ifdef WIN32
		ReleaseMutex(LogMutex);
#else
		m_mutex.Unlock();
#endif
		incrementOpenFailed();
		return false;
	}

	resetOpenFailed();
	return true;
}

//PRIVATE: Close the file
inline void  CLog::close()
{
	if(!m_f)
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);

#ifndef WIN32
	m_flock.l_type   = F_UNLCK;  /* tell it to unlock the region */
		
	if( fcntl(fileno(m_f), F_SETLK, &m_flock) == -1) /* set the region to unlocked */
		throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
#endif

	fclose(m_f);

	m_f=NULL;

#ifdef WIN32
	ReleaseMutex(LogMutex);
#else
	m_mutex.Unlock();
#endif
}

//PRIVATE: Convert the enum into message
const wchar_t *CLog::getLevel(tLOG_Level level)
{
	switch(level)
	{
		case LOG_LEVEL_CRITICAL:
			return L"CRITICAL";
		case LOG_LEVEL_ERROR:
			return L"ERROR";
		case LOG_LEVEL_WARNING:
			return L"WARNING";
		case LOG_LEVEL_INFO:
			return L"INFO";
		case LOG_LEVEL_DEBUG:
			return L"DEBUG";
		default:
			return getLevel(LOG_LEVEL_DEFAULT);
	}

}

//PRIVATE: Get local time in format YYYY-MM-DD hh:mm:ss by default
void CLog::getLocalTimeW(std::wstring &timestamp, const wchar_t *format)
{
	time_t rawtime;
	struct tm timeinfo;
	wchar_t buffer [20];

	time ( &rawtime );
#ifdef WIN32
	localtime_s( &timeinfo, &rawtime );
#else
	timeinfo = *(localtime(&rawtime));
#endif

	wcsftime (buffer,20,format,&timeinfo);

	timestamp.assign(buffer);
}

void CLog::getLocalTimeA(std::string &timestamp, const char *format)
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer [20];

	time ( &rawtime );
#ifdef WIN32
	localtime_s( &timeinfo, &rawtime );
#else
	timeinfo = *(localtime(&rawtime));
#endif

	strftime (buffer,20,format,&timeinfo);

	timestamp.assign(buffer);
}

//Write to log from a variable number of parameter
void CLog::write(tLOG_Level level,const wchar_t *format, ...)
{
	if(writeLineHeaderW(level))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::write(tLOG_Level level,const char *format, ...)
{
	if(writeLineHeaderA(level))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write to log from a va_list
void CLog::write(tLOG_Level level,const wchar_t *format, va_list args)
{
	if (writeLineHeaderW(level))
		writeLineMessageW(format, args);
}

void CLog::write(tLOG_Level level,const char *format, va_list args)
{
	if (writeLineHeaderA(level))
		writeLineMessageA(format, args);
}

//Write to log from a variable number of parameter with filename and line number
void CLog::write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...)
{
	if(writeLineHeaderW(level,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::write(tLOG_Level level,const int line,const char *file,const char *format, ...)
{
	if(writeLineHeaderA(level,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write to log from a va_list with filename and line number
void CLog::write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, va_list args)
{
	if (writeLineHeaderW(level,line,file))
		writeLineMessageW(format, args);
}

void CLog::write(tLOG_Level level,const int line,const char *file,const char *format, va_list args)
{
	if (writeLineHeaderA(level,line,file))
		writeLineMessageA(format, args);
}



//ATTENTION : Design for use with macro
//            Must be follow by writeLineMessage to close the file
//Write to log the first part of the line
bool CLog::writeLineHeaderW(tLOG_Level level,const int line,const wchar_t *file)
{

 	if(level>m_maxlevel)
		return false;

	long lPreviousOpenFailed = getOpenFailed();

	if(!open(true))
		return false;

	std::wstring timestamp;
	getLocalTimeW(timestamp);	
	
	if(lPreviousOpenFailed > 0)
	{
		if(isFileMixingGroups())
		{
			fwprintf_s(m_f,L"%ls - %ld - %ls: ...ERROR: This file could not be opened. %ld logging line(s) are missing...\n",timestamp.c_str(),CThread::getCurrentPid(),m_group.c_str(),lPreviousOpenFailed);
		}
		else
		{
			fwprintf_s(m_f,L"%ls - %ld: ...ERROR: This file could not be opened. %ld logging line(s) are missing...\n",timestamp.c_str(),CThread::getCurrentPid(),lPreviousOpenFailed);
		}
	}

	if(isFileMixingGroups())
	{
		if(line>0 && wcslen(file)>0)
			fwprintf_s(m_f,L"%ls - %ld|%ld - %ls - %ls -'%ls'-line=%d: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),m_group.c_str(),getLevel(level),file,line);
		else
			fwprintf_s(m_f,L"%ls - %ld|%ld - %ls - %ls: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),m_group.c_str(),getLevel(level));
	}
	else
	{
		if(line>0 && wcslen(file)>0)
			fwprintf_s(m_f,L"%ls - %ld|%ld - %ls -'%ls'-line=%d: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),getLevel(level),file,line);
		else
			fwprintf_s(m_f,L"%ls - %ld|%ld - %ls: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),getLevel(level));
	}

	return true;

}

bool CLog::writeLineHeaderA(tLOG_Level level_in,const int line,const char *file)
{

 	if(level_in>m_maxlevel)
		return false;
	
	long lPreviousOpenFailed = getOpenFailed();

	if(!open(false))
		return false;

	std::string timestamp;
	getLocalTimeA(timestamp);

	if(lPreviousOpenFailed > 0)
	{
		if(isFileMixingGroups())
		{
			fprintf_s(m_f,"%s - %ld - %s: ...ERROR: This file could not be opened. %ld logging line(s) are missing...\n",timestamp.c_str(),CThread::getCurrentPid(),m_group.c_str(),lPreviousOpenFailed);
		}
		else
		{
			fprintf_s(m_f,"%s - %ld: ...ERROR: This file could not be opened. %ld logging line(s) are missing...\n",timestamp.c_str(),CThread::getCurrentPid(),lPreviousOpenFailed);
		}
	}

	std::string level=utilStringNarrow(getLevel(level_in));

	if(isFileMixingGroups())
	{
		std::string group=utilStringNarrow(m_group);

		if(line>0 && strlen(file)>0)
			fprintf_s(m_f,"%s - %ld|%ld - %s - %s -'%s'-line=%d: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),group.c_str(),level.c_str(),file,line);
		else
			fprintf_s(m_f,"%s - %ld|%ld - %s - %s: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),group.c_str(),level.c_str());
	}
	else
	{
		if(line>0 && strlen(file)>0)
			fprintf_s(m_f,"%s - %ld|%ld - %s -'%s'-line=%d: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),level.c_str(),file,line);
		else
			fprintf_s(m_f,"%s - %ld|%ld - %s: ",timestamp.c_str(),CThread::getCurrentPid(),CThread::getCurrentThreadId(),level.c_str());
	}

	return true;

}
//ATTENTION : Design for use with macro
//            Must be preceded by writeLineHeader to close the file
//Write to log the second part of the line
bool CLog::writeLineMessageW(const wchar_t *format, ...)
{
 	if(!m_f)	//Should not happend, as this method must only be called if the writeLineHeader succeed
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);

	va_list args;
	va_start(args, format);
	writeLineMessageW(format, args);
	va_end(args);

	return true;
}

bool CLog::writeLineMessageA(const char *format, ...)
{
 	if(!m_f)	//Should not happend, as this method must only be called if the writeLineHeader succeed
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);

	va_list args;
	va_start(args, format);
	writeLineMessageA(format, args);
	va_end(args);

	return true;
}

//PRIVATE
//Write to log the second part of the line

void CLog::writeLineMessageW(const wchar_t *format, va_list argList)
{

 	if(!m_f)	//Should not happend, as this method must only be called if the writeLineHeader succeed
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);

	vfwprintf_s(m_f, format, argList);
	fwprintf_s(m_f,L"%c",'\n');
	close();
}

void CLog::writeLineMessageA(const char *format, va_list argList)
{

 	if(!m_f)	//Should not happend, as this method must only be called if the writeLineHeader succeed
		throw CMWEXCEPTION(EIDMW_FILE_NOT_OPENED);

	vfprintf_s(m_f, format, argList);
	fprintf_s(m_f,"%c",'\n');
	close();
}

//Write Critical level to log
void CLog::writeCritical(const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_CRITICAL))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeCritical(const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_CRITICAL))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Critical level to log with filename and line number
void CLog::writeCritical(const int line,const wchar_t *file,const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_CRITICAL,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeCritical(const int line,const char *file,const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_CRITICAL,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Error level to log
void CLog::writeError(const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_ERROR))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}

}

void CLog::writeError(const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_ERROR))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}

}
//Write Error level to log with filename and line number
void CLog::writeError(const int line,const wchar_t *file,const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_ERROR,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeError(const int line,const char *file,const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_ERROR,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Warning level to log
void CLog::writeWarning(const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_WARNING))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeWarning(const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_WARNING))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Warning level to log with filename and line number
void CLog::writeWarning(const int line,const wchar_t *file,const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_WARNING,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeWarning(const int line,const char *file,const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_WARNING,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Info level to log
void CLog::writeInfo(const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_INFO))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeInfo(const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_INFO))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Info level to log with filename and line number
void CLog::writeInfo(const int line,const wchar_t *file,const wchar_t *format, ...)
{
	if(writeLineHeaderW(LOG_LEVEL_INFO,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeInfo(const int line,const char *file,const char *format, ...)
{
	if(writeLineHeaderA(LOG_LEVEL_INFO,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Debug level to log
void CLog::writeDebug(const wchar_t *format, ...)
{
//    Beeps(1, 1000);
	if(writeLineHeaderW(LOG_LEVEL_DEBUG))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeDebug(const char *format, ...)
{
//    Beeps(1, 1000);
	if(writeLineHeaderA(LOG_LEVEL_DEBUG))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

//Write Debug level to log with filename and line number
void CLog::writeDebug(const int line,const wchar_t *file,const wchar_t *format, ...)
{

	if(writeLineHeaderW(LOG_LEVEL_DEBUG,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageW(format, args);
		va_end(args);
	}
}

void CLog::writeDebug(const int line,const char *file,const char *format, ...)
{

	if(writeLineHeaderA(LOG_LEVEL_DEBUG,line,file))
	{
		va_list args;
		va_start(args, format);
		writeLineMessageA(format, args);
		va_end(args);
	}
}

bool CLog::isFileMixingGroups()
{
	return (!m_groupinnewfile || m_group.size()==0);
}

long CLog::getOpenFailed()
{
	if(isFileMixingGroups())
		return m_sopenfailed;
	else
		return m_openfailed;
}

bool CLog::canWeTryToOpen()
{
	//To avoid delay if open failed 5 times consecutively,
	//we retry only once every 100 times
	if(isFileMixingGroups())
		return (m_sopenfailed <= 5 || (m_sopenfailed % 100) == 0);
	else
		return (m_openfailed <= 5 || (m_openfailed % 100) == 0);
}

void CLog::incrementOpenFailed()
{
	if(isFileMixingGroups())
		m_sopenfailed++;
	else
		m_openfailed++;
}

void CLog::resetOpenFailed()
{
	if(isFileMixingGroups())
		m_sopenfailed=0;
	else
		m_openfailed=0;
}

}
