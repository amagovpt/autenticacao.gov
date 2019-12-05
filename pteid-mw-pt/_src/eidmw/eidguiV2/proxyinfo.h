/*-****************************************************************************

 * Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <QString>
#include <string>

class ProxyInfo
{
public:
	ProxyInfo();
	QString getProxyHost();

	QString getProxyPort();

	QString getProxyUser() { return m_proxy_user; }

	QString getProxyPwd() { return m_proxy_pwd; }

	bool isSystemProxy() { return system_proxy;  }

	bool isAutoConfig() { return auto_configured; }

	bool isManualConfig() { return manual_configured; }

	void getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port);

private:
	bool auto_configured;
	bool manual_configured;
	bool system_proxy;
	QString m_pac_url;
	QString m_proxy_host;
	QString m_proxy_port;
	QString m_proxy_user;
	QString m_proxy_pwd;

};

#endif