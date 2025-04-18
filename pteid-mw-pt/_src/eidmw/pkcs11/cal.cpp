/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012-2014, 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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
#include "ByteArray.h"
#include "eidErrors.h"
#include "pteid_p11.h"
#include "CardLayer.h"
#include "ReadersInfo.h"
#include "p11.h"
#include "util.h"
#include "cal.h"
#include "log.h"
#include "cert.h"
#include "Config.h"
#include <openssl/asn1.h>

#ifndef WIN32
#define strcpy_s(a, b, c) strcpy((a), (c))
#define sprintf_s(a, b, c, d) sprintf((a), (c), (d))
#endif

using namespace eIDMW;

CCardLayer *oCardLayer;
CReadersInfo *oReadersInfo;

extern "C" {
extern unsigned int gRefCount;
extern unsigned int nReaders;
extern P11_SLOT gpSlot[MAX_SLOTS];
// local functions
int cal_translate_error(const char *WHERE, long err);
int cal_map_status(tCardStatus calstatus);
}

#define WHERE "cal_init()"
int cal_init() {
	int ret = 0;
	long lRet = 0;

	if (gRefCount > 0)
		return 0;

	try {
		oCardLayer = new CCardLayer();
		oReadersInfo = new CReadersInfo(oCardLayer->ListReaders());
	} catch (CMWException e) {
		if (oCardLayer)
			delete oCardLayer;
		if (oReadersInfo)
			delete oReadersInfo;
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		if (oCardLayer)
			delete oCardLayer;
		if (oReadersInfo)
			delete oReadersInfo;

		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	// init slots and token in slots
	memset(gpSlot, 0, sizeof(gpSlot));
	ret = cal_init_slots();
	if (ret)
		log_trace(WHERE, "E: p11_init_slots() returns %d", ret);

	return (ret);
}
#undef WHERE

#define WHERE "cal_close()"
int cal_close() {
	int ret = 0;

	if (oCardLayer)
		delete (oCardLayer);
	if (oReadersInfo)
		delete (oReadersInfo);

	oCardLayer = NULL;
	oReadersInfo = NULL;

	return (ret);
}

#undef WHERE

#define WHERE "cal_init_slots()"
int cal_init_slots(void) {
	unsigned long ret = 0;
	long lRet = 0;
	unsigned int i;

	try {
		// CReadersInfo oReadersInfo = oCardLayer->ListReaders();
		nReaders = oReadersInfo->ReaderCount();
		// get readernames
		for (i = 0; i < nReaders; i++) {
			// initialize login state to not logged in by SO nor user
			gpSlot[i].login_type = -1;
			std::string reader = oReadersInfo->ReaderName(i);
			strcpy_n((unsigned char *)gpSlot[i].name, (const char *)reader.c_str(), (unsigned int)reader.size(),
					 (char)'\x00');
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	return (ret);
}
#undef WHERE

#define WHERE "cal_token_present()"
int cal_token_present(CK_SLOT_ID hSlot) {
	int status;
	status = cal_update_token(hSlot);

	switch (status) {
	case P11_CARD_INSERTED:
	case P11_CARD_STILL_PRESENT:
	case P11_CARD_OTHER:
		return (1);
	case P11_CARD_NOT_PRESENT:
	case P11_CARD_REMOVED:
	default:
		return (0);
	}

	return (0);
}
#undef WHERE

#define WHERE "cal_get_token_info()"
CK_RV cal_get_token_info(CK_SLOT_ID hSlot, CK_TOKEN_INFO_PTR pInfo) {
	int ret = CKR_OK;
	int status;
	long lRet = 0;
	P11_SLOT *pSlot = NULL;
	bool needs_pace = false;

	pInfo->flags = 0;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string reader = pSlot->name;

	status = cal_update_token(hSlot);
	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT)) {
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	try {
		CReader &oReader = oCardLayer->getReader(reader);

		auto card_type = oReader.GetCardType();

		// Detect not recognized cards and fail early
		if (card_type == CARD_UNKNOWN) {
			return CKR_TOKEN_NOT_RECOGNIZED;
		}

		if (card_type == eIDMW::CARD_PTEID_IAS5 && oReader.isCardContactless()) {
			needs_pace = true;
		}

		// All the strings in TOKEN_INFO struct are space-padded AND without NULL terminator
		// so we need a specialized strcpy to generate them

		if (needs_pace) {
			strcpy_n(pInfo->label, "CITIZEN CARD", 32, ' ');
			std::string oSerialNr = "UNKNOWN";
			strcpy_n(pInfo->serialNumber, oSerialNr.c_str(), oSerialNr.size(), ' ');
		} else {
			strcpy_n(pInfo->label, oReader.GetCardLabel().c_str(), 32, ' ');

			// Take the last 16 hex chars of the serialnr.
			// For PTeID cards, the serial nr. is 32 hex chars long,
			// and the first one are the same for all cards
			std::string oSerialNr = oReader.GetSerialNr();
			size_t serialNrLen = oSerialNr.size();
			size_t snoffset = serialNrLen > 16 ? serialNrLen - 16 : 0;
			size_t snlen = serialNrLen - snoffset > 16 ? 16 : serialNrLen - snoffset;
			strcpy_n(pInfo->serialNumber, oSerialNr.c_str() + snoffset, snlen, ' ');
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	strcpy_n(pInfo->manufacturerID, "Portuguese Government", 32, ' ');
	strcpy_n(pInfo->model, "Portuguese eID NG", 16, ' ');

	pInfo->ulMaxSessionCount = MAX_SESSIONS; // CK_EFFECTIVELY_INFINITE;
	pInfo->ulSessionCount = pSlot->nsessions;
	pInfo->ulMaxRwSessionCount = MAX_RW_SESSIONS;
	pInfo->ulRwSessionCount = 0; /* FIXME */
	pInfo->ulTotalPublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePublicMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulTotalPrivateMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->ulFreePrivateMemory = CK_UNAVAILABLE_INFORMATION;
	pInfo->hardwareVersion.major = 1;
	pInfo->hardwareVersion.minor = 0;
	pInfo->firmwareVersion.major = 1;
	pInfo->firmwareVersion.minor = 0;

	pInfo->ulMaxPinLen = 8;
	pInfo->ulMinPinLen = 6;
	strcpy_s((char *)pInfo->utcTime, sizeof(pInfo->utcTime), "20080101000000");

	pInfo->flags |= CKF_TOKEN_INITIALIZED;
	if (needs_pace) {
		pInfo->flags |= CKF_LOGIN_REQUIRED;
	} else {
		pInfo->flags |= CKF_USER_PIN_INITIALIZED;
	}

cleanup:

	return ((CK_RV)ret);
}
#undef WHERE

#define WHERE "cal_get_mechanism_list()"
CK_RV cal_get_mechanism_list(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE_PTR pMechanismList, CK_ULONG_PTR pulCount) {
	int ret = CKR_OK;
	int status;
	long lRet = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	unsigned long algos = 0;
	unsigned int n = 0;

	status = cal_update_token(hSlot);
	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT)) {
		return (CKR_TOKEN_NOT_PRESENT);
	}

	try {
		CReader &oReader = oCardLayer->getReader(szReader);
		algos = oReader.GetSupportedAlgorithms();
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	if (pMechanismList == NULL) {
		*pulCount = 4; // for 4 hash algos

		if (algos & SIGN_ALGO_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA1_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA256_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA384_RSA_PKCS)
			*pulCount += 1;
		if (algos & SIGN_ALGO_SHA512_RSA_PKCS)
			*pulCount += 1;

		// ECDSA algos
		if (algos & SIGN_ALGO_ECDSA)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_SHA1)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_SHA224)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_SHA256)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_SHA384)
			*pulCount += 1;
		if (algos & SIGN_ALGO_ECDSA_SHA512)
			*pulCount += 1;

		// 4 variants of RSA-PSS
		if (algos & SIGN_ALGO_RSA_PSS)
			*pulCount += 4;
		return (CKR_OK);
	}

	/* hash algos */
	if (n++ <= *pulCount)
		pMechanismList[n - 1] = CKM_SHA_1;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ <= *pulCount)
		pMechanismList[n - 1] = CKM_SHA256;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ <= *pulCount)
		pMechanismList[n - 1] = CKM_SHA384;
	else
		return (CKR_BUFFER_TOO_SMALL);

	if (n++ <= *pulCount)
		pMechanismList[n - 1] = CKM_SHA512;
	else
		return (CKR_BUFFER_TOO_SMALL);

	/* sign algos */
	if (algos & SIGN_ALGO_RSA_PKCS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA1_RSA_PKCS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA1_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA256_RSA_PKCS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA256_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA384_RSA_PKCS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA384_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_SHA512_RSA_PKCS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA512_RSA_PKCS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_RSA_PSS) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA256_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);

		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA384_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);

		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_SHA512_RSA_PKCS_PSS;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_SHA1) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA1;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_SHA224) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA224;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_SHA256) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA256;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_SHA384) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA384;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}
	if (algos & SIGN_ALGO_ECDSA_SHA512) {
		if (n++ <= *pulCount)
			pMechanismList[n - 1] = CKM_ECDSA_SHA512;
		else
			return (CKR_BUFFER_TOO_SMALL);
	}

	return (ret);
}
#undef WHERE

