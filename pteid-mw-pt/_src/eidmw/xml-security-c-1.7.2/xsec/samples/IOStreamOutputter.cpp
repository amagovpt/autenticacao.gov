/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * IOStreamOutputer.cpp := Used by all samples that need to output a DOM document
 *							to an IOStream
 *
 *							Very ugly - should really be a class.  The original
 *							Xerces examples had this as part of a single CPP file for
 *							One overall example - much nicer.
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#include "IOStreamOutputter.hpp"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <memory.h>

// Uplift everything to the Xerces name space
XERCES_CPP_NAMESPACE_USE


// --------------------------------------------------------------------------------
//           Much code taken from the DOMPrint Xerces example
// --------------------------------------------------------------------------------

static XMLFormatter*            gFormatter             = 0;
static XMLCh*                   gEncodingName          = 0;
static XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;




static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};

static const XMLCh  gStartCDATA[] =
{
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};



// ---------------------------------------------------------------------------
//  Local classes
// ---------------------------------------------------------------------------

class DOMPrintFormatTarget : public XMLFormatTarget
{
public:
    DOMPrintFormatTarget()  {};
    ~DOMPrintFormatTarget() {};

    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const   xsecsize_t    count,
                            XMLFormatter * const formatter)
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        cout.write((char *) toWrite, count);
    };

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMPrintFormatTarget(const DOMPrintFormatTarget& other);
    void operator=(const DOMPrintFormatTarget& rhs);
};


// ---------------------------------------------------------------------------
//  ostream << DOMNode
//
//  Stream out a DOM node, and, recursively, all of its children. This
//  function is the heart of writing a DOM tree out as XML source. Give it
//  a document node and it will do the whole thing.
// ---------------------------------------------------------------------------
ostream& operator<<(ostream& target, DOMNode* toWrite)
{
    // Get the name and value out for convenience
    const XMLCh*   nodeName = toWrite->getNodeName();
    const XMLCh*   nodeValue = toWrite->getNodeValue();
    xsecsize_t lent = XMLString::stringLen(nodeValue);

    switch (toWrite->getNodeType())
    {
        case DOMNode::TEXT_NODE:
        {
            gFormatter->formatBuf(nodeValue,
                                  lent, XMLFormatter::CharEscapes);
            break;
        }


        case DOMNode::PROCESSING_INSTRUCTION_NODE :
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartPI  << nodeName;
            if (lent > 0)
            {
                *gFormatter << chSpace << nodeValue;
            }
            *gFormatter << XMLFormatter::NoEscapes << gEndPI;
            break;
        }


        case DOMNode::DOCUMENT_NODE :
        {

            DOMNode *child = toWrite->getFirstChild();
            while( child != 0)
            {
                target << child;
                // add linefeed in requested output encoding
                *gFormatter << chLF;
                target << flush;
                child = child->getNextSibling();
            }
            break;
        }


        case DOMNode::ELEMENT_NODE :
        {
            // The name has to be representable without any escapes
            *gFormatter  << XMLFormatter::NoEscapes
                         << chOpenAngle << nodeName;

            // Output the element start tag.

            // Output any attributes on this element
            DOMNamedNodeMap *attributes = toWrite->getAttributes();
            XMLSize_t attrCount = attributes->getLength();
            for (XMLSize_t i = 0; i < attrCount; i++)
            {
                DOMNode  *attribute = attributes->item(i);

                //
                //  Again the name has to be completely representable. But the
                //  attribute can have refs and requires the attribute style
                //  escaping.
                //
                *gFormatter  << XMLFormatter::NoEscapes
                             << chSpace << attribute->getNodeName()
                             << chEqual << chDoubleQuote
                             << XMLFormatter::AttrEscapes
                             << attribute->getNodeValue()
                             << XMLFormatter::NoEscapes
                             << chDoubleQuote;
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOMNode *child = toWrite->getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;

                while( child != 0)
                {
                    target << child;
                    child = child->getNextSibling();
                }

                //
                // Done with children.  Output the end tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << gEndElement
                            << nodeName << chCloseAngle;
            }
            else
            {
                //
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
            }
            break;
        }


        case DOMNode::ENTITY_REFERENCE_NODE:
            {
                //DOMNode *child;
#if 0
                for (child = toWrite.getFirstChild();
                child != 0;
                child = child.getNextSibling())
                {
                    target << child;
                }
#else
                //
                // Instead of printing the refernece tree
                // we'd output the actual text as it appeared in the xml file.
                // This would be the case when -e option was chosen
                //
                    *gFormatter << XMLFormatter::NoEscapes << chAmpersand
                        << nodeName << chSemiColon;
#endif
                break;
            }


        case DOMNode::CDATA_SECTION_NODE:
            {
            *gFormatter << XMLFormatter::NoEscapes << gStartCDATA
                        << nodeValue << gEndCDATA;
            break;
        }


        case DOMNode::COMMENT_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartComment
                        << nodeValue << gEndComment;
            break;
        }


        case DOMNode::DOCUMENT_TYPE_NODE:
        {
            DOMDocumentType *doctype = (DOMDocumentType *)toWrite;;

            *gFormatter << XMLFormatter::NoEscapes  << gStartDoctype
                        << nodeName;

            const XMLCh* id = doctype->getPublicId();
            if (id != 0)
            {
                *gFormatter << XMLFormatter::NoEscapes << chSpace << gPublic
                    << id << chDoubleQuote;
                id = doctype->getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace
                       << chDoubleQuote << id << chDoubleQuote;
                }
            }
            else
            {
                id = doctype->getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace << gSystem
                        << id << chDoubleQuote;
                }
            }

            id = doctype->getInternalSubset();
            if (id !=0)
                *gFormatter << XMLFormatter::NoEscapes << chOpenSquare
                            << id << chCloseSquare;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;
            break;
        }


        case DOMNode::ENTITY_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartEntity
                        << nodeName;

            const XMLCh * id = ((DOMEntity *)toWrite)->getPublicId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gPublic
                            << id << chDoubleQuote;

            id = ((DOMEntity *)toWrite)->getSystemId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gSystem
                            << id << chDoubleQuote;

            id = ((DOMEntity *)toWrite)->getNotationName();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gNotation
                            << id << chDoubleQuote;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle << chLF;

            break;
        }

