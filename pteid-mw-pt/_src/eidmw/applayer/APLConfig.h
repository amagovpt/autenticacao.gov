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
#pragma once

#ifndef __APL_CONFIG_H__
#define __APL_CONFIG_H__

#include <string>
#include "Export.h"
#include "Config.h"

namespace eIDMW
{

/******************************************************************************//**
  * Class to access the config parameters
  *********************************************************************************/
class APL_Config
{
public:
	EIDMW_APL_API APL_Config(const CConfig::Param_Str param);
	EIDMW_APL_API APL_Config(const CConfig::Param_Num param);

    EIDMW_APL_API APL_Config(const char *csName, const char *czSection, const char *csDefaultValue);
    EIDMW_APL_API APL_Config(const char *csName, const wchar_t *czSection, const wchar_t *csDefaultValue);

    EIDMW_APL_API APL_Config(const char *csName, const char *czSection, long lDefaultValue);
    EIDMW_APL_API APL_Config(const wchar_t *csName, const wchar_t *czSection, long lDefaultValue);

	EIDMW_APL_API virtual ~APL_Config();				/**< Destructor */


    EIDMW_APL_API const char *getString();
    EIDMW_APL_API const wchar_t *getWString();
    EIDMW_APL_API long getLong();

    EIDMW_APL_API void setString(const char *csValue, bool system=false);
    EIDMW_APL_API void setWString(const wchar_t *csValue, bool system=false);
	EIDMW_APL_API void setLong(long lValue, bool system=false);
    
	enum tLookupBehaviour
    {
		NORMAL=0,
        USER_ONLY,
		SYSTEM_ONLY
    };

	EIDMW_APL_API void ChangeLookupBehaviour(tLookupBehaviour eBehaviour);

private:
	APL_Config(const APL_Config& certif);				/**< Copy not allowed - not implemented */
	APL_Config& operator= (const APL_Config& certif);	/**< Copy not allowed - not implemented */

	void getSpecialValue();

	std::wstring m_name;
	std::wstring m_section;

	std::wstring m_strdefvalue;
	std::wstring m_strwvalue;
	std::string m_strvalue;

	tLookupBehaviour m_eBehaviour;
	
	long m_lvalue;
	long m_ldefvalue;

	bool m_numtype;
};

EIDMW_APL_API bool GetProxyFromPac(const char *csPacFile, const char *csUrl, std::string *proxy_host, std::string *proxy_port);

}

#endif //__APL_CONFIG_H__
