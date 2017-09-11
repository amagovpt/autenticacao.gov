#ifndef SCAPSIGNATURE_H

#include <vector>
#include <QString>
#include <string>
#include "ASService/soapH.h"
#include "ErrorConn.h"


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

//Fwd declarations
class ns3__AttributeSupplierType;

class ScapServices {
public:

	void getAttributeSuppliers();

	void getEntityAttributes();
	void getCompanyAttributes();

	void signPDF(const char *content, unsigned long pdf_len);

	/* Connection error functions */
	//The error message should be in the GUI, we should just return an enum
    //QString getConnErrStr();

private:
	std::vector<ns3__AttributeSupplierType *> m_suppliersList;
	ProxyInfo m_proxyInfo;
	ErrorConn connectionErr;
	void setConnErr( int soapConnectionErr, void *in_suppliers_resp );
	
};

#endif