#define WHERE "cal_is_mechanism_supported"
CK_RV cal_is_mechanism_supported(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE mechanism) {
	CK_RV ret = CKR_OK;
	BOOL supported_algorithm = FALSE;
	CK_ULONG count = 0;

	cal_get_mechanism_list(hSlot, NULL, &count);
	CK_MECHANISM_TYPE_PTR pMechanismList = (CK_MECHANISM_TYPE_PTR)malloc(count * sizeof(CK_MECHANISM_TYPE));

	if (pMechanismList == NULL) {
		log_trace(WHERE, "E: error allocating memory");
		ret = CKR_HOST_MEMORY;
		goto cleanup;
	}

	cal_get_mechanism_list(hSlot, pMechanismList, &count);

	for (unsigned int i = 0; i < count; i++) {
		if (pMechanismList[i] == mechanism) {
			supported_algorithm = TRUE;
			break;
		}
	}

	if (!supported_algorithm) {
		ret = CKR_MECHANISM_INVALID;
		goto cleanup;
	}

cleanup:
	if (pMechanismList)
		free(pMechanismList);

	return ret;
}
#undef WHERE

#define WHERE cal_get_mechanism_info()
CK_RV cal_get_mechanism_info(CK_SLOT_ID hSlot, CK_MECHANISM_TYPE type, CK_MECHANISM_INFO_PTR pInfo) {
	CK_RV ret = CKR_OK;
	P11_MECHANISM_INFO table[] = CAL_MECHANISM_TABLE;
	P11_MECHANISM_INFO *info = NULL;
	int i;

	if (pInfo == NULL_PTR) {
		ret = CKR_ARGUMENTS_BAD;
		goto cleanup;
	}

	// look for type in table
	for (i = 0; i < (int)(sizeof(table) / sizeof(table[0])); i++) {
		if (table[i].type == type) {
			info = &table[i];
			break;
		}
	}

	if ((info) && (info->type)) {
		pInfo->ulMinKeySize = info->ulMinKeySize;
		pInfo->ulMaxKeySize = info->ulMaxKeySize;
		pInfo->flags = info->flags;
	} else
		ret = CKR_MECHANISM_INVALID;

cleanup:

	return (ret);
}
#undef WHERE

