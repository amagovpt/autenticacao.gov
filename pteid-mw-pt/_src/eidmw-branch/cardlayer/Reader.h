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

#include "PKCS15.h"
#include "Pinpad.h"
#include "../common/Hash.h"

namespace eIDMW
{

class CCardLayer;
class CCard;

class EIDMW_CAL_API CReader
{
public:
    ~CReader(void);

	/**
	 * Returns the reader name
	 */
	std::string & GetReaderName();

	/** Specify a callback function to be called each time a
	 * card is inserted/remove in/from this reader.
	 * The returned handle can be used to stop the callbacks
	 * when they are no longer needed. */
    unsigned long SetEventCallback(
		void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef);
	/** Returns true is ulState indicates that a card is present, false otherwise. */
	static bool CardPresent(unsigned long ulState);
	/** To tell that the callbacks are not longer needed. */
	void StopEventCallback(unsigned long ulHandle);

	/**
	 * Get the status w.r.t. a card being present in the reader
	 * \retval #CARD_INSERTED      a card has been inserted
	 * \retval #CARD_NOT_PRESENT   no card is present
	 * \retval #CARD_STILL_PRESENT the card we connected to is stil present
	 * \retval #CARD_REMOVED       the card has been removed
	 * \retval #CARD_OTHER         the card has been removed and replace by (another) one,
	 *             this can only be returned if bReconnect = true
	 * If a card has been inserted then this function won't connect to the card
	 * If the card has been removed and a (new) card has been inserted
	 * then if bReconnect is true, this function will reconnect to the (new) card.
	 */
    tCardStatus Status(bool bReconnect = false);

	/**
	 * Connect to the card; it's safe to call this function multiple times.
	 * Returns true if successfully connected, false otherwise (in which case
	 * no card or an unresponsive card is present).
	 * NOTE: this method must be called successfully before calling
	 * any of the other functions below.
	 */
    bool Connect();

	/** Disconnect from the card; it's safe to call this function multiple times */
	void Disconnect(tDisconnectMode disconnectMode = DISCONNECT_LEAVE_CARD);

	/**
	 * Returns the ATR of the card that is currently present.
	 */
    CByteArray GetATR();

    bool IsPinpadReader();

    tCardType GetCardType();

    std::string GetSerialNr();
    std::string GetCardLabel();

    void setSSO(bool value);

	/* Lock the card for exclusive use. Multiple calls are possible
	 * (only the first call will lock the card), but for each Lock()
	 * call, an UnLock() must be called. */
    void Lock();
    void Unlock();

    void SelectApplication(const CByteArray & oAID);

    /* Read the file indicated by 'csPath'.
     * This path can be absolute, relative or empty
	 * (in which case the currenlty selected file is read)
	 * If too much bytes are specified by ulMaxLen, no
	 * exception is throw, the function just returns the
	 * number of bytes that are available. */
    CByteArray ReadFile(const std::string &csPath,
        unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE, bool bDoNotCache=false);
    /* Write to the file indicated by 'csPath'.
     * This path can be absolute, relative or empty
	 * (in which case the currenlty selected file is written) */
    void WriteFile(const std::string &csPath, unsigned long ulOffset,
        const CByteArray & oData);

	/* Return the remaining PIN attempts;
	 * returns PIN_STATUS_UNKNOWN if this info isn't available */
    unsigned long PinStatus(const tPin & Pin);

    /* Get the CVC CA public key that
     * this card uses to verify the CVC key; */
    CByteArray RootCAPubKey();

    bool Activate(const char *pinCode, CByteArray &BCDDate, bool blockActivationPIN);

    bool unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long unblockFlags);

    bool PinCmd(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, bool bShowDlg=true, void *wndGeometry = 0, unsigned long unblockFlags = 0);

	/** Returns the OR-ing of all supported crypto algorithms */
	unsigned long GetSupportedAlgorithms();

	/* Sign data. If necessary, a PIN will be asked */
    CByteArray Sign(const tPrivKey & key, unsigned long algo,
        const CByteArray & oData);

    CByteArray GetRandom(unsigned long ulLen);

    CByteArray SendAPDU(const CByteArray & oCmdAPDU);

    //--- P15 functions
    unsigned long PinCount();
    /** ulIndex ranges from 0 to PinCount() - 1 */
    tPin GetPin(unsigned long ulIndex);
	/** If bValid == false, then no PIN with this ID was found */
    tPin GetPinByID(unsigned long ulID);

    unsigned long CertCount();
    /** ulIndex ranges from 0 to CertCount() - 1 */
    tCert GetCert(unsigned long ulIndex);
	/** If bValid == false, then no PIN with this ID was found */
    tCert GetCertByID(unsigned long ulID);

    unsigned long PrivKeyCount();
    /** ulIndex ranges from 0 to PrivKeyCount() - 1 */
    tPrivKey GetPrivKey(unsigned long ulIndex);
	/** If bValid == false, then no PIN with this ID was found */
    tPrivKey GetPrivKeyByID(unsigned long ulID);

	/** Returns a card-independent way to identify a PIN */
	PinUsage GetPinUsage(const tPin & pin);

private:
    CReader(const std::string & csReader, CContext *poContext);
    // No copies allowed
    CReader(const CReader & oReader);
    CReader & operator = (const CReader & oReader);

	bool m_bIgnoreRemoval;
    std::string m_csReader;
	std::wstring m_wsReader;
    CCard *m_poCard;
    CPKCS15 m_oPKCS15;
    CPinpad *m_oPinpad;
    
    friend class CCardLayer; // calls the CReader constructor

    CContext *m_poContext;
};

}