/*
        case DOMNode::NOTATION_NODE:
        {
            const XMLCh *  str;

            *gFormatter << gXMLDecl1 << ((DOMXMLDecl *)toWrite)->getVersion();

            *gFormatter << gXMLDecl2 << gEncodingName;

            str = ((DOMXMLDecl *)toWrite)->getStandalone();
            if (str != 0)
                *gFormatter << gXMLDecl3 << str;

            *gFormatter << gXMLDecl4;

            break;
        }

*/
        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite->getNodeType() << endl;
    }
    return target;
}



// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------
/*ostream& operator<< (ostream& target, const DOMString& s)
{
    char *p = s.transcode();
    target << p;
    delete [] p;
    return target;
}


XMLFormatter& operator<< (XMLFormatter& strm, const DOMString& s)
{
    unsigned int lent = s.length();

	if (lent <= 0)
		return strm;

    XMLCh*  buf = new XMLCh[lent + 1];
    XMLString::copyNString(buf, s.rawBuffer(), lent);
    buf[lent] = 0;
    strm << buf;
    delete [] buf;
    return strm;
}*/

// --------------------------------------------------------------------------------
//           End of outputter
// --------------------------------------------------------------------------------

class DOMMemFormatTarget : public XMLFormatTarget
{
public:
    
	unsigned char * buffer;		// Buffer to write to

	DOMMemFormatTarget()  {};
    ~DOMMemFormatTarget() {};

