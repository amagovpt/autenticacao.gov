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
#ifndef __DIAGLIB_READER_H__
#define __DIAGLIB_READER_H__

#include <iostream>
#include <string.h>
#include <vector>

#include "report.h"

typedef enum e_Reader_SOURCE {
	DEVICE_READER_SOURCE,
	PCSC_READER_SOURCE,
	MW_READER_SOURCE
} Reader_SOURCE;

typedef struct t_Reader_ID {
	std::wstring Name;
	Reader_SOURCE Source;
} Reader_ID;

typedef std::vector<Reader_ID> Reader_LIST;

typedef struct t_Reader_INFO {
	Reader_ID id;
} Reader_INFO;


typedef std::wstring Card_SERIAL;

typedef struct t_Card_ID {
	Reader_ID Reader;
	Card_SERIAL Serial;
} Card_ID;

typedef std::vector<Card_ID> Card_LIST;
typedef std::vector<unsigned char> Card_FILE;

typedef struct t_Card_INFO {
	Card_ID id;
    std::wstring FirstName;
    std::wstring LastName;
    std::wstring Street;
	bool ReadIdOk;
	Card_FILE FileId;
	bool ReadAddressOk;
	Card_FILE FileAddress;
	bool ReadPictureOk;
	Card_FILE FilePicture;
	bool ReadTokenInfoOk;
	Card_FILE FileTokenInfo;
	bool ReadCertRrnOk;
	Card_FILE FileCertRrn;
	bool ReadCertRootOk;
	Card_FILE FileCertRoot;
	bool ReadCertCaOk;
	Card_FILE FileCertCa;
	bool ReadCertSignOk;
	Card_FILE FileCertSign;
	bool ReadCertAuthOk;
	Card_FILE FileCertAuth;
} Card_INFO;

typedef enum e_Cert_TYPE {
	AUTH_CERT_TYPE,
	SIGN_CERT_TYPE
} Cert_TYPE;

int readerGetListFromDevice(Reader_LIST *readersList);
int readerGetListFromPcsc(Reader_LIST *readersList);
int readerGetListFromMW(Reader_LIST *readersList);
int readerGetInfo(Reader_ID reader, Reader_INFO *info);

int readerGetCardInfo(Card_ID id, Card_INFO *info);

int readerReportInfo(Report_TYPE type, const Reader_INFO &info);
int readerReportList(Report_TYPE type, const Reader_LIST &readerList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_READER_H__

