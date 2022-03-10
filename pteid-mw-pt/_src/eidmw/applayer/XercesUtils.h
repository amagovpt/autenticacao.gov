/** 
     applayer internal Xerces/XML utilities for xml-security 2.0 and later
     Required to have available the makeQName() functions
     Starting with libxml-security-c v2.0.0+ makeQName() functions became internal.
*/
#ifndef __APL_XERCESUTIL_H__
#define __APL_XERCESUTIL_H__

#include <xsec/framework/XSECDefs.hpp>

#if _XSEC_VERSION_FULL >= 20000L
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#endif

#include "ByteArray.h"

//XAdES/XML-related utility functions are not exposed in xml-security-c
#if _XSEC_VERSION_FULL >= 20000L

#define XERCES_NS XERCES_CPP_NAMESPACE_QUALIFIER
XERCES_CPP_NAMESPACE_USE

namespace eIDMW
{

	safeBuffer &makeQName(safeBuffer & qname, safeBuffer &prefix, const char * localName);
	safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const char * localName);
	safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const XMLCh * localName);
	void makeHexByte(XMLCh * h, unsigned char b);
	CByteArray *DOMDocumentToByteArray(XERCES_NS DOMDocument *doc);

}

#endif

#endif  // __APL_XERCESUTIL_H__