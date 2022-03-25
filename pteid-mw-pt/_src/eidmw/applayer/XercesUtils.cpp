/** 
   pteid-mw internal Xerces/XML utilities for xml-security 2.0 and later
*/

// Xerces
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/Janitor.hpp>

// XML-Security-C (XSEC)
#include <xsec/framework/XSECDefs.hpp>
#if _XSEC_VERSION_FULL < 20000L
#include <xsec/utils/XSECDOMUtils.hpp>
#else
#include <xsec/utils/XSECPlatformUtils.hpp>
#endif

#include <xsec/framework/XSECProvider.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>

#include "eidErrors.h"
#include "Log.h"
#include "MWException.h"
#include "XercesUtils.h"


XERCES_CPP_NAMESPACE_USE

#if _XSEC_VERSION_FULL >= 20000L
namespace eIDMW
{


// Utility functions used in XadesSignature needed to support xml-security >= 2.0.0
// The various makeQName functions became internal to xml-security

    // --------------------------------------------------------------------------------

    //           Make a QName
    // --------------------------------------------------------------------------------

    safeBuffer &makeQName(safeBuffer & qname, safeBuffer &prefix, const char * localName) {

        if (prefix[0] == '\0') {
            qname = localName;
        }
        else {
            qname = prefix;
            qname.sbStrcatIn(":");
            qname.sbStrcatIn(localName);
        }

        return qname;

    }

    safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const char * localName) {

        if (prefix == NULL || prefix[0] == 0) {
            qname.sbTranscodeIn(localName);
        }
        else {
            qname.sbXMLChIn(prefix);
            qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
            qname.sbXMLChCat(localName);    // Will transcode
        }

        return qname;
    }

    safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const XMLCh * localName) {

        if (prefix == NULL || prefix[0] == 0) {
            qname.sbXMLChIn(localName);
        }
        else {
            qname.sbXMLChIn(prefix);
            qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
            qname.sbXMLChCat(localName);
        }

        return qname;
    }

    void makeHexByte(XMLCh * h, unsigned char b) {

        unsigned char toConvert =  (b & 0xf0);
        toConvert = (toConvert >> 4);

        if (toConvert < 10)
            h[0] = chDigit_0 + toConvert;
        else
            h[0] = chLatin_a + toConvert - 10;

        toConvert =  (b & 0xf);

        if (toConvert < 10)
            h[1] = chDigit_0 + toConvert;
        else
            h[1] = chLatin_a + toConvert - 10;

    }

    CByteArray *DOMDocumentToByteArray(XERCES_NS DOMDocument * doc) {
        CByteArray * ba_out = new CByteArray();
        XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);

        // construct the DOMWriter
        DOMLSSerializer *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
        DOMLSOutput *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();

         // construct the MemBufFormatTarget
        MemBufFormatTarget * myFormatTarget = new MemBufFormatTarget();
        theOutputDesc->setByteStream(myFormatTarget);
        theOutputDesc->setEncoding(XMLString::transcode("UTF-8"));
        theSerializer->write(doc, theOutputDesc);

        // serialize the DOMNode to a UTF-8 string
        const XMLByte* utf8_string = myFormatTarget->getRawBuffer();
        XMLSize_t size = myFormatTarget->getLen();
        MWLOG(LEV_DEBUG, MOD_APL, L"DOMDocumentToByteArray: Returning XML byte array, size=%d", size);

        ba_out->Append((const unsigned char *)utf8_string, size);
		ba_out->Append(0x0);

        delete myFormatTarget;
        return ba_out;
    }

} // namespace eIDMW
#endif