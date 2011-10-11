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

#include "user.h"
#include "error.h"
#include "log.h"
#include "util.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int userGetInfo(User_ID user, User_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id=user;
	FILE *			pF;
	size_t			bytesRead = 0;
	char			streamBuffer[1024];

	// popen will fork and invoke the sw_vers command and return a stream reference with its result data
	char cmdBuff[20];
	if(user == CURRENT_USER_ID)
	{
		sprintf(cmdBuff,"id");
	}
	else
	{
		sprintf(cmdBuff,"id %d",user);
	}

	pF = popen(cmdBuff, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 1024, pF)))
	{
		LOG_LASTERROR(L"fread failed");
		pclose (pF);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	// Close the stream
	pclose (pF);

	streamBuffer[bytesRead-1] = 0x00;

	std::vector<std::string> list;
	TokenizeS(streamBuffer,list," ");
	if(list.size() <= 0)
	{
		LOG_LASTERROR(L"TokenizeS failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	//Extract the user name
	size_t pos1;
	size_t pos2;
	if (std::string::npos == (pos1 = list[0].find('(')))
	{
		LOG_LASTERROR(L"Name not found");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	if (std::string::npos == (pos2 = list[0].rfind(')')))
	{
		LOG_LASTERROR(L"Name not found");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	if(pos2-pos1-1 <= 0)
	{
		LOG_LASTERROR(L"Name not found");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	info->Name = wstring_From_string(list[0].substr(pos1+1, pos2-pos1-1));

	userIsAdministrator(user, &info->IsAdmin);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int userIsAdministrator(User_ID user, bool *admin)
/*++
Routine Description: This routine returns TRUE in *admin if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token.
Arguments: None.
Return Value:
   TRUE - Caller has Administrators local group.
   FALSE - Caller does not have Administrators local group. --
*/
{
	int iReturnCode = DIAGLIB_OK;

	if(admin==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*admin=false;

	FILE *			pF;
	size_t			bytesRead = 0;
	char			streamBuffer[1024];

	// popen will fork and invoke the sw_vers command and return a stream reference with its result data
	char cmdBuff[20];
	if(user == CURRENT_USER_ID)
	{
		sprintf(cmdBuff,"id");
	}
	else
	{
		sprintf(cmdBuff,"id %d",user);
	}

	pF = popen(cmdBuff, "r");
	if (pF == NULL )
	{
		LOG_LASTERROR(L"popen failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	// Read the stream into a memory buffer
	if(0 == (bytesRead = fread(streamBuffer, sizeof(char), 1024, pF)))
	{
		LOG_LASTERROR(L"fread failed");
		pclose (pF);
		return RETURN_LOG_INTERNAL_ERROR;
	}
	// Close the stream
	pclose (pF);

	streamBuffer[bytesRead-1] = 0x00;

	// if found "(admin)" in groups list, current user have administrator privileges
	if (NULL != strstr (streamBuffer, "(admin)"))
	{
		*admin=true;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
