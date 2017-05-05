#include "pdfsignatureclient.h"
#include "scapsignature.h"

#include "treeiteminfo.h"

#include "qfileinfo.h"
#include "qfile.h"
#include <string.h>

#include "settings.h"

#include "eidlibdefines.h"
#include "ACService/envStub.h"

#include "PDFSignature/PDFSignatureH.h"
#include "PDFSignature/PDFSignatureSoapBindingProxy.h"
#include "PDFSignature/PDFSignatureSoapBinding.nsmap"

#include "ACService/ACServiceH.h"

//using namespace PDFSignature;

PDFSignature::ns1__AttributeType* convertAttributeType(ACService::ns3__AttributeType *, soap *);

PDFSignatureClient::PDFSignatureClient()
{

}
const char * processId = "10001";
const char * pdf_endpoint = "/PADES/PDFSignature";

bool PDFSignatureClient::signPDF(ProxyInfo proxyInfo, QString finalfilepath, QString filepath, QString citizenName, QString citizenId, int ltv, PDFSignatureInfo signatureInfo, std::vector<ACService::ns3__AttributeType *> &attributeTypeList) {
    //const SOAP_ENV__Fault * fault = NULL;

    soap * sp = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);

    //Define appropriate network timeouts
    sp->recv_timeout = 20;
    sp->send_timeout = 20;
    sp->connect_timeout = 20;

    soap_set_namespaces(sp, namespacesPDFSignature);

	//TODO: this disables server certificate verification !!
	soap_ssl_client_context(sp, SOAP_SSL_NO_AUTHENTICATION, NULL, NULL, NULL, NULL, NULL);

    PDFSignature::PDFSignatureSoapBindingProxy proxy(sp);

    // Get endpoint from settings
    ScapSettings settings;
	std::string s_endpoint = QString("https://" + settings.getScapServerHost() + ":" + settings.getScapServerPort().append(pdf_endpoint)).toStdString();

	std::string proxy_host;
	long proxy_port;
	//Proxy support using the gsoap BindingProxy
	if (proxyInfo.isSystemProxy())
	{
		proxyInfo.getProxyForHost(s_endpoint, &proxy_host, &proxy_port);
		if (proxy_host.size() > 0)
		{
			sp->proxy_host = proxy_host.c_str();
			sp->proxy_port = proxy_port;

			eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "signPDF: Using System Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);
		}
	}
	else if (proxyInfo.getProxyHost().size() > 0)
	{
		sp->proxy_host = proxyInfo.getProxyHost().toUtf8().constData();
		sp->proxy_port = proxyInfo.getProxyPort().toLong();
		eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature", "signPDF: Using Manual Proxy: host=%s, port=%ld", sp->proxy_host, sp->proxy_port);

		if (proxyInfo.getProxyUser().size() > 0)
		{
			sp->proxy_userid = proxyInfo.getProxyUser().toUtf8().constData();
			sp->proxy_passwd = proxyInfo.getProxyPwd().toUtf8().constData();
		}
	}

    const char * c_endpoint = s_endpoint.c_str();
    proxy.soap_endpoint = c_endpoint;

    std::cout << "PDF Signing endpoint: " << c_endpoint << std::endl;

    // Get PDF File
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly))
        return false; // Error opening file

    // Finds signature position
    static const char needleValues[] = { 0x2F, 0x54, 0x20, 0x28 };
    static const char endNeedle = 0x29;
    QByteArray needle = QByteArray::fromRawData(needleValues, sizeof(needleValues));
    QByteArray fileByteArray = file.readAll();
    char * fileBinary = fileByteArray.data();
    int pdfBinaryLen = fileByteArray.size();

    int indexOfNeedle = fileByteArray.lastIndexOf(needle);
    if(indexOfNeedle < 0)
    {
        std::cout << "Could not find signature field1: " << pdfBinaryLen << std::endl;
        return false;
    }
    indexOfNeedle += needle.length();

    int i = indexOfNeedle;
    int endOfNeedle = 0;
    while( i != pdfBinaryLen )
    {
        if(fileByteArray.at(i) == endNeedle)
        {
            endOfNeedle = i;
            break;
        }
        i++;
    }

    if(endOfNeedle == 0)
    {
        std::cout << "Could not find signature field2: " << pdfBinaryLen << std::endl;
        return false;
    }
    QString signatureField(fileByteArray.mid(indexOfNeedle, endOfNeedle-indexOfNeedle));


    // Gets PDF Base 64
    PDFSignature::xsd__base64Binary * base64PDF = PDFSignature::soap_new_set_xsd__base64Binary(sp, (unsigned char *)fileBinary, pdfBinaryLen, NULL, NULL, NULL);


    std::string citizenNIC = citizenId.toStdString();
    citizenNIC = citizenNIC.substr(2, citizenNIC.length() - 2);
    // Get Citizen Info
    PDFSignature::ns1__PersonalDataType * personalData = PDFSignature::soap_new_req_ns1__PersonalDataType(sp, citizenName.toStdString(), citizenNIC);
    std::cerr << "Citizen Name : " << citizenName.toStdString() << std::endl;
    std::cerr << "Citizen NIC : " << citizenNIC << std::endl;


    // Get Attributes selected on Tree
    std::vector<PDFSignature::ns1__AttributeType *> attributes;
    for (uint pos = 0; pos < attributeTypeList.size(); pos++)
    {
        ACService::ns3__AttributeType * acAttributeType = attributeTypeList.at(pos);

        PDFSignature::ns1__AttributeType * convertedAttribute = convertAttributeType(acAttributeType, sp);
        attributes.push_back(convertedAttribute);
    }

    // Create Attribute list type
    PDFSignature::ns1__AttributeListType *attributeList = PDFSignature::soap_new_req_ns1__AttributeListType(sp, attributes);

    PDFSignature::ns1__SignatureOrientationEnumType orientationType =
            signatureInfo.isPortrait() ? PDFSignature::ns1__SignatureOrientationEnumType__PORTRAIT : PDFSignature::ns1__SignatureOrientationEnumType__LANDSCAPE;

    // Request Sign PDF
    try
    {
        PDFSignature::ns1__SignRequest * signRequest = PDFSignature::soap_new_set_ns1__SignRequest(sp, processId, personalData, attributeList,
                        signatureField.toStdString(), *base64PDF, &ltv, signatureInfo.getSelectedPage(), signatureInfo.getX(), signatureInfo.getY(), orientationType);

        PDFSignature::ns1__SignResponse resp;
        int rc = proxy.Sign(signRequest, resp);

        if (rc != SOAP_OK)
        {
            if (rc == SOAP_FAULT)
            {
                //fault = proxy.soap_fault();
				if (proxy.soap->fault != NULL && proxy.soap->fault->faultstring != NULL)
					eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "PDF Service returned SOAP Fault: %s", proxy.soap->fault->faultstring);
            }
            else
            {
                eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", 
					"Error in PDFSignature::sign(): GSoap returned Error code: %d", rc);
            }
            return false;
        }
        std::string respCode = resp.Status->Code;

        if( respCode.compare("00") != 0 )
        {

            eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "PDF Service application-level error Code: %s Message: %S", respCode.c_str(), 
			resp.Status->Message.c_str());
            std::cerr << "Service error code " << respCode << ". Service returned Message: " << resp.Status->Message << std::endl;
            return false;
        }

        std::cout << "Service returned Message: " << resp.Status->Message << std::endl;

        // Save PDF to final destination
        PDFSignature::xsd__base64Binary *signedDoc = resp.SignedDocument;
        QFile signedDocFile(finalfilepath);

        if (!signedDocFile.open(QIODevice::ReadWrite))
        {
            fprintf(stderr, "Could not write on selected destination");
            return false;
        }

        signedDocFile.write( (const char *)signedDoc->__ptr, signedDoc->__size);
        signedDocFile.close();

        return true;
    }
    catch(...)
    {
        const char * error_msg = "Exception thrown in ns1__SignResponse. Signed PDF File not written";
        fputs(error_msg, stderr);

        eIDMW::PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_ERROR, "ScapSignature", "%S", error_msg);
        return false;
    }

}

