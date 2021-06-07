/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2020 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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

#define LOGTYPE_ERROR       0
#define LOGTYPE_WARNING     1
#define LOGTYPE_INFO        2
#define LOGTYPE_TRACE       3

#define LOGTYPE_NONE        666

#ifdef __cplusplus
extern "C" {
#endif

extern void LogInit(void);
extern void LogTrace(int info, const char *pWhere, const char *format, ...);

extern void LogDump(int iStreamLg, unsigned char *pa_cStream);
extern void LogDumpBin(char *pa_cName, int iStreamLg, unsigned char *pa_cStream);

// Get log_level and log_dirname from hKey registry key
// in: hKey (HKEY_CURRENT_USER HKEY_LOCAL_MACHINE HKEY_USERS), cbLogDirname (pbLogDirname length)
// out: dwLogLevel, pbLogDirname
void GetRegLogLevelAndDir(HKEY hKey, INT *dwLogLevel, LPBYTE pbLogDirname, DWORD cbLogDirname);

#ifdef __cplusplus
}
#endif