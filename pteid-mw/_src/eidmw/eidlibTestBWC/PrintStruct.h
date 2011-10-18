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
#ifndef __BWC_PRINT_STRUCT_H__
#define __BWC_PRINT_STRUCT_H__

#include <stdio.h>

#include "eidlib.h"

void PrintStatus(FILE *f, const char *fctname, const BEID_Status *status);
void PrintBytes(FILE *f, const char *text, const BEID_Bytes *bytes);
void PrintBytesLenght(FILE *f, const char *text, const BYTE *bytes, unsigned long length);
void PrintPins(FILE *f, const BEID_Pins *pins);
void PrintPin(FILE *f, const BEID_Pin *pin,long lTriesLeft);
void PrintCertificates(FILE *f, const BEID_Certif_Check *tCheck);
void PrintId(FILE *f, const BEID_ID_Data *idData);
void PrintAddress(FILE *f, const BEID_Address *adData);
void PrintVersionInfo(FILE *f, const BEID_VersionInfo *vdData);
void PrintRawData(FILE *f, const BEID_Raw *rawData);

#endif //__BWC_PRINT_STRUCT_H__
