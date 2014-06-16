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
 * AnonymousResolver := Class to resolve the IAIK anonymous sample reference
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: AnonymousResolver.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/framework/XSECURIResolver.hpp>


#define anonURI "../digestInputs/anonymousReferenceSignature.firstReference.txt"


// ----------------------------------------------------------------------------
//           AnonymousResolver
// ----------------------------------------------------------------------------

/*
 * The anonymous resolver is a very simple resolver used for the IAIK 
 * anonymousReferenceSignature.xml interop test example.
 * It simply takes an anonymous (NULL) uri reference and links to the 
 * relevant file in the data suite
 */

class AnonymousResolver : public XSECURIResolver {

public:

	AnonymousResolver() {mp_baseURI = NULL;}
	virtual ~AnonymousResolver();

	// Interface method

	virtual XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream * resolveURI(const XMLCh * uri);
 
	// Interface method

	virtual XSECURIResolver * clone(void);

	// Extra methods

	void setBaseURI(const XMLCh * uri);
private:
	XMLCh * mp_baseURI;
};

