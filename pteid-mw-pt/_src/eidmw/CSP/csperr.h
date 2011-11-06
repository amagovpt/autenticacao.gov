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
/**
 * The middleware function throw CMWEXCEPTIONs (and perhaps others)
 * that have to be converted to a returnvalue (TRUE/FALSE) while
 * GetLastError() should return the error code.
 * To avoid obfuscating the code with try-catch blocks that are the
 * some for all function, we just provide the __CSP_TRY__ and
 * __CSP_CATCH_SETLASTERROR__ macros.
 * Because logging seems to overwrite the GetLastError() value,
 * we first store this value before logging using  __STORE_LASTERROR__
 * and do a SetLastError() afterwards in __CSP_CATCH_SETLASTERROR__.
 */

#ifndef __CSPERR_H__
#define __CSPERR_H__

#include "../common/Log.h"
#include "../common/MWException.h"

#define __CSP_TRY__ try {

#define __STORE_LASTERROR__ \
	if (!bReturnVal && dwLastError == 0) \
		dwLastError = GetLastError();

#define __CSP_CATCH_SETLASTERROR__ \
	} \
	catch(CMWException & e) \
	{ \
		MWLOG(LEV_ERROR, MOD_CSP, L"ERR: caught exception 0x%0x", e.GetError()); \
		dwLastError = mwerror2win(e.GetError()); \
		bReturnVal = FALSE; \
	} \
	catch (...) \
	{ \
		MWLOG(LEV_ERROR, MOD_CSP, L"ERR: caught unknown exception"); \
		dwLastError = NTE_FAIL; \
		bReturnVal = FALSE; \
	} \
	SetLastError(dwLastError);

/** Middleware errors -> Windows errors */
long mwerror2win(unsigned long ulMwErr);

#endif
