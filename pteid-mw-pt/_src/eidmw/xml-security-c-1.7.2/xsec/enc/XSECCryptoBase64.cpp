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
 * XSECCryptoBase64 := Implementation of Base64 utility fns
 *
 * $Id: XSECCryptoBase64.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoBase64.hpp>
#include <xsec/framework/XSECError.hpp>

#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Clean a buffer of Base64 characters
// --------------------------------------------------------------------------------


char * XSECCryptoBase64::cleanBuffer(const char * buffer, 
									  unsigned int bufLen, 
									  unsigned int &retBufLen) {

	if (bufLen == 0)
		bufLen = XMLString::stringLen(buffer);

	char * res;

	XSECnew(res, char[bufLen + (bufLen / 72) + 3]);

	unsigned int j = 0;
	unsigned int k = 0;
	for (unsigned int i = 0; i < bufLen; ++i) {
		res[j++] = buffer[i];
		if (buffer[i] == '\n' || buffer[i] == '\r')
			k = 0;
		else {
			if (++k >= 72) {
				res[j++] = '\n';
				k = 0;
			}
		}
	}

	res[j] = '\0';
	retBufLen = j;

	return res;

}




