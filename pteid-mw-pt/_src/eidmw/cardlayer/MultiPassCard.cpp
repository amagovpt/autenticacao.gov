#include "MultiPassCard.h"
#include <openssl/des.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

/* Constructor for MultiPass cards in CL mode */
CMultiPassCard::CMultiPassCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol)
	: CIcaoCard(hCard, poContext, poPinpad, protocol) {
	setProtocol(protocol);
	m_cardType = CARD_MULTIPASS;
	m_pace.reset(nullptr);
}

typedef struct {
	unsigned char rnd_ifd[8];
	unsigned char rnd_icc[8];
	unsigned char kifd[16];
	unsigned char kicc[16];
	unsigned char k_enc[16];
	unsigned char k_mac[16];
} bac_keys_t;

bac_keys_t *bac_keys_new() {
	bac_keys_t *keys = (bac_keys_t*)OPENSSL_zalloc(sizeof(bac_keys_t));

	return keys;
}

sm_keys_t *sm_keys_new() {
	sm_keys_t *sm_keys = (sm_keys_t*)OPENSSL_zalloc(sizeof(bac_keys_t));

	return sm_keys;
}

void sm_keys_delete(sm_keys_t *sm_keys) { OPENSSL_clear_free(sm_keys, sizeof(sm_keys_t)); }

void bac_keys_delete(bac_keys_t *keys) { OPENSSL_clear_free(keys, sizeof(bac_keys_t)); }


void generateAccessKeys(const CByteArray &mrzInfo, bac_keys_t *out) {

	unsigned char k_seed[SHA_DIGEST_LENGTH];
	unsigned char k_seed_digest[SHA_DIGEST_LENGTH];

	// Key seed is the hash of MRZ
	SHA1(mrzInfo.GetBytes(), mrzInfo.Size(), k_seed);

	// Only the 16 most significant bytes are used from kseed so we can overwrite
	// the last 4
	k_seed[SHA_DIGEST_LENGTH - 4] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 3] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 2] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(out->k_enc, k_seed_digest, 16);

	k_seed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(out->k_mac, k_seed_digest, 16);
}

int generate3DesKeysFromKeySeed(const unsigned char *key_seed_16, size_t key_seed_len, unsigned char *ks_enc,
								unsigned char *ks_mac) {

	unsigned char k_seed[SHA_DIGEST_LENGTH];
	unsigned char k_seed_digest[SHA_DIGEST_LENGTH];
	if (key_seed_len != 16) {
		fprintf(stderr, "Illegal input for %s: should be a 16-byte key_seed\n", __FUNCTION__);
		return 1;
	}
	memcpy(k_seed, key_seed_16, 16);
	// Only the 16 most significant bytes are used from kseed so we can overwrite
	// the last 4
	k_seed[SHA_DIGEST_LENGTH - 4] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 3] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 2] = 0x00;
	k_seed[SHA_DIGEST_LENGTH - 1] = 0x01;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(ks_enc, k_seed_digest, 16);

	k_seed[SHA_DIGEST_LENGTH - 1] = 0x02;

	SHA1(k_seed, SHA_DIGEST_LENGTH, k_seed_digest);
	memcpy(ks_mac, k_seed_digest, 16);
}

const int MAC_KEYSIZE = 8;

CByteArray decrypt_data_3des(CByteArray &key, CByteArray &in) {
	CByteArray plain_text;
	unsigned char *out = (unsigned char *)malloc(in.Size());
	EVP_CIPHER_CTX *ctx = NULL;
	unsigned int len = 0;

	if (key.Size() == 0) {
		fprintf(stderr, "decrypt_data_3des(): Empty key!\n");
		goto err;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		goto err;

	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (unsigned char *)key.GetBytes(), NULL, 0) ||
		!EVP_CIPHER_CTX_set_padding(ctx, 0))
		goto err;

	if (EVP_DecryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
		fprintf(stderr, "Error in decrypt_data_3des() !\n");
	}

	if (len != in.Size()) {
		fprintf(stderr, "decrypt_data_3des() Error: len < in.size()\n");
	}

	plain_text = CByteArray((const unsigned char *)out, (unsigned long)len);
	free(out);
	return plain_text;

