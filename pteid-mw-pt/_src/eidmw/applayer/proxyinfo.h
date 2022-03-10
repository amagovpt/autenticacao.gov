/*-****************************************************************************

* Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
* Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <string>
#include "Export.h"

class ProxyInfo
{
public:
    EIDMW_APL_API ProxyInfo();

	// The configs returned are utf8 encoded
    EIDMW_APL_API std::string getProxyHost();

    EIDMW_APL_API std::string getProxyPort();

    EIDMW_APL_API std::string getProxyUser() { return m_proxy_user; }

    EIDMW_APL_API std::string getProxyPwd() { return m_proxy_pwd; }

    EIDMW_APL_API bool isSystemProxy() { return system_proxy;  }

    EIDMW_APL_API bool isAutoConfig() { return auto_configured; }

    EIDMW_APL_API bool isManualConfig() { return manual_configured; }

    EIDMW_APL_API void getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port);

private:
	bool auto_configured;
	bool manual_configured;
	bool system_proxy;
	std::string m_pac_url;
	std::string m_proxy_host;
	std::string m_proxy_port;
	std::string m_proxy_user;
	std::string m_proxy_pwd;
};

#endif
