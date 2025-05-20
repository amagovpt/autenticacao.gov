/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012, 2015-2017 André Guerreiro - <aguerreiro1985@gmail.com>
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
#ifndef __PTEIDCARD_H__
#define __PTEIDCARD_H__

#include "PkiCard.h"
#include "Card.h"

using namespace eIDMW;

CCard *PteidCardGetInstance(unsigned long ulVersion, const char *csReader, SCARDHANDLE hCard, CContext *poContext,
							GenericPinpad *poPinpad, const void *protocol_struct);

namespace eIDMW {

// Workaround needed for Windows 8 and later: With the do-nothing call to SCardStatus() in PCSC::Status()
// we make sure the transaction has activity so that Windows doesn't kill it
class KeepAliveThread : public CThread {
public:
	KeepAliveThread(CPCSC *poPCSC, SCARDHANDLE &card) {
		m_hCard = card;
		m_poPCSC = poPCSC;
	}

	~KeepAliveThread() { RequestStop(); }

	void Run();

private:
	CPCSC *m_poPCSC;
	SCARDHANDLE m_hCard;
};

class CPteidCard : public CPkiCard {
public:
	CPteidCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, tSelectAppletMode selectAppletMode,
			   unsigned long ulVersion, const void *protocol);

	CPteidCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad, const void *protocol);
	~CPteidCard(void);

	virtual tCardType GetType();
	virtual CByteArray GetSerialNrBytes();
	/** Returns 3 bytes:
	 *   - the appletversion (1 byte): 0x10, 0x11, 0x20
	 *   - the global OS version (2 bytes) */
	virtual CByteArray GetInfo();
	virtual std::string GetAppletVersion();

	virtual DlgPinUsage PinUsage2Dlg(const tPin &Pin, const tPrivKey *pKey);
	virtual bool PinCmd(tPinOperation operation, const tPin &Pin, const std::string &csPin1, const std::string &csPin2,
						unsigned long &ulRemaining, const tPrivKey *pKey = NULL, bool bShowDlg = true,
						void *wndGeometry = 0, unsigned long unblockFlags = 0);
	virtual unsigned long PinStatus(const tPin &Pin);
	virtual bool isPinVerified(const tPin &Pin);
	virtual CByteArray RootCAPubKey();
	virtual bool Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN);
	virtual bool unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin,
						   unsigned long &triesLeft, unsigned long unblockFlags);

	virtual void InitEncryptionKey();
	virtual void ReadSerialNumber();

	virtual unsigned long GetSupportedAlgorithms();

protected:
	virtual bool ShouldSelectApplet(unsigned char ins, unsigned long ulSW12);
	virtual bool SelectApplet();

	virtual tFileInfo SelectFile(const std::string &csPath, bool bReturnFileInfo = false);

	virtual void showPinDialog(tPinOperation operation, const tPin &Pin, std::string &csPin1, std::string &csPin2,
							   const tPrivKey *pKey, void *wndGeometry = 0);

	virtual void SetSecurityEnv(const tPrivKey &key, unsigned long paddingType, unsigned long ulInputLen);
	virtual CByteArray SignInternal(const tPrivKey &key, unsigned long paddingType, const CByteArray &oData,
									const tPin *pPin = NULL);

	virtual tCacheInfo GetCacheInfo(const std::string &csPath);

	CByteArray m_oCardData;
	CByteArray m_oSerialNr;
};

} // namespace eIDMW

#endif