#define WHERE "cal_connect()"
int cal_connect(CK_SLOT_ID hSlot) {
	int ret = 0;
	int status;
	P11_SLOT *pSlot = NULL;

	// connect to token
	status = cal_update_token(hSlot);
	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT)) {
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	pSlot->connect++;

cleanup:

	return (0);
}
#undef WHERE

#define WHERE "cal_disconnect()"
int cal_disconnect(CK_SLOT_ID hSlot) {
	long lRet = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	if (pSlot->connect > 0)
		pSlot->connect--;

	if (pSlot->connect < 1) {
		pSlot->connect = 0;
		std::string szreader = pSlot->name;
		try {
			CReader &oReader = oCardLayer->getReader(szreader);
			oReader.Disconnect();
		} catch (CMWException e) {
			printf("pSlot->name;");
			return (cal_translate_error(WHERE, e.GetError()));
		} catch (...) {
			lRet = -1;
			return (CKR_FUNCTION_FAILED);
		}
	}
	return (0);
}
#undef WHERE

#define WHERE "cal_init_objects()"
int cal_init_objects(P11_SLOT *pSlot) {
	int ret = CKR_OK;
	long lRet = 0;
	CK_ATTRIBUTE PRV_KEY[] = PTEID_TEMPLATE_PRV_KEY;
	CK_ATTRIBUTE PUB_KEY[] = PTEID_TEMPLATE_PUB_KEY;
	CK_ATTRIBUTE CERTIFICATE[] = PTEID_TEMPLATE_CERTIFICATE;
	CK_ULONG hObject = 0;
	P11_OBJECT *pObject = NULL;
	CK_KEY_TYPE keytype = CKK_RSA;
	CK_BBOOL btrue = CK_TRUE;
	CK_BBOOL bfalse = CK_FALSE;
	CK_ULONG modsize = 0; /* TODO read from pkcs15 */
	CK_ULONG id = 0;
	unsigned int i = 0;
	char clabel[128];
	CK_CERTIFICATE_TYPE certType = CKC_X_509;

	// printf("::::: CREATING INIT OBJECTS!!! :::::\n");

	// this function will initialize objects as they are valid for the token
	// this function does not read the actual values but enables an application to
	// search for an attribute
	// attributes are only read from the token as needed and thereafter they are cached
	// they remain valid as long as the connection with the smartcard remains valid
	// an application might check the PCKS#15 on the card to initialize the PKCS#11 objects here
	// or can decide to do this statically

	// mapping between these PKCS11 objects and "real" pteid objects is done through CLASS type and ID
	//  e.g. CKA_CLASS=CKO_PRIVATE_KEY and CK_ID=0   => this object is representative for the authentication key on the
	//  card.
	//       CKA_CLASS=CKO_PUBKEY and CK_ID=0        => this object is represents the public key found in the
	//       authentication certificate
	//                                                  to read this public key, we read the certificate and extract the
	//                                                  public key components

	// set attribute template, CKA_TOKEN to true, fill CKA_CLASS type, ID value and CKA_PRIVATE flag

	std::string szReader = pSlot->name;
	try {
		CReader &oReader = oCardLayer->getReader(szReader);
		oReader.PrivKeyCount();

		if (oReader.GetCardType() == CARD_PTEID_IAS5) {
			keytype = CKK_EC;
		}

		for (i = 0; i < oReader.PrivKeyCount(); i++) {
			/***************/
			/* Private key */
			/***************/
			tPrivKey key = oReader.GetPrivKey(i);
			id = (CK_ULONG)key.ulID;

			sprintf_s(clabel, sizeof(clabel), "%s", key.csLabel.c_str());

			ret = p11_add_slot_object(pSlot, PRV_KEY, sizeof(PRV_KEY) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PRIVATE_KEY,
									  id, CK_TRUE, &hObject);
			if (ret)
				goto cleanup;

			// put some other attribute items already so the key can be used for signing
			pObject = p11_get_slot_object(pSlot, hObject);

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel,
										  (CK_ULONG)strlen(clabel));
			if (ret)
				goto cleanup;
			// KEY_TYPE is always RSA
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_KEY_TYPE, (CK_VOID_PTR)&keytype,
										  sizeof(CK_KEY_TYPE));
			if (ret)
				goto cleanup;

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_SIGN, (CK_VOID_PTR)&btrue, sizeof(btrue));
			if (ret)
				goto cleanup;

			// TODO error in cal, size is in bits already
			modsize = key.ulKeyLenBytes * 8;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_MODULUS_BITS, (CK_VOID_PTR)&modsize,
										  sizeof(CK_ULONG));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_EXTRACTABLE, (CK_VOID_PTR)&bfalse,
										  sizeof(bfalse));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_ALWAYS_AUTHENTICATE, (CK_VOID_PTR)&bfalse,
										  sizeof(bfalse));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_DERIVE, (CK_VOID_PTR)&bfalse,
										  sizeof(bfalse));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LOCAL, (CK_VOID_PTR)&bfalse,
										  sizeof(bfalse));
			if (ret)
				goto cleanup;
			/**************************************************/
			/* Public key corresponding to private key object */
			/**************************************************/
			ret = p11_add_slot_object(pSlot, PUB_KEY, sizeof(PUB_KEY) / sizeof(CK_ATTRIBUTE), CK_TRUE, CKO_PUBLIC_KEY,
									  id, CK_FALSE, &hObject);
			if (ret)
				goto cleanup;

			pObject = p11_get_slot_object(pSlot, hObject);

			sprintf_s(clabel, sizeof(clabel), "%s", key.csLabel.c_str());
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel,
										  (CK_ULONG)strlen(clabel));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_KEY_TYPE, (CK_VOID_PTR)&keytype,
										  sizeof(CK_KEY_TYPE));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_MODULUS_BITS, (CK_VOID_PTR)&modsize,
										  sizeof(CK_ULONG));
			if (ret)
				goto cleanup;
		}

		/* add all certificate objects from card */
		for (i = 0; i < oReader.CertCount(); i++) {
			id = (CK_ULONG)oReader.GetCert(i).ulID;

			sprintf_s(clabel, sizeof(clabel), "%s", oReader.GetCert(i).csLabel.c_str());

			ret = p11_add_slot_object(pSlot, CERTIFICATE, sizeof(CERTIFICATE) / sizeof(CK_ATTRIBUTE), CK_TRUE,
									  CKO_CERTIFICATE, id, CK_FALSE, &hObject);
			if (ret)
				goto cleanup;
			pObject = p11_get_slot_object(pSlot, hObject);

			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_CERTIFICATE_TYPE, (CK_VOID_PTR)&certType,
										  sizeof(CK_ULONG));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pObject->pAttr, pObject->count, CKA_LABEL, (CK_VOID_PTR)clabel,
										  (CK_ULONG)strlen(clabel));
			if (ret)
				goto cleanup;
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

