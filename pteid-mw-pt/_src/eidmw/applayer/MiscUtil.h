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
#pragma once

#ifndef __APL_MISCSUTIL_H__
#define __APL_MISCSUTIL_H__

#include <string>
#include <cstring> //POSIX basename
#ifdef __APPLE__
#include <libgen.h>
#endif
#include <vector>
#include <map>
#include <time.h>
#include "TLVBuffer.h"
#include <openssl/x509.h>

namespace eIDMW
{

#define PTEID_USER_AGENT "User-Agent: PTeID Middleware v2"
#define PTEID_USER_AGENT_VALUE "PTeID Middleware v2"

 const void *memmem(const void *haystack, size_t n, const void *needle, size_t m);

//Implementation of some utility functions over POSIX and Win32
char * Basename(char *absolute_path);
int Truncate(const char *path);
//Charset conversion
void latin1_to_utf8(unsigned char * in, unsigned char *out);
char * utf8_to_latin1(char * in);

void replace_lastdot_inplace(char *in);

char *X509_to_PEM( X509 *x509 );
X509 *PEM_to_X509( char *pem );

char *Base64Encode(const unsigned char *input, long length);
void Base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len);

//Common type between 2/3 different cpp files
typedef struct _hashed_file_
{
	CByteArray *hash;
	std::string *URI;
} tHashedFile;

/******************************************************************************//**
  * Util class for timestamp features
  *********************************************************************************/
class CTimestampUtil
{
public:

	/**
	  * Return timestamp in format with delay
	  */
	static void getTimestamp(std::string &timestamp,long delay,const char *format);

	/**
	  * return true if timestamp > now
	  */
	static bool checkTimestamp(std::string &timestamp,const char *format);
};

/******************************************************************************//**
  * Util class for path and directory features
  *********************************************************************************/
class CPathUtil
{
public:
	/**
	  * Return the current working directory
	  */
	static std::string getWorkingDir();

	/**
	  * Return the directory from a full path
	  */
	static std::string getDir(const char *path);

	/**
	  * Return true if the file exist
	  */
	static bool existFile(const char *filePath);

	/**
	  * Check directory and create it if not exist
	  */
	static void checkDir(const char *directory);

	/**
	  * Scan the directory and call the call back function for each file corresponding to the extension
	  */
	static void scanDir(const char *Dir,const char *SubDir,const char *ext,bool &bStopRequest,void *param,void (* callback)(const char *SubDir, const char *File, void *param));

	/**
	  * Return the name where the crl file could be found (Relative to the cache dir)
	  */
	static std::string getRelativePath(const char *uri);

	/**
	  * Return the name where the crl file could be found on the hard drive
	  */
	static std::string getFullPath(const char *rootPath, const char *relativePath);

	/**
	  * Return the name where the crl file could be found on the hard drive
	  */
	static std::wstring getFullPath(const wchar_t *rootPath, const wchar_t *relativePath);

	/**
	  * Return the name where the crl file could be found on the hard drive
	  */
	static std::string getFullPathFromUri(const char *rootPath, const char *uri);

	/**
	  * Return basename of file ater removing its extension if it exists
	  */
	static std::string remove_ext_from_basename(const char *base);
};

class CByteArray;

/******************************************************************************//**
  * Util class for parsing CSV file
  *********************************************************************************/
#define CSV_SEPARATOR ';'

class CSVParser
{
public:

	CSVParser(const CByteArray &data, unsigned char separator);

	virtual ~CSVParser();

	unsigned long count();

	const CByteArray &getData(unsigned long idx);

private:
	void parse(const CByteArray &data, unsigned char separator);

	std::vector<CByteArray *> m_vector;
};

class CTLV;

/******************************************************************************//**
  * Util class for parsing TLV file
  *********************************************************************************/
class TLVParser : public CTLVBuffer
{
public:

	TLVParser();

	virtual ~TLVParser();

    CTLV *GetSubTagData(unsigned char ucTag,unsigned char ucSubTag);

private:
	std::map<unsigned char,CTLVBuffer *> m_subfile;
};
}

#endif // __APL_MISCSUTIL_H__
