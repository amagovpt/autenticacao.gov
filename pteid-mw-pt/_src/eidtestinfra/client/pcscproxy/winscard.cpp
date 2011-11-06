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

#include "WinscardImpl.h"
#include <windows.h>
#include <atlbase.h>

#pragma pack(1)

HINSTANCE                      hLThis = 0;
HINSTANCE                      hL     = 0;
FARPROC                        p[63]  = { 0 };

eidmw::pcscproxy::WinscardImpl *impl = 0;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		hLThis = hInst;
		hL     = LoadLibrary("REALPCSCLIB");
		if (!hL)
		{
			char csPath[_MAX_PATH];
			if (0 != GetSystemDirectory(csPath, sizeof(csPath) - 50))
			{
				strcat_s(csPath, sizeof(csPath), "\\winscard.dll");
				hL = LoadLibrary(csPath);
			}
		}
		if (!hL)
		{
			return false;
		}

		impl = new eidmw::pcscproxy::WinscardImpl(p);

		p[0]  = GetProcAddress(hL, "ClassInstall32");
		p[1]  = GetProcAddress(hL, "SCardAccessNewReaderEvent");
		p[2]  = GetProcAddress(hL, "SCardAccessStartedEvent");
		p[3]  = GetProcAddress(hL, "SCardAddReaderToGroupA");
		p[4]  = GetProcAddress(hL, "SCardAddReaderToGroupW");
		p[5]  = GetProcAddress(hL, "SCardBeginTransaction");
		p[6]  = GetProcAddress(hL, "SCardCancel");
		p[7]  = GetProcAddress(hL, "SCardConnectA");
		p[8]  = GetProcAddress(hL, "SCardConnectW");
		p[9]  = GetProcAddress(hL, "SCardControl");
		p[10] = GetProcAddress(hL, "SCardDisconnect");
		p[11] = GetProcAddress(hL, "SCardEndTransaction");
		p[12] = GetProcAddress(hL, "SCardEstablishContext");
		p[13] = GetProcAddress(hL, "SCardForgetCardTypeA");
		p[14] = GetProcAddress(hL, "SCardForgetCardTypeW");
		p[15] = GetProcAddress(hL, "SCardForgetReaderA");
		p[16] = GetProcAddress(hL, "SCardForgetReaderGroupA");
		p[17] = GetProcAddress(hL, "SCardForgetReaderGroupW");
		p[18] = GetProcAddress(hL, "SCardForgetReaderW");
		p[19] = GetProcAddress(hL, "SCardFreeMemory");
		p[20] = GetProcAddress(hL, "SCardGetAttrib");
		p[21] = GetProcAddress(hL, "SCardGetCardTypeProviderNameA");
		p[22] = GetProcAddress(hL, "SCardGetCardTypeProviderNameW");
		p[23] = GetProcAddress(hL, "SCardGetProviderIdA");
		p[24] = GetProcAddress(hL, "SCardGetProviderIdW");
		p[25] = GetProcAddress(hL, "SCardGetStatusChangeA");
		p[26] = GetProcAddress(hL, "SCardGetStatusChangeW");
		p[27] = GetProcAddress(hL, "SCardIntroduceCardTypeA");
		p[28] = GetProcAddress(hL, "SCardIntroduceCardTypeW");
		p[29] = GetProcAddress(hL, "SCardIntroduceReaderA");
		p[30] = GetProcAddress(hL, "SCardIntroduceReaderGroupA");
		p[31] = GetProcAddress(hL, "SCardIntroduceReaderGroupW");
		p[32] = GetProcAddress(hL, "SCardIntroduceReaderW");
		p[33] = GetProcAddress(hL, "SCardIsValidContext");
		p[34] = GetProcAddress(hL, "SCardListCardsA");
		p[35] = GetProcAddress(hL, "SCardListCardsW");
		p[36] = GetProcAddress(hL, "SCardListInterfacesA");
		p[37] = GetProcAddress(hL, "SCardListInterfacesW");
		p[38] = GetProcAddress(hL, "SCardListReaderGroupsA");
		p[39] = GetProcAddress(hL, "SCardListReaderGroupsW");
		p[40] = GetProcAddress(hL, "SCardListReadersA");
		p[41] = GetProcAddress(hL, "SCardListReadersW");
		p[42] = GetProcAddress(hL, "SCardLocateCardsA");
		p[43] = GetProcAddress(hL, "SCardLocateCardsByATRA");
		p[44] = GetProcAddress(hL, "SCardLocateCardsByATRW");
		p[45] = GetProcAddress(hL, "SCardLocateCardsW");
		p[46] = GetProcAddress(hL, "SCardReconnect");
		p[47] = GetProcAddress(hL, "SCardReleaseAllEvents");
		p[48] = GetProcAddress(hL, "SCardReleaseContext");
		p[49] = GetProcAddress(hL, "SCardReleaseNewReaderEvent");
		p[50] = GetProcAddress(hL, "SCardReleaseStartedEvent");
		p[51] = GetProcAddress(hL, "SCardRemoveReaderFromGroupA");
		p[52] = GetProcAddress(hL, "SCardRemoveReaderFromGroupW");
		p[53] = GetProcAddress(hL, "SCardSetAttrib");
		p[54] = GetProcAddress(hL, "SCardSetCardTypeProviderNameA");
		p[55] = GetProcAddress(hL, "SCardSetCardTypeProviderNameW");
		p[56] = GetProcAddress(hL, "SCardState");
		p[57] = GetProcAddress(hL, "SCardStatusA");
		p[58] = GetProcAddress(hL, "SCardStatusW");
		p[59] = GetProcAddress(hL, "SCardTransmit");
		p[60] = GetProcAddress(hL, "g_rgSCardRawPci");
		p[61] = GetProcAddress(hL, "g_rgSCardT0Pci");
		p[62] = GetProcAddress(hL, "g_rgSCardT1Pci");
	}
	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hL);
		if (impl)
			delete impl;
	}

	return 1;
}

