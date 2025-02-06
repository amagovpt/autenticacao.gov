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

#pragma once

#include "ByteArray.h"
#include "Log.h"
#include "eidErrors.h"
#include <cstdint>
#include <memory>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <vector>

namespace eIDMW {

/**
 * @brief Base class for Cipher context
 */
class CipherCtx {
protected:
	std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx;

public:
	CipherCtx();
	virtual ~CipherCtx() = default;

	CipherCtx(const CipherCtx &) = delete;
	CipherCtx &operator=(const CipherCtx &) = delete;

	CipherCtx(CipherCtx &&) = default;
	CipherCtx &operator=(CipherCtx &&) = default;

	operator EVP_CIPHER_CTX *() { return ctx.get(); }
};

/**
 * @brief Base class for block cipher operations
 */
class BlockCipherCtx {
protected:
	CipherCtx ctx;
	std::unique_ptr<EVP_CIPHER, decltype(&EVP_CIPHER_free)> cipher;
	const size_t block_size;
	const size_t key_size;

public:
	/**
	 * @brief Init cipher with algorithm params
	 * @throws EIDMW_ERR_BAC_CRYPTO_ERROR if init fails
	 */
	BlockCipherCtx(const char *cipher_name, size_t block_sz, size_t key_sz);
	virtual ~BlockCipherCtx() = default;

	BlockCipherCtx(const BlockCipherCtx &) = delete;
	BlockCipherCtx &operator=(const BlockCipherCtx &) = delete;

	BlockCipherCtx(BlockCipherCtx &&) = default;
	BlockCipherCtx &operator=(BlockCipherCtx &&) = default;

	/**
	 * @brief Setup for encrypt/decrypt
	 * @param key MUST be key_size bytes
	 * @param iv iv or nullptr for zero
	 */
	bool init(const unsigned char *key, const unsigned char *iv, bool encrypt);

	/**
	 * @brief Process a block
	 * @throws EIDMW_ERR_BAC_CRYPTO_ERROR on size mismatch/nulls
	 */
	std::vector<uint8_t> processBlock(const uint8_t *data, size_t len);

	/**
	 * @brief Encrypts blob of data based on constructed cipher
	 * @param key MUST be key_size bytes
	 * @param data pointer to data to be encrypted
	 * @param len size of data
	 * @param iv iv or nullptr for zero
	 */
	template <typename T>
	static CByteArray decrypt(const unsigned char *key, const uint8_t *data, size_t len,
							  const unsigned char *iv = nullptr) {
		T cipher;
		cipher.init(key, nullptr, false); // decrypt mode
		auto decrypted = cipher.processBlock(data, len);

		if (len != decrypted.size()) {
			LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR,
						  "Input length and decrypted length do not match (%ld != %ld)", len, decrypted.size());
		}

		return {decrypted.data(), decrypted.size()};
	}

	/**
	 * @brief Decrypts blob of data based on constructed cipher
	 * @param key MUST be key_size bytes
	 * @param data pointer to data to be decrypted
	 * @param len size of data
	 * @param iv iv or nullptr for zero
	 */
	template <typename T>
	static CByteArray encrypt(const unsigned char *key, const uint8_t *data, size_t len,
							  const unsigned char *iv = nullptr) {
		T cipher;
		cipher.init(key, nullptr, true); // encrypt mode
		auto decrypted = cipher.processBlock(data, len);

		if (len != decrypted.size()) {
			LOG_AND_THROW(LEV_ERROR, MOD_CAL, EIDMW_ERR_BAC_CRYPTO_ERROR,
						  "Input length and encrypted length do not match (%ld != %ld)", len, decrypted.size());
		}

		return {decrypted.data(), decrypted.size()};
	}
};

/**
 * @brief Triple des
 */
class TripleDesCipher : public BlockCipherCtx {
public:
	TripleDesCipher() : BlockCipherCtx("DES-EDE-CBC", 8, 16) {}
};

/**
 * @brief Aes256
 */
class AesCipher : public BlockCipherCtx {
public:
	AesCipher() : BlockCipherCtx("AES-256-CBC", 16, 32) {}
};

class DesCipher : public BlockCipherCtx {
public:
	DesCipher() : BlockCipherCtx("des-cbc", 8, 8) {}
};

} // namespace eIDMW
