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
#include "MWException.h"
#include "eidErrors.h"
#include <cstdint>
#include <memory>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <vector>

namespace eIDMW {

namespace Crypto {

static constexpr size_t MAC_KEYSIZE = 8;

static constexpr size_t DES_BLOCK_SIZE = 8;
static constexpr size_t AES_BLOCK_SIZE = 16;

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
	BlockCipherCtx(const char *cipher_name, size_t block_sz, size_t key_sz, const char *provider = nullptr);
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
	static CByteArray decrypt(const unsigned char *key, const CByteArray &input, const unsigned char *iv = nullptr) {
		T cipher;
		cipher.init(key, nullptr, false); // decrypt mode
		auto decrypted = cipher.processBlock(input.GetBytes(), input.Size());

		if (input.Size() != decrypted.size()) {
			MWLOG_CTX(LEV_ERROR, MOD_SSL, "Input length and decrypted length do not match (%ld != %ld)", input.Size(),
					  decrypted.size());
			throw CMWEXCEPTION(EIDMW_ERR_BAC_CRYPTO_ERROR);
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
	static CByteArray encrypt(const unsigned char *key, const CByteArray &input, const unsigned char *iv = nullptr) {
		T cipher;
		cipher.init(key, nullptr, true); // encrypt mode
		auto decrypted = cipher.processBlock(input.GetBytes(), input.Size());

		if (input.Size() != decrypted.size()) {
			MWLOG_CTX(LEV_ERROR, MOD_SSL, "Input length and encrypted length do not match (%ld != %ld)", input.Size(),
					  decrypted.size());
			throw CMWEXCEPTION(EIDMW_ERR_BAC_CRYPTO_ERROR);
		}

		return {decrypted.data(), decrypted.size()};
	}

	/**
	 * @brief ICAO 9303 BAC Retail MAC (ISO/IEC 9797-1 Algorithm 3) implementation
	 * @param key 16-byte key (K1||K2) from BAC key derivation
	 * @param macInput concatenated challenge data, MUST be block-aligned (8-byte blocks)
	 * @return 8-byte MAC value for BAC authentication
	 */
	static CByteArray retailMac(const CByteArray &key, const CByteArray &input);
};

/**
 * Adds iso7816-4 padding to input (0x80 + zeroes) up to block boundary
 *
 * @param input      source data to pad
 * @param blockSize  target block size (default DES_BLOCK_SIZE)
 * @return          new CByteArray, length will be multiple of blockSize
 */
CByteArray withIso7816Padding(const CByteArray &input, size_t blockSize = DES_BLOCK_SIZE);

/**
 * @brief Triple des
 */
class TripleDesCipher : public BlockCipherCtx {
public:
	TripleDesCipher() : BlockCipherCtx("DES-EDE-CBC", MAC_KEYSIZE, DES_BLOCK_SIZE) {}
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
	DesCipher() : BlockCipherCtx("des-cbc", MAC_KEYSIZE, DES_BLOCK_SIZE) {}
};

} // namespace Crypto

} // namespace eIDMW