// ClassInstall32
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm
	{
		jmp p[0 * 4];
	}
}

// SCardAccessNewReaderEvent
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1 * 4];
	}
}

// SCardAccessStartedEvent
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2 * 4];
	}
}

// SCardAddReaderToGroupA
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3 * 4];
	}
}

// SCardAddReaderToGroupW
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4 * 4];
	}
}

// SCardBeginTransaction
extern "C" LONG __stdcall __E__5__(IN SCARDHANDLE hCard)
{
	return impl->SCardBeginTransaction(hCard);
}

// SCardCancel
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm
	{
		jmp p[6 * 4];
	}
}

// SCardConnectA
extern "C" LONG __stdcall __E__7__(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol)
{
	return impl->SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
}


// SCardConnectW
extern "C" LONG __stdcall __E__8__(IN SCARDCONTEXT hContext, IN LPCWSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol)
{
	return impl->SCardConnect(hContext, CW2A(szReader), dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
}

// SCardControl
extern "C" LONG __stdcall __E__9__(IN SCARDHANDLE hCard, IN DWORD dwControlCode, IN LPCVOID lpInBuffer,
	IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize,
	OUT LPDWORD lpBytesReturned)
{
	return impl->SCardControl(hCard, dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
}

// SCardDisconnect
extern "C" LONG __stdcall __E__10__(IN SCARDHANDLE hCard, IN DWORD dwDisposition)
{
	return impl->SCardDisconnect(hCard, dwDisposition);
}

// SCardEndTransaction
extern "C" LONG __stdcall __E__11__(IN SCARDHANDLE hCard, IN DWORD dwDisposition)
{
	return impl->SCardEndTransaction(hCard, dwDisposition);
}

// SCardEstablishContext
extern "C" __declspec(naked) void __stdcall __E__12__()
{
	__asm
	{
		jmp p[12 * 4];
	}
}

// SCardForgetCardTypeA
extern "C" __declspec(naked) void __stdcall __E__13__()
{
	__asm
	{
		jmp p[13 * 4];
	}
}

// SCardForgetCardTypeW
extern "C" __declspec(naked) void __stdcall __E__14__()
{
	__asm
	{
		jmp p[14 * 4];
	}
}

// SCardForgetReaderA
extern "C" __declspec(naked) void __stdcall __E__15__()
{
	__asm
	{
		jmp p[15 * 4];
	}
}

// SCardForgetReaderGroupA
extern "C" __declspec(naked) void __stdcall __E__16__()
{
	__asm
	{
		jmp p[16 * 4];
	}
}

// SCardForgetReaderGroupW
extern "C" __declspec(naked) void __stdcall __E__17__()
{
	__asm
	{
		jmp p[17 * 4];
	}
}

// SCardForgetReaderW
extern "C" __declspec(naked) void __stdcall __E__18__()
{
	__asm
	{
		jmp p[18 * 4];
	}
}

// SCardFreeMemory
extern "C" LONG __stdcall __E__19__(IN SCARDCONTEXT hContext, IN LPVOID pvMem)
{
	return impl->SCardFreeMemory(hContext, pvMem);
}

// SCardGetAttrib
extern "C" LONG __stdcall __E__20__(IN SCARDHANDLE hCard, IN DWORD dwAttrId, OUT LPBYTE pbAttr, IN OUT LPDWORD pcbAttrLen)
{
	return impl->SCardGetAttrib(hCard,dwAttrId,pbAttr,pcbAttrLen);
}

// SCardGetCardTypeProviderNameA
extern "C" __declspec(naked) void __stdcall __E__21__()
{
	__asm
	{
		jmp p[21 * 4];
	}
}

// SCardGetCardTypeProviderNameW
extern "C" __declspec(naked) void __stdcall __E__22__()
{
	__asm
	{
		jmp p[22 * 4];
	}
}

// SCardGetProviderIdA
extern "C" __declspec(naked) void __stdcall __E__23__()
{
	__asm
	{
		jmp p[23 * 4];
	}
}

// SCardGetProviderIdW
extern "C" __declspec(naked) void __stdcall __E__24__()
{
	__asm
	{
		jmp p[24 * 4];
	}
}

// SCardGetStatusChangeA
extern "C" LONG __stdcall __E__25__(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates, IN DWORD cReaders)
{
	return impl->SCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);
}

// SCardGetStatusChangeW
extern "C" LONG __stdcall __E__26__(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_W rgReaderStates, IN DWORD cReaders)
{
	//copy to ansi version of readerstates
	LPSCARD_READERSTATE_A a_rgReaderStates = new SCARD_READERSTATE_A[cReaders];
	for (size_t i = 0; i < cReaders; i++)
	{
		//convert szReader to ansi
		size_t readerNameLen = wcslen(rgReaderStates[i].szReader);
		LPSTR  a_szReader    = new CHAR[readerNameLen + 1];
		memcpy(a_szReader, CW2A(rgReaderStates[i].szReader), readerNameLen + 1);
		a_rgReaderStates[i].szReader = a_szReader;
		//copy relevant params
		a_rgReaderStates[i].dwCurrentState = rgReaderStates[i].dwCurrentState;
		a_rgReaderStates[i].dwEventState   = rgReaderStates[i].dwEventState;
	}

	//call ansi version of SCardGetStatusChange
	LONG result = impl->SCardGetStatusChange(hContext, dwTimeout, a_rgReaderStates, cReaders);

	//copy back to existing wide readerstates and
	//!! delete ansi szReader created on the heap above !!
	for (size_t i = 0; i < cReaders; i++)
	{
		//delete szReader
		delete[] a_rgReaderStates[i].szReader;
		//copy relevant params
		rgReaderStates[i].dwEventState = a_rgReaderStates[i].dwEventState;
		rgReaderStates[i].cbAtr        = a_rgReaderStates[i].cbAtr;
		memcpy(rgReaderStates[i].rgbAtr, a_rgReaderStates[i].rgbAtr, 36);
	}

	//delete readerstates
	delete[] a_rgReaderStates;

	return result;
}

// SCardIntroduceCardTypeA
extern "C" __declspec(naked) void __stdcall __E__27__()
{
	__asm
	{
		jmp p[27 * 4];
	}
}

// SCardIntroduceCardTypeW
extern "C" __declspec(naked) void __stdcall __E__28__()
{
	__asm
	{
		jmp p[28 * 4];
	}
}

// SCardIntroduceReaderA
extern "C" __declspec(naked) void __stdcall __E__29__()
{
	__asm
	{
		jmp p[29 * 4];
	}
}

// SCardIntroduceReaderGroupA
extern "C" __declspec(naked) void __stdcall __E__30__()
{
	__asm
	{
		jmp p[30 * 4];
	}
}

// SCardIntroduceReaderGroupW
extern "C" __declspec(naked) void __stdcall __E__31__()
{
	__asm
	{
		jmp p[31 * 4];
	}
}

// SCardIntroduceReaderW
extern "C" __declspec(naked) void __stdcall __E__32__()
{
	__asm
	{
		jmp p[32 * 4];
	}
}

// SCardIsValidContext
extern "C" __declspec(naked) void __stdcall __E__33__()
{
	__asm
	{
		jmp p[33 * 4];
	}
}

// SCardListCardsA
extern "C" __declspec(naked) void __stdcall __E__34__()
{
	__asm
	{
		jmp p[34 * 4];
	}
}

// SCardListCardsW
extern "C" __declspec(naked) void __stdcall __E__35__()
{
	__asm
	{
		jmp p[35 * 4];
	}
}

// SCardListInterfacesA
extern "C" __declspec(naked) void __stdcall __E__36__()
{
	__asm
	{
		jmp p[36 * 4];
	}
}

// SCardListInterfacesW
extern "C" __declspec(naked) void __stdcall __E__37__()
{
	__asm
	{
		jmp p[37 * 4];
	}
}

// SCardListReaderGroupsA
extern "C" __declspec(naked) void __stdcall __E__38__()
{
	__asm
	{
		jmp p[38 * 4];
	}
}

// SCardListReaderGroupsW
extern "C" __declspec(naked) void __stdcall __E__39__()
{
	__asm
	{
		jmp p[39 * 4];
	}
}

// SCardListReadersA
extern "C" LONG __stdcall __E__40__(IN SCARDCONTEXT hContext, IN LPCSTR mszGroups, OUT LPSTR mszReaders, IN OUT LPDWORD pcchReaders)
{
	return impl->SCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
}

// SCardListReadersW
extern "C" LONG __stdcall __E__41__(IN SCARDCONTEXT hContext, IN LPCWSTR mszGroups,
	OUT LPWSTR mszReaders, IN OUT LPDWORD pcchReaders)
{
	CW2A a_mszGroups(mszGroups);
	CW2A a_mszReaders(mszReaders);
	LONG result = impl->SCardListReaders(hContext, a_mszGroups, a_mszReaders, pcchReaders);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, a_mszReaders, *pcchReaders, mszReaders, *pcchReaders);
	return result;
}

