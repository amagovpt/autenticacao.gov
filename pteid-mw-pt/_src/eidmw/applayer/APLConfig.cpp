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
#include "APLConfig.h"

#include "CardLayer.h"

#include "MWException.h"
#include "eidErrors.h"
#include "Config.h"
#include "Util.h"
#include "Log.h"
#include "APLReader.h"
#include "APLCardPteid.h"

#ifdef WIN32
	#include "Winhttp.h"

	typedef BOOL (WINAPI *FCTDEF_WinHttpGetIEProxyConfigForCurrentUser)
						(
							WINHTTP_CURRENT_USER_IE_PROXY_CONFIG *
						);

	typedef HINTERNET (WINAPI *FCTDEF_WinHttpOpen)
						(
							IN LPCWSTR pwszUserAgent,
							IN DWORD   dwAccessType,
							IN LPCWSTR pwszProxyName   OPTIONAL,
							IN LPCWSTR pwszProxyBypass OPTIONAL,
							IN DWORD   dwFlags
						);

	typedef BOOL (WINAPI *FCTDEF_WinHttpCloseHandle)
						(
							IN HINTERNET hInternet
						);

	typedef BOOL (WINAPI *FCTDEF_WinHttpDetectAutoProxyConfigUrl)
						(
							__in  DWORD     dwAutoDetectFlags,
							__out LPWSTR *  ppwszAutoConfigUrl
						);

	typedef BOOL (WINAPI *FCTDEF_WinHttpGetProxyForUrl)
						(
							IN  HINTERNET                   hSession,
							IN  LPCWSTR                     lpcwszUrl,
							IN  WINHTTP_AUTOPROXY_OPTIONS * pAutoProxyOptions,
							OUT WINHTTP_PROXY_INFO *        pProxyInfo
						);
#endif

#ifdef __APPLE__
	#include <CoreFoundation/CFDictionary.h>
	#include <SystemConfiguration/SCDynamicStoreCopySpecific.h>
	//#include <CFNetwork/CFHTTPStream.h>
	#include <CFHTTPStream.h>
#endif

