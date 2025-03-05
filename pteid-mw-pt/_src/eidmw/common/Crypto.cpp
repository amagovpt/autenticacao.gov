/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2025 Daniel Dron - <daniel.dron@caixamagica.pt>
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNULesser General Public License version
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

#include "Crypto.h"
#include "Log.h"
#include "Util.h"
#include "eidErrors.h"
#include <cstdio>
#include <cstring>
#include <openssl/evp.h>

namespace eIDMW {

namespace Crypto {

CipherCtx::CipherCtx() : ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free) {}

BlockCipherCtx::BlockCipherCtx(const char *cipher_name, size_t block_sz, size_t key_sz, const char *provider_name)
	: cipher(EVP_CIPHER_fetch(nullptr, cipher_name, provider_name), EVP_CIPHER_free), block_size(block_sz),
	  key_size(key_sz) {
	if (!ctx || !cipher) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Failed to initialize cipher context");
	}

	EVP_CIPHER_CTX_set_padding(ctx, 0);
}

bool BlockCipherCtx::init(const unsigned char *key, const unsigned char *iv, bool encrypt) {
	if (!key) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "null key");
	}

	static const std::array<uint8_t, 16> zero_iv = {0}; // max block size is 16
	const unsigned char *actual_iv = iv ? iv : zero_iv.data();

	return EVP_CipherInit_ex2(ctx, cipher.get(), key, actual_iv, encrypt, nullptr) > 0;
}

std::vector<uint8_t> BlockCipherCtx::processBlock(const uint8_t *data, size_t len) {
	if (!data) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Empty input");
	}

	if (len % block_size != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Input length not aligned to block size");
	}

	if (len > SIZE_MAX - block_size) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Input too large");
	}

	std::vector<uint8_t> out(len);
	int outlen = 0;

	if (!EVP_CipherUpdate(ctx, out.data(), &outlen, data, len)) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Cipher update failed");
	}

	return out;
}

CByteArray BlockCipherCtx::retailMac(const CByteArray &key, const CByteArray &input) {
	if (key.Size() != 16) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Key must be size 16 for retail mac");
	}

	if (input.Size() % MAC_KEYSIZE != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Input MUST be multiple of block size (8 bytes)");
	}

	if (input.Size() == 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Empty input not allowed");
	}

	auto key1 = key.GetBytes(0, MAC_KEYSIZE);
	auto key2 = key.GetBytes(MAC_KEYSIZE, MAC_KEYSIZE);

	CByteArray res(MAC_KEYSIZE, 0);
	CByteArray xx(MAC_KEYSIZE, 0);

	for (size_t j = 0; j < (input.Size() / MAC_KEYSIZE); j++) {
		for (size_t i = 0; i < MAC_KEYSIZE; i++)
			xx.SetByte(input.GetByte(i + j * MAC_KEYSIZE) ^ res.GetByte(i), i);

		res = BlockCipherCtx::encrypt<DesCipher>(key1.GetBytes(), xx);
	}
	res = BlockCipherCtx::decrypt<DesCipher>(key2.GetBytes(), res);
	return BlockCipherCtx::encrypt<DesCipher>(key1.GetBytes(), res);
}

MessageDigestCtx::MessageDigestCtx(const char *md_name, size_t dgst_size, const char *provider)
	: md(EVP_MD_fetch(nullptr, md_name, provider), EVP_MD_free), digest_size(dgst_size) {
	if (!ctx || !md) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Failed to initialize message digest context");
	}
}

bool MessageDigestCtx::init() { return EVP_DigestInit_ex2(ctx, md.get(), nullptr) > 0; }

bool MessageDigestCtx::update(const uint8_t *data, size_t len) { return EVP_DigestUpdate(ctx, data, len) > 0; }

CByteArray MessageDigestCtx::final() {
	std::vector<uint8_t> digest(digest_size);
	unsigned int len;
	if (!EVP_DigestFinal_ex(ctx, digest.data(), &len)) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Digest finalization failed");
	}
	return CByteArray(digest.data(), len);
}

void SecureMessagingKeys::deriveKeys(const CByteArray &seed) {
	if (seed.Size() == 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Empty seed for secure messaging key derivation");
	}

	if (seed.Size() != 16) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR,
					  "Seed for secure messaging key derivation must be 16 bytes long");
	}

	CByteArray kSeed(SHA_DIGEST_LENGTH);
	kSeed.Append(seed);
	kSeed.Append(0x00);
	kSeed.Append(0x00);
	kSeed.Append(0x00);
	kSeed.Append(0x00);

	// Enc key
	kSeed.SetByte(0x01, SHA_DIGEST_LENGTH - 1);
	auto out = MessageDigestCtx::hash<Sha1>(kSeed);
	memcpy(m_ksEnc.data(), out.GetBytes(), 16);

	kSeed.SetByte(0x02, SHA_DIGEST_LENGTH - 1);
	out = MessageDigestCtx::hash<Sha1>(kSeed);
	memcpy(m_ksMac.data(), out.GetBytes(), 16);
}

CByteArray withIso7816Padding(const CByteArray &input, size_t blockSize) {
	int padLen = blockSize - input.Size() % blockSize;

	CByteArray paddedContent;
	paddedContent.Append(input);
	paddedContent.Append(0x80);

	// Padlen - 1 because one byte was already spent with 0x80
	for (int i = 0; i < padLen - 1; i++)
		paddedContent.Append(0x00);

	return paddedContent;
}

CByteArray removeIso7816Padding(const CByteArray &input) {
	if (input.Size() == 0) {
		return input;
	}

	int i = input.Size() - 1;
	while (i >= 0 && input.GetByte(i) == 0x00) {
		i--;
	}

	if (i >= 0 && input.GetByte(i) == 0x80) {
		return CByteArray(input.GetBytes(), i);
	}

	return input;
}

void SecureMessagingKeys::upgradeKeys(const CByteArray &enc, const CByteArray &mac) {
	if (enc.Size() != 16 || mac.Size() != 16) {
		LOG_AND_THROW(LEV_ERROR, MOD_SSL, EIDMW_ERR_BAC_CRYPTO_ERROR,
					  "Seed for secure messaging key derivation must be 16 bytes long");
	}

	memcpy(m_ksEnc.data(), enc.GetBytes(), 16);
	memcpy(m_ksMac.data(), mac.GetBytes(), 16);
}

} // namespace Crypto
} // namespace eIDMW
