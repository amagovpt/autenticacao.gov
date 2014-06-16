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
 * TXFMOutputFile := Transform that outputs the byte stream to a file without changing
 *							  the actual data in any way
 *
 * $Id: TXFMOutputFile.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef TXFMOUTPUTFILE_INCLUDE
#define TXFMOUTPUTFILE_INCLUDE

#include <xsec/transformers/TXFMBase.hpp>

#include <fstream>

using std::ofstream;

/**
 * \brief Transformer to handle file output transforms
 * @ingroup internal
 *
 * This transformer simply reads from the input transform 
 * and passes the bytes to the transformer that reads from it.
 *
 * On the way through, the writes are written to an output
 * stream (a file).  This is primarily a debugging tool.
 */


class DSIG_EXPORT TXFMOutputFile : public TXFMBase {

private:

	ofstream f;

public:

	TXFMOutputFile(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc) : 
		TXFMBase(doc) {input = NULL;}
	~TXFMOutputFile();

	// Methods to set the inputs

	void setInput(TXFMBase *newInput);

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void);
	virtual TXFMBase::ioType getOutputType(void);
	virtual TXFMBase::nodeType getNodeType(void);

	// Set output file

	bool setFile(char * const fileName);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *getDocument();
	virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *getFragmentNode();
	virtual const XMLCh * getFragmentId();
	
private:
	TXFMOutputFile();
};


#endif /* #define TXFMOUTPUTFILE_INCLUDE */
