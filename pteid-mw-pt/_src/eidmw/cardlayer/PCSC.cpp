/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2010-2011 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2013, 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>


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
#ifdef UNICODE
#undef UNICODE
#endif

// pteid-common headers
#include "Config.h"
#include "MWException.h"
#include "Log.h"
#include "Thread.h"
#include "Util.h"

// cardlayer headers
#include "PCSC.h"
#include "InternalConst.h"

#include <exception>
#include <utility>

namespace eIDMW {

#ifdef __USE_PCSC__
PTEID_CardProtocol pcscProtocolToPteid(DWORD protocol) {
	switch (protocol) {
	case SCARD_PROTOCOL_T0:
		return PTEID_CardProtocol::T0;
	case SCARD_PROTOCOL_T1:
		return PTEID_CardProtocol::T1;
	default:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}
}

CPCSC::CPCSC() {
	CConfig config;

	m_ulCardTxDelay = config.GetLong(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY);
	m_hContext = 0;
	m_iTimeoutCount = 0;
	m_iListReadersCount = 0;
}

CPCSC::~CPCSC(void) { ReleaseContext(); }

void CPCSC::EstablishContext() {
	if (m_hContext == 0) {
		SCARDCONTEXT hCtx = 0;
		long lRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);

		m_hContext = hCtx;
		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardEstablishContext(): 0x%0x", lRet);
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
	}
}

void CPCSC::ReleaseContext() {
	if (m_hContext != 0) {
		SCardReleaseContext(m_hContext);
		m_hContext = 0;
	}
}

PTEID_CardHandle CPCSC::RegisterHandle(SCARDHANDLE hCard) {
	// check if already exists
	if (m_reverse_handles.find(hCard) != m_reverse_handles.end()) {
		return m_reverse_handles[hCard];
	}

	PTEID_CardHandle handle = {0};

	// check if there is a free handle we can use
	if (m_freeHandles.size() > 0) {
		handle = {m_freeHandles.back()};
		m_freeHandles.pop();
	} else {
		handle = {this->m_lastHandle.handle++};
	}

	m_handles.insert({handle, hCard});
	m_reverse_handles.insert({hCard, handle});

	return handle;
}

