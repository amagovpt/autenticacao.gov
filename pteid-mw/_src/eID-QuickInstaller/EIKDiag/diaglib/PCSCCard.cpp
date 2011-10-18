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
#include "StdAfx.h"
#include "pcsccard.h"
#include "ErrorFmt.h"

#include "autovec.h"
using namespace sc;

#undef SUPPORT_WINSCARP
//#define SUPPORT_WINSCARP

#pragma hdrstop

PCSCCard::PCSCCard(void) {
    this->_Initialized = false;
    this->_Connected = false;
    this->_hWinsCard = 0;
    this->_hSC = 0;
}

PCSCCard::~PCSCCard(void) {
    if (this->_Connected)
        this->Disconnect(true);

    if (this->_hSC != 0)
        _SCardReleaseContext(this->_hSC);
    if (this->_hWinsCard != 0)
        FreeLibrary(this->_hWinsCard);
}

bool PCSCCard::Connect(const string &ReaderName) {

    DWORD activeProtocol = 0;
    LONG rc;

    if ((rc = this->_SCardConnect(
            _hSC,
            ReaderName.c_str(),
            SCARD_SHARE_SHARED,
            SCARD_PROTOCOL_Tx, //SCARD_PROTOCOL_Tx,
            &_hCard,
            &activeProtocol)) != SCARD_S_SUCCESS) {

        this->_LastErrorMsg = CErrorFmt::FormatError(rc,
                "SCardConnect");
        return false;

    }

    this->_Connected = true;
    ::Sleep(150);
    this->ReadAtr();

    return true;

}

bool PCSCCard::Disconnect(bool EjectCard) {

    LONG rc;

    if (!this->_Connected) {
        return true;
    }
    if ((rc = this->_SCardDisconnect(
            this->_hCard,
            EjectCard ? SCARD_LEAVE_CARD : SCARD_EJECT_CARD)) != SCARD_S_SUCCESS) {

        this->_LastErrorMsg = CErrorFmt::FormatError(rc, "SCarDisconnect");
        this->_hCard = 0;
        return false;

    }

    this->_hCard = 0;
    this->_Connected = false;
    return true;

}

bool PCSCCard::Initialize() {

    const char *MW_DLL = "winscarp.dll";
    const char *MS_DLL = "winscard.dll";
    this->_LastErrorMsg = "";

    // dynamically load the PCSC dll 

#ifdef SUPPORT_WINSCARP
    // first try "winscarp.dll", then "winscard.dll"
    this->_hWinsCard = LoadLibrary(MW_DLL);
    if (this->_hWinsCard == NULL) {
#endif SUPPORT_WINSCARP
        this->_hWinsCard = LoadLibrary(MS_DLL);
        if (this->_hWinsCard == NULL) {
            this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "LoadLibrary(winscard.dll)");
            return false;
        }
#ifdef SUPPORT_WINSCARP
    }
#endif SUPPORT_WINSCARP
    this->_SCardEstablishContext = reinterpret_cast<fSCardEstablishContext> (
            GetProcAddress(this->_hWinsCard, "SCardEstablishContext"));
    if (this->_SCardEstablishContext == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardEstablishContext)");
        return false;
    }

    this->_SCardReleaseContext = reinterpret_cast<fSCardReleaseContext> (
            GetProcAddress(this->_hWinsCard, "SCardReleaseContext"));
    if (this->_SCardReleaseContext == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardReleaseContext)");
        return false;
    }

    this->_SCardListReaders = reinterpret_cast<fSCardListReaders> (
            GetProcAddress(this->_hWinsCard, "SCardListReadersA"));
    if (this->_SCardListReaders == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardListReaders)");
        return false;
    }

    this->_SCardConnect = reinterpret_cast<fSCardConnect> (
            GetProcAddress(this->_hWinsCard, "SCardConnectA"));
    if (this->_SCardConnect == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardConnect)");
        return false;
    }

    this->_SCardDisconnect = reinterpret_cast<fSCardDisconnect> (
            GetProcAddress(this->_hWinsCard, "SCardDisconnect"));
    if (this->_SCardDisconnect == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardDisconnect)");
        return false;
    }

#ifndef SUPPORT_WINSCARP
    this->_SCardFreeMemory = reinterpret_cast<fSCardFreeMemory> (
            GetProcAddress(this->_hWinsCard, "SCardFreeMemory"));
    if (this->_SCardFreeMemory == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardFreeMemory)");
        return false;
    }

    this->_SCardGetAttrib = reinterpret_cast<fSCardGetAttrib> (
            GetProcAddress(this->_hWinsCard, "SCardGetAttrib"));
    if (this->_SCardGetAttrib == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardGetAttrib)");
        return false;
    }