err:
	free(out);
	return CByteArray();
}

CByteArray paddedByteArray(CByteArray &input) {
	int padLen = 8 - input.Size() % 8;

	CByteArray paddedContent;
	paddedContent.Append(input);
	paddedContent.Append(0x80);

	// Padlen - 1 because one byte was already spent with 0x80
	for (int i = 0; i < padLen - 1; i++)
		paddedContent.Append(0x00);

	return paddedContent;
}

// FIXME: THIS IS ONE FOR CARD, CANT BE GLOBAL
uint64_t g_ssc = 0;

#define MAC_KEYSIZE 8
#define MAC_LEN 8

CByteArray sm_retail_mac_des(CByteArray &key, CByteArray &mac_input, uint64_t ssc) {
	CByteArray macInputWithSSC;
	CByteArray secondKey = key.GetBytes(MAC_KEYSIZE, MAC_KEYSIZE);

	unsigned char ssc_block[] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char xx[MAC_KEYSIZE];
	unsigned char des_out[MAC_KEYSIZE];
	unsigned char *msg = NULL;
	DES_key_schedule ks_a;
	DES_key_schedule ks_b;
	size_t i, j;

	CByteArray in;

	// SSC is an 8-byte counter that serves as the first block of the MAC input
	for (i = 0; i < sizeof(ssc_block); i++)
		ssc_block[i] = (0xFF & ssc >> (7 - i) * 8);

	// unsigned char output[8];
	macInputWithSSC.Append(ssc_block, sizeof(ssc_block));
	macInputWithSSC.Append(mac_input);

	msg = macInputWithSSC.GetBytes();

	memset(des_out, 0, sizeof(des_out));

	DES_set_key_unchecked((const_DES_cblock *)key.GetBytes(), &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)secondKey.GetBytes(), &ks_b);

	for (j = 0; j < (macInputWithSSC.Size() / MAC_KEYSIZE); j++) {
		// fprintf(stderr, "Running DES_ENCRYPT iteration: %d\n", j);
		for (i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ des_out[i];

		DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);
	}

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_b, 0);

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);

	CByteArray result(des_out, MAC_KEYSIZE);

	return result;
}

CByteArray encrypt_data_3des(CByteArray &key, CByteArray &in) {
	CByteArray cipher_text;
	unsigned char *out = (unsigned char *)malloc(in.Size());
	EVP_CIPHER_CTX *ctx = NULL;
	unsigned int len = 0;

	if (key.Size() == 0) {
		fprintf(stderr, "encrypt_data_3des(): Empty key!\n");
		goto err;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		goto err;

	// Null as the 5th param means that IV is empty
	//  1 as last param means encryption
	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (unsigned char *)key.GetBytes(), NULL, 1) ||
		!EVP_CIPHER_CTX_set_padding(ctx, 0))
		goto err;

	if (EVP_EncryptUpdate(ctx, out, (int *)&len, in.GetBytes(), in.Size()) == 0) {
		fprintf(stderr, "Error in encrypt_data_3des() !\n");
	}

	if (len != in.Size()) {
		fprintf(stderr, "encrypt_data_3des() error: len < in.size()\n");
	}

	cipher_text = CByteArray((const unsigned char *)out, (unsigned long)len);
	free(out);
	return cipher_text;

err:
	free(out);
	return CByteArray();
}