SCARDHANDLE CPCSC::GetPcscHandleFrom(PTEID_CardHandle pteidHandle) {
	if (m_handles.find(pteidHandle) != m_handles.end()) {
		return m_handles[pteidHandle];
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
}

CByteArray CPCSC::ListReaders() {
	char csReaders[1024];
	DWORD dwReadersLen = sizeof(csReaders);

	LONG lRet = SCardListReaders(m_hContext, NULL, csReaders, &dwReadersLen);
	if (SCARD_S_SUCCESS != lRet && m_iListReadersCount < 6) {
		MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardListReaders(): 0x%0x try: %d", lRet, m_iListReadersCount);
		m_iListReadersCount++;
	}

	if (SCARD_S_SUCCESS == lRet) {
		m_iListReadersCount = 0;
		return CByteArray((unsigned char *)csReaders, dwReadersLen);
	} else if (SCARD_E_NO_READERS_AVAILABLE == lRet) {
		return CByteArray();
	} else {
		ReleaseContext();
		throw CMWEXCEPTION(PcscToErr(lRet));
	}
}

bool CPCSC::GetStatusChange(unsigned long ulTimeout, tReaderInfo *pReaderInfos, unsigned long ulReaderCount) {
	bool bChanged = false;

	SCARD_READERSTATEA txReaderStates[MAX_READERS];
	DWORD tChangedState[MAX_READERS];

	// Convert from tReaderInfo[] -> SCARD_READERSTATE array
	for (DWORD i = 0; i < ulReaderCount; i++) {
		txReaderStates[i].szReader = pReaderInfos[i].csReader.c_str();
		txReaderStates[i].dwCurrentState = pReaderInfos[i].ulEventState;
		txReaderStates[i].cbAtr = 0;
		txReaderStates[i].pvUserData = 0;
	}

wait_again:
	LONG lRet = SCardGetStatusChange(m_hContext, ulTimeout, txReaderStates, ulReaderCount);
	if (SCARD_E_TIMEOUT != lRet) {
		if (SCARD_S_SUCCESS != lRet)
			throw CMWEXCEPTION(PcscToErr(lRet));
		// On Windows, often/always the SCARD_STATE_CHANGED is always set,
		// and in case of a remove/insert or insert/remove, you have to do a
		// second SCardGetStatusChange() to get the final reader state.
		for (DWORD i = 0; i < ulReaderCount; i++) {
#ifdef WIN32
			// There's a SCARD_STATE_EMPTY and a SCARD_STATE_PRESENT flag.
			// So we take the exor of the current and the event state for
			// both flags; if the exor isn't 0 then at least 1 of the flags
			// changed value
			DWORD exor1 = (txReaderStates[i].dwCurrentState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT)) ^
						  (txReaderStates[i].dwEventState & (SCARD_STATE_EMPTY | SCARD_STATE_PRESENT));
			bool bUnpowered = false; // Ignore this state
			//((txReaderStates[i].dwCurrentState & SCARD_STATE_UNPOWERED) == 0) &&
			//((txReaderStates[i].dwEventState & SCARD_STATE_UNPOWERED) != 0);
			tChangedState[i] = ((exor1 == 0) && !bUnpowered) ? 0 : SCARD_STATE_CHANGED;
#else
			tChangedState[i] = txReaderStates[i].dwEventState & SCARD_STATE_CHANGED;
#endif
			bChanged |= (tChangedState[i] != 0);
		}

#ifdef WIN32
		if (bChanged) {
			for (DWORD i = 0; i < ulReaderCount; i++) {
				// take previous state, reset bits that are not supported as input
				txReaderStates[i].dwCurrentState =
					(txReaderStates[i].dwEventState & ~SCARD_STATE_CHANGED & ~SCARD_STATE_UNKNOWN);
				txReaderStates[i].pvUserData = 0;
			}
			long lRet = SCardGetStatusChange(m_hContext, 0, txReaderStates, ulReaderCount);
			if (SCARD_S_SUCCESS != lRet && SCARD_E_TIMEOUT != lRet)
				throw CMWEXCEPTION(PcscToErr(lRet));
		}
#endif

		// Update the event states in pReaderInfos
		for (DWORD i = 0; i < ulReaderCount; i++) {
			pReaderInfos[i].ulCurrentState = pReaderInfos[i].ulEventState;
			// Clear and SCARD_STATE_CHANGED flag, and use tChangedState instead
			pReaderInfos[i].ulEventState = (txReaderStates[i].dwEventState & ~SCARD_STATE_CHANGED) | tChangedState[i];
		}

		// Sometimes, it seems we're getting here even without a status change,
		// so in this case we'll just go waiting again, if there's no timeout
		if (!bChanged) {
			unsigned long ulDelay = ulTimeout > 250 ? 250 : ulTimeout;
			if (ulTimeout != TIMEOUT_INFINITE)
				ulTimeout -= ulDelay;
			if (ulTimeout != 0) {
				CThread::SleepMillisecs(ulDelay);
				goto wait_again;
			}
		}
	}

	return bChanged;
}