#endif SUPPORT_WINSCARP

    this->_SCardStatus = reinterpret_cast<fSCardStatus> (
            GetProcAddress(this->_hWinsCard, "SCardStatusA"));
    if (this->_SCardStatus == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardStatus)");
        return false;
    }

    this->_SCardTransmit = reinterpret_cast<fSCardTransmit> (
            GetProcAddress(this->_hWinsCard, "SCardTransmit"));
    if (this->_SCardTransmit == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardTransmit)");
        return false;
    }

    this->_SCardBeginTransaction = reinterpret_cast<fSCardBeginTransaction> (
            GetProcAddress(this->_hWinsCard, "SCardBeginTransaction"));
    if (this->_SCardBeginTransaction == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardBeginTransaction)");
        return false;
    }

    this->_SCardEndTransaction = reinterpret_cast<fSCardEndTransaction> (
            GetProcAddress(this->_hWinsCard, "SCardEndTransaction"));
    if (this->_SCardEndTransaction == NULL) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "GetProcAddress(SCardEndTransaction)");
        return false;
    }


    // contact PCSC subsystem, and list the readers in the system

    if (this->_SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &this->_hSC) != SCARD_S_SUCCESS) {
        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "SCardEstablishContext");
        return false;
    }

    char *mszReaders = 0;
    DWORD bcReaders = 0;

#ifndef SUPPORT_WINSCARP 

    bcReaders = SCARD_AUTOALLOCATE;

    if (this->_SCardListReaders(
            _hSC,
            NULL,
            reinterpret_cast<LPTSTR> (&mszReaders),
            &bcReaders) != SCARD_S_SUCCESS) {

        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "SCardListReaders");

        return false;

    }
#else

#define RDRBUF_SIZE 4096
    auto_vec<char> rdrBuf(new char[RDRBUF_SIZE]);
    bcReaders = RDRBUF_SIZE;
    mszReaders = rdrBuf.get();

    if (this->_SCardListReaders(
            _hSC,
            NULL,
            reinterpret_cast<LPTSTR> (mszReaders),
            &bcReaders) != SCARD_S_SUCCESS) {

        this->_LastErrorMsg = CErrorFmt::FormatError(GetLastError(), "SCardListReaders");

        return false;

    }

#endif

    if (bcReaders > 0) {
        char *pReaderName = mszReaders;
        while (true) {
            if (*pReaderName == 0)
                break;
            string readername = pReaderName;
            _Readers.push_back(readername);
            while (*pReaderName++ != 0)
                ;
        }
    }

#ifndef SUPPORT_WINSCARP
    this->_SCardFreeMemory(_hSC, mszReaders);
#endif

    return true;

}

AtrString PCSCCard::ReadAtr(void) {

    LONG rc;
    this->_atr.clear();

    if (!this->_Connected) {
        this->_LastErrorMsg = "ReadAtr: Not connected to any smartcard.";
        return this->_atr;
    }


    BYTE *pbAttr = NULL;
    DWORD cbAttr = 0;

#ifndef SUPPORT_WINSCARP

    cbAttr = SCARD_AUTOALLOCATE;

    if ((rc = this->_SCardGetAttrib(
            this->_hCard,
            SCARD_ATTR_ATR_STRING,
            reinterpret_cast<LPBYTE> (&pbAttr),
            &cbAttr)) == SCARD_S_SUCCESS) {
        for (size_t i = 0; i < cbAttr; ++i)
            this->_atr.push_back(pbAttr[i]);
        this->_SCardFreeMemory(this->_hSC, pbAttr);
    } else {
        this->_LastErrorMsg = CErrorFmt::FormatError(rc, "SCardGetAttrib");
    }
#else

#define RDRBUF_SIZE 4096
#define ATRBUF_SIZE 256

    auto_vec<char> rdrBuf(new char[RDRBUF_SIZE]);
    DWORD rdrBufLen = RDRBUF_SIZE;
    char *mszReaders = rdrBuf.get();

    auto_vec<BYTE> atrBuf(new BYTE[ATRBUF_SIZE]);
    cbAttr = ATRBUF_SIZE;
    pbAttr = atrBuf.get();

    DWORD dwState;
    DWORD dwProtocol;

    if ((rc = this->_SCardStatus(
            this->_hCard,
            reinterpret_cast<LPTSTR> (mszReaders),
            &rdrBufLen,
            &dwState,
            &dwProtocol,
            reinterpret_cast<LPBYTE> (pbAttr),
            &cbAttr)) == SCARD_S_SUCCESS) {
        for (size_t i = 0; i < cbAttr; ++i)
            this->_atr.push_back(pbAttr[i]);
    } else {
        this->_LastErrorMsg = CErrorFmt::FormatError(rc, "SCardGetAttrib");
    }

#endif

    return this->_atr;

}

