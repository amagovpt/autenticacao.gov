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

#include "ByteArray.h"
#include "Context.h"
#include "Crypto.h"
#include <cstdint>

namespace eIDMW {

class BacAuthentication {
public:
	BacAuthentication(CContext *poContext);
	~BacAuthentication();

	void authenticate(SCARDHANDLE hCard, const void *paramStructure, const CByteArray &mrzInfo);
	CByteArray sendSecureAPDU(const CByteArray &apdu);
	CByteArray decryptData(const CByteArray &data);

private:
	CByteArray sendAPDU(const CByteArray &apdu, long &returnValue);
	CByteArray getRandomFromCard();
	bool checkMacInResponse(CByteArray &response);
	CByteArray retailMacWithSSC(const CByteArray &macInput, uint64_t ssc);
	CByteArray retailMacWithPadding(const CByteArray &key, const CByteArray &macInput);

	CContext *m_context = {0};

	// temporary state set after calling `authenticate`
	SCARDHANDLE m_card = {0};
	const void *m_param = {0};

	bool m_authenticated = false;
	Crypto::SecureMessagingKeys m_sm;
};
} // namespace eIDMW
