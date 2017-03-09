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
#include "Log.h"
#include "Config.h"
#include "eidErrors.h"
#include "MWException.h"
#include "Util.h"
#include <string>

#define DO_LOGGING



namespace eIDMW
{

tLOG_Level MapLevel(tLevel levelIn)
{
	switch(levelIn)
	{
	case LEV_CRIT:
		return LOG_LEVEL_CRITICAL;
	case LEV_ERROR:
		return LOG_LEVEL_ERROR;
	case LEV_WARN:
		return LOG_LEVEL_WARNING;
	case LEV_INFO:
		return LOG_LEVEL_INFO;
	case LEV_DEBUG:
		return LOG_LEVEL_DEBUG;
	default:
		return LOG_LEVEL_DEFAULT;
	}
}

CLog &MapModule(tModule moduleIn)
{
	std::wstring group;

    switch(moduleIn)
	{
		case MOD_CAL:
			group = L"cardlayer";
			break;
		case MOD_P11:
			group = L"pkcs11";
			break;
		case MOD_LIB:
			group = L"eidlib";
			break;
		case MOD_GUI:
			group = L"eidgui";
			break;
		case MOD_TA:
			group = L"trayapplet";
			break;
		case MOD_DLG:
			group = L"dialog";
			break;
		case MOD_CSP:
			group = L"CSP";
			break;
		case MOD_TEST:
			group = L"unit_test";
			break;
		case MOD_APL:
			group = L"applayer";
			break;
		case MOD_CRL:
			group = L"crlservice";
			break;
		case MOD_SSL:
			group = L"openssl";
			break;
		case MOD_SDK:
			group = L"sdk";
			break;
		case MOD_CMD:
			group = L"CCMovel";
			break;
		default:
			group = L"";
			break;
	}

	return CLogger::instance().getLogW(group.c_str());
}

// MWLOG(tLevel level, tModule mod, const char *format, ...)
bool MWLOG(tLevel level, tModule mod, const wchar_t *format, ...)
{

#ifdef DO_LOGGING

	try
	{
		CLog &log=MapModule(mod);

		va_list args;
		va_start(args, format);

		log.write(MapLevel(level),format,args);

		va_end(args);
	}
	catch(CMWException &e)
	{
		if(e.GetError()!=(long)EIDMW_ERR_LOGGER_APPLEAVING)
			throw e;

		return false;
	}

#endif

	return true;
}

bool MWLOG(tLevel level, tModule mod, const char *format, ...)
{
	try
	{
		CLog &log=MapModule(mod);

		va_list args;
		va_start(args, format);

		log.write(MapLevel(level),format,args);

		va_end(args);
	}
	catch(CMWException &e)
	{
		if(e.GetError()!=(long)EIDMW_ERR_LOGGER_APPLEAVING)
			throw e;

		return false;
	}

	return true;
}

// MWLOG(tLevel level, tModule mod, CMWEXCEPTION theException)
bool MWLOG(tLevel level, tModule mod, CMWException theException)
{

#ifdef DO_LOGGING

	//char buffer[256];
	//sprintf_s(buffer,sizeof(buffer), "Exception thrown at file = %s, line = %s", theException.GetFile(), theException.GetLine());
	//return MWLOG(level, mod, "  %s", buffer);
	try
	{
		CLog &log=MapModule(mod);

		if (theException.GetLine()==0)
		{
			log.write(MapLevel(level),L"Exception 0x%0x thrown", theException.GetError());
		}
		else
		{
			int line = (int) theException.GetLine();
			log.write(MapLevel(level),line,utilStringWiden(theException.GetFile()).c_str(),L"Exception 0x%0x thrown",theException.GetError());
		}
	}
	catch(CMWException &e)
	{
		if(e.GetError()!=(long)EIDMW_ERR_LOGGER_APPLEAVING)
			throw e;

		return false;
	}

#endif

	return true;
}

}
