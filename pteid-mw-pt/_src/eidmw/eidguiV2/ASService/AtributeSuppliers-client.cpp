#include "soapH.h"             
#include "AttributeSupplierBinding.nsmap"
#include "soapAttributeSupplierBindingProxy.h"

//STD Library
#include <iostream>
#include <sstream>
#include <vector>


int main()
{

	AttributeSupplierBindingProxy suppliers_proxy;
	ns2__AttributeSupplierResponseType suppliers_resp;

	suppliers_proxy.soap_endpoint = "https://preprod.scap.autenticacao.gov.pt/DSS/ASService";

	int ret = suppliers_proxy.AttributeSuppliers(suppliers_resp);

	if (ret != SOAP_OK)
	{
		fprintf(stderr, "Error in AttributeSuppliers: Error code: %d\n", ret);
		return 1;
	}

	std::vector<ns3__AttributeSupplierType *> suppliers = suppliers_resp.AttributeSupplier;
	printf("Returned %lu suppliers!\n", suppliers.size());

	for (int i = 0; i!=suppliers.size(); i++)
	{
		printf("Fornecedor de atributos: ID=> %s Nome=> %s\n", suppliers[i]->Id.c_str(), suppliers[i]->Name.c_str());
	}

/*
	std::string processId = "100001";
	std::vector<ns3__AttributeSupplierType *> vec_suppliers;

	vec_suppliers.push_back(suppliers[5]);
	ns2__AttributeSupplierListType * suppliers_list = soap_new_req_ns2__AttributeSupplierListType(soap,	vec_suppliers);

	ns3__PersonalDataType * citizen = 
	soap_new_req_ns3__PersonalDataType(soap, "Ricardo Valido", "99000434");

	ns2__AttributeRequestType * attr_request = soap_new_req_ns2__AttributeRequestType(soap, processId, citizen, suppliers_list);

	// write to string via std::ostream* soap->os
	soap->os = &ss;
	if (soap_write_ns2__AttributeRequestType(soap, attr_request))
	{
		std::cerr << "Error!" <<std::endl;
	}

	std::cout << ss.str() << std::endl;*/


}
