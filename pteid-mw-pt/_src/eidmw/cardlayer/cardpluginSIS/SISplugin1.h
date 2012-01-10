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
 * This API should be implemented by libraries (also called
 * plugins) that want to support a reader for reading a SIS
 * card with the eID middleware.
 *
 * SIS card plugins should start with siscardplugin1 (Windows)
 * or libsiscardplugin1 (Linux, Mac).
 * The first part of PCSC name of the reader for which the plugin
 * can be used must be put between "__".
 * E.g. for the ACR38U reader, whose PCSC name on Windows is
 * "ACS ACR38 USB Reader 0", a valid plugin name would be
 * "siscardplugin1__ACS ACR38U__.dll" or
 * "siscardplugin1_BE_EID__ACS ACR38U__.dll".
 *
 * The SIS card plugin should export 1 function: the SISPluginReadCard()
 * function declared below.
 *
 * Location:
 * - Windows: "system-dir" + \siscardplugins       eg.: C:\WINDOWS\system32\siscardplugins\siscardplugin1*.dll
 * - Mac, Linux; <PREFIX>/etc/siscardplugins/
 */

#ifndef __SISPLUGIN1_H__
#define __SISPLUGIN1_H__

#ifndef WIN32
#include <wintypes.h>
#define SISPLUGIN_EXPORT
#else
#define SISPLUGIN_EXPORT __declspec(dllexport)
#endif
#include <winscard.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define SISPLUGIN_VERSION 100

/** Wrapper functions to PCSC functions, see the PCSC API for more info. */
typedef struct {
	long (*pCardConnect)(LPCSTR szReader,
		DWORD dwShareMode,
		DWORD dwPreferredProtocols,
		LPSCARDHANDLE phCard,
		LPDWORD pdwActiveProtocol);
	long (*pCardDisconnect)(SCARDHANDLE hCard, DWORD dwDisposition);
	long (*pCardBeginTransaction)(SCARDHANDLE hCard);
	long (*pCardEndTransaction)(SCARDHANDLE hCard, DWORD dwDisposition);
	long (*pCardTransmit)(SCARDHANDLE hCard,
		LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength,
		LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength);
	long (*pCardControl)(SCARDHANDLE hCard, DWORD dwControlCode,
		LPCVOID lpInBuffer, DWORD nInBufferSize,
		LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
	long (*pCtrl)(unsigned long ulControl, const void *pIn, void *pOut);
} tPCSCfunctions;

/**
 * Connect to a SIS card if needed and read it's contents.
 * An SCardEstablishContext() has already been done, as well as an attempt
 * to do an SCardConnect(). If this failed, *phCardHandle is 0 and this function
 * should connect to the card. Otherwise, *phCardHandle differs from 0 and this
 * function can either use the card handle or, if needed, disconnect and connect
 * again.
 * When returning successfull, the 404 data bytes should have been  written to pucData
 * and *phCardHandle should contain a valid card handle that can be used by the
 * middleware in an SCardStatus() function. No SCardDisconnect() should be done
 *
 * - ulVersion: IN major version (= ulVerion / 100) and minor version (= ulVersion % 100);
 *              The major version must be 1, an other value can mean that the API has been changed. 
 *              The minor version numbers could be used to check for optional features (e.g. in ulReserved, pReserved).
 * - pPCSCfunctions: IN pointer to a tPCSCfunctions struct, contains wrapper functions
 *                   for the PCSC functions
 * - csReaderName: IN the reader name as returned by SCardListReaders()
 * - pucData: OUT a buffer to write the SIS card data to (404 bytes)
 * - ulReserved: IN Reserved for future use
 * - pReserved: IN/OUT Reserved for future use
 *
 * Returns: 0 if successfull; other values will only be used for logging purposes
 *          and are not specified.
 */

SISPLUGIN_EXPORT long SISPluginReadCard(unsigned long ulVersion, void *pPCSCfunctions,
	const char *csReaderName, SCARDHANDLE *phCardHandle,
	unsigned char *pucData,
	unsigned long ulReserved, void *pReserved);

#ifdef  __cplusplus
}
#endif

#endif