bool CPCSC::Status(const std::string &csReader) {
	SCARD_READERSTATEA xReaderState;
	xReaderState.szReader = csReader.c_str();
	xReaderState.dwCurrentState = 0;
	xReaderState.cbAtr = 0;

	LONG lRet = SCardGetStatusChange(m_hContext, 0, &xReaderState, 1);
	if (SCARD_S_SUCCESS != lRet)
		throw CMWEXCEPTION(PcscToErr(lRet));

	return (xReaderState.dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT;
}

std::pair<PTEID_CardHandle, PTEID_CardProtocol> CPCSC::Connect(const std::string &csReader,
															   PTEID_CardProtocol preferredProtocols) {
	DWORD dwActiveProtocol;
	SCARDHANDLE hCard = 0;

	auto protocol = 0;
	switch (preferredProtocols) {
	case PTEID_CardProtocol::T0:
		protocol = SCARD_PROTOCOL_T0;
		break;
	case PTEID_CardProtocol::T1:
		protocol = SCARD_PROTOCOL_T1;
		break;
	case PTEID_CardProtocol::ANY:
#ifdef _WIN32
		protocol = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1;
#else
		protocol = SCARD_PROTOCOL_ANY;
#endif
		break;
	};

	LONG lRet = SCardConnect(m_hContext, csReader.c_str(), SCARD_SHARE_SHARED, protocol, &hCard, &dwActiveProtocol);

	MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardConnect(%ls): 0x%0x", utilStringWiden(csReader).c_str(), lRet);

	if (SCARD_E_NO_SMARTCARD == lRet)
		hCard = 0;
	else if (SCARD_S_SUCCESS != lRet)
		throw CMWEXCEPTION(PcscToErr(lRet));
	/*
	else
	{
		// XX: is this still useful ??
		// If you do an SCardTransmit() too fast after an SCardConnect(),
		// some cards/readers will return an error (e.g. 0x801002f)
		//CThread::SleepMillisecs(200);
	} */

	auto handle = this->RegisterHandle(hCard);
	return std::make_pair(handle, pcscProtocolToPteid(dwActiveProtocol));
}

void CPCSC::Disconnect(PTEID_CardHandle hCard, tDisconnectMode disconnectMode) {
	auto handle = this->GetPcscHandleFrom(hCard);

	// Remove both handles
	m_reverse_handles.erase(handle);
	m_handles.erase(hCard);

	// save freed handle
	m_freeHandles.push(hCard);

	DWORD dwDisposition = disconnectMode == DISCONNECT_RESET_CARD ? SCARD_RESET_CARD : SCARD_LEAVE_CARD;

	LONG lRet = SCardDisconnect(handle, dwDisposition);
	MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardDisconnect(0x%0x): 0x%0x ; mode: %d", handle, lRet, dwDisposition);
	if (SCARD_S_SUCCESS != lRet)
		throw CMWEXCEPTION(PcscToErr(lRet));
}

std::pair<bool, CByteArray> CPCSC::StatusWithATR(PTEID_CardHandle hCard) {
	auto handle = m_handles[hCard];
	DWORD dwReaderLen = 0;
	DWORD dwState, dwProtocol;
	unsigned char tucATR[64];
	DWORD dwATRLen = sizeof(tucATR);

	static int iStatusCount = 0;

	LONG lRet = SCardStatus(handle, NULL, &dwReaderLen, &dwState, &dwProtocol, tucATR, &dwATRLen);

	if (iStatusCount < 5 || SCARD_S_SUCCESS != lRet) {
		iStatusCount++;
		MWLOG(LEV_DEBUG, MOD_CAL, L" SCardStatus(0x%0x): 0x%0x", handle, lRet);
	}

	if (lRet != SCARD_S_SUCCESS && lRet != SCARD_W_REMOVED_CARD)
		throw CMWEXCEPTION(PcscToErr(lRet));

	return std::make_pair(SCARD_S_SUCCESS == lRet,
						  (SCARD_S_SUCCESS == lRet) ? CByteArray(tucATR, dwATRLen) : CByteArray());
}

CByteArray CPCSC::Transmit(PTEID_CardHandle hCard, const CByteArray &inputAPDU, long *plRetVal,
						   PTEID_CardProtocol protocol) {
	auto handle = m_handles[hCard];
	CByteArray oCmdAPDU(inputAPDU);

	unsigned char tucRecv[APDU_BUF_LEN];
	memset(tucRecv, 0, sizeof(tucRecv));
	DWORD dwRecvLen = sizeof(tucRecv);

	unsigned char ucINS = oCmdAPDU.Size() >= 4 ? oCmdAPDU.GetByte(1) : 0;
	unsigned long ulLen = ucINS == 0xA4 || ucINS == 0x22 ? 0xFFFFFFFF : 5;

	// const SCARD_IO_REQUEST *pioSendPci = (const SCARD_IO_REQUEST *)pSendPci;
	const SCARD_IO_REQUEST *pioSendPci = nullptr;
	switch (protocol) {
	case PTEID_CardProtocol::T0:
		pioSendPci = SCARD_PCI_T0;
		break;
	case PTEID_CardProtocol::T1:
		pioSendPci = SCARD_PCI_T1;
		break;
	case PTEID_CardProtocol::ANY:
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		break;
	}
	// SCARD_IO_REQUEST *pioRecvPci = (pRecvPci != NULL) ? (SCARD_IO_REQUEST*) pRecvPci : &m_ioRecvPci;

	// DEBUG
	// printf ("      SCardTransmit(%ls) \n", oCmdAPDU.ToWString(true, true, 0, ulLen).c_str() );

	MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardTransmit(%ls)", oCmdAPDU.ToWString(true, true, 0, ulLen).c_str());

	// On Windows we can't send APDUs with Le byte on T=0 cards so the implemented change to support T=1 is not
	// backwards-compatible !!
	if (pioSendPci->dwProtocol == SCARD_PROTOCOL_T0) {
		if (oCmdAPDU.Size() > 4 && oCmdAPDU.GetByte(4) == oCmdAPDU.Size() - 6) {
			oCmdAPDU.Chop(1);
		}
	}

	// Very strange: sometimes an SCardTransmit() returns a communications
	// error or a SW12 = 6D 00 error.
	// It occurs with most readers (some more then others) and depends heavily
	// on the type of card (e.g. nearly always with the test Kids card).
	// It seems to be fixed when adding a delay before sending something to the card...
	CThread::SleepMillisecs(m_ulCardTxDelay);

#ifdef __APPLE__
	int iRetryCount = 0;
try_again:
#endif
	LONG lRet =
		SCardTransmit(handle, pioSendPci, oCmdAPDU.GetBytes(), (DWORD)oCmdAPDU.Size(), NULL, tucRecv, &dwRecvLen);

	*plRetVal = PcscToErr(lRet);
	if (SCARD_S_SUCCESS != lRet) {
#ifdef __APPLE__
		if (SCARD_E_SHARING_VIOLATION == lRet && iRetryCount < 3) {
			iRetryCount++;
			CThread::SleepMillisecs(500);
			goto try_again;
		}
#endif
		MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): 0x%0x", lRet);

		throw CMWEXCEPTION(PcscToErr(lRet));
	}

	// Don't log the full response for privacy reasons, only SW1-SW2
	MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardTransmit(): SW12 = %02X %02X Len = %ld", tucRecv[dwRecvLen - 2],
		  tucRecv[dwRecvLen - 1], dwRecvLen);
	// DEBUG
	// printf ("SCardTransmit(): SW12 = %02X %02X\n", tucRecv[dwRecvLen - 2], tucRecv[dwRecvLen - 1]);

	// check response, and add 25 ms delay when error was returned
	if ((tucRecv[dwRecvLen - 2] != 0x90) && (tucRecv[dwRecvLen - 1] != 0x00) && (tucRecv[dwRecvLen - 2] != 0x61)) {
		CThread::SleepMillisecs(25);
	}

	return CByteArray(tucRecv, (unsigned long)dwRecvLen);
}