// SCardLocateCardsA
extern "C" __declspec(naked) void __stdcall __E__42__()
{
	__asm
	{
		jmp p[42 * 4];
	}
}

// SCardLocateCardsByATRA
extern "C" __declspec(naked) void __stdcall __E__43__()
{
	__asm
	{
		jmp p[43 * 4];
	}
}

// SCardLocateCardsByATRW
extern "C" __declspec(naked) void __stdcall __E__44__()
{
	__asm
	{
		jmp p[44 * 4];
	}
}

// SCardLocateCardsW
extern "C" __declspec(naked) void __stdcall __E__45__()
{
	__asm
	{
		jmp p[45 * 4];
	}
}

// SCardReconnect
extern "C" LONG __stdcall __E__46__(IN SCARDHANDLE hCard, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	IN DWORD dwInitialization, OUT LPDWORD pdwActiveProtocol)
{
	return impl->SCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
}

// SCardReleaseAllEvents
extern "C" __declspec(naked) void __stdcall __E__47__()
{
	__asm
	{
		jmp p[47 * 4];
	}
}

// SCardReleaseContext
extern "C" __declspec(naked) void __stdcall __E__48__()
{
	__asm
	{
		jmp p[48 * 4];
	}
}

// SCardReleaseNewReaderEvent
extern "C" __declspec(naked) void __stdcall __E__49__()
{
	__asm
	{
		jmp p[49 * 4];
	}
}

// SCardReleaseStartedEvent
extern "C" __declspec(naked) void __stdcall __E__50__()
{
	__asm
	{
		jmp p[50 * 4];
	}
}

// SCardRemoveReaderFromGroupA
extern "C" __declspec(naked) void __stdcall __E__51__()
{
	__asm
	{
		jmp p[51 * 4];
	}
}

// SCardRemoveReaderFromGroupW
extern "C" __declspec(naked) void __stdcall __E__52__()
{
	__asm
	{
		jmp p[52 * 4];
	}
}

// SCardSetAttrib
extern "C" __declspec(naked) void __stdcall __E__53__()
{
	__asm
	{
		jmp p[53 * 4];
	}
}

// SCardSetCardTypeProviderNameA
extern "C" __declspec(naked) void __stdcall __E__54__()
{
	__asm
	{
		jmp p[54 * 4];
	}
}

// SCardSetCardTypeProviderNameW
extern "C" __declspec(naked) void __stdcall __E__55__()
{
	__asm
	{
		jmp p[55 * 4];
	}
}

// SCardState
extern "C" __declspec(naked) void __stdcall __E__56__()
{
	__asm
	{
		jmp p[56 * 4];
	}
}

// SCardStatusA
extern "C" LONG __stdcall __E__57__(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
	OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen)
{
	return impl->SCardStatus(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}

// SCardStatusW
extern "C" LONG __stdcall __E__58__(IN SCARDHANDLE hCard, OUT LPWSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
	OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen)
{
	CW2A a_szReaderName(szReaderName);
	LONG result = impl->SCardStatus(hCard, a_szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, a_szReaderName, *pcchReaderLen, szReaderName, *pcchReaderLen);
	return result;
}

// SCardTransmit
extern "C" LONG __stdcall __E__59__(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer, IN DWORD cbSendLength,
	IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	return impl->SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
}

// g_rgSCardRawPci
extern "C" __declspec(naked) void __stdcall __E__60__()
{
	__asm
	{
		jmp p[60 * 4];
	}
}

// g_rgSCardT0Pci
extern "C" __declspec(naked) void __stdcall __E__61__()
{
	__asm
	{
		jmp p[61 * 4];
	}
}

// g_rgSCardT1Pci
extern "C" __declspec(naked) void __stdcall __E__62__()
{
	__asm
	{
		jmp p[62 * 4];
	}
}

