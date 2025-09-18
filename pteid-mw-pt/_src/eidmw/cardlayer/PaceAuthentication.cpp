#include "PaceAuthentication.h"

#include "ChipAuthentication.h"

#include "ByteArray.h"
#include "Log.h"
#include "Util.h"
#include "eidErrors.h"

#include "eac/eac.h"
#include "eac/pace.h"
#include "eac/objects.h"

#include "Log.h"
#include "Crypto.h"

#include <mutex>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <openssl/asn1.h>
#include <openssl/objects.h>

#include <cassert>
#include <climits>

namespace eIDMW {

using namespace Crypto;

class PaceAuthenticationImpl {

	BUF_MEM *findObjectMem(const CByteArray &array, long tag) {
		long size = 0;
		const unsigned char *desc_data = findASN1Object(array, size, tag);
		if (desc_data == NULL)
			return NULL;

		BUF_MEM *mem = BUF_MEM_new();
		mem->data = (char *)malloc(size * sizeof(char));
		memcpy(mem->data, desc_data, size);
		mem->length = size;
		mem->max = mem->length;
		return mem;
	}

public:
	explicit PaceAuthenticationImpl(CContext *poContext)
		: m_context(poContext), m_secret(NULL), m_ctx(NULL), m_secretLen(0) {}

	CByteArray buildSetPaceAPDU(int protocol_nid) {
		unsigned char setPace[] = {0x00, 0x22, 0xC1, 0xA4, 0x0F, 0x80};
		CByteArray apdu_cmd(setPace, sizeof(setPace));
		unsigned char *protocol_oid_der = NULL;
		int protocol_oid_len = 0;

		// Which PACE algorithm should we use ?
		ASN1_OBJECT *pace_algo = OBJ_nid2obj(protocol_nid);
		protocol_oid_len = i2d_ASN1_OBJECT(pace_algo, &protocol_oid_der);

		if (protocol_oid_len > 0 && protocol_oid_len < 256) {
			unsigned char data_len = protocol_oid_len - 2;
			apdu_cmd.Append(data_len);
			apdu_cmd.Append(protocol_oid_der + 2, data_len);
		} else {
			MWLOG(LEV_ERROR, MOD_CAL, "%s: Failed to set PACE algorithm from protocol_nid: %d!", __FUNCTION__,
				  protocol_nid);
		}

		apdu_cmd.Append(0x83); // Pace password parameter
		apdu_cmd.Append(0x01);
		apdu_cmd.Append(m_secretType == PaceSecretType::PACEMRZ ? 0x01 : 0x02);

		ASN1_OBJECT_free(pace_algo);
		free(protocol_oid_der);

		return apdu_cmd;
	}

	void logSelectedPACEProtocol() {
		int protocol_nid = m_ctx->pace_ctx->protocol;
		auto protocol_obj = OBJ_nid2obj(protocol_nid);
		char protocol_name[100] = {0};
		OBJ_obj2txt(protocol_name, sizeof(protocol_name), protocol_obj, 0);
		MWLOG(LEV_DEBUG, MOD_CAL, "Selected PACE algorithm name: %s NID: %d", protocol_name, protocol_nid);
	}

	void initAuthentication(PTEID_CardHandle &hCard, PTEID_CardProtocol param_structure) {
		std::lock_guard<std::mutex> guard(m_mutex);
		BUF_MEM *mappingData = NULL, *cardMappingData = NULL, *pubkey = NULL, *cardPubKey = NULL;
		BUF_MEM *token = NULL, *cardToken = NULL;
		int r = 0;

		// EAC_init();
		m_ctx = EAC_CTX_new();

		// Select MF to cancel out any previous application selection due to EF.CardAccess read permissions
		long fileReturn = 0;
		CByteArray selectMf(SELECT_MF, sizeof(SELECT_MF));
		m_context->m_oCardInterface->Transmit(hCard, selectMf, &fileReturn, param_structure);

		CByteArray oCmd(8);
		oCmd.Append(SELECT_ADF, sizeof(SELECT_ADF));
		const unsigned char SIZE_AND_EF_ACCESS[] = {0x02, 0x01, 0x01C, 0x00};
		oCmd.Append(SIZE_AND_EF_ACCESS, sizeof(SIZE_AND_EF_ACCESS));
		m_context->m_oCardInterface->Transmit(hCard, oCmd, &fileReturn, param_structure);
		const unsigned char READ_BINARY[] = {0x00, 0xB0, 0x9C, 0x00, 0x00};
		CByteArray readBinary(READ_BINARY, sizeof(READ_BINARY));
		CByteArray readBinEFAccess =
			m_context->m_oCardInterface->Transmit(hCard, readBinary, &fileReturn, param_structure);
		// Discard SW12 from received data
		readBinEFAccess.Chop(2);

		if (!m_ctx || !EAC_CTX_init_ef_cardaccess(readBinEFAccess.GetBytes(), readBinEFAccess.Size(), m_ctx) ||
			!m_ctx->pace_ctx) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't process EF.CardAccess");
			throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
		}

