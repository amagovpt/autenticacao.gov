/*-****************************************************************************

 * Copyright (C) 2017-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "scapsignature.h"
#include "eidlib.h"
#include "eidlibException.h"

#include "ScapSettings.h"
#include "gapi.h"
#include "Util.h"
#include "Config.h"

#include "SCAP-services-v3/SCAPH.h"
#include "SCAP-services-v3/SCAPAttributeSupplierBindingProxy.h"
//#include "SCAPServices/SCAP.nsmap"

// Client for WS PADES/PDFSignature
#include "pdfsignatureclient.h"
#include <string>
#include <codecvt>

/*
  SCAPSignature implementation for eidguiV2
*/

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
    int index = 0;

    for (int i=0; i!=attributes_index.size(); i++) {
        index = 0;

        for (int j=0; j < m_attributesList.size(); j++) {

            try {
                parent = m_attributesList.at(j);
            }
            catch (std::out_of_range &e)
            {
                qDebug() << "Invalid attribute index: "
                         << attributes_index[j] << "This shouldn't happen!";
                continue;
            }

            std::vector<ns5__SignatureType *> childs = parent->SignedAttributes->ns3__SignatureAttribute;
            for(uint childPos = 0;  childPos < childs.size(); childPos++)
            {
                ns5__SignatureType * child = childs.at(childPos);
                if ( child->ns5__Object.size() > 0 && index == attributes_index[i] )
                {
                    ns5__ObjectType * objType = child->ns5__Object.at(0);
                    ns3__AttributeType * attr = objType->union_ObjectType.ns3__Attribute;
                    //"Fix" the AttributeSupplier (ID and Name): they can have different values in SCAP and the actual entity Attribute
                    attr->AttributeSupplier->Id = parent->ATTRSupplier->Id;
                    attr->AttributeSupplier->Name = parent->ATTRSupplier->Name;
                    parsedAttributes.push_back(attr);
                    /*qDebug() << "Selected attribute from supplier: " << attr->AttributeSupplier->Name.c_str();*/
                }
                index++;
            }
        }
    }

    return parsedAttributes;
}

/*
*  SCAP signature with citizen signature using CMD
*/
void ScapServices::executeSCAPWithCMDSignature(GAPI *parent, QString &savefilepath, int selected_page,
        double location_x, double location_y, QString &location, QString &reason, int ltv_years,
        std::vector<int> attributes_index, CmdSignedFileDetails cmd_details,
        bool useCustomImage, QByteArray &m_jpeg_scaled_data) {

    std::vector<ns3__AttributeType*> selected_attributes = getSelectedAttributes(attributes_index);

    if (selected_attributes.size() == 0)
    {
        qDebug() << "Couldn't find any index in m_attributesList!";
        return;
    }

    PDFSignatureClient scap_signature_client;
    ProxyInfo m_proxyInfo;
    int successful;
    try{
         successful = scap_signature_client.signPDF(m_proxyInfo, savefilepath, cmd_details.signedCMDFile, cmd_details.citizenName,
            cmd_details.citizenId, ltv_years, false, PDFSignatureInfo(selected_page, location_x, location_y,
            false, strdup(location.toUtf8().constData()), strdup(reason.toUtf8().constData())), selected_attributes,
            useCustomImage, m_jpeg_scaled_data);
    }
    catch (eIDMW::PTEID_Exception &e)
    {
        std::cerr << "Caught exception signing PDF. Error code: " << e.GetError() << std::endl;
        throw;
    }
    if (successful == GAPI::ScapSucess) {
        parent->signCMDFinished(ERR_NONE);
        emit parent->signalOpenFile();
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "ScapSignature",
                  "SCAP CMD ScapSucess");
    }
    else if (successful == GAPI::ScapTimeOutError) {
        qDebug() << "Error in SCAP service Timeout with CMD service!";
        parent->signalSCAPServiceTimeout();
        parent->signCMDFinished(SCAP_GENERIC_ERROR_CODE);
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "SCAP CMD Error. ScapTimeOutError");
    }
    else if (successful == GAPI::ScapClockError) {
        qDebug() << "Error in SCAP service Clock Error with CMD service!";
        parent->signCMDFinished(SCAP_CLOCK_ERROR_CODE);
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "SCAP CMD Error. ScapClockError");
    }
    else if (successful == GAPI::ScapSecretKeyError) {
        qDebug() << "Error in SCAP service SecretKey Error with CMD service!";
        parent->signalShowLoadAttrButton();
        parent->signCMDFinished(SCAP_SECRETKEY_ERROR_CODE);
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "SCAP CMD Error. ScapSecretKeyError");
    }
    else {
        qDebug() << "Error in SCAP Signature with CMD service!";
        parent->signalSCAPServiceFail(successful);
        parent->signCMDFinished(SCAP_GENERIC_ERROR_CODE);
        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                  "SCAP CMD Error. ScapPdfSignResult = %d",successful);
    }

    parent->signalUpdateProgressBar(100);

}

