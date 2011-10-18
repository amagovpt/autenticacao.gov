/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

#include "../common/ByteArray.h"
#include "../common/Export.h"
#include "EmulationCard.h"

namespace eIDMW
{

EIDMW_PCSCEMU_API void SetReaders(unsigned long ulCount);

EIDMW_PCSCEMU_API CEmulationCard *SetCard(unsigned long ulReader, CEmulationCard *poCard);

EIDMW_PCSCEMU_API void SetCardPresent(unsigned long ulReader, bool bPresent);

}
