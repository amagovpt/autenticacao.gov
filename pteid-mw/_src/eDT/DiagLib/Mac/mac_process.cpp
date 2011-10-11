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
#include "mac_helper.h"
#include <signal.h>

#include "process.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "progress.h"

#define G_BUFFER_SIZE 65000
static char g_buffer[G_BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processFillList(Proc_LIST *processList, const wchar_t *processName);
int getPidFromParentid(pid_t parentid, std::string *command, pid_t *pid);
bool systemIsLeopard();

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processUsingLibrary(Lib_ID library, Proc_LIST *processList)
{
	int iReturnCode = DIAGLIB_OK;

	if(library.empty() || processList==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Check for use of library '%ls' --> ",library.c_str());

	processList->clear();

	FILE *			pF;
	size_t			bytesRead = 0;

	// popen will fork and invoke the ps command and return a stream reference with its result data
	char cmdBuff[50];
	sprintf(cmdBuff,"lsof -F p \"%ls\"",library.c_str());

	pF = popen(cmdBuff, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR1(L"popen '%s' failed", cmdBuff);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
	{
		LOG(L"NOT USED\n");
		pclose (pF);
		return DIAGLIB_OK;
	}
	// Close the stream
	pclose (pF);

	// add EOL to stream
	g_buffer[bytesRead-1] = 0x00;

	// initialze the string array to hold all lines in separate string.
	std::vector<std::string> procList;
	Proc_ID pid=0;

	TokenizeS(g_buffer,procList,"\n");

	if(procList.size() == 0)
	{
		LOG(L"NOT USED\n");
	}
	else
	{
		progressInit(procList.size());
		for (unsigned long i=0; i < procList.size() ; i++)
		{
			if(procList[i].length() < 2)
			{
				LOG_ERROR(L"Bad format in popen output");
			}
			else
			{
				pid=atol(procList[i].substr(1).c_str());	//First caracter contain 'p'
				processList->push_back(pid);
				LOG(L"FOUND Process (pid=%ld)\n",pid);
			}
			progressIncrement();
		}
		progressRelease();
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processGetInfo(Proc_ID process, Proc_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask info on pid=%ld --> ",process);

	info->id=process;

	FILE *			pF;
	size_t			bytesRead = 0;

	// popen will fork and invoke the ps command and return a stream reference with its result data
	char cmdBuff[50];
	if(systemIsLeopard())
	{
		sprintf(cmdBuff,"ps -p %ld -o pid,comm",process);
	}
	else
	{	//comm is not available on tiger (command return all the command line with argument)
		sprintf(cmdBuff,"ps -p %ld -o pid,command",process);
	}

	pF = popen(cmdBuff, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR1(L"popen '%s' failed", cmdBuff);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
	{
		LOG_ERROR(L"Process not found");
		pclose (pF);
		return RETURN_LOG_ERROR(DIAGLIB_ERR_PROCESS_NOT_FOUND);
	}
	// Close the stream
	pclose (pF);

	// add EOL to stream
	g_buffer[bytesRead-1] = 0x00;

	// initialze the string array to hold all lines in separate string.
	std::vector<std::string> procList;
	std::vector<std::string> proc;
	std::vector<std::string> path;

	// split the resulting stream into separate lines (splitting on the newline char).
	TokenizeS(g_buffer,procList,"\n");
	if(procList.size()<=1)
	{
		LOG_ERROR(L"Process not found");
		return RETURN_LOG_ERROR(DIAGLIB_ERR_PROCESS_NOT_FOUND);
	}

	TokenizeS(procList[1],proc," ",2);

	if(proc.size() < 2)
	{
		LOG_ERROR(L"Bad output format");
		return RETURN_LOG_INTERNAL_ERROR;
	}
	TokenizeS(proc[1],path,"/");

	info->Name = wstring_From_string(path[path.size()-1]);
	info->Path = wstring_From_string(proc[1]);
	info->FullPath = info->Path;
	
	info->modulesLoaded.clear();
	sprintf(g_buffer,"lsof -p %ld -Fn",process);
	FILE* ps=popen(g_buffer,"r");
	while(fgets(g_buffer,G_BUFFER_SIZE,ps) != NULL)
	{
		g_buffer[strlen(g_buffer)-1]='\0';
		if(g_buffer[0]=='n' && strstr(g_buffer,".dylib")!=NULL)
			info->modulesLoaded.insert(wstring_From_string(g_buffer+1));
	}
	pclose(ps);
	

	LOG(L"DONE\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processKill(Proc_ID process)
{
	int iReturnCode = DIAGLIB_OK;

	if(process <= 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for killing pid=%ld --> ",process);

	int err = 0;

	if (-1 == (err = kill(process,SIGKILL)))
	{
		LOG_LASTERROR(L"TerminateProcess failed");
		iReturnCode = DIAGLIB_ERR_PROCESS_KILL_FAILED;
	}
	else
	{
		LOG(L"FOUND and KILLED (Return code = %ld)\n",err);
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processStart(Proc_NAME process, Proc_ID *id, int waitTimeSecs, int *exitCode)
{
	int iReturnCode = DIAGLIB_OK;

	if(process.empty() || id == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Ask for start process named '%ls' --> ",process.c_str());

	std::string command = string_From_wstring(process);

	unloadResources();

	pid_t parentid = fork();
	if(parentid<0)
	{
		LOG_LASTERROR(L"CreateProcess failed");
		iReturnCode = DIAGLIB_ERR_PROCESS_START_FAILED;
	}
	else if(parentid == 0)
	{
		exit(system(command.c_str()));
	}
	else
	{
		Sleep(100);
		pid_t pid;
		if(DIAGLIB_OK != (iReturnCode = getPidFromParentid(parentid,&command,&pid)))
		{
			LOG_ERROR(L"getPidFromParentid failed");
			iReturnCode = DIAGLIB_ERR_PROCESS_START_FAILED;
		}
		else
		{
			*id=pid;
			LOG(L"STARTED with pid=%ld\n",*id);
			//We kill the fork
			if (-1 == kill(parentid,SIGKILL))
			{
				LOG_LASTERROR(L"TerminateProcess failed");
			}
			if(waitTimeSecs>0)
			{
				iReturnCode=processWait(*id, exitCode, waitTimeSecs);
			}
		}
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processStartAsAdmin(Proc_NAME process, Proc_ID *id, int waitTimeSecs, int *exitCode)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int processWait(Proc_ID process, int *exitCode, int waitTimeSecs)
{
	int iReturnCode = DIAGLIB_OK;

	if(process == 0)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	LOG_TIME(L"Waiting for process pid=%ld --> ",process);

	int count=0;
	FILE *pF;
	size_t bytesRead = 0;
	char cmdBuff[50];
	sprintf(cmdBuff,"ps -p %ld -o pid",process);

	std::vector<std::string> procList;

	progressInit(waitTimeSecs);
	while(1)
	{
		pF = popen(cmdBuff, "r");
		if (pF == NULL )
		{
			LOG_LASTERROR1(L"popen '%s' failed", cmdBuff);
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}

		// Read the stream into a memory buffer
		if(0 == (bytesRead = fread(g_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
		{
			if(exitCode) *exitCode=0;
			LOG(L"QUIT NORMALLY\n");
			pclose (pF);
			break;
		}
		// Close the stream
		pclose (pF);

		// add EOL to stream
		g_buffer[bytesRead-1] = 0x00;

		// split the resulting stream into separate lines (splitting on the newline char).
		TokenizeS(g_buffer,procList,"\n");
		if(procList.size() <= 1) //If there is only one line (header), the process is finished
		{
			if(exitCode) *exitCode=0;
			LOG(L"QUIT NORMALLY\n");
			break;
		}
		else if(procList.size() > 2)
		{
			LOG_LASTERROR(L"Too much pid???");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}

		if(count < waitTimeSecs)
		{
			Sleep(1000);
			count++;
		}
		else
		{
			LOG_ERROR(L"TIMEOUT");
			iReturnCode = DIAGLIB_ERR_PROCESS_WAIT_TIMEOUT;
			break;
		}
		progressIncrement();
	}
	progressRelease();

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int processFillList(Proc_LIST *processList, const wchar_t *processName)
{
	int iReturnCode = DIAGLIB_OK;

	if(processList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	if(processName)
	{
		LOG_TIME(L"Ask for process list (with name = %ls) --> ",processName);
	}
	else
	{
		LOG_TIME(L"Ask for list of all process --> ");
	}

	processList->clear();

	FILE *			pF;
	size_t			bytesRead = 0;

	// popen will fork and invoke the ps command and return a stream reference with its result data
	const char *psCommand = NULL;
	psCommand = "ps -A -c -o pid,command";

	pF = popen(psCommand, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR1(L"popen '%s' failed", psCommand);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
	{
		pclose (pF);
		LOG_ERROR(L"fread failed");
		return DIAGLIB_OK;
	}
	// Close the stream
	pclose (pF);

	// add EOL to stream
	g_buffer[bytesRead-1] = 0x00;

	// initialze the string array to hold all lines in separate string.
	std::vector<std::string> procList;
	std::vector<std::string> proc;

	// split the resulting stream into separate lines (splitting on the newline char).
	TokenizeS(g_buffer,procList,"\n");

	progressInit(procList.size());

	bool add = false;
	Proc_ID pid=0;
	// we start from line 1, the first line contains the header.
	// perhaps also remove "ps -eaxw" from the list ?!
	for (unsigned long i=1; i < procList.size() ; i++)
	{
		TokenizeS(procList[i],proc," ",2);
		if(proc.size() < 2)
		{
			LOG_ERROR(L"Bad output format");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}

		if(0 != strcmp(proc[1].c_str(),"ps")) //We remove the specific ps command from the list
		{
			add = false;

			if(processName == NULL)
			{
				add = true;
			}
			else
			{
				if(0 == strcmp(proc[1].c_str(),string_From_wstring(processName).c_str()))
				{
					add = true;
				}
			}
			if(add)
			{
				pid = atol(proc[0].c_str());
				processList->push_back(pid);
			}
		}
		progressIncrement();
	}

	progressRelease();

	if(processList->size() == 0)
	{
		LOG(L"NO PROCESS FOUND\n");
	}
	else
	{
		LOG(L"pid=");
		for (unsigned long i=0; i < processList->size() ; i++)
		{
			LOG(L"%ld,",processList->at(i));
		}
		LOG(L"DONE\n");
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int getPidFromParentid(pid_t parentid, std::string *command, pid_t *pid)
{
	int iReturnCode = DIAGLIB_OK;

	if(command == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	FILE *			pF;
	size_t			bytesRead = 0;

	// popen will fork and invoke the ps command and return a stream reference with its result data
	const char *psCommand="ps -o pid,ppid,command";
	pF = popen(psCommand, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR1(L"popen '%s' failed", psCommand);
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(g_buffer, sizeof(char), G_BUFFER_SIZE, pF)))
	{
		pclose (pF);
		return DIAGLIB_ERR_PROCESS_NOT_FOUND;
	}
	// Close the stream
	pclose (pF);

	// add EOL to stream
	g_buffer[bytesRead-1] = 0x00;

	// initialze the string array to hold all lines in separate string.
	std::vector<std::string> procList;
	std::vector<std::string> proc;

	// split the resulting stream into separate lines (splitting on the newline char).
	TokenizeS(g_buffer,procList,"\n");

	progressInit(procList.size());

	bool found = false;
	bool foundParent = false;

	// we start from line 1, the first line contains the header.
	for (unsigned long i=1; i < procList.size() ; i++)
	{
		TokenizeS(procList[i],proc," ",3);
		if(proc.size() < 3)
		{
			LOG_ERROR(L"Bad output format");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		}

		if(0 != strcmp(proc[2].c_str(),psCommand)) //We remove the specific ps command from the list
		{
			if(parentid == atol(proc[0].c_str()))
			{
				foundParent=true;
			}

			if(parentid == atol(proc[1].c_str()))
			{
				if(command == NULL)
				{
					*pid=atol(proc[0].c_str());
					found = true;
					break;
				}
				else
				{
					if(0 == strcmp(proc[2].c_str(),command->c_str()))
					{
						*pid=atol(proc[0].c_str());
						found = true;
						break;
					}
				}
			}
		}

		progressIncrement();
	}

	//If the parent could not be found, that means the process is already terminated
	if(!found && foundParent)
	{
		iReturnCode = DIAGLIB_ERR_PROCESS_NOT_FOUND;
	}

	progressRelease();

	return iReturnCode;
}

