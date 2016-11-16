#ifndef SCAPACSERVICE_H
#define SCAPACSERVICE_H

#include "soapStub.h"

#ifndef SOAP_TYPE_ns2__AttributeSupplierListType
#define SOAP_TYPE_ns2__AttributeSupplierListType (111)
#endif
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns2__AttributeSupplierListType(struct soap*, const char*, int, const ns2__AttributeSupplierListType *, const char*);
SOAP_FMAC3 ns2__AttributeSupplierListType * SOAP_FMAC4 soap_in_ns2__AttributeSupplierListType(struct soap*, const char*, ns2__AttributeSupplierListType *, const char*);
SOAP_FMAC1 ns2__AttributeSupplierListType * SOAP_FMAC2 soap_instantiate_ns2__AttributeSupplierListType(struct soap*, int, const char*, const char*, size_t*);

inline ns2__AttributeSupplierListType * soap_new_ns2__AttributeSupplierListType(struct soap *soap, int n = -1) { return soap_instantiate_ns2__AttributeSupplierListType(soap, n, NULL, NULL, NULL); }

inline ns2__AttributeSupplierListType * soap_new_req_ns2__AttributeSupplierListType(
    struct soap *soap,
    const std::vector<ns3__AttributeSupplierType *> & AttributeSupplier)
{	ns2__AttributeSupplierListType *_p = soap_new_ns2__AttributeSupplierListType(soap);
    if (_p)
    {	_p->soap_default(soap);
        _p->ns2__AttributeSupplierListType::AttributeSupplier = AttributeSupplier;
    }
    return _p;
}

inline ns2__AttributeSupplierListType * soap_new_set_ns2__AttributeSupplierListType(
    struct soap *soap,
    const std::vector<ns3__AttributeSupplierType *> & AttributeSupplier)
{	ns2__AttributeSupplierListType *_p = soap_new_ns2__AttributeSupplierListType(soap);
    if (_p)
    {	_p->soap_default(soap);
        _p->ns2__AttributeSupplierListType::AttributeSupplier = AttributeSupplier;
    }
    return _p;
}

#ifndef soap_write_ns2__AttributeSupplierListType
#define soap_write_ns2__AttributeSupplierListType(soap, data) ( soap_free_temp(soap), soap_begin_send(soap) || ((data)->soap_serialize(soap), 0) || (data)->soap_put(soap, "ns2:AttributeSupplierListType", NULL) || soap_end_send(soap), (soap)->error )
#endif

SOAP_FMAC3 ns2__AttributeSupplierListType * SOAP_FMAC4 soap_get_ns2__AttributeSupplierListType(struct soap*, ns2__AttributeSupplierListType *, const char*, const char*);

#ifndef soap_read_ns2__AttributeSupplierListType
#define soap_read_ns2__AttributeSupplierListType(soap, data) ( ((data) ? ((data)->soap_default(soap), 0) : 0) || soap_begin_recv(soap) || !soap_get_ns2__AttributeSupplierListType(soap, (data), NULL, NULL) || soap_end_recv(soap), (soap)->error )
#endif

#ifndef SOAP_TYPE_ns2__AttributeSupplierResponseType
#define SOAP_TYPE_ns2__AttributeSupplierResponseType (108)
#endif
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns2__AttributeSupplierResponseType(struct soap*, const char*, int, const ns2__AttributeSupplierResponseType *, const char*);
SOAP_FMAC3 ns2__AttributeSupplierResponseType * SOAP_FMAC4 soap_in_ns2__AttributeSupplierResponseType(struct soap*, const char*, ns2__AttributeSupplierResponseType *, const char*);
SOAP_FMAC1 ns2__AttributeSupplierResponseType * SOAP_FMAC2 soap_instantiate_ns2__AttributeSupplierResponseType(struct soap*, int, const char*, const char*, size_t*);

inline ns2__AttributeSupplierResponseType * soap_new_ns2__AttributeSupplierResponseType(struct soap *soap, int n = -1) { return soap_instantiate_ns2__AttributeSupplierResponseType(soap, n, NULL, NULL, NULL); }

inline ns2__AttributeSupplierResponseType * soap_new_req_ns2__AttributeSupplierResponseType(
    struct soap *soap)
{   ns2__AttributeSupplierResponseType *_p = soap_new_ns2__AttributeSupplierResponseType(soap);
    if (_p)
    {   _p->soap_default(soap);
    }
    return _p;
}

inline ns2__AttributeSupplierResponseType * soap_new_set_ns2__AttributeSupplierResponseType(
    struct soap *soap,
    const std::vector<ns3__AttributeSupplierType *> & AttributeSupplier,
    std::string *ErrorMessage)
{   ns2__AttributeSupplierResponseType *_p = soap_new_ns2__AttributeSupplierResponseType(soap);
    if (_p)
    {   _p->soap_default(soap);
        _p->ns2__AttributeSupplierResponseType::AttributeSupplier = AttributeSupplier;
        _p->ns2__AttributeSupplierResponseType::ErrorMessage = ErrorMessage;
    }
    return _p;
}

#ifndef soap_write_ns2__AttributeSupplierResponseType
#define soap_write_ns2__AttributeSupplierResponseType(soap, data) ( soap_free_temp(soap), soap_begin_send(soap) || ((data)->soap_serialize(soap), 0) || (data)->soap_put(soap, "ns2:AttributeSupplierResponseType", NULL) || soap_end_send(soap), (soap)->error )
#endif

SOAP_FMAC3 ns2__AttributeSupplierResponseType * SOAP_FMAC4 soap_get_ns2__AttributeSupplierResponseType(struct soap*, ns2__AttributeSupplierResponseType *, const char*, const char*);

#ifndef soap_read_ns2__AttributeSupplierResponseType
#define soap_read_ns2__AttributeSupplierResponseType(soap, data) ( ((data) ? ((data)->soap_default(soap), 0) : 0) || soap_begin_recv(soap) || !soap_get_ns2__AttributeSupplierResponseType(soap, (data), NULL, NULL) || soap_end_recv(soap), (soap)->error )
#endif


#endif // SCAPACSERVICE_H