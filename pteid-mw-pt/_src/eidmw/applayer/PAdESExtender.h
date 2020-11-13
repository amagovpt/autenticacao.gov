/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2020 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef __PADES_EXTENDER_H
#define __PADES_EXTENDER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "PDFSignature.h"

/* Forward declaration of ValidationDataElement defined in pteid-poppler. */
class ValidationDataElement;

namespace eIDMW
{

    /* PAdESExtender allows to extend the level of an existing PAdES-B or PAdES-T signed document. 
       This can be done in a incremental way without breaking the existing signature.
       Revocation information and related certificates are added to the /DSS dictionary in Catalog object
       For PAdES-LTA level an additional DocumentTimestamp is added at the end
    */
    class PAdESExtender
    {
    public:
        EIDMW_APL_API PAdESExtender(PDFSignature *signedPdfDoc);

        EIDMW_APL_API bool addT();
        EIDMW_APL_API bool addLT();
        EIDMW_APL_API bool addLTA();

    private:
        /* addValidationElement method takes a VDE and adds a dynamically allocated copy of it to m_validationData, if not already present.
        It returns a pointer to the element in m_validationData */
        ValidationDataElement* addValidationElement(ValidationDataElement &elem);

        bool findIssuerInEidStore(APL_CryptoFwkPteid * cryptoFwk, CByteArray &certif_ba, CByteArray &issuer_ba);
        bool addOCSPCertToValidationData(CByteArray &ocsp_response_ba, CByteArray &out_ocsp_cert);
        bool isOCSPSigningCert(CByteArray &cert);
        bool addCRLRevocationInfo(CByteArray & cert, std::unordered_set<std::string> vri_keys);

        PDFSignature *m_signedPdfDoc;
        std::vector<ValidationDataElement*> m_validationData;
        std::unordered_map<unsigned long, ValidationDataElement *> m_certsInDoc; // used to avoid adding repeated certificates

        bool m_calledFromLtaMethod;
    };
}
#endif