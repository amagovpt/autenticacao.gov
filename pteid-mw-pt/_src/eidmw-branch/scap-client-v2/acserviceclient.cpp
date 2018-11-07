#include "acserviceclient.h"

#include "ACService/ACServiceH.h"
#include "ACService/AttributeClientServiceBinding.nsmap"
#include "ASService/soapH.h"

#include <sstream>
#include <QString>
#include <vector>

#include "eidlibdefines.h"

#include "settings.h"
#include "treeiteminfo.h"

#include <eidlib.h>
#include "eidlibException.h"

ACServiceClient::ACServiceClient()
{

}

const char * soapAction = "http://www.cartaodecidadao.pt/services/ccc/ACS/Operations/Attributes";
const char * ac_endpoint = "/DSS/ACService";

std::vector<ACService::ns2__AttributesType *> ACServiceClient::reqAttributeSupplierListType(std::vector<ns3__AttributeSupplierType *> attributeSupplierTypeVec){
    std::vector<ACService::ns2__AttributesType *> result;
    try
    {

        eIDMW::PTEID_ReaderContext& readerContext = eIDMW::ReaderSet.getReader();

        if (readerContext.isCardPresent())
        {
            soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
			//TODO: this disables server certificate verification !!
			soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

            // Get suppliers List
            std::vector<ACService::ns3__AttributeSupplierType *> vec_suppliers;
            for(int i = 0; i < attributeSupplierTypeVec.size(); i++)
            {
                ns3__AttributeSupplierType * attSupType = attributeSupplierTypeVec.at(i);

                ACService::ns3__AttributeSupplierType * ac_attributeSupplierType = ACService::soap_new_req_ns3__AttributeSupplierType(sp,attSupType->Id, attSupType->Name);
                vec_suppliers.push_back(ac_attributeSupplierType);
            }
            ACService::ns2__AttributeSupplierListType * suppliers = ACService::soap_new_req_ns2__AttributeSupplierListType(sp,	vec_suppliers);


            // Get Citizen info
            QString fullname = readerContext.getEIDCard().getID().getGivenName();
            fullname.append(" ");
            fullname.append(readerContext.getEIDCard().getID().getSurname());
            const char* idNumber = readerContext.getEIDCard().getID().getCivilianIdNumber();
            ACService::ns3__PersonalDataType * citizen = ACService::soap_new_req_ns3__PersonalDataType(sp, fullname.toStdString(), idNumber);


            // Get Attribute request
            ACService::ns2__AttributeRequestType * attr_request = ACService::soap_new_req_ns2__AttributeRequestType(sp, "10001", citizen, suppliers);
            std::stringstream ss;
            sp->os = &ss;
            if (soap_write_ns2__AttributeRequestType(sp, attr_request))
            {
                std::cerr << "Error!" <<std::endl;
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

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR
                            , "ScapSignature"
                            , "ACService returned: %s"
                            , ( scapResult ? scapResult : "Null SCAP result" ) );

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

            std::cout << "Creating cache file on location: " << fileLocation.toStdString() << std::endl;
            QFile cacheFile(fileLocation);
            if( cacheFile.open(QIODevice::WriteOnly) ) {
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
			//TODO: this disables server certificate verification !!
            soap_set_namespaces(soap2, namespacesACService);

            soap2->is = istream;

            // Retrieve ns2__AttributeResponseType
            ACService::ns2__AttributeResponseType attr_response;
            long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);


            std::cerr << "Got response from converting XML to object. Size: "<< attr_response.AttributeResponseValues.size()  << std::endl;

            if(ret != 0) {
                std::cerr << "Error reading AttributeResponseType" << std::endl;
                return result;
            }
            result = attr_response.AttributeResponseValues;
        }
    }
    catch(...){
        std::cout << "reqAttributeSupplierListType ERROR" << std::endl << std::flush;
    }
    return result;
}

TreeItemInfo ACServiceClient::getAttrSupplierInfo(ACService::ns2__AttributesType * attributesSupplier){
    return TreeItemInfo(attributesSupplier->ATTRSupplier->Name.c_str(), attributesSupplier->ATTRSupplier->Id.c_str());
}