		if (m_ctx->pace_ctx == NULL) {
			std::string cardAccess_hexdump = readBinEFAccess.ToString(true, false);
			MWLOG(LEV_ERROR, MOD_CAL,
				  "EAC_CTX_init_ef_cardaccess failed to initialize PACE_CTX. CardAccess supplied: %s",
				  cardAccess_hexdump.c_str());
			throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
		}

		logSelectedPACEProtocol();

		PACE_CTX *pace_context = m_ctx->pace_ctx;
		if (pace_context == NULL) {
			MWLOG(LEV_ERROR, MOD_CAL, "PACE context not initialized!");
			throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
		}

		s_type secretType = (s_type)m_secretType;
		PACE_SEC *pace_secret = PACE_SEC_new(m_secret, m_secretLen, secretType);

		if (!pace_secret) {
			MWLOG(LEV_ERROR, MOD_CAL, "Invalid secret provided for PACE authentication!");
			throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
		}

		const unsigned char finalAuth[] = {0x00, 0x86, 0x00, 0x00, 0x0C, 0x7C, 0x0A, 0x85, 0x08};
		CByteArray setPaceAuth = buildSetPaceAPDU(pace_context->protocol);
		CByteArray sendMapData;
		CByteArray responseMappingData;
		CByteArray sendEphePubKey;
		CByteArray responseEphePubKey;
		CByteArray verifyToken;
		CByteArray responseverifyToken;

		CByteArray cardNonceResponse =
			m_context->m_oCardInterface->Transmit(hCard, setPaceAuth, &fileReturn, param_structure);

		CByteArray encryptedNonce;
		const unsigned char authEncrypt[] = {0x10, 0x86, 0x00, 0x00};
		const unsigned char authType[] = {0x02, 0x7C, 0x00, 0x00};
		encryptedNonce.Append(authEncrypt, sizeof(authEncrypt));
		encryptedNonce.Append(authType, sizeof(authType));

		CByteArray responseEncryptedNonce =
			m_context->m_oCardInterface->Transmit(hCard, encryptedNonce, &fileReturn, param_structure);

		BUF_MEM *dec_nonce = findObjectMem(responseEncryptedNonce, 0x80);

		if (!PACE_STEP2_dec_nonce(m_ctx, pace_secret, dec_nonce)) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't decrypt card nonce");
			r = -1;
			goto err;
		}

		mappingData = PACE_STEP3A_generate_mapping_data(m_ctx);

		sendMapData.Append(authEncrypt, sizeof(authEncrypt));
		assert((mappingData->length + 4) <= UCHAR_MAX);
		sendMapData.Append((unsigned char)(mappingData->length + 4));
		sendMapData.Append(0x7C);
		sendMapData.Append((unsigned char)mappingData->length + 2);
		sendMapData.Append(0x81);
		sendMapData.Append((unsigned char)mappingData->length);
		sendMapData.Append(reinterpret_cast<unsigned char *>(mappingData->data), (unsigned long)mappingData->length);
		sendMapData.Append(0x00);

