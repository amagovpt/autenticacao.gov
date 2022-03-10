/*-****************************************************************************

* Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
* Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
* Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "proxyinfo.h"
#include <codecvt>
#include <string.h>
#include "Util.h"
#include "APLConfig.h"
#include "Log.h"

using namespace eIDMW;

ProxyInfo::ProxyInfo()
{
    APL_Config config_host(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);
    APL_Config config_port(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);
    APL_Config config_username(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);
    APL_Config config_pwd(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);
    APL_Config config_pacfile(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);
    APL_Config useSystemProxy(CConfig::EIDMW_CONFIG_PARAM_PROXY_USE_SYSTEM);

#ifdef WIN32
    // in Windows the proxy configs come in utf16. Convert them to utf8:
    std::wstring proxy_hostW = eIDMW::utilStringWiden(config_host.getString());
    std::wstring proxy_usernameW = eIDMW::utilStringWiden(config_username.getString());
    std::wstring proxy_pwdW = eIDMW::utilStringWiden(config_pwd.getString());

    std::wstring proxy_host_u16(proxy_hostW.begin(), proxy_hostW.end());
    std::wstring proxy_username_u16(proxy_usernameW.begin(), proxy_usernameW.end());
    std::wstring proxy_pwd_16(proxy_pwdW.begin(), proxy_pwdW.end());

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    std::string proxy_host = convert.to_bytes(proxy_host_u16);
    std::string proxy_username = convert.to_bytes(proxy_username_u16);
    std::string proxy_pwd = convert.to_bytes(proxy_pwd_16);

    std::string pacfile;
    if (config_pacfile.getString()) {
        std::wstring pacfileW = eIDMW::utilStringWiden(config_pacfile.getString());
        std::wstring pacfile_16(pacfileW.begin(), pacfileW.end());
        pacfile = convert.to_bytes(pacfile_16);
    }

#else
    std::string proxy_host = config_host.getString();
    std::string proxy_username = config_username.getString();
    std::string proxy_pwd = config_pwd.getString();
    std::string pacfile;
    if (config_pacfile.getString()) {
        pacfile = config_pacfile.getString();
    }
#endif

    long proxy_port = config_port.getLong();
    system_proxy = (useSystemProxy.getLong() == 1);
    auto_configured = false;
    manual_configured = false;

    if (!proxy_host.empty() && proxy_port != 0)
    {
        manual_configured = true;
        m_proxy_host = proxy_host;
        m_proxy_port = std::to_string(proxy_port);

        if (!proxy_username.empty())
        {
            m_proxy_user = proxy_username;
            m_proxy_pwd = proxy_pwd;
        }
    }

    if (system_proxy && pacfile.length() > 0)
    {
        MWLOG(LEV_DEBUG, MOD_APL,
            "Proxy configured with pacfile in system: %s", pacfile.c_str());
        auto_configured = true;
        m_pac_url = pacfile;
    }
    else if (manual_configured)
    {
        if (system_proxy)
        {
            MWLOG(LEV_DEBUG, MOD_APL,
                "Proxy configured manually in system: host=%s, port=%lu", proxy_host.c_str(), proxy_port);
        }
        else
        {
            MWLOG(LEV_DEBUG, MOD_APL,
                "Proxy configured manually in application: host=%s, port=%lu", proxy_host.c_str(), proxy_port);
        }
    }
    else
    {
        MWLOG(LEV_DEBUG, MOD_APL, "No proxy configured");
    }
}


void ProxyInfo::getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port)
{
    if (!auto_configured){
        MWLOG(LEV_ERROR, MOD_APL,
            "getProxyForHost: Trying to get proxy from pacfile but it is not configured.");
        return;
    }
    std::string proxy_port_str;

#ifdef WIN32
    GetProxyFromPac(m_pac_url.c_str(), urlToFetch.c_str(), proxy_host, &proxy_port_str);
#endif // WIN32

    if (proxy_host->size() > 0 && proxy_port_str.size() > 0)
        *proxy_port = atol(proxy_port_str.c_str());

    MWLOG(LEV_DEBUG, MOD_APL,
        "getProxyForHost: Obtained proxy for endpoint %s: proxy_host=%s proxy_port=%lu", urlToFetch.c_str(),
        proxy_host->c_str(), *proxy_port);
}

std::string ProxyInfo::getProxyHost() {
    if (auto_configured)
    {
        MWLOG(LEV_WARN, MOD_APL,
            "getProxyHost: Getting manually configured proxy host when there is also a pacfile configured. \
            			To get the proxy from pacfile use the getProxyForHost method.");
    }
    if (!manual_configured)
    {
        MWLOG(LEV_ERROR, MOD_APL,
            "getProxyHost: Trying to get proxy host but there is not manual configuration.");
        return "";
    }
    return m_proxy_host;
}

std::string ProxyInfo::getProxyPort() {
    if (auto_configured)
    {
        MWLOG(LEV_WARN, MOD_APL,
            "getProxyPort: Getting manually configured proxy port when there is also a pacfile configured. \
            			To get the proxy from pacfile use the getProxyForHost method.");
    }
    if (!manual_configured)
    {
        MWLOG(LEV_ERROR, MOD_APL,
            "getProxyPort: Trying to get proxy port but there is not manual configuration.");
        return "";
    }
    return m_proxy_port;
}
