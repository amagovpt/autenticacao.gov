/*-****************************************************************************

 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#ifndef __PADES_EXTENDER_H
#define __PADES_EXTENDER_H

#include "PDFSignature.h"

namespace eIDMW
{
    /* PAdESExtender allows to extend the level of an existing PAdES signed document. 
    This can be done in a incremental way without breaking the existing signature. */
    class PAdESExtender
    {
    public:
        EIDMW_APL_API PAdESExtender(PDFSignature *signedPdfDoc);

        EIDMW_APL_API bool addT();
        EIDMW_APL_API bool addLT();
        EIDMW_APL_API bool addLTA();

    private:
        PDFSignature *m_signedPdfDoc;
    };
}
#endif