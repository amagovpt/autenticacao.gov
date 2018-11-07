#include "proxyinfo.h"
#include "eidlib.h"
#include "eidlibdefines.h"

ProxyInfo::ProxyInfo()
{
	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);
	eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
	eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
	eIDMW::PTEID_Config pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
	eIDMW::PTEID_Config useSystemProxy(eIDMW::PTEID_PARAM_PROXY_USE_SYSTEM);

	std::string proxy_host = config.getString();
	std::string proxy_username = config_username.getString();
	std::string proxy_pwd = config_pwd.getString();
	long proxy_port = config2.getLong();
	const char * pacfile_url = pacfile.getString();

	if (useSystemProxy.getLong() == 1 && pacfile_url != NULL && strlen(pacfile_url) > 0)
	{
		system_proxy = true;
		m_pac_url = QString(pacfile_url);
	}

	if (!proxy_host.empty() && proxy_port != 0)
	{

		m_proxy_host = QString::fromStdString(proxy_host);
		m_proxy_port = QString::number(proxy_port);

		if (!proxy_username.empty())
		{
			m_proxy_user = QString::fromStdString(proxy_username);
			m_proxy_pwd = QString::fromStdString(proxy_pwd);
		}

	}
}

void ProxyInfo::getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port)
{
	if (!system_proxy)
		return;
	std::string proxy_port_str;

	eIDMW::PTEID_GetProxyFromPac(m_pac_url.toUtf8().constData(), urlToFetch.c_str(), proxy_host, &proxy_port_str);

	if (proxy_host->size() > 0 && proxy_port_str.size() > 0)
		*proxy_port = atol(proxy_port_str.c_str());

}