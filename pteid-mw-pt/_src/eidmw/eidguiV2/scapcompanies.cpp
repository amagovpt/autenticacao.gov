/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <vector>
#include <algorithm>
#include <sstream>
#include "eidlib.h"

#include "SCAP-services-v3/SCAPH.h"
#include "SCAP-services-v3/SCAP.nsmap"
#include "SCAP-services-v3/SCAPAttributeClientServiceBindingProxy.h"

#include "scapsignature.h"
#include "ScapSettings.h"
#include <QDir>

#include "Util.h"
#include "Config.h"

#include "gapi.h"

std::vector<ns2__AttributesType *> loadCacheFile(QString &filePath) {
    std::vector<ns2__AttributesType *> attributesType;
    QFile cacheFile(filePath);
    if( !cacheFile.open(QIODevice::ReadOnly) ) {
        qDebug() << "loadCacheFile() Error: " << cacheFile.errorString();
        return attributesType;
    }
    qDebug() << "Reading XML cached file";

    QByteArray fileContent = cacheFile.readAll();

    // Convert string to istream
    std::istringstream replyStream(fileContent.data());
    std::istream * istream = &replyStream;

    soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
    soap_set_namespaces(soap2, SCAP_namespaces);

    soap2->is = istream;

    // Retrieve ns2__AttributeResponseType
    ns2__AttributeResponseType attr_response;
    long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);

    qDebug() << "Retrieved attributes from converting XML to object. Size: "<< attr_response.AttributeResponseValues.size();

    if (ret != 0) {
        qDebug() << "Error reading AttributeResponseType";
        return attributesType;
    }
    attributesType = attr_response.AttributeResponseValues;

    //Remove malformed elements contained in the cache file (returned by service response)
    attributesType.erase(
        std::remove_if(attributesType.begin(), attributesType.end(), [] (ns2__AttributesType *attr) 
            { return attr->ATTRSupplier == NULL || attr->ResponseResult->ResultCode != "200" ;} ),
        attributesType.end());
    
    return attributesType;
}

std::vector<ns2__AttributesType *>
    ScapServices::loadAttributesFromCache(bool isCompanies) {
    std::vector<ns2__AttributesType *> attributesType;
    try
    {
        ScapSettings settings;
        QString scapCachePath(settings.getCacheDir() + "/scap_attributes/");
        QDir scapCacheDir(scapCachePath);
        QStringList flist = scapCacheDir.entryList(QStringList("*.xml"), QDir::Files | QDir::NoSymLinks);
        
        m_attributesList.clear();

        foreach (QString str, flist) {
            if ((!isCompanies ? str.endsWith(ENTITIES_SUFFIX) : str.endsWith(COMPANIES_SUFFIX)))
            {
                QString cachefilePath(scapCachePath+str);
                attributesType = loadCacheFile(cachefilePath);

                if (attributesType.size() > 0)
                {
                    //Merge into single vector
                    m_attributesList.insert(m_attributesList.end(), attributesType.begin(), attributesType.end());
                }
            }
        }
    }
    catch(...) {
        qDebug() << "Error ocurred while loading attributes from cache!";
        //TODO: report error
    }
    return m_attributesList;
}
// Not used: Load attributes from cache only from a unique citizen NIC
/*std::vector<ns2__AttributesType *>
    ScapServices::loadAttributesFromCache(eIDMW::PTEID_EIDCard &card, bool isCompanies) {

    std::vector<ns2__AttributesType *> attributesType;
    try
    {
        QString citizenNIC(card.getID().getCivilianIdNumber());
        citizenNIC.chop(1);//remove check digit
        ScapSettings settings;
        QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";

        QString filePath = scapCacheDir + citizenNIC + (isCompanies ? COMPANIES_SUFFIX : ENTITIES_SUFFIX);

        attributesType = loadCacheFile(filePath);
    }
    catch(...) {
        std::cerr << "Error ocurred while loading attributes from cache!";
        //TODO: report error
    }

    return attributesType;
}*/

