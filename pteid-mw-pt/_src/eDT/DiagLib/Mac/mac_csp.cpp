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
#include "csp.h"
#include "error.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int cspIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available=false;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cspRegisterCertificates(Card_ID id)
{
	int iReturnCode = DIAGLIB_OK;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cspTestSign(Card_ID id, Cert_TYPE cert, bool *succeed)
{
	int iReturnCode = DIAGLIB_OK;

	if(succeed==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*succeed=false;

	return RETURN_LOG_ERROR(DIAGLIB_ERR_NOT_AVAILABLE);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
