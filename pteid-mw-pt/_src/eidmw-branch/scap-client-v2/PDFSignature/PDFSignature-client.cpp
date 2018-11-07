#include "PDFSignatureH.h"
#include "PDFSignatureSoapBindingProxy.h"
#include "PDFSignatureSoapBinding.nsmap"

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//STD Library
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


char * readCompleteFile(char *filePath, size_t *bytes_read)
{
    FILE *pFile;
    char *buffer;
    size_t result;
    struct stat statbuf;
    stat(filePath, &statbuf);

    if (!(pFile = fopen(filePath, "r"))) {
    	fprintf(stderr, "File error for file: %s\n", filePath); 
    	return NULL;
    }

    // memory is cheap!!
    if ((buffer = (char*) malloc(statbuf.st_size)) ==0) {
    	fputs ("Memory error\n", stderr); return NULL;
    }
    result = fread(buffer, 1, statbuf.st_size, pFile);

    fclose (pFile);
    if (result != statbuf.st_size) 
    {
    		fputs ("Read error\n", stderr); return NULL;
    }

    *bytes_read = result; 
    fprintf(stderr, "PDF file read: %lu bytes\n", result);

    return buffer;
}

using namespace PDFSignature;

int main(int argc, char **argv)
{
	PDFSignatureSoapBindingProxy proxy;
	const SOAP_ENV__Fault * fault = NULL;

	//proxy.soap_endpoint = "http://localhost:8900/PADES/PDFSignature";
	proxy.soap_endpoint = "https://preprod.scap.autenticacao.gov.pt/PADES/PDFSignature";

	soap * soap = soap_new();
	int page = 1;
	int x = 100;
	int y = 100;
	std::string processId = "10001";

	if (argc < 2)
	{
		printf("Missing argument: \n");
	}
	
	ns1__SignResponse resp;

	size_t pdfBinaryLen = 0;
	char * pdfBinary = readCompleteFile(argv[1], &pdfBinaryLen);
	

	if (!pdfBinary)
	{
		return 1;
	}

	ns1__PersonalDataType * personalData = soap_new_req_ns1__PersonalDataType(soap, "Ricardo Valido Emv Cap", "99000434");

	xsd__base64Binary * base64PDF = soap_new_set_xsd__base64Binary(soap, (unsigned char *)pdfBinary, pdfBinaryLen, NULL, NULL, NULL);


	std::vector<ns1__AttributeType *> attributes;
	std::string attributeId = "http://interop.gov.pt/SCCC/506945901/4CA29A23-4FAC-4064-9835-9C56D4C98B2D";

	ns1__MainAttributeType *main_attrib = soap_new_req_ns1__MainAttributeType(soap, attributeId);
	/*
	soap_new_req_ns1__AttributeType * attr = soap_new_req_ns1__AttributeType(
	struct soap *soap,
	ns1__AttributeSupplierType *AttributeSupplier,
	ns1__MainAttributeType *MainAttribute)
	*/

	ns1__AttributeSupplierType * supp = soap_new_set_ns1__AttributeSupplierType(soap, "http://interop.gov.pt/SCCC/506945901", "Caixa Magica", NULL);

	ns1__AttributeType* attr1 = soap_new_req_ns1__AttributeType(soap, supp, main_attrib);

	attributes.push_back(attr1);

	ns1__AttributeListType *attributeList = soap_new_req_ns1__AttributeListType(soap, attributes);

	ns1__SignRequest * signRequest = soap_new_req_ns1__SignRequest(
		soap, processId, personalData, attributeList, "Signature1202688459", *base64PDF, page, x, y, ns1__SignatureOrientationEnumType__PORTRAIT);

	//ns1__SignRequest *ns1__SignRequest_, ns1__SignResponse &ns1__SignResponse_
	int rc = proxy.Sign(signRequest, resp);

	if (rc != SOAP_OK)
	{

		if (rc == SOAP_FAULT)
		{
			fault = proxy.soap_fault();
			fprintf(stderr, "SOAP Fault! %s\n", fault->faultstring);
			//fault->
		}
		else
		{
			fprintf(stderr, "Error in proxy.Sign(): Error code: %d\n", rc);
			return 1;
		}
	}

	fprintf(stderr, "Service returned status: %s\n", resp.Status->Code.c_str());
	fprintf(stderr, "Service returned Message: %s\n", resp.Status->Message.c_str());

	xsd__base64Binary *final_pdf = resp.SignedDocument;

	return 0;
}
