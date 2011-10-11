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

//
// HOWTO
//
// Assuming that winscard.h pcsclite.h and wintypes.h are on the same path as proxy_pcsclite.c
// compile:
// $> gcc -fPIC -c proxy_pcsclite.c -I.
// link:
// $> gcc -shared -fPIC -ldl -pthread -o  libproxy_pcsclite.so.1.0 APDURequest.o  Control.o  FileStructure.o  HexUtils.o  proxy_pcsclite.o  SoftCard.o  SoftReaderManager.o  SoftReader.o  Transaction.o  WinscardImpl.o
// $> cp proxy_pcsclite.so.1.0 ~/mylibdir/
// $> cd ~/mylibdir/
// $> ln -s libproxy_pcsclite.so.1.0 libpcsclite.so.1
// $> export LD_LIBRARY_PATH ~/mylibdir/
//
// $> ldd /usr/local/lib/libpteidcardlayer.so.3
// --- Verify result of ldd contains the following entry: ---
// --- libpcsclite.so.1 => ~/mylibdir/libpcsclite.so.1 ---
//
//
// /usr/sbin/pcscd -df
//

#include <dlfcn.h>
#include <stdio.h>
#include <winscard.h>
#include <pcsclite.h>
#include "WinscardImpl.h"

void                           *lib;
const void                     *p[24] = { 0 };

eidmw::pcscproxy::WinscardImpl *impl = 0;

extern void __attribute__((constructor)) _init()
{
#ifdef __APPLE__
	const char *pcscLibPath = "/System/Library/Frameworks/PCSC.framework/PCSC";
#else
	const char *pcscLibPath = "/usr/lib/libpcsclite.so";
#endif

	lib = dlopen("REALPCSCLIB", RTLD_LAZY);
	if (!lib)
		lib = dlopen(pcscLibPath, RTLD_LAZY);
	if (!lib)
	{
		printf("Error loading library");
		return;
	}

	impl = new eidmw::pcscproxy::WinscardImpl(p);

	p[0]  = dlsym(lib, "SCardEstablishContext");
	p[1]  = dlsym(lib, "SCardReleaseContext");
	p[2]  = dlsym(lib, "SCardIsValidContext");
	p[3]  = dlsym(lib, "SCardListReaders");
	p[4]  = dlsym(lib, "SCardListReaderGroups");
	p[5]  = dlsym(lib, "SCardConnect");
	p[6]  = dlsym(lib, "SCardReconnect");
	p[7]  = dlsym(lib, "SCardDisconnect");
	p[8]  = dlsym(lib, "SCardBeginTransaction");
	p[9]  = dlsym(lib, "SCardEndTransaction");
	p[10] = dlsym(lib, "SCardTransmit");
#ifdef USE_SCARD_CONTROL_112
	p[23] = dlsym(lib, "SCardControl132");
#endif
	p[11] = dlsym(lib, "SCardControl");
	p[12] = dlsym(lib, "SCardStatus");
	p[13] = dlsym(lib, "SCardGetStatusChange");
	p[14] = dlsym(lib, "SCardCancel");
	p[15] = dlsym(lib, "SCardSetTimeout");
	p[16] = dlsym(lib, "SCardGetAttrib");
	p[17] = dlsym(lib, "SCardSetAttrib");
	p[18] = dlsym(lib, "SCardFreeMemory");
	p[19] = dlsym(lib, "pcsc_stringify_error");
	p[20] = dlsym(lib, "g_rgSCardRawPci");
	p[21] = dlsym(lib, "g_rgSCardT0Pci");
	p[22] = dlsym(lib, "g_rgSCardT1Pci");
}

extern void __attribute__((destructor)) _fini()
{
	if (lib)
		dlclose(lib);
	if (impl)
		delete impl;
}

extern "C" LONG SCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	eidmw::pcscproxy::t_SCardEstablishContext pps = (eidmw::pcscproxy::t_SCardEstablishContext) p[0];
	return pps(dwScope, pvReserved1, pvReserved2, phContext);
}

extern "C" LONG SCardReleaseContext(SCARDCONTEXT hContext)
{
	eidmw::pcscproxy::t_SCardReleaseContext pps = (eidmw::pcscproxy::t_SCardReleaseContext) p[1];
	return pps(hContext);
}

extern "C" LONG SCardIsValidContext(SCARDCONTEXT hContext)
{
	eidmw::pcscproxy::t_SCardIsValidContext pps = (eidmw::pcscproxy::t_SCardIsValidContext) p[2];
	return pps(hContext);
}

extern "C" LONG SCardListReaders(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders)
{
	return impl->SCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
}

extern "C" LONG SCardListReaderGroups(SCARDCONTEXT hContext, LPSTR mszGroups, LPDWORD pcchGroups)
{
	eidmw::pcscproxy::t_SCardListReaderGroups pps = (eidmw::pcscproxy::t_SCardListReaderGroups) p[4];
	return pps(hContext, mszGroups, pcchGroups);
}

extern "C" LONG SCardConnect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	return impl->SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
}

extern "C" LONG SCardReconnect(SCARDHANDLE hCard, DWORD dwShareMode, DWORD dwPreferredProtocols, DWORD dwInitialization, LPDWORD pdwActiveProtocol)
{
	return impl->SCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
}

