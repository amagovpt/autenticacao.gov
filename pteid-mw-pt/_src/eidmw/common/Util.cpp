/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012, 2015-2016 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#include <algorithm>
#include <codecvt>
#include <fstream>
#include <functional>
#include <vector>
#include <iostream>
#include <iterator>
#include <locale>

#include <openssl/asn1.h>

#ifndef WIN32
#include <unistd.h>
#include "iconv.h"
#endif
#include <stdlib.h>

#include "MWException.h"
#include "Config.h"
#include "eidErrors.h"

#include "Util.h"

char a_cHexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

namespace eIDMW {

std::wstring utilStringWiden(const std::string &in) {
#ifndef WIN32
	if (in.empty())
		return std::wstring();

	iconv_t cd = iconv_open("UTF-32LE", "UTF-8");
	if (cd == (iconv_t)-1) {
		return std::wstring();
	}

	size_t inSize = in.size();
	size_t outSize = (in.length() + 1) * 4;
	std::vector<char> outputBuffer(outSize);

	char* inBuf = const_cast<char*>(in.data());
	char* outBuf = outputBuffer.data();

	size_t inBytesLeft = inSize;
	size_t outBytesLeft = outSize;

	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		return std::wstring();
	}

	iconv_close(cd);

	return std::wstring(reinterpret_cast<wchar_t*>(outputBuffer.data()), (outSize - outBytesLeft) / sizeof(wchar_t));

#else
	int required_size = MultiByteToWideChar(CP_UTF8, 0, in.c_str(), (int)in.size(), NULL, 0);

	if (required_size == 0)
		return std::wstring();

	std::vector<wchar_t> buf(++required_size);

	::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), (int)in.size(), &buf[0], required_size);

	return std::wstring(&buf[0]);
#endif
}

#ifdef _WIN32
std::wstring windowsANSIToWideString(const std::string &in) {
	int required_size = MultiByteToWideChar(CP_ACP, 0, in.c_str(), (int)in.size(), NULL, 0);

	if (required_size == 0)
		return std::wstring();

	std::vector<wchar_t> buf(++required_size);

	::MultiByteToWideChar(CP_ACP, 0, in.c_str(), (int)in.size(), &buf[0], required_size);

	return std::wstring(&buf[0]);
}
#endif

#ifndef _WIN32
/* Using C++11 features
   Adapted from https://en.cppreference.com/w/cpp/locale/wstring_convert */
std::u32string stringWidenUTF32(std::string utf8_str) {
	// the UTF-8 / UTF-32 standard conversion facet
	std::u32string utf32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(utf8_str);

	return utf32;
}
#endif

const unsigned char *findASN1Object(const unsigned char **array, long &size, long tag, const int &maxLength) {
	const unsigned char *old_data = NULL;
	const unsigned char *desc_data = *array;
	int xclass = 0;
	int ans1Tag = 0;
	int returnValue = 0;
	long genTag = 0;

	while (tag != genTag) {
		if (old_data == desc_data)
			return NULL;
		old_data = desc_data; // test this
		long returnValue = ASN1_get_object(&desc_data, &size, &ans1Tag, &xclass, maxLength);
		int constructed = returnValue == V_ASN1_CONSTRUCTED ? 1 : 0;
		genTag = xclass | (constructed & 0b1) << 5 | ans1Tag;
	}

	return desc_data;
}

unsigned short readTwoBytes(const unsigned char *data) { return (data[0] << 8) | data[1]; }

const unsigned char *findASN1Object(const CByteArray &array, long &size, long tag) {
	const unsigned char *desc_data = array.GetBytes();

	return findASN1Object(&desc_data, size, tag, array.Size());
}

std::string utilStringNarrow(const std::wstring &in) {
#ifdef WIN32
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(in);

#else
	if (in.empty())
		return std::string();

	iconv_t cd = iconv_open("UTF-8", "UTF-32LE");
	if (cd == (iconv_t)-1) {
		return std::string();
	}

	size_t inSize = in.size() * sizeof(wchar_t);
	size_t outSize = in.length() * 4;
	std::vector<char> outputBuffer(outSize);

	char* inBuf = reinterpret_cast<char*>(const_cast<wchar_t*>(in.data()));
	char* outBuf = outputBuffer.data();

	size_t inBytesLeft = inSize;
	size_t outBytesLeft = outSize;

	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		return std::string();
	}

	iconv_close(cd);

	return std::string(outputBuffer.data(), outSize - outBytesLeft);
#endif
}

/**
 * Case insensitve search, csSearch should be in lower case.
 * Returns true is csSearch is present in csData.
 */
bool StartsWithCI(const char *csData, const char *csSearch) {
	for (const char *pc1 = csData, *pc2 = csSearch; *pc2 != '\0'; pc1++, pc2++) {
		if ((*pc1 != *pc2) && (*pc1 - 'A' + 'a' != *pc2))
			return false;
	}

	return true;
}