std::vector<ns2__AttributesType *>
    ScapServices::reloadAttributesFromCache() {

    std::vector<ns2__AttributesType *> attributesType;
    try
    {
        //QString citizenNIC(card.getID().getCivilianIdNumber());

        ScapSettings settings;
        QString scapCachePath(settings.getCacheDir() + "/scap_attributes/");
        QDir scapCacheDir(scapCachePath);
        QStringList flist = scapCacheDir.entryList(QStringList("*.xml"), QDir::Files | QDir::NoSymLinks);
        
        m_attributesList.clear();

        foreach (QString str, flist) {
            if (str.endsWith(ENTITIES_SUFFIX))
            {
                QString cachefilePath(scapCachePath+str);
                attributesType = loadCacheFile(cachefilePath);

                if (attributesType.size() > 0)
                {
                    //Merge into single vector
                    m_attributesList.insert(m_attributesList.end(), attributesType.begin(), attributesType.end());
                }
            }
        }

        foreach (QString str, flist) {
            if (str.endsWith(COMPANIES_SUFFIX))
            {
                QString cachefilePath(scapCachePath+str);
                attributesType = loadCacheFile(cachefilePath);

                if (attributesType.size() > 0)
                {
                    //Merge into single vector
                    m_attributesList.insert(m_attributesList.end(), attributesType.begin(), attributesType.end());
                }
            }
        }

        /*

        QString filePathEntities = scapCacheDir + citizenNIC + ENTITIES_SUFFIX;

        attributesType = loadCacheFile(filePathEntities);

        m_attributesList.clear();

        if (attributesType.size() > 0)
        {
            //Merge into single vector
            m_attributesList.insert(m_attributesList.end(), attributesType.begin(), attributesType.end());
        }

        QString filePathCompanies = scapCacheDir + citizenNIC + COMPANIES_SUFFIX;

        attributesType = loadCacheFile(filePathCompanies);

        if (attributesType.size() > 0)
        {
            //Merge into single vector
            m_attributesList.insert(m_attributesList.end(), attributesType.begin(), attributesType.end());
        }
        */
    }
    catch(...) {
        qDebug() << "Error ocurred while loading attributes from cache!";
        //TODO: report error
    }
    return m_attributesList;
}

bool ScapServices::removeAttributesFromCache() {

    try
    {
        ScapSettings settings;
        QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";

        qDebug() << "C++: Removing cache files: " << scapCacheDir;

        QDir dir(scapCacheDir);

        dir.setNameFilters(QStringList() << "*.xml");
        dir.setFilter(QDir::Files);

        foreach(QString dirFile, dir.entryList())
        {
            dir.remove(dirFile);
        }

        return true;

    }
    catch(...) {
        qDebug() << "Error ocurred while removing attributes from cache!";
        return false;
    }
}

/* http_post_header and http_send_header functions are adapted/copied from stdsoap2.cpp
 * here to add the authorization token to the HTTP header. 
 * Instead of using the field of the soap struct http_extra_header, we point fposthdr to 
 * http_post_header where the token is added. This is because when using proxy the header
 * is sent in the CONNECT and set to NULL afterwards.
 */
int http_send_header(struct soap *soap, const char *s)
{
    const char *t;
    do
    {
        t = strchr(s, '\n'); /* disallow \n in HTTP headers */
        if (!t)
            t = s + strlen(s);
        if (soap_send_raw(soap, s, t - s))
            return soap->error;
        s = t + 1;
    } while (*t);
    return SOAP_OK;
}
std::string oauthToken;
int http_post_header(struct soap *soap, const char *key, const char *val)
{
    if (key != NULL && strcmp(key, "SOAPAction") == 0)
    {
        http_post_header(soap, "Authorization", oauthToken.c_str());
    }
    if (key)
    {
        if (http_send_header(soap, key))
            return soap->error;
        if (val && (soap_send_raw(soap, ": ", 2) || http_send_header(soap, val)))
            return soap->error;
    }
    return soap_send_raw(soap, "\r\n", 2);
}

