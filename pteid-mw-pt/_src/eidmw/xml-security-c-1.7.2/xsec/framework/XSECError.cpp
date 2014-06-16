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
 * XSECError := General class for handling errors
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

#include <xsec/framework/XSECError.hpp>
#include <xsec/framework/XSECDefs.hpp>
// Real definition of strings

const char * XSECExceptionStrings [] = {

	"No Error",
	"Error allocating memory",
	"No TEXT child found under <DigestValue> element",
	"Unknown Attribute found in DSIG element",
	"Did not find expected DSIG child element",
	"Unknown algorithm found in <Transform> element",
	"Transform input/output mismatch",
	"Referenced ID is not in DOM Document",
	"Unsupported Xpointer expression found",
	"An error occured during an XPath evalaution",
	"An error occured during an XSLT transformation",
	"The called feature is unsupported (general error)",
	"Attempted to load an empty signature node",
	"Attempted to load a non signature DOM Node as a <Signature>",
	"Unknown canonicalization algorithm referenced",
	"Unknown signature and hashing algorithms referenced",
	"Attempted to load an empty X509Data Node",
	"Attempted to load a non X509Data node as a <X509Data>",
	"Error occurred in OpenSSL routine",
	"Error occured when attempting to Verify a Signature",
	"Attempted to load an empty SignedInfo node",
	"Attempted to load a non SignedInfo node as a <SignedInfo>",
	"Expected URI attribute in <REFERENCE> node",
	"A method has been called without load() being called first",
	"An error occurred when interacting with the Crypto Provider",
	"An error occurred during processing of <KeyInfo> list",
	"An error occurred during a signing operation",
	"Attempted to load an empty KeyInfoName node",
	"Attempted to load a non <KeyName> node as a KeyName",
	"Unknown key type found in <KeyValue> element",
	"An error occurred during the creation of a DSIGSignature object",
	"An error occurred when trying to open a URI input stream",
	"An error occurred in the XSEC Provider",
	"CATASTROPHE - An error has been found in internal state",
	"An error occurred in the Envelope Transform handler",
	"A function has been called which is not supported in the compiled library",
	"An error occured in a DSIGTransform holder",
	"An error occured in a safe buffer",
	"An error occurred processing an HTTP request via internal resolver",
	"An error occurred decoding a DSIG encoded Distinguished name",
	"An error occurred during processing of an Encrypted[Data|Key] node",
	"Did not find expected XENC child element",
	"An error occurred processing a CipherData node",
	"An error occurred processing a CipherValue node",
	"An error occurred in the XSECCipher processor",
	"An error occurred in the Algorithm Mapper",
	"An error occurred processing an EncryptionMethod node",
	"An error occurred processing a CipherReference node",
	"An error occurred processing an Object node",
	"An error occurred in XKMS processing",
	"An expected XKMS DOM node or attribute was not found",
	"An error occurred processing a MessageAbstractType object",
	"An error occurred processing a RequestAbstractType object",
	"An error occurred processing a ResultType object",
	"An error occurred processing a Status object",
	"Unknown Error type",

};
//const char ** XSECExceptionStrings = XSECExceptionStringsArray;








