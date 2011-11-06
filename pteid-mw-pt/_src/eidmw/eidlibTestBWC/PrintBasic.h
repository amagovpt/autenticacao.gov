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
#ifndef __BWC_PRINT_BASIC_H__
#define __BWC_PRINT_BASIC_H__

#include <stdio.h>

#define WIDTH_SCREEN	79
#define WIDTH_FILE		96

void PrintLine(FILE *f, const char *text,char c, int screen, int uppercase);
void PrintHex(FILE *f, const char *title, const unsigned char *buffer, unsigned long len);

void PrintTestHeader(FILE *f, const char *text);
void PrintHeader(FILE *f, const char *text);
void PrintTestFunction(FILE *f, const char *text);
void PrintTitle(FILE *f, const char *text);
void PrintBlock(FILE *f, const char *text);
void PrintSeperator(FILE *f);

void PrintComment(FILE *f, const char *text);

void PrintWARNING(FILE *f, const char *text);
void PrintERROR(const char *text);
void PrintSUCCESS(const char *text);

#endif //__BWC_PRINT_BASIC_H__
