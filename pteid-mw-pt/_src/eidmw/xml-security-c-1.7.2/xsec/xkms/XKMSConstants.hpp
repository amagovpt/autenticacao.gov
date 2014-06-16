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
 * XKMSConstants := Definitions of varius XKMS constants (mainly strings)
 *
 * $Id: XKMSConstants.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#ifndef XKMSCONSTANTS_HEADER
#define XKMSCONSTANTS_HEADER

// Xerces
#include <xsec/framework/XSECDefs.hpp>

// Name Spaces

#define URI_ID_XKMS		"http://www.w3.org/2002/03/xkms#"
//#define URI_ID_XKMS		"http://www.w3.org/2004/07/xkms#"
#define URI_ID_SOAP11   "http://schemas.xmlsoap.org/soap/envelope/"
#define URI_ID_SOAP12   "http://www.w3.org/2003/05/soap-envelope"


// --------------------------------------------------------------------------------
//           Constant Strings Class
// --------------------------------------------------------------------------------

class DSIG_EXPORT XKMSConstants {

public:

	// URI_IDs
	static const XMLCh * s_unicodeStrURIXKMS;
	static const XMLCh * s_unicodeStrURISOAP11;
	static const XMLCh * s_unicodeStrURISOAP12;

	// Tags - note all are UTF-16, but not marked as such
	static const XMLCh s_tagApplication[];
	static const XMLCh s_tagAuthentication[];
	static const XMLCh s_tagCompoundRequest[];
	static const XMLCh s_tagCompoundResult[];
	static const XMLCh s_tagD[];
	static const XMLCh s_tagDP[];
	static const XMLCh s_tagDQ[];
	static const XMLCh s_tagEncryption[];
	static const XMLCh s_tagExchange[];
	static const XMLCh s_tagExponent[];
	static const XMLCh s_tagFailure[];
	static const XMLCh s_tagId[];
	static const XMLCh s_tagIdentifier[];
	static const XMLCh s_tagIndeterminateReason[];
	static const XMLCh s_tagInvalidReason[];
	static const XMLCh s_tagInverseQ[];
	static const XMLCh s_tagKeyBinding[];
	static const XMLCh s_tagKeyBindingAuthentication[];
	static const XMLCh s_tagKeyInfo[];
	static const XMLCh s_tagKeyName[];
	static const XMLCh s_tagKeyUsage[];
	static const XMLCh s_tagKeyValue[];
	static const XMLCh s_tagLocateRequest[];
	static const XMLCh s_tagLocateResult[];
	static const XMLCh s_tagMessageExtension[];
	static const XMLCh s_tagModulus[];
	static const XMLCh s_tagNonce[];
	static const XMLCh s_tagNotBoundAuthentication[];
	static const XMLCh s_tagNotBefore[];
	static const XMLCh s_tagNotOnOrAfter[];
	static const XMLCh s_tagOpaqueClientData[];
	static const XMLCh s_tagOpaqueData[];
	static const XMLCh s_tagOriginalRequestId[];
	static const XMLCh s_tagP[];
	static const XMLCh s_tagPending[];
	static const XMLCh s_tagPendingRequest[];
	static const XMLCh s_tagPrivateKey[];
	static const XMLCh s_tagProofOfPossession[];
	static const XMLCh s_tagProtocol[];
	static const XMLCh s_tagPrototypeKeyBinding[];
	static const XMLCh s_tagQ[];
	static const XMLCh s_tagQueryKeyBinding[];
	static const XMLCh s_tagRecoverKeyBinding[];
	static const XMLCh s_tagRecoverRequest[];
	static const XMLCh s_tagRecoverResult[];
	static const XMLCh s_tagRegisterRequest[];
	static const XMLCh s_tagRegisterResult[];
	static const XMLCh s_tagReissueKeyBinding[];
	static const XMLCh s_tagReissueRequest[];
	static const XMLCh s_tagReissueResult[];
	static const XMLCh s_tagRepresent[];
	static const XMLCh s_tagRequestId[];
	static const XMLCh s_tagRequestSignatureValue[];
	static const XMLCh s_tagRespondWith[];
	static const XMLCh s_tagResponseId[];
	static const XMLCh s_tagResponseLimit[];
	static const XMLCh s_tagResponseMechanism[];
	static const XMLCh s_tagResult[];
	static const XMLCh s_tagResultMajor[];
	static const XMLCh s_tagResultMinor[];
	static const XMLCh s_tagRevocationCodeIdentifier[];
	static const XMLCh s_tagRevocationCode[];
	static const XMLCh s_tagRevokeKeyBinding[];
	static const XMLCh s_tagRevokeRequest[];
	static const XMLCh s_tagRevokeResult[];
	static const XMLCh s_tagRSAKeyPair[];
	static const XMLCh s_tagService[];
	static const XMLCh s_tagSignature[];
	static const XMLCh s_tagStatus[];
	static const XMLCh s_tagStatusRequest[];
	static const XMLCh s_tagStatusResult[];
	static const XMLCh s_tagStatusValue[];
	static const XMLCh s_tagSuccess[];
	static const XMLCh s_tagUnverifiedKeyBinding[];
	static const XMLCh s_tagUseKeyWith[];
	static const XMLCh s_tagValidateRequest[];
	static const XMLCh s_tagValidateResult[];
	static const XMLCh s_tagValidReason[];
	static const XMLCh s_tagValidityInterval[];
	static const XMLCh s_tagValue[];
	static const XMLCh s_tagX509Cert[];
	static const XMLCh s_tagX509Chain[];

	// ResultMajor codes

	static const XMLCh s_tagResultMajorCodes[][16];
	static const XMLCh s_tagResultMinorCodes[][20];
	static const XMLCh s_tagStatusValueCodes[][15];
	static const XMLCh s_tagStatusReasonCodes[][17];

	XKMSConstants();

	static void create();
	static void destroy();

};

#endif /* XKMSCONSTANTS_HEADER */

