#include "ACServiceH.h"
#include "new.nsmap"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>

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


int main(int argc, char **argv)
{
			size_t bytes = 0;
			//struct soap soap2;
   			soap * soap2 = soap_new2(SOAP_C_UTFSTRING, SOAP_C_UTFSTRING);
            //soap * soap2 = soap_new2(0, 0);
            
            //soap_set_namespaces(&soap2, namespacesACService);
			char *file_content = readCompleteFile(argv[1], &bytes);

	       // Convert string to istream
            std::istringstream replyStream;
            replyStream.str(file_content);
            std::istream * istream = &replyStream;

            // Create soap request with generated body content
            //struct soap *soapResponse = soap_new();
            soap2->is = istream;

            // Retrieve ns2__AttributeResponseType
            ACService::ns2__AttributeResponseType attr_response;
            long ret = soap_read_ns2__AttributeResponseType(soap2, &attr_response);

            std::cerr << "Return value: " << attr_response.AttributeResponseValues.size() << std::endl;

            ACService::ns2__AttributesType * attributesType = attr_response.AttributeResponseValues.at(0);

            //std::cout << attributesType->ATTRSupplier->Name << std::endl;

            std::stringstream ss;
            soap2->os = &ss;
            if (soap_write_ns2__AttributesType(soap2, attributesType))
            {
                std::cerr << "Error!" <<std::endl;
            }

            std::string attribute_str = ss.str();

            std::cout << "Serialized Attribute: " << attribute_str << std::endl;

}