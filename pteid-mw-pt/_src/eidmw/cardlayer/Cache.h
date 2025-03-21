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
/**
 * Caching of files on an eID card to RAM and to hard disk
 */

#ifndef __CACHE_H__
#define __CACHE_H__

#include "Context.h"
#include <stdlib.h>
#include <map>
#include <functional>

namespace eIDMW {

typedef std::map<std::string, CByteArray> tCacheMap;

class EIDMW_CAL_API CCache {
public:
	CCache(CContext *poContext);
	~CCache(void);

	/**
	 * Return the cache file name as a combination of the serialnr and path
	 */
	static std::string GetSimpleName(const std::string &csSerialNr, const std::string &csPath);

	/**
	 * Return the requested contents.
	 *
	 * If you set bFromDisk to false, then if the contents are read
	 * from hard disk they won't be copied to memory. Upon return,
	 * bFromDisk = true if the contents were read from hard disk,
	 * and false otherwise.
	 * This features allows to consult the PF when contents are cached
	 * on the hard disk but can't be returned before the PF is consulted.
	 * (If the contents are in memory, then the PF doesn't have to be
	 * consulted because it means they were read from the disk earlier,
	 * at which time the PF has been consulted.
	 */
	CByteArray GetFile(const std::string &csName, bool &bFileFound, bool &bFromDisk, unsigned long ulOffset = 0,
					   unsigned long ulMaxLen = FULL_FILE);

	/**
	 * Store the data in a file called csName.
	 * If data was already stored, it will be overwritten
	 */
	void StoreFile(const std::string &csName, const CByteArray &oData, bool bIsFullFile);

	/**
	 * Store the data to memory only.
	 * If data was already stored, it won't be stored again.
	 * Used in case of we use the Privacy Filter, because then
	 * the data read from disk is not stored to memory until
	 * the PF says it's OK.
	 */
	void StoreFileToMem(const std::string &csName, const CByteArray &oData, bool bIsFullFile);

	/**
	 * Delete all the Disk cache files starting with 'csName';
	 * if csName = "" then delete all cache files
	 * Since cache file names start with the card's serial number,
	 * specifying the serial number will cause all cache
	 * files for that specific card to be deleted.
	 * Returns true is something was returned, false otherwise.
	 */
	static bool Delete(const std::string &csName);

	/**
	 * Will try to limit the ammount of cached files on disk.
	 * If the amount of already existing eIDs cached on disk
	 * surpasses the amount passed through _ulMaxCacheFiles_,
	 * the oldest eID files will be permanently deleted.
	 */
	static bool LimitDiskCacheFiles(unsigned long ulMaxCacheFiles);

	/**
	 * Set the encryption key to be used for all cache files
	 * perteining to the current eID.
	 */
	void setEncryptionKey(const CByteArray &newEncryptionKey);

protected:
	CByteArray MemGetFile(const std::string &csName);
	void MemStoreFile(const std::string &csName, const CByteArray &oData);

	CByteArray DiskGetFile(const std::string &csName);
	void DiskStoreFile(const std::string &csName, const CByteArray &oData);

	static void DeleteNonEncryptedFiles();

	static std::string GetCacheDir(bool bAddSlash = true);

	static void CacheDirIterate(std::function<void(const char *FileName, const char *FullPath)> step);

	// Cache encryption
	unsigned int Encrypt(const unsigned char *plaintext, int plaintext_len, const unsigned char *key,
						 const unsigned char *iv, unsigned char *ciphertext);

	unsigned int Decrypt(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key,
						 const unsigned char *iv, unsigned char *plaintext);

	static const unsigned long ENCRYPTION_KEY_LENGTH = 16;
	static constexpr const char *CACHE_EXT = "bin";
	static constexpr const char *ENCRYPTED_CACHE_EXT = "ebin";

	unsigned char *m_pucTemp;
	CContext *m_poContext;
	std::string m_csCacheDir;
	CByteArray encryptionKey = CByteArray(ENCRYPTION_KEY_LENGTH);

#ifdef WIN32
// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
	tCacheMap m_MemCache;
#ifdef WIN32
#pragma warning(pop)
#endif
};

} // namespace eIDMW

#endif