void CPCSC::Recover(PTEID_CardHandle hCard, unsigned long *pulLockCount) {
	auto handle = m_handles[hCard];
	// try to recover when the card is not responding (properly) anymore

	DWORD ap = 0;
	int i = 0;
	LONG lRet = SCARD_F_INTERNAL_ERROR;

	MWLOG(LEV_WARN, MOD_CAL, L"Card is not responding properly, trying to recover...");

	for (i = 0; (i < 10) && (lRet != SCARD_S_SUCCESS); i++) {
		if (i != 0)
			CThread::SleepMillisecs(100);

		lRet = SCardReconnect(handle, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0, SCARD_RESET_CARD, &ap);
		if (lRet != SCARD_S_SUCCESS) {
			MWLOG(LEV_DEBUG, MOD_CAL, L"        [%d] SCardReconnect errorcode: [0x%02X]", i, lRet);
			continue;
		}
		// transaction is lost after an SCardReconnect()
		if (*pulLockCount > 0) {
			lRet = SCardBeginTransaction(handle);
			if (lRet != SCARD_S_SUCCESS) {
				MWLOG(LEV_DEBUG, MOD_CAL, L"        [%d] SCardBeginTransaction errorcode: [0x%02X]", i, lRet);
				continue;
			}
			*pulLockCount = 1;
		}

		MWLOG(LEV_INFO, MOD_CAL, L"        Card recovered in loop %d", i);
	}
}

