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
#include <sys/stat.h>
#include "Cache.h"
#include "../common/Util.h"
#include "../common/Config.h"

namespace eIDMW
{
#ifdef WIN32
CHAR test;
#endif
#define MAX_CACHE_SIZE 65000

///////////////// Header for the cache files on Disk /////////////////////

#define HEADER_VERSION  0x10
#pragma pack(push, tCacheHeader, 1)
typedef struct {
	unsigned char version;   /* currently 0x10 */
	unsigned char crc[4];    /* checksum over the contents */
	unsigned char rfu[13];    /* set to 0 for this version */
} tCacheHeader;
#pragma pack(pop, tCacheHeader)
static void MakeHeader(tCacheHeader *header, const CByteArray oData);
static bool CheckHeader(const unsigned char *pucData, unsigned long ulDataLen);

//////////////////////////////////////////////////////////////////////////

CCache::CCache(CContext *poContext) :
	m_poContext(poContext)
{
	m_pucTemp = (unsigned char *) malloc(MAX_CACHE_SIZE);
}

CCache::~CCache(void)
{
	if (m_pucTemp)
		free(m_pucTemp);
	m_MemCache.clear();
}

std::string CCache::GetSimpleName(const std::string & csSerialNr, const std::string & csPath)
{
	return csSerialNr + "_" + csPath + ".bin";
}

CByteArray CCache::GetFile(const std::string & csName,
	bool &bFileFound, bool &bFromDisk,
	unsigned long ulOffset, unsigned long ulMaxLen)
{
	CByteArray oData = MemGetFile(csName);

	// If not present in Memory, then try to get it from Disk
	if (oData.Size() == 0)
	{
		oData = DiskGetFile(csName);
		if (oData.Size() != 0)
		{
			if (bFromDisk)
				MemStoreFile(csName, oData); // Found on disk -> store to Memory
			bFromDisk = true;
		}
		else
			bFromDisk = false;
	}
	else
		bFromDisk = false;

	bFileFound = oData.Size() != 0;

	if (!bFileFound || (ulOffset == 0 && ulMaxLen == FULL_FILE))
		return oData;
	else
	{
		if (ulOffset > oData.Size())
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		if (ulMaxLen > ulOffset + oData.Size())
			ulMaxLen = oData.Size() - ulOffset;
		return CByteArray(oData.GetBytes() + ulOffset, ulMaxLen);
	}
}

void CCache::StoreFile(const std::string & csName,
	const CByteArray &oData, bool bIsFullFile)
{
	// Currently we only store complete files
	if (bIsFullFile)
	{
		MemStoreFile(csName, oData);
		DiskStoreFile(csName, oData);
	}
}

void CCache::StoreFileToMem(const std::string & csName,
	const CByteArray &oData, bool bIsFullFile)
{
	tCacheMap::iterator it = m_MemCache.begin();
	for ( ; it != m_MemCache.end(); it++)
	{
		if (it->first == csName)
		{
			break;
		}
	}

	if (it == m_MemCache.end())
		MemStoreFile(csName, oData);
}

////////////////////////// Memory ////////////////////////

CByteArray CCache::MemGetFile(const std::string & csName)
{
	tCacheMap::iterator it = m_MemCache.begin();
	for ( ; it != m_MemCache.end(); it++)
	{
		if (it->first == csName)
		{
			return it->second;
		}
	}

	// Nothing found: return an empty CByteArray
	return CByteArray();
}

void CCache::MemStoreFile(const std::string & csName,
	const CByteArray &oData)
{
	m_MemCache[csName] = oData;
}

/////////////////////////// Disk /////////////////////////

CByteArray CCache::DiskGetFile(const std::string & csName)
{
	if (m_pucTemp == NULL)
		throw CMWEXCEPTION(EIDMW_ERR_MEMORY);

	if (m_csCacheDir == "")
		m_csCacheDir = GetCacheDir();
	std::string csFileName = m_csCacheDir + csName;

	FILE *f=NULL;
	int err = fopen_s(&f,csFileName.c_str(), "rb");
	if (f == NULL || err !=0 )
		return CByteArray();
	else
	{
		size_t len = fread(m_pucTemp, 1, MAX_CACHE_SIZE, f);
		fclose(f);

		if (!CheckHeader(m_pucTemp, (unsigned long) len))
			return CByteArray();

		return CByteArray(m_pucTemp + sizeof(tCacheHeader),
			(unsigned long) (len - sizeof(tCacheHeader)));
	}
}

void CCache::DiskStoreFile(const std::string & csName,
	const CByteArray &oData)
{
	if (m_csCacheDir == "")
		m_csCacheDir = GetCacheDir();
	std::string csFileName = m_csCacheDir + csName;

	tCacheHeader header;
	MakeHeader(&header, oData);

	FILE *f=NULL;
	int err = fopen_s(&f,csFileName.c_str(), "wb");
	if (f == NULL || err != 0)
		; // TODO: log
	else
	{
		size_t tmpHeader = fwrite(&header, sizeof(tCacheHeader), 1, f);
		tmpHeader = tmpHeader;	//avoid warning
		size_t tmpData   = fwrite(oData.GetBytes(), 1, oData.Size(), f);
		tmpData = tmpData;	//avoid warning
		fclose(f);
	}
}

//////////////////////// Platform-dependent code /////////////////////////

#ifdef WIN32

#include <io.h>
#include <direct.h>
#include <windows.h>

std::string CCache::GetCacheDir(bool bAddSlash)
{
	std::string csCacheDir;

	char *pHomeDir;
	size_t len;
	errno_t err = _dupenv_s( &pHomeDir, &len, "APPDATA" );
	if ( err )
	{
		pHomeDir = (char*)malloc(1);
		pHomeDir[0]=0;
	}

	if (pHomeDir != NULL)
		csCacheDir = pHomeDir + std::string("\\.pteid-ng");
	else
	{
		// Assuming single-user OS: use the Windows dir
		char csPath[_MAX_PATH];
		if (GetWindowsDirectoryA(csPath, sizeof(csPath)) == 0)
			; // TODO: log
		else
			csCacheDir = csPath + std::string("\\.pteid-ng");
	}
	free( pHomeDir );


	DWORD dwError = 0;
	DWORD dwAttr = GetFileAttributesA(csCacheDir.c_str());
	if(dwAttr == INVALID_FILE_ATTRIBUTES) dwError = GetLastError();
	if(dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
	{
		_mkdir(csCacheDir.c_str());
		SetFileAttributesA(csCacheDir.c_str(),FILE_ATTRIBUTE_HIDDEN);
	}

	if (bAddSlash)
		csCacheDir += "\\";


	return csCacheDir;
}

bool CCache::Delete(const std::string & csName)
{
	std::string strCacheDir = GetCacheDir();
	std::string strSearchFor = strCacheDir + csName + "*.bin";
	const char *csSearchFor = strSearchFor.c_str();

	bool bDeleted = false; // wether or no we deleted something

	bool bContinue = true; // continue as long as we deleted a file
	while (bContinue)
	{
		struct _finddata_t c_file;
		intptr_t hFile = _findfirst(csSearchFor, &c_file);
		if (hFile != -1)
		{
			bool bOK = (0 != DeleteFileA((strCacheDir + c_file.name).c_str()));
			_findclose(hFile);
			if (bOK)
			{
				bContinue = true;
				bDeleted = true;
			}
			else
			{
				DWORD dwErr = GetLastError();
				// TODO: log dwErr
				throw CMWEXCEPTION(EIDMW_ERR_DELETE_CACHE);
			}
		}
		else
			bContinue = false;
	}

	return bDeleted;
}

#else

#include <sys/stat.h>
#include <dirent.h>


std::string CCache::GetCacheDir(bool bAddSlash)
{
	std::string csCacheDir;

	//We fist check the config
	csCacheDir = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR).c_str());