/**
 * Returns true is csSearch is present in csData.
 */
bool StartsWith(const char *csData, const char *csSearch) {
	for (const char *pc1 = csData, *pc2 = csSearch; *pc2 != '\0'; pc1++, pc2++) {
		if (*pc1 != *pc2)
			return false;
	}

	return true;
}

void SubstringInplace(char *buffer, size_t from, size_t to) {
	size_t initial_str_size = strlen(buffer);
	size_t new_str_size = to - from;

	if (from <= to && to <= initial_str_size) {
		std::memmove(buffer, buffer + from, strlen(buffer + from));
		std::memset(buffer + new_str_size, 0, initial_str_size - new_str_size);
	}
}

/* convert binary to ascii-hexadecimal, terminate with a 00-byte
   You have to free the returned buffer yourself !!
 */
char *bin2AsciiHex(const unsigned char *pData, unsigned long ulLen) {
	char *pszHex = new char[ulLen * 2 + 1];
	if (pData != NULL) {
		int j = 0;
		for (unsigned long i = 0; i < ulLen; i++) {
			pszHex[j++] = a_cHexChars[pData[i] >> 4 & 0x0F];
			pszHex[j++] = a_cHexChars[pData[i] & 0x0F];
		}
		pszHex[j] = 0;
	}
	return pszHex;
}

// implementation adapted from https://stackoverflow.com/a/32936928
void truncateUtf8String(std::string &utf8String, size_t numberOfChars) {
	const char *ptr = utf8String.c_str();
	size_t count = 0;
	size_t byteIdx = 0;
	while (*ptr && count < numberOfChars) {
		count += (*ptr++ & 0xC0) != 0x80;
		byteIdx++;
	}
	unsigned char last = utf8String.at(byteIdx - 1);
	// Add remaining UTF-8 continuation bytes if last is a leading byte in a multi-byte char
	if ((last & 0xF0) == 0xF0)
		byteIdx += 3;
	else if ((last & 0xE0) == 0xE0)
		byteIdx += 2;
	else if ((last & 0xC0) == 0xC0)
		byteIdx++;
	utf8String = utf8String.substr(0, byteIdx);
}

#ifdef WIN32
void ReadReg(HKEY hive, const wchar_t *subKey, const wchar_t *leafKey, DWORD *dwType, void *output, DWORD *outputSize) {
	HKEY hKey;
	LONG result = RegOpenKeyEx(hive, subKey, 0, KEY_READ, &hKey);
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		throw CMWEXCEPTION(EIDMW_CONF);
		return;
	}
	result = RegQueryValueExW(hKey, leafKey, NULL, dwType, (LPBYTE)output, outputSize);
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		return;
	}
	RegCloseKey(hKey);
}
void WriteReg(HKEY hive, const wchar_t *subKey, const wchar_t *leafKey, DWORD dwType, void *input, DWORD inputSize) {
	HKEY hKey;

	LONG result = RegCreateKeyEx(hive, subKey, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		throw CMWEXCEPTION(EIDMW_CONF);
		return;
	}
	result = RegSetValueExW(hKey, leafKey, NULL, dwType, (LPBYTE)input, inputSize);
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_BAD);
		return;
	}
	RegCloseKey(hKey);
}
#endif

#ifdef WIN32

void scanDir(const char *Dir, const char *SubDir, const char *Ext, bool &bStopRequest, void *param,
			 std::function<void(const char *, const char *, const char *, void *param)> callback) {
	WIN32_FIND_DATAA FindFileData;
	std::string path;
	std::string subdir;
	HANDLE hFind;
	DWORD a = 0;

	path = Dir;
	path += "\\*.*";

	// Get the first file
	hFind = FindFirstFileA(path.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	while (a != ERROR_NO_MORE_FILES) {

		if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0) {

			path = Dir;
			path += "\\";
			path += FindFileData.cFileName;

			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {

				subdir = SubDir;
				if (strlen(SubDir) != 0)
					subdir += "\\";
				subdir += FindFileData.cFileName;
				scanDir(path.c_str(), subdir.c_str(), Ext, bStopRequest, param, callback);
			} else {
				std::string file = FindFileData.cFileName;
				std::string ext = ".";
				ext += Ext;
				if (strlen(Ext) == 0 ||
					(file.size() > ext.size() && file.compare(file.size() - ext.size(), ext.size(), ext) == 0)) {

					callback(Dir, SubDir, FindFileData.cFileName, param);
				}
			}

			if (bStopRequest)
				break;
		}

		// Get next file
		if (!FindNextFileA(hFind, &FindFileData))
			a = GetLastError();
	}
	FindClose(hFind);
}

#else
#include <sys/stat.h>
#include "dirent.h"
#include "errno.h"

