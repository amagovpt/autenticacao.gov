#include "scapsignature.h"
#include "eidlib.h"

//tr() function
#include <QObject>

#include "ScapSettings.h"
#include "ASService/soapH.h"
#include "ASService/soapAttributeSupplierBindingProxy.h"
#include "ASService/AttributeSupplierBinding.nsmap"
#include <string>

/*
  SCAPSignature implementation for eidguiV2
*/

ProxyInfo::ProxyInfo()
{
	eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_PROXY_HOST);
	eIDMW::PTEID_Config config2(eIDMW::PTEID_PARAM_PROXY_PORT);
	eIDMW::PTEID_Config config_username(eIDMW::PTEID_PARAM_PROXY_USERNAME);
	eIDMW::PTEID_Config config_pwd(eIDMW::PTEID_PARAM_PROXY_PWD);
	eIDMW::PTEID_Config pacfile(eIDMW::PTEID_PARAM_PROXY_PACFILE);

	std::string proxy_host = config.getString();
	std::string proxy_username = config_username.getString();
	std::string proxy_pwd = config_pwd.getString();
	long proxy_port = config2.getLong();
	const char * pacfile_url = pacfile.getString();

	if (pacfile_url != NULL && strlen(pacfile_url) > 0)
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

/*
QString ScapServices::getConnErrStr() {
    QString error_msg( tr( "Error loading entities" ) );

    std::string strConnErr = connectionErr.getErrStr();
    if (strConnErr.empty()) {
        qDebug() << "ScapSignature::getConnErrStr() - strConnErr empty";
        return error_msg;
    }

    error_msg += "\n\n";
    error_msg += tr( strConnErr.c_str() );

    return error_msg;
}
*/

void ScapServices::setConnErr( int soapConnErr, void *in_suppliers_resp ){
    qDebug() << "ScapSignature::setConnErr() - soapConnErr: " << soapConnErr;
    connectionErr.setErr( soapConnErr, in_suppliers_resp );
}


void ScapServices::getAttributeSuppliers()
{
	const char * as_endpoint = "/DSS/ASService";

	qDebug() << "getAttributeSuppliers(): " << m_suppliersList.size();

	soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
	//TODO: this disables server certificate verification !!
	soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

	// Get Endpoint from settings
	ScapSettings settings;
	std::string port = settings.getScapServerPort().toStdString();
	std::string sup_endpoint = std::string("https://") + settings.getScapServerHost().toStdString() + ":" + port + as_endpoint;

    //Define appropriate network timeouts
    sp->recv_timeout = 20;
    sp->send_timeout = 20;
    sp->connect_timeout = 20;

	std::string proxy_host;
	long proxy_port;
	//Proxy support using the gsoap BindingProxy
	if (m_proxyInfo.isSystemProxy())
	{
		m_proxyInfo.getProxyForHost(sup_endpoint, &proxy_host, &proxy_port);
		if (proxy_host.size() > 0)
		{
			sp->proxy_host = proxy_host.c_str();
			sp->proxy_port = proxy_port;

			eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Using System Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);
		}
	}
	else if (m_proxyInfo.getProxyHost().size() > 0)
	{
		sp->proxy_host = strdup(m_proxyInfo.getProxyHost().toUtf8().constData());
		sp->proxy_port = m_proxyInfo.getProxyPort().toLong();
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "Using Manual Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);

		if (m_proxyInfo.getProxyUser().size() > 0)
		{
			sp->proxy_userid = strdup(m_proxyInfo.getProxyUser().toUtf8().constData());
			sp->proxy_passwd = strdup(m_proxyInfo.getProxyPwd().toUtf8().constData());
		}
	}

    ns2__AttributeSupplierResponseType suppliers_resp;

    const char * c_sup_endpoint = sup_endpoint.c_str();
    const char * entities_soapAction = 
     "http://www.cartaodecidadao.pt/CCC/Attribute/AttributeSupplier/services/AttributeSupplierService/EntitySupplierList";

    AttributeSupplierBindingProxy suppliers_proxy(sp);
    //suppliers_proxy.soap_endpoint = c_sup_endpoint;

    //int ret = suppliers_proxy.AttributeSuppliers(suppliers_resp);
    int ret = suppliers_proxy.AttributeSuppliers(c_sup_endpoint, entities_soapAction, suppliers_resp);

    setConnErr( ret, &suppliers_resp );

    if (ret != SOAP_OK) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
                        , "ScapSignature"
                        , "Error in getAttributeSuppliers: Gsoap returned %d, Attribute suppliers end point: %s, host: %s, port: %s"
                        , ret
                        , c_sup_endpoint
                        , settings.getScapServerHost().toStdString().c_str()
                        , settings.getScapServerPort().toStdString().c_str() );

        if ( ( suppliers_proxy.soap->fault != NULL )
			&& (suppliers_proxy.soap->fault->faultstring != NULL)) {

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
                            , "ScapSignature", "SOAP Fault: %s"
                            , suppliers_proxy.soap->fault->faultstring );
        }
    }

    m_suppliersList = suppliers_resp.AttributeSupplier;
    qDebug() << "List of attribute suppliers Size= " << m_suppliersList.size();
}