		responseMappingData = m_context->m_oCardInterface->Transmit(hCard, sendMapData, &fileReturn, param_structure);
		cardMappingData = findObjectMem(responseMappingData, 0x82);
		if (!PACE_STEP3A_map_generator(m_ctx, cardMappingData)) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't generate map");
			r = -1;
			goto err;
		}

		pubkey = PACE_STEP3B_generate_ephemeral_key(m_ctx);

		sendEphePubKey.Append(authEncrypt, sizeof(authEncrypt));
		assert((pubkey->length + 4) <= UCHAR_MAX);
		sendEphePubKey.Append((unsigned char)pubkey->length + 4);
		sendEphePubKey.Append(0x7C);
		sendEphePubKey.Append((unsigned char)pubkey->length + 2);
		sendEphePubKey.Append(0x83);
		sendEphePubKey.Append((unsigned char)pubkey->length);
		sendEphePubKey.Append(reinterpret_cast<unsigned char *>(pubkey->data), (unsigned long)pubkey->length);
		sendEphePubKey.Append(0x00);

		responseEphePubKey = m_context->m_oCardInterface->Transmit(hCard, sendEphePubKey, &fileReturn, param_structure);

		cardPubKey = findObjectMem(responseEphePubKey, 0x84);

		if (!PACE_STEP3B_compute_shared_secret(m_ctx, cardPubKey) || !PACE_STEP3C_derive_keys(m_ctx)) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't compute shared secret or derive keys");
			r = -1;
			goto err;
		}

		token = PACE_STEP3D_compute_authentication_token(m_ctx, cardPubKey);

		verifyToken.Append(finalAuth, sizeof(finalAuth));
		assert(token->length <= ULONG_MAX);
		verifyToken.Append(reinterpret_cast<unsigned char *>(token->data), (unsigned long)token->length);
		verifyToken.Append(0x00);

		responseverifyToken = m_context->m_oCardInterface->Transmit(hCard, verifyToken, &fileReturn, param_structure);
		if (responseverifyToken.Size() <= 2) {
			MWLOG(LEV_ERROR, MOD_CAL, "Error verifying generated token, perhaps wrong CAN code, error response: %s",
				  responseverifyToken.ToString().c_str());
			r = -2;
			goto err;
		}

		cardToken = findObjectMem(responseverifyToken, 0x86);

		if (!PACE_STEP3D_verify_authentication_token(m_ctx, cardToken)) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't verify authentication from card");
			r = -1;
			goto err;
		}

		if (!EAC_CTX_set_encryption_ctx(m_ctx, EAC_ID_PACE)) {
			MWLOG(LEV_ERROR, MOD_CAL, "Couldn't initialize encryption");
			r = -1;
		}

	err:
		if (pace_secret) {
			PACE_SEC_clear_free(pace_secret);
		}
		if (mappingData) {
			BUF_MEM_clear_free(mappingData);
		}
		if (pubkey) {
			BUF_MEM_clear_free(pubkey);
		}
		if (dec_nonce) {
			BUF_MEM_clear_free(dec_nonce);
		}
		if (cardMappingData) {
			BUF_MEM_clear_free(cardMappingData);
		}
		if (cardPubKey) {
			BUF_MEM_clear_free(cardPubKey);
		}
		if (token) {
			BUF_MEM_clear_free(token);
		}
		if (cardToken) {
			BUF_MEM_clear_free(cardToken);
		}
		if (r < 0) {
			EAC_CTX_clear_free(m_ctx);
			m_ctx = NULL;
			EAC_cleanup();
			if (r == -2) {
				throw CMWEXCEPTION(EIDMW_PACE_ERR_BAD_TOKEN);
			} else if (r == -1) {
				throw CMWEXCEPTION(EIDMW_PACE_ERR_UNKNOWN);
			}
		}
	}

	void setAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType) {
		if (m_secret) {
			free((void *)m_secret);
			EAC_CTX_clear_free(m_ctx);
			m_ctx = NULL;
			EAC_cleanup();
		}
		m_secret = (char *)malloc(sizeof(char) * secretLen);
		memcpy(m_secret, secret, secretLen);
		m_secretLen = secretLen;
		m_secretType = secretType;
	}

	~PaceAuthenticationImpl() {
		free(m_secret);
		if (m_ctx) {
			EAC_CTX_clear_free(m_ctx);
		}
		EAC_cleanup();
	}

private:
	char *m_secret;
	size_t m_secretLen;
	PaceSecretType m_secretType;
	friend class PaceAuthentication;
	CContext *m_context;
	EAC_CTX *m_ctx;
	std::mutex m_mutex;
};

