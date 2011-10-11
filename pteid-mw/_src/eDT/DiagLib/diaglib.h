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
#ifndef __DIAGLIB_H__
#define __DIAGLIB_H__

#include <string.h>

#define REPORT_SYSTEM_SEPARATOR		L'#'
#define REPORT_DEVICE_SEPARATOR		L'@'
#define REPORT_SOFTWARE_SEPARATOR	L'*'
#define REPORT_SERVICE_SEPARATOR	L'^'
#define REPORT_PROCESS_SEPARATOR	L'='
#define REPORT_MW_SEPARATOR			L'$'
#define REPORT_READER_SEPARATOR		L'+'
#define REPORT_USER_SEPARATOR		L'£'
#define REPORT_CARD_SEPARATOR		L'!'
#define REPORT_FILE_SEPARATOR		L'§'

const unsigned char DATA_TO_SIGN[] = "This is just a small sample of data to sign.";

const char *diaglibVersion();

#endif //__DIAGLIB_H__