std::vector<TreeItemInfo> ACServiceClient::getChildrenTreeInfo(ACService::ns2__AttributesType * attributesSupplier){
    std::vector<TreeItemInfo> childrensList;
    std::vector<ACService::ns5__SignatureType *> signatureAttributeList =     attributesSupplier->SignedAttributes->ns3__SignatureAttribute;

    for(uint i = 0; i < signatureAttributeList.size(); i++){
        ACService::ns5__SignatureType * signatureType = signatureAttributeList.at(i);
        if(signatureType->ns5__Object.size() > 0)
        {
            ACService::ns5__ObjectType * signatureObject = signatureType->ns5__Object.at(0);
            ACService::ns3__MainAttributeType * mainAttributeObject = signatureObject->union_ObjectType.ns3__Attribute->MainAttribute;

            std::string description = mainAttributeObject->Description->c_str();

            QString subAttributes(" (");
            QString subAttributesValues;
            for(uint subAttributePos = 0; subAttributePos < mainAttributeObject->SubAttributeList->SubAttribute.size(); subAttributePos++){
                ACService::ns3__SubAttributeType * subAttribute = mainAttributeObject->SubAttributeList->SubAttribute.at(subAttributePos);

                QString subDescription(subAttribute->Description->c_str());
                QString subValue(subAttribute->Value->c_str());
                subAttributesValues.append(subDescription + ": " + subValue + ", ");
            }
            // Chop 2 to remove last 2 chars (', ')
            subAttributesValues.chop(2);
            subAttributes.append(subAttributesValues + ")");

            std::cout << "Sub attributes : " << subAttributes.toStdString() << std::endl;

            description += subAttributes.toStdString();
            childrensList.push_back(TreeItemInfo(description.c_str(), mainAttributeObject->AttributeID.c_str()));
        }
    }
    return childrensList;
}

ACService::ns3__AttributeType * ACServiceClient::getAttributeType(QString parentID, QString childID, std::vector<ACService::ns2__AttributesType *> attributesTypeList){
    for(uint parentPos = 0;  parentPos < attributesTypeList.size(); parentPos++)
    {
        ACService::ns2__AttributesType * parent = attributesTypeList.at(parentPos);
        QString pId(parent->ATTRSupplier->Id.c_str());
        if(pId.compare(parentID) == 0)
        {
            std::vector<ACService::ns5__SignatureType *> childs = parent->SignedAttributes->ns3__SignatureAttribute;
            for(uint childPos = 0;  childPos < childs.size(); childPos++)
            {
                ACService::ns5__SignatureType * child = childs.at(childPos);
                if ( child->ns5__Object.size() > 0 )
                {
                    ACService::ns5__ObjectType * objType = child->ns5__Object.at(0);
                    QString cId(objType->union_ObjectType.ns3__Attribute->MainAttribute->AttributeID.c_str());
                    if (cId.compare(childID) == 0)
                    {
                         ACService::ns3__AttributeType * attr = objType->union_ObjectType.ns3__Attribute;
                         //"Fix" the AttributeSupplier (ID and Name): they can have different values in SCAP and the actual entity Attribute
                         attr->AttributeSupplier->Id = parent->ATTRSupplier->Id;
                         attr->AttributeSupplier->Name = parent->ATTRSupplier->Name;
                         return attr;
                    }
                }
            }
        }
    }
    return NULL;
}


std::vector<ACService::ns2__AttributesType *> ACServiceClient::loadAttributesFromCache()
{
    std::vector<ACService::ns2__AttributesType *> attributesType;
    try
    {
        eIDMW::PTEID_ReaderContext& readerContext = eIDMW::ReaderSet.getReader();

        if (readerContext.isCardPresent())
        {
            QString citizenNIC (readerContext.getEIDCard().getID().getCivilianIdNumber());

            // Save to cache
            ScapSettings settings;
            QString scapCacheDir = settings.getCacheDir() + "/scap_attributes/";
            QString fileLocation = scapCacheDir + citizenNIC + ".xml";

            QFile cacheFile(fileLocation);
            if( !cacheFile.open(QIODevice::ReadOnly) ) {
                std::cerr << "Couldn't load attributes from cache. Error: " << cacheFile.errorString().toStdString() << std::endl;
                return attributesType;
            }
            std::cout << "Reading XML cached file" << std::endl;

            QByteArray fileContent = cacheFile.readAll();

            // Convert string to istream
            std::istringstream replyStream(fileContent.data());
            std::istream * istream = &replyStream;

            // Create soap request with generated body content.
            soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
            //TODO: this disables server certificate verification !!
            soap_set_namespaces(soap2, namespacesACService);

            soap2->is = istream;

            // Retrieve ns2__AttributeResponseType
            ACService::ns2__AttributeResponseType attr_response;
            long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);

            std::cout << "Got response from converting XML to object. Size: "<< attr_response.AttributeResponseValues.size()  << std::endl;

            if (ret != 0) {
                std::cerr << "Error reading AttributeResponseType" << std::endl;
                return attributesType;
            }
            attributesType = attr_response.AttributeResponseValues;
        }
    }
    catch(...){
        std::cerr << "Error ocurred while loading attributes from cache";
    }

    return attributesType;
}
