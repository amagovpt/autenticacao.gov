
#Link to libgsoapssl++ to have available SSL binding
all:
	 g++ -O2 -o attrsuppliers-client soapC.cpp soapAttributeSupplierBindingProxy.cpp AtributeSuppliers-client.cpp -lgsoapssl++

generate-code:
	wsdl2h -o SCAP-ACService.h ACService.wsdl
	soapcpp2 -j SCAP-ACService.h
