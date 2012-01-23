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
 * $Id: DSIGXPathHere.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGXPathHere.hpp>

#ifndef XSEC_NO_XPATH

#if defined XSEC_XALAN_REQS_MEMORYMANAGER
// Xalan 1.9 and above
    XALAN_USING_XALAN(XalanCopyConstruct);
#endif

DSIGXPathHere::DSIGXPathHere() {

	XalanHereNode = NULL;

}

DSIGXPathHere::DSIGXPathHere(XalanNode * here) {

	XalanHereNode = here;

}

DSIGXPathHere::~DSIGXPathHere() {}

// These methods are inherited from Function ...

XObjectPtr DSIGXPathHere::execute(
			XPathExecutionContext&	executionContext,
			XalanNode*				context,
			// const XObjectPtr		arg1,
			const Locator*			locator) const {

	// Simple function - simply return the Xalan Node we already have
	
	typedef XPathExecutionContext::BorrowReturnMutableNodeRefList	BorrowReturnMutableNodeRefList;

	// This list will hold the nodes we find.

	BorrowReturnMutableNodeRefList	nl(executionContext);

	nl->addNodeInDocOrder(XalanHereNode, executionContext);

	return executionContext.getXObjectFactory().createNodeSet(nl);
}




#if defined(XSEC_NO_COVARIANT_RETURN_TYPE)
	Function*
#else
	DSIGXPathHere*
#endif
#if defined (XSEC_XALAN_REQS_MEMORYMANAGER)
	DSIGXPathHere::clone(MemoryManagerType& theManager)
#else
	DSIGXPathHere::clone()
#endif
	const {

#if defined (XSEC_XALAN_REQS_MEMORYMANAGER)
		return XalanCopyConstruct(theManager, *this);
#else
		DSIGXPathHere *ret;

		ret = new DSIGXPathHere(*this);
		ret->XalanHereNode = XalanHereNode;
		return ret;
#endif
	}
		
	const XalanDOMString
#if defined XSEC_XALAN_REQS_MEMORYMANAGER
    // We cheat - the memory manager happened at the same time as the string
	&DSIGXPathHere::getError(XalanDOMString& theBuffer)
#else
	DSIGXPathHere::getError() 
#endif
    const {

#if defined XSEC_XALAN_REQS_MEMORYMANAGER
        theBuffer = "The here() function takes no arguments!";
		return theBuffer;
#else
		return StaticStringToDOMString(XALAN_STATIC_UCODE_STRING("The here() function takes no arguments!"));
#endif

	}


#endif /* NO_XPATH */
