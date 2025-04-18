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

#define LOGTYPE_ERROR 0
#define LOGTYPE_WARNING 1
#define LOGTYPE_INFO 2
#define LOGTYPE_DEBUG 3

#define LOGTYPE_NONE 666

extern void LogInit(void);
extern void LogTrace(int info, const char *pWhere, const char *format, ...);

extern void LogDumpHex(int iStreamLg, unsigned char *pa_cStream);
extern void LogDumpBin(char *fileName, int iStreamLg, unsigned char *pa_cStream);
