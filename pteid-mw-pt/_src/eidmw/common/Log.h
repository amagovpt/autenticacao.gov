/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
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
   TEMPORARY - logfile hardcoded to "."	in MWLOG_Init()
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


*/
#include "Export.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "LogBase.h"
#include "MWException.h"

namespace eIDMW {

typedef enum {
	LEV_NOLOG, // This must not be given in MWLOG() !
	LEV_CRIT,
	LEV_ERROR,
	LEV_WARN,
	LEV_INFO,
	LEV_DEBUG,
} tLevel;

typedef enum {
	MOD_CAL,  // CardLayer
	MOD_P11,  // PKCS11
	MOD_LIB,  // eID lib
	MOD_GUI,  // GUI
	MOD_TA,	  // Tray applet
	MOD_DLG,  // Dialog
	MOD_CSP,  // CSP
	MOD_APL,  // Application layer
	MOD_SSL,  // Open SSL operation
	MOD_CRL,  // CRL service
	MOD_CMD,  // CMD
	MOD_SDK,  // SDK
	MOD_TEST, // Unit test
	MOD_KSP,  // KSP
	MOD_SCAP, // SCAP
} tModule;

/* Format string checking for printf wrapper functions is not available on MSVC */
#ifdef _WIN32
  #define FORMAT_CHECK_FROM_3RD_ARG 
#else
  #define FORMAT_CHECK_FROM_3RD_ARG __attribute__((format(printf, 3, 4)))
#endif

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define MWLOG_CTX(level, mod, format, ...)                                                                             \
	MWLOG(level, mod, "[%s:%s:%d] " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/**
 * Log.
 * Example:
 *          MWLOG(LEV_ERROR, MOD_P11, "Invalid session handle %d\n", handle);
 */
EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, const wchar_t *format, ...);

EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, const char *format, ...) FORMAT_CHECK_FROM_3RD_ARG ;

/**
 * Log.
 * Example:
 *          MWLOG(LEV_ERROR, theException);
 */
EIDMW_CMN_API bool MWLOG(tLevel level, tModule mod, CMWException theException);

} // namespace eIDMW
