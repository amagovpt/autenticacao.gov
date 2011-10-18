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

#ifdef _WIN32
#include <windows.h>
#include <scarderr.h>
#else
#include <pcsclite.h>
#endif

const char* pcscerr2string(long err)
{
	switch (err) {
	case SCARD_S_SUCCESS:
		return "Command successful.";
	case SCARD_E_CANCELLED:
		return "Command cancelled.";
	case SCARD_E_CANT_DISPOSE:
		return "Cannot dispose handle.";
	case SCARD_E_INSUFFICIENT_BUFFER:
		return "Insufficient buffer.";
	case SCARD_E_INVALID_ATR:
		return "Invalid ATR.";
	case SCARD_E_INVALID_HANDLE:
		return "Invalid handle.";
	case SCARD_E_INVALID_PARAMETER:
		return "Invalid parameter given.";
	case SCARD_E_INVALID_TARGET:
		return "Invalid target given.";
	case SCARD_E_INVALID_VALUE:
		return "Invalid value given.";
	case SCARD_E_NO_MEMORY:
		return "Not enough memory.";
	case SCARD_F_COMM_ERROR:
		return "RPC transport error.";
	case SCARD_F_INTERNAL_ERROR:
		return "Unknown internal error.";
	case SCARD_F_UNKNOWN_ERROR:
		return "Unknown internal error.";
	case SCARD_F_WAITED_TOO_LONG:
		return "Waited too long.";
	case SCARD_E_UNKNOWN_READER:
		return "Unknown reader specified.";
	case SCARD_E_TIMEOUT:
		return "Command timeout.";
	case SCARD_E_SHARING_VIOLATION:
		return "Sharing violation.";
	case SCARD_E_NO_SMARTCARD:
		return "No smartcard inserted.";
	case SCARD_E_UNKNOWN_CARD:
		return "Unknown card.";
	case SCARD_E_PROTO_MISMATCH:
		return "Card protocol mismatch.";
	case SCARD_E_NOT_READY:
		return "Subsystem not ready.";
	case SCARD_E_SYSTEM_CANCELLED:
		return "System cancelled.";
	case SCARD_E_NOT_TRANSACTED:
		return "Transaction failed.";
	case SCARD_E_READER_UNAVAILABLE:
		return "Reader/s is unavailable.";
	case SCARD_W_UNSUPPORTED_CARD:
		return "Card is not supported.";
	case SCARD_W_UNRESPONSIVE_CARD:
		return "Card is unresponsive.";
	case SCARD_W_UNPOWERED_CARD:
		return "Card is unpowered.";
	case SCARD_W_RESET_CARD:
		return "Card was reset.";
	case SCARD_W_REMOVED_CARD:
		return "No card present in reader.";
//	case SCARD_W_INSERTED_CARD:
//		return "Card was inserted.";
//		break;
//	case SCARD_E_UNSUPPORTED_FEATURE:
//		return "Feature not supported.";
//		break;
	case SCARD_E_PCI_TOO_SMALL:
		return "PCI struct too small.";
	case SCARD_E_READER_UNSUPPORTED:
		return "Reader is unsupported.";
	case SCARD_E_DUPLICATE_READER:
		return "Reader already exists.";
	case SCARD_E_CARD_UNSUPPORTED:
		return "Card is unsupported.";
	case SCARD_E_NO_SERVICE:
		return "Service not available.";
	case SCARD_E_SERVICE_STOPPED:
		return "Service was stopped.";
	case 0x8010002f:
		return "SCARD_E_COMM_DATA_LOST";
#ifdef _WIN32
	case ERROR_INSUFFICIENT_BUFFER:
		return "ERROR_INSUFFICIENT_BUFFER.";
	case ERROR_INVALID_PARAMETER:
		return "ERROR_INVALID_PARAMETER.";
#endif
	default:
		return "Unknown error.";
	}
	;
}
