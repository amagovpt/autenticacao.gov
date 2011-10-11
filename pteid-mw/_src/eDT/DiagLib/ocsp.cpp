/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#ifdef WIN32
#include <windows.h>
#elif __APPLE__
#include "Mac/mac_helper.h"
#endif

#include "ocsp.h"
#include "error.h"
#include "log.h"
#include "util.h"

#include "pteidlib.h"


////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int ocspIsAvailable(Card_ID id, bool *available)
{	
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = false;

	const PTeidlibWrapper *pteidlib = NULL;
	
	if(NULL == (pteidlib = loadPTeidLibWrapper()))
	{
		LOG_ERROR(L"loadPTeidLibWrapper failed");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	
	return pteidlib->pteidlibIsOcspAvailable(id, available);
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
