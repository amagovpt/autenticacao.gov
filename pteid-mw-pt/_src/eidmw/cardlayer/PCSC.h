/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
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
/*
Takes care of
 - communication with the reader/smart card via Winscard/PCSClite API
 - pinpad handling (loading of pinpad libs, making pinpad commands, ...)
*/

#ifndef PCSC_H
#define PCSC_H

#include "Export.h"
#include "eidErrors.h"
#include "ByteArray.h"
#include "MWException.h"
#include "CardLayerConst.h"
#include "InternalConst.h"
#include "../eidlib/CardCallbacks.h"

#include <cstdint>
#include <unordered_map>
#include <utility>

#ifndef WIN32
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#include "Reader.h"

#ifndef SCARD_READERSTATE_A
#define SCARD_READERSTATE_A SCARD_READERSTATE
#endif

#ifndef SCARD_READERSTATEA
#define SCARD_READERSTATEA SCARD_READERSTATE_A
#endif

#ifndef SCARD_ATTR_VENDOR_IFD_VERSION
#define SCARD_ATTR_VENDOR_IFD_VERSION ((DWORD)0x00010102)
#endif
#else
#include <winscard.h>
#endif

#define IOCTL_SMARTCARD_SET_CARD_TYPE SCARD_CTL_CODE(2060)

// #include <winscard.h>

namespace eIDMW {

// Copied from PCSC
#define EIDMW_STATE_CHANGED 0x00000002
#define EIDMW_STATE_PRESENT 0x00000020

/**
 * Provides an abstraction of the PCSC SCARD_READERSTATE struct.
 * The ulCurrentState and ulEventState correspond to the
 * dwCurrentState resp. dwEventState in the SCARD_READERSTATE struct,
 * except that the SCARD_STATE_CHANGED is only set when a card
 * insert/removal occurred (as opposed to Windows' PCSC).
 */
typedef struct {
	std::string csReader;
	unsigned long ulCurrentState; // the state when we last checked
	unsigned long ulEventState;	  // the state after the new check
} tReaderInfo;

class EIDMW_CAL_API CardInterface {
public:
	virtual void EstablishContext() = 0;
	virtual void ReleaseContext() = 0;

	virtual CByteArray ListReaders() = 0;

	virtual CByteArray GetATR(PTEID_CardHandle hCard) = 0;

	virtual bool GetStatusChange(unsigned long ulTimeout, tReaderInfo *pReaderInfos, unsigned long ulReaderCount) = 0;
	virtual bool Status(const std::string &csReader) = 0;
	virtual bool Status(PTEID_CardHandle hCard) = 0;

	virtual void BeginTransaction(PTEID_CardHandle hCard) = 0;
	virtual void EndTransaction(PTEID_CardHandle hCard) = 0;

	virtual std::pair<PTEID_CardHandle, DWORD>
	Connect(const std::string &csReader, unsigned long ulShareMode = SCARD_SHARE_SHARED,
			unsigned long ulPreferredProtocols = SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1) = 0;
	virtual void Disconnect(PTEID_CardHandle hCard, tDisconnectMode disconnectMode) = 0;

	virtual CByteArray Transmit(PTEID_CardHandle hCard, const CByteArray &oCmdAPDU, long *plRetVal,
								const void *pSendPci = NULL, void *pRecvPci = NULL) = 0;
	virtual void Recover(PTEID_CardHandle hCard, unsigned long *pulLockCount) = 0;
	virtual CByteArray Control(PTEID_CardHandle hCard, unsigned long ulControl, const CByteArray &oCmd,
							   unsigned long ulMaxResponseSize = CTRL_BUF_LEN) = 0;

	long SW12ToErr(unsigned long ulSW12);

private:
};

class EIDMW_CAL_API CPCSC : public CardInterface {
public:
	CPCSC(void);
	~CPCSC(void);

	void EstablishContext() override;

	void ReleaseContext() override;

