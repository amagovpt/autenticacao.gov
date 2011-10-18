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
/*
This file contains 2 classes for log purpose : CLogger and CLog

CLOGGER
-------
CLogger is a singleton that keep a vector of CLog (one log by group).

Instance : -The instance() method give access to the singleton object

Initialization : -The CLogger may be init. 
                 -If it isn't, the default parameters are used.
                 -The init parameters are transfered to CLog object when the CLogger create them.
                 -The init may be call anywhere to change the parameters 
				 but these changes only affect the future created CLog

Retreive a Log : -Use the getLog() method with the name of the group as argument

CLOG
----
Each CLog represents a set of log file. (One set by group) 
The constructor is not enabled but objects are created by the logger when you ask for a new group.
The files are closed after each write.

PARAMETERS
----------
You pass the parameters with the init method of the CLogger and it passes them to CLog at creation.

These parameters are :
*Directory : -The folder in which the log files are created. 
             -If it doesn't exist, the current directory is used.
*Prefix :	 -Use to define the name of the files.
			 -The name is <Prefix>_[<Group>_]Index.log (Index begins to 0 until FileNr-1)
*FileSize :  -The maximum size of the log file.
			 -0 means no limit
			 -When the file is full, we use a new file with the next index.
*FileNr :	 -The maximum number of files (by group)
			 -If the FileSize is 0, this parameter is not used.
			 -The minimum value is 2
			 -When the last file (Index=FileNr-1) is full, the file with index 0 is removed,
			 the File(1) is renamed to File(0), File(i) to File(i-1), 
			 then File(FileNr-1) become free again.

*GroupInNewFile :	-If true, new log files are made for each group
					-If false, the name of the group (if any) appears on each entry line

USAGE
-----
In complement of usual object usage, some macros are defined to give simple access to methods.
These macros are define below in this file.
You will also find macro template to enable easy use of group. Copy these macros and customize them for your own purpose.

Example of usage through objects and macro are give at the end of this file
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Mutex.h"

#ifndef __WFILE__
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#endif

#ifndef WIN32
#include <sys/file.h>
#endif

#include <stdarg.h>

namespace eIDMW
{

class CLog; //define below

typedef enum {
    LEV_LEVEL_NOLOG,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} tLOG_Level;

#define LOG_LEVEL_DEFAULT LOG_LEVEL_ERROR

class CLogger
{
  public:
    EIDMW_CMN_API static CLogger &instance();
    EIDMW_CMN_API ~CLogger();
 
  private:
    CLogger();
	CLogger(const CLogger &logger);
	CLogger &operator= (const CLogger &logger);

  public:
	EIDMW_CMN_API void initFromConfig();
	EIDMW_CMN_API void init(const wchar_t *directory,const wchar_t *prefix,long filesize,long filenr,tLOG_Level minlevel,bool groupinnewfile); 
	EIDMW_CMN_API void init(const char *directory,const char *prefix,long filesize,long filenr,tLOG_Level minlevel,bool groupinnewfile); 
	EIDMW_CMN_API CLog &getLogW(const wchar_t *group=L"");
	EIDMW_CMN_API CLog &getLogA(const char *group="");
	EIDMW_CMN_API void writeToGroup(const wchar_t *group,tLOG_Level level,const wchar_t *format, ...);
	EIDMW_CMN_API void writeToGroup(const char *group,tLOG_Level level,const char *format, ...);
	EIDMW_CMN_API void writeToGroup(const wchar_t *group,tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeToGroup(const char *group,tLOG_Level level,const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const wchar_t *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const char *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const char *file,const char *format, ...);

  private:
    static std::auto_ptr<CLogger> m_instance;
    static bool m_bApplicationLeaving;

	std::wstring m_directory;
	std::wstring m_prefix;
	long m_filesize;
	long m_filenr;
	tLOG_Level m_maxlevel;
	bool m_groupinnewfile;

	std::vector <CLog*> m_logStore;
};

class CLog
{
  public:
    EIDMW_CMN_API ~CLog();
 
  private:
    CLog(const wchar_t *directory,const wchar_t *prefix,const wchar_t *group,long filesize,long filenr,tLOG_Level minlevel,bool groupinnewfile);	
	CLog(const CLog &log);				
	CLog &  operator= (const CLog &) ;

  public:
	EIDMW_CMN_API void write(tLOG_Level level,const wchar_t *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const char *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const wchar_t *format, va_list args);
	EIDMW_CMN_API void write(tLOG_Level level,const char *format, va_list args);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const wchar_t *file,const wchar_t *format, va_list args);
	EIDMW_CMN_API void write(tLOG_Level level,const int line,const char *file,const char *format, va_list args);

	EIDMW_CMN_API void writeCritical(const wchar_t *format, ...);
	EIDMW_CMN_API void writeCritical(const char *format, ...);
	EIDMW_CMN_API void writeCritical(const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeCritical(const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void writeError(const wchar_t *format, ...);
	EIDMW_CMN_API void writeError(const char *format, ...);
	EIDMW_CMN_API void writeError(const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeError(const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void writeWarning(const wchar_t *format, ...);
	EIDMW_CMN_API void writeWarning(const char *format, ...);
	EIDMW_CMN_API void writeWarning(const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeWarning(const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void writeInfo(const wchar_t *format, ...);
	EIDMW_CMN_API void writeInfo(const char *format, ...);
	EIDMW_CMN_API void writeInfo(const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeInfo(const int line,const char *file,const char *format, ...);
	EIDMW_CMN_API void writeDebug(const wchar_t *format, ...);
	EIDMW_CMN_API void writeDebug(const char *format, ...);
	EIDMW_CMN_API void writeDebug(const int line,const wchar_t *file,const wchar_t *format, ...);
	EIDMW_CMN_API void writeDebug(const int line,const char *file,const char *format, ...);

	//USE ONLY IN MACRO
	EIDMW_CMN_API bool writeLineHeaderW(tLOG_Level level,const int line=0,const wchar_t *file=L"");
	EIDMW_CMN_API bool writeLineHeaderA(tLOG_Level level,const int line=0,const char *file="");
	EIDMW_CMN_API bool writeLineMessageW(const wchar_t *format, ...);
	EIDMW_CMN_API bool writeLineMessageA(const char *format, ...);

  friend class CLogger;

private:
	void getFilename(std::wstring &filename);
	void renameFiles(const wchar_t *root_filename);
	bool open(bool bWchar);
	void close();
	void writeLineMessageW(const wchar_t *format, va_list argList);
	void writeLineMessageA(const char *format, va_list argList);
	const wchar_t *getLevel(tLOG_Level level);
	void getLocalTimeW(std::wstring &timestamp, const wchar_t *format=L"%Y-%m-%d %H:%M:%S");
	void getLocalTimeA(std::string &timestamp, const char *format="%Y-%m-%d %H:%M:%S");

	bool isFileMixingGroups();
	long getOpenFailed();
	bool canWeTryToOpen();
	void incrementOpenFailed();
	void resetOpenFailed();

	std::wstring m_directory;
	std::wstring m_prefix;
	std::wstring m_group;
	long m_filesize;
	long m_filenr;
	tLOG_Level m_maxlevel;
	bool m_groupinnewfile;
	long m_openfailed;
	static long m_sopenfailed;

	FILE *m_f;

#ifndef WIN32
	struct flock m_flock;
#endif
};

//SHORTCUT MACRO
//General use
#define ZS_LOG_INIT			CLogger::instance().init
#define ZS_LOG				CLogger::instance().write
#define ZS_LOG_GROUP		CLogger::instance().writeToGroup

//Defining a level
#define ZS_LOG_CRITICAL		CLogger::instance().getLogW(L"").writeCritical
#define ZS_LOG_ERROR		CLogger::instance().getLogW(L"").writeError
#define ZS_LOG_WARNING		CLogger::instance().getLogW(L"").writeWarning
#define ZS_LOG_INFO			CLogger::instance().getLogW(L"").writeInfo
#define ZS_LOG_DEBUG		CLogger::instance().getLogW(L"").writeDebug

//Defining a level and trace file and line
#define ZS_LOG_CRITICAL_TRACE	!CLogger::instance().getLogW(L"").writeLineHeaderW(LOG_LEVEL_CRITICAL,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"").writeLineMessageW
#define ZS_LOG_ERROR_TRACE		!CLogger::instance().getLogW(L"").writeLineHeaderW(LOG_LEVEL_ERROR,__LINE__,__WFILE__)		? 0 : CLogger::instance().getLogW(L"").writeLineMessageW
#define ZS_LOG_WARNING_TRACE	!CLogger::instance().getLogW(L"").writeLineHeaderW(LOG_LEVEL_WARNING,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"").writeLineMessageW
#define ZS_LOG_INFO_TRACE		!CLogger::instance().getLogW(L"").writeLineHeaderW(LOG_LEVEL_INFO,__LINE__,__WFILE__)		? 0 : CLogger::instance().getLogW(L"").writeLineMessageW
#define ZS_LOG_DEBUG_TRACE		!CLogger::instance().getLogW(L"").writeLineHeaderW(LOG_LEVEL_DEBUG,__LINE__,__WFILE__)		? 0 : CLogger::instance().getLogW(L"").writeLineMessageW

//MACRO EXTENSION
//Example of extanding to a groupe named Group1
#define ZS_LOG_GROUP1			CLogger::instance().getLogW(L"Group1").write

#define ZS_LOG_GROUP1_CRITICAL	CLogger::instance().getLogW(L"Group1").writeCritical
#define ZS_LOG_GROUP1_ERROR		CLogger::instance().getLogW(L"Group1").writeError
#define ZS_LOG_GROUP1_WARNING	CLogger::instance().getLogW(L"Group1").writeWarning
#define ZS_LOG_GROUP1_INFO		CLogger::instance().getLogW(L"Group1").writeInfo
#define ZS_LOG_GROUP1_DEBUG		CLogger::instance().getLogW(L"Group1").writeDebug

#define ZS_LOG_GROUP1_CRITICAL_TRACE	!CLogger::instance().getLogW(L"Group1").writeLineHeaderW(LOG_LEVEL_CRITICAL,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"Group1").writeLineMessageW
#define ZS_LOG_GROUP1_ERROR_TRACE		!CLogger::instance().getLogW(L"Group1").writeLineHeaderW(LOG_LEVEL_ERROR,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"Group1").writeLineMessageW
#define ZS_LOG_GROUP1_WARNING_TRACE		!CLogger::instance().getLogW(L"Group1").writeLineHeaderW(LOG_LEVEL_WARNING,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"Group1").writeLineMessageW
#define ZS_LOG_GROUP1_INFO_TRACE		!CLogger::instance().getLogW(L"Group1").writeLineHeaderW(LOG_LEVEL_INFO,__LINE__,__WFILE__)		? 0 : CLogger::instance().getLogW(L"Group1").writeLineMessageW
#define ZS_LOG_GROUP1_DEBUG_TRACE		!CLogger::instance().getLogW(L"Group1").writeLineHeaderW(LOG_LEVEL_DEBUG,__LINE__,__WFILE__)	? 0 : CLogger::instance().getLogW(L"Group1").writeLineMessageW

}

/*
*************
*** USAGE ***
*************
	int i=1;
//INITIALIZING THE LOGGER
//-----------------------
//Object use
	CLogger &logger = CLogger::instance();
	logger.init(L"C:\\Tmp", L"Test", 100000, 5, true);

//Or
	CLogger::instance().init(L"C:\\Tmp", L"Test", 100000, 5, true);

//Macro
	ZS_LOG_INIT(L"C:\\Tmp", L"Test", 100000, 5, true);

//SIMPLE EXAMPLE (NO GROUP, NO FILE TRACE)
//----------------------------------------
//Object use
	CLog &logA=logger.getLogW();
	logA.write(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"First test");

//Or
	logA.writeError(L"Example %d: %s", i++, L"First test");

//Or
	logger.write(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"First test");

//Or
	CLogger::instance().write(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"First test");

//Macro
	ZS_LOG(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"First test");

//Or
	ZS_LOG_ERROR(L"Example %d: %s", i++, L"First test");

//SIMPLE EXAMPLE WITH FILE TRACE (NO GROUP)
//----------------------------------------
//Object use
	CLog &logB=logger.getLogW();
	logB.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Second test");

//Or
	logB.writeError(__LINE__,__WFILE__,L"Example %d: %s", i++, L"Second test");

//Or
	logger.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Second test");

//Or
	CLogger::instance().write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Second test");

//Macro
	ZS_LOG(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Second test");

//Or
	ZS_LOG_ERROR_TRACE(L"Example %d: %s", i++, L"Second test");

//EXAMPLE WITH GROUP (NO FILE TRACE)
//----------------------------------
//Object use
	CLog &logGroup1A=logger.getLog(L"Group1");
	logGroup1A.write(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"Third test");

//Or
	logGroup1A.writeError(L"Example %d: %s", i++, L"Third test");

//Or
	logger.writeToGroup(L"Group1",LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"Third test");

//Or
	CLogger::instance().writeToGroup(L"Group1",LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"Third test");

//Macro
	ZS_LOG_GROUP(L"Group1",LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"Third test");

//Or
	ZS_LOG_GROUP1(LOG_LEVEL_ERROR,L"Example %d: %s", i++, L"Third test");

//Or
	ZS_LOG_GROUP1_ERROR(L"Example %d: %s", i++, L"Third test");

//EXAMPLE WITH GROUP AND FILE TRACE
//---------------------------------
//Object use
	CLog &logGroup1B=logger.getLog(L"Group1");
	logGroup1B.write(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Or
	logGroup1B.writeError(__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Or
	logger.writeToGroup(L"Group1",LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Or
	CLogger::instance().writeToGroup(L"Group1",LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Macro
	ZS_LOG_GROUP(L"Group1",LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Or
	ZS_LOG_GROUP1(LOG_LEVEL_ERROR,__LINE__,__WFILE__,L"Example %d: %s", i++, L"Fourth test");

//Or
	ZS_LOG_GROUP1_ERROR_TRACE(L"Example %d: %s", i++, L"Fourth test");

*/