	struct stat buffer;
	if ( stat(csCacheDir.c_str(),&buffer))
	{
		mkdir(csCacheDir.c_str(), 0700);
	}

	if (bAddSlash)
		csCacheDir += "/";

	return csCacheDir;
}

bool CCache::Delete(const std::string & strName)
{
	std::string strCacheDirNoSlash = GetCacheDir(false);
	const char *csCacheDirNoSlash = strCacheDirNoSlash.c_str();
	std::string strCacheDir = strCacheDirNoSlash + "/";

	const char *csName = strName.c_str();
	size_t nameLen = strName.size();

	bool bDeleteAll = strName == "";
	bool bDeleted = false; // wether or no we deleted something

	// We loop until all files are deleted
	// The inner loops stops after it deleted 1 file,
	// because continuing to list files of which
	// one has just been deleted could cause troubles
	bool bContinue = true;
	while (bContinue)
	{
		bContinue = false;
		DIR *pDir = opendir(csCacheDirNoSlash);
		if(pDir != NULL)
		{
			struct dirent *pFile = readdir(pDir);
			for ( ;pFile != NULL; pFile = readdir(pDir))
			{
				// Check if this file starts with 'csName'
				if (strcmp(pFile->d_name, "..") != 0 && strcmp(pFile->d_name, ".") != 0 &&
					(bDeleteAll || memcmp(pFile->d_name, csName, nameLen) == 0))
				{
					// Delete the file
					std::string csPath = strCacheDir + pFile->d_name;
					if (! unlink(csPath.c_str()))
					{
						bContinue = true;
						bDeleted = true;
					}
					else
					{
						// TODO: log
						throw CMWEXCEPTION(EIDMW_ERR_DELETE_CACHE);
					}
					break;
				}
			}

			closedir(pDir);
		}
	}

	return bDeleted;
}
#endif