std::vector<ns2__AttributesType *> ScapServices::getAttributes(
        GAPI *parent, eIDMW::PTEID_EIDCard *card, std::vector<int> supplier_ids, bool useOAuth) {

    std::vector<ns2__AttributesType *> result;
    ScapSettings settings;
    bool allEnterprises = true;
    const char * soapAction = "http://www.cartaodecidadao.pt/services/ccc/ACS/Operations/Attributes";
    const char * ac_endpoint = "/DSS/ACService";



    std::string appID = settings.getAppID().toStdString();
    /*qDebug() << "SCAP AppID = " << appID.c_str();*/

    std::string appName("Autenticacao.gov");

    qDebug() << "C++: getAttributes called";

    try {

        soap sp;
        soap_init2(&sp, SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
        soap_set_namespaces(&sp, SCAP_namespaces);
        
        long ret;
        if (useOAuth) {
            sp.recv_timeout = 60;
            sp.send_timeout = 60;
            sp.connect_timeout = 60;

            char * ca_path = NULL;
            std::string cacerts_file;
#ifdef __linux__
            ca_path = "/etc/ssl/certs";
#else
            cacerts_file = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR)) + "/cacerts.pem";
#endif
            ret = soap_ssl_client_context(&sp, SOAP_SSL_DEFAULT,
                NULL,
                NULL,
                cacerts_file.size() > 0 ? cacerts_file.c_str() : NULL,
                ca_path,
                NULL);

            if (ret != SOAP_OK) {
                qDebug() << "soap_ssl_client_context() failed - code: " << ret;
                soap_print_fault(&sp, stderr);
                ERR_print_errors_fp(stderr);

                soap_destroy(&sp);
                soap_end(&sp);
                soap_done(&sp);
                return result;
            }
        }

        // Get suppliers List
        std::vector<ns3__AttributeSupplierType *> vec_suppliers;

        if (supplier_ids.size() > 0)
        {
            allEnterprises = false;
        }

        for(int i = 0; i < supplier_ids.size(); i++)
        {
            ns3__AttributeSupplierType * supplier = m_suppliersList.at(supplier_ids.at(i));

            if (supplier == NULL)
            {
                qDebug() << "Couldn't find attributeSupplier with index: " << supplier_ids.at(i);
                continue;
            }

            ns3__AttributeSupplierType * ac_attributeSupplierType = soap_new_req_ns3__AttributeSupplierType(&sp, supplier->Id, supplier->Name);
            vec_suppliers.push_back(ac_attributeSupplierType);
        }
        
        ns2__AttributeSupplierListType * suppliers = soap_new_req_ns2__AttributeSupplierListType(&sp, vec_suppliers);


        // Get Citizen info
        QString fullname;
        QString cleanUuid;
        QString idNumber;
        QString request_uuid = QUuid::createUuid().toString();
        cleanUuid = request_uuid.midRef(1, request_uuid.size() - 2).toString();
        
        OAuthAttributes oauth;
        m_oauth = &oauth;
        if (!useOAuth)
        {
            fullname = card->getID().getGivenName();
            fullname.append(" ");
            fullname.append(card->getID().getSurname());
            idNumber = card->getID().getCivilianIdNumber();
            idNumber.chop(1);//remove check digit
        }
        else
        {
            std::vector<CitizenAttribute> attributes;
            attributes.push_back(CitizenAttribute::NIC);
            attributes.push_back(CitizenAttribute::COMPLETENAME);
            oauth.setAttributes(attributes);
            OAuthResult oauthResult = oauth.fetchAttributes();
            parent->signalEndOAuth((eIDMW::OAuthResult)oauthResult);
            if (oauthResult != OAuthSuccess) {
                soap_destroy(&sp);
                soap_end(&sp);
                soap_done(&sp);
                m_oauth = NULL;
                return result;
            }
            std::map<CitizenAttribute, std::string> *attributesMap = oauth.getAttributes();
            fullname = (*attributesMap)[CitizenAttribute::COMPLETENAME].c_str();
            idNumber = (*attributesMap)[CitizenAttribute::NIC].c_str();
        }

        ns3__PersonalDataType * citizen = soap_new_req_ns3__PersonalDataType(&sp, fullname.toStdString(), idNumber.toStdString());

        //Last param secretKey can be NULL, because we may not have a secretKey stored in configuration (??)
        ns2__AttributeRequestType * attr_request = soap_new_set_ns2__AttributeRequestType(
            &sp, cleanUuid.toStdString(), citizen, suppliers, &allEnterprises, &appID, &appName, nullptr);

        std::stringstream ss;
        if (!useOAuth) {
            sp.os = &ss;
            if (soap_write__ns2__AttributeRequest(&sp, attr_request))
            {
                qDebug() << "Error serializing AttributeRequest!";
            }
        }

        std::string s_soapBody = ss.str();
        s_soapBody.erase(0, s_soapBody.find("\n") + 1);

        // Get host from configuration
        
        std::string serverHost = settings.getScapServerHost().toStdString();
        const char * c_serverHost = serverHost.c_str();

        // SCAP Request
        char * scapAddr = strdup(c_serverHost);

        std::string serverPort = settings.getScapServerPort().toStdString();
        char * scapPort = strdup(serverPort.c_str());
        char * soapBody = strdup(s_soapBody.c_str());
        char * c_endpoint = strdup(ac_endpoint);
        char * c_soapAction = strdup(soapAction);

        std::string scapResult;
		//SCAP Attribute loading with CC - uses PTEID_ScapConnection for TLS with client authentication
        if (!useOAuth)
        {
            //TODO: the PTEID_ScapConnection class does not implement a network timeout because it uses OpenSSL blocking IO mode
            //To implement connection timeout it should be done like this: http://stackoverflow.com/a/16035100/9906
            eIDMW::PTEID_ScapConnection scap(scapAddr, scapPort);
            scapResult.assign(scap.postSoapRequest(c_endpoint, c_soapAction, soapBody));
        }
        else  //SCAP Attribute loading with CMD - direct gSoap call
        {
            // Add the authorization token in http_post_header
            sp.fposthdr = http_post_header;
            oauthToken = oauth.getToken();
            std::string endpoint("https://");
            endpoint.append(scapAddr);
            endpoint.append(c_endpoint);
            const char * soap_action = c_soapAction;

            ProxyInfo m_proxyInfo;
            std::string proxy_host;
            long proxy_port = 0;
            if (m_proxyInfo.isAutoConfig())
            {
                m_proxyInfo.getProxyForHost(endpoint, &proxy_host, &proxy_port);
                if (proxy_host.size() > 0)
                {
                    sp.proxy_host = proxy_host.c_str();
                    sp.proxy_port = proxy_port;
                }
            }
            else if (m_proxyInfo.isManualConfig())
            {
                long proxyinfo_port;
                try {
                    proxyinfo_port = std::stol(m_proxyInfo.getProxyPort());
                }
                catch (...) {
                    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapCompanies", "Error parsing proxy port to number value.");
                }
                sp.proxy_host = strdup(m_proxyInfo.getProxyHost().c_str());
                sp.proxy_port = proxyinfo_port;

                if (m_proxyInfo.getProxyUser().size() > 0)
                {
                    sp.proxy_userid = strdup(m_proxyInfo.getProxyUser().c_str());
                    sp.proxy_passwd = strdup(m_proxyInfo.getProxyPwd().c_str());
                }
            }
            
            AttributeClientServiceBindingProxy proxy(&sp);
            ns2__AttributeResponseType attr_response;
            ret = proxy.Attributes(endpoint.c_str(), soap_action, attr_request, attr_response);
            if (ret != SOAP_OK) {
                qDebug() << "Error returned by calling Attributes in SoapBindingProxy(). Error code: " << ret;
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                          "Error returned by calling Attributes in SoapBindingProxy. Error code: %d",ret);
                parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
                soap_destroy(&sp);
                soap_end(&sp);
                soap_done(&sp);
                m_oauth = NULL;
                return result;
            }

            sp.os = &ss;
            if (soap_write_ns2__AttributeResponseType(&sp, &attr_response))
            {
                qDebug() << "Error serializing AttributeResponse!";
            }
            sp.os = NULL;
            scapResult = ss.str();
        }
        soap_destroy(&sp);
        soap_end(&sp);
        soap_done(&sp);
        
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG , "ScapSignature",
                        "ACService returned: %s",
                        ( !scapResult.empty() ? "SCAP result is not null" : "Null SCAP result" ));

        //PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG , "ScapSignature",
          //      "ACService returned: %s",( !scapResult.empty() ? scapResult.c_str() : "Null SCAP result" ));

		
        std::string replyString = scapResult;
		//We don't need Windows line-endings in this string and it will be only used for debug log
		scapResult.erase(std::remove(scapResult.begin(), scapResult.end(), '\r'), scapResult.end());
		// Skip response HTTP headers by searching for the expected root XML tag
        if (!useOAuth)
        {
            size_t initialPos = replyString.find("<AttributeResponse");
            if (initialPos == std::string::npos)
            {
                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                    "Error reading AttributeResponse! Malformed XML response.");

                PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG , "ScapSignature",
                        "Malformed XML response: %s",( !scapResult.empty() ? scapResult.c_str() : "Null SCAP result" ));

                parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
                m_oauth = NULL;
                return result;
            }
            std::string endString = "</AttributeResponse>";

            size_t finalPos = replyString.find(endString) + endString.length();
            replyString = replyString.substr(initialPos, finalPos - initialPos);
        }
        else
        {
            replyString = replyString.substr(replyString.find("<ns2:AttributeResponseType"));
        }

        // Save to cache
        QString s_scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
        QDir scapCacheDir;
		// Tries to create if does not exist
		if (!scapCacheDir.mkpath(s_scapCacheDir)) {
			qDebug() << "couldn't create SCAP cache folder";
			parent->signalCacheFolderNotCreated();
		}
        QString fileLocation = s_scapCacheDir + idNumber + (allEnterprises ? COMPANIES_SUFFIX : ENTITIES_SUFFIX);

        // Convert string to istream
        std::istringstream replyStream(replyString);
        std::istream * istream = &replyStream;

        // Create soap request with generated body content.
        soap soap2;
        soap_init2(&soap2, SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

        soap_set_namespaces(&soap2, SCAP_namespaces);

        soap2.is = istream;

        // Retrieve ns2__AttributeResponseType
        ns2__AttributeResponseType attr_response;
        ret = soap_read_ns2__AttributeResponseType(&soap2, &attr_response);

        if (ret != 0) {
            qDebug() << "Error reading AttributeResponseType! Malformed XML response";
            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                      "Error reading AttributeResponseType! Malformed XML response. Error Code: %d",ret);

            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG , "ScapSignature",
                    "Malformed XML AttributeResponse: %s",( !scapResult.empty() ? scapResult.c_str() : "Null SCAP result" ));

            parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
            //FIXME: mem leak
            //soap_destroy(&soap2);
            //soap_end(&soap2);
            //soap_done(&soap2);
            m_oauth = NULL;
            return result;
        }

        unsigned int resp_size = attr_response.AttributeResponseValues.size();

        qDebug() << "Got response from converting XML to object. Size: " << resp_size;

        if (resp_size > 0) {

            if (attr_response.SecretKey != NULL) {
                // We received a SCAP secretKey so let's save it!
                settings.setSecretKey(*attr_response.SecretKey, idNumber);
            }

            int getAttr = SCAP_ATTRIBUTES_OK;
            bool getAttrHaveChilds = false;

            for(int k = 0; k < (int)resp_size; k++){
                ns2__AttributesType * parentAttribute = attr_response.AttributeResponseValues.at((unsigned long)k);
                std::string resultCode = parentAttribute->ResponseResult->ResultCode;

                int resultCodeValue = atoi(resultCode.c_str());

                if (resultCodeValue != SCAP_ATTRIBUTES_OK)
                {
                    eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                     "Error in getAttributes(): Result Code: %d", resultCodeValue);
                    qDebug() << "Error in getAttributes(): Result Code: " << resultCodeValue ;
                    getAttr = resultCodeValue;
                } else {
                    std::vector<ns5__SignatureType *> childs = parentAttribute->SignedAttributes->ns3__SignatureAttribute;
                    if (parentAttribute->SignedAttributes == NULL || childs.size() == 0) {
                        qDebug() << "Zero child attributes in AttributeResponseValues!";
                        PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                  "Zero child attributes in AttributeResponseValues!");
                    } else {
                        qDebug() << "getAttributes(): Attributes response!";
                        getAttrHaveChilds = true;
                    }
                }
            }

            if(getAttr != SCAP_ATTRIBUTES_OK){
                if(resp_size > 1 && getAttrHaveChilds == true){
                    // Keep going and cache the file. At least loaded one attribute.
                    parent->signalSCAPDefinitionsServiceFail(GAPI::ScapMultiEntityError, allEnterprises);
                }else{
                    if(getAttr == SCAP_ATTRIBUTES_EXPIRED){
                        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapAttributesExpiredError, allEnterprises);
                    }else if(getAttr == SCAP_ZERO_ATTRIBUTES){
                        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapZeroAttributesError, allEnterprises);
                    }else if(getAttr == SCAP_ATTRIBUTES_NOT_VALID){
                        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapNotValidAttributesError, allEnterprises);
                    }else{
                        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
                    }
                    //FIXME: mem leak
                    //soap_destroy(&soap2);
                    //soap_end(&soap2);
                    //soap_done(&soap2);
                    m_oauth = NULL;
                    return result;
                }
            }

            if(getAttrHaveChilds == false){
                if(allEnterprises) {
                    parent->signalCompanyAttributesLoadedError();
                }else{
                    parent->signalEntityAttributesLoadedError();
                }
                //FIXME: mem leak
                //soap_destroy(&soap2);
                //soap_end(&soap2);
                //soap_done(&soap2);
                m_oauth = NULL;
                return result;
            }

            qDebug() << "Creating cache file on location: " << fileLocation;
            QFile cacheFile(fileLocation);
            
            if (cacheFile.open(QIODevice::WriteOnly)) {
                cacheFile.write(replyString.c_str(), replyString.length());
            }
            else
            {
                qDebug() << "Couldn't save attribute result to cache. Error: " << cacheFile.errorString();
				parent->signalCacheNotWritable();
            }
        } else {

            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG , "ScapSignature",
                        "Attributes response error: %s",( !scapResult.empty() ? scapResult.c_str() : "Null SCAP result" ));

            if(allEnterprises) {
                parent->signalCompanyAttributesLoadedError();
            }else{
                parent->signalEntityAttributesLoadedError();
            }
        }
        
        result = attr_response.AttributeResponseValues;

        //FIXME: mem leak
        //soap_destroy(&soap2);
        //soap_end(&soap2);
        //soap_done(&soap2);
    }
    catch(const std::exception &ex) {
        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                         "Error in getAttributes(): %s", ex.what());

        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
        qDebug() << "Error in getAttributes():" << ex.what();
    }
    m_oauth = NULL;
    return result;
}

void ScapServices::cancelGetAttributesWithCMD() {
    if (m_oauth){
        m_oauth->closeListener();
    }
}
