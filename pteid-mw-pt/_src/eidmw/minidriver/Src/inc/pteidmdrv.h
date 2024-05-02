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
#include "globmdrv.h"

#ifndef __PTEIDMDRV__H__
#define __PTEIDMDRV__H__

DWORD WINAPI CardGetContainerProperty(__in PCARD_DATA pCardData, __in BYTE bContainerIndex, __in LPCWSTR wszProperty,
									  __out_bcount_part_opt(cbData, *pdwDataLen) PBYTE pbData, __in DWORD cbData,
									  __out PDWORD pdwDataLen, __in DWORD dwFlags);
DWORD WINAPI CardSetContainerProperty(__in PCARD_DATA pCardData, __in BYTE bContainerIndex, __in LPCWSTR wszProperty,
									  __in_bcount(cbDataLen) PBYTE pbData, __in DWORD cbDataLen, __in DWORD dwFlags);
DWORD CardGetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetFreeSpace(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetCapabilities(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetKeysizes(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetReadOnly(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetCacheMode(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen,
									   DWORD dwFlags);
DWORD CardSetSupportsWinX509Enrollment(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetGuid(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetSerialNo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinInfo(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetListPins(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetAuthenticatedState(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthVerify(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthChange(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinStrengthUnblock(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetParentWindow(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);
DWORD CardGetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, DWORD dwFlags);
DWORD CardSetPinContextString(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);

DWORD CardSetPropertyUnsupported(PCARD_DATA pCardData, PBYTE pbData, DWORD cbData, DWORD dwFlags);

DWORD WINAPI CardGetProperty(__in PCARD_DATA pCardData, __in LPCWSTR wszProperty,
							 __out_bcount_part_opt(cbData, *pdwDataLen) PBYTE pbData, __in DWORD cbData,
							 __out PDWORD pdwDataLen, __in DWORD dwFlags);
DWORD WINAPI CardSetProperty(__in PCARD_DATA pCardData, __in LPCWSTR wszProperty, __in_bcount(cbDataLen) PBYTE pbData,
							 __in DWORD cbDataLen, __in DWORD dwFlags);
#endif
