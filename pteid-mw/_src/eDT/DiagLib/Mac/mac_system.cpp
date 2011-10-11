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
#include <Carbon/Carbon.h>

#include "system.h"
#include "error.h"
#include "log.h"
#include "util.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int systemGetInfo(System_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	LOG_TIME(L"Ask for system info --> ");

	info->OsType=L"Mac";
	info->PlatformId = 0;
	info->ProductType = 0;
	info->ServicePack.clear();
	info->DefaultLanguage.clear();

	FILE * pF;
	size_t bytesRead = 0;
	char streamBuffer[64];
	const char *command=NULL;

	// Get product name
	command = "sw_vers -productName";
	pF = popen(command, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
	}
	else
	{
		if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 64, pF)))
		{
			LOG_LASTERROR(L"fread failed");
		}
		else
		{
			streamBuffer[bytesRead-1] = 0x00;
			info->ProductName.assign(wstring_From_string(streamBuffer));
		}
		pclose (pF);
	}

	// Get product version
	command = "sw_vers -productVersion";
	pF = popen(command, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
	}
	else
	{
		if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 64, pF)))
		{
			LOG_LASTERROR(L"fread failed");
		}
		else
		{
			streamBuffer[bytesRead-1] = 0x00;
			char *pos1=NULL;
			char *pos2=NULL;
			pos1=strchr(streamBuffer,'.');
			if(pos1 != NULL)
			{
				pos2=strchr(++pos1,'.');
			}
			if(pos2 == NULL)
			{
				info->MajorVersion.assign(wstring_From_string(streamBuffer));
				info->MinorVersion.assign(L"0");
			}
			else
			{
				*pos2=0;
				info->MajorVersion.assign(wstring_From_string(streamBuffer));
				info->MinorVersion.assign(wstring_From_string(++pos2));
			}
		}
		pclose (pF);
	}

	// Get build version
	command = "sw_vers -buildVersion";
	pF = popen(command, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
	}
	else
	{
		if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 64, pF)))
		{
			LOG_LASTERROR(L"fread failed");
		}
		else
		{
			streamBuffer[bytesRead-1] = 0x00;
			info->BuildNumber.assign(wstring_From_string(streamBuffer));
		}
		pclose (pF);
	}
	info->Description.assign(info->ProductName);
	info->Description.append(L" ");
	info->Description.append(info->MajorVersion);
	info->Description.append(L".");
	info->Description.append(info->MinorVersion);
	info->Description.append(L" (build ");
	info->Description.append(info->BuildNumber);
	info->Description.append(L")");

	// Get Architecture	
	command = "system_profiler SPSoftwareDataType -detaillevel mini | grep 64";
	pF = popen(command, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
	}
	else
	{
		if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 64, pF)))
		{
			LOG_LASTERROR(L"fread failed");
		}
		else
		{
			streamBuffer[bytesRead-1] = 0x00;
			info->Architecture.assign(wstring_From_string(streamBuffer));
		}
		pclose (pF);
	}
	LOG(L"DONE\n");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int systemReboot (void)
{
	int iReturnCode = DIAGLIB_OK;

	AEEventID	EventToSend = kAERestart;
	AEAddressDesc targetDesc;
	static const ProcessSerialNumber kPSNOfSystemProcess = { 0, kSystemProcess };
	AppleEvent eventReply = {typeNull, NULL};
	AppleEvent appleEventToSend = {typeNull, NULL};
	OSStatus error = noErr;

    error = AECreateDesc(typeProcessSerialNumber, &kPSNOfSystemProcess,
                                            sizeof(kPSNOfSystemProcess), &targetDesc);

    if (error != noErr)
    {
		LOG_ERRORCODE(L"AECreateDesc failed",error);
		return DIAGLIB_ERR_INTERNAL;
    }

    error = AECreateAppleEvent(kCoreEventClass, EventToSend, &targetDesc,
                   kAutoGenerateReturnID, kAnyTransactionID, &appleEventToSend);

    AEDisposeDesc(&targetDesc);
    if (error != noErr)
    {
		LOG_ERRORCODE(L"AECreateAppleEvent failed",error);
        return DIAGLIB_ERR_INTERNAL;
    }

    error = AESend(&appleEventToSend, &eventReply, kAENoReply,
                  kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

    AEDisposeDesc(&appleEventToSend);
    if (error != noErr)
    {
		LOG_ERRORCODE(L"AESend failed",error);
        return DIAGLIB_ERR_INTERNAL;
    }

    AEDisposeDesc(&eventReply);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////int systemGetInfo(System_INFO *info)
static int isLeopard=-1;
bool systemIsLeopard()
{
	if(isLeopard == -1)
	{
		System_INFO info;
		if(DIAGLIB_OK != systemGetInfo(&info))
		{
		   isLeopard=0;
		}
		else if(wcscmp(info.MajorVersion.c_str(),L"10.5") == 0)
		{
		   isLeopard=1;
		}
		else
		{
		   isLeopard=0;
		}
	}

	return (isLeopard==1);
}