	/**
	 * We can't return a string because the output is a "multistring",
	 * which means a multiple strings separated by a 0x00 and ended
	 * by 2 0x00 bytes.
	 */
	CByteArray ListReaders() override;

	/** Returns true if something changed */
	bool GetStatusChange(unsigned long ulTimeout, tReaderInfo *pReaderInfos, unsigned long ulReaderCount) override;

	bool Status(const std::string &csReader) override;

	std::pair<PTEID_CardHandle, DWORD> Connect(const std::string &csReader, unsigned long ulShareMode = SCARD_SHARE_SHARED,
										 unsigned long ulPreferredProtocols = SCARD_PROTOCOL_T0 |
																			  SCARD_PROTOCOL_T1) override;
	void Disconnect(PTEID_CardHandle hCard, tDisconnectMode disconnectMode) override;

	CByteArray GetATR(PTEID_CardHandle hCard) override;

	/**
	 * Returns true if the same card is still present,
	 * false if the card has been removed (and perhaps
	 * the same or antoher card has been inserted).
	 */
	bool Status(PTEID_CardHandle hCard) override;

	CByteArray Transmit(PTEID_CardHandle hCard, const CByteArray &oCmdAPDU, long *plRetVal, const void *pSendPci = NULL,
						void *pRecvPci = NULL) override;
	void Recover(PTEID_CardHandle hCard, unsigned long *pulLockCount) override;
	CByteArray Control(PTEID_CardHandle hCard, unsigned long ulControl, const CByteArray &oCmd,
					   unsigned long ulMaxResponseSize = CTRL_BUF_LEN) override;

	void BeginTransaction(PTEID_CardHandle hCard) override;
	void EndTransaction(PTEID_CardHandle hCard) override;

private:
	long PcscToErr(unsigned long lRet);

	// unsigned long m_hContext;
	SCARDCONTEXT m_hContext;

	friend class CPinpad;

	int m_iTimeoutCount;
	int m_iListReadersCount;

	unsigned long m_ulCardTxDelay; // delay before each transmission to a smartcard; in millie-seconds, default 1

	std::unordered_map<PTEID_CardHandle, SCARDHANDLE> m_handles;
};

class EIDMW_CAL_API ExternalCardInterface : public CardInterface {
public:
	ExternalCardInterface(const PTEID_CardInterfaceCallbacks *callbacks);
	~ExternalCardInterface();

	void EstablishContext() override;

	void ReleaseContext() override;

	CByteArray ListReaders() override;
	bool GetStatusChange(unsigned long ulTimeout, tReaderInfo *pReaderInfos, unsigned long ulReaderCount) override;

	bool Status(const std::string &csReader) override;

	std::pair<PTEID_CardHandle, DWORD> Connect(const std::string &csReader, unsigned long ulShareMode = SCARD_SHARE_SHARED,
										 unsigned long ulPreferredProtocols = SCARD_PROTOCOL_T0 |
																			  SCARD_PROTOCOL_T1) override;
	void Disconnect(PTEID_CardHandle hCard, tDisconnectMode disconnectMode) override;

	CByteArray GetATR(PTEID_CardHandle hCard) override;

	bool Status(PTEID_CardHandle hCard) override;

	CByteArray Transmit(PTEID_CardHandle hCard, const CByteArray &oCmdAPDU, long *plRetVal, const void *pSendPci = NULL,
						void *pRecvPci = NULL) override;
	void Recover(PTEID_CardHandle hCard, unsigned long *pulLockCount) override;
	CByteArray Control(PTEID_CardHandle hCard, unsigned long ulControl, const CByteArray &oCmd,
					   unsigned long ulMaxResponseSize = CTRL_BUF_LEN) override;

	void BeginTransaction(PTEID_CardHandle hCard) override;
	void EndTransaction(PTEID_CardHandle hCard) override;

private:
	PTEID_CardInterfaceCallbacks callbacks = {};
};

} // namespace eIDMW
#endif