cleanup:

	return (ret);
}
#undef WHERE

#define WHERE "cal_pace"
int cal_pace(CK_SLOT_ID hSlot, size_t l_can, CK_CHAR_PTR can) {
	auto pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try {
		CReader &oReader = oCardLayer->getReader(pSlot->name);

		oReader.initPaceAuthentication((const char *)can, l_can, PaceSecretType::PACECAN);
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	return CKR_OK;
}
#undef WHERE

#define WHERE "cal_logon()"
int cal_logon(CK_SLOT_ID hSlot, size_t l_pin, CK_CHAR_PTR pin, int sec_messaging) {
	int ret = CKR_OK;
	long lRet = 0;
	char cpin[20];
	P11_SLOT *pSlot = NULL;

	// printf("---In cal_logon. PIN size: %d\n",l_pin);

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		// printf("---Invalid slot\n");
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;

	memset(cpin, 0, sizeof(cpin));
	if (pin && (l_pin < sizeof(cpin)))
		memcpy(cpin, pin, l_pin);

	std::string csPin = cpin;
	unsigned long ulRemaining = 0;
	unsigned long ulPinIdx = 0;

	try {
		CReader &oReader = oCardLayer->getReader(szReader);
		tPin tpin = oReader.GetPin(ulPinIdx);
		if (!oReader.PinCmd(PIN_OP_VERIFY, tpin, csPin, "", ulRemaining)) {
			if (ulRemaining == 0) {
				ret = CKR_PIN_LOCKED;
			} else
				ret = CKR_PIN_INCORRECT;
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}
	return (ret);
}
#undef WHERE

#define WHERE "cal_logout()"
int cal_logout(CK_SLOT_ID hSlot) {
	int ret = CKR_OK;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	// CReader &oReader = oCardLayer->getReader(szReader);

	/*TODO ??? oReader.*/

	return (ret);
}
#undef WHERE

#define WHERE "cal_change_pin()"
int cal_change_pin(CK_SLOT_ID hSlot, int l_oldpin, CK_CHAR_PTR oldpin, int l_newpin, CK_CHAR_PTR newpin) {
	int ret = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	std::string szReader = pSlot->name;
	CReader &oReader = oCardLayer->getReader(szReader);

	static std::string csPin = (char *)oldpin;
	static std::string csNewPin = (char *)newpin;
	unsigned long ulRemaining = 0;

	tPin tpin = oReader.GetPin(0);

	if (!oReader.PinCmd(PIN_OP_CHANGE, tpin, csPin, csNewPin, ulRemaining)) {
		if (ulRemaining == 0)
			ret = CKR_PIN_LOCKED;
		else
			ret = CKR_PIN_INCORRECT;
	}

	return (ret);
}
#undef WHERE

#define WHERE "cal_read_object()"
int cal_read_object(CK_SLOT_ID hSlot, P11_OBJECT *pObject) {
	int ret = 0;
	int status;
	long lRet = 0;
	CK_ULONG *pID = NULL;
	CK_ULONG *pClass = NULL;
	CK_ULONG len = 0;
	CK_BBOOL btrue = CK_TRUE;
	CK_BBOOL bfalse = CK_FALSE;
	P11_OBJECT *pCertObject = NULL;
	P11_OBJECT *pPubKeyObject = NULL;
	P11_OBJECT *pPrivKeyObject = NULL;
	T_CERT_INFO certinfo;
	T_RSA_KEY_INFO rsa_keyinfo;
	CByteArray oCertData;
	tCert cert;
	tPrivKey key;
	std::string szReader;
	P11_SLOT *pSlot = NULL;
	CK_KEY_TYPE keytype = CKK_RSA;
	CByteArray ec_params;
	CByteArray ec_point;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	szReader = pSlot->name;

	status = cal_update_token(hSlot);
	if ((status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT)) {
		ret = CKR_TOKEN_NOT_PRESENT;
		goto cleanup;
	}

	// read ID of object we want to read from token
	ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_ID, (CK_VOID_PTR *)&pID, &len);
	if (ret)
		goto cleanup;

	ret = p11_get_attribute_value(pObject->pAttr, pObject->count, CKA_CLASS, (CK_VOID_PTR *)&pClass, &len);
	if (ret)
		goto cleanup;

	// get all objects related to this ID
	ret = p11_find_slot_object(pSlot, CKO_CERTIFICATE, *pID, &pCertObject);
	if (ret)
		goto cleanup;

	p11_find_slot_object(pSlot, CKO_PRIVATE_KEY, *pID, &pPrivKeyObject);
	// in case of cacertificate there is no private key with this id
	p11_find_slot_object(pSlot, CKO_PUBLIC_KEY, *pID, &pPubKeyObject);
	// in case of cacertificate there is no public key with this id

	try {
		CReader &oReader = oCardLayer->getReader(szReader);
		cert = oReader.GetCertByID(*pID);

		// Select EID app before reading certificate
		if (oReader.GetCardType() == CARD_PTEID_IAS5) {
			keytype = CKK_EC;
			oReader.SelectApplication({PTEID_2_APPLET_EID, sizeof(PTEID_2_APPLET_EID)});
		}

		if (cert.bValid)
			oCertData = oReader.ReadFile(cert.csPath);
		else {
			return (CKR_DEVICE_ERROR);
		}

		ret = cert_get_info(oCertData.GetBytes(), oCertData.Size(), &certinfo);
		if (ret)
			goto cleanup;

		if (keytype == CKK_RSA) {
			ret = get_rsa_key_info(oCertData.GetBytes(), oCertData.Size(), &rsa_keyinfo);
			if (ret)
				goto cleanup;
		}

		ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_SUBJECT,
									  (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
		if (ret)
			goto cleanup;
		ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_ISSUER, (CK_VOID_PTR)certinfo.issuer,
									  (CK_ULONG)certinfo.l_issuer);
		if (ret)
			goto cleanup;
		ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_SERIAL_NUMBER,
									  (CK_VOID_PTR)certinfo.serial, (CK_ULONG)certinfo.l_serial);
		if (ret)
			goto cleanup;
		// use real length from decoder here instead of lg from cal
		ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_VALUE,
									  (CK_VOID_PTR)oCertData.GetBytes(), (CK_ULONG)certinfo.lcert);
		if (ret)
			goto cleanup;
		// TODO Check this in the cal if we can be sure that the certificate can be trusted and not be modified on the
		// card
		ret = p11_set_attribute_value(pCertObject->pAttr, pCertObject->count, CKA_TRUSTED, (CK_VOID_PTR)&btrue,
									  sizeof(btrue));
		if (ret)
			goto cleanup;

		pCertObject->state = P11_CACHED;

		// Read public key file directly from card
		if (oReader.GetCardType() == CARD_PTEID_IAS5) {
			// pID = type
			unsigned char cmd[16] = {0x00, 0xCB, 0x00, 0xFF, 0x0A, 0xB6, 0x03, 0x83,
									 0x01, 0x08, 0x7F, 0x49, 0x02, 0x06, 0x00, 0x00};
			long len;
			CByteArray result_buff;

			if (*pID == 0x45)
				cmd[9] = 0x06; // Auth
			else
				cmd[9] = 0x08; // Sign

			// Read EC_PARAMS
			result_buff = oReader.SendAPDU({cmd, sizeof(cmd)});
			len = result_buff.Size();
			unsigned char *params = parse_ec_params(result_buff.GetBytes(), &len);
			if (params == NULL)
				return CKR_DEVICE_ERROR;

			ec_params.Append(params, len);

			// Read EC_POINT
			cmd[13] = 0x86;
			result_buff = oReader.SendAPDU({cmd, sizeof(cmd)});
			len = result_buff.Size();
			unsigned char *point = parse_ec_point(result_buff.GetBytes(), &len);
			if (point == NULL)
				return CKR_DEVICE_ERROR;

			ec_point.Append(0x4);
			assert(len <= UCHAR_MAX);
			ec_point.Append((unsigned char) len);
			ec_point.Append(point, len);
		}

		if (pPrivKeyObject) {
			key = oReader.GetPrivKeyByID(*pID);
			ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SUBJECT,
										  (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SENSITIVE,
										  (CK_VOID_PTR)&btrue, sizeof(btrue));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_DECRYPT,
										  (CK_VOID_PTR)&bfalse, sizeof(bfalse));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_SIGN_RECOVER,
										  (CK_VOID_PTR)&bfalse, sizeof(CK_BBOOL));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_UNWRAP,
										  (CK_VOID_PTR)&bfalse, sizeof(CK_BBOOL));

			if (keytype == CKK_RSA) {
				if (ret)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_MODULUS,
											  (CK_VOID_PTR)rsa_keyinfo.mod, (CK_ULONG)rsa_keyinfo.l_mod);
				if (ret)
					goto cleanup;
				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_PUBLIC_EXPONENT,
											  (CK_VOID_PTR)rsa_keyinfo.exp, (CK_ULONG)rsa_keyinfo.l_exp);
				if (ret)
					goto cleanup;
			} else if (keytype == CKK_EC) {
				ret = p11_set_attribute_value(pPrivKeyObject->pAttr, pPrivKeyObject->count, CKA_EC_PARAMS,
											  (CK_VOID_PTR)ec_params.GetBytes(), ec_params.Size());
				if (ret)
					goto cleanup;
			}

			pPrivKeyObject->state = P11_CACHED;
		}

		if (pPubKeyObject) {
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_SUBJECT,
										  (CK_VOID_PTR)certinfo.subject, (CK_ULONG)certinfo.l_subject);
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_SENSITIVE,
										  (CK_VOID_PTR)&btrue, sizeof(btrue));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_VERIFY, (CK_VOID_PTR)&btrue,
										  sizeof(btrue));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_ENCRYPT, (CK_VOID_PTR)&bfalse,
										  sizeof(bfalse));
			if (ret)
				goto cleanup;
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_WRAP, (CK_VOID_PTR)&bfalse,
										  sizeof(CK_BBOOL));
			if (ret)
				goto cleanup;
			if (keytype == CKK_RSA) {
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_MODULUS,
											  (CK_VOID_PTR)rsa_keyinfo.mod, rsa_keyinfo.l_mod);
				if (ret)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_PUBLIC_EXPONENT,
											  (CK_VOID_PTR)rsa_keyinfo.exp, rsa_keyinfo.l_exp);
				if (ret)
					goto cleanup;
			} else if (keytype == CKK_EC) {
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_PARAMS,
											  (CK_VOID_PTR)ec_params.GetBytes(), ec_params.Size());
				if (ret)
					goto cleanup;
				ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_EC_POINT,
											  (CK_VOID_PTR)ec_point.GetBytes(), ec_point.Size());
				if (ret)
					goto cleanup;
			}
			// TODO test if we can set the trusted flag...
			ret = p11_set_attribute_value(pPubKeyObject->pAttr, pPubKeyObject->count, CKA_TRUSTED, (CK_VOID_PTR)&btrue,
										  sizeof(btrue));
			if (ret)
				goto cleanup;

			pPubKeyObject->state = P11_CACHED;
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	if (ret != 0) {
		return (CKR_DEVICE_ERROR);
	}

