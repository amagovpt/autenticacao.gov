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
#include <map>

#include "services.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#define G_BUFFER_SIZE 32767
static TCHAR g_buffer[G_BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int serviceFillList(Service_LIST *serviceList, const wchar_t *processDisplayName);
const wchar_t *serviceGetStatusLabel(int Status);
int serviceGetInfoFromAPI(Service_ID id, Service_INFO *info);
int serviceGetInfoFromManagerHandle(SC_HANDLE schManager, Service_ID id, Service_INFO *info);
int serviceGetInfoFromServiceHandle(SC_HANDLE schService, Service_ID id, Service_INFO *info);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetInfo(Service_ID id, Service_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;
	LOG_TIME(L"Ask for info on service '%ls' --> ",id.c_str());

	if(DIAGLIB_OK == (iReturnCode = serviceGetInfoFromAPI(id, info)))
	{
		LOG(L"DONE\n");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceStart (Service_ID id, unsigned long lTimeout)
{
	int iReturnCode = DIAGLIB_OK;

	SERVICE_STATUS strServiceStatus;
	SC_HANDLE      schManager=0;
	SC_HANDLE      schService=0;

	LOG_TIME(L"Ask for starting service '%ls' --> ",id.c_str());

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_EXECUTE )) ) 
	{
		LOG_LASTERROR(L"OpenSCManager failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if( NULL == (schService = OpenService( schManager, id.c_str(), SERVICE_START | SERVICE_QUERY_STATUS )) ) 
		{
			switch( GetLastError() ) 
			{
			case ERROR_ACCESS_DENIED:
			case ERROR_INVALID_HANDLE:
				LOG_LASTERROR(L"OpenService failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			default:
				LOG(L"NOT FOUND\n");
				break;
			}
		}
		else
		{
			if( 0 == StartService( schService, 0, NULL ) ) 
			{
				LOG_LASTERROR(L"StartService failed");
				iReturnCode = DIAGLIB_ERR_SERVICE_START_FAILED;
			}

			if(iReturnCode == DIAGLIB_OK)
			{
				long count=lTimeout;
				do {
					if( 0 == QueryServiceStatus(schService, &strServiceStatus) )
					{
						switch( GetLastError() ) 
						{
						case NO_ERROR:
						case ERROR_SERVICE_NOT_ACTIVE:
						case ERROR_SHUTDOWN_IN_PROGRESS:
							break;
						default:
							LOG_LASTERROR(L"QueryServiceStatus failed");
							iReturnCode = DIAGLIB_ERR_INTERNAL;
							break;
						}
						break;
					}
					Sleep(1000);
					count--;
				} while(count>0 && strServiceStatus.dwCurrentState == SERVICE_START_PENDING);

				if(iReturnCode == DIAGLIB_OK)
				{
					if(strServiceStatus.dwCurrentState == SERVICE_START_PENDING)
					{
						LOG_ERROR(L"Starting service timeout");
						iReturnCode = DIAGLIB_ERR_SERVICE_START_TIMEOUT;
					}
					else if( strServiceStatus.dwCurrentState != SERVICE_RUNNING ) 
					{
						LOG(L"Service not started (CurrentState=%d)\n",strServiceStatus.dwCurrentState);
						iReturnCode = DIAGLIB_ERR_SERVICE_START_FAILED;
					}
					else
					{
						LOG(L"STARTED\n");
					}
				}
			}
		}
	}

    if(schService) CloseServiceHandle(schService); 
    if(schManager) CloseServiceHandle(schManager);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceStop (Service_ID id, unsigned long lTimeout)
{
	int iReturnCode = DIAGLIB_OK;

	SERVICE_STATUS strServiceStatus;
	SC_HANDLE      schManager=0;
	SC_HANDLE      schService=0;

	LOG_TIME(L"Ask for stopping service '%ls' --> ",id.c_str());

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_EXECUTE )) ) 
	{
		LOG_LASTERROR(L"OpenSCManager failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if( NULL == (schService = OpenService( schManager, id.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS )) ) 
		{
			switch( GetLastError() ) 
			{
			case ERROR_ACCESS_DENIED:
			case ERROR_INVALID_HANDLE:
				LOG_LASTERROR(L"OpenService failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			default:
				LOG(L"NOT FOUND\n");
				break;
			}
		}
		else
		{
			if( 0 == ControlService( schService, SERVICE_CONTROL_STOP, &strServiceStatus ) ) 
			{
				switch( GetLastError() ) 
				{
				case ERROR_ACCESS_DENIED:
				case ERROR_DEPENDENT_SERVICES_RUNNING:
				case ERROR_INVALID_HANDLE:
				case ERROR_INVALID_PARAMETER:
				case ERROR_INVALID_SERVICE_CONTROL:
				case ERROR_SERVICE_CANNOT_ACCEPT_CTRL:
				case ERROR_SERVICE_REQUEST_TIMEOUT:
				default:
					LOG_LASTERROR(L"ControlService failed");
					iReturnCode = DIAGLIB_ERR_SERVICE_STOP_FAILED;
					break;
				case NO_ERROR:
				case ERROR_SERVICE_NOT_ACTIVE:
				case ERROR_SHUTDOWN_IN_PROGRESS:
					iReturnCode = DIAGLIB_OK;
					break;
				}
			}

			if(iReturnCode == DIAGLIB_OK)
			{
				long count=lTimeout;
				while(count>0 && strServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
				{
					Sleep(1000);
					if( 0 == QueryServiceStatus(schService, &strServiceStatus) )
					{
						switch( GetLastError() ) 
						{
						case NO_ERROR:
						case ERROR_SERVICE_NOT_ACTIVE:
						case ERROR_SHUTDOWN_IN_PROGRESS:
							break;
						default:
							LOG_LASTERROR(L"QueryServiceStatus failed");
							iReturnCode = DIAGLIB_ERR_INTERNAL;
							break;
						}
						break;
					}
					count--;
				}
				if(iReturnCode == DIAGLIB_OK)
				{
					if(strServiceStatus.dwCurrentState == SERVICE_STOP_PENDING)
					{
						LOG_ERROR(L"Stopping service timeout");
						iReturnCode = DIAGLIB_ERR_SERVICE_STOP_TIMEOUT;
					}
					else if( strServiceStatus.dwCurrentState != SERVICE_STOPPED ) 
					{
						LOG(L"Service not stopped (CurrentState=%d)\n",strServiceStatus.dwCurrentState);
						iReturnCode = DIAGLIB_ERR_SERVICE_STOP_FAILED;
					}
					else
					{
						LOG(L"STOPPED\n");
					}
				}
			}
		}
	}

    if(schService) CloseServiceHandle(schService); 
    if(schManager) CloseServiceHandle(schManager);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceRemove (Service_ID id)
{
	int iReturnCode = DIAGLIB_OK;

	SERVICE_STATUS strServiceStatus;
	SC_HANDLE      schManager=0;
	SC_HANDLE      schService=0;

	LOG_TIME(L"Ask for removing service '%ls' --> ",id.c_str());

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_WRITE )) ) 
	{
		LOG_LASTERROR(L"OpenSCManager failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if( NULL == (schService = OpenService( schManager, id.c_str(), DELETE | SERVICE_QUERY_STATUS )) ) 
		{
			switch( GetLastError() ) 
			{
			case ERROR_ACCESS_DENIED:
			case ERROR_INVALID_HANDLE:
				LOG_LASTERROR(L"OpenService failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			default:
				LOG(L"NOT FOUND\n");
				break;
			}
		}
		else
		{
			if( 0 == QueryServiceStatus(schService, &strServiceStatus) )
			{
				switch( GetLastError() ) 
				{
				case NO_ERROR:
				case ERROR_SERVICE_NOT_ACTIVE:
				case ERROR_SHUTDOWN_IN_PROGRESS:
					break;
				default:
					LOG_LASTERROR(L"QueryServiceStatus failed");
					iReturnCode = DIAGLIB_ERR_INTERNAL;
					break;
				}
			}
			if(iReturnCode == DIAGLIB_OK)
			{
				if( strServiceStatus.dwCurrentState != SERVICE_STOPPED ) 
				{
					LOG(L"Service not stopped (CurrentState=%d)\n",strServiceStatus.dwCurrentState);
					iReturnCode = DIAGLIB_ERR_SERVICE_DELETE_FAILED;
				}
				else
				{
					if(0 == DeleteService( schService ))  
					{
						LOG_LASTERROR(L"DeleteService failed");
						iReturnCode = DIAGLIB_ERR_SERVICE_DELETE_FAILED;
					}
					else
					{	
						LOG(L"REMOVED\n");
					}
				}
			}
		}
	}

    if(schService) CloseServiceHandle(schService); 
    if(schManager) CloseServiceHandle(schManager);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int serviceFillList (Service_LIST *serviceList, const wchar_t *serviceDisplayName)
{
	int iReturnCode = DIAGLIB_OK;

	if(serviceList==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	serviceList->clear();

	SC_HANDLE      schManager=0;
	SC_HANDLE      schService=0;

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_READ )) ) 
	{
		LOG_LASTERROR(L"OpenSCManager failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		DWORD dwBytesNeeded = 0;
		DWORD dwBufSize = 0;
		DWORD dwNumberOfService = 0;
		PUCHAR pBuf = NULL;
		ENUM_SERVICE_STATUS_PROCESS *pStatus = NULL;

		//First call to get the size of the buffer
		if(0 != EnumServicesStatusEx( schManager,						// __in        SC_HANDLE hSCManager,
							  SC_ENUM_PROCESS_INFO,				// __in        SC_ENUM_TYPE InfoLevel,
							  SERVICE_DRIVER | SERVICE_WIN32,	// __in        DWORD dwServiceType,
							  SERVICE_STATE_ALL,				// __in        DWORD dwServiceState,
							  NULL,							    //__out_opt    LPBYTE lpServices,
							  0,								//__in         DWORD cbBufSize,
							  &dwBytesNeeded,					//__out        LPDWORD pcbBytesNeeded,
							  &dwNumberOfService,				//__out        LPDWORD lpServicesReturned,
							  NULL,								//__inout_opt  LPDWORD lpResumeHandle,
							  NULL								//__in_opt     LPCTSTR pszGroupName
							  ))
		{
			LOG_LASTERROR(L"EnumServicesStatusEx failed to get size needed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else
		{
			if( ERROR_MORE_DATA != GetLastError() || dwBytesNeeded < 1)
			{
				LOG_LASTERROR(L"EnumServicesStatusEx failed to get size needed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
		}

		if(iReturnCode == DIAGLIB_OK) 
		{
			//Allocate the buffer
			dwBufSize = dwBytesNeeded + 0x10; 
			if(NULL == (pBuf  = (PUCHAR) malloc(dwBufSize)))
			{
				LOG_ERROR(L"Memory allocation failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			else if(0 == EnumServicesStatusEx( schManager,				// __in        SC_HANDLE hSCManager,
									  SC_ENUM_PROCESS_INFO,				// __in        SC_ENUM_TYPE InfoLevel,
									  SERVICE_DRIVER | SERVICE_WIN32,	// __in        DWORD dwServiceType,
									  SERVICE_STATE_ALL,				// __in        DWORD dwServiceState,
									  pBuf,							    //__out_opt    LPBYTE lpServices,
									  dwBufSize,						//__in         DWORD cbBufSize,
									  &dwBytesNeeded,					//__out        LPDWORD pcbBytesNeeded,
									  &dwNumberOfService,				//__out        LPDWORD lpServicesReturned,
									  NULL,								//__inout_opt  LPDWORD lpResumeHandle,
									  NULL								//__in_opt     LPCTSTR pszGroupName
									  ))
			{
				LOG_LASTERROR(L"EnumServicesStatusEx failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
			}
			else
			{
				bool bAddToList;
				Service_INFO info;

				progressInit(dwNumberOfService);

				pStatus = (LPENUM_SERVICE_STATUS_PROCESS)pBuf; 
				for (ULONG i=0;i<dwNumberOfService;i++) 
				{  
					bAddToList=false;
					if(serviceDisplayName == NULL)
					{
						bAddToList=true;
					}
					else if(0 == _wcsicmp(pStatus[i].lpDisplayName,serviceDisplayName))	
					{
						bAddToList=true;
					}

					if(bAddToList)
					{
						//serviceGetInfoFromManagerHandle(schManager, pStatus[i].lpServiceName,&info);
						serviceList->push_back(pStatus[i].lpServiceName);
					}
					progressIncrement();
				}

				progressRelease();
			}
			if(pBuf) free(pBuf);
		}
	}
    if(schManager) CloseServiceHandle(schManager);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
const wchar_t *serviceGetStatusLabel(int Status)
{
	switch(Status)
	{
	case SERVICE_CONTINUE_PENDING:				//5
		return L"SERVICE_CONTINUE_PENDING";
	case SERVICE_PAUSE_PENDING:					//6
		return L"SERVICE_PAUSE_PENDING";
	case SERVICE_PAUSED:						//7
		return L"SERVICE_PAUSED";
	case SERVICE_RUNNING:						//4
		return L"SERVICE_RUNNING";
	case SERVICE_START_PENDING:					//2
		return L"SERVICE_START_PENDING";
	case SERVICE_STOP_PENDING:					//3
		return L"SERVICE_STOP_PENDING";
	case SERVICE_STOPPED:						//1
		return L"SERVICE_STOPPED";
	}

	return L"UNKNOWN_STATUS";
}

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetInfoFromAPI(Service_ID id, Service_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	SC_HANDLE      schManager=0;

	if( NULL == (schManager = OpenSCManager( NULL, SERVICES_ACTIVE_DATABASE,GENERIC_READ )) ) 
	{
		LOG_LASTERROR(L"OpenSCManager failed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		iReturnCode = serviceGetInfoFromManagerHandle(schManager,id,info);
	}

    if(schManager) CloseServiceHandle(schManager);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetInfoFromManagerHandle(SC_HANDLE schManager, Service_ID id, Service_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(schManager==0 || info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	SC_HANDLE      schService=0;

	if( NULL == (schService = OpenService( schManager, id.c_str(), SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG )) ) 
	{
		switch( GetLastError() ) 
		{
		case ERROR_ACCESS_DENIED:
		case ERROR_INVALID_HANDLE:
			LOG_LASTERROR(L"OpenService failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
			break;
		default:
			LOG(L"NOT FOUND\n");
			break;
		}
	}
	else 
	{
		iReturnCode = serviceGetInfoFromServiceHandle(schService,id,info);
	}

    if(schService) CloseServiceHandle(schService); 

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int serviceGetInfoFromServiceHandle(SC_HANDLE schService, Service_ID id, Service_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(schService==0 || info==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	DWORD dwBytesNeeded = 0;
	DWORD dwBufSize = 0;
	PUCHAR pBuf = NULL;

	info->id=id;

	//QueryServiceStatusEx
	SERVICE_STATUS_PROCESS *pStatus = NULL;
	//First call to get the size of the buffer
	if( 0 != QueryServiceStatusEx(schService,				//__in       SC_HANDLE hService,
							      SC_STATUS_PROCESS_INFO,	//__in       SC_STATUS_TYPE InfoLevel,
							      NULL,						//__out_opt  LPBYTE lpBuffer,
							      0,						//__in       DWORD cbBufSize,
							      &dwBytesNeeded			//__out      LPDWORD pcbBytesNeeded
							      ))
	{
		LOG_LASTERROR(L"QueryServiceStatusEx failed to get size needed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if( ERROR_INSUFFICIENT_BUFFER != GetLastError() || dwBytesNeeded < 1)
		{
			LOG_LASTERROR(L"QueryServiceStatusEx failed to get size needed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
	}

	if(iReturnCode == DIAGLIB_OK) 
	{
		//Allocate the buffer
		dwBufSize = dwBytesNeeded + 0x10; 
		if(NULL == (pBuf  = (PUCHAR) malloc(dwBufSize)))
		{
			LOG_ERROR(L"Memory allocation failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if( 0 == QueryServiceStatusEx(	schService,				//__in       SC_HANDLE hService,
											SC_STATUS_PROCESS_INFO,	//__in       SC_STATUS_TYPE InfoLevel,
											pBuf,					//__out_opt  LPBYTE lpBuffer,
											dwBufSize,				//__in       DWORD cbBufSize,
											&dwBytesNeeded			//__out      LPDWORD pcbBytesNeeded
											))
		{
			switch( GetLastError() ) 
			{
			case NO_ERROR:
			case ERROR_SERVICE_NOT_ACTIVE:
			case ERROR_SHUTDOWN_IN_PROGRESS:
				break;
			default:
				LOG_LASTERROR(L"QueryServiceStatusEx failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			}
		}
		else
		{
			pStatus = (SERVICE_STATUS_PROCESS*)pBuf; 
			info->Status=pStatus->dwCurrentState;
			info->StatusLabel=serviceGetStatusLabel(info->Status);
			info->ProcessId=pStatus->dwProcessId;
		}
		if(pBuf) free(pBuf);
	}

	//QueryServiceConfig 
	QUERY_SERVICE_CONFIG *pConfig = NULL;
	//First call to get the size of the buffer
	if( 0 != QueryServiceConfig(schService,				//__in       SC_HANDLE hService,
							      NULL,					//__out_opt  LPQUERY_SERVICE_CONFIG lpServiceConfig,
							      0,					//__in       DWORD cbBufSize,
							      &dwBytesNeeded		//__out      LPDWORD pcbBytesNeeded
							      ))
	  
	{
		LOG_LASTERROR(L"QueryServiceConfig failed to get size needed");
		iReturnCode = DIAGLIB_ERR_INTERNAL;
	}
	else
	{
		if( ERROR_INSUFFICIENT_BUFFER != GetLastError() || dwBytesNeeded < 1)
		{
			LOG_LASTERROR(L"QueryServiceConfig failed to get size needed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
	}

	if(iReturnCode == DIAGLIB_OK) 
	{
		//Allocate the buffer
		dwBufSize = dwBytesNeeded; 
		if(NULL == (pConfig  = (QUERY_SERVICE_CONFIG *) malloc(dwBufSize)))
		{
			LOG_ERROR(L"Memory allocation failed");
			iReturnCode = DIAGLIB_ERR_INTERNAL;
		}
		else if( 0 == QueryServiceConfig(	schService,		//__in       SC_HANDLE hService,
											pConfig,		//__out_opt  LPQUERY_SERVICE_CONFIG lpServiceConfig,
											dwBufSize,		//__in       DWORD cbBufSize,
											&dwBytesNeeded	//__out      LPDWORD pcbBytesNeeded
											))
		{
			switch( GetLastError() ) 
			{
			case NO_ERROR:
			case ERROR_SERVICE_NOT_ACTIVE:
			case ERROR_SHUTDOWN_IN_PROGRESS:
				break;
			default:
				LOG_LASTERROR(L"QueryServiceConfig failed");
				iReturnCode = DIAGLIB_ERR_INTERNAL;
				break;
			}
		}
		else
		{
			info->DisplayName=pConfig->lpDisplayName;
		}
		if(pConfig) free(pConfig);
	}

	return iReturnCode;
}