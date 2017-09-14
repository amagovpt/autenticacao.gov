#include <vector>
#include <sstream>
#include "eidlib.h"

#include "SCAPServices/SCAPH.h"
#include "SCAPServices/SCAP.nsmap"
#include "scapsignature.h"
#include "ScapSettings.h"
#include <QDir>


std::vector<ns2__AttributesType *> 
    ScapServices::loadAttributesFromCache(eIDMW::PTEID_EIDCard &card) {

    std::vector<ns2__AttributesType *> attributesType;
    try
    {

        QString citizenNIC (card.getID().getCivilianIdNumber());

        // Save to cache
        ScapSettings settings;
        QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
        QString fileLocation = scapCacheDir + citizenNIC + ".xml";

        QFile cacheFile(fileLocation);
        if( !cacheFile.open(QIODevice::ReadOnly) ) {
            std::cerr << "Couldn't load attributes from cache. Error: " << cacheFile.errorString().toStdString() << std::endl;
            return attributesType;
        }
        qDebug() << "Reading XML cached file";

        QByteArray fileContent = cacheFile.readAll();

        // Convert string to istream
        std::istringstream replyStream(fileContent.data());
        std::istream * istream = &replyStream;

        // Create soap request with generated body content.
        soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
        //TODO: this disables server certificate verification !!
        soap_set_namespaces(soap2, SCAPnamespaces);

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
    }
    catch(...) {
        std::cerr << "Error ocurred while loading attributes from cache";
    }

    return attributesType;
}

void ScapServices::getCompanyAttributes(eIDMW::PTEID_EIDCard &card) {

    std::vector<ns2__AttributesType *> result;
    bool allEnterprises = true;
    const char * soapAction = "http://www.cartaodecidadao.pt/services/ccc/ACS/Operations/Attributes";
	const char * ac_endpoint = "/DSS/ACService";
    qDebug() << "C++: getCompanyAttributes called";

    try {

        soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
		//TODO: this disables server certificate verification !!
		soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

        // Get suppliers List
        std::vector<ns3__AttributeSupplierType *> vec_suppliers;

        /*TODO: what do we do to fill the dummy attribute suppliers ??
        for(int i = 0; i < attributeSupplierTypeVec.size(); i++)
        {
            ns3__AttributeSupplierType * attSupType = attributeSupplierTypeVec.at(i);

            ns3__AttributeSupplierType * ac_attributeSupplierType = soap_new_req_ns3__AttributeSupplierType(sp,attSupType->Id, attSupType->Name);
            vec_suppliers.push_back(ac_attributeSupplierType);
        }
        */
        ns2__AttributeSupplierListType * suppliers = soap_new_req_ns2__AttributeSupplierListType(sp, vec_suppliers);


        // Get Citizen info
        QString fullname = card.getID().getGivenName();
        fullname.append(" ");
        fullname.append(card.getID().getSurname());

        const char* idNumber = card.getID().getCivilianIdNumber();

        ns3__PersonalDataType * citizen = soap_new_req_ns3__PersonalDataType(sp, fullname.toStdString(), idNumber);

        // Get Attribute request
        //ns2__AttributeRequestType * attr_request = soap_new_req_ns2__AttributeRequestType(sp, "10001", citizen, suppliers);
        ns2__AttributeRequestType * attr_request = soap_new_set_ns2__AttributeRequestType(sp, "10001", citizen, suppliers, &allEnterprises);
        std::stringstream ss;
        sp->os = &ss;
        if (soap_write_ns2__AttributeRequestType(sp, attr_request))
        {
            qDebug() << "Error serializing AttributeRequest!";
        }

        std::string s_soapBody = ss.str();
        s_soapBody.erase(0, s_soapBody.find("\n") + 1);


        // Get host from configuration
        ScapSettings settings;
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
        scapCacheDir.mkpath(s_scapCacheDir);

        QString fileLocation = s_scapCacheDir + idNumber + ".xml";

        qDebug() << "Creating cache file on location: " << fileLocation;
        QFile cacheFile(fileLocation);
        if (cacheFile.open(QIODevice::WriteOnly)) {
            cacheFile.write(replyString.c_str(), replyString.length());
        }
        else
        {
            std::cerr << "Couldn't save attribute result to cache. Error: " << cacheFile.errorString().toStdString() << std::endl;
        }

        // Convert string to istream
        std::istringstream replyStream(replyString);
        std::istream * istream = &replyStream;

        // Create soap request with generated body content.
        soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

        soap_set_namespaces(soap2, SCAPnamespaces);

        soap2->is = istream;

        // Retrieve ns2__AttributeResponseType
        ns2__AttributeResponseType attr_response;
        long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);


        std::cerr << "Got response from converting XML to object. Size: "<< attr_response.AttributeResponseValues.size()  << std::endl;

        if (ret != 0) {
            std::cerr << "Error reading AttributeResponseType" << std::endl;
            //return result;
        }
        result = attr_response.AttributeResponseValues;
    }
    catch(...) {
        qDebug() << "reqAttributeSupplierListType ERROR << - TODO: improve error handling";
    }

}