cleanup:
	if (certinfo.serial)
		OPENSSL_free(certinfo.serial);
	if (certinfo.issuer)
		OPENSSL_free(certinfo.issuer);
	if (certinfo.subject)
		OPENSSL_free(certinfo.subject);

	if (keytype == CKK_RSA) {
		if (rsa_keyinfo.exp)
			OPENSSL_free(rsa_keyinfo.exp);
		if (rsa_keyinfo.mod)
			OPENSSL_free(rsa_keyinfo.mod);
	}

	return (ret);
}
#undef WHERE

#define WHERE "cal_sign()"
int cal_sign(CK_SLOT_ID hSlot, P11_SIGN_DATA *pSignData, unsigned char *in, unsigned long l_in, unsigned char *out,
			 unsigned long *l_out) {
	int ret = 0;
	long lRet = 0;
	CByteArray oData(in, l_in);
	CByteArray oDataOut;
	unsigned long algo;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}
	std::string szReader = pSlot->name;

	try {
		CReader &oReader = oCardLayer->getReader(szReader);

		tPrivKey key = oReader.GetPrivKeyByID(pSignData->id);

		switch (pSignData->mechanism) {
		case CKM_RSA_PKCS:
			algo = SIGN_ALGO_RSA_PKCS;
			break;
		case CKM_SHA_1:
		case CKM_SHA1_RSA_PKCS:
			algo = SIGN_ALGO_SHA1_RSA_PKCS;
			break;
		case CKM_SHA256:
		case CKM_SHA256_RSA_PKCS:
			algo = SIGN_ALGO_SHA256_RSA_PKCS;
			break;
		case CKM_SHA384:
		case CKM_SHA384_RSA_PKCS:
			algo = SIGN_ALGO_SHA384_RSA_PKCS;
			break;
		case CKM_SHA512:
		case CKM_SHA512_RSA_PKCS:
			algo = SIGN_ALGO_SHA512_RSA_PKCS;
			break;
		case CKM_ECDSA:
			algo = SIGN_ALGO_ECDSA;
			break;
		case CKM_ECDSA_SHA1:
			algo = SIGN_ALGO_ECDSA_SHA1;
			break;
		case CKM_ECDSA_SHA224:
			algo = SIGN_ALGO_ECDSA_SHA224;
			break;
		case CKM_ECDSA_SHA256:
			algo = SIGN_ALGO_ECDSA_SHA256;
			break;
		case CKM_ECDSA_SHA384:
			algo = SIGN_ALGO_ECDSA_SHA384;
			break;
		case CKM_ECDSA_SHA512:
			algo = SIGN_ALGO_ECDSA_SHA512;
			break;
		case CKM_RSA_PKCS_PSS:
		case CKM_SHA384_RSA_PKCS_PSS:
		case CKM_SHA512_RSA_PKCS_PSS:
		case CKM_SHA256_RSA_PKCS_PSS:
			algo = SIGN_ALGO_RSA_PSS;
			break;
		default:
			ret = CKR_MECHANISM_INVALID;
			goto cleanup;
		}

		if (algo == SIGN_ALGO_RSA_PKCS && l_in == 51 && oReader.GetCardType() == CARD_PTEID_IAS07) {
			// Strip away the PKCS-1 padding (first 19 bytes)
			oData = oData.GetBytes(19);
			algo = SIGN_ALGO_SHA256_RSA_PKCS;
		}

		oDataOut = oReader.Sign(key, algo, oData);
	} catch (CMWException &e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_FUNCTION_FAILED);
	}

	*l_out = oDataOut.Size();
	if (oDataOut.Size() > *l_out)
		return (CKR_BUFFER_TOO_SMALL);

	memcpy(out, oDataOut.GetBytes(), *l_out);

