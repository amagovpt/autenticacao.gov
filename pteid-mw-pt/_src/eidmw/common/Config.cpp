/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include <iostream>
#include <string>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

#include "Config.h"
#include "Util.h"
#include "datafile.h"
#include "eidErrors.h"
#include "MWException.h"
#include "prefix.h"

/*
Linux/Mac implementation of CConfig.
All configuration is stored in a single per-user file: ~/.config/pteid.conf
The tLocation parameter is accepted in the API for cross-platform compatibility
with the Windows registry implementation (ConfigReg.cpp), but is ignored all
reads and writes go to the user config file.
*/

namespace eIDMW {

bool bIsInitialized = false;
bool CConfig::bTestModeEnabled = false;

CDataFile CConfig::o_userDataFile;
CMutex CConfig::m_Mutex;

std::wstring home_path;

CConfig::CConfig(void) {}

CConfig::~CConfig() {}

void CConfig::Init() {

	if (!bIsInitialized) {
		home_path = utilStringWiden(std::string(getenv("HOME")));
#ifdef __APPLE__
		const std::wstring user_datafilePath = L"Library/Preferences/";
#else
		const std::wstring user_datafilePath = L".config/";
		mkdir(utilStringNarrow(home_path + L"/" + user_datafilePath).c_str(),
			  0700); // Create the directory if it doesn't exist
#endif

		const std::wstring userFile = home_path + L"/" + user_datafilePath + L"pteid.conf";

		o_userDataFile.SetFileName(userFile);

		bIsInitialized = true;
	}
}

/**
 * Macro expansion:
 *  - $install -> $PREFIX (default: /usr/local)
 *  - $home    -> user home dir
 *  - $common  -> /tmp
 */
std::wstring ExpandSection(const std::wstring &czSectionOriginal) {
	int iResult;
	std::basic_string<char>::size_type iTotLenght = czSectionOriginal.length();
	std::basic_string<char>::size_type iStrLenght;

	//--- Find if anything to expand
	if ((iTotLenght == 0) || (czSectionOriginal[0] != '$')) {
		// nothing to replace
		return (czSectionOriginal);
	}

	//--- check for EIDMW_CNF_MACRO_INSTALL
	iStrLenght = wcslen(EIDMW_CNF_MACRO_INSTALL);
	iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_INSTALL);
	if (iResult == 0) {
		// replace EIDMW_CNF_MACRO_INSTALL
		std::wstring czSectionExpanded(utilStringWiden(STRINGIFY(EIDMW_PREFIX)));

		// add part after the $-macro
		czSectionExpanded.append(
			czSectionOriginal.substr(iStrLenght, iTotLenght - iStrLenght)); // add part after the $-macro
		return (czSectionExpanded);
	}

	//--- check for EIDMW_CNF_MACRO_HOME
	iStrLenght = wcslen(EIDMW_CNF_MACRO_HOME);
	iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_HOME);
	if (iResult == 0) {
		std::wstring czSectionExpanded(home_path);

		// add part after the $-macro
		czSectionExpanded.append(
			czSectionOriginal.substr(iStrLenght, iTotLenght - iStrLenght)); // add part after the $-macro
		return (czSectionExpanded);
	}

	//--- check for EIDMW_CNF_MACRO_COMMON
	iStrLenght = wcslen(EIDMW_CNF_MACRO_COMMON);
	iResult = czSectionOriginal.compare(0, iStrLenght, EIDMW_CNF_MACRO_COMMON);
	if (iResult == 0) {
		std::wstring czSectionExpanded(L"/tmp");

		// add part after the $-macro
		czSectionExpanded.append(
			czSectionOriginal.substr(iStrLenght, iTotLenght - iStrLenght)); // add part after the $-macro
		return (czSectionExpanded);
	}

	return (czSectionOriginal);
}

std::wstring CConfig::GetStringInt(tLocation location, const std::wstring &csName, const std::wstring &csSection,
								   bool bExpand) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	std::wstring csResult = o_userDataFile.GetString(csName, csSection);

	if (csResult != L"")
		return bExpand ? ExpandSection(csResult) : csResult;

	throw CMWEXCEPTION(EIDMW_CONF);
}

std::wstring CConfig::GetStringInt(const std::wstring &csName, const std::wstring &csSection, bool bExpand) {
	return GetStringInt(CConfig::USER, csName, csSection, bExpand);
}

// std::wstring CConfig::GetString(t_Str szKey, t_Str szSection)
std::wstring CConfig::GetString(const Param_Str param) {
	return (GetString(param.csParam, param.csSection, param.csDefault, true));
}

std::wstring CConfig::GetString(const std::wstring &csName, const std::wstring &szSection) {
	return GetStringInt(csName, szSection, true);
}

