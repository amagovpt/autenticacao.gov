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
 * DSIGXPathHere := Implementation of the "here()" XPath function.
 *
 * $Id: DSIGXPathHere.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef DSIGXPATHHERE_INCLUDE
#define DSIGXPATHHERE_INCLUDE

#include <xsec/framework/XSECDefs.hpp>

#include <xercesc/util/PlatformUtils.hpp>

#ifndef XSEC_NO_XALAN

#if defined(_MSC_VER)
#	pragma warning(disable: 4267)
#endif

#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/XPath/XObjectFactory.hpp>
#include <xalanc/XalanDOM/XalanElement.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XPath/Function.hpp>
#include <xalanc/XPath/XObjectTypeCallback.hpp>
#include <xalanc/XPath/MutableNodeRefList.hpp>
#include <xalanc/XPath/NodeRefListBase.hpp>

#if defined(_MSC_VER)
#	pragma warning(default: 4267)
#endif

// Namespace usage

XALAN_USING_XALAN(Function);
XALAN_USING_XALAN(XalanNode);
XALAN_USING_XALAN(XPathExecutionContext);
XALAN_USING_XALAN(XalanDOMString);
XALAN_USING_XALAN(XObjectPtr);
//XALAN_USING_XALAN(StaticStringToDOMString);

#if defined XSEC_XALAN_REQS_MEMORYMANAGER
// Xalan 1.9 and above
    XALAN_USING_XALAN(MemoryManagerType);
#endif

XSEC_USING_XERCES(Locator);

#endif

#ifndef XSEC_NO_XPATH

// If there is no XPath then let's not even bother with this class.

class DSIGXPathHere : public Function {

private:

	XalanNode * XalanHereNode;

public:

	typedef Function	ParentType;

	DSIGXPathHere();
	DSIGXPathHere(XalanNode *here);

	virtual
	~DSIGXPathHere();

	// These methods are inherited from Function ...

	virtual XObjectPtr
	execute(
			XPathExecutionContext&	executionContext,
			XalanNode*				context,
			//const XObjectPtr		arg1,
			const LocatorType*		locator) const;

#if !defined(XALAN_NO_USING_DECLARATION)
	using ParentType::execute;
#endif

#if defined(XSEC_NO_COVARIANT_RETURN_TYPE)
	virtual Function*
#else
	virtual DSIGXPathHere*
#endif
#if defined (XSEC_XALAN_REQS_MEMORYMANAGER)
	clone(MemoryManagerType& theManager) const;
#else
    clone() const;
#endif

protected:

	const XalanDOMString
#if defined (XSEC_XALAN_REQS_MEMORYMANAGER)
    // We cheat - the memory manager happened at the same time as the string
	&getError(XalanDOMString& theBuffer) const;
#else
    getError() const;
#endif

private:

	// Not implemented...
	DSIGXPathHere&
	operator=(const DSIGXPathHere&);

	bool
	operator==(const DSIGXPathHere&) const;
};

#endif

#endif