CByteArray buildSecureAPDU(CByteArray &plaintext_apdu, CByteArray &encryption_key, CByteArray &mac_key) {
	CByteArray final_apdu;
	const unsigned char Tcg = 0x87;
	const unsigned char Tcc = 0x8e;
	const unsigned char Tle = 0x97;
	const unsigned char paddingIndicator = 0x01;

	const unsigned char controlByte = 0x0C;

	// Contains data
	if (plaintext_apdu.Size() > 5) {

		CByteArray command_header(plaintext_apdu.GetBytes(0, 4));
		// Mark the APDU as SM
		command_header.SetByte(command_header.GetByte(0) | controlByte, 0);

		CByteArray input_data(plaintext_apdu.GetBytes(5, plaintext_apdu.Size() - 5));
		CByteArray paddedInput = paddedByteArray(input_data);

		CByteArray cryptogram = encrypt_data_3des(encryption_key, paddedInput);
		// LCg = Len(Cg) + Len(PI = 1)
		int lcg = cryptogram.Size() + 1;
		CByteArray inputForMac = paddedByteArray(command_header);

		CByteArray paddedCryptogram;
		paddedCryptogram.Append(Tcg);
		// This is safe because Lcg will be always lower than
		paddedCryptogram.Append((unsigned char)lcg);
		paddedCryptogram.Append(paddingIndicator);
		paddedCryptogram.Append(cryptogram);

		paddedCryptogram = paddedByteArray(paddedCryptogram);

		// Padded Command Header + Padded [Tcg Lcg PI,CG]
		inputForMac.Append(paddedCryptogram);

		// Pre-increment SSC
		g_ssc++;

		CByteArray mac = sm_retail_mac_des(mac_key, inputForMac, g_ssc);

		// Build final APDU including Cryptogram and MAC
		int lc_final = MAC_LEN + 2 + cryptogram.Size() + 3; // CG + Tcg + Lcg + PI
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(Tcg);
		final_apdu.Append((unsigned char)lcg);
		final_apdu.Append(paddingIndicator);
		final_apdu.Append(cryptogram);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le

		return final_apdu;
	} else {

		// For commands sending no data the format should be
		// XC INS P1 P2 Lc Tle 01 Le Tcc Lcc MAC
		CByteArray TlvLe;
		CByteArray command_header(plaintext_apdu.GetBytes(0, 4));
		command_header.SetByte(command_header.GetByte(0) | controlByte, 0);
		int le = plaintext_apdu.GetByte(4);
		CByteArray inputForMac = paddedByteArray(command_header);

		TlvLe.Append(Tle);
		TlvLe.Append(0x01);
		TlvLe.Append(le);

		inputForMac.Append(paddedByteArray(TlvLe));

		// Pre-increment SSC
		g_ssc++;

		CByteArray mac = sm_retail_mac_des(mac_key, inputForMac, g_ssc);

		int lc_final = MAC_LEN + 2 + TlvLe.Size();
		final_apdu.Append(command_header);
		final_apdu.Append((unsigned char)lc_final);
		final_apdu.Append(TlvLe);
		final_apdu.Append(Tcc);
		final_apdu.Append(MAC_LEN); // Lcc
		final_apdu.Append(mac);
		final_apdu.Append(0x00); // Le

		return final_apdu;
	}
}

int triple_des_cbc_nopadding(const unsigned char *enc_input, size_t len, const unsigned char *k_enc, unsigned char *out,
							 int encrypt_mode) {
	unsigned char zero_iv[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	int outl = 0;
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		goto cleanup;
	}
	EVP_CIPHER_CTX_init(ctx);
	if (!EVP_CipherInit_ex(ctx, EVP_des_ede_cbc(), NULL, (const unsigned char *)k_enc, zero_iv, encrypt_mode)) {
		goto cleanup;
	}
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	if (!EVP_CipherUpdate(ctx, out, &outl, enc_input, len)) {
		goto cleanup;
	}
	fprintf(stderr, "DBG: %s %d bytes\n", encrypt_mode ? "Encrypted" : "Decrypted", outl);
	return outl;

cleanup:
	EVP_CIPHER_CTX_free(ctx);
	return outl;
}

unsigned char *retail_mac_des(const unsigned char *key, const unsigned char *mac_input, size_t mac_input_len) {

	unsigned char xx[MAC_KEYSIZE];
	unsigned char *des_out = reinterpret_cast<unsigned char *>(malloc(MAC_KEYSIZE));
	unsigned char padding[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	if (mac_input_len % MAC_KEYSIZE != 0) {
		fprintf(stderr, "Warning: input data is not multiple of cipher block size!\n");
	}

	unsigned char *msg = NULL;
	DES_key_schedule ks_a;
	DES_key_schedule ks_b;
	const unsigned char *key_a = key;
	const unsigned char *key_b = key + 8;

	// Padding method 2 will always add some padding bytes
	size_t input_len = mac_input_len + MAC_KEYSIZE;
	msg = reinterpret_cast<unsigned char *>(malloc(input_len));

	memcpy(msg, mac_input, mac_input_len);
	memcpy(msg + mac_input_len, padding, sizeof(padding));

	memset(des_out, 0, MAC_KEYSIZE);

	DES_set_key_unchecked((const_DES_cblock *)key_a, &ks_a);
	DES_set_key_unchecked((const_DES_cblock *)key_b, &ks_b);

	for (size_t j = 0; j < (input_len / MAC_KEYSIZE); j++) {
		for (size_t i = 0; i < MAC_KEYSIZE; i++)
			xx[i] = msg[i + j * MAC_KEYSIZE] ^ des_out[i];

		DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);
	}

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_b, 0);

	memcpy(xx, des_out, MAC_KEYSIZE);
	DES_ecb_encrypt((const_DES_cblock *)xx, (DES_cblock *)des_out, &ks_a, 1);

	return des_out;
}

