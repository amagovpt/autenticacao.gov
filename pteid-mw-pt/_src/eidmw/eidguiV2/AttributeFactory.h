/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef ATTRIBUTE_FACTORY_H
#define ATTRIBUTE_FACTORY_H

#include <string>

// URIs defined in https://www.autenticacao.gov.pt/documents/10179/11463/Manual+de+Integra%C3%A7%C3%A3o+do+Fornecedor+de+Autentica%C3%A7%C3%A3o+v1.4.8/09c23770-05d1-450d-93d6-1b9c97dbe89e
#define ATTR_URI_NIC                "http://interop.gov.pt/MDC/Cidadao/NIC"
#define ATTR_URI_NAME               "http://interop.gov.pt/MDC/Cidadao/NomeProprio"
#define ATTR_URI_SURNAME            "http://interop.gov.pt/MDC/Cidadao/NomeApelido" 
#define ATTR_URI_COMPLETENAME       "http://interop.gov.pt/MDC/Cidadao/NomeCompleto"
//TODO: complete attributes if needed

namespace eIDMW
{
    typedef enum {
        NONE,
        NIC,
        NAME,
        SURNAME,
        COMPLETENAME,
    } CitizenAttribute;

    CitizenAttribute uriToAttribute(std::string uri);
    std::string attributeToUri(CitizenAttribute attribute);
}
#endif