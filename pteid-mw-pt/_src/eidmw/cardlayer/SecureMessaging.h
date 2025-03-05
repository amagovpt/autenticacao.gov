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

#include "Context.h"
#include <openssl/types.h>

namespace eIDMW {
	
struct CAParams;

/**
 * @class SecureMessaging
 * @brief Abstract base class defining secure messaging protocol interface
 *
 * Provides the foundation for secure communication channels with smart cards
 * using various secure messaging protocols. Implementations should handle
 * command encryption, response decryption, and message authentication.
 *
 * @note Child classes must implement the sendSecureAPDU methods
 * @see BacAuthentication
 */
class SecureMessaging {
public:
	SecureMessaging(SCARDHANDLE hCard, CContext *poContext, const void *paramStructure)
		: m_card(hCard), m_context(poContext), m_param(paramStructure), m_authenticated(false) {}

	virtual bool isInitialized() { return m_authenticated; };

	/**
	 * @brief Sends an APDU through a secure channel
	 *
	 * @param apdu The command to send (will be encrypted)
	 * @param retValue Stores the return code from card transmission
	 * @return CByteArray Decrypted response with status code appended at the end
	 */
	virtual CByteArray sendSecureAPDU(const APDU &apdu, long &retValue) = 0;

	/**
	 * @brief sends raw APDU bytes through a secure channel
	 *
	 * @param apdu The raw command bytes to send (will be encrypted)
	 * @param retValue Stores the return code from card transmission
	 * @return CByteArray Decrypted response with status code appended at the end
	 */
	virtual CByteArray sendSecureAPDU(const CByteArray &apdu, long &retValue) = 0;

	virtual void upgradeKeys(EVP_PKEY *eph_pkey, BUF_MEM *shared_secret, CByteArray enc, CByteArray mac,
							 const CAParams &params) = 0;

protected:
	SCARDHANDLE m_card = {0};
	const void *m_param = {0};
	CContext *m_context = {0};

	bool m_authenticated = false;
};

} // namespace eIDMW
