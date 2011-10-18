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

#include "middleware.h"
#include "error.h"
#include "log.h"
#include "progress.h"

#include "folder.h"
#include "beidlib.h"

typedef enum e_MW_INSTALLTYPE {
	INSTALLTYPE_MSI,
	INSTALLTYPE_IS,
	INSTALLTYPE_IS_MSI
} MW_INSTALLTYPE;

//For uninstall future use
#define IDR_ISS_24  0
#define IDR_ISS_25  0
#define IDR_ISS_26  0
#define IDR_ISS_30  0
#define IDR_ISS_35  0
#define IDR_ISS_35P 0

typedef struct t_MW_DEFINITION {
	MW_VERSION Version;
	std::wstring Label;
	std::wstring Guid;
	MW_INSTALLTYPE TypeInstall;
	int UninstallResource;
} MW_DEFINITION;

const MW_DEFINITION g_MW_Definition[] =
{
	{MW_VERSION_23,					L"2.3",						L"{44CFED0B-BF92-455B-94D3-FA967A81712E}", INSTALLTYPE_MSI,		0			},
	{MW_VERSION_24,					L"2.4",						L"{BA42ACEA-3782-4CAD-AA10-EBC2FA14BB7E}", INSTALLTYPE_IS_MSI,	IDR_ISS_24	},
	{MW_VERSION_25,					L"2.5",						L"{85D158F2-648C-486A-9ECC-C5D4F4ACC965}", INSTALLTYPE_IS_MSI,	IDR_ISS_25	},
	{MW_VERSION_26,					L"2.6",						L"{EA248851-A7D5-4906-8C46-A3CA267F6A24}", INSTALLTYPE_IS_MSI,	IDR_ISS_26	},	
	{MW_VERSION_30,					L"3.0",						L"{82493A8F-7125-4EAD-8B6D-E9EA889ECD6A}", INSTALLTYPE_IS,		IDR_ISS_30	},
	{MW_VERSION_35_IS,				L"3.5 \"IS version\"",		L"{40420E84-2E4C-46B2-942C-F1249E40FDCB}", INSTALLTYPE_IS,		IDR_ISS_35	},
	{MW_VERSION_35_PRO_IS,			L"3.5 Pro \"IS version\"",	L"{4C2FBD23-962C-450A-A578-7556BC79B8B2}", INSTALLTYPE_IS,		IDR_ISS_35P },
	{MW_VERSION_35_MSI,				L"3.5",						L"{824563DE-75AD-4166-9DC0-B6482F2DED5A}", INSTALLTYPE_MSI,		0			},	
	{MW_VERSION_35_PRO_MSI,			L"3.5 Pro",					L"{FBB5D096-1158-4e5e-8EA3-C73B3F30780A}", INSTALLTYPE_MSI,		0			},
	{MW_VERSION_351_OR_GREATER,		L"3.5",						L"{824563DE-75AD-4166-9DC0-B6482F2?????}", INSTALLTYPE_MSI,		0			},
	{MW_VERSION_351_PRO_OR_GREATER,	L"3.5 Pro",					L"{FBB5D096-1158-4e5e-8EA3-C73B3F3?????}", INSTALLTYPE_MSI,		0			}
};

#define MW_DEFINITION_COUNT sizeof(g_MW_Definition)/sizeof(MW_DEFINITION)


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwFillList(MW_LIST *middlewaretList);
int getVersionFromGuid(Soft_GUID guid);
void getLabelVersion(Soft_GUID guid, int installVersion, std::wstring *label);
int checkStringPattern(const wchar_t *wzPattern, const wchar_t *wzString); //define in win_registry.cpp

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwGetInfo (MW_ID middleware, MW_INFO *info)
{
	int iReturnCode = DIAGLIB_OK;

	if(info == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	info->id=middleware;
	info->Version=g_MW_Definition[getVersionFromGuid(middleware.Guid)].Version;

	Soft_INFO softInfo;
	if(DIAGLIB_OK != (iReturnCode=softwareGetInfo(middleware,&softInfo)))
	{
		LOG_ERROR(L"softwareGetInfo failed");
		return iReturnCode;
	}

	info->BuildNumber=softInfo.Version & 0xFFFF;
	info->DisplayName=softInfo.DisplayName;

	getLabelVersion(info->id.Guid, softInfo.Version, &info->LabelVersion);
	

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int mwUninstall (MW_ID middleware)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int mwFillList(MW_LIST *middlewaretList)
{
	int iReturnCode = DIAGLIB_OK;

	if(middlewaretList == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	middlewaretList->clear();

	progressInit(MW_DEFINITION_COUNT);
	
	MW_ID id;

	Soft_LIST softList;

	for(int i=0;i<MW_DEFINITION_COUNT;i++)
	{
		if( DIAGLIB_OK != (iReturnCode = softwareGetIDs(g_MW_Definition[i].Guid, &softList)))
		{
			LOG_ERROR(L"softwareGetIDs failed");
			break;
		}

		Soft_LIST::const_iterator itrSoft;
		for(itrSoft=softList.begin();itrSoft!=softList.end();itrSoft++)
		{
			id=*itrSoft;
			
			//The maching patern can include a Middleware that has already been added to the list,
			//so we check the existence in the middleware list first
			bool bFound=false;
			MW_LIST::const_iterator itrMw;
			for(itrMw=middlewaretList->begin();itrMw!=middlewaretList->end();itrMw++)
			{
				if(id.Guid.compare(itrMw->Guid)==0 && id.Type==itrMw->Type)
				{
					bFound=true;
					break;
				}
			}

			if(!bFound)
			{
				middlewaretList->push_back(id);
			}
		}

		progressIncrement();
	}

	progressRelease();

	return iReturnCode;

}

////////////////////////////////////////////////////////////////////////////////////////////////
int getVersionFromGuid(Soft_GUID guid)
{
	for(int i=0;i<MW_DEFINITION_COUNT;i++)
	{
		if(checkStringPattern(g_MW_Definition[i].Guid.c_str(),guid.c_str()))
			return i;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void getLabelVersion(Soft_GUID guid, int installVersion, std::wstring *label)
{
	label->clear();

	int idx = getVersionFromGuid(guid);
	if(idx<0)
	{
		label->append(L"???");
		return;
	}

	wchar_t buff[10];
	
	int major=(installVersion & 0xFF000000)/0x1000000;
	int minor=(installVersion & 0x00FF0000)/0x10000;
	int build=installVersion & 0x0000FFFF;
	
	if(g_MW_Definition[idx].Version==MW_VERSION_351_OR_GREATER)
	{
		if( -1 == swprintf_s(buff, 10, L"%ld.%ld",major,minor))
		{
			label->append(L"???");
			return;
		}		
		label->append(buff);
	}
	else if(g_MW_Definition[idx].Version==MW_VERSION_351_PRO_OR_GREATER)
	{
		if( -1 == swprintf_s(buff, 10, L"%ld.%ld Pro",major,minor))
		{
			label->append(L"???");
			return;
		}		
		label->append(buff);
	}
	else
	{
		label->append(g_MW_Definition[idx].Label);
	}

	if(build > 0)
	{
		if( -1 == swprintf_s(buff, 10, L"%ld",build))
		{
			label->append(L" (build ????)");
			return;
		}

		label->append(L" (build ");
		label->append(buff);
		label->append(L")");
	}
}
