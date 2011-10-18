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
#ifndef __BWC_TEST_VIRTUAL_H__
#define __BWC_TEST_VIRTUAL_H__

#include <stdio.h>

void PrintTestRawFile1(FILE *f, int Ocsp, int Crl);
void PrintTestRawFile2(FILE *f, int Ocsp, int Crl);
int test_RawFile(const char *folder, const char *reader, int bVerify, int Ocsp, int Crl);

#endif //__BWC_TEST_VIRTUAL_H__