extern "C" LONG SCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
	return impl->SCardDisconnect(hCard, dwDisposition);
}

extern "C" LONG SCardBeginTransaction(SCARDHANDLE hCard)
{
	return impl->SCardBeginTransaction(hCard);
}

extern "C" LONG SCardEndTransaction(SCARDHANDLE hCard, DWORD dwDisposition)
{
	return impl->SCardEndTransaction(hCard, dwDisposition);
}

extern "C" LONG SCardTransmit(SCARDHANDLE hCard, LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength, LPSCARD_IO_REQUEST pioRecvPci,
	LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	return impl->SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
}

#ifndef __OLD_PCSC_API__
extern "C" LONG SCardControl(SCARDHANDLE hCard, DWORD dwControlCode,
	LPCVOID pbSendBuffer, DWORD cbSendLength,
	LPVOID pbRecvBuffer, DWORD pcbRecvLength, LPDWORD lpBytesReturned)
{
	return impl->SCardControl(hCard, dwControlCode, pbSendBuffer, cbSendLength, pbRecvBuffer, pcbRecvLength, lpBytesReturned);
}
#else // For Mac OS

#ifdef USE_SCARD_CONTROL_112 // For Mac OS 10.5 or higer
extern "C" LONG SCardControl132(SCARDHANDLE hCard, DWORD dwControlCode,
	LPCVOID pbSendBuffer, DWORD cbSendLength,
	LPVOID pbRecvBuffer, DWORD pcbRecvLength, LPDWORD lpBytesReturned)
{
	return impl->SCardControl132(hCard, dwControlCode, pbSendBuffer, cbSendLength, pbRecvBuffer, pcbRecvLength, lpBytesReturned);
}

extern "C" LONG SCardControl(SCARDHANDLE hCard,
	const void *pbSendBuffer, DWORD cbSendLength,
	void *pbRecvBuffer, LPDWORD pcbRecvLength)
{
	return impl->SCardControl(hCard, (const unsigned char*) pbSendBuffer, cbSendLength, (unsigned char *) pbRecvBuffer, pcbRecvLength);
}

#else // For Mac OS 10.4

extern "C" LONG SCardControl(SCARDHANDLE hCard,
	const unsigned char *pbSendBuffer, DWORD cbSendLength,
	unsigned char *pbRecvBuffer, LPDWORD pcbRecvLength)
{
	return impl->SCardControl(hCard, pbSendBuffer, cbSendLength, pbRecvBuffer, pcbRecvLength);
}

#endif

#endif

extern "C" LONG SCardStatus(SCARDHANDLE hCard, LPSTR szReaderName, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	return impl->SCardStatus(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);

	//eidmw::pcscproxy::t_SCardStatus pps = (eidmw::pcscproxy::t_SCardStatus) p[12];
	//return pps(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}

extern "C" LONG SCardGetStatusChange(SCARDCONTEXT hContext, DWORD dwTimeout, LPSCARD_READERSTATE_A rgReaderStates, DWORD cReaders)
{
	return impl->SCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);

//	eidmw::pcscproxy::t_SCardGetStatusChange pps = (eidmw::pcscproxy::t_SCardGetStatusChange) p[13];
//	return pps(hContext, dwTimeout, rgReaderStates, cReaders);
}

extern "C" LONG SCardCancel(SCARDCONTEXT hContext)
{
	eidmw::pcscproxy::t_SCardCancel pps = (eidmw::pcscproxy::t_SCardCancel) p[14];
	return pps(hContext);
}

extern "C" LONG SCardSetTimeout(SCARDCONTEXT hContext, DWORD dwTimeout)
{
	eidmw::pcscproxy::t_SCardSetTimeout pps = (eidmw::pcscproxy::t_SCardSetTimeout) p[15];
	return pps(hContext, dwTimeout);
}

extern "C" LONG SCardGetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPBYTE pbAttr, LPDWORD pcbAttrLen)
{
	return impl->SCardGetAttrib(hCard, dwAttrId, pbAttr, pcbAttrLen);

	//eidmw::pcscproxy::t_SCardGetAttrib pps = (eidmw::pcscproxy::t_SCardGetAttrib) p[16];
	//return pps(hCard, dwAttrId, pbAttr, pcbAttrLen);
}

extern "C" LONG SCardSetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPCBYTE pbAttr, DWORD cbAttrLen)
{
	eidmw::pcscproxy::t_SCardSetAttrib pps = (eidmw::pcscproxy::t_SCardSetAttrib) p[17];
	return pps(hCard, dwAttrId, pbAttr, cbAttrLen);
}

extern "C" LONG SCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem)
{
	eidmw::pcscproxy::t_SCardFreeMemory pps = (eidmw::pcscproxy::t_SCardFreeMemory) p[18];
	return pps(hContext, pvMem);
}

#ifndef __APPLE__
extern "C" char *pcsc_stringify_error(long error)
{
#else
extern "C" char *pcsc_stringify_error(int32_t error)
{
#endif
	eidmw::pcscproxy::t_pcsc_stringify_error pps = (eidmw::pcscproxy::t_pcsc_stringify_error) p[19];
	return pps(error);
}

SCARD_IO_REQUEST
g_rgSCardT0Pci,
g_rgSCardT1Pci,
g_rgSCardRawPci;

