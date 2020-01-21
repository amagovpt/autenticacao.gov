/*-****************************************************************************

* Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
* Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
* Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
*
* Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "proxyinfo.h"
#include "eidlib.h"
#include "eidlibdefines.h"
#include <codecvt>
#include <string.h>
#include "Util.h"

ProxyInfo::ProxyInfo()
{
    eIDMW::PTEID_Config config_host(eIDMW::PTEID_PARAM_PROXY_HOST);
    eIDMW::PTEID_Config config_port(eIDMW::PTEID_PARAM_PROXY_PORT);
    eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
    eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
    eIDMW::PTEID_Config config_pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);
    eIDMW::PTEID_Config useSystemProxy(eIDMW::PTEID_PARAM_PROXY_USE_SYSTEM);

#ifdef WIN32
    // in Windows the proxy configs come in utf16. Convert them to utf8:
    std::wstring proxy_hostW = eIDMW::utilStringWiden(config_host.getString());
    std::wstring proxy_usernameW = eIDMW::utilStringWiden(config_username.getString());
    std::wstring proxy_pwdW = eIDMW::utilStringWiden(config_pwd.getString());

    std::u16string proxy_host_u16(proxy_hostW.begin(), proxy_hostW.end());
    std::u16string proxy_username_u16(proxy_usernameW.begin(), proxy_usernameW.end());
    std::u16string proxy_pwd_16(proxy_pwdW.begin(), proxy_pwdW.end());

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string proxy_host = convert.to_bytes(proxy_host_u16);
    std::string proxy_username = convert.to_bytes(proxy_username_u16);
    std::string proxy_pwd = convert.to_bytes(proxy_pwd_16);

    std::string pacfile;
    if (config_pacfile.getString()) {
        std::wstring pacfileW = eIDMW::utilStringWiden(config_pacfile.getString());
        std::u16string pacfile_16(pacfileW.begin(), pacfileW.end());
        pacfile = convert.to_bytes(pacfile_16);
    }

#else
    std::string proxy_host = config.getString();
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
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
            "Proxy configured with pacfile in system: %s", pacfile.c_str());
        auto_configured = true;
        m_pac_url = pacfile;
    }
    else if (manual_configured)
    {
        if (system_proxy)
        {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
                "Proxy configured manually in system: host=%s, port=%lu", proxy_host.c_str(), proxy_port);
        }
        else
        {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
                "Proxy configured manually in application: host=%s, port=%lu", proxy_host.c_str(), proxy_port);
        }
    }
    else
    {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo", "No proxy configured");
    }
}


void ProxyInfo::getProxyForHost(std::string urlToFetch, std::string * proxy_host, long *proxy_port)
{
    if (!auto_configured){
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ProxyInfo",
            "getProxyForHost: Trying to get proxy from pacfile but it is not configured.");
        return;
    }
    std::string proxy_port_str;

    eIDMW::PTEID_GetProxyFromPac(m_pac_url.c_str(), urlToFetch.c_str(), proxy_host, &proxy_port_str);

    if (proxy_host->size() > 0 && proxy_port_str.size() > 0)
        *proxy_port = atol(proxy_port_str.c_str());

    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ProxyInfo",
        "getProxyForHost: Obtained proxy for endpoint %s: proxy_host=%s proxy_port=%lu", urlToFetch.c_str(),
        proxy_host->c_str(), *proxy_port);
}

std::string ProxyInfo::getProxyHost() {
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

std::string ProxyInfo::getProxyPort() {
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