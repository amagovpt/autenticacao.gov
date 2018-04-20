#ifndef _PROXYINFO_H
#define _PROXYINFO_H

#include <QString>
#include <string>

class ProxyInfo
{
public:
	ProxyInfo();
	QString getProxyHost() {
		return m_proxy_host;
	}
	QString getProxyPort() {
		return m_proxy_port;
	}

	QString getProxyUser() { return m_proxy_user; }

	QString getProxyPwd() { return m_proxy_pwd; }

	bool isSystemProxy() { return system_proxy;  }

	void getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port);

private:
	bool system_proxy;
	QString m_pac_url;
	QString m_proxy_host;
	QString m_proxy_port;
	QString m_proxy_user;
	QString m_proxy_pwd;

};

#endif