void ScapServices::executeSCAPSignature(GAPI *parent, QString &inputPath, QString &savefilepath, int selected_page,
    double location_x, double location_y, QString &location, QString &reason, int ltv_years,
    std::vector<int> attributes_index, bool useCustomImage, QByteArray &m_jpeg_scaled_data )
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
    qDebug() << "Generating first tempFile: " << temp_save_path;
    int sign_rc = 0;

    try {
        PTEID_ReaderContext& readerContext = ReaderSet.getReader();
        if (!readerContext.isCardPresent())
        {
            std::cerr << "PDF Signature error: No card found in the reader!" << std::endl;
            //this->success = SIG_ERROR;
        }


        eIDMW::PTEID_EIDCard &card = readerContext.getEIDCard();

        // Get Citizen info
        QString citizenName = card.getID().getGivenName();
        citizenName.append(" ");
        citizenName.append(card.getID().getSurname());

        citizenId = card.getID().getCivilianIdNumber();

        std::cout << "Sent PDF Signature coordinates. X:" << location_x << " Y:" << location_y << std::endl;

        PTEID_PDFSignature pdf_sig(strdup(inputPath.toUtf8().constData()));

        // Sign pdf
        sign_rc = card.SignPDF(pdf_sig, selected_page, 0, false, strdup(location.toUtf8().constData()),
                                strdup(reason.toUtf8().constData()), temp_save_path);
        if (sign_rc == 0)
        {
            PDFSignatureClient scap_signature_client;
            ProxyInfo m_proxyInfo;
            int successful = scap_signature_client.signPDF(
                        m_proxyInfo, savefilepath, QString(temp_save_path), QString(citizenName),
                        QString(citizenId), ltv_years, true, PDFSignatureInfo(selected_page, location_x, location_y,
                        false, strdup(location.toUtf8().constData()), strdup(reason.toUtf8().constData())), selected_attributes,
                        useCustomImage, m_jpeg_scaled_data);
            if (successful == GAPI::ScapSucess) {
                parent->signalPdfSignSucess(parent->SignMessageOK);
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_CRITICAL, "ScapSignature",
                          "SCAP CC ScapSuccess");
            }
            else if (successful == GAPI::ScapTimeOutError) {
                qDebug() << "Error in SCAP service Timeout!";
                parent->signalSCAPServiceTimeout();
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                          "SCAP CC ScapTimeOutError");
            }
            else if (successful == GAPI::ScapClockError) {
                qDebug() << "Error in SCAP service Clock Error!";
                parent->signalSCAPServiceFail(successful);
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                          "SCAP CC ScapClockError");
            }
            else if (successful == GAPI::ScapSecretKeyError) {
                qDebug() << "Error in SCAP service SecretKey Error!";
                parent->signalSCAPServiceFail(successful);
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                          "SCAP CC ScapSecretKeyError");
            }
            else {
                qDebug() << "Error in SCAP Signature service!";
                parent->signalSCAPServiceFail(successful);
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                          "SCAP CC Error. ScapPdfSignResult = %d",successful);
            }
        }
        else {
            parent->signalSCAPServiceFail(GAPI::ScapGenericError);
            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                      "SCAP CC Card Generic Error");
        }
                
    }
    catch (eIDMW::PTEID_Exception &e)
    {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
              "executeSCAPSignature - Exception accessing EIDCard. Error code: %08x",  e.GetError());
        free(temp_save_path);
        throw;
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

	char * ca_path = NULL;
	std::string cacerts_file;

    //Define appropriate network timeouts
    sp->recv_timeout = RECV_TIMEOUT;
    sp->send_timeout = SEND_TIMEOUT;
    sp->connect_timeout = CONNECT_TIMEOUT;

#ifdef __linux__
	ca_path = "/etc/ssl/certs";
	//Load CA certificates from file provided with pteid-mw
#elif WIN32
	cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR)) + "/cacerts.pem";
	//TODO
#elif __APPLE__
	cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";
#endif

	int ret = soap_ssl_client_context(sp, SOAP_SSL_DEFAULT,
		NULL,
		NULL,
		cacerts_file.size() > 0 ? cacerts_file.c_str() : NULL, /* cacert file to store trusted certificates (needed to verify server) */
		ca_path,
		NULL);

	if (ret != SOAP_OK) {
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
			, "ScapSignature"
			, "Error in getAttributeSuppliers: Gsoap returned %d "
			, ret);
	}

	// Get Endpoint from settings
	ScapSettings settings;
	std::string port = settings.getScapServerPort().toStdString();
	std::string sup_endpoint = std::string("https://") + settings.getScapServerHost().toStdString() + ":" + port + as_endpoint;


	std::string proxy_host;
    long proxy_port = 0;
	//Proxy support using the gsoap BindingProxy
    ProxyInfo m_proxyInfo;
	
    if (m_proxyInfo.isAutoConfig())
	{
		m_proxyInfo.getProxyForHost(sup_endpoint, &proxy_host, &proxy_port);
		if (proxy_host.size() > 0)
		{
			sp->proxy_host = proxy_host.c_str();
			sp->proxy_port = proxy_port;
		}
	}
	else if (m_proxyInfo.isManualConfig())
	{
        long proxyinfo_port;
        try {
            proxyinfo_port = std::stol(m_proxyInfo.getProxyPort());
        }
        catch (...) {
            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "Error parsing proxy port to number value.");
        }
        sp->proxy_host = strdup(m_proxyInfo.getProxyHost().c_str());
        sp->proxy_port = proxyinfo_port;
        
		if (m_proxyInfo.getProxyUser().size() > 0)
		{
            sp->proxy_userid = strdup(m_proxyInfo.getProxyUser().c_str());
            sp->proxy_passwd = strdup(m_proxyInfo.getProxyPwd().c_str());
		}
	}

    ns9__AttributeSupplierResponseType suppliers_resp;

    const char * c_sup_endpoint = sup_endpoint.c_str();
    const char * entities_soapAction = 
     "http://www.cartaodecidadao.pt/CCC/Attribute/AttributeSupplier/services/AttributeSupplierService/EntitySupplierList";

    AttributeSupplierBindingProxy suppliers_proxy(sp);
    //suppliers_proxy.soap_endpoint = c_sup_endpoint;

    //int ret = suppliers_proxy.AttributeSuppliers(suppliers_resp);
    ret = suppliers_proxy.AttributeSuppliers(c_sup_endpoint, entities_soapAction, suppliers_resp);

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
