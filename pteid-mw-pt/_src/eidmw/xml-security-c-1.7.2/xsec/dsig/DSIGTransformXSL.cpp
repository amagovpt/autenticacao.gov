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
 * DSIGTransformXSL := Class that Handles DSIG XSLT Transforms
 *
 * $Id: DSIGTransformXSL.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/dsig/DSIGTransformXSL.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/transformers/TXFMXSL.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>

XERCES_CPP_NAMESPACE_USE

class XSECDomToSafeBuffer {
public:
    XSECDomToSafeBuffer(DOMNode *node);
    virtual ~XSECDomToSafeBuffer() {}

    operator const safeBuffer&() const {
        return m_buffer;
    }
private:
    safeBuffer m_buffer;
};

XSECDomToSafeBuffer::XSECDomToSafeBuffer(DOMNode* node)
{
    static const XMLCh _LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementationLS* impl = DOMImplementationRegistry::getDOMImplementation(_LS);

    MemBufFormatTarget* target = new MemBufFormatTarget;
    Janitor<MemBufFormatTarget> j_target(target);

#if defined (XSEC_XERCES_DOMLSSERIALIZER)
    // DOM L3 version as per Xerces 3.0 API
    DOMLSSerializer* theSerializer = impl->createLSSerializer();
    Janitor<DOMLSSerializer> j_theSerializer(theSerializer);

    DOMLSOutput *theOutput = impl->createLSOutput();
    Janitor<DOMLSOutput> j_theOutput(theOutput);
    theOutput->setByteStream(target);
#else
    DOMWriter* theSerializer = impl->createDOMWriter();
    Janitor<DOMWriter> j_theSerializer(theSerializer);
#endif

    try
    {
#if defined (XSEC_XERCES_DOMLSSERIALIZER)
        theSerializer->write(node, theOutput);
#else
        theSerializer->writeNode(target, *node);
#endif
        m_buffer.sbMemcpyIn(0, target->getRawBuffer(), target->getLen());
    }
    catch(const XMLException&)
    {
        throw XSECException(XSECException::UnknownError);
    }
    catch(const DOMException&)
    {
        throw XSECException(XSECException::UnknownError);
    }
}

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGTransformXSL::DSIGTransformXSL(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node),
mp_stylesheetNode(NULL) {};


DSIGTransformXSL::DSIGTransformXSL(const XSECEnv * env) :
DSIGTransform(env),
mp_stylesheetNode(NULL) {};
		  

DSIGTransformXSL::~DSIGTransformXSL() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------


transformType DSIGTransformXSL::getTransformType() {

	return TRANSFORM_XSLT;

}


void DSIGTransformXSL::appendTransformer(TXFMChain * input) {


#ifdef XSEC_NO_XSLT

	throw XSECException(XSECException::UnsupportedFunction,
		"XSLT Transforms not supported in this compilation of the library");
#else

	if (mp_stylesheetNode == 0)
		throw XSECException(XSECException::XSLError, "Style Sheet not found for XSL Transform");


	TXFMBase * nextInput;

	// XSLT Transform - requires a byte stream input
	
	if (input->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {
		
		// Use c14n to translate to BYTES
		
		XSECnew(nextInput, TXFMC14n(mp_txfmNode->getOwnerDocument()));
		input->appendTxfm(nextInput);
	}

	TXFMXSL * x;
	
	// Create the XSLT transform
	XSECnew(x, TXFMXSL(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(x);
	
	// Patch to avoid c14n of stylesheet
    XSECDomToSafeBuffer sbStyleSheet(mp_stylesheetNode);
    x->evaluateStyleSheet(sbStyleSheet);
#endif /* NO_XSLT */

}


DOMElement * DSIGTransformXSL::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIXSLT);

	mp_txfmNode = ret;
	mp_stylesheetNode = NULL;

	return ret;

}

void DSIGTransformXSL::load(void) {

	// find the style sheet
	mp_stylesheetNode = mp_txfmNode->getFirstChild();
	while (mp_stylesheetNode != 0 && 
		mp_stylesheetNode->getNodeType() != DOMNode::ELEMENT_NODE && !strEquals(mp_stylesheetNode->getNodeName(), "xsl:stylesheet"))
		mp_stylesheetNode = mp_stylesheetNode->getNextSibling();

	if (mp_stylesheetNode == 0)
		throw XSECException(XSECException::XSLError, "Style Sheet not found for XSL Transform");


}
// --------------------------------------------------------------------------------
//           XSLT Specific Methods
// --------------------------------------------------------------------------------

DOMNode * DSIGTransformXSL::setStylesheet(DOMNode * stylesheet) {

	DOMNode * ret = mp_stylesheetNode;

	if (mp_stylesheetNode) {
	    if (stylesheet)
	        mp_txfmNode->insertBefore(stylesheet, mp_stylesheetNode);
		mp_txfmNode->removeChild(mp_stylesheetNode);
	}
	else if (stylesheet) {
	    mp_txfmNode->appendChild(stylesheet);
	}

	mp_stylesheetNode = stylesheet;

	return ret;

}

DOMNode * DSIGTransformXSL::getStylesheet(void) {

	return mp_stylesheetNode;

}
