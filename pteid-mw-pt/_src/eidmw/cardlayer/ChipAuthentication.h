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
#include "SecureMessaging.h"

namespace eIDMW {

struct CAParams {
	const EVP_MD *kdf_md; // MD used for key derivation
	size_t key_size;
	int nid;
	const EVP_CIPHER *cipher;
};

class ChipAuthentication {
public:
	bool upgradeSecureMessaging(SecureMessaging *sm, EVP_PKEY *pkey, ASN1_OBJECT *oid);
};

} // namespace eIDMW