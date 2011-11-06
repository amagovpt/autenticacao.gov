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
#include <windows.h>

#include "user.h"
#include "error.h"
#include "log.h"

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

	if(user != CURRENT_USER_ID)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	info->id=user;

	DWORD dwBytesNeeded = 0;
	DWORD dwBufSize = 0;
	wchar_t *pBuf = NULL;

	if(::GetUserName( NULL, &dwBytesNeeded))
	{
		LOG_LASTERROR(L"::GetUserName failed to get size needed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError() || dwBytesNeeded < 1) 
		{
			LOG_LASTERROR(L"::GetUserName failed to get size needed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
	}

	if(iReturnCode == DIAGLIB_OK) 
	{
		//Allocate the buffer
  		dwBufSize = dwBytesNeeded + 0x10; 
		if(NULL == (pBuf  = (wchar_t *) malloc(dwBufSize)))
		{
			LOG_ERROR(L"Memory allocation failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if (!::GetUserName(pBuf, &dwBufSize)) 
		{
			LOG_LASTERROR(L"::GetUserName failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			info->Name=pBuf;
		}
		if(pBuf) free(pBuf);
	}

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

	if(user != CURRENT_USER_ID)
	{
		return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);
	}

	*admin=false;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup; 
	if(!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup))
	{
		LOG_LASTERROR(L"AllocateAndInitializeSid");
		iReturnCode=DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		BOOL b;
		if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
		{
			LOG_LASTERROR(L"CheckTokenMembership");
			iReturnCode=DIAGLIB_ERR_INTERNAL;
		} 
		else
		{
			if(b)
			{
				*admin=true;
			}
		}

		if(NULL != FreeSid(AdministratorsGroup))
		{
			LOG_LASTERROR(L"FreeSid");
			iReturnCode=DIAGLIB_ERR_INTERNAL;
		} 
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
