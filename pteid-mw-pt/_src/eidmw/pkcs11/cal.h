/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2017, 2019 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#if !defined(__CAL__)
#define __CAL__

#include "pteid_p11.h"
// #include "CardLayer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAL_MECHANISM_TABLE                                                                                            \
	{                                                                                                                  \
		{CKM_SHA_1, 160, 160, CKF_DIGEST}, {CKM_SHA256, 256, 256, CKF_DIGEST}, {CKM_SHA384, 384, 384, CKF_DIGEST},     \
			{CKM_SHA512, 512, 512, CKF_DIGEST}, {CKM_RSA_PKCS, 1024, 3072, CKF_HW | CKF_SIGN},                         \
			{CKM_SHA1_RSA_PKCS, 1024, 3072, CKF_HW | CKF_SIGN}, {CKM_SHA256_RSA_PKCS, 1024, 3072, CKF_HW | CKF_SIGN},  \
			{CKM_SHA384_RSA_PKCS, 1024, 3072, CKF_HW | CKF_SIGN},                                                      \
			{CKM_SHA512_RSA_PKCS, 1024, 3072, CKF_HW | CKF_SIGN}, {CKM_RSA_PKCS_PSS, 1024, 3072, CKF_HW | CKF_SIGN},   \
			{CKM_SHA256_RSA_PKCS_PSS, 1024, 3072, CKF_HW | CKF_SIGN},                                                  \
			{CKM_SHA384_RSA_PKCS_PSS, 1024, 3072, CKF_HW | CKF_SIGN},                                                  \
			{CKM_SHA512_RSA_PKCS_PSS, 1024, 3072, CKF_HW | CKF_SIGN}, {CKM_ECDSA, 256, 384, CKF_HW | CKF_SIGN},        \
			{CKM_ECDSA_SHA1, 256, 384, CKF_HW | CKF_SIGN}, {CKM_ECDSA_SHA224, 256, 384, CKF_HW | CKF_SIGN},            \
			{CKM_ECDSA_SHA256, 256, 384, CKF_HW | CKF_SIGN}, {CKM_ECDSA_SHA384, 256, 384, CKF_HW | CKF_SIGN},          \
			{CKM_ECDSA_SHA512, 256, 384, CKF_HW | CKF_SIGN},                                                           \
	}

#define PTEID_TEMPLATE_PRV_KEY                                                                                         \
	{                                                                                                                  \
		{CKA_CLASS, (CK_VOID_PTR)NULL, 0}, {CKA_ID, (CK_VOID_PTR)NULL, 0}, {CKA_SUBJECT, (CK_VOID_PTR)NULL, 0},        \
			{CKA_LABEL, (CK_VOID_PTR)NULL, 0}, {CKA_TOKEN, (CK_VOID_PTR)NULL, 0},                                      \
			{CKA_KEY_TYPE, (CK_VOID_PTR)NULL, 0}, {CKA_SENSITIVE, (CK_VOID_PTR)NULL, 0},                               \
			{CKA_SIGN, (CK_VOID_PTR)NULL, 0}, {CKA_SIGN_RECOVER, (CK_VOID_PTR)NULL, 0},                                \
			{CKA_DECRYPT, (CK_VOID_PTR)NULL, 0}, {CKA_UNWRAP, (CK_VOID_PTR)NULL, 0},                                   \
			{CKA_LOCAL, (CK_VOID_PTR)NULL, 0}, {CKA_EC_PARAMS, (CK_VOID_PTR)NULL, 0},                                  \
			{CKA_MODULUS, (CK_VOID_PTR)NULL, 0}, {CKA_MODULUS_BITS, (CK_VOID_PTR)NULL, 0},                             \
			{CKA_EXTRACTABLE, (CK_VOID_PTR)NULL, 0}, {CKA_ALWAYS_AUTHENTICATE, (CK_VOID_PTR)NULL, 0},                  \
			{CKA_PUBLIC_EXPONENT, (CK_VOID_PTR)NULL, 0}, {CKA_PRIVATE, (CK_VOID_PTR)NULL, 0},                          \
			{CKA_PRIVATE_EXPONENT, (CK_VOID_PTR)NULL, 0}, {CKA_PRIME_1, (CK_VOID_PTR)NULL, 0},                         \
			{CKA_PRIME_2, (CK_VOID_PTR)NULL, 0}, {CKA_EXPONENT_1, (CK_VOID_PTR)NULL, 0},                               \
			{CKA_EXPONENT_2, (CK_VOID_PTR)NULL, 0}, {CKA_COEFFICIENT, (CK_VOID_PTR)NULL, 0}, {                         \
			CKA_DERIVE, (CK_VOID_PTR)NULL, 0                                                                           \
		}                                                                                                              \
	}