namespace eIDMW
{

void getProxySystemWide(const wchar_t *host_default, long port_default, const wchar_t *pac_default, std::wstring *host, long *port, std::wstring *pac);
void getHostAndPortFromSettings(const wchar_t *cwSettings, std::wstring *host, long *port);
void getHostAndPortFromSettings(const wchar_t *cwSettings, std::string *host, std::string *port);
void getHostAndPortFromSettings(const char *csSettings, std::string *host, std::string *port);

std::string getHostFromUrl(const char *csUrl);


/*****************************************************************************************
------------------------------------ APL_Config ---------------------------------------
*****************************************************************************************/
APL_Config::APL_Config(const CConfig::Param_Str param)
{
	m_eBehaviour=NORMAL;

	m_name=param.csParam;
	m_section=param.csSection;

	m_strdefvalue=param.csDefault;
	m_ldefvalue=0;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=false;
}

APL_Config::APL_Config(const CConfig::Param_Num param)
{
	m_eBehaviour=NORMAL;

	m_name=param.csParam;
	m_section=param.csSection;

	m_strdefvalue=L"";
	m_ldefvalue=param.lDefault;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=true;
}

APL_Config::APL_Config(const char *csName, const char *czSection, const char *csDefaultValue)
{
	m_eBehaviour=NORMAL;

	m_name=utilStringWiden(csName);
	m_section=utilStringWiden(czSection);

	m_strdefvalue=utilStringWiden(csDefaultValue);
	m_ldefvalue=0;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=false;
}

APL_Config::APL_Config(const char *csName, const wchar_t *czSection, const wchar_t *csDefaultValue)
{
	m_eBehaviour=NORMAL;

	m_name=utilStringWiden(csName);
	m_section=czSection;

	m_strdefvalue=csDefaultValue;
	m_ldefvalue=0;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=false;
}

APL_Config::APL_Config(const char *csName, const char *czSection, long lDefaultValue)
{
	m_eBehaviour=NORMAL;

	m_name=utilStringWiden(csName);
	m_section=utilStringWiden(czSection);

	m_strdefvalue=L"";
	m_ldefvalue=lDefaultValue;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=true;
}

APL_Config::APL_Config(const wchar_t *csName, const wchar_t *czSection, long lDefaultValue)
{
	m_eBehaviour=NORMAL;

	m_name=csName;
	m_section=czSection;

	m_strdefvalue=L"";
	m_ldefvalue=lDefaultValue;

	m_strwvalue=m_strdefvalue;
	m_strvalue=utilStringNarrow(m_strdefvalue);

	m_lvalue=m_ldefvalue;

	m_numtype=true;
}

APL_Config::~APL_Config(void)
{
}

void APL_Config::ChangeLookupBehaviour(tLookupBehaviour eBehaviour)
{
	if(eBehaviour!=NORMAL && eBehaviour!=USER_ONLY && eBehaviour!=SYSTEM_ONLY)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	m_eBehaviour=eBehaviour;
}

const char *APL_Config::getString()
{
	if(m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	if(m_eBehaviour==NORMAL)
		m_strwvalue=CConfig::GetString(m_name,m_section,m_strdefvalue);
	else
		m_strwvalue=CConfig::GetString((m_eBehaviour==SYSTEM_ONLY?CConfig::SYSTEM:CConfig::USER), m_name,m_section,m_strdefvalue);

	getSpecialValue();
	m_strvalue=utilStringNarrow(m_strwvalue);

	return m_strvalue.c_str();
}

const wchar_t *APL_Config::getWString()
{
	if(m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	if(m_eBehaviour==NORMAL)
		m_strwvalue=CConfig::GetString(m_name,m_section,m_strdefvalue);
	else
		m_strwvalue=CConfig::GetString((m_eBehaviour==SYSTEM_ONLY?CConfig::SYSTEM:CConfig::USER), m_name,m_section,m_strdefvalue);

	getSpecialValue();
	m_strvalue=utilStringNarrow(m_strwvalue);

	return m_strwvalue.c_str();
}

long APL_Config::getLong()
{
	if(!m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	if(m_eBehaviour==NORMAL)
		m_lvalue=CConfig::GetLong(m_name,m_section,m_ldefvalue);
	else
		m_lvalue=CConfig::GetLong((m_eBehaviour==SYSTEM_ONLY?CConfig::SYSTEM:CConfig::USER), m_name,m_section,m_ldefvalue);

	getSpecialValue();

	return m_lvalue;
}

void APL_Config::setString(const char *csValue, bool system)
{
	if(m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	m_strvalue=csValue;
	m_strwvalue=utilStringWiden(m_strvalue);

	CConfig::SetString((system?CConfig::SYSTEM:CConfig::USER),m_name,m_section,m_strwvalue);
}

void APL_Config::setWString(const wchar_t *csValue, bool system)
{
	if(m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	m_strwvalue=csValue;
	m_strvalue=utilStringNarrow(m_strwvalue);

	CConfig::SetString((system?CConfig::SYSTEM:CConfig::USER),m_name,m_section,csValue);
}

void APL_Config::setLong(long lValue, bool system)
{
	if(!m_numtype)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);

	CConfig::SetLong((system?CConfig::SYSTEM:CConfig::USER),m_name,m_section,lValue);
}

void APL_Config::getSpecialValue()
{
	if(m_numtype)
	{
		//EIDMW_CNF_PROXY_PORT
		if(wcscmp(m_section.c_str(),EIDMW_CNF_SECTION_PROXY)==0 && wcscmp(m_name.c_str(),EIDMW_CNF_PROXY_PORT)==0)
		{
			//Proxy Host, Port and PacFile are linked together
			//If host = none then the 3 params keep their default value
			//If pac is not the default one, the host and port are.
			std::wstring	host=CConfig::GetString(EIDMW_CNF_PROXY_HOST,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault);
			std::wstring	pac =CConfig::GetString(EIDMW_CNF_PROXY_PACFILE,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault);
			if(host.compare(L"none")==0)
			{
				m_lvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault;
			}
			else if(pac.compare(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault)!=0)
			{
				m_lvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault;
			}
			else
			{
				//If there are the default values, we check if there is special value
				if(host.compare(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault)==0
					&& m_lvalue==m_ldefvalue)
					getProxySystemWide(L"", m_ldefvalue, L"", NULL, &m_lvalue, NULL);
			}
		}
	}
	else
	{
		//EIDMW_CNF_PROXY_HOST
		if(wcscmp(m_section.c_str(),EIDMW_CNF_SECTION_PROXY)==0 && wcscmp(m_name.c_str(),EIDMW_CNF_PROXY_HOST)==0)
		{
			//Proxy Host, Port and PacFile are linked together
			//If host = none then the 3 params keep their default value
			//If pac is not the default one, the host and port are.
			std::wstring	pac =CConfig::GetString(EIDMW_CNF_PROXY_PACFILE,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault);
			long			port=CConfig::GetLong(EIDMW_CNF_PROXY_PORT,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault);
			if(m_strwvalue.compare(L"none")==0)
			{
				m_strwvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault;
			}
			else if(pac.compare(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault)!=0)
			{
				m_strwvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault;
			}
			else
			{
				//If there are the default values, we check if there is special value
				if(m_strwvalue.compare(m_strdefvalue)==0
					&& port==CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault)
					getProxySystemWide(m_strdefvalue.c_str(), 0, L"", &m_strwvalue, NULL, NULL);
			}
		}
		//EIDMW_CNF_PROXY_PACFILE
		else if(wcscmp(m_section.c_str(),EIDMW_CNF_SECTION_PROXY)==0 && wcscmp(m_name.c_str(),EIDMW_CNF_PROXY_PACFILE)==0)
		{
			//Proxy Host, Port and PacFile are linked together
			//If host = none then the 3 params keep their default value
			//If pac is not the default one, the host and port are.
			std::wstring	host=CConfig::GetString(EIDMW_CNF_PROXY_HOST,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault);
			long			port=CConfig::GetLong(EIDMW_CNF_PROXY_PORT,m_section,CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault);
			if(host.compare(L"none")==0)
			{
				m_strwvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault;
			}
			else if(m_strwvalue.compare(m_strdefvalue)!=0)
			{
					m_strwvalue=CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE.csDefault;
			}
			else
			{
				//If there are the default values, we check if there is special value
				if(host.compare(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST.csDefault)==0
					&& port==CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT.lDefault)
					getProxySystemWide(L"", 0, m_strdefvalue.c_str(), NULL, NULL, &m_strwvalue);
			}
		}
	}
}

void getProxySystemWide(const wchar_t *host_default, long port_default, const wchar_t *pac_default, std::wstring *host, long *port, std::wstring *pac)
{
	if(host)
		*host=host_default;

	if(port)
		*port=port_default;

	if(pac)
		*pac=pac_default;

	APL_Config config_useSystemProxy(CConfig::EIDMW_CONFIG_PARAM_PROXY_USE_SYSTEM);

	if (config_useSystemProxy.getLong() != 1)
	{
		MWLOG(LEV_DEBUG, MOD_APL, L"getProxySystemWide(): Skipping because config flag is OFF");
		return;
	}

#ifdef WIN32
	//On windows if there is no proxy parameters, we check if there is some IE settings
	HMODULE hLibrary=LoadLibrary(L"winhttp.dll");
	if(!hLibrary)
		return;

	MWLOG(LEV_DEBUG, MOD_APL, "getProxySystemWide() was called for %s", pac != NULL ? "pacfile" : "host/port");

 	FCTDEF_WinHttpDetectAutoProxyConfigUrl			WinHttpDetectAutoProxyConfigUrl			= (FCTDEF_WinHttpDetectAutoProxyConfigUrl)			GetProcAddress(hLibrary, "WinHttpDetectAutoProxyConfigUrl");
	FCTDEF_WinHttpGetIEProxyConfigForCurrentUser	WinHttpGetIEProxyConfigForCurrentUser	= (FCTDEF_WinHttpGetIEProxyConfigForCurrentUser)	GetProcAddress(hLibrary, "WinHttpGetIEProxyConfigForCurrentUser");

	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig;

	// If the function address is valid, call the function.

	if (NULL != WinHttpGetIEProxyConfigForCurrentUser)
	{
		if(WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
		{
			LPWSTR lpszPacUrl=NULL;
			LPWSTR lpszAutoDetectUrl=NULL;


			if (proxyConfig.lpszAutoConfigUrl != NULL)
			{
				MWLOG(LEV_DEBUG, MOD_APL, "Getting PAC URL (manual configuration)");
				lpszPacUrl = proxyConfig.lpszAutoConfigUrl;
			}
			else if (proxyConfig.fAutoDetect)
			{
				MWLOG(LEV_DEBUG, MOD_APL, "Getting PAC URL from WinHttpDetectAutoProxyConfigUrl()");
				//If autodetect is checked, we try to get the pacfile with WinHttpDetectAutoProxyConfigUrl
				if(WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DHCP, &lpszAutoDetectUrl))
				{
					
					if (lpszAutoDetectUrl != NULL)
					{
						lpszPacUrl = lpszAutoDetectUrl;
						MWLOG(LEV_DEBUG, MOD_APL, L"PAC URL obtained via DHCP: %s", lpszAutoDetectUrl);
					}
				}
				else
				{
					unsigned long WinHttpErr = GetLastError();

					MWLOG(LEV_DEBUG, MOD_APL, "Failed to retrieve proxy PAC URL (DHCP). WinHttpDetectAutoProxyConfigUrl Error=%d", WinHttpErr);
				}
				lpszAutoDetectUrl = NULL;

				if (WinHttpDetectAutoProxyConfigUrl(WINHTTP_AUTO_DETECT_TYPE_DNS_A, &lpszAutoDetectUrl))
				{

					if (lpszAutoDetectUrl != NULL)
					{
						lpszPacUrl = lpszAutoDetectUrl;
						MWLOG(LEV_DEBUG, MOD_APL, L"PAC URL obtained via DNS: %s", lpszAutoDetectUrl);
					}
				}
				else
				{
					unsigned long WinHttpErr = GetLastError();

					MWLOG(LEV_DEBUG, MOD_APL, "Failed to retrieve proxy PAC URL (DNS). WinHttpDetectAutoProxyConfigUrl Error=%d", WinHttpErr);
				}
			}

			//If there is a pac file, host and port keep their default value
			if (lpszPacUrl != NULL)
			{
				if(pac)
					*pac=lpszPacUrl;
			}
			else if (proxyConfig.lpszProxy != NULL)
			{
				if(host || port) //if the host and port are NULL, the folowing call is useless
					getHostAndPortFromSettings(proxyConfig.lpszProxy,host,port);
			}

			if (lpszAutoDetectUrl) GlobalFree( lpszAutoDetectUrl );
			if (proxyConfig.lpszProxy) GlobalFree( proxyConfig.lpszProxy );
			if (proxyConfig.lpszProxyBypass) GlobalFree( proxyConfig.lpszProxyBypass );
			if (proxyConfig.lpszAutoConfigUrl) GlobalFree( proxyConfig.lpszAutoConfigUrl );
		}
	}

	FreeLibrary(hLibrary);
#endif

#ifdef __APPLE__
    Boolean             result;
    CFDictionaryRef     proxyDict;
    CFStringRef         hostStr;
	char				hostChar[255];
    CFNumberRef         portNum;
    long                portLong;

    // Get the dictionary.

    proxyDict = SCDynamicStoreCopyProxies(NULL);
    result = (proxyDict != NULL);

    // Get the proxy host.  DNS names must be in ASCII.  If you
    // put a non-ASCII character  in the "Secure Web Proxy"
    // field in the Network preferences panel, the CFStringGetCString
    // function will fail and this function will return false.

    if (result) {
        hostStr = (CFStringRef) CFDictionaryGetValue(proxyDict,
                    kCFStreamPropertyHTTPProxyHost);

        result = (hostStr != NULL)
            && (CFGetTypeID(hostStr) == CFStringGetTypeID());
    }
    if (result) {
        result = CFStringGetCString(hostStr, hostChar,
            (CFIndex) 255, kCFStringEncodingASCII);
    }

    // Get the proxy port.

    if (result) {
        portNum = (CFNumberRef) CFDictionaryGetValue(proxyDict,
                kCFStreamPropertyHTTPProxyPort);

        result = (portNum != NULL)
            && (CFGetTypeID(portNum) == CFNumberGetTypeID());
    }
    if (result) {
        result = CFNumberGetValue(portNum, kCFNumberIntType, &portLong);
    }

    // Clean up.

    if (proxyDict != NULL) {
        CFRelease(proxyDict);
    }
    if ( result ) {
		if(host)
			*host=utilStringWiden(hostChar).c_str();
		if(port)
			*port=portLong;
    }
#endif
}
bool GetProxyFromPac(const char *csPacFile,const char *csUrl, std::string *proxy_host, std::string *proxy_port)
{
	bool bReturn=false;

	proxy_host->clear();
	proxy_port->clear();

#ifdef WIN32
	MWLOG(LEV_DEBUG, MOD_APL, "GetProxyFromPac() was called for URL: %s, PAC URL: %s", csUrl, csPacFile);
	HMODULE hLibraryWinHttp=LoadLibrary(L"winhttp.dll");
	if(!hLibraryWinHttp)
		return bReturn;

 	FCTDEF_WinHttpOpen								WinHttpOpen								= (FCTDEF_WinHttpOpen)								GetProcAddress(hLibraryWinHttp, "WinHttpOpen");
 	FCTDEF_WinHttpCloseHandle						WinHttpCloseHandle						= (FCTDEF_WinHttpCloseHandle)						GetProcAddress(hLibraryWinHttp, "WinHttpCloseHandle");
 	FCTDEF_WinHttpGetProxyForUrl					WinHttpGetProxyForUrl					= (FCTDEF_WinHttpGetProxyForUrl)					GetProcAddress(hLibraryWinHttp, "WinHttpGetProxyForUrl");

	std::wstring wsPacFile = utilStringWiden(csPacFile);

	HINTERNET WinHttpSession = WinHttpOpen(L"PTEID MiddleWare",
											WINHTTP_ACCESS_TYPE_NO_PROXY,
											WINHTTP_NO_PROXY_NAME,
											WINHTTP_NO_PROXY_BYPASS,
											0);

	 //csPacFile must contain http or https uri (like L"http://127.0.0.1/proxy.pac")

     WINHTTP_AUTOPROXY_OPTIONS  ProxyOptions;
     ProxyOptions.dwFlags					= WINHTTP_AUTOPROXY_CONFIG_URL;
     ProxyOptions.dwAutoDetectFlags			= 0;
	 ProxyOptions.lpszAutoConfigUrl			= wsPacFile.c_str();
	 ProxyOptions.fAutoLogonIfChallenged	= FALSE;
	 ProxyOptions.dwReserved				= 0;
	 ProxyOptions.lpvReserved				= NULL;

     WINHTTP_PROXY_INFO ProxyInfo;
	 memset(&ProxyInfo,0,sizeof(WINHTTP_PROXY_INFO));

	 std::wstring wsUrl = utilStringWiden(csUrl);
	 if( WinHttpGetProxyForUrl( WinHttpSession, wsUrl.c_str(), &ProxyOptions,&ProxyInfo ) )
	 {
		if (ProxyInfo.lpszProxy != NULL)
		{
			getHostAndPortFromSettings(ProxyInfo.lpszProxy,proxy_host,proxy_port);
		}
		bReturn=true;
	 }
	 else
	 {
        unsigned long WinHttpErr = GetLastError();

        MWLOG(LEV_ERROR, MOD_APL
                , "Failed to retrieve proxy info from pac file (%s). WinHttpGetProxyForUrl Error=%d"
                , csPacFile, WinHttpErr );

        if (ProxyInfo.lpszProxy != NULL){
            std::string host = "";
            std::string port = "";

            getHostAndPortFromSettings(ProxyInfo.lpszProxy, &host, &port);

            MWLOG(LEV_ERROR, MOD_APL
                    , "Failed to retrieve proxy info: URL: %s, host: %s, port: %s"
                    , csUrl
                    , host.c_str()
                    , port.c_str() );
        }
    }

     WinHttpCloseHandle(WinHttpSession);

	if(hLibraryWinHttp)		FreeLibrary(hLibraryWinHttp);

	return bReturn;
#endif

#ifdef __APPLE__
#endif

	return bReturn;
}

void getHostAndPortFromSettings(const wchar_t *cwSettings, std::wstring *host, long *port)
{
	if(!cwSettings)
		return;

	//There is different kinds of settings
	// - Case 1 : Simple settings like : 127.0.0.1:8080
	// - Case 2 : Advanced settings like : ftp=127.0.0.2:8082;http=127.0.0.1:8080;https=127.0.0.3:8083
	// - Case 3 : Multi settings like (from pac file and WinHttp api) : 127.0.0.1:8080;127.0.0.2:8081
	// - Case 4 : Multi settings like (from pac file and PacParser api) :  PROXY 127.0.0.1:8080;PROXY 127.0.0.2:8081; DIRECT
	//We just try to get the http part

	wchar_t * buffer = new wchar_t[wcslen(cwSettings)+1];
	wcscpy_s(buffer,wcslen(cwSettings)+1,cwSettings);

	wchar_t * sHost = buffer;

//CASE 2
	wchar_t * sHttp = wcsstr(sHost,L"http=");
	if(sHttp)
	{
		sHttp += 5;
		sHost = sHttp;
	}

	//We stop at the first ';' (if none, we take the whole string into account)
	wchar_t * sEnd = wcschr(sHost,L';');
	if(sEnd)
	{
		*sEnd = '\0';
	}

//CASE 4
	wchar_t *sDirect = wcsstr(sHost,L"DIRECT ");
	if(!sDirect)
	{
		wchar_t *sProxy = wcsstr(sHost,L"PROXY ");
		if(sProxy)
		{
			sProxy += 6;
			sHost = sProxy;
		}
	}
	else
	{
		*sHost='\0';
	}

	wchar_t * sPort = wcschr(sHost,L':');
	if(sPort)
	{
		*sPort = '\0';
		sPort++;

		if(host)
			*host=sHost;

		if(port)
		{
			wchar_t *stop;
			*port=wcstol(sPort,&stop,10);
		}
	}

	delete[] buffer;
}

void getHostAndPortFromSettings(const wchar_t *cwSettings, std::string *host, std::string *port)
{
	if(!cwSettings)
		return;

	std::wstring wsHost;
	long lPort;

	getHostAndPortFromSettings(cwSettings,&wsHost,&lPort);

	host->assign(utilStringNarrow(wsHost));
	char buffer[10];
	sprintf_s(buffer,sizeof(buffer),"%ld",lPort);
	port->assign(buffer);
}

void getHostAndPortFromSettings(const char *csSettings, std::string *host, std::string *port)
{
	if(!csSettings)
		return;

	std::wstring cwSettings=utilStringWiden(csSettings);
	getHostAndPortFromSettings(cwSettings.c_str(),host,port);
}

std::string getHostFromUrl(const char *csUrl)
{
	std::string sHostOut;

	char *sHost = new char[strlen(csUrl)+1];
	strcpy_s(sHost,strlen(csUrl)+1,csUrl);

	char *pHost = strstr(sHost,"//");
	if(!pHost)
	{
		pHost = strstr(sHost,"\\\\");
	}
	if(pHost)
	{
		pHost += 2;
		char * pEnd = strchr(pHost,L'/');
		if(pEnd)
		{
			*pEnd = '\0';
		}
		else
		{
			char * pEnd = strchr(pHost,L'\\');
			if(pEnd)
			{
				*pEnd = '\0';
			}
		}
	}
	else
	{
		sHost[0] = '\0';
		pHost=sHost;
	}

	sHostOut=pHost;

	delete[] sHost;

	return sHostOut;
}

}
