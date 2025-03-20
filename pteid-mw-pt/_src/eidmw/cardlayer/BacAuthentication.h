/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2025 Daniel Dron - <daniel.dron@caixamagica.pt>
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

#pragma once

#include "SecureMessaging.h"
#include "ByteArray.h"
#include "Context.h"
#include "Crypto.h"
#include <cstdint>

struct eac_ctx;

namespace eIDMW {

/**
 * @class BacAuthentication
 * @brief Basic Access Control implementation for eID secure messaging
 *
 * Implements the Basic Access Control (BAC) protocol used in the Portuguese
 * multi-pass feature for citizen cards. BAC establishes a secure channel with
 * session keys for protected communication between the card and terminal.
 *
 * @details This class handles encryption, decryption, and MAC verification
 * for secure communication according to the BAC protocol specification.
 * Communication uses 3DES encryption and ISO/IEC 9797-1 MAC algorithm 3.
 *
 * @see SecureMessaging
 */
class BacAuthentication : public SecureMessaging {
public:
	BacAuthentication(SCARDHANDLE hCard, CContext *poContext, const void *paramStructure);
	~BacAuthentication();

	/**
	 * @brief Establishes the BAC secure channel with the card
	 *
	 * @param mrzInfo MRZ data
	 * @throws EIDMW_ERR_BAC_* on authentication failure
	 */
	void authenticate(const CByteArray &mrzInfo);

protected:
	virtual CByteArray encryptData(const CByteArray &data) override;
	virtual CByteArray decryptData(const CByteArray &encryptedData) override;
	virtual CByteArray computeMac(const CByteArray &data) override;
	virtual CByteArray addPadding(const CByteArray &data) override;
	virtual CByteArray removePadding(const CByteArray &data) override;
	virtual void incrementSSC() override;

private:
	CByteArray getRandomFromCard();
	bool checkMacInResponse(CByteArray &response);
	CByteArray retailMacWithSSC(const CByteArray &macInput, uint64_t ssc);
	CByteArray retailMacWithPadding(const CByteArray &key, const CByteArray &macInput);

	Crypto::SecureMessagingKeys m_sm;
	eac_ctx *m_ctx = nullptr;
};
} // namespace eIDMW
