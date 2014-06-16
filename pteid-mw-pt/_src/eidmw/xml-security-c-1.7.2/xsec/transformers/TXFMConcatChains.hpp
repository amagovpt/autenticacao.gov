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
 * TXFMConcatChain := Takes multiple input chains and then provides
 *					  BYTE_STREAM output for each chain in order.
 *
 * $Id: TXFMConcatChains.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/transformers/TXFMBase.hpp>

class TXFMChain;

/**
 * \brief Transformer to handle concatination of transform chains
 * @ingroup internal
 */

class DSIG_EXPORT TXFMConcatChains : public TXFMBase {

	#if defined(XSEC_NO_NAMESPACES)
	typedef vector<TXFMChain *>			TXFMChainVectorType
#else
	typedef std::vector<TXFMChain *>	TXFMChainVectorType;
#endif

#if defined(XSEC_SIZE_T_IN_NAMESPACE_STD)
	typedef std::size_t		size_type;
#else
	typedef size_t			size_type;
#endif

public:

	TXFMConcatChains(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc);
	~TXFMConcatChains();

	// Methods to set the inputs.  For the concat class, this will
	// append the input to the last appended chain

	void setInput(TXFMBase *newInput);
	void setInput(TXFMChain *newInputChain);
	
	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual TXFMBase::nodeType getNodeType(void);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private:
	TXFMConcatChains();

	TXFMChainVectorType			m_chains;
	TXFMBase					* mp_currentTxfm;
	size_type					m_currentChain;
	bool						m_complete;
};
