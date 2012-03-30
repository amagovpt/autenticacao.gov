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
 * This is the parent class for all PKI cards(such as most
 * eID cards); containing functionality for reading/writing
 * files and crypto (sign, random, ...)
 */
#pragma once

#ifndef ASYMCARD_H
#define ASYMCARD_H

#include "Card.h"

namespace eIDMW
{

class CPkiCard : public CCard
{
public:
    CPkiCard(SCARDHANDLE hCard, CContext *poContext, GenericPinpad *poPinpad);
    virtual ~CPkiCard(void);

    virtual tCardType GetType() = 0;
    virtual CByteArray GetInfo() = 0;

    virtual CByteArray GetSerialNrBytes() = 0;

    virtual void SelectApplication(const CByteArray & oAID);

    virtual CByteArray ReadUncachedFile(const std::string & csPath,
        unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE);
    virtual void WriteUncachedFile(const std::string & csPath, unsigned long ulOffset,
        const CByteArray & oData);

    virtual DlgPinUsage PinUsage2Dlg(const tPin & Pin, const tPrivKey *pKey) = 0;

    virtual unsigned long PinStatus(const tPin & Pin) = 0;
    virtual CByteArray RootCAPubKey() = 0;
    virtual bool Activate(const char *pinCode, CByteArray &BCDDate) = 0;
    virtual bool unlockPIN(const tPin &pin, const tPin *puk, const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft) = 0;
    virtual bool PinCmd(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey = NULL,
        bool bShowDlg=true);
    virtual bool PinCmdIAS(tPinOperation operation, const tPin & Pin,
        const std::string & csPin1, const std::string & csPin2,
        unsigned long & ulRemaining, const tPrivKey *pKey = NULL,
        bool bShowDlg=true);
	virtual bool LogOff(const tPin & Pin);

    virtual CByteArray Sign(const tPrivKey & key, const tPin & Pin,
        unsigned long algo, const CByteArray & oData);
    virtual CByteArray Sign(const tPrivKey & key, const tPin & Pin,
        unsigned long algo, CHash & oHash);

    virtual CByteArray GetRandom(unsigned long ulLen);

    virtual CByteArray Ctrl(long ctrl, const CByteArray & oCmdData) = 0;

protected:
    std::map <unsigned int, std::string> m_verifiedPINs;

    virtual bool ShouldSelectApplet(unsigned char ins, unsigned long ulSW12);
    virtual bool SelectApplet();

	virtual tFileInfo SelectFile(const std::string & csPath, bool bReturnFileInfo = false);
    virtual CByteArray SelectByPath(const std::string & csPath, bool bReturnFileInfo = false);
    virtual tFileInfo ParseFileInfo(CByteArray & oFCI) = 0;

    virtual CByteArray ReadBinary(unsigned long ulOffset, unsigned long ulLen);
    virtual CByteArray UpdateBinary(unsigned long ulOffset, const CByteArray & oData);

	virtual unsigned char PinUsage2Pinpad(const tPin & Pin, const tPrivKey *pKey);
	virtual DlgPinOperation PinOperation2Dlg(tPinOperation operation);
    virtual void showPinDialog(tPinOperation operation, const tPin & Pin,
        std::string & csPin1, std::string & csPin2, const tPrivKey *pKey) = 0;

    virtual CByteArray MakePinCmd(tPinOperation operation, const tPin & Pin);
    virtual CByteArray MakePinCmdIAS(tPinOperation operation, const tPin & Pin);
    virtual CByteArray MakePinBuf(const tPin & Pin, const std::string & csPin, bool bEmptyPin, bool bPukMerge);

    virtual void SetSecurityEnv(const tPrivKey & key, unsigned long algo,
        unsigned long ulInputLen) = 0;
    virtual CByteArray SignInternal(const tPrivKey & key, unsigned long algo,
        const CByteArray & oData, const tPin *pPin = NULL) = 0;

	tSelectAppletMode m_selectAppletMode;
};

}
#endif
