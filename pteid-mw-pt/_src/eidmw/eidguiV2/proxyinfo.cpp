#include "proxyinfo.h"
#include "eidlib.h"
#include "eidlibdefines.h"

ProxyInfo::ProxyInfo()
{
	eIDMW::PTEID_Config config_host(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config_port(eIDMW::PTEID_PARAM_PROXY_PORT);
	eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
	eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
	eIDMW::PTEID_Config pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
	eIDMW::PTEID_Config useSystemProxy(eIDMW::PTEID_PARAM_PROXY_USE_SYSTEM);

	std::string proxy_host = config_host.getString();
	std::string proxy_username = config_username.getString();
	std::string proxy_pwd = config_pwd.getString();
	long proxy_port = config_port.getLong();
	const char * pacfile_url = pacfile.getString();
	system_proxy = (useSystemProxy.getLong() == 1);
	auto_configured = false;
	manual_configured = false;

	if (!proxy_host.empty() && proxy_port != 0)
	{
		manual_configured = true;
		m_proxy_host = QString::fromStdString(proxy_host);
		m_proxy_port = QString::number(proxy_port);

		if (!proxy_username.empty())
		{
			m_proxy_user = QString::fromStdString(proxy_username);
			m_proxy_pwd = QString::fromStdString(proxy_pwd);
		}
	}

	if (system_proxy && pacfile_url != NULL && strlen(pacfile_url) > 0)
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
			"Proxy configured with pacfile in system.");
		auto_configured = true;
		m_pac_url = QString(pacfile_url);
	}
	else if (manual_configured)
	{
		if (system_proxy)
		{
			eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
				"Proxy configured manually in system: host=%s, port=%ul", proxy_host.c_str(), proxy_port);
		}
		else
		{
			eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
				"Proxy configured manually in application: host=%s, port=%ul", proxy_host.c_str(), proxy_port);
		}
	}
	else
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo", "No proxy configured");
	}
}

QString ProxyInfo::getProxyHost() {
	if (auto_configured)
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_WARNING, "ProxyInfo",
			"getProxyHost: Getting manually configured proxy host when there is also a pacfile configured. \
			To get the proxy from pacfile use the getProxyForHost method.");
	}
	if (!manual_configured)
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ProxyInfo",
			"getProxyHost: Trying to get proxy host but there is not manual configuration.");
		return "";
	}
	return m_proxy_host;
}

QString ProxyInfo::getProxyPort() {
	if (auto_configured)
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_WARNING, "ProxyInfo",
			"getProxyPort: Getting manually configured proxy port when there is also a pacfile configured. \
			To get the proxy from pacfile use the getProxyForHost method.");
	}
	if (!manual_configured)
	{
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ProxyInfo",
			"getProxyPort: Trying to get proxy port but there is not manual configuration.");
		return "";
	}
	return m_proxy_port;
}

void ProxyInfo::getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port)
{
	if (!auto_configured){
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ProxyInfo",
			"getProxyForHost: Trying to get proxy from pacfile but it is not configured.");
		return;
	}
	std::string proxy_port_str;

	eIDMW::PTEID_GetProxyFromPac(m_pac_url.toUtf8().constData(), urlToFetch.c_str(), proxy_host, &proxy_port_str);

	if (proxy_host->size() > 0 && proxy_port_str.size() > 0)
		*proxy_port = atol(proxy_port_str.c_str());

	eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
		"getProxyForHost: Obtained proxy for endpoint %s: proxy_host=%s proxy_port=%ul", urlToFetch.c_str(),
		proxy_host->c_str(), *proxy_port);
}