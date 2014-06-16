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
 * XKMSConstants := Definitions of varius XKMSconstants (mainly strings)
 *
 * $Id: XKMSConstants.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/xkms/XKMSConstants.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE


// --------------------------------------------------------------------------------
//           Constant Strings Storage
// --------------------------------------------------------------------------------

const XMLCh * XKMSConstants::s_unicodeStrURIXKMS;
const XMLCh * XKMSConstants::s_unicodeStrURISOAP11;
const XMLCh * XKMSConstants::s_unicodeStrURISOAP12;

// NOTE All tags are unicode (UTF-16) - but are not marked as such

const XMLCh XKMSConstants::s_tagApplication[] = {

	chLatin_A,
	chLatin_p,
	chLatin_p,
	chLatin_l,
	chLatin_i,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagAuthentication[] = {

	chLatin_A,
	chLatin_u,
	chLatin_t,
	chLatin_h,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_i,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagCompoundRequest[] = {

	chLatin_C,
	chLatin_o,
	chLatin_m,
	chLatin_p,
	chLatin_o,
	chLatin_u,
	chLatin_n,
	chLatin_d,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagCompoundResult[] = {

	chLatin_C,
	chLatin_o,
	chLatin_m,
	chLatin_p,
	chLatin_o,
	chLatin_u,
	chLatin_n,
	chLatin_d,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagD[] = {

	chLatin_D,
	chNull
};

const XMLCh XKMSConstants::s_tagDP[] = {

	chLatin_D,
	chLatin_P,
	chNull
};

const XMLCh XKMSConstants::s_tagDQ[] = {

	chLatin_D,
	chLatin_Q,
	chNull
};

const XMLCh XKMSConstants::s_tagEncryption[] = {

	chLatin_E,
	chLatin_n,
	chLatin_c,
	chLatin_r,
	chLatin_y,
	chLatin_p,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagExchange[] = {

	chLatin_E,
	chLatin_x,
	chLatin_c,
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_g,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagExponent[] = {

	chLatin_E,
	chLatin_x,
	chLatin_p,
	chLatin_o,
	chLatin_n,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagFailure[] = {

	chLatin_F,
	chLatin_a,
	chLatin_i,
	chLatin_l,
	chLatin_u,
	chLatin_r,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagId[] = {

	chLatin_I,
	chLatin_d,
	chNull
};

const XMLCh XKMSConstants::s_tagIdentifier[] = {

	chLatin_I,
	chLatin_d,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_i,
	chLatin_f,
	chLatin_i,
	chLatin_e,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagInvalidReason[] = {

	chLatin_I,
	chLatin_n,
	chLatin_v,
	chLatin_a,
	chLatin_l,
	chLatin_i,
	chLatin_d,
	chLatin_R,
	chLatin_e,
	chLatin_a,
	chLatin_s,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagInverseQ[] = {

	chLatin_I,
	chLatin_n,
	chLatin_v,
	chLatin_e,
	chLatin_r,
	chLatin_s,
	chLatin_e,
	chLatin_Q,
	chNull
};

const XMLCh XKMSConstants::s_tagIndeterminateReason[] = {

	chLatin_I,
	chLatin_n,
	chLatin_d,
	chLatin_e,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chLatin_m,
	chLatin_i,
	chLatin_n,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_a,
	chLatin_s,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyBinding[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyBindingAuthentication[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chLatin_A,
	chLatin_u,
	chLatin_t,
	chLatin_h,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_i,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyInfo[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_I,
	chLatin_n,
	chLatin_f,
	chLatin_o,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyName[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_N,
	chLatin_a,
	chLatin_m,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyUsage[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_U,
	chLatin_s,
	chLatin_a,
	chLatin_g,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagKeyValue[] = {

	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagLocateRequest[] = {

	chLatin_L,
	chLatin_o,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagLocateResult[] = {

	chLatin_L,
	chLatin_o,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagMessageExtension[] = {

	chLatin_M,
	chLatin_e,
	chLatin_s,
	chLatin_s,
	chLatin_a,
	chLatin_g,
	chLatin_e,
	chLatin_E,
	chLatin_x,
	chLatin_t,
	chLatin_e,
	chLatin_n,
	chLatin_s,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagModulus[] = {

	chLatin_M,
	chLatin_o,
	chLatin_d,
	chLatin_u,
	chLatin_l,
	chLatin_u,
	chLatin_s,
	chNull
};

const XMLCh XKMSConstants::s_tagNonce[] = {

	chLatin_N,
	chLatin_o,
	chLatin_n,
	chLatin_c,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagNotBoundAuthentication[] = {

	chLatin_N,
	chLatin_o,
	chLatin_t,
	chLatin_B,
	chLatin_o,
	chLatin_u,
	chLatin_n,
	chLatin_d,
	chLatin_A,
	chLatin_u,
	chLatin_t,
	chLatin_h,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_i,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagNotBefore[] = {

	chLatin_N,
	chLatin_o,
	chLatin_t,
	chLatin_B,
	chLatin_e,
	chLatin_f,
	chLatin_o,
	chLatin_r,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagNotOnOrAfter[] = {

	chLatin_N,
	chLatin_o,
	chLatin_t,
	chLatin_O,
	chLatin_n,
	chLatin_O,
	chLatin_r,
	chLatin_A,
	chLatin_f,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagOpaqueClientData[] = {

	chLatin_O,
	chLatin_p,
	chLatin_a,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_C,
	chLatin_l,
	chLatin_i,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_D,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull
};

const XMLCh XKMSConstants::s_tagOpaqueData[] = {

	chLatin_O,
	chLatin_p,
	chLatin_a,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_D,
	chLatin_a,
	chLatin_t,
	chLatin_a,
	chNull
};

const XMLCh XKMSConstants::s_tagOriginalRequestId[] = {

	chLatin_O,
	chLatin_r,
	chLatin_i,
	chLatin_g,
	chLatin_i,
	chLatin_n,
	chLatin_a,
	chLatin_l,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chLatin_I,
	chLatin_d,
	chNull
};

const XMLCh XKMSConstants::s_tagP[] = {

	chLatin_P,
	chNull
};

const XMLCh XKMSConstants::s_tagPending[] = {

	chLatin_P,
	chLatin_e,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagPendingRequest[] = {

	chLatin_P,
	chLatin_e,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagPrivateKey[] = {

	chLatin_P,
	chLatin_r,
	chLatin_i,
	chLatin_v,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chNull
};

const XMLCh XKMSConstants::s_tagProofOfPossession[] = {

	chLatin_P,
	chLatin_r,
	chLatin_o,
	chLatin_o,
	chLatin_f,
	chLatin_O,
	chLatin_f,
	chLatin_P,
	chLatin_o,
	chLatin_s,
	chLatin_s,
	chLatin_e,
	chLatin_s,
	chLatin_s,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagProtocol[] = {

	chLatin_P,
	chLatin_r,
	chLatin_o,
	chLatin_t,
	chLatin_o,
	chLatin_c,
	chLatin_o,
	chLatin_l,
	chNull
};

const XMLCh XKMSConstants::s_tagPrototypeKeyBinding[] = {

	chLatin_P,
	chLatin_r,
	chLatin_o,
	chLatin_t,
	chLatin_o,
	chLatin_t,
	chLatin_y,
	chLatin_p,
	chLatin_e,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagQ[] = {

	chLatin_Q,
	chNull
};

const XMLCh XKMSConstants::s_tagQueryKeyBinding[] = {

	chLatin_Q,
	chLatin_u,
	chLatin_e,
	chLatin_r,
	chLatin_y,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagRecoverKeyBinding[] = {

	chLatin_R,
	chLatin_e,
	chLatin_c,
	chLatin_o,
	chLatin_v,
	chLatin_e,
	chLatin_r,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagRecoverRequest[] = {

	chLatin_R,
	chLatin_e,
	chLatin_c,
	chLatin_o,
	chLatin_v,
	chLatin_e,
	chLatin_r,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRecoverResult[] = {

	chLatin_R,
	chLatin_e,
	chLatin_c,
	chLatin_o,
	chLatin_v,
	chLatin_e,
	chLatin_r,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRegisterRequest[] = {

	chLatin_R,
	chLatin_e,
	chLatin_g,
	chLatin_i,
	chLatin_s,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRegisterResult[] = {

	chLatin_R,
	chLatin_e,
	chLatin_g,
	chLatin_i,
	chLatin_s,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagReissueKeyBinding[] = {

	chLatin_R,
	chLatin_e,
	chLatin_i,
	chLatin_s,
	chLatin_s,
	chLatin_u,
	chLatin_e,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagReissueRequest[] = {

	chLatin_R,
	chLatin_e,
	chLatin_i,
	chLatin_s,
	chLatin_s,
	chLatin_u,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagReissueResult[] = {

	chLatin_R,
	chLatin_e,
	chLatin_i,
	chLatin_s,
	chLatin_s,
	chLatin_u,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRepresent[] = {

	chLatin_R,
	chLatin_e,
	chLatin_p,
	chLatin_r,
	chLatin_e,
	chLatin_s,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRequestId[] = {

	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chLatin_I,
	chLatin_d,
	chNull
};

const XMLCh XKMSConstants::s_tagRequestSignatureValue[] = {

	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chLatin_S,
	chLatin_i,
	chLatin_g,
	chLatin_n,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_r,
	chLatin_e,
	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagRespondWith[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_p,
	chLatin_o,
	chLatin_n,
	chLatin_d,
	chLatin_W,
	chLatin_i,
	chLatin_t,
	chLatin_h,
	chNull
};

const XMLCh XKMSConstants::s_tagResponseId[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_p,
	chLatin_o,
	chLatin_n,
	chLatin_s,
	chLatin_e,
	chLatin_I,
	chLatin_d,
	chNull
};

const XMLCh XKMSConstants::s_tagResponseLimit[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_p,
	chLatin_o,
	chLatin_n,
	chLatin_s,
	chLatin_e,
	chLatin_L,
	chLatin_i,
	chLatin_m,
	chLatin_i,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagResponseMechanism[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_p,
	chLatin_o,
	chLatin_n,
	chLatin_s,
	chLatin_e,
	chLatin_M,
	chLatin_e,
	chLatin_c,
	chLatin_h,
	chLatin_a,
	chLatin_n,
	chLatin_i,
	chLatin_s,
	chLatin_m,
	chNull
};

const XMLCh XKMSConstants::s_tagResult[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagResultMajor[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chLatin_M,
	chLatin_a,
	chLatin_j,
	chLatin_o,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagResultMinor[] = {

	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chLatin_M,
	chLatin_i,
	chLatin_n,
	chLatin_o,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagRevocationCode[] = {

	chLatin_R,
	chLatin_e,
	chLatin_v,
	chLatin_o,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chLatin_C,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagRevocationCodeIdentifier[] = {

	chLatin_R,
	chLatin_e,
	chLatin_v,
	chLatin_o,
	chLatin_c,
	chLatin_a,
	chLatin_t,
	chLatin_i,
	chLatin_o,
	chLatin_n,
	chLatin_C,
	chLatin_o,
	chLatin_d,
	chLatin_e,
	chLatin_I,
	chLatin_d,
	chLatin_e,
	chLatin_n,
	chLatin_t,
	chLatin_i,
	chLatin_f,
	chLatin_i,
	chLatin_e,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagRevokeKeyBinding[] = {

	chLatin_R,
	chLatin_e,
	chLatin_v,
	chLatin_o,
	chLatin_k,
	chLatin_e,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagRevokeRequest[] = {

	chLatin_R,
	chLatin_e,
	chLatin_v,
	chLatin_o,
	chLatin_k,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRevokeResult[] = {

	chLatin_R,
	chLatin_e,
	chLatin_v,
	chLatin_o,
	chLatin_k,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagRSAKeyPair[] = {

	chLatin_R,
	chLatin_S,
	chLatin_A,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_P,
	chLatin_a,
	chLatin_i,
	chLatin_r,
	chNull
};

const XMLCh XKMSConstants::s_tagService[] = {

	chLatin_S,
	chLatin_e,
	chLatin_r,
	chLatin_v,
	chLatin_i,
	chLatin_c,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagSignature[] = {

	chLatin_S,
	chLatin_i,
	chLatin_g,
	chLatin_n,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_r,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagStatus[] = {

	chLatin_S,
	chLatin_t,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_s,
	chNull
};

const XMLCh XKMSConstants::s_tagStatusRequest[] = {

	chLatin_S,
	chLatin_t,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_s,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagStatusResult[] = {

	chLatin_S,
	chLatin_t,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_s,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagStatusValue[] = {

	chLatin_S,
	chLatin_t,
	chLatin_a,
	chLatin_t,
	chLatin_u,
	chLatin_s,
	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagSuccess[] = {

	chLatin_S,
	chLatin_u,
	chLatin_c,
	chLatin_c,
	chLatin_e,
	chLatin_s,
	chLatin_s,
	chNull
};

const XMLCh XKMSConstants::s_tagUnverifiedKeyBinding[] = {

	chLatin_U,
	chLatin_n,
	chLatin_v,
	chLatin_e,
	chLatin_r,
	chLatin_i,
	chLatin_f,
	chLatin_i,
	chLatin_e,
	chLatin_d,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_B,
	chLatin_i,
	chLatin_n,
	chLatin_d,
	chLatin_i,
	chLatin_n,
	chLatin_g,
	chNull
};

const XMLCh XKMSConstants::s_tagUseKeyWith[] = {

	chLatin_U,
	chLatin_s,
	chLatin_e,
	chLatin_K,
	chLatin_e,
	chLatin_y,
	chLatin_W,
	chLatin_i,
	chLatin_t,
	chLatin_h,
	chNull
};

const XMLCh XKMSConstants::s_tagValidateRequest[] = {

	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_i,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_q,
	chLatin_u,
	chLatin_e,
	chLatin_s,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagValidateResult[] = {

	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_i,
	chLatin_d,
	chLatin_a,
	chLatin_t,
	chLatin_e,
	chLatin_R,
	chLatin_e,
	chLatin_s,
	chLatin_u,
	chLatin_l,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagValidReason[] = {

	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_i,
	chLatin_d,
	chLatin_R,
	chLatin_e,
	chLatin_a,
	chLatin_s,
	chLatin_o,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagValidityInterval[] = {

	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_i,
	chLatin_d,
	chLatin_i,
	chLatin_t,
	chLatin_y,
	chLatin_I,
	chLatin_n,
	chLatin_t,
	chLatin_e,
	chLatin_r,
	chLatin_v,
	chLatin_a,
	chLatin_l,
	chNull
};

const XMLCh XKMSConstants::s_tagValue[] = {

	chLatin_V,
	chLatin_a,
	chLatin_l,
	chLatin_u,
	chLatin_e,
	chNull
};

const XMLCh XKMSConstants::s_tagX509Cert[] = {

	chLatin_X,
	chDigit_5,
	chDigit_0,
	chDigit_9,
	chLatin_C,
	chLatin_e,
	chLatin_r,
	chLatin_t,
	chNull
};

const XMLCh XKMSConstants::s_tagX509Chain[] = {

	chLatin_X,
	chDigit_5,
	chDigit_0,
	chDigit_9,
	chLatin_C,
	chLatin_h,
	chLatin_a,
	chLatin_i,
	chLatin_n,
	chNull
};

const XMLCh XKMSConstants::s_tagResultMajorCodes[][16] = {

	{
		chLatin_N,
		chLatin_o,
		chLatin_n,
		chLatin_e,
		chNull
	},

	{			
		chLatin_S,
		chLatin_u,
		chLatin_c,
		chLatin_c,
		chLatin_e,
		chLatin_s,
		chLatin_s,
		chNull
	},

	{
		chLatin_V,
		chLatin_e,
		chLatin_r,
		chLatin_s,
		chLatin_i,
		chLatin_o,
		chLatin_n,
		chLatin_M,
		chLatin_i,
		chLatin_s,
		chLatin_m,
		chLatin_a,
		chLatin_t,
		chLatin_c,
		chLatin_h,
		chNull
	},

	{
		chLatin_S,
		chLatin_e,
		chLatin_n,
		chLatin_d,
		chLatin_e,
		chLatin_r,
		chNull
	},

	{
		chLatin_R,
		chLatin_e,
		chLatin_c,
		chLatin_e,
		chLatin_i,
		chLatin_v,
		chLatin_e,
		chLatin_r,
		chNull
	},

	{
		chLatin_R,
		chLatin_e,
		chLatin_p,
		chLatin_r,
		chLatin_e,
		chLatin_s,
		chLatin_e,
		chLatin_n,
		chLatin_t,
		chNull
	},

	{
		chLatin_P,
		chLatin_e,
		chLatin_n,
		chLatin_d,
		chLatin_i,
		chLatin_n,
		chLatin_g,
		chNull
	},

};

const XMLCh XKMSConstants::s_tagResultMinorCodes[][20] = {

	{

		chLatin_N,
		chLatin_o,
		chLatin_n,
		chLatin_e,
		chNull
	},

	{
		chLatin_N,
		chLatin_o,
		chLatin_M,
		chLatin_a,
		chLatin_t,
		chLatin_c,
		chLatin_h,
		chNull
	},

	{
		chLatin_T,
		chLatin_o,
		chLatin_o,
		chLatin_M,
		chLatin_a,
		chLatin_n,
		chLatin_y,
		chLatin_R,
		chLatin_e,
		chLatin_s,
		chLatin_p,
		chLatin_o,
		chLatin_n,
		chLatin_s,
		chLatin_e,
		chLatin_s,
		chNull
	},

	{
		chLatin_I,
		chLatin_n,
		chLatin_c,
		chLatin_o,
		chLatin_m,
		chLatin_p,
		chLatin_l,
		chLatin_e,
		chLatin_t,
		chLatin_e,
		chNull
	},

	{
		chLatin_F,
		chLatin_a,
		chLatin_i,
		chLatin_l,
		chLatin_u,
		chLatin_r,
		chLatin_e,
		chNull
	},

	{
		chLatin_R,
		chLatin_e,
		chLatin_f,
		chLatin_u,
		chLatin_s,
		chLatin_e,
		chLatin_d,
		chNull
	},

	{
		chLatin_N,
		chLatin_o,
		chLatin_A,
		chLatin_u,
		chLatin_t,
		chLatin_h,
		chLatin_e,
		chLatin_n,
		chLatin_t,
		chLatin_i,
		chLatin_c,
		chLatin_a,
		chLatin_t,
		chLatin_i,
		chLatin_o,
		chLatin_n,
		chNull
	},

	{
		chLatin_M,
		chLatin_e,
		chLatin_s,
		chLatin_s,
		chLatin_a,
		chLatin_g,
		chLatin_e,
		chLatin_N,
		chLatin_o,
		chLatin_t,
		chLatin_S,
		chLatin_u,
		chLatin_p,
		chLatin_p,
		chLatin_o,
		chLatin_r,
		chLatin_t,
		chLatin_e,
		chLatin_d,
		chNull
	},

	{
		chLatin_U,
		chLatin_n,
		chLatin_k,
		chLatin_n,
		chLatin_o,
		chLatin_w,
		chLatin_n,
		chLatin_R,
		chLatin_e,
		chLatin_s,
		chLatin_p,
		chLatin_o,
		chLatin_n,
		chLatin_s,
		chLatin_e,
		chLatin_I,
		chLatin_d,
		chNull
	},

	{
		chLatin_R,
		chLatin_e,
		chLatin_p,
		chLatin_r,
		chLatin_e,
		chLatin_s,
		chLatin_e,
		chLatin_n,
		chLatin_t,
		chLatin_R,
		chLatin_e,
		chLatin_q,
		chLatin_u,
		chLatin_i,
		chLatin_r,
		chLatin_e,
		chLatin_d,
		chNull
	},

	{
		chLatin_N,
		chLatin_o,
		chLatin_t,
		chLatin_S,
		chLatin_y,
		chLatin_n,
		chLatin_c,
		chLatin_h,
		chLatin_r,
		chLatin_o,
		chLatin_n,
		chLatin_o,
		chLatin_u,
		chLatin_s,
		chNull
	},

};

const XMLCh XKMSConstants::s_tagStatusValueCodes[][15] = {

	{
		chLatin_U,
		chLatin_n,
		chLatin_d,
		chLatin_e,
		chLatin_f,
		chLatin_i,
		chLatin_n,
		chLatin_e,
		chLatin_d,
		chNull
	},

	{			
		chLatin_V,
		chLatin_a,
		chLatin_l,
		chLatin_i,
		chLatin_d,
		chNull
	},

	{
		chLatin_I,
		chLatin_n,
		chLatin_v,
		chLatin_a,
		chLatin_l,
		chLatin_i,
		chLatin_d,
		chNull
	},

	{
		chLatin_I,
		chLatin_n,
		chLatin_d,
		chLatin_e,
		chLatin_t,
		chLatin_e,
		chLatin_r,
		chLatin_m,
		chLatin_i,
		chLatin_n,
		chLatin_a,
		chLatin_t,
		chLatin_e,
		chNull
	},

};

const XMLCh XKMSConstants::s_tagStatusReasonCodes[][17] = {

	{
		chLatin_U,
		chLatin_n,
		chLatin_d,
		chLatin_e,
		chLatin_f,
		chLatin_i,
		chLatin_n,
		chLatin_e,
		chLatin_d,
		chNull
	},

	{
		chLatin_I,
		chLatin_s,
		chLatin_s,
		chLatin_u,
		chLatin_e,
		chLatin_r,
		chLatin_T,
		chLatin_r,
		chLatin_u,
		chLatin_s,
		chLatin_t,
		chNull
	},

	{			
		chLatin_R,
		chLatin_e,
		chLatin_v,
		chLatin_o,
		chLatin_c,
		chLatin_a,
		chLatin_t,
		chLatin_i,
		chLatin_o,
		chLatin_n,
		chLatin_S,
		chLatin_t,
		chLatin_a,
		chLatin_t,
		chLatin_u,
		chLatin_s,
		chNull
	},

	{
		chLatin_V,
		chLatin_a,
		chLatin_l,
		chLatin_i,
		chLatin_d,
		chLatin_i,
		chLatin_t,
		chLatin_y,
		chLatin_I,
		chLatin_n,
		chLatin_t,
		chLatin_e,
		chLatin_r,
		chLatin_v,
		chLatin_a,
		chLatin_l,
		chNull
	},

	{
		chLatin_S,
		chLatin_i,
		chLatin_g,
		chLatin_n,
		chLatin_a,
		chLatin_t,
		chLatin_u,
		chLatin_r,
		chLatin_e,
		chNull
	},

};

	// --------------------------------------------------------------------------------
//           Constant Strings Creation and Deletion
// --------------------------------------------------------------------------------

void XKMSConstants::create() {

	// Set up the static strings

	s_unicodeStrURIXKMS = XMLString::transcode(URI_ID_XKMS);
	s_unicodeStrURISOAP11 = XMLString::transcode(URI_ID_SOAP11);
	s_unicodeStrURISOAP12 = XMLString::transcode(URI_ID_SOAP12);

}

void XKMSConstants::destroy() {

    XMLCh* temp1 = const_cast<XMLCh*>(s_unicodeStrURIXKMS);
    XMLCh* temp2 = const_cast<XMLCh*>(s_unicodeStrURISOAP11);
    XMLCh* temp3 = const_cast<XMLCh*>(s_unicodeStrURISOAP12);
	XSEC_RELEASE_XMLCH(temp1);
	XSEC_RELEASE_XMLCH(temp2);
	XSEC_RELEASE_XMLCH(temp3);
    s_unicodeStrURIXKMS = NULL;
    s_unicodeStrURISOAP11 = NULL;
    s_unicodeStrURISOAP12 = NULL;
}