CByteArray CPCSC::Control(PTEID_CardHandle hCard, unsigned long ulControl, const CByteArray &oCmd,
						  unsigned long ulMaxResponseSize) {
	auto handle = m_handles[hCard];
	MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardControl(ctrl=0x%0x, %ls)", ulControl,
		  oCmd.ToWString(true, true, 0, 5).c_str());

	/* Full message: it might include prebuilt APDU with PUK and/or PIN
	// Commented out for security
	MWLOG(LEV_DEBUG, MOD_CAL, L"      SCardControl(ctrl=0x%0x, %ls)",
			ulControl, oCmd.ToWString(true, false).c_str());
	*/
	unsigned char *pucRecv = new unsigned char[ulMaxResponseSize];
	if (pucRecv == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	DWORD dwRecvLen = ulMaxResponseSize;

	long lRet = SCardControl(handle, ulControl, oCmd.GetBytes(), (DWORD)oCmd.Size(), pucRecv, dwRecvLen, &dwRecvLen);

	if (SCARD_S_SUCCESS != lRet) {
#ifndef WIN32
		// Special-casing the PIN Blocked response for GemPC Pinpad under pcsc-lite
		if (lRet == SCARD_E_NOT_TRANSACTED) {
			pucRecv[0] = 0x64;
			pucRecv[1] = 0x02;
			dwRecvLen = 2;
		} else {
#endif
			MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl() err: 0x%0x", lRet);
			delete[] pucRecv;
			throw CMWEXCEPTION(PcscToErr(lRet));
#ifndef WIN32
		}
#endif
	}

	if (dwRecvLen == 2) {
		MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl(): 2 bytes returned: 0x%02X%02X", pucRecv[0], pucRecv[1]);
	} else {
		MWLOG(LEV_DEBUG, MOD_CAL, L"        SCardControl(): %02d bytes returned", dwRecvLen);
	}

	CByteArray oResp(pucRecv, (unsigned long)dwRecvLen);
	delete[] pucRecv;

	return oResp;
}

void CPCSC::BeginTransaction(PTEID_CardHandle hCard) {
	auto handle = m_handles[hCard];
	LONG lRet = SCardBeginTransaction(handle);
	MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardBeginTransaction(0x%0x): 0x%0x", hCard, lRet);
	if (SCARD_S_SUCCESS != lRet)
		throw CMWEXCEPTION(PcscToErr(lRet));
}

void CPCSC::EndTransaction(PTEID_CardHandle hCard) {
	auto handle = m_handles[hCard];
	LONG lRet = SCardEndTransaction(handle, SCARD_LEAVE_CARD);
	MWLOG(LEV_DEBUG, MOD_CAL, L"    SCardEndTransaction(0x%0x): 0x%0x", handle, lRet);

	// Log this specific case as error, it may be useful to know if the user removed the card in the middle of an
	// operation
	if (lRet == SCARD_W_REMOVED_CARD)
		MWLOG(LEV_DEBUG, MOD_CAL, L"Smart card removed when performing SCardEndTransaction!");
}

