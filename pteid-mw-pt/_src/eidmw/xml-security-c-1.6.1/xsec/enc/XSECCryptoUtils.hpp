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
 * XSECCryptoUtils:= Helper crypo utilities that make life easier
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoUtils.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XSECCRYPTOUTILS_INCLUDE
#define XSECCRYPTOUTILS_INCLUDE

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/dsig/DSIGConstants.hpp>

/**
 * \brief Helper utilities for crypto.
 * @ingroup crypto
 */

// --------------------------------------------------------------------------------
//           XKMS Limited-Use Shared Secret handling
// --------------------------------------------------------------------------------

#define XKMSAuthenticationValue					0x01
#define	XKMSRevocationCodeIdenfitierEncoding1	0x02
#define	XKMSRevocationCodeIdenfitierEncoding2	0x03
#define XKMSKeyEncryption						0x04

int DSIG_EXPORT CalculateXKMSAuthenticationKey(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen);
int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding1(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen);
/* Calculate encoding 2 - Input is original pass phrase */
int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding2(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen);
/* Calculate encoding 2 - Input is encoding 1 */
int DSIG_EXPORT CalculateXKMSRevocationCodeIdentifierEncoding2From1(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen);
int DSIG_EXPORT CalculateXKMSKEK(unsigned char * input, int inputLen, unsigned char * output, int maxOutputLen);

// --------------------------------------------------------------------------------
//           Some Base64 helpers
// --------------------------------------------------------------------------------

XMLCh DSIG_EXPORT * EncodeToBase64XMLCh(unsigned char * input, int inputLen);
unsigned int DSIG_EXPORT DecodeFromBase64XMLCh(const XMLCh * input, unsigned char * output, int maxOutputLen);
unsigned int DSIG_EXPORT DecodeFromBase64(const char * input, unsigned char * output, int maxOutputLen);

// --------------------------------------------------------------------------------
//           Some stuff to help with wierd signatures
// --------------------------------------------------------------------------------

// Convert an ASN.1 format DSA signature (!!!) to the two component integers
// NOTE - both r and s must be at least 20 bytes long

bool DSIG_EXPORT ASN2DSASig(const unsigned char * input, unsigned char * r, unsigned char * s);

// --------------------------------------------------------------------------------
//           Calculate correct OIDs for an RSA sig
// --------------------------------------------------------------------------------

unsigned char * getRSASigOID(hashMethod hm, int &oidLen);

#endif /* XSECCRYPTOUTILS_INCLUDE */


