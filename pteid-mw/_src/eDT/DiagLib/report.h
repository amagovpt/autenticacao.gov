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
#ifndef __DIAGLIB_REPORT_H__
#define __DIAGLIB_REPORT_H__

#include <stdio.h>

typedef enum e_Report_TYPE
{
	REPORT_TYPE_RESULT,
	REPORT_TYPE_MAIN,
	REPORT_TYPE_COMPLEMENT,
} Report_TYPE;

int reportGetFileName(const wchar_t **fileName, bool bAbstractOnly = false);
int reportInit();

int reportPrint(Report_TYPE type, const wchar_t *format, ...);
int reportPrintLine(Report_TYPE type, const wchar_t *text,wchar_t c, int uppercase, int padding=0);
int reportPrintHex(Report_TYPE type, const wchar_t *title, const unsigned char *buffer, unsigned long len, const wchar_t sep);

int reportPrintHeader1(Report_TYPE type, const wchar_t *text, const wchar_t sep);
int reportPrintHeader2(Report_TYPE type,  const wchar_t *text, const wchar_t sep);
int reportPrintTitle1(Report_TYPE type, const wchar_t *text, const wchar_t sep);
int reportPrintTitle2(Report_TYPE type, const wchar_t *text);
int reportPrintSeparator(Report_TYPE type, const wchar_t sep);

int reportPrintComment(Report_TYPE type, const wchar_t *text);

int reportPrintResult(Report_TYPE type, const wchar_t *text);

int reportUnloadResources();

int reportFinalize(const wchar_t *wzReportAbstract,const wchar_t *wzReportTotal);

#endif //__DIAGLIB_REPORT_H__