bac_keys_t *generateBacData(const CByteArray &mrzInfo, const CByteArray random, unsigned char *bac_data) {
	const int CIPHER_KEY_SIZE = 16;

	bac_keys_t *bac_keys = bac_keys_new();

	// Generate Access Keys
	generateAccessKeys(mrzInfo, bac_keys);

	// Copy random ICC to bac keys
	memcpy(bac_keys->rnd_icc, random.GetBytes(), random.Size());

	// Generate random IFD and KIFD
	RAND_bytes(bac_keys->rnd_ifd, 8);
	RAND_bytes(bac_keys->kifd, 16);

	unsigned char enc_input[32] = {0};
	memcpy(enc_input, bac_keys->rnd_ifd, 8);
	memcpy(enc_input + 8, random.GetBytes(), 8);
	memcpy(enc_input + 16, bac_keys->kifd, 16);

	// Encrypt SIFD = (RND.IFD || RND.ICC || KIFD) and generate MAC
	int outl = triple_des_cbc_nopadding(enc_input, sizeof(enc_input), bac_keys->k_enc, bac_data, 1);

	unsigned char *mac = retail_mac_des(bac_keys->k_mac, bac_data, outl);
	memcpy(bac_data + 32, mac, MAC_KEYSIZE);

	return bac_keys;
}

sm_keys_t generateSessionKeysAndSSC(bac_keys_t *bac_keys) {
	unsigned char *rnd_icc = bac_keys->rnd_icc;
	unsigned char *rnd_ifd = bac_keys->rnd_ifd;

	sm_keys_t sm_keys = {};
	unsigned char *ssc = &(sm_keys.ssc[0]);
	// Initial value of SSC = 4LSB(Rnd.ICC) + 4LSB(Rnd.IFD)
	memcpy(ssc, rnd_icc + 4, 4);
	memcpy(ssc + 4, rnd_ifd + 4, 4);

	unsigned char kseed[16] = {0}; // Kseed = XOR of Kicc and Kifd
	for (int i = 0; i < sizeof(bac_keys->kifd); i++) {
		kseed[i] = bac_keys->kifd[i] ^ bac_keys->kicc[i];
	}
	generate3DesKeysFromKeySeed(kseed, sizeof(kseed), sm_keys.ks_enc, sm_keys.ks_mac);

	return sm_keys;
}

bool checkMacInResponseAPDU(CByteArray &resp, CByteArray &mac_key) {
	CByteArray receivedMac;
	CByteArray paddedInput;

	/*
		Deal with 2 different formats of response APDUs: with just SW12 or also with data
	*/
	if (resp.GetByte(0) == 0x99) {
		receivedMac = (resp.GetBytes(6, 8));
		CByteArray inputForMac = resp.GetBytes(0, 4);
		paddedInput = paddedByteArray(inputForMac);

	} else if (resp.GetByte(0) == 0x87 || resp.GetByte(0) == 0x81) {
		resp.Chop(2);
		int mac_offset = resp.Size() - 8;
		int lcg = resp.GetByte(1) == 0x81 ? resp.GetByte(2) : resp.GetByte(1);
		CByteArray inputForMac = resp.GetByte(1) == 0x81 ? resp.GetBytes(0, lcg + 3) : resp.GetBytes(0, lcg + 2);
		receivedMac = (resp.GetBytes(mac_offset, 8));
		paddedInput = paddedByteArray(inputForMac);
	}

	// Pre-increment SSC
	g_ssc++;

	CByteArray mac = sm_retail_mac_des(mac_key, paddedInput, g_ssc);
	return mac.Equals(receivedMac);
}

