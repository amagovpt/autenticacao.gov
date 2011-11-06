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
#ifdef WIN32
#include <windows.h>
#elif defined __APPLE__
#include "Mac/mac_helper.h"
#endif

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "diaglib.h"
#include "report.h"
#include "file.h"

#include "error.h"
#include "log.h"
#include "folder.h"
#include "repository.h"

#define REPORT_TOTAL L"diagreport.txt"
#define REPORT_ABSTRACT L"diagabstract.txt"
#define REPORT_OPEN_ATTEMPT_COUNT 5

//#define FILE_ENCODING L", ccs=UTF-8" 
#define FILE_ENCODING L"" 

#define REPORT_WIDTH		80

static std::wstring g_wsReportTotal;
static std::wstring g_wsReportAbstract;
static std::wstring g_wsReportLevel1;
static std::wstring g_wsReportLevel2;
static std::wstring g_wsReportLevel3;

static FILE *g_pfile_level1=NULL;
static FILE *g_pfile_level2=NULL;
static FILE *g_pfile_level3=NULL;
static bool g_reportFirstOpen=true;

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
FILE *getFile(Report_TYPE type);
int reportPrintFileSeparator(Report_TYPE type);
int reportLoadResources();

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int reportGetFileName(const wchar_t **fileName, bool bAbstractOnly)
{
	int iReturnCode=DIAGLIB_OK;

	if(fileName==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;	
	}

	if(bAbstractOnly)
	{
		if(g_wsReportAbstract.empty())
		{
			return RETURN_LOG_BAD_FUNCTION_CALL; //First make a Finalize
		}

		*fileName=g_wsReportAbstract.c_str();
	}
	else
	{
		if(g_wsReportTotal.empty())
		{
			return RETURN_LOG_BAD_FUNCTION_CALL; //First make a Finalize
		}

		*fileName=g_wsReportTotal.c_str();
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportInit()
{
	int iReturnCode = DIAGLIB_OK;

	//Initialize g_wsReportResult, g_wsReportMain and g_wsReportComplement
	if(g_wsReportLevel1.empty())
	{
		if(DIAGLIB_OK != folderGetTempFileName(&g_wsReportLevel1))
		{
			return RETURN_LOG_INTERNAL_ERROR;
		}
	}
	if(g_wsReportLevel2.empty())
	{
		if(DIAGLIB_OK != folderGetTempFileName(&g_wsReportLevel2))
		{
			return RETURN_LOG_INTERNAL_ERROR;
		}
	}
	if(g_wsReportLevel3.empty())
	{
		if(DIAGLIB_OK != folderGetTempFileName(&g_wsReportLevel3))
		{
			return RETURN_LOG_INTERNAL_ERROR;
		}
	}
	
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportLoadResources()
{
	int iReturnCode=DIAGLIB_OK;
	
	//File already open
	if(g_pfile_level1 != NULL && g_pfile_level2 != NULL && g_pfile_level3 != NULL)
	{
		return DIAGLIB_OK;	
	}

	if(g_wsReportLevel1.empty() || g_wsReportLevel2.empty() || g_wsReportLevel3.empty()) 
	{
		reportInit();
	}
		
	errno_t err=0;
	
	//Create the files (Erase previous file)
	if (!g_pfile_level1)
	{
		for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
		{
			err = _wfopen_s(&g_pfile_level1, g_wsReportLevel1.c_str(), (g_reportFirstOpen?L"w" FILE_ENCODING:L"a" FILE_ENCODING));
			if (!g_pfile_level1)
			{
				LOG_ERRORCODE(L"_wfopen_s failed",err);
				return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
			}
			if (g_pfile_level1)
				break;
			else
				Sleep(50);
		}
	}
	if(g_pfile_level1 && g_reportFirstOpen)
	{
#ifdef WIN32
		fwprintf_s(g_pfile_level1,L"RUNNING eDT VERSION: %hs\n",diaglibVersion());
#elif __APPLE__
		fwprintf_s(g_pfile_level1,L"RUNNING eDT VERSION: %s\n",diaglibVersion());
#endif	
	}
	
	if (!g_pfile_level2)
	{
		for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
		{
			err = _wfopen_s(&g_pfile_level2, g_wsReportLevel2.c_str(), (g_reportFirstOpen?L"w" FILE_ENCODING:L"a" FILE_ENCODING));
			if (!g_pfile_level2)
			{
				LOG_ERRORCODE(L"_wfopen_s failed",err);
				return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
			}
			if (g_pfile_level2)
				break;
			else
				Sleep(50);
		}
	}
	
	if (!g_pfile_level3)
	{
		for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
		{
			err = _wfopen_s(&g_pfile_level3, g_wsReportLevel3.c_str(), (g_reportFirstOpen?L"w" FILE_ENCODING:L"a" FILE_ENCODING));
			if (!g_pfile_level3)
			{
				LOG_ERRORCODE(L"_wfopen_s failed",err);
				return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
			}
			if (g_pfile_level3)
				break;
			else
				Sleep(50);
		}
	}
	
	g_reportFirstOpen = false;
	
	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintLine(Report_TYPE type, const wchar_t *text,wchar_t c, int uppercase, int padding)
{
	int iReturnCode=DIAGLIB_OK;

	FILE *pfile;
	if(NULL == (pfile=getFile(type)))
		return RETURN_LOG_INTERNAL_ERROR;

	unsigned long i=0;
	size_t iMaxSep=0;
	size_t iMaxPad=0;
	size_t iLen=0;

	wchar_t *buff=NULL;

	buff= new wchar_t[wcslen(text)+1];
	wcscpy_s(buff,wcslen(text)+1,text);

	if(uppercase)
	{
		for(i=0;i<wcslen(text);i++)
			buff[i]=toupper(buff[i]);
	}

	iLen=wcslen(buff);
	if(iLen)
		iLen+=2;

	//Print in File
	if(REPORT_WIDTH<iLen)
	{
		iMaxSep=0;
		iMaxPad=0;
	}
	else if(padding>0 && REPORT_WIDTH >= iLen + 2*padding)
	{
		iMaxPad=padding;
		iMaxSep=(REPORT_WIDTH - iLen) / 2 - iMaxPad;
	}
	else
	{
		size_t iLeft=(REPORT_WIDTH - iLen) / 2;
		iMaxPad=(padding*2*iLeft)/REPORT_WIDTH;
		iMaxSep=(REPORT_WIDTH - iLen) / 2 - iMaxPad;
	}

	for(i=0;i<iMaxSep;i++) fwprintf_s(pfile,L"%lc",c);
	for(i=0;i<iMaxPad;i++) fwprintf_s(pfile,L" ");

	if(iLen)
		fwprintf_s(pfile,L" %ls ",buff);

	for(i=0;i<iMaxPad;i++) fwprintf_s(pfile,L" ");
	iMaxSep=REPORT_WIDTH<iLen?0:REPORT_WIDTH - iLen - iMaxSep - 2*iMaxPad;
	for(i=0;i<iMaxSep;i++) fwprintf_s(pfile,L"%lc",c);

	fwprintf_s(pfile,L"\n");

	delete[] buff;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintHex(Report_TYPE type, const wchar_t *title, const unsigned char *buffer, unsigned long len, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	FILE *pfile;
	if(NULL == (pfile=getFile(type)))
		return RETURN_LOG_INTERNAL_ERROR;

	reportPrintTitle1(type, title, sep);

	unsigned long i=0;
	unsigned long line=0;

	for(i=0,line=0;i<len;i++,line++)
	{
		if(line==REPORT_WIDTH/3)
		{
			fwprintf_s(pfile,L"\n");
			line=0;
		}

		fwprintf_s(pfile,L"%02x ",buffer[i]);
	}

	fwprintf_s(pfile,L"\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrint(Report_TYPE type, const wchar_t *format, ...)
{
	int iReturnCode=DIAGLIB_OK;

	FILE *pfile;
	if(NULL == (pfile=getFile(type)))
		return RETURN_LOG_INTERNAL_ERROR;

	va_list args;
	va_start(args, format);
	
	vfwprintf_s(pfile, format, args);
	
	va_end(args);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintHeader1(Report_TYPE type, const wchar_t *text, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"", sep, 0))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"", sep, 0, 30))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, text,sep, 1, 35))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"", sep, 0 ,30))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"", sep, 0))) return iReturnCode;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintHeader2(Report_TYPE type, const wchar_t *text, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"",sep, 0))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, text,sep, 1, 15))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"",sep, 0))) return iReturnCode;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintTitle1(Report_TYPE type, const wchar_t *text, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, text,sep, 1))) return iReturnCode;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintTitle2(Report_TYPE type, const wchar_t *text, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, text,sep, 0))) return iReturnCode;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintSeparator(Report_TYPE type, const wchar_t sep)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"",sep, 0))) return iReturnCode;

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintComment(Report_TYPE type, const wchar_t *text)
{
	int iReturnCode=DIAGLIB_OK;

	FILE *pfile;
	if(NULL == (pfile=getFile(type)))
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);;

	fwprintf_s(pfile,L"---> %ls\n", text);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintResult(Report_TYPE type, const wchar_t *text)
{
	int iReturnCode=DIAGLIB_OK;

	FILE *pfile;
	if(NULL == (pfile=getFile(type)))
		return RETURN_LOG_ERROR(DIAGLIB_ERR_INTERNAL);;

//	fwprintf_s(pfile,L"%ls\n\n", text);
	fwprintf_s(pfile,L"%ls\n", text);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportUnloadResources()
{
	int iReturnCode=DIAGLIB_OK;

	//Close res main and cpl
	if(g_pfile_level1)
	{
		if(0 != fclose(g_pfile_level1))
		{
			LOG_ERRORCODE(L"Failed to close the file",errno);
		}
		g_pfile_level1=NULL;
	}
	
	if(g_pfile_level2)
	{
		if(0 != fclose(g_pfile_level2))
		{
			LOG_ERRORCODE(L"Failed to close the file",errno);
		}
		g_pfile_level2=NULL;
	}
	
	if(g_pfile_level3)
	{
		if(0 != fclose(g_pfile_level3))
		{
			LOG_ERRORCODE(L"Failed to close the file",errno);
		}
		g_pfile_level3=NULL;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportFinalize(const wchar_t *wzReportAbstract,const wchar_t *wzReportTotal)
{
	int iReturnCode=DIAGLIB_OK;

	if(!g_wsReportTotal.empty())
	{
		LOG_ERROR(L"reportFinalize already be done");
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}
	
	if(DIAGLIB_OK != (iReturnCode=reportPrintFileSeparator(REPORT_TYPE_MAIN)))
		return iReturnCode;

	//Initialize FinalReport
	if(wzReportTotal==NULL)
	{
		folderGetPath(FOLDER_TEMP, &g_wsReportTotal);
		g_wsReportTotal.append(REPORT_TOTAL);	
	}
	else if(wcslen(wzReportTotal)==0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;	
	}
	else
	{
		g_wsReportTotal.assign(wzReportTotal);
	}

	if(wzReportAbstract==NULL)
	{
		folderGetPath(FOLDER_TEMP, &g_wsReportAbstract);
		g_wsReportAbstract.append(REPORT_ABSTRACT);	
	}
	else if(wcslen(wzReportAbstract)==0)
	{
		g_wsReportTotal.clear();
		return RETURN_LOG_BAD_FUNCTION_CALL;	
	}
	else
	{
		g_wsReportAbstract.assign(wzReportAbstract);
	}

	reportUnloadResources();

	errno_t err;

	FILE *fWrite = NULL;
	FILE *fWriteAbstract = NULL;
	FILE *fRead = NULL;
	wchar_t line[REPORT_WIDTH+1];

	//Open Final report for writing
	for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&fWrite, g_wsReportTotal.c_str(), L"w" FILE_ENCODING);
		if (!fWrite)
		{
			LOG_ERRORCODE(L"_wfopen_s failed",err);
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
		}
		if (fWrite)
			break;
		else
			Sleep(50);
	}

	//Open Result report for writing
	for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&fWriteAbstract, g_wsReportAbstract.c_str(), L"w" FILE_ENCODING);
		if (!fWriteAbstract)
		{
			LOG_ERRORCODE(L"_wfopen_s failed",err);
			return RETURN_LOG_ERROR(DIAGLIB_ERR_FILE_CREATE_FAILED);
		}
		if (fWriteAbstract)
			break;
		else
			Sleep(50);
	}


	//Open res for reading
	for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&fRead, g_wsReportLevel1.c_str(), L"r" FILE_ENCODING);
		if (!fRead)
		{
			LOG_ERRORCODE(L"_wfopen_s failed",err);
			return DIAGLIB_ERR_FILE_OPEN_FAILED;
		}

		if (fRead)
			break;
		else
			Sleep(50);
	}

	//write content into res
	while( !feof( fRead ) )
	{
		if( fgetws( line, REPORT_WIDTH+1, fRead ) == NULL)
		{
			break;
		}
		fwprintf_s(fWrite,L"%ls", line);
		fwprintf_s(fWriteAbstract,L"%ls", line);
    }

	//close Res
	if(0 != fclose(fRead))
	{
		LOG_ERRORCODE(L"Failed to close the file",errno);
	}
	fRead=NULL;

	//close ResultReport
	if(0 != fclose(fWriteAbstract))
	{
		LOG_ERRORCODE(L"Failed to close the file",errno);
	}
	fWriteAbstract=NULL;

	//delete Res
	fileDelete(g_wsReportLevel1);
	g_wsReportLevel1.clear();

	//Open main for reading
	for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&fRead, g_wsReportLevel2.c_str(), L"r" FILE_ENCODING);
		if (!fRead)
		{
			LOG_ERRORCODE(L"_wfopen_s failed",err);
			return DIAGLIB_ERR_FILE_OPEN_FAILED;
		}

		if (fRead)
			break;
		else
			Sleep(50);
	}

	//write content into res
	while( !feof( fRead ) )
	{
		if( fgetws( line, REPORT_WIDTH+1, fRead ) == NULL)
		{
			break;
		}
		fwprintf_s(fWrite,L"%ls", line);
    }

	//close main
	if(0 != fclose(fRead))
	{
		LOG_ERRORCODE(L"Failed to close the file",errno);
	}
	fRead=NULL;

	//delete main
	fileDelete(g_wsReportLevel2);
	g_wsReportLevel2.clear();

	//Open cpl for reading
	for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
	{
		err = _wfopen_s(&fRead, g_wsReportLevel3.c_str(), L"r" FILE_ENCODING);
		if (!fRead)
		{
			LOG_ERRORCODE(L"_wfopen_s failed",err);
			return DIAGLIB_ERR_FILE_OPEN_FAILED;
		}

		if (fRead)
			break;
		else
			Sleep(50);
	}

	//write content into res
	while( !feof( fRead ) )
	{
		if( fgetws( line, REPORT_WIDTH+1, fRead ) == NULL)
		{
			break;
		}
		fwprintf_s(fWrite,L"%ls", line);
    }

	//close cpl
	if(0 != fclose(fRead))
	{
		LOG_ERRORCODE(L"Failed to close the file",errno);
	}
	fRead=NULL;

	//delete cpl
	fileDelete(g_wsReportLevel3);
	g_wsReportLevel3.clear();

	//WARNING NO MORE LOGGING AFTER THIS POINT
	//Adding the log to the report
	logFinalize();

	const wchar_t *logFileName = NULL;

	if(DIAGLIB_OK == logGetFileName(&logFileName))
	{
		for(int i=0;i<REPORT_OPEN_ATTEMPT_COUNT;i++)
		{
			err = _wfopen_s(&fRead, logFileName, L"r" FILE_ENCODING);
			if (!fRead)
			{
				return DIAGLIB_ERR_FILE_OPEN_FAILED;
			}

			if (fRead)
				break;
			else
				Sleep(50);
		}

		fwprintf_s(fWrite,L"\n\n");
		fwprintf_s(fWrite,L"-------------------\n");
		fwprintf_s(fWrite,L"--- LOG CONTENT ---\n");
		fwprintf_s(fWrite,L"-------------------\n");

		//write content into res
		while( !feof( fRead ) )
		{
			if( fgetws( line, REPORT_WIDTH+1, fRead ) == NULL)
			{
				break;
			}
			fwprintf_s(fWrite,L"%ls", line);
		}

		//close main
		if(0 != fclose(fRead))
		{
			//LOG_ERRORCODE(L"Failed to close the file",errno);
		}
		fRead=NULL;
	}

	fwprintf_s(fWrite,L"\n\n");
	fwprintf_s(fWrite,L"----------------\n");
	fwprintf_s(fWrite,L"--- RAW DATA ---\n");
	fwprintf_s(fWrite,L"----------------\n\n");

	fwprintf_s(fWrite,L"<repository>\n");
	ContributionMap contributions=REP_RESULTS();
	for(ContributionMap::const_iterator i=contributions.begin();i!=contributions.end();i++)
		fwprintf_s(fWrite,(L"\t<contribution label=\"" + i->first + L"\"> <![CDATA[" + i->second + L"]] </contribution>\n").c_str());
	fwprintf_s(fWrite,L"<repository>\n");

	//close FinalReport
	if(0 != fclose(fWrite))
	{
		//LOG_ERRORCODE(L"Failed to close the file",errno);
	}
	fWrite=NULL;


	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
FILE *getFile(Report_TYPE type)
{
	if(type!=REPORT_TYPE_MAIN && type!=REPORT_TYPE_COMPLEMENT && type!=REPORT_TYPE_RESULT)
	{
		return NULL;	
	}

	if(g_pfile_level1 == NULL || g_pfile_level2 == NULL || g_pfile_level3 == NULL)
		reportLoadResources();

	switch(type)
	{
	case REPORT_TYPE_RESULT:
		return g_pfile_level1;
	case REPORT_TYPE_MAIN:
		return g_pfile_level2;
	case REPORT_TYPE_COMPLEMENT:
		return g_pfile_level3;
	default:
		return NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
int reportPrintFileSeparator(Report_TYPE type)
{
	int iReturnCode=DIAGLIB_OK;

	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"",L'<', 0))) return iReturnCode;
	if(DIAGLIB_OK != (iReturnCode=reportPrintLine(type, L"",L'>', 0))) return iReturnCode;

	return iReturnCode;
}