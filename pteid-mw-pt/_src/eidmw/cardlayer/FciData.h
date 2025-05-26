/* ****************************************************************************

 * PT eID Middleware Project.
 * Copyright (C) 2024 André Guerreiro - <aguerreiro1985@gmail.com>
 */
#pragma once

#include "ByteArray.h"

namespace eIDMW {

/* Extract EF file length from FCI data: file length tag can be up to 4 bytes long
   This function will search for tag 0x80 and then tag 0x81 if 0x80 is not found
 */
int extractEFSize(CByteArray &fci_data);

} // namespace eIDMW