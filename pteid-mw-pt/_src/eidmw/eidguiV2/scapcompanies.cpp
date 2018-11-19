#include <vector>
#include <sstream>
#include "eidlib.h"

#include "SCAP-services-v3/SCAPH.h"
#include "SCAP-services-v3/SCAP.nsmap"
#include "scapsignature.h"
#include "ScapSettings.h"
#include <QDir>

#include "gapi.h"

std::vector<ns2__AttributesType *> loadCacheFile(QString &filePath) {
    std::vector<ns2__AttributesType *> attributesType;
    QFile cacheFile(filePath);
    if( !cacheFile.open(QIODevice::ReadOnly) ) {
        std::cerr << "loadCacheFile() Error: " << cacheFile.errorString().toStdString() << std::endl;
        return attributesType;
    }
    qDebug() << "Reading XML cached file";

    QByteArray fileContent = cacheFile.readAll();

    // Convert string to istream
    std::istringstream replyStream(fileContent.data());
    std::istream * istream = &replyStream;

    soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
    soap_set_namespaces(soap2, namespaces);

    soap2->is = istream;

    // Retrieve ns2__AttributeResponseType
    ns2__AttributeResponseType attr_response;
    long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);

    qDebug() << "Retrieved attributes from converting XML to object. Size: "<< attr_response.AttributeResponseValues.size();

    if (ret != 0) {
        std::cerr << "Error reading AttributeResponseType" << std::endl;
        return attributesType;
    }
    attributesType = attr_response.AttributeResponseValues;

    //Remove malformed elements contained in the cache file (returned by service response)
    attributesType.erase(
        std::remove_if(attributesType.begin(), attributesType.end(), [] (ns2__AttributesType *attr) { return attr->ATTRSupplier == NULL; } ),
        attributesType.end());
    
    return attributesType;
}

std::vector<ns2__AttributesType *>
    ScapServices::loadAttributesFromCache(eIDMW::PTEID_EIDCard &card, bool isCompanies) {

    std::vector<ns2__AttributesType *> attributesType;
    try
    {
        QString citizenNIC(card.getID().getCivilianIdNumber());

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
}

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
        std::cerr << "Error ocurred while loading attributes from cache!";
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
        std::cerr << "Error ocurred while removing attributes from cache!";
        return false;
    }
}