std::wstring CConfig::GetString(const std::wstring &csName, const std::wstring &czSection,
								const std::wstring &csDefaultValue, bool bExpand) {
	try {
		return GetStringInt(csName, czSection, bExpand);
	} catch (...) {
		return bExpand ? ExpandSection(csDefaultValue) : csDefaultValue;
	}
};

std::wstring CConfig::GetString(tLocation location, const Param_Str param) {
	return (GetString(location, param.csParam, param.csSection, param.csDefault));
}

std::wstring CConfig::GetString(tLocation location, const std::wstring &csName, const std::wstring &szSection) {
	return GetStringInt(location, csName, szSection, true);
}

std::wstring CConfig::GetString(tLocation location, const std::wstring &csName, const std::wstring &czSection,
								const std::wstring &csDefaultValue, bool bExpand) {
	try {
		return GetStringInt(location, csName, czSection, bExpand);
	} catch (...) {
		return bExpand ? ExpandSection(csDefaultValue) : csDefaultValue;
	}
};

long CConfig::GetLong(tLocation location, const Param_Num param) {
	return (GetLong(location, param.csParam, param.csSection, param.lDefault));
}

long CConfig::GetLong(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	long lResult = o_userDataFile.GetLong(csName, czSection);

	if (lResult != LONG_MIN)
		return lResult;

	throw CMWEXCEPTION(EIDMW_CONF);
};

long CConfig::GetLong(tLocation location, const std::wstring &csName, const std::wstring &czSection,
					  long lDefaultValue) {

	try {
		return GetLong(location, csName, czSection);
	} catch (...) {
		return lDefaultValue;
	}
};

long CConfig::GetLong(const Param_Num param) { return (GetLong(param.csParam, param.csSection, param.lDefault)); }

long CConfig::GetLong(const std::wstring &csName, const std::wstring &czSection) {
	return GetLong(CConfig::USER, csName, czSection);
};

long CConfig::GetLong(const std::wstring &csName, const std::wstring &czSection, long lDefaultValue) {

	try {
		return GetLong(csName, czSection);
	} catch (...) {
		return lDefaultValue;
	}
};

void CConfig::SetString(tLocation location, const struct Param_Str param, const std::wstring &csValue) {
	return (SetString(location, param.csParam, param.csSection, csValue));
}

void CConfig::SetString(tLocation location, const std::wstring &csName, const std::wstring &czSection,
						const std::wstring &csValue) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	o_userDataFile.SetValue(csName, csValue, L"", czSection);
	if (!o_userDataFile.Save())
		throw CMWEXCEPTION(EIDMW_CONF);
};

void CConfig::DeleteKeysByPrefix(tLocation location, const struct Param_Str param) {
	return (DeleteKeysByPrefix(location, param.csParam, param.csSection));
};

void CConfig::DeleteKeysByPrefix(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	o_userDataFile.DeleteKeysByPrefix(csName, czSection);
	if (!o_userDataFile.Save())
		throw CMWEXCEPTION(EIDMW_CONF);
};

unsigned int CConfig::CountKeysByPrefix(tLocation location, const struct Param_Str param) {
	return (CountKeysByPrefix(location, param.csParam, param.csSection));
};

unsigned int CConfig::CountKeysByPrefix(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	unsigned int count = o_userDataFile.CountKeysByPrefix(csName, czSection);
	if (!o_userDataFile.Save())
		throw CMWEXCEPTION(EIDMW_CONF);

	return count;
};

void CConfig::SetLong(tLocation location, const struct Param_Num param, long lValue) {
	return (SetLong(location, param.csParam, param.csSection, lValue));
}

void CConfig::SetLong(tLocation location, const std::wstring &csName, const std::wstring &czSection, long lValue) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	o_userDataFile.SetLong(csName, lValue, L"", czSection);
	if (!o_userDataFile.Save())
		throw CMWEXCEPTION(EIDMW_CONF);
};

void CConfig::DelString(tLocation location, const struct Param_Str param) {
	return (DelString(location, param.csParam, param.csSection));
}

void CConfig::DelString(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
	CAutoMutex autoMutex(&m_Mutex);

	if (!bIsInitialized)
		Init();

	if (!o_userDataFile.DeleteKey(csName, czSection))
		throw CMWEXCEPTION(EIDMW_CONF);
	if (!o_userDataFile.Save())
		throw CMWEXCEPTION(EIDMW_CONF);
};

void CConfig::DelLong(tLocation location, const struct Param_Num param) {
	return (DelLong(location, param.csParam, param.csSection));
}

void CConfig::DelLong(tLocation location, const std::wstring &csName, const std::wstring &czSection) {
	DelString(location, csName, czSection);
};

} // namespace eIDMW
