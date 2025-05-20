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

#include "Export.h"
#include "ByteArray.h"
#include "Log.h"
#include "MWException.h"
#include "eidErrors.h"
#include <array>
#include <cstdint>
#include <memory>
#if defined(__WINCRYPT_H__)
#undef X509_NAME
#undef X509_EXTENSIONS
#undef X509_CERT_PAIR
#undef PKCS7_ISSUER_AND_SERIAL
#undef PKCS7_SIGNER_INFO
#endif
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
class EIDMW_CMN_API CipherCtx {
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
class EIDMW_CMN_API BlockCipherCtx {
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

		return {decrypted.data(), (unsigned long)decrypted.size()};
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

		return {decrypted.data(), (unsigned long)decrypted.size()};
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
 * @brief Base class for message digest context management
 */
class EIDMW_CMN_API HashCtx {
protected:
	std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx;

public:
	HashCtx() : ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free) {}
	virtual ~HashCtx() = default;

	HashCtx(const HashCtx &) = delete;
	HashCtx &operator=(const HashCtx &) = delete;

	HashCtx(HashCtx &&) = default;
	HashCtx &operator=(HashCtx &&) = default;

	operator EVP_MD_CTX *() { return ctx.get(); }
};

/**
 * @brief Base class for cryptographic hash operations
 */
class EIDMW_CMN_API MessageDigestCtx {
protected:
	HashCtx ctx;
	std::unique_ptr<EVP_MD, decltype(&EVP_MD_free)> md;
	const size_t digest_size;

public:
	/**
	 * @brief Initialize digest with algorithm params
	 * @param md_name openssl digest name (e.g. "SHA256")
	 * @param dgst_size expected size of digest output
	 * @param provider optional provider name for openssl 3.0
	 * @throws EIDMW_ERR_BAC_CRYPTO_ERROR if init fails
	 */
	MessageDigestCtx(const char *md_name, size_t dgst_size, const char *provider = nullptr);

	/**
	 * @brief setup digest context
	 * @return true if initialization succeeded
	 */
	bool init();

	/**
	 * @brief update digest with new data
	 * @param data pointer to input data
	 * @param len length of input
	 * @return true if update succeeded
	 */
	bool update(const uint8_t *data, size_t len);

	/**
	 * @brief finalize and get digest
	 * @return CByteArray containing the digest
	 * @throws EIDMW_ERR_BAC_CRYPTO_ERROR if finalization fails
	 */
	CByteArray final();

	/**
	 * @brief convenience method to hash a complete buffer
	 * @param data input buffer
	 * @return digest of input
	 */
	template <typename T> static CByteArray hash(const CByteArray &input) {
		T md;
		md.init();
		md.update(input.GetBytes(), input.Size());
		return md.final();
	}
};

class Sha256 : public MessageDigestCtx {
public:
	static constexpr size_t DIGEST_SIZE = 32;
	Sha256() : MessageDigestCtx("SHA256", DIGEST_SIZE) {}
};

class Sha1 : public MessageDigestCtx {
public:
	static constexpr size_t DIGEST_SIZE = 20;
	Sha1() : MessageDigestCtx("SHA1", DIGEST_SIZE) {}
};

/**
 * @brief Implements generic ICAO 9303 key derivation and secure messaging state
 *
 * Handles generation and management of session keys for BAC/PACE/CA protocols
 * per ICAO 9303 part 11:
 *   1. append 4 zero bytes to input seed
 *   2. derive Kenc using counter 0x01
 *   3. derive Kmac using counter 0x02
 *
 * Maintains send sequence counter (SSC) state for secure messaging.
 * Supports both 3DES (16 bytes for K1||K2) and AES-based implementations.
 */
class EIDMW_CMN_API SecureMessagingKeys {
public:
	void deriveKeys(const CByteArray &seed);

	CByteArray getCBAEncKey() const { return {m_ksEnc.data(), KEY_SIZE }; };
	CByteArray getCBAMacKey() const { return {m_ksMac.data(), KEY_SIZE }; };

	void upgradeKeys(const CByteArray &enc, const CByteArray &mac);

	void setSSC(uint64_t newSsc) { m_ssc = newSsc; }
	uint64_t getSSC() const { return m_ssc; }
	void incrementSSC() { m_ssc++; }

private:
	const static unsigned long KEY_SIZE = 16;
	std::array<unsigned char, KEY_SIZE> m_ksEnc;
	std::array<unsigned char, KEY_SIZE> m_ksMac;
	uint64_t m_ssc = {0};
};

/**
 * Adds iso7816-4 padding to input (0x80 + zeroes) up to block boundary
 *
 * @param input      source data to pad
 * @param blockSize  target block size (default DES_BLOCK_SIZE)
 * @return          new CByteArray, length will be multiple of blockSize
 */
EIDMW_CMN_API CByteArray withIso7816Padding(const CByteArray &input, size_t blockSize = DES_BLOCK_SIZE);

/**
 * Removes iso7816-4 padding from input (0x80 + zeroes)
 *
 * @param input      source data to remove padding from
 * @return          new CByteArray
 */
EIDMW_CMN_API CByteArray removeIso7816Padding(const CByteArray &input);

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

// Base template class for unique reference wrapper types
template <typename T> class Ref;

template <> class Ref<BUF_MEM> {
public:
	Ref() = default;

	static Ref takeOwnership(BUF_MEM *buf) {
		auto ref = Ref();
		ref.m_buf = buf;
		return ref;
	}

	explicit Ref(const CByteArray &byteArray) : Ref() {
		if (byteArray.Size() > 0) {
			BUF_MEM_grow(m_buf, byteArray.Size());
			memcpy(m_buf->data, byteArray.GetBytes(), byteArray.Size());
		}
	}

	~Ref() {
		if (m_buf) {
			BUF_MEM_free(m_buf);
		}
	}

	// Move constructor
	Ref(Ref &&other) noexcept : m_buf(other.m_buf) { other.m_buf = nullptr; }

	// Delete copy constructor and assignment
	Ref(const Ref &) = delete;
	Ref &operator=(const Ref &) = delete;
	Ref &operator=(Ref &&) = delete;

	BUF_MEM *operator->() const { return m_buf; }
	operator BUF_MEM *() const { return m_buf; }

	CByteArray toByteArray() const { return CByteArray((unsigned char *)m_buf->data, m_buf->length); }

private:
	BUF_MEM *m_buf = nullptr;
};

// Create a non-owning BUF_MEM view of a CByteArray
// WARNING:
// - The CByteArray must remain valid while BUF_MEM is in use
// - Do not pass to non-const parameters
inline BUF_MEM bufMemView(const CByteArray &array) {
	BUF_MEM view = {0};
	view.data = (char *)array.GetBytes();
	view.length = array.Size();
	view.max = array.Size();
	return view;
}

} // namespace Crypto

} // namespace eIDMW
