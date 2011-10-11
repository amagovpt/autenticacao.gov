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
/**
 * Caching of files on an eID card to RAM and to hard disk
 */

#ifndef __CACHE_H__
#define __CACHE_H__

#include "Context.h"
#include <stdlib.h>
#include <map>

namespace eIDMW
{

typedef std::map <std::string, CByteArray> tCacheMap;

class EIDMW_CAL_API CCache
{
public:
    CCache(CContext *poContext);
    ~CCache(void);

	/**
	 * Return the cache file name as a combination of the serialnr and path
	 */
	static std::string GetSimpleName(const std::string & csSerialNr, const std::string & csPath);

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
	CByteArray GetFile(const std::string & csName,
		bool &bFileFound, bool &bFromDisk,
		unsigned long ulOffset = 0, unsigned long ulMaxLen = FULL_FILE);

	/**
	 * Store the data in a file called csName.
	 * If data was already stored, it will be overwritten
	 */
	void StoreFile(const std::string & csName,
		const CByteArray &oData, bool bIsFullFile);

	/**
	 * Store the data to memory only.
	 * If data was already stored, it won't be stored again.
	 * Used in case of we use the Privacy Filter, because then
	 * the data read from disk is not stored to memory until
	 * the PF says it's OK.
	 */
	void StoreFileToMem(const std::string & csName,
		const CByteArray &oData, bool bIsFullFile);

	/**
	 * Delete all the Disk cache files starting with 'csName';
	 * if csName = "" then delete all cache files
	 * Since cache file names start with the card's serial number,
	 * specifying the serial number will cause all cache
	 * files for that specific card to be deleted.
	 * Returns true is something was returned, false otherwise.
	 */
	static bool Delete(const std::string & csName);

protected:
	CByteArray MemGetFile(const std::string & csName);
	void MemStoreFile(const std::string & csName, const CByteArray &oData);

	CByteArray DiskGetFile(const std::string & csName);
	void DiskStoreFile(const std::string & csName, const CByteArray &oData);

	static std::string GetCacheDir(bool bAddSlash = true);

	unsigned char *m_pucTemp;
	CContext *m_poContext;
	std::string m_csCacheDir;

#ifdef WIN32
// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#pragma warning(push)
#pragma warning(disable:4251)
#endif
	tCacheMap m_MemCache;
#ifdef WIN32
#pragma warning(pop)
#endif
};

}

#endif