#define PTEID_TEMPLATE_PUB_KEY                                                                                         \
	{                                                                                                                  \
		{CKA_CLASS, (CK_VOID_PTR)NULL, 0}, {CKA_ID, (CK_VOID_PTR)NULL, 0}, {CKA_SUBJECT, (CK_VOID_PTR)NULL, 0},        \
			{CKA_LABEL, (CK_VOID_PTR)NULL, 0}, {CKA_TOKEN, (CK_VOID_PTR)NULL, 0},                                      \
			{CKA_KEY_TYPE, (CK_VOID_PTR)NULL, 0}, {CKA_PRIVATE, (CK_VOID_PTR)NULL, 0},                                 \
			{CKA_TRUSTED, (CK_VOID_PTR)NULL, 0}, {CKA_SENSITIVE, (CK_VOID_PTR)NULL, 0},                                \
			{CKA_ENCRYPT, (CK_VOID_PTR)NULL, 0}, {CKA_VERIFY, (CK_VOID_PTR)NULL, 0}, {CKA_WRAP, (CK_VOID_PTR)NULL, 0}, \
			{CKA_VALUE, (CK_VOID_PTR)NULL, 0}, {CKA_EC_PARAMS, (CK_VOID_PTR)NULL, 0},                                  \
			{CKA_EC_POINT, (CK_VOID_PTR)NULL, 0}, {CKA_MODULUS, (CK_VOID_PTR)NULL, 0},                                 \
			{CKA_MODULUS_BITS, (CK_VOID_PTR)NULL, 0}, {CKA_PUBLIC_EXPONENT, (CK_VOID_PTR)NULL, 0}, {                   \
			CKA_DERIVE, (CK_VOID_PTR)NULL, 0                                                                           \
		}                                                                                                              \
	}

#define PTEID_TEMPLATE_CERTIFICATE                                                                                     \
	{                                                                                                                  \
		{CKA_CLASS, (CK_VOID_PTR)NULL, 0}, {CKA_ID, (CK_VOID_PTR)NULL, 0}, {CKA_SUBJECT, (CK_VOID_PTR)NULL, 0},        \
			{CKA_ISSUER, (CK_VOID_PTR)NULL, 0}, {CKA_TOKEN, (CK_VOID_PTR)NULL, 0},                                     \
			{CKA_TRUSTED, (CK_VOID_PTR)NULL, 0}, {CKA_CERTIFICATE_TYPE, (CK_VOID_PTR)NULL, 0},                         \
			{CKA_PRIVATE, (CK_VOID_PTR)NULL, 0}, {CKA_SERIAL_NUMBER, (CK_VOID_PTR)NULL, 0},                            \
			{CKA_VALUE, (CK_VOID_PTR)NULL, 0}, {CKA_LABEL, (CK_VOID_PTR)NULL, 0}, {                                    \
			0, (CK_VOID_PTR)NULL, 0                                                                                    \
		}                                                                                                              \
	}

int cal_init();
int cal_close();
int cal_init_slots(void);
int cal_token_present(CK_SLOT_ID hSlot);
CK_RV cal_get_token_info(CK_SLOT_ID hSlot, CK_TOKEN_INFO_PTR pInfo);
CK_RV cal_get_mechanism_list(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE_PTR pMechanismList, CK_ULONG_PTR pulCount);
CK_RV cal_is_mechanism_supported(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE mechanism);
int cal_connect(CK_SLOT_ID hSlot);
int cal_disconnect(CK_SLOT_ID hSlot);
int cal_init_objects(CK_SLOT_ID hSlot);
CK_RV cal_get_mechanism_info(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo);
int cal_logon(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR pin, int sec_messaging);
int cal_logout(CK_SLOT_ID hSlot);
int cal_change_pin(CK_SLOT_ID hSlot, int l_oldpin, CK_CHAR_PTR oldpin, int l_newpin, CK_CHAR_PTR newpin);
int cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT *pObject);
int cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA *pSignData, unsigned char *in, unsigned long l_in, unsigned char *out,
			 unsigned long *l_out);
int cal_validate_session(P11_SESSION *pSession);
int cal_update_token(CK_SLOT_ID hSlot);
int cal_wait_for_slot_event(int block, int *cardevent, int *ph);
int cal_pace(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR can);

#ifdef __cplusplus
}
#endif

#endif