void dumpByteArray(unsigned char *ba, size_t len) {
	for (size_t i = 0; i < len; i++)
		printf("%02X ", ba[i] & 0xFF);

	puts("\n");
}

int check_icc_bac_data(bac_keys_t *bac_keys, const unsigned char *icc_bac_data, size_t icc_data_len) {
	if (icc_data_len < 40) {
		fprintf(stderr, "Wrong BAC output from card command!\n");
		return 1;
	}
	unsigned char decrypted_icc_data[40] = {0};
	const size_t MAC_OFFSET = icc_data_len - MAC_KEYSIZE;
	// Check ICC MAC
	unsigned char *mac = retail_mac_des(bac_keys->k_mac, icc_bac_data, icc_data_len - MAC_KEYSIZE);
	// MAC is after 32 bytes of encrypted data
	const unsigned char *icc_mac = icc_bac_data + MAC_OFFSET;
	if (memcmp(mac, icc_mac, MAC_KEYSIZE) != 0) {
		fprintf(stderr, "%s: Failed to verify ICC MAC!\n", __FUNCTION__);
		return 1;
	}
	// Check values in decrypted data
	int outl = triple_des_cbc_nopadding(icc_bac_data, sizeof(decrypted_icc_data) - MAC_KEYSIZE, bac_keys->k_enc,
										decrypted_icc_data, 0);
	fprintf(stderr, "DBG: decrypted ICC bytes: %d\n", outl);
	int match_rndicc = memcmp(bac_keys->rnd_icc, decrypted_icc_data, 8);
	int match_rndifd = memcmp(bac_keys->rnd_ifd, decrypted_icc_data + 8, 8);
	fprintf(stderr, "DBG: compare Rnd.ICC: %d\n", match_rndicc);
	fprintf(stderr, "DBG: compare Rnd.IFD: %d\n", match_rndifd);

	if (match_rndicc + match_rndifd != 0) {
		return 1;
	}

	// Store Kicc to derive keys
	memcpy(bac_keys->kicc, decrypted_icc_data + 16, 16);

	return 0;
}

uint64_t bigEndianBytesToLong(const uint8_t *bytes, size_t length) {
    long value = 0;
    for (size_t i = 0; i < length; i++) {
        value = (value << 8) | bytes[i];  // Shift existing value and add next byte
    }
    return value;
}

