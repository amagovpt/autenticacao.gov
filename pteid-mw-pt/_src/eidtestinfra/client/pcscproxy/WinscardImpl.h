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

#ifndef WINSCARDIMPL_H
#define WINSCARDIMPL_H

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <pthread.h>
#include "Mutex.h"
#define IN
#define OUT
#define __stdcall
#define FARPROC    void*
#endif

namespace eidmw { namespace pcscproxy {
#ifndef _WIN32
				  typedef LONG (*t_SCardEstablishContext)(IN DWORD dwScope, IN LPCVOID pvReserved1, IN LPCVOID pvReserved2, OUT LPSCARDCONTEXT phContext);
				  typedef LONG (*t_SCardReleaseContext)(IN SCARDCONTEXT hContext);
				  typedef LONG (*t_SCardIsValidContext)(IN SCARDCONTEXT hContext);
				  typedef LONG (*t_SCardListReaderGroups)(IN SCARDCONTEXT hContext, OUT LPSTR mszGroups, IN OUT LPDWORD pcchGroups);
#ifndef __OLD_PCSC_API__
				  typedef LONG (*t_SCardControl)(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
												 IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
												 OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
#else
				  typedef LONG (*t_SCardControl132)(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
												 IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
												 OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
				  typedef LONG (*t_SCardControl)(SCARDHANDLE hCard,
												 const unsigned char *pbSendBuffer, DWORD cbSendLength,
												 unsigned char *pbRecvBuffer, LPDWORD pcbRecvLength);
#endif
				  typedef LONG (*t_SCardStatus)(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
												OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen);
				  typedef LONG (*t_SCardGetStatusChange)(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates,
														 IN DWORD cReaders);
				  typedef LONG (*t_SCardCancel)(IN SCARDCONTEXT hContext);
				  typedef LONG (*t_SCardSetTimeout)(IN SCARDCONTEXT hContext, IN DWORD dwTimeout);
				  typedef LONG (*t_SCardGetAttrib)(IN SCARDHANDLE hCard, IN DWORD dwAttrId, OUT LPBYTE pbAttr, IN OUT LPDWORD pcbAttrLen);
				  typedef LONG (*t_SCardSetAttrib)(IN SCARDHANDLE hCard, IN DWORD dwAttrId, IN LPCBYTE pbAttr, IN DWORD cbAttrLen);
				  typedef char * (*t_pcsc_stringify_error)(const long pcscError);
#endif

				  typedef LONG (__stdcall * t_SCardBeginTransaction)(IN SCARDHANDLE hCard);
				  typedef LONG (__stdcall * t_SCardConnect)(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
															OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);
				  typedef LONG (__stdcall * t_SCardDisconnect)(IN SCARDHANDLE hCard, IN DWORD dwDisposition);
				  typedef LONG (__stdcall * t_SCardEndTransaction)(IN SCARDHANDLE hCard, IN DWORD dwDisposition);
				  typedef LONG (__stdcall * t_SCardListReaders)(IN SCARDCONTEXT hContext, IN LPCSTR mszGroups, OUT LPSTR mszReaders, IN OUT LPDWORD pcchReaders);
				  typedef LONG (__stdcall * t_SCardReconnect)(IN SCARDHANDLE hCard, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
															  IN DWORD dwInitialization, OUT LPDWORD pdwActiveProtocol);
				  typedef LONG (__stdcall * t_SCardTransmit)(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer, IN DWORD cbSendLength,
															 IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);
				  typedef LONG (__stdcall * t_SCardFreeMemory)(SCARDCONTEXT hContext, LPCVOID pvMem);
				  typedef LONG (__stdcall * t_SCardStatus)(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
														   OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen);
				  typedef LONG (__stdcall * t_SCardGetStatusChange)(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates,
																	IN DWORD cReaders);
				  typedef LONG (__stdcall * t_SCardGetAttrib)(IN SCARDHANDLE hCard,IN DWORD dwAttrId,OUT LPBYTE pbAttr,IN OUT LPDWORD pcbAttrLen);

#ifndef __OLD_PCSC_API__
				  typedef LONG (__stdcall * t_SCardControl)(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
															IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
															OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
#else
				  typedef LONG (__stdcall * t_SCardControl132)(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
															IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
															OUT LPVOID lpOutBuffer, IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
				  typedef LONG (__stdcall * t_SCardControl)(SCARDHANDLE hCard,
															const unsigned char *pbSendBuffer, DWORD cbSendLength,
															unsigned char *pbRecvBuffer, LPDWORD pcbRecvLength);
#endif

#ifdef _WIN32
				  enum WINSCARD_F_INDEX {
					  iSCardBeginTransaction = 5,
					  iSCardConnect          = 7,
					  iSCardControl          = 9,
					  iSCardDisconnect       = 10,
					  iSCardEndTransaction   = 11,
					  iSCardFreeMemory       = 19,
					  iSCardGetAttrib	     = 20,
					  iSCardGetStatusChange  = 25,
					  iSCardListReaders      = 40,
					  iSCardReconnect        = 46,
					  iSCardStatus           = 57,
					  iSCardTransmit         = 59,
					  ig_rgSCardT0Pci        = 61
				  };
#else
				  enum WINSCARD_F_INDEX {
					  iSCardBeginTransaction = 8,
					  iSCardConnect          = 5,
					  iSCardControl132       = 23,
					  iSCardControl          = 11,
					  iSCardDisconnect       = 7,
					  iSCardEndTransaction   = 9,
					  iSCardFreeMemory       = 18,
					  iSCardGetAttrib	     = 16,
					  iSCardGetStatusChange  = 13,
					  iSCardListReaders      = 3,
					  iSCardReconnect        = 6,
					  iSCardStatus           = 12,
					  iSCardTransmit         = 10,
					  ig_rgSCardT0Pci        = 21
				  };
#endif

///////////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
				  const int CTP_USE_THIS_HANDLE = 1;
				  const int CTP_DO_CONNECT = 2;
				  const int CTP_DO_CONNECT_AND_ADD = 3;
				  const int CTP_DO_DISCONNECT = 11;
				  const int CTP_DONT_DISCONNECT = 12;

				  typedef struct {
					std::string reader;
					 pthread_t threadID;
					SCARDHANDLE handle;
					unsigned long counter;
				  } ReaderThreadHandle;

				  class CardThreadPool {
public:
				  CardThreadPool();
				  ~CardThreadPool();

				  /** Returns CTP_USE_THIS_HANDLE, CTP_DO_CONNECT or CTP_DO_CONNECT_AND_ADD */
				  int GetCardHandle(IN LPCSTR szReader, IN OUT LPSCARDHANDLE phCard);
				  void addCardHandle(IN LPCSTR szReader, IN SCARDHANDLE hCard);
				  /**  Returns CTP_DO_DISCONNECT or CTP_DONT_DISCONNECT */
				  int disconnect(IN SCARDHANDLE hCard);

private:
				  std::vector<ReaderThreadHandle> readerThreadHandles;
				  };
#endif

///////////////////////////////////////////////////////////////////////////////////////

				  class SoftReaderManager;
				  class SoftCard;
				  class WinscardImpl {
public:
				  WinscardImpl(const FARPROC*);
				  ~WinscardImpl();

				  LONG SCardBeginTransaction(IN SCARDHANDLE hCard);

				  LONG SCardConnect(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					  OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);

				  LONG SCardDisconnect(IN SCARDHANDLE hCard, IN DWORD dwDisposition);

				  LONG SCardEndTransaction(IN SCARDHANDLE hCard, IN DWORD dwDisposition);

				  LONG SCardListReaders(IN SCARDCONTEXT hContext, IN LPCSTR mszGroups, OUT LPSTR mszReaders, IN OUT LPDWORD pcchReaders);

				  LONG SCardReconnect(IN SCARDHANDLE hCard, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					  IN DWORD dwInitialization, OUT LPDWORD pdwActiveProtocol);

				  LONG SCardTransmit(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer,
					  IN DWORD cbSendLength, IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);

				  LONG SCardFreeMemory(SCARDCONTEXT hContext, LPVOID pvMem);

				  LONG SCardStatus(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
					  OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen);

				  LONG SCardGetStatusChange(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates, IN DWORD cReaders);

				  LONG SCardGetAttrib(IN SCARDHANDLE hCard,IN DWORD dwAttrId,OUT LPBYTE pbAttr,IN OUT LPDWORD pcbAttrLen);
#ifndef __OLD_PCSC_API__
				  LONG SCardControl(IN SCARDHANDLE hCard, IN DWORD dwControlCode, IN LPCVOID lpInBuffer, IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer,
					  IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
#else
				  LONG SCardControl132(IN SCARDHANDLE hCard, IN DWORD dwControlCode, IN LPCVOID lpInBuffer, IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer,
					  IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
				  LONG SCardControl(IN SCARDHANDLE IN hCard,
					  IN const unsigned char *pbSendBuffer, IN DWORD cbSendLength,
					  OUT unsigned char *pbRecvBuffer, OUT LPDWORD pcbRecvLength);
#endif

private:
				  SoftReaderManager * srmngr;
				  const FARPROC     * fps;
				  WinscardImpl();                               //do not implement
				  WinscardImpl(const WinscardImpl&);            //do not implement
				  WinscardImpl& operator=(const WinscardImpl&); //do not implement

				  bool setSelectedOnHardCard(const SoftCard*) const;
				  bool loadSoftData(SoftCard*) const;

				  LONG SCardConnect_Threading(IN t_SCardConnect pps,
					IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);
				  LONG SCardDisconnect_Threading(IN t_SCardDisconnect pps, IN SCARDHANDLE hCard, IN DWORD dwDisposition);
#ifndef _WIN32
				  CardThreadPool cardThreadPool;
				  EidInfra::CMutex cardThreadPoolMutex;
#endif

				  };
				  } //pcscproxy
}                   //eidmw

#endif
