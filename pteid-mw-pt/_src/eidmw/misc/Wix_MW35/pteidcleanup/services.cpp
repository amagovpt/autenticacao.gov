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
#include <windows.h>

#include "services.h"
#include "error.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int StopAndRemoveService( const wchar_t *wzServiceName, long lTimeout, bool bRemove)
{
	int iReturnCode = RETURN_OK;

	SERVICE_STATUS strServiceStatus;
	SC_HANDLE      schManager=0;
	SC_HANDLE      schService=0;
	DWORD          dwError;

	LOG(L"Ask for stopping%s service \"%s\"\n",(bRemove?L" and removing":L""),wzServiceName);

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,SC_MANAGER_ALL_ACCESS )) ) 
	{
		LOG(L" --> ERROR - OpenSCManager failed (LastError=%d)\n", GetLastError());
		iReturnCode = RETURN_ERR_INTERNAL;
	}
	else
	{
		if( NULL == (schService = OpenService( schManager, wzServiceName, SERVICE_STOP | (bRemove?DELETE:0) | SERVICE_QUERY_STATUS )) ) 
		{
			switch( dwError = GetLastError() ) 
			{
			case ERROR_ACCESS_DENIED:
			case ERROR_INVALID_HANDLE:
				LOG(L" --> ERROR - OpenService failed (LastError=%d)\n", dwError);
				iReturnCode = RETURN_ERR_INTERNAL;
				break;
			default:
				LOG(L" --> NOT FOUND\n");
				break;
			}
		}
		else
		{
			if( 0 == ControlService( schService, SERVICE_CONTROL_STOP, &strServiceStatus ) ) 
			{
				switch( dwError = GetLastError() ) 
				{
				case ERROR_ACCESS_DENIED:
				case ERROR_DEPENDENT_SERVICES_RUNNING:
				case ERROR_INVALID_HANDLE:
				case ERROR_INVALID_PARAMETER:
				case ERROR_INVALID_SERVICE_CONTROL:
				case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
				case ERROR_SERVICE_REQUEST_TIMEOUT:
				default:
					LOG(L" --> ERROR - ControlService failed (LastError=%d)\n", dwError);
					iReturnCode = RETURN_ERR_STOPSERVICE_FAILED;
					break;
				case NO_ERROR:
				case ERROR_SERVICE_NOT_ACTIVE:
				case ERROR_SHUTDOWN_IN_PROGRESS:
					iReturnCode = RETURN_OK;
					break;
				}
			}

			if(iReturnCode == RETURN_OK)
			{
				long count=lTimeout;
				while(count>0 && strServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
				{
					Sleep(1000);
					if( 0 == QueryServiceStatus(schService, &strServiceStatus) )
					{
						switch( dwError = GetLastError() ) 
						{
						case NO_ERROR:
						case ERROR_SERVICE_NOT_ACTIVE:
						case ERROR_SHUTDOWN_IN_PROGRESS:
							break;
						default:
							LOG(L" --> ERROR - QueryServiceStatus failed (LastError=%d)\n", dwError);
							iReturnCode = RETURN_ERR_STOPSERVICE_FAILED;
							break;
						}
						break;
					}
					count--;
				}
				if(iReturnCode == RETURN_OK)
				{
					if(strServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						LOG(L" --> ERROR - Stopping service timeout\n");
						iReturnCode = RETURN_ERR_STOPSERVICE_FAILED;
					}
					else if( strServiceStatus.dwCurrentState != SERVICE_STOPPED ) 
					{
						LOG(L" --> ERROR - Service not stopped (CurrentState=%d)\n",strServiceStatus.dwCurrentState);
						iReturnCode = RETURN_ERR_STOPSERVICE_FAILED;
					}
					else
					{
						LOG(L" --> STOPPED\n");
						if( bRemove && (0 == DeleteService( schService )) ) 
						{
							LOG(L" --> ERROR - DeleteService failed (LastError=%d)\n", GetLastError());
							iReturnCode = RETURN_ERR_DELETESERVICE_FAILED;
						}
						else
						{					
							LOG(L" --> REMOVED\n");
						}
					}
				}
			}
		}
	}

    if(schService) CloseServiceHandle(schService); 
    if(schManager) CloseServiceHandle(schManager);

	LOG(L"\n");
	return iReturnCode;
}

