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
#include <openssl/evp.h>

namespace eIDMW {

CipherCtx::CipherCtx() : ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free) {}

BlockCipherCtx::BlockCipherCtx(const char *cipher_name, size_t block_sz, size_t key_sz)
	: cipher(EVP_CIPHER_fetch(nullptr, cipher_name, nullptr), EVP_CIPHER_free), block_size(block_sz), key_size(key_sz) {
	if (!ctx || !cipher) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Failed to initialize cipher context");
	}

	EVP_CIPHER_CTX_set_padding(ctx, 0);
}

bool BlockCipherCtx::init(const unsigned char *key, const unsigned char *iv, bool encrypt) {
	if (!key) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "null key");
	}

	static const std::array<uint8_t, 16> zero_iv = {0}; // max block size is 16
	const unsigned char *actual_iv = iv ? iv : zero_iv.data();

	return EVP_CipherInit_ex2(ctx, cipher.get(), key, actual_iv, encrypt, nullptr) > 0;
}

std::vector<uint8_t> BlockCipherCtx::processBlock(const uint8_t *data, size_t len) {
	if (!data) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Empty input");
	}

	if (len % block_size != 0) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Input length not aligned to block size");
	}

	if (len > SIZE_MAX - block_size) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Input too large");
	}

	std::vector<uint8_t> out(len);
	int outlen = 0;

	if (!EVP_CipherUpdate(ctx, out.data(), &outlen, data, len)) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR, "Cipher update failed");
	}

	return out;
}

std::vector<uint8_t> BlockCipherCtx::decrypt(unsigned char *key, const uint8_t *data, size_t len,
										  const unsigned char *iv) {
	TripleDesCipher cipher;
	cipher.init(key, nullptr, false); // decrypt mode
	auto decrypted = cipher.processBlock(data, len);

	if (len != decrypted.size()) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR,
					  "Input length and decrypted length do not match (%ld != %ld)", len, decrypted.size());
	}

	return decrypted;
}

std::vector<uint8_t> BlockCipherCtx::encrypt(unsigned char *key, const uint8_t *data, size_t len,
										  const unsigned char *iv) {
	TripleDesCipher cipher;
	cipher.init(key, nullptr, true); // encrypt mode
	auto decrypted = cipher.processBlock(data, len);

	if (len != decrypted.size()) {
		LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR,
					  "Input length and encrypted length do not match (%ld != %ld)", len, decrypted.size());
	}

	return decrypted;
}

} // namespace eIDMW