PDFSignature::ns1__AttributeType* convertAttributeType(ACService::ns3__AttributeType * attributeType, soap * sp){
    PDFSignature::ns1__AttributeType * convertedAttribute = PDFSignature::soap_new_ns1__AttributeType(sp);

    // Attribute suppliers
    PDFSignature::ns1__AttributeSupplierType * attrSuppl = PDFSignature::soap_new_ns1__AttributeSupplierType(sp);
    attrSuppl->Id = attributeType->AttributeSupplier->Id;
    attrSuppl->Name = attributeType->AttributeSupplier->Name;

    // Personal Data
    PDFSignature::ns1__PersonalDataType * persData = PDFSignature::soap_new_ns1__PersonalDataType(sp);
    persData->Name = attributeType->PersonalData->Name;
    persData->NIC = attributeType->PersonalData->NIC;

    // Main Attribute
    PDFSignature::ns1__MainAttributeType *main_attrib = PDFSignature::soap_new_ns1__MainAttributeType(sp);
    main_attrib->AttributeID = attributeType->MainAttribute->AttributeID;
    main_attrib->Description = attributeType->MainAttribute->Description;

    //Main LegalAct List
    PDFSignature::ns1__LegalActListType *legalActList = PDFSignature::soap_new_ns1__LegalActListType(sp);
//    std::cout << "Legal act: " << std::endl;
//    for(uint i = 0; i < attributeType->MainAttribute->LegalActList->LegalAct.size(); i++){
//        std::string string = attributeType->MainAttribute->LegalActList->LegalAct.at(0);
//        for(uint c = 0; c<string.length(); c++){
//            printf("%06x ", string.at(c));
//        }

//    }

    if (attributeType->MainAttribute->LegalActList != NULL)
    {
        legalActList->LegalAct = attributeType->MainAttribute->LegalActList->LegalAct;
        main_attrib->LegalActList = legalActList;
    }

    if (attributeType->MainAttribute->SubAttributeList != NULL)
    {

        //Main Sub AttributeList
        PDFSignature::ns1__SubAttributeListType *subAttrList = PDFSignature::soap_new_ns1__SubAttributeListType(sp);
        for(uint i = 0; i < attributeType->MainAttribute->SubAttributeList->SubAttribute.size(); i++){
            ACService::ns3__SubAttributeType *acSubAttr = attributeType->MainAttribute->SubAttributeList->SubAttribute.at(i);
            PDFSignature::ns1__SubAttributeType * subAttr = PDFSignature::soap_new_ns1__SubAttributeType(sp);
            subAttr->AttributeID = acSubAttr->AttributeID;
            subAttr->Description = acSubAttr->Description;
            subAttr->Value = acSubAttr->Value;
            subAttrList->SubAttribute.push_back(subAttr);
        }

        main_attrib->SubAttributeList = subAttrList;
    }

    // Final
    convertedAttribute->AttributeSupplier = attrSuppl;
    convertedAttribute->MainAttribute = main_attrib;

    return convertedAttribute;
}