	void setBuffer (unsigned char * toSet) {buffer = toSet;};


    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const   unsigned int    count,
                            XMLFormatter * const formatter)
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        memcpy(buffer, (char *) toWrite, (int) count);
		buffer[count] = '\0';
    };

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMMemFormatTarget(const DOMMemFormatTarget& other);
    void operator=(const DOMMemFormatTarget& rhs);

	
};

// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------


DOMPrintFormatTarget *DOMtarget;
DOMMemFormatTarget *MEMtarget;
XMLFormatter *formatter, *MEMformatter;
unsigned char *charBuffer;

#if 0


// --------------------------------------------------------------------------------
//           Much code taken from the DOMPrint Xerces example
// --------------------------------------------------------------------------------

static XMLFormatter*            gFormatter             = 0;
static XMLCh*                   gEncodingName          = 0;
static XMLFormatter::UnRepFlags gUnRepFlags            = XMLFormatter::UnRep_CharRef;




static const XMLCh  gEndElement[] = { chOpenAngle, chForwardSlash, chNull };
static const XMLCh  gEndPI[] = { chQuestion, chCloseAngle, chNull};
static const XMLCh  gStartPI[] = { chOpenAngle, chQuestion, chNull };
static const XMLCh  gXMLDecl1[] =
{
        chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l
    ,   chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i
    ,   chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl2[] =
{
        chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c
    ,   chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual
    ,   chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl3[] =
{
        chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a
    ,   chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o
    ,   chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
static const XMLCh  gXMLDecl4[] =
{
        chDoubleQuote, chQuestion, chCloseAngle
    ,   chLF, chNull
};

static const XMLCh  gStartCDATA[] =
{
        chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
        chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};

static const XMLCh  gEndCDATA[] =
{
    chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
static const XMLCh  gStartComment[] =
{
    chOpenAngle, chBang, chDash, chDash, chNull
};

static const XMLCh  gEndComment[] =
{
    chDash, chDash, chCloseAngle, chNull
};

static const XMLCh  gStartDoctype[] =
{
    chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
    chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
static const XMLCh  gPublic[] =
{
    chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
    chLatin_C, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gSystem[] =
{
    chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
    chLatin_M, chSpace, chDoubleQuote, chNull
};
static const XMLCh  gStartEntity[] =
{
    chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
    chLatin_T, chLatin_Y, chSpace, chNull
};
static const XMLCh  gNotation[] =
{
    chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
    chSpace, chDoubleQuote, chNull
};



// ---------------------------------------------------------------------------
//  Local classes
// ---------------------------------------------------------------------------

class DOMPrintFormatTarget : public XMLFormatTarget
{
public:
    DOMPrintFormatTarget()  {};
    ~DOMPrintFormatTarget() {};

    // -----------------------------------------------------------------------
    //  Implementations of the format target interface
    // -----------------------------------------------------------------------

    void writeChars(const   XMLByte* const  toWrite,
                    const   unsigned int    count,
                            XMLFormatter * const formatter)
    {
        // Surprisingly, Solaris was the only platform on which
        // required the char* cast to print out the string correctly.
        // Without the cast, it was printing the pointer value in hex.
        // Quite annoying, considering every other platform printed
        // the string with the explicit cast to char* below.
        cout.write((char *) toWrite, (int) count);
    };

private:
    // -----------------------------------------------------------------------
    //  Unimplemented methods.
    // -----------------------------------------------------------------------
    DOMPrintFormatTarget(const DOMPrintFormatTarget& other);
    void operator=(const DOMPrintFormatTarget& rhs);
};


// ---------------------------------------------------------------------------
//  ostream << DOM_Node
//
//  Stream out a DOM node, and, recursively, all of its children. This
//  function is the heart of writing a DOM tree out as XML source. Give it
//  a document node and it will do the whole thing.
// ---------------------------------------------------------------------------
ostream& operator<<(ostream& target, DOM_Node& toWrite)
{
    // Get the name and value out for convenience
    DOMString   nodeName = toWrite.getNodeName();
    DOMString   nodeValue = toWrite.getNodeValue();
    unsigned long lent = nodeValue.length();

    switch (toWrite.getNodeType())
    {
        case DOM_Node::TEXT_NODE:
        {
            gFormatter->formatBuf(nodeValue.rawBuffer(),
                                  lent, XMLFormatter::CharEscapes);
            break;
        }


        case DOM_Node::PROCESSING_INSTRUCTION_NODE :
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartPI  << nodeName;
            if (lent > 0)
            {
                *gFormatter << chSpace << nodeValue;
            }
            *gFormatter << XMLFormatter::NoEscapes << gEndPI;
            break;
        }


        case DOM_Node::DOCUMENT_NODE :
        {

            DOM_Node child = toWrite.getFirstChild();
            while( child != 0)
            {
                target << child;
                // add linefeed in requested output encoding
                *gFormatter << chLF;
                target << flush;
                child = child.getNextSibling();
            }
            break;
        }


        case DOM_Node::ELEMENT_NODE :
        {
            // The name has to be representable without any escapes
            *gFormatter  << XMLFormatter::NoEscapes
                         << chOpenAngle << nodeName;

            // Output the element start tag.

            // Output any attributes on this element
            DOM_NamedNodeMap attributes = toWrite.getAttributes();
            int attrCount = attributes.getLength();
            for (int i = 0; i < attrCount; i++)
            {
                DOM_Node  attribute = attributes.item(i);

                //
                //  Again the name has to be completely representable. But the
                //  attribute can have refs and requires the attribute style
                //  escaping.
                //
                *gFormatter  << XMLFormatter::NoEscapes
                             << chSpace << attribute.getNodeName()
                             << chEqual << chDoubleQuote
                             << XMLFormatter::AttrEscapes
                             << attribute.getNodeValue()
                             << XMLFormatter::NoEscapes
                             << chDoubleQuote;
            }

            //
            //  Test for the presence of children, which includes both
            //  text content and nested elements.
            //
            DOM_Node child = toWrite.getFirstChild();
            if (child != 0)
            {
                // There are children. Close start-tag, and output children.
                // No escapes are legal here
                *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;

                while( child != 0)
                {
                    target << child;
                    child = child.getNextSibling();
                }

                //
                // Done with children.  Output the end tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << gEndElement
                            << nodeName << chCloseAngle;
            }
            else
            {
                //
                //  There were no children. Output the short form close of
                //  the element start tag, making it an empty-element tag.
                //
                *gFormatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
            }
            break;
        }


        case DOM_Node::ENTITY_REFERENCE_NODE:
            {
                DOM_Node child;
#if 0
                for (child = toWrite.getFirstChild();
                child != 0;
                child = child.getNextSibling())
                {
                    target << child;
                }
#else
                //
                // Instead of printing the refernece tree
                // we'd output the actual text as it appeared in the xml file.
                // This would be the case when -e option was chosen
                //
                    *gFormatter << XMLFormatter::NoEscapes << chAmpersand
                        << nodeName << chSemiColon;
#endif
                break;
            }


        case DOM_Node::CDATA_SECTION_NODE:
            {
            *gFormatter << XMLFormatter::NoEscapes << gStartCDATA
                        << nodeValue << gEndCDATA;
            break;
        }


        case DOM_Node::COMMENT_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartComment
                        << nodeValue << gEndComment;
            break;
        }


        case DOM_Node::DOCUMENT_TYPE_NODE:
        {
            DOM_DocumentType doctype = (DOM_DocumentType &)toWrite;;

            *gFormatter << XMLFormatter::NoEscapes  << gStartDoctype
                        << nodeName;

            DOMString id = doctype.getPublicId();
            if (id != 0)
            {
                *gFormatter << XMLFormatter::NoEscapes << chSpace << gPublic
                    << id << chDoubleQuote;
                id = doctype.getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace
                       << chDoubleQuote << id << chDoubleQuote;
                }
            }
            else
            {
                id = doctype.getSystemId();
                if (id != 0)
                {
                    *gFormatter << XMLFormatter::NoEscapes << chSpace << gSystem
                        << id << chDoubleQuote;
                }
            }

            id = doctype.getInternalSubset();
            if (id !=0)
                *gFormatter << XMLFormatter::NoEscapes << chOpenSquare
                            << id << chCloseSquare;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle;
            break;
        }


        case DOM_Node::ENTITY_NODE:
        {
            *gFormatter << XMLFormatter::NoEscapes << gStartEntity
                        << nodeName;

            DOMString id = ((DOM_Entity &)toWrite).getPublicId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gPublic
                            << id << chDoubleQuote;

            id = ((DOM_Entity &)toWrite).getSystemId();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gSystem
                            << id << chDoubleQuote;

            id = ((DOM_Entity &)toWrite).getNotationName();
            if (id != 0)
                *gFormatter << XMLFormatter::NoEscapes << gNotation
                            << id << chDoubleQuote;

            *gFormatter << XMLFormatter::NoEscapes << chCloseAngle << chLF;

            break;
        }


        case DOM_Node::XML_DECL_NODE:
        {
            DOMString  str;

            *gFormatter << gXMLDecl1 << ((DOM_XMLDecl &)toWrite).getVersion();

            *gFormatter << gXMLDecl2 << gEncodingName;

            str = ((DOM_XMLDecl &)toWrite).getStandalone();
            if (str != 0)
                *gFormatter << gXMLDecl3 << str;

            *gFormatter << gXMLDecl4;

            break;
        }


        default:
            cerr << "Unrecognized node type = "
                 << (long)toWrite.getNodeType() << endl;
    }
    return target;
}



// ---------------------------------------------------------------------------
//  ostream << DOMString
//
//  Stream out a DOM string. Doing this requires that we first transcode
//  to char * form in the default code page for the system
// ---------------------------------------------------------------------------
ostream& operator<< (ostream& target, const DOMString& s)
{
    char *p = s.transcode();
    target << p;
    XSEC_RELEASE_XMLCH(p);
    return target;
}


XMLFormatter& operator<< (XMLFormatter& strm, const DOMString& s)
{
    unsigned int lent = s.length();

	if (lent <= 0)
		return strm;

    XMLCh*  buf = new XMLCh[lent + 1];
    XMLString::copyNString(buf, s.rawBuffer(), lent);
    buf[lent] = 0;
    strm << buf;
    delete [] buf;
    return strm;
}

#endif

void docSetup(DOMDocument *doc) {

	// Print out the doc

	DOMPrintFormatTarget* formatTarget = new DOMPrintFormatTarget();
	
    const XMLCh* encNameStr = XMLString::transcode("UTF-8");
    DOMNode *aNode = doc->getFirstChild();
    if (aNode->getNodeType() == DOMNode::ENTITY_NODE)
    {
        const XMLCh* aStr = ((DOMEntity *)aNode)->
#if defined XSEC_XERCES_DOMENTITYINPUTENCODING
            getInputEncoding();
#else
            getEncoding();
#endif

        if (!strEquals(aStr, ""))
        {
            encNameStr = aStr;
        }
    }
    xsecsize_t lent = XMLString::stringLen(encNameStr);
    gEncodingName = new XMLCh[lent + 1];
    XMLString::copyNString(gEncodingName, encNameStr, lent);
    gEncodingName[lent] = 0;

	
	
#if defined(XSEC_XERCES_FORMATTER_REQUIRES_VERSION)
	gFormatter = new XMLFormatter("UTF-8", 0, formatTarget,
                                          XMLFormatter::NoEscapes, gUnRepFlags);
#else
	gFormatter = new XMLFormatter("UTF-8", formatTarget,
                                          XMLFormatter::NoEscapes, gUnRepFlags);
#endif
}

// --------------------------------------------------------------------------------
//           End of outputter
// --------------------------------------------------------------------------------
