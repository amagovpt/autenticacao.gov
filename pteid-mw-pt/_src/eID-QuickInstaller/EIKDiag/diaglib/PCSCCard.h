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

#ifndef __PCSCCARD__
#define __PCSCCARD__

#pragma once

#undef UNICODE

#include <winscard.h>

#include <windows.h>
#include <string>
#include <vector>

using namespace std;

typedef vector<string> ReaderList;
typedef ReaderList::iterator ReaderIterator;
typedef vector<BYTE> AtrString;
typedef AtrString::iterator AtrIterator;
typedef vector<BYTE> Buffer;
typedef Buffer::iterator BufferIterator;

typedef LONG(__stdcall *fSCardEstablishContext)(
        DWORD dwScope,
        LPCVOID pvReserved1,
        LPCVOID pvReserved2,
        LPSCARDCONTEXT phContext
        );
typedef LONG(__stdcall *fSCardReleaseContext)(
        SCARDCONTEXT hContext
        );


typedef LONG(__stdcall *fSCardConnect)(
        SCARDCONTEXT hContext,
        LPCTSTR szReader,
        DWORD dwShareMode,
        DWORD dwPreferredProtocols,
        LPSCARDHANDLE phCard,
        LPDWORD pdwActiveProtocol
        );

typedef LONG(__stdcall *fSCardDisconnect)(
        SCARDHANDLE hCard,
        DWORD dwDisposition
        );

typedef LONG(__stdcall *fSCardFreeMemory)(
        SCARDCONTEXT hContext,
        LPCVOID pvMem
        );

typedef LONG(__stdcall *fSCardListReaders)(
        SCARDCONTEXT hContext,
        LPCTSTR mszGroups,
        LPTSTR mszReaders,
        LPDWORD pcchReaders
        );

typedef LONG(__stdcall *fSCardGetAttrib)(
        SCARDHANDLE hCard,
        DWORD dwAttrId,
        LPBYTE pbAttr,
        LPDWORD pcbAttrLen
        );

typedef LONG(__stdcall *fSCardStatus)(
        SCARDHANDLE hCard,
        LPTSTR szReaderName,
        LPDWORD pcchReaderLen,
        LPDWORD pdwState,
        LPDWORD pdwProtocol,
        LPBYTE pbAtr,
        LPDWORD pcbAtrLen
        );



typedef LONG(__stdcall *fSCardTransmit)(
        SCARDHANDLE hCard,
        LPCSCARD_IO_REQUEST pioSendPci,
        LPCBYTE pbSendBuffer,
        DWORD cbSendLength,
        LPSCARD_IO_REQUEST pioRecvPci,
        LPBYTE pbRecvBuffer,
        LPDWORD pcbRecvLength
        );

typedef LONG(__stdcall *fSCardBeginTransaction)(
        SCARDHANDLE hCard
        );

typedef LONG(__stdcall *fSCardEndTransaction)(
        SCARDHANDLE hCard,
        DWORD dwDisposition
        );

class PCSCCard {
public:
    PCSCCard(void);
    virtual ~PCSCCard(void);

    bool Initialize(void);

    ReaderIterator FirstReader() {
        return _Readers.begin();
    };

    ReaderIterator LastReader() {
        return _Readers.end();
    };

    size_t ReaderCount() {
        return _Readers.size();
    };

    ReaderList& Readers() {
        return _Readers;
    };

    AtrString& Atr() {
        return _atr;
    };
    bool Connect(const string& Readername);
    bool Disconnect(bool EjectCard);

    string LastErrorMsg() {
        return _LastErrorMsg;
    };
public:
    AtrString ReadAtr(void);
    Buffer ReadCardFile(const BYTE *Path, const int PathLen);
private:
    // Windows handles
    HMODULE _hWinsCard;
    SCARDCONTEXT _hSC;
    SCARDHANDLE _hCard;
    // GetProcAdrress() function pointers
    fSCardEstablishContext _SCardEstablishContext;
    fSCardReleaseContext _SCardReleaseContext;
    fSCardConnect _SCardConnect;
    fSCardListReaders _SCardListReaders;
    fSCardDisconnect _SCardDisconnect;
    fSCardFreeMemory _SCardFreeMemory;
    fSCardGetAttrib _SCardGetAttrib;
    fSCardStatus _SCardStatus;
    fSCardTransmit _SCardTransmit;
    fSCardBeginTransaction _SCardBeginTransaction;
    fSCardEndTransaction _SCardEndTransaction;

    // internal stuff
    bool _Initialized;
    bool _Connected;
    string _LastErrorMsg;
    ReaderList _Readers;
    AtrString _atr;
    BYTE _SW1;
    BYTE _SW2;
    Buffer SendApdu(const Buffer& Apdu);
};

#endif __PCSCCARD__