PaceAuthentication::PaceAuthentication(PTEID_CardHandle hCard, CContext *poContext, PTEID_CardProtocol protocol)
	: SecureMessaging(hCard, poContext, protocol), m_impl(new PaceAuthenticationImpl(poContext)) {
	EAC_init();
}

PaceAuthentication::~PaceAuthentication() {}

void PaceAuthentication::initPaceAuthentication(PTEID_CardHandle &hCard, PTEID_CardProtocol protocol) {
	m_impl->initAuthentication(hCard, protocol);
	m_authenticated = true;
}

void PaceAuthentication::setAuthentication(const char *secret, size_t secretLen, PaceSecretType secretType) {
	return m_impl->setAuthentication(secret, secretLen, secretType);
}

CByteArray PaceAuthentication::encryptData(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto encrypted = Ref<BUF_MEM>::takeOwnership(EAC_encrypt(m_impl->m_ctx, &dataMem));
	return encrypted.toByteArray();
}

CByteArray PaceAuthentication::decryptData(const CByteArray &encryptedData) {
	auto dataBem = bufMemView(encryptedData);
	auto decrypted = Ref<BUF_MEM>::takeOwnership(EAC_decrypt(m_impl->m_ctx, &dataBem));
	return decrypted.toByteArray();
};

CByteArray PaceAuthentication::computeMac(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto mac = Ref<BUF_MEM>::takeOwnership(EAC_authenticate(m_impl->m_ctx, &dataMem));
	return mac.toByteArray();
};

CByteArray PaceAuthentication::addPadding(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto padded = Ref<BUF_MEM>::takeOwnership(EAC_add_iso_pad(m_impl->m_ctx, &dataMem));
	return padded.toByteArray();
};

CByteArray PaceAuthentication::removePadding(const CByteArray &data) {
	auto dataMem = bufMemView(data);
	auto unpadded = Ref<BUF_MEM>::takeOwnership(EAC_remove_iso_pad(&dataMem));
	return unpadded.toByteArray();
};

void PaceAuthentication::incrementSSC() { EAC_increment_ssc(m_impl->m_ctx); };

OID_INFO get_oid_info(int nid) {
	OID_INFO info = {};
	info.nid = nid;
	info.object = OBJ_nid2obj(nid);
	info.short_name = OBJ_nid2sn(nid);

	return info;
}

bool OID_INFO::is_valid() {
	// verify nid
	if (nid == NID_undef) {
		return false;
	}

	const char *ln = OBJ_nid2ln(nid);
	const char *sn = OBJ_nid2sn(nid);
	if (!ln && !sn) {
		return false;
	}

	// verify object
	if (object) {
		int obj_nid = OBJ_obj2nid(object);
		if (obj_nid != nid && obj_nid != NID_undef) {
			return false;
		}
	}

	// verify short name
	if (short_name) {
		if (strlen(short_name) == 0) {
			return false;
		}

		if (strcmp(short_name, "UNDEF") == 0) {
			return false;
		}
	}

	return true;
}

// Wrappers around OpenPace NIDs for Applayer
OID_INFO get_id_CA_ECDH_3DES_CBC_CBC() { return get_oid_info(NID_id_CA_ECDH_3DES_CBC_CBC); }
OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_128() { return get_oid_info(NID_id_CA_ECDH_AES_CBC_CMAC_128); }
OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_192() { return get_oid_info(NID_id_CA_ECDH_AES_CBC_CMAC_192); }
OID_INFO get_id_CA_ECDH_AES_CBC_CMAC_256() { return get_oid_info(NID_id_CA_ECDH_AES_CBC_CMAC_256); }
OID_INFO get_id_CA_DH_3DES_CBC_CBC() { return get_oid_info(NID_id_CA_DH_3DES_CBC_CBC); }
OID_INFO get_id_CA_DH_AES_CBC_CMAC_128() { return get_oid_info(NID_id_CA_DH_AES_CBC_CMAC_128); }
OID_INFO get_id_CA_DH_AES_CBC_CMAC_192() { return get_oid_info(NID_id_CA_DH_AES_CBC_CMAC_192); }
OID_INFO get_id_CA_DH_AES_CBC_CMAC_256() { return get_oid_info(NID_id_CA_DH_AES_CBC_CMAC_256); }
} // namespace eIDMW