#pragma pack(1)

typedef struct _SCardSendBuffer { // 256 byte send buffer
    BYTE Data[280]; // SCARD_T0_COMMAND, followed by data
} SCardSendBuffer, *PScardSendBuffer;

typedef struct _SCardRecvBuffer { // 256 byte receive buffer + SW1 SW2
    BYTE Data[258];
} SCardRecvBuffer, *PSCardRecvBuffer;
#pragma pack()

Buffer PCSCCard::SendApdu(const Buffer& Apdu) {

    SCARD_T0_REQUEST SendPCI;
    SCARD_T0_REQUEST RecvPCI;
    SCardSendBuffer SendBuffer;
    SCardRecvBuffer RecvBuffer;
    DWORD cbRecvLength;
    DWORD lReturn;

    Buffer GetResponseAPDU;
    Buffer Result;

    // prepare pcsc databuffers
    this->_SW1 = 0xFF;
    this->_SW2 = 0xFF;
    // Protocol info 
    memset(&SendPCI, 0, sizeof (SendPCI));
    SendPCI.ioRequest.dwProtocol = SCARD_PROTOCOL_T0;
    SendPCI.ioRequest.cbPciLength = sizeof (SendPCI.ioRequest);
    memset(&RecvPCI, 0, sizeof (RecvPCI));
    RecvPCI.ioRequest.dwProtocol = SCARD_PROTOCOL_T0;
    RecvPCI.ioRequest.cbPciLength = sizeof (RecvPCI.ioRequest) + 2;
    // send buffer 
    memset(&SendBuffer, 0, sizeof (SendBuffer));
    int apdulen = static_cast<int> (Apdu.size());
    for (int i = 0; i < apdulen; ++i)
        SendBuffer.Data[i] = Apdu.at(i);
    // receive buffer 
    memset(&RecvBuffer, 0, sizeof (RecvBuffer));
    cbRecvLength = sizeof (RecvBuffer);

	// old cards in fast readers 
	::Sleep(10);

	// transmit
	if ((lReturn = this->_SCardTransmit(
                this->_hCard,
                reinterpret_cast<LPSCARD_IO_REQUEST> (&SendPCI),
                reinterpret_cast<LPCBYTE> (&SendBuffer),
                apdulen,
                reinterpret_cast<LPSCARD_IO_REQUEST> (&RecvPCI),
                reinterpret_cast<LPBYTE> (&RecvBuffer),
                &cbRecvLength)) != SCARD_S_SUCCESS) 
	{
            this->_LastErrorMsg = CErrorFmt::FormatError(lReturn, "SCardTransmit");
            this->_SW1 = RecvPCI.bSw1;
            this->_SW2 = RecvPCI.bSw2;
            // if error in SCardTransmit, return empty result.

            return Result; // empty
    }

    // Geen fout, maar enkel status, geen antwoord data 
    if (cbRecvLength < 2) {
        this->_SW1 = RecvPCI.bSw1;
        this->_SW2 = RecvPCI.bSw2;
        return Result; // empty
    }
    // Wel antwoord data, haal antwoord en status op 
    cbRecvLength--;
    this->_SW2 = RecvBuffer.Data[cbRecvLength];
    cbRecvLength--;
    this->_SW1 = RecvBuffer.Data[cbRecvLength];
    if (cbRecvLength > 0) {
        for (int i = 0; i < static_cast<int> (cbRecvLength); ++i)
            Result.push_back(RecvBuffer.Data[i]);
    }
    // Heeft de kaart nog meer data ? 
    if (this->_SW1 == 0x61) {
        // ja, voer GetResponse uit en geef die terug
        // recursief, overschrijft dus _SWx status bytes !
        const BYTE apdu[] = {0x00, 0xC0, 0x00, 0x00
        };
        for (int i = 0; i < sizeof (apdu); ++i)
            GetResponseAPDU.push_back(apdu[i]);
        GetResponseAPDU.push_back(this->_SW2);
        Buffer MoreResult = SendApdu(GetResponseAPDU);
        // plak de results aaneen
        for (BufferIterator it = MoreResult.begin(); it != MoreResult.end(); ++it)
            Result.push_back(*it);
    }

    return Result;

}

