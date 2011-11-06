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
#include "csperr.h"
#include "../common/eidErrors.h"

long mwerror2win(unsigned long ulMwErr)
{
	switch(ulMwErr)
	{
	case EIDMW_ERR_PARAM_BAD:
	case EIDMW_ERR_PARAM_RANGE:
	case EIDMW_ERR_ALGO_BAD:
		return NTE_BAD_UID;
	case EIDMW_ERR_MEMORY:
		return NTE_NO_MEMORY;
	case EIDMW_ERR_PIN_CANCEL:
		return SCARD_W_CANCELLED_BY_USER;
	case EIDMW_ERR_TIMEOUT:
		return WAIT_TIMEOUT;
	case EIDMW_ERR_NO_READER:
		return SCARD_E_NO_READERS_AVAILABLE;
	case EIDMW_ERR_CARD_COMM:
		return SCARD_F_COMM_ERROR;
	case EIDMW_ERR_PIN_BAD:
		return SCARD_W_WRONG_CHV;
	case EIDMW_ERR_PIN_BLOCKED:
		return SCARD_W_CHV_BLOCKED;
	default:
		// If it's another eIDMW error, return NTE_FAIL
		// If it would be another error (shouldn't occur)
		// then just return this value..
		if ((ulMwErr / 0x00100000) == 0xe1d)
			return NTE_FAIL;
		else
			return (long) ulMwErr;
	}
}
