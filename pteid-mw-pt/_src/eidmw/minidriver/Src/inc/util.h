/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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
#ifndef __UTIL_H__
#define __UTIL_H__
extern void				bin2AsciiHex(const unsigned char * pData, char * out, unsigned long ulLen); 
extern DWORD            PteidGetPubKey(PCARD_DATA  pCardData, DWORD cbCertif, PBYTE pbCertif, DWORD *pcbPubKey, PBYTE *ppbPubKey);
extern DWORD            PteidCreateMSRoots(PCARD_DATA  pCardData, DWORD *pcbMSRoots, PBYTE *ppbMSRoots);
extern BYTE				translateCertType(DWORD dwCertSpec);
extern BOOL runningUnderService();
extern BOOL             ReadReg(LPCTSTR szSubKey, LPCTSTR szValueName, LPDWORD lpType, LPBYTE pbOutput, LPDWORD lpcbOutputSize);

#endif