/////////////////////////  Disk cache header + CRC ////////////////////////

/* CRC-32 checksum table, used in PNG, see http://www.w3.org/TR/PNG-CRCAppendix.html */
const static unsigned long crc_table[] = {
	0x00000000,0x77073096,0xee0e612c,0x990951ba,0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d,
};

static void MakeHeader(tCacheHeader *header, const CByteArray oData)
{
	memset(header, 0, sizeof(tCacheHeader));

	header->version = HEADER_VERSION;

	const unsigned char *contents = oData.GetBytes();
	int contentslen = (int) oData.Size();
	unsigned long crc = 0xFFFFFFFF;
	for (int i = 0; i < contentslen; i++)
		crc = crc_table[(crc ^ contents[i]) & 0xff] ^ (crc >> 8);
	crc ^= 0xFFFFFFFF;
	for (int i = 3; i >= 0; i--) {
		header->crc[i] = (unsigned char) (0xFF & crc);
		crc >>= 8;
	}
}

/* Check header version + CRC */
static bool CheckHeader(const unsigned char *pucData, unsigned long ulDataLen)
{
	if (ulDataLen < sizeof(tCacheHeader))
		return false; // No header found

	// Split in header and real contents
	tCacheHeader *header = (tCacheHeader *) pucData;
	const unsigned char *contents = pucData + sizeof(tCacheHeader);
	int contentslen = (int) (ulDataLen - sizeof(tCacheHeader));

	/* Incompatible versions have a different most significant nibble */
	if ((header->version & 0xF0) != (HEADER_VERSION & 0xF0))
		return false; // Unsupported version of this cache file

	unsigned long crc = 0xFFFFFFFF;
	for (int i = 0; i < contentslen; i++)
		crc = crc_table[(crc ^ contents[i]) & 0xff] ^ (crc >> 8);
	crc ^= 0xFFFFFFFF;
	for (int i = 3; i >= 0; i--) {
		if (header->crc[i] != (0xFF & crc))
			return false; // Bad checksum
		crc >>= 8;
	}

	return true;
}

}