long CPCSC::PcscToErr(unsigned long lPcscErr) {
	long lRet = EIDMW_ERR_CARD;

	switch (lPcscErr) {
	case SCARD_E_PROTO_MISMATCH:
	case SCARD_E_COMM_DATA_LOST:
	case SCARD_F_COMM_ERROR:
		lRet = EIDMW_ERR_CARD_COMM;
		break;
	case SCARD_E_INSUFFICIENT_BUFFER:
		lRet = EIDMW_ERR_PARAM_RANGE;
		break;
	case SCARD_E_INVALID_PARAMETER:
		lRet = EIDMW_ERR_PARAM_BAD;
		break;
	case SCARD_W_REMOVED_CARD:
		lRet = EIDMW_ERR_NO_CARD;
		break;
	case SCARD_E_NO_ACCESS:
		lRet = EIDMW_ERR_CMD_NOT_ALLOWED;
		break;
	case SCARD_W_UNRESPONSIVE_CARD:
	case SCARD_W_UNPOWERED_CARD:
	case SCARD_W_UNSUPPORTED_CARD:
		lRet = EIDMW_ERR_CANT_CONNECT;
		break;
	case SCARD_E_NO_SERVICE:
	case SCARD_E_SERVICE_STOPPED:
		lRet = EIDMW_ERR_NO_READER;
		break;
	case SCARD_W_RESET_CARD:
		lRet = EIDMW_ERR_CARD_RESET;
		break;
	case SCARD_E_SHARING_VIOLATION:
		lRet = EIDMW_ERR_CARD_SHARING;
		break;
	case SCARD_E_NOT_TRANSACTED:
		lRet = EIDMW_ERR_NOT_TRANSACTED;
		break;
	case 0x45d:
		lRet = EIDMW_ERR_INCOMPATIBLE_READER;
		break;
	}

	return lRet;
}
#endif

long CardInterface::SW12ToErr(unsigned long ulSW12) {
	long lRet = EIDMW_ERR_CARD;

	switch (ulSW12) {
	case 0x9000:
		lRet = EIDMW_OK;
		break;
	case 0x6400:
		lRet = EIDMW_ERR_TIMEOUT;
		break;
	case 0x6401:
		lRet = EIDMW_ERR_PIN_CANCEL;
		break;
	case 0x6402:
		lRet = EIDMW_NEW_PINS_DIFFER;
		break;
	case 0x6403:
		lRet = EIDMW_WRONG_PIN_FORMAT;
		break;
	case 0x6982:
		lRet = EIDMW_ERR_NOT_AUTHENTICATED;
		break;
	case 0x6B00:
		lRet = EIDMW_ERR_BAD_P1P2;
		break;
	case 0x6A86:
		lRet = EIDMW_ERR_BAD_P1P2;
		break;
	case 0x6986:
		lRet = EIDMW_ERR_CMD_NOT_ALLOWED;
		break;
	case 0x6A82:
		lRet = EIDMW_ERR_FILE_NOT_FOUND;
		break;
	case 0x6B80:
		lRet = EIDMW_PINPAD_ERR;
		break;
	}

	return lRet;
}

ExternalCardInterface::ExternalCardInterface(const PTEID_CardInterfaceCallbacks *callbacks) {
	if (callbacks != nullptr) {
		this->callbacks = *callbacks;
	} else {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}
}

long ExternalCardInterface::CallbackToInternalError(PTEID_CallbackResult callbackResult) {
	switch (callbackResult) {
	case PTEID_CALLBACK_OK:
		return EIDMW_OK;
	case PTEID_CALLBACK_ERR_INVALID_PARAM:
		return EIDMW_ERR_PARAM_BAD;
	case PTEID_CALLBACK_ERR_NO_CARD:
		return EIDMW_ERR_NO_CARD;
	case PTEID_CALLBACK_ERR_COMM_ERROR:
		return EIDMW_ERR_CARD_COMM;
	case PTEID_CALLBACK_ERR_NO_READER:
		return EIDMW_ERR_NO_READER;
	case PTEID_CALLBACK_ERR_ACCESS_DENIED:
		return EIDMW_ERR_CARD_SHARING;
	case PTEID_CALLBACK_ERR_NOT_SUPPORTED:
		return EIDMW_ERR_NOT_SUPPORTED;
	case PTEID_CALLBACK_ERR_BUFFER_TOO_SMALL:
		return EIDMW_ERR_PARAM_RANGE;
	case PTEID_CALLBACK_ERR_CARD_REMOVED:
		return EIDMW_ERR_NO_CARD;
	case PTEID_CALLBACK_ERR_UNRESPONSIVE:
		return EIDMW_ERR_CANT_CONNECT;
	case PTEID_CALLBACK_ERR_GENERIC:
	default:
		return EIDMW_ERR_CARD;
	}
}

