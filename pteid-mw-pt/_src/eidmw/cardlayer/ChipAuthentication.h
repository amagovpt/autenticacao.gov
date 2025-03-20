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
#include "Reader.h"
#include "SecureMessaging.h"
#include <eac/eac.h>
#include <openssl/types.h>

namespace eIDMW {

struct CAParams {
	const EVP_MD *kdf_md; // MD used for key derivation
	size_t key_size;
	int nid;
	const EVP_CIPHER *cipher;
};

class ChipAuthSecureMessaging : public SecureMessaging {
public:
	ChipAuthSecureMessaging(SCARDHANDLE hCard, CContext *poContext, const void *paramStructure);
	bool authenticate(SecureMessaging *sm, EVP_PKEY *pkey, ASN1_OBJECT *oid);

protected:
	virtual CByteArray encryptData(const CByteArray &data) override;
	virtual CByteArray decryptData(const CByteArray &encryptedData) override;
	virtual CByteArray computeMac(const CByteArray &data) override;
	virtual CByteArray addPadding(const CByteArray &data) override;
	virtual CByteArray removePadding(const CByteArray &data) override;
	virtual void incrementSSC() override;

private:
	void initEACContext(EVP_PKEY *eph_pkey, BUF_MEM *shared_secret, CByteArray enc, CByteArray mac,
						const CAParams &params);

	EAC_CTX *m_ctx;
};

} // namespace eIDMW