void CMultiPassCard::openBACChannel(const CByteArray &mrzInfo) {
	auto icc_random = GetRandom();
	unsigned char ifd_bac_data[40] = {0};

	unsigned char icc_bac_data[40] = {0}; // returned by the BAC command from the card

	bac_keys_t* bac_keys = generateBacData(mrzInfo, icc_random, ifd_bac_data);

	// ------------------
	// Bac authenticate
	unsigned char mutual_auth[] = {0x00, 0x82, 0x00, 0x00, 0x28};
	const size_t cmd_len = sizeof(ifd_bac_data) + sizeof(mutual_auth) + 1;

	unsigned char* full_command = (unsigned char*) malloc(cmd_len);
	memcpy(full_command, mutual_auth, sizeof(mutual_auth));
	memcpy((full_command + sizeof(mutual_auth)), ifd_bac_data, sizeof(ifd_bac_data));
	full_command[cmd_len - 1] = 0x00; // Le

	auto resp = SendAPDU(CByteArray{full_command, cmd_len});
	assert(getSW12(resp.GetBytes(resp.Size() - 2)) == 0x9000);
	assert(resp.Size() - 2 == sizeof(icc_bac_data));

	memcpy(icc_bac_data, resp.GetBytes(), resp.Size());

	// -----------------
	// Check ICC
	unsigned char decrypted_icc_data[40] = {0};
	const size_t MAC_OFFSET = sizeof(icc_bac_data) - MAC_KEYSIZE;
	// Check ICC MAC
	unsigned char *mac = retail_mac_des(bac_keys->k_mac, icc_bac_data, sizeof(icc_bac_data) - MAC_KEYSIZE);
	// MAC is after 32 bytes of encrypted data
	const unsigned char *icc_mac = icc_bac_data + MAC_OFFSET;
	if (memcmp(mac, icc_mac, MAC_KEYSIZE) != 0) {
		fprintf(stderr, "%s: Failed to verify ICC MAC!\n", __FUNCTION__);
		assert(false);
	}
	// Check values in decrypted data
	int outl = triple_des_cbc_nopadding(icc_bac_data, sizeof(decrypted_icc_data) - MAC_KEYSIZE, bac_keys->k_enc,
										decrypted_icc_data, 0);
	fprintf(stderr, "DBG: decrypted ICC bytes: %d\n", outl);
	int match_rndicc = memcmp(bac_keys->rnd_icc, decrypted_icc_data, 8);
	int match_rndifd = memcmp(bac_keys->rnd_ifd, decrypted_icc_data + 8, 8);
	fprintf(stderr, "DBG: compare Rnd.ICC: %d\n", match_rndicc);
	fprintf(stderr, "DBG: compare Rnd.IFD: %d\n", match_rndifd);

	if (match_rndicc + match_rndifd != 0) {
		assert(false);
	}

	// Store Kicc to derive keys
	memcpy(bac_keys->kicc, decrypted_icc_data + 16, 16);


	// -----------------
	// Generate session keys and SSC
	unsigned char *rnd_icc = bac_keys->rnd_icc;
	unsigned char *rnd_ifd = bac_keys->rnd_ifd;

	m_sm_keys = sm_keys_new();
	unsigned char *ssc = &(m_sm_keys->ssc[0]);
	// Initial value of SSC = 4LSB(Rnd.ICC) + 4LSB(Rnd.IFD)
	memcpy(ssc, rnd_icc + 4, 4);
	memcpy(ssc + 4, rnd_ifd + 4, 4);

	unsigned char kseed[16] = {0}; // Kseed = XOR of Kicc and Kifd
	for (int i = 0; i < sizeof(bac_keys->kifd); i++) {
		kseed[i] = bac_keys->kifd[i] ^ bac_keys->kicc[i];
	}
	generate3DesKeysFromKeySeed(kseed, sizeof(kseed), m_sm_keys->ks_enc, m_sm_keys->ks_mac);

	assert(m_sm_keys);
}

CByteArray CMultiPassCard::readToken() {
	CByteArray encryption_key = {m_sm_keys->ks_enc, sizeof(m_sm_keys->ks_enc)};
	CByteArray mac_key = {m_sm_keys->ks_mac, sizeof(m_sm_keys->ks_mac)};

	g_ssc = bigEndianBytesToLong(m_sm_keys->ssc, sizeof(m_sm_keys->ssc));
	printf("Initial SSC: %lu\n", g_ssc);
	/* SELECT EF.DG13 */
	CByteArray plaintext_apdu("00A4020C02010D", true);
	auto sm_apdu = buildSecureAPDU(plaintext_apdu, encryption_key, mac_key);

	printf("[SC] SM started: select APDU\n");
	auto resp1 = SendAPDU(sm_apdu);

	if (!checkMacInResponseAPDU(resp1, mac_key)) {
		fprintf(stderr, "APDU 1 - MAC verification failed\n");
	}
	else {
		printf("DBG: [SC] SM started: read binary APDU\n");

		CByteArray read_binary("00B0000000", true);
		auto sm_apdu2 = buildSecureAPDU(read_binary, encryption_key, mac_key);
		auto resp2 = SendAPDU(sm_apdu2);

		CByteArray cryptogram(resp2.GetBytes(3, resp2.GetByte(1)-1));

		// Decrypt a block of file data
		CByteArray dg13_data = decrypt_data_3des(encryption_key, cryptogram);
		printf("DBG: Multipass token data:\n");
		dumpByteArray(dg13_data.GetBytes(), dg13_data.Size());
		
		return dg13_data; 
	}

	return {};
}

CByteArray CMultiPassCard::GetRandom() {
	APDU apdu;
	apdu.ins() = 0x84;
	apdu.p1() = 0x00;
	apdu.p2() = 0x00;
	apdu.setLe(0x08);
	return SendAPDU(apdu);
}

tCardType CMultiPassCard::GetType() { return m_cardType; }