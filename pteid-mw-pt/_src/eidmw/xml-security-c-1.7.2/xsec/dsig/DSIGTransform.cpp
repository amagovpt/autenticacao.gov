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
 * DSIGTransform := Base (virtual) class that defines a DSIG Transform
 *
 * $Id: DSIGTransform.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/dsig/DSIGTransform.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/framework/XSECEnv.hpp>

XERCES_CPP_NAMESPACE_USE

DOMElement * DSIGTransform::createTransformNode() {

	// Create a transform node (and transforms if they do not exist)

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());

	return ret;

}