Buffer PCSCCard::ReadCardFile(const BYTE *Path, const int PathLen) {

    const int MAX_READ_SIZE = 0xF8;
	const byte SELECT_APPLET_APDU[] = {0x00,0xA4,0x04,0x00,0x0F,0xA0,0x00,0x00,0x00,0x30,0x29,0x05,0x70,0x00,0xAD,0x13,0x10,0x01,0x01,0xFF};
    const byte SELECT_FILE_APDU[] = {0x00, 0xA4, 0x08, 0x0C, 0x00};
    const byte READ_BINARY_APDU[] = {0x00, 0xB0, 0x00, 0x00, 0xF8};

    Buffer Result;

    Buffer APDU;
    int offset;
    bool done;
    Buffer tmpResult;
    DWORD lReturn;

    this->_LastErrorMsg = "";

    // Start Transaction : Also nessesary for read-only actions.
    if ((lReturn = this->_SCardBeginTransaction(this->_hCard)) != SCARD_S_SUCCESS) 
	{
        this->_LastErrorMsg = CErrorFmt::FormatError(lReturn, "SCardBeginTransaction");
        return Result; // empty
    }

    // build select applet apdu
    APDU.reserve(sizeof (SELECT_APPLET_APDU));
    for (int i = 0; i < sizeof (SELECT_APPLET_APDU); ++i)
        APDU.push_back(SELECT_APPLET_APDU[i]);
    // select applet
    this->SendApdu(APDU);
    if (!((this->_SW1 == 0x90) && (this->_SW2 == 0x00))) {
        char buf[64];
        sprintf_s(buf, sizeof (buf), "**Error** Select applet: SW1=%2.2x SW2=%2.2x", this->_SW1, this->_SW2);
        this->_LastErrorMsg = &buf[0];
        return Result; // empty
    }

    // build select file apdu
    APDU.clear();
    APDU.reserve(PathLen + sizeof (SELECT_FILE_APDU));
    for (int i = 0; i < sizeof (SELECT_FILE_APDU); ++i)
        APDU.push_back(SELECT_FILE_APDU[i]);
    for (int i = 0; i < PathLen; ++i)
        APDU.push_back(Path[i]);
    APDU.at(4) = static_cast<BYTE> (PathLen); //{ Lc = length path }
    // select file
    this->SendApdu(APDU);
    if (!((this->_SW1 == 0x90) && (this->_SW2 == 0x00))) {
        char buf[64];
        sprintf_s(buf, sizeof (buf), "**Error** Select File: SW1=%2.2x SW2=%2.2x", this->_SW1, this->_SW2);
        this->_LastErrorMsg = &buf[0];
        return Result; // empty
    }
    done = false;
    offset = 0;
    do {
        APDU.clear();
        APDU.reserve(sizeof (READ_BINARY_APDU));
        for (int i = 0; i < sizeof (READ_BINARY_APDU); ++i)
            APDU.push_back(READ_BINARY_APDU[i]);
        APDU.at(4) = MAX_READ_SIZE; // read MAX_READ_SIZE bytes 
        APDU.at(2) = ((offset & 0xFF00) >> 8); // P1 = HiByte(offset) 
        APDU.at(3) = (offset & 0xFF); // P2 = LoByte(Offset) }
        tmpResult = SendApdu(APDU);
        // als de offset voorbij EOF zit, hebben we gedaan 
        if ((this->_SW1 == 0x6B) && (this->_SW2 == 0x00)) {
            done = true;
            continue;
        }
        // als er niet meer zoveel is: lees laatste of enige stukje en stop 
        if (this->_SW1 == 0x6C) {
            APDU.at(4) = this->_SW2; // gebruik juiste lengte voor Le 
            tmpResult = SendApdu(APDU); // en lees opnieuw 
            if (!((this->_SW1 == 0x90) && (this->_SW2 == 0x00))) {
                done = true; // oei, t'is mis 
                continue;
            }
            // plak laatste stuk eraan
            Result.insert(Result.end(), tmpResult.begin(), tmpResult.end());
            done = true; // laatste/enige stukje gelezen, 't is gedaan 
            continue;
        }
        // als MAX_READ_SIZE bytes gelezen: accumuleer en lees verder 
        if ((this->_SW1 == 0x90) && (this->_SW2 == 0x00)) {
            Result.insert(Result.end(), tmpResult.begin(), tmpResult.end());
            offset += MAX_READ_SIZE;
            continue;
        }
        // als we hier komen is er iets mis, keer terug met SW1 en SW2 en LastError 
        done = true;
    } while (!done);

    // End Transaction
    if ((lReturn = this->_SCardEndTransaction(this->_hCard, SCARD_LEAVE_CARD)) != SCARD_S_SUCCESS) 
	{
		this->_LastErrorMsg = CErrorFmt::FormatError(lReturn, "SCardEndTransaction");
        // result is valid, even if end-transaction fails ??
	}

    return Result;

}

