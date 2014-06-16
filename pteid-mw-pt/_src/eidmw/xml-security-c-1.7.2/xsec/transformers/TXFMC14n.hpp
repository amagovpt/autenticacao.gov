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
 * TXFMBaseC14n := Class that performs C14n canonicalisation
 *
 * $Id: TXFMC14n.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECNameSpaceExpander.hpp>

/**
 * \brief Transformer to handle canonicalisation transforms
 * @ingroup internal
 */

class DSIG_EXPORT TXFMC14n : public TXFMBase {

private:

	XSECC14n20010315		* mp_c14n;			// The actual canonicaliser

public:

	TXFMC14n(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	~TXFMC14n();

	// Methods to set the inputs

	void setInput(TXFMBase *newInput);

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual TXFMBase::nodeType getNodeType(void);

	// Overrid comment acivation
	virtual void activateComments(void);

	// Set exclusive
	virtual void setExclusive();
	virtual void setExclusive(safeBuffer & NSList);

	// Set inclusive 1.1
	virtual void setInclusive11();

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();

private:
	TXFMC14n();
};