void ExternalCardInterface::EstablishContext() {
	if (callbacks.establishContext == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	auto result = callbacks.establishContext(callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

void ExternalCardInterface::ReleaseContext() {
	if (callbacks.releaseContext == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}
	auto result = callbacks.releaseContext(callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

CByteArray ExternalCardInterface::ListReaders() {
	if (callbacks.listReaders == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	unsigned char buffer[1024];
	unsigned long bufferSize = sizeof(buffer);

	auto result = callbacks.listReaders(buffer, &bufferSize, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	return CByteArray(buffer, bufferSize);
}

bool ExternalCardInterface::GetStatusChange(unsigned long ulTimeout, tReaderInfo *pReaderInfos,
											unsigned long ulReaderCount) {
	throw CMWEXCEPTION(EIDMW_ERR_NOT_IMPLEMENTED);
}

bool ExternalCardInterface::Status(const std::string &csReader) {
	if (callbacks.cardPresentInReader == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	bool cardPresent;
	auto result = callbacks.cardPresentInReader(csReader.c_str(), callbacks.context, &cardPresent) != 0;
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	return cardPresent;
}

std::pair<PTEID_CardHandle, PTEID_CardProtocol> ExternalCardInterface::Connect(const std::string &csReader,
																			   PTEID_CardProtocol preferredProtocols) {

	if (callbacks.connect == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	PTEID_CardHandle outHandle = {0};
	PTEID_CardProtocol outProtocol = {};

	auto result = callbacks.connect(csReader.c_str(), &outHandle, &outProtocol, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	return std::make_pair(outHandle, outProtocol);
}

void ExternalCardInterface::Disconnect(PTEID_CardHandle hCard, tDisconnectMode) {
	if (callbacks.disconnect == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	auto result = callbacks.disconnect(hCard, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

CByteArray ExternalCardInterface::Transmit(PTEID_CardHandle hCard, const CByteArray &oCmdAPDU, long *plRetVal,
										   PTEID_CardProtocol protocol) {
	if (callbacks.transmit == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	unsigned char responseBuffer[APDU_BUF_LEN];
	unsigned long respBufferSize = sizeof(responseBuffer);

	auto result = callbacks.transmit(hCard, oCmdAPDU.GetBytes(), oCmdAPDU.Size(), responseBuffer, &respBufferSize,
									 plRetVal, protocol, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	return CByteArray(responseBuffer, respBufferSize);
}

void ExternalCardInterface::Recover(PTEID_CardHandle hCard, unsigned long *pulLockCount) {
	if (callbacks.recover == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	auto result = callbacks.recover(hCard, pulLockCount, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

CByteArray ExternalCardInterface::Control(PTEID_CardHandle hCard, unsigned long ulControl, const CByteArray &oCmd,
										  unsigned long ulMaxResponseSize) {
	if (callbacks.control == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	unsigned char *respBuffer = new unsigned char[ulMaxResponseSize];
	if (respBuffer == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);
	}

	unsigned long respBufferSize = ulMaxResponseSize;

	auto result = callbacks.control(hCard, ulControl, oCmd.GetBytes(), oCmd.Size(), respBuffer, &respBufferSize,
									ulMaxResponseSize, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	CByteArray res(respBuffer, respBufferSize);
	delete[] respBuffer;

	return res;
}

void ExternalCardInterface::BeginTransaction(PTEID_CardHandle hCard) {
	if (callbacks.beginTransaction == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	auto result = callbacks.beginTransaction(hCard, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

void ExternalCardInterface::EndTransaction(PTEID_CardHandle hCard) {
	if (callbacks.endTransaction == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	auto result = callbacks.endTransaction(hCard, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}
}

std::pair<bool, CByteArray> ExternalCardInterface::StatusWithATR(PTEID_CardHandle hCard) {
	if (callbacks.statusWithATR == nullptr) {
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
	}

	unsigned char buffer[64];
	unsigned long bufferSize = sizeof(buffer);

	auto result = callbacks.statusWithATR(hCard, buffer, &bufferSize, callbacks.context);
	if (result != PTEID_CALLBACK_OK) {
		throw CMWEXCEPTION(CallbackToInternalError(result));
	}

	return std::make_pair(true, CByteArray(buffer, bufferSize));
}

} // namespace eIDMW
