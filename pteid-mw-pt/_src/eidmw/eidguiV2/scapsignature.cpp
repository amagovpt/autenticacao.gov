#include "scapsignature.h"
#include "eidlib.h"
#include "eidlibException.h"

#include "ScapSettings.h"

//#include "ASService/soapH.h"
//#include "ASService/soapAttributeSupplierBindingProxy.h"
//#include "ASService/AttributeSupplierBinding.nsmap"
#include "SCAPServices/SCAPH.h"
#include "SCAPServices/SCAPAttributeSupplierBindingProxy.h"
//#include "SCAPServices/SCAP.nsmap"

// Client for WS PADES/PDFSignature
#include "pdfsignatureclient.h"
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

void ScapServices::setConnErr( int soapConnErr, void *in_suppliers_resp ) {
    qDebug() << "ScapSignature::setConnErr() - soapConnErr: " << soapConnErr;
    connectionErr.setErr( soapConnErr, in_suppliers_resp );
}

using namespace eIDMW;


std::vector<ns3__AttributeType*> ScapServices::getSelectedAttributes(std::vector<int> attributes_index) {

    std::vector<ns3__AttributeType*> parsedAttributes;
    ns2__AttributesType * parent = NULL;

    for (int i=0; i!=attributes_index.size(); i++) {
        try {
            parent = m_attributesList.at(attributes_index[i]);    
        }
        catch (std::out_of_range &e) 
        {
            qDebug() << "Invalid attribute index: "
                     << attributes_index[i] <<"This shouldn't happen!";
            continue;
        }

        std::vector<ns5__SignatureType *> childs = parent->SignedAttributes->ns3__SignatureAttribute;
        for(uint childPos = 0;  childPos < childs.size(); childPos++)
        {
            ns5__SignatureType * child = childs.at(childPos);
            if ( child->ns5__Object.size() > 0 )
            {
                ns5__ObjectType * objType = child->ns5__Object.at(0);
                ns3__AttributeType * attr = objType->union_ObjectType.ns3__Attribute;
                //"Fix" the AttributeSupplier (ID and Name): they can have different values in SCAP and the actual entity Attribute
                attr->AttributeSupplier->Id = parent->ATTRSupplier->Id;
                attr->AttributeSupplier->Name = parent->ATTRSupplier->Name;
                parsedAttributes.push_back(attr);
               
           }
       }

    }

    return parsedAttributes;
}

void ScapServices::executeSCAPSignature(QString &inputPath, QString &savefilepath, int selected_page,
         double location_x, double location_y, int ltv_years, std::vector<int> attributes_index)
{
    // Sets user selected file save path
    const char* citizenId = NULL;

    std::vector<ns3__AttributeType*> selected_attributes = getSelectedAttributes(attributes_index);

    if (selected_attributes.size() == 0)
    {
        qDebug() << "Couldn't find any index in m_attributesList!";
        return;
    }

    // Creates a temporary file
    QTemporaryFile tempFile;

    if (!tempFile.open()) {
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "PDF Signature error: Error creating temporary file");
        return;
    }

    char *temp_save_path = strdup(tempFile.fileName().toStdString().c_str());
    int sign_rc = 0;

    try {
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        if (!readerContext.isCardPresent())
        {
            std::cerr << "PDF Signature error: No card found in the reader!" << std::endl;
            //this->success = SIG_ERROR;
        }

        try
        {
            eIDMW::PTEID_EIDCard &card = readerContext.getEIDCard();

            // Get Citizen info
            QString citizenName = card.getID().getGivenName();
            citizenName.append(" ");
            citizenName.append(card.getID().getSurname());

            citizenId = card.getID().getCivilianIdNumber();

            std::cout << "Sent PDF Signature coordinates. X:" << location_x << " Y:" << location_y << std::endl;

            PTEID_PDFSignature pdf_sig(strdup(inputPath.toUtf8().constData()));

            // Sign pdf
            sign_rc = card.SignPDF(pdf_sig, selected_page, 0, false, "" , "", temp_save_path);

            if (sign_rc == 0)
            {
                //this->success = SIG_ERROR;
                bool successful = PDFSignatureClient::signPDF(m_proxyInfo, savefilepath, QString(temp_save_path), QString(citizenName),
                    QString(citizenId), ltv_years, PDFSignatureInfo(selected_page, location_x, location_y, false), selected_attributes);

                if (successful) {
                	
                }
                else {
                    qDebug() << "Error in PADES/PDFSignature service!";
                }
            }
            else {

            	//TODO: Handle error in the local signature
            }
                
        }
        catch (eIDMW::PTEID_Exception &e)
        {
            std::cerr << "Caught exception getting EID Card. Error code: " << hex << e.GetError() << std::endl;
            //this->success = SIG_ERROR;
        }

    }
    catch(eIDMW::PTEID_Exception &e) {
        std::cerr << "Caught exception getting reader. Error code: " << hex << e.GetError() << std::endl;
        //this->success = SIG_ERROR;
    }

    free(temp_save_path);
}


std::vector<ns3__AttributeSupplierType *> ScapServices::getAttributeSuppliers()
{
	const char * as_endpoint = "/DSS/ASService";

    //TODO: should we cache the suppliers list ??

	//qDebug() << "getAttributeSuppliers(): " << m_suppliersList.size();

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
    long proxy_port = 0;
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

    ns9__AttributeSupplierResponseType suppliers_resp;

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
    qDebug() << "Reloaded list of attribute suppliers Size= " << m_suppliersList.size();

    return m_suppliersList;
}
