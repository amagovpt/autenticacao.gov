#ifndef SCAPSIGNATURE_H
#define SCAPSIGNATURE_H

#include <vector>
#include <QString>
#include <string>
#include <vector>
//#include "ASService/soapH.h"
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
namespace eIDMW {
	class PTEID_EIDCard;
}

struct CmdSignedFileDetails;


class ns2__AttributesType;
class ns3__AttributeType;

class ns3__AttributeSupplierType;

#define COMPANIES_SUFFIX "_ENTERPRISES.xml"
#define ENTITIES_SUFFIX "_INSTITUTIONS.xml"

class GAPI;

class ScapServices {
public:

	std::vector<ns3__AttributeSupplierType *> getAttributeSuppliers();

	void getEntityAttributes();
    std::vector<ns2__AttributesType *> getAttributes(eIDMW::PTEID_EIDCard &card, std::vector<int> supplier_ids);

    void executeSCAPWithCMDSignature(GAPI *parent, QString &savefilepath, int selected_page,
   				double location_x, double location_y, int ltv_years, std::vector<int> attributes_index, CmdSignedFileDetails cmd_details);

	void executeSCAPSignature(GAPI *parent, QString &inputPath, QString &savefilepath, int selected_page,
	     double location_x, double location_y, int ltv_years, std::vector<int> selected_attributes);

    std::vector<ns2__AttributesType *> loadAttributesFromCache(eIDMW::PTEID_EIDCard &card, bool isCompanies);
    std::vector<ns2__AttributesType *> reloadAttributesFromCache();
    bool removeAttributesFromCache(eIDMW::PTEID_EIDCard &card, bool isCompanies);
	/* Connection error functions */
	//The error message should be in the GUI, we should just return an enum
    //QString getConnErrStr();

private:
	std::vector<ns3__AttributeSupplierType *> m_suppliersList;
	std::vector<ns2__AttributesType *> m_attributesList;
	ProxyInfo m_proxyInfo;
	ErrorConn connectionErr;
	std::vector<ns3__AttributeType*> getSelectedAttributes(std::vector<int> attributes_index);
	void setConnErr( int soapConnectionErr, void *in_suppliers_resp );
	
};

#endif //SCAPSIGNATURE_H