cleanup:

	return (ret);
}
#undef WHERE

#define WHERE "cal_validate_session()"
int cal_validate_session(P11_SESSION *pSession) {
	int ret = 0;
	int status;
	long lRet = 0;

	if (pSession->inuse == 0) {
		// printf("******** inuse = 0. Session handle invalid\n");
		return (CKR_SESSION_HANDLE_INVALID);
	}

	// check status flag in session first
	if (pSession->state != P11_CARD_STILL_PRESENT) {
		// state allready invalid for this session so we can return here
		switch (pSession->state) {
		case P11_CARD_INSERTED: // card is inserted (Opensession allready called connect(update) so state PRESENT is not
								// expected anymore)
		case P11_CARD_NOT_PRESENT: // card is not present
		case P11_CARD_REMOVED:	   // card is removed
		case P11_CARD_OTHER:	   // other card has been inserted
		default:
			//        return (CKR_TOKEN_NOT_PRESENT);
			return (CKR_DEVICE_REMOVED);
			//        return (CKR_SESSION_HANDLE_INVALID);
		}
	}

	try {
		// previous state is STILL_PRESENT so get new state to see if this has changed
		status = cal_update_token(pSession->hslot);
		if (status == P11_CARD_STILL_PRESENT)
			ret = 0;
		else {
			//      if ( (status == P11_CARD_REMOVED) || (status == P11_CARD_NOT_PRESENT) )
			ret = CKR_DEVICE_REMOVED;
			//      else
			////         ret = CKR_SESSION_HANDLE_INVALID;
			//         ret = CKR_OK;
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_GENERAL_ERROR);
	}
	return (ret);
}
#undef WHERE

