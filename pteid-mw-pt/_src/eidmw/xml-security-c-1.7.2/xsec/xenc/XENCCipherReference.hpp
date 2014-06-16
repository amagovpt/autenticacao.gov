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
 * XENCCipherReference := Interface definition for CipherReference element
 *
 * $Id: XENCCipherReference.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XENCCIPHERREFERENCE_INCLUDE
#define XENCCIPHERREFERENCE_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

class DSIGTransformList;
class DSIGTransformBase64;
class DSIGTransformXPath;
class DSIGTransformXPathFilter;
class DSIGTransformXSL;
class DSIGTransformC14n;

XSEC_DECLARE_XERCES_CLASS(DOMElement);
XSEC_DECLARE_XERCES_CLASS(DOMNode);

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the CipherReference object
 *
 * The \<CipherReference\> element provides the information necessary for
 * an application to find the data being referenced.  Like a Reference in
 * XML-DSIG, the CipherReference starts with a URI that provides the base
 * location of the data in question.
 *
 * A list of transforms may then be provided, which the library will apply
 * to the data found at the provided URI.  It is expected that the result of
 * these transforms will be the raw encrypted octets,
 *
 * The schema for CipherReference is as follows:
 *
 * \verbatim
  <element name='CipherReference' type='xenc:CipherReferenceType'/>
   <complexType name='CipherReferenceType'>
       <sequence>
         <element name='Transforms' type='xenc:TransformsType' minOccurs='0'/>
       </sequence>
       <attribute name='URI' type='anyURI' use='required'/>
   </complexType>

    <complexType name='TransformsType'>
       <sequence>
         <element ref='ds:Transform' maxOccurs='unbounded'/> 
       </sequence>
     </complexType>
\endverbatim
 */


class XENCCipherReference {

	/** @name Constructors and Destructors */
	//@{

protected:

	XENCCipherReference() {};

public:

	virtual ~XENCCipherReference() {};

	/** @name Get Interface Methods */
	//@{

	/**
	 * \brief Obtain the transforms for this CipherReference
	 *
	 * Get the DSIGTransformList object for this CipherReference.  Can be used to
	 * obtain information about the transforms and also change the the transforms
	 */

	virtual DSIGTransformList * getTransforms(void) const = 0;

	/**
	 * \brief Obtain the URI for this CipherReference
	 *
	 * @returns A pointer to the URI string for this CipherReference
	 */

	virtual const XMLCh * getURI (void) const = 0;
	
	/**
	 * \brief Get the DOM Node of this structure
	 *
	 * @returns the DOM Node representing the \<CipherValue\> element
	 */

	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * getElement(void) const = 0;

	//@}

	/** @name Set Interface Methods */
	//@{
	
	/**
	 * \brief Append a Base64 Transform to the Reference.
	 *
	 * @returns The newly created Base64 transform.
	 * @todo Move to DSIGTransformList rather than re-implement in both DSIGReference
	 * and XENCCipherReference
	 */

	virtual DSIGTransformBase64 * appendBase64Transform() = 0;
	
	/**
	 * \brief Append an XPath Transform to the Reference.
	 *
	 * <p> Append an XPath transform.  Namespaces can be added to the 
	 * transform directly using the returned <em>DSIGTransformXPath</em>
	 * structure</p>
	 *
	 * @param expr The XPath expression to be placed in the transform.
	 * @returns The newly created XPath transform
	 * @todo Move to DSIGTransformList rather than re-implement in both DSIGReference
	 * and XENCCipherReference
	 */

	virtual DSIGTransformXPath * appendXPathTransform(const char * expr) = 0;
	
	/**
	 * \brief Append an XPath-Filter2 Transform to the Reference.
	 *
	 * The returned DSIGTransformXPathFilter will have no actual filter
	 * expressions loaded, but calls can be made to
	 * DSIGTransformXPathFilter::appendTransform to add them.
	 *
	 * @returns The newly created XPath Filter transform
	 * @todo Move to DSIGTransformList rather than re-implement in both DSIGReference
	 * and XENCCipherReference
	 */

	virtual DSIGTransformXPathFilter * appendXPathFilterTransform(void) = 0;

	/**
	 * \brief Append an XSLT Transform to the Reference.
	 *
	 * <p>The caller must have already create the stylesheet and turned it into
	 * a DOM structure that is passed in as the stylesheet parameter.</p>
	 *
	 * @param stylesheet The stylesheet DOM structure to be placed in the reference.
	 * @returns The newly create XSLT transform
	 * @todo Move to DSIGTransformList rather than re-implement in both DSIGReference
	 * and XENCCipherReference
	 */

	virtual DSIGTransformXSL * appendXSLTransform(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *stylesheet) = 0;
	
	/**
	 * \brief Append a Canonicalization Transform to the Reference.
	 *
	 * @param cm The type of canonicalisation to be added.
	 * @returns The newly create canonicalisation transform
	 * @todo Move to DSIGTransformList rather than re-implement in both DSIGReference
	 * and XENCCipherReference
	 */

	virtual DSIGTransformC14n * appendCanonicalizationTransform(canonicalizationMethod cm) = 0;

	//@}

private:

	// Unimplemented
	XENCCipherReference(const XENCCipherReference &);
	XENCCipherReference & operator = (const XENCCipherReference &);

};

#endif /* XENCCIPHERREFERENCE_INCLUDE */