void scanDir(const char *Dir, const char *SubDir, const char *Ext, bool &bStopRequest, void *param,
			 std::function<void(const char *, const char *, const char *, void *param)> callback) {
	std::string path = Dir;
	std::string subdir;

	DIR *pDir = opendir(Dir);

	// If pDir is NULL then the dir doesn't exist
	if (pDir != NULL) {
		struct dirent *pFile = readdir(pDir);

		for (; pFile != NULL; pFile = readdir(pDir)) {

			// skip the . and .. directories
			if (strcmp(pFile->d_name, ".") != 0 && strcmp(pFile->d_name, "..") != 0) {

				path = Dir;
				path += "/";
				path += pFile->d_name;

				// check file attributes
				struct stat buffer;
				if (!stat(path.c_str(), &buffer)) {
					if (S_ISDIR(buffer.st_mode)) {
						// this is a directory: recursive scan
						subdir = SubDir;
						if (strlen(SubDir) != 0)
							subdir += "/";
						subdir += pFile->d_name;

						scanDir(path.c_str(), subdir.c_str(), Ext, bStopRequest, param, callback);
					} else {
						// this is a regular file
						std::string file = pFile->d_name;
						std::string ext = ".";
						ext += Ext;
						// check if the file has the requested extension
						if (strlen(Ext) == 0 || (file.size() > ext.size() &&
												 file.compare(file.size() - ext.size(), ext.size(), ext) == 0)) {
							callback(Dir, SubDir, file.c_str(), param);
						}
					}
				} else {
					// log error
					fprintf(stderr, "CPathUtil::scanDir stat failed: %s\n", strerror(errno));
				}
			}
			if (bStopRequest)
				break;
		}
		closedir(pDir);
	} else {
		// log error
		fprintf(stderr, "CPathUtil::scanDir \"%s\" : %s\n", Dir, strerror(errno));
		return;
	}
}

#endif
} // namespace eIDMW

#ifndef WIN32

#include "assert.h"

int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...) {
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL) {
		r = strcpy_s(buffer, sizeOfBuffer, csTmp);
		free(csTmp);
	}

	return r;
}

int strcat_s(char *dest, size_t len, const char *src) {
	if (dest == NULL)
		return -1;

	for (; *dest != '\0' && len > 1; dest++, len--)
		;

	for (; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

int strcpy_s(char *dest, size_t len, const char *src) {
	if (dest == NULL)
		return -1;

	for (; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

int strncpy_s(char *dest, size_t len, const char *src, long count) {

	if (dest == NULL)
		return -1;

	// On windows _TRUNCATE means that we could copy the maximum of character available
	if (count == _TRUNCATE) {
		for (; len > 1 && *src != '\0'; dest++, src++, len--)
			*dest = *src;

		*dest = '\0';

		return 0; // OK
	} else {
		char *dest_start = dest;
		size_t orig_len = len;

		for (; len > 1 && *src != '\0' && count > 0; dest++, src++, len--, count--)
			*dest = *src;

		*dest = '\0';

		if (*src == '\0' || count == 0)
			return 0; // OK

		if (orig_len > 0)
			*dest_start = '\0';
	}

	return -1;
}

int fopen_s(FILE **pFile, const char *filename, const char *mode) {
	int r = 0;

	if (pFile == NULL)
		return -1;

	FILE *f = fopen(filename, mode);

	if (f != NULL)
		*pFile = f;
	else
		r = -1;

	return r;
}

int wcscpy_s(wchar_t *dest, size_t len, const wchar_t *src) {
	if (dest == NULL)
		return -1;

	for (; len > 1 && *src != '\0'; dest++, src++, len--)
		*dest = *src;

	*dest = '\0';

	return *src == '\0' ? 0 : -1; // 0: OK, -1: NOK
}

EIDMW_CMN_API int fprintf_s(FILE *stream, const char *format, ...) {
	va_list args;
	char *csTmp = NULL;
	int r = -1;

	va_start(args, format);
	r = vasprintf(&csTmp, format, args);
	va_end(args);

	if (r != -1 && csTmp != NULL) {
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}

EIDMW_CMN_API int vfprintf_s(FILE *stream, const char *format, va_list argptr) {
	char *csTmp = NULL;
	int r = -1;

	r = vasprintf(&csTmp, format, argptr);

	if (r != -1 && csTmp != NULL) {
		r = fprintf(stream, "%s", csTmp);
		free(csTmp);
	}

	return r;
}

size_t read_binary_file(const char *filename, unsigned char **outBuffer) {
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		perror("Failed to open file");
		return -1;
	}

	// Seek to the end of the file to determine the file size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file); // Go back to the start of the file

	// Allocate memory for the entire file
	*outBuffer = (unsigned char *)malloc(fileSize);
	if (*outBuffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		fclose(file);
		return -1;
	}

	// Read the file into the buffer
	size_t bytesRead = fread(*outBuffer, 1, fileSize, file);
	if (bytesRead != fileSize) {
		fprintf(stderr, "Error reading file\n");
		free(*outBuffer);
		fclose(file);
		return -1;
	}

	// Close the file
	fclose(file);
	return fileSize;
}

#endif