#define WHERE "cal_update_token()"
int cal_update_token(CK_SLOT_ID hSlot) {
	long lRet;
	P11_OBJECT *pObject = NULL;
	int status;
	int ret = 0;
	unsigned int i = 0;
	P11_SLOT *pSlot = NULL;

	pSlot = p11_get_slot(hSlot);
	if (pSlot == NULL) {
		log_trace(WHERE, "E: Invalid slot (%d)", hSlot);
		return (CKR_SLOT_ID_INVALID);
	}

	try {
		std::string reader = pSlot->name;
		CReader &oReader = oCardLayer->getReader(reader);
		status = cal_map_status(oReader.Status(true));

		if (status != P11_CARD_STILL_PRESENT) {
			// clean objects
			for (i = 1; i <= pSlot->nobjects; i++) {
				pObject = p11_get_slot_object(pSlot, i);
				p11_clean_object(pObject);
			}

			// invalidate sessions
			p11_invalidate_sessions(hSlot, status);

			// if Present, other => init objects
			if ((status == P11_CARD_OTHER) || (status == P11_CARD_INSERTED)) {
				//(re)initialize objects
				ret = cal_init_objects(pSlot);
				if (ret) {
					log_trace(WHERE, "E: cal_init_objects() returned %s", log_map_error(ret));
				}
			}
		}
	} catch (CMWException e) {
		return (cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		return (CKR_SESSION_HANDLE_INVALID);
	}
	return ((int)status);
}
#undef WHERE

#define WHERE "cal_wait_for_slot_event()"
int cal_wait_for_slot_event(int block, int *cardevent, int *ph) {
	int ret = 0;
	long lRet = 0;
	bool calevent = false;
	int first = 1;
	P11_SLOT *pSlot = NULL;

	try {
		if (block) {
			calevent = oReadersInfo->CheckReaderEvents(TIMEOUT_INFINITE, ALL_READERS);
			log_trace(WHERE, "W: calevent (1) = %d", calevent == true ? 1 : 0);
			//    if (calevent == false)
			//       {
			//       calevent = oReadersInfo.CheckReaderEvents(TIMEOUT_INFINITE, ALL_READERS);
			//       log_trace(WHERE, "W: calevent (2) = %d", calevent == true ? 1:0);
			//       }
		} else {
			calevent = oReadersInfo->CheckReaderEvents(0);
		}

		if (calevent) {
			*cardevent = 1;
			// 0 to nreaders match the handles of readers in slotlist
			for (int i = 0; i < p11_get_nreaders(); i++) {
				if (oReadersInfo->ReaderStateChanged(i)) {
					// return first reader that changed state
					// there could be more than one reader that changed state,
					// keep these events in the slotlist
					if (first) {
						*ph = i;
						first = 0;
					} else {
						pSlot = p11_get_slot(i);
						if (oReadersInfo->CardPresent(i))
							pSlot->ievent = 1;
						else
							pSlot->ievent = -1;
					}
				}
			}
		} else
			ret = CKR_NO_EVENT;
	} catch (CMWException e) {
		CLEANUP(cal_translate_error(WHERE, e.GetError()));
	} catch (...) {
		lRet = -1;
		log_trace(WHERE, "E: unknown exception thrown");
		CLEANUP(CKR_FUNCTION_FAILED);
	}
cleanup:

	return (ret);
}
#undef WHERE

#define WHERE "cal_map_status()"
int cal_map_status(tCardStatus calstatus) {
	switch (calstatus) {
	case CARD_INSERTED:
		return (P11_CARD_INSERTED);
	case CARD_NOT_PRESENT:
		return (P11_CARD_NOT_PRESENT);
	case CARD_STILL_PRESENT:
		return (P11_CARD_STILL_PRESENT);
	case CARD_REMOVED:
		return (P11_CARD_REMOVED);
	case CARD_OTHER:
		return (P11_CARD_OTHER);
	default:
		return (-1);
	}
}
#undef WHERE

int cal_translate_error(const char *WHERE, long err) {
	log_trace(WHERE, "E: MiddlewareException thrown: 0x%0x", err);

	switch (err) {
	case EIDMW_OK:
		return (0);
		break;
	/** A function parameter has an unexpected value (general) */
	case EIDMW_ERR_PARAM_BAD:
		return (CKR_FUNCTION_FAILED);
		break; // return(CKR_ARGUMENTS_BAD);
	/** A function parameter exceeded the allowed range */
	case EIDMW_ERR_PARAM_RANGE:
		return (CKR_FUNCTION_FAILED);
		break; // return(CKR_ARGUMENTS_BAD);
	/** Bad file path (invalid characters, length no multiple of 4) */
	case EIDMW_ERR_BAD_PATH:
		return (CKR_FUNCTION_FAILED);
		break;
	/** Unknown/unsupported algorithm */
	case EIDMW_ERR_ALGO_BAD:
		return (CKR_MECHANISM_INVALID);
		break;
	/** Invalid/unsupported PIN operation */
	case EIDMW_ERR_PIN_OPERATION:
		return (CKR_FUNCTION_FAILED);
		break;
	/** PIN not allowed for this card (invalid characters, too short/long) */
	case EIDMW_ERR_PIN_FORMAT:
		return (CKR_FUNCTION_FAILED);
		break;

	// Card errors
	/** Generic card error */
	case EIDMW_ERR_CARD:
		return (CKR_DEVICE_ERROR);
		break;
	/** Not Authenticated (no PIN specified) */
	case EIDMW_ERR_NOT_AUTHENTICATED:
		return (CKR_USER_NOT_LOGGED_IN);
		break;
		/** This command is not supported by this card */
	case EIDMW_ERR_NOT_SUPPORTED:
		return (CKR_DEVICE_ERROR);
		break;
	/** Bad PIN */
	case EIDMW_ERR_PIN_BAD:
		return (CKR_PIN_INCORRECT);
		break;
	/** PIN blocked */
	case EIDMW_ERR_PIN_BLOCKED:
		return (CKR_PIN_LOCKED);
		break;
	/** No card present or card has been removed */
	case EIDMW_ERR_NO_CARD:
		return (CKR_PIN_LOCKED);
		break;
	/** Bad parameter P1 or P2 */
	case EIDMW_ERR_BAD_P1P2:
		return (CKR_DEVICE_ERROR);
		break;
	/** Command not allowed */
	case EIDMW_ERR_CMD_NOT_ALLOWED:
		return (CKR_DEVICE_ERROR);
		break;
	/** File not found */
	case EIDMW_ERR_FILE_NOT_FOUND:
		return (CKR_DEVICE_ERROR);
		break;
	/** Unable to read applet version from the card */
	case EIDMW_ERR_APPLET_VERSION_NOT_FOUND:
		return (CKR_DEVICE_ERROR);
		break;

	// Reader errors
	/** Error communicating with the card */
	case EIDMW_ERR_CARD_COMM:
		return (CKR_DEVICE_ERROR);
		break;
	/** No reader has been found */
	case EIDMW_ERR_NO_READER:
		return (CKR_DEVICE_ERROR);
		break;
	/** The pinpad reader returned an error */
	case EIDMW_ERR_PINPAD:
		return (CKR_DEVICE_ERROR);
		break;
	/** A card is present but we can't connect in a normal way (e.g. SIS card) */
	case EIDMW_ERR_CANT_CONNECT:
		return (CKR_DEVICE_ERROR);
		break;

	// Internal errors (caused by the middleware)
	/** An internal limit has been reached */
	case EIDMW_ERR_LIMIT:
		return (CKR_DEVICE_ERROR);
		break;
	/** An internal check failed */
	case EIDMW_ERR_CHECK:
		return (CKR_DEVICE_ERROR);
		break;
	/** And unknown error occurred */
	case EIDMW_ERR_UNKNOWN:
		return (CKR_GENERAL_ERROR);
		break;
	/** The pinpad reader received a wrong/unknown value */
	case EIDMW_PINPAD_ERR:
		return (CKR_DEVICE_ERROR);
		break;
	/** Dynamic library couldn't be loaded (found found at the specified location) */
	case EIDMW_CANT_LOAD_LIB:
		return (CKR_GENERAL_ERROR);
		break;
	/** Memory error */
	case EIDMW_ERR_MEMORY:
		return (CKR_HOST_MEMORY);
		break;
	/** Couldn't delete cache file(s) */
	case EIDMW_ERR_DELETE_CACHE:
		return (CKR_GENERAL_ERROR);
		break;
	/** Error getting or writing config data */
	case EIDMW_CONF:
		return (CKR_GENERAL_ERROR);
		break;

	// User errors/events
	/** User pressed Cancel in PIN dialog */
	case EIDMW_ERR_PIN_CANCEL:
		return (CKR_FUNCTION_CANCELED);
		break;
	/** Pinpad timeout */
	case EIDMW_ERR_TIMEOUT:
		return (CKR_FUNCTION_CANCELED);
		break;
	/** The new PINs that were entered differ */
	case EIDMW_NEW_PINS_DIFFER:
	/** A PIN with invalid length or format was entered */
	case EIDMW_WRONG_PIN_FORMAT:
		return (CKR_FUNCTION_FAILED);
		break;

	// Parser errors
	/** Could not find expected elements in parsed ASN.1 vector */
	case EIDMW_WRONG_ASN1_FORMAT:
		return (CKR_FUNCTION_FAILED);
		break;

	// I/O errors
	// errors modelled on the definitions in errno.h
	/** File could not be opened */
	case EIDMW_FILE_NOT_OPENED:
		return (CKR_FUNCTION_FAILED);
		break;
	/** Search permission is denied for a component of the path prefix of path. */
	case EIDMW_PERMISSION_DENIED:
		return (CKR_FUNCTION_FAILED);
		break;
	/** A loop exists in symbolic links encountered during resolution of the path argument. */
	/** A component of path does not name an existing file or path is an empty string.*/
	/** A component of the path prefix is not a directory. */
	/** The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}. */
	case EIDMW_INVALID_PATH:
		return (CKR_FUNCTION_FAILED);
		break;
	/** {OPEN_MAX} file descriptors are currently open in the calling process. */
	/** Too many files are currently open in the system.*/
	case EIDMW_TOO_MANY_OPENED_FILES:
		return (CKR_FUNCTION_FAILED);
		break;
	/** Call of the Logger after destruct time */
	case EIDMW_ERR_LOGGER_APPLEAVING:
		return (CKR_FUNCTION_FAILED);
		break;

	/** This card type is unknown */
	case EIDMW_ERR_CARDTYPE_UNKNOWN:
		return (CKR_TOKEN_NOT_RECOGNIZED);
		break;
	/** No release of SDK object has been done before closing the application */
	case EIDMW_ERR_RELEASE_NEEDED:
		return (CKR_FUNCTION_FAILED);
		break;

	default:
		return (CKR_GENERAL_ERROR);
	}
}
