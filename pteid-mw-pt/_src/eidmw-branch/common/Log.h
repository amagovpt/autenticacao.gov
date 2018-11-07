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
/*

TODO:
   TEMPORARY - logfile hardcoded to "."	in MWLOG_Init() !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


*/
#include "Export.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "LogBase.h"
#include "MWException.h"

namespace eIDMW
{

typedef enum {
    LEV_NOLOG,   // This must not be given in MWLOG() !
    LEV_CRIT,
    LEV_ERROR,
    LEV_WARN,
    LEV_INFO,
    LEV_DEBUG,
} tLevel;

typedef enum {
    MOD_CAL,   // CardLayer
    MOD_P11,   // PKCS11
    MOD_LIB,   // eID lib
    MOD_GUI,   // GUI
    MOD_TA ,   // Tray applet
    MOD_DLG,   // Dialog
    MOD_CSP,   // CSP
    MOD_APL,   // Application layer
    MOD_SSL,   // Open SSL operation
    MOD_CRL,   // CRL service
    MOD_CMD,   // CMD
    MOD_SDK,   // SDK
    MOD_TEST,  // Unit test
} tModule;

/**
 * Log.
 * Example:
 *          MWLOG(LEV_ERROR, MOD_P11, "Invalid session handle %d\n", handle);
 */
EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, const wchar_t *format, ...);

EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, const char *format, ...);

/**
 * Log.
 * Example:
 *          MWLOG(LEV_ERROR, theException);
 */
EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, CMWException theException);


}