std::vector<ns2__AttributesType *> ScapServices::getAttributes(GAPI *parent, eIDMW::PTEID_EIDCard &card, std::vector<int> supplier_ids) {

    std::vector<ns2__AttributesType *> result;
    ScapSettings settings;
    bool allEnterprises = true;
    const char * soapAction = "http://www.cartaodecidadao.pt/services/ccc/ACS/Operations/Attributes";
    const char * ac_endpoint = "/DSS/ACService";



    std::string appID = settings.getAppID().toStdString();
    qDebug() << "SCAP AppID = " << appID.c_str();

    std::string appName("Autenticacao.gov");

    qDebug() << "C++: getAttributes called";

    try {

        soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

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

            ns3__AttributeSupplierType * ac_attributeSupplierType = soap_new_req_ns3__AttributeSupplierType(sp, supplier->Id, supplier->Name);
            vec_suppliers.push_back(ac_attributeSupplierType);
        }
        
        ns2__AttributeSupplierListType * suppliers = soap_new_req_ns2__AttributeSupplierListType(sp, vec_suppliers);


        // Get Citizen info
        QString fullname = card.getID().getGivenName();
        fullname.append(" ");
        fullname.append(card.getID().getSurname());
        QString request_uuid = QUuid::createUuid().toString();
        QString cleanUuid = request_uuid.midRef(1, request_uuid.size() - 2).toString();

        const char* idNumber = card.getID().getCivilianIdNumber();

        std::string secretKey = settings.getSecretKey(idNumber);

        ns3__PersonalDataType * citizen = soap_new_req_ns3__PersonalDataType(sp, fullname.toStdString(), idNumber);

        //Last param secretKey can be NULL, because we may not have a secretKey stored in configuration (??)
        ns2__AttributeRequestType * attr_request = soap_new_set_ns2__AttributeRequestType(
            sp, cleanUuid.toStdString(), citizen, suppliers, &allEnterprises, &appID, &appName, secretKey.size() > 0 ? &secretKey : NULL);

        std::stringstream ss;
        sp->os = &ss;
        if (soap_write_ns2__AttributeRequestType(sp, attr_request))
        {
            qDebug() << "Error serializing AttributeRequest!";
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

        //TODO: the PTEID_ScapConnection class does not implement a network timeout because it uses OpenSSL blocking IO mode
        //To implement connection timeout it should be done like this: http://stackoverflow.com/a/16035100/9906
        eIDMW::PTEID_ScapConnection scap(scapAddr, scapPort);
        char * scapResult = scap.postSoapRequest(c_endpoint, c_soapAction, soapBody);

        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR , "ScapSignature",
                        "ACService returned: %s",
                        ( scapResult ? scapResult : "Null SCAP result" ));

        // Remove request answer headers
        std::string replyString = scapResult;

        int initialPos = replyString.find("<AttributeResponse");
        std::string endString = "</AttributeResponse>";

        int finalPos = replyString.find(endString) + endString.length();
        replyString = replyString.substr(initialPos, finalPos - initialPos);

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
        soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

        soap_set_namespaces(soap2, namespaces);

        soap2->is = istream;

        // Retrieve ns2__AttributeResponseType
        ns2__AttributeResponseType attr_response;
        long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);
        
        if (ret != 0) {
            qDebug() << "Error reading AttributeResponseType! Malformed XML response";
            std::cerr << "Error reading AttributeResponseType! Malformed XML response" << std::endl;
            parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
            return result;
        }

        unsigned int resp_size = attr_response.AttributeResponseValues.size();

        //std::cerr << "Got response from converting XML to object. Size: " << resp_size  << std::endl;
        
        if (resp_size > 0) {

            if (attr_response.SecretKey != NULL) {
                qDebug() << "We received a SCAP secretKey so let's save it!";
                settings.setSecretKey(*attr_response.SecretKey, idNumber);
            }

            ns2__AttributesType * parentAttribute = attr_response.AttributeResponseValues.at(0);
            std::string resultCode = parentAttribute->ResponseResult->ResultCode;

            int resultCodeValue = atoi(resultCode.c_str());

            if (resultCodeValue != SCAP_ATTRIBUTES_OK)
            {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature",
                                 "Error in getAttributes(): Result Code: %d", resultCodeValue);
                qDebug() << "Error in getAttributes(): Result Code: " << resultCodeValue ;

                if(resultCodeValue == SCAP_ATTRIBUTES_EXPIRED){
                    parent->signalSCAPDefinitionsServiceFail(GAPI::ScapAttributesExpiredError, allEnterprises);
                }else if(resultCodeValue == SCAP_ZERO_ATTRIBUTES){
                    parent->signalSCAPDefinitionsServiceFail(GAPI::ScapZeroAttributesError, allEnterprises);
                }else if(resultCodeValue == SCAP_ATTRIBUTES_NOT_VALID){
                    parent->signalSCAPDefinitionsServiceFail(GAPI::ScapNotValidAttributesError, allEnterprises);
                }else{
                    parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
                }

                return result;
            }

            std::vector<ns5__SignatureType *> childs = parentAttribute->SignedAttributes->ns3__SignatureAttribute;
            if (parentAttribute->SignedAttributes == NULL || childs.size() == 0) {
                qDebug() << "getAttributes(): Empty attributes response!";
                parent->signalCompanyAttributesLoadedError();
                return result;
            }

            qDebug() << "Creating cache file on location: " << fileLocation;
            QFile cacheFile(fileLocation);
            
            if (cacheFile.open(QIODevice::WriteOnly)) {
                cacheFile.write(replyString.c_str(), replyString.length());
            }
            else
            {
                std::cerr << "Couldn't save attribute result to cache. Error: " << cacheFile.errorString().toStdString() << std::endl;
				parent->signalCacheNotWritable();
            }
        } else {

            if(allEnterprises) {
                parent->signalCompanyAttributesLoadedError();
            }else{
                parent->signalEntityAttributesLoadedError();
            }
        }
        
        result = attr_response.AttributeResponseValues;
    }
    catch(...) {
        parent->signalSCAPDefinitionsServiceFail(GAPI::ScapGenericError, allEnterprises);
        qDebug() << "reqAttributeSupplierListType ERROR << - TODO: improve error handling";
    }

    return result;
}
