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

#ifndef __CRLSERVICE_H__
#define __CRLSERVICE_H__

#include <memory>
#include <map>
#include <string>
#include "Mutex.h"
#include "Thread.h"
#include "APLReader.h"


#ifndef WIN32
#include <fcntl.h>
#endif

namespace eIDMW
{

class APL_CdcLine;
class APL_DownloadControl;
class APL_CrlService;

typedef enum 
{
	DOWNLOAD_NOTIFY_BEGIN,
	DOWNLOAD_NOTIFY_ERR_CONNECT,
	DOWNLOAD_NOTIFY_END_INTERRUPT,
	DOWNLOAD_NOTIFY_END_OK
} tDownloadNotify;
 
typedef enum 
{
	DOWNLOAD_STATUS_OK=0,
	DOWNLOAD_STATUS_ERR_CONNECT,
	DOWNLOAD_STATUS_ERR_UNKNOWN
} tDownloadStatus;

#define CDC_MAX_LINE_LENGHT 250
#define CDC_VALIDITY_FORMAT "%Y%m%dT%H%M%S" //YYYYMMDDThhmmss

class APL_CryptoFwk;
class CByteArray;

/******************************************************************************//**
  * This class is a cache to get CRL from the hard drive 
  *
  *	If it doesn't exist or is not valid any mor it is downloaded
  *********************************************************************************/
class APL_CrlDownloadingCache
{
public:
	/**
	  * Destructor 
	  */
	virtual ~APL_CrlDownloadingCache();

	/**
	  * Refresh the cache to be sure that the previous process did not leave it in a bad state
	  */
	void refreshCache(bool &bStopRequest, bool bCleanUp);

	/**
	  * Keep the cache file synchro with the contain of directory
	  * - Add line for new file
	  * - Remove line if file doesn't exist
	  */
	void keepCacheSynchro(bool &bStopRequest);

	/**
	  * Check if Crl must be download (validity expired)
	  */
	void checkForDownload(bool &bStopRequest);

	/**
	  * Return the directory of the crl cache (m_cachedirpath)
	  */
	const char *getCacheDirPath();

	/**
	  * Force the CRL to be download (because signature failed)
	  */
	void forceCacheUpdate(const char *uri,bool bHighPriority);

	/**
	  * Return the content of a CRL in the byte array
	  *
	  * @param uri is the uri where the file has been downloaded
	  * @param crl is the byte array to receive the data
	  *
	  * @return the status of the download
	  */
	tDownloadStatus getCrlDataFromUri(const char *uri, CByteArray &crl);

	/**
	  * Return the content of a CRL in the byte array
	  *
	  * @param file is the name of the file to load
	  * @param crl is the byte array to receive the data
	  *
	  * @return true if the file is loaded, false if problem occure
	  */
	bool getCrlDataFromFile(const char *file, CByteArray &crl);

	void notifyDownload(const char *uri,tDownloadNotify notify,const char *downloadValidity);

	void startAllThreads();
	void stopAllThreads();

private:

	/**
	  * Constructor
	  */
	APL_CrlDownloadingCache(APL_CryptoFwk *cryptoFwk);

	APL_CrlDownloadingCache(const APL_CrlDownloadingCache &service);			/**< Copy not allowed - not implemented */
	APL_CrlDownloadingCache &operator=(const APL_CrlDownloadingCache &service);	/**< Copy not allowed - not implemented */

	/**
	  * Delete the object in the vector (m_lines) and empty it.
	  */
	void resetLines();

	/**
	  * Load the cache file,
	  * create the APL_CdcLine objects and add it to the vector (m_lines)
	  */
	bool loadCacheFile();

	/**
	  * Write the APL_CdcLine objects in the vector (m_lines) into the cache file
	  */
	bool writeCacheFile(bool bCleanUp=false);

	/**
	  * Close the cache file
	  */
	bool closeCacheFile();
	
	/**
	  * Find the line corresponding to the uri in the cache vector
	  */
	APL_CdcLine *findLineInCache(const char *uri);

	/**
	  * This is a callback function for the scanDir
	  *  - create lines of cache in m_lines if not exist
	  *	 - set the fileExist flag for thes file to true (other file could be removed)
	  *
	  * @param SubDir is the sub directory relative the the Dir of scanDir
	  * @param File is the file to threat
	  * @param param must be this pointer
	  */
	static void setFileExist(const char *SubDir, const char *File, void *param);

	CMutex m_Mutex;									/**< Mutex for exclusive access to the cache file */

	std::string m_cachedirpath;						/**< Path of the crl directory */
	std::string m_cachefilename;					/**< Name of the cache file */
	std::string m_crlextension;						/**< Extension for crl files */

	std::map <std::string,APL_CdcLine*> m_lines;		/**< List of the line in the cache */

	FILE *m_f;											/**< Pointer to the cache file */

	APL_CrlService *m_crlService;						/**< Pointer to CRLService */
	APL_CryptoFwk *m_cryptoFwk;							/**< Pointer to the crypto framework */

#ifdef WIN32
	std::string m_cachefilelock;						/**< Name of the lock file for the cache */
	FILE *m_flock;										/**< Pointer to the lock file */
#else
	struct flock m_tFlCache;
#endif

friend void CAppLayer::startAllServices();				/**< This method must access private constructor */
};

/******************************************************************************//**
  * This class as for goal to maintain the CRLs in the strore up to date
  *********************************************************************************/
class APL_CrlService : public CThread
{
public:
	/**
	  * Destructor 
	  */
	virtual ~APL_CrlService();


	/**
	  * Run the thread
	  */
	virtual void Run();

	bool pushNewDownload(const char *uri, const char *file, bool bHighPriority,APL_CrlDownloadingCache *cache);

private:
	/**
	  * Constructor
	  */
	APL_CrlService(APL_CrlDownloadingCache *cache,APL_CryptoFwk *cryptoFwk);

	APL_CrlService(const APL_CrlService &service);			/**< Copy not allowed - not implemented */
	APL_CrlService &operator=(const APL_CrlService &service);	/**< Copy not allowed - not implemented */

	virtual int Start();										/**< To avoid running 2 threads we make this method private */

	bool startControl();
	void stopControl();
	
	bool catchFlagFile();
	bool releaseFlagFile();

	std::string m_flagfilename;
	FILE *m_flagService;								/**< Pointer to the file CRL service flag*/
	APL_DownloadControl *m_control;						/**< Pointer to the download control thread */
	
	APL_CrlDownloadingCache *m_cache;					/**< Pointer to the APL_CrlDownloadingCache */
	APL_CryptoFwk *m_cryptoFwk;

#ifndef WIN32
	struct flock m_tFlFlag;
#endif

friend void APL_CrlDownloadingCache::startAllThreads();	/**< This method must access private constructor */
};

class APL_CrlDownload;

/******************************************************************************//**
  * The goal of this class is to control the download threads
  * We add new download with pushNewDownload
  * The control class frequently delete threads that are finished (deleteOldThreads)
  *     and start new ones (startNewThreads);
  *********************************************************************************/
class APL_DownloadControl : public CThread
{
public:
	APL_DownloadControl(APL_CrlDownloadingCache *cache,APL_CryptoFwk *cryptoFwk);			/**< Constructor */
	virtual ~APL_DownloadControl();	/**< Destructor */

	virtual void Run();				/**< Run the thread */
	virtual void Stop();			/**< Ask all the threads to stop and wait until they do */
	bool pushNewDownload(const char *uri, const char *file, bool bHighPriority,APL_CrlDownloadingCache *cache); /**< Add new download in the vector */

private:

	APL_DownloadControl(const APL_DownloadControl &control);			/**< Copy not allowed - not implemented */
	APL_DownloadControl &operator=(const APL_DownloadControl &control);	/**< Copy not allowed - not implemented */
	
	/**
	  * Delete the threads object from the vector
	  * if bForceAll = false, only delete threads that has finished
	  * if bForceAll = true, ask all the threads to stop and then delete them
	  */
	void deleteOldThreads(bool bForceAll); 

	/**
	  * Check to see if new download could be run
	  */
	void startNewThreads();

	APL_CrlDownloadingCache *m_cache;	/**< The cache class that ask for the download */
	APL_CryptoFwk *m_cryptoFwk;

	CMutex m_Mutex;										/**< Mutex for exclusive access to m_threads */

	std::vector <APL_CrlDownload *> m_threads;			/**< list of the running download threads */

	unsigned long m_ulMaxNumber;						/**< Maximum download of files in parallel */
	unsigned long m_ulCurrentlyRunning;					/**< Number of download currently running */
	unsigned long m_ulDownloadingDelay;					/**< The delay to download the file */

};

/******************************************************************************//**
  * The goal of this class is to download CRL in a separate thread
  *********************************************************************************/
class APL_CrlDownload : public CThread
{
public:
	APL_CrlDownload(const char *uri, const char *file, bool bHighPriority,unsigned long delay);	/**< Constructor */
	virtual ~APL_CrlDownload();		/**< Destructor */

	virtual int Start();			/**< Start the thread (and place the m_bWaitingStart flag to false) */
	virtual void Run();				/**< Run the thread */
	virtual void Stop();			/**< Ask the thread to stop and wait until it does */

	bool isStarted();				/**< If true, the thread has been started */
	bool isFinished();				/**< If true, the thread has been staarted and has finised */
	bool isHighPriority();			/**< True if the thread is High priority */
	bool checkValidity();			/**< Return true if the download is still valid (Validity > now) */

	const char *getUri()						{return m_uri.c_str();}				/**< Return the Uri */
	void setHighPriority(bool bHighPriority)	{m_bHighPriority=bHighPriority;}	/**< Set the High priority flag */

	static void setCache(APL_CrlDownloadingCache *cache)	{m_cache=cache;}			/**< Define the cache */
	static void setCryptoFwk(APL_CryptoFwk *cryptoFwk)		{m_cryptoFwk=cryptoFwk;}	/**< Define the crypto framework */

private:
	APL_CrlDownload(const APL_CrlDownload &download);				/**< Copy not allowed - not implemented */
	APL_CrlDownload &operator=(const APL_CrlDownload &download);	/**< Copy not allowed - not implemented */
	
	virtual bool download();			/**< Download the file */

	void setValidity();					/**< Set the Validity timestamp in format YYYYMMDDThhmmss by defaul */
	void resetValidity();				/**< Reset the validity timestamp */

	static APL_CrlDownloadingCache *m_cache;	/**< The cache class that ask for the download */
	static APL_CryptoFwk *m_cryptoFwk;

	std::string m_uri;				/**< The uri of the file to download */
	std::string m_file;				/**< The name of the file on disk */

	bool m_bStarted;				/**< True = the download is running or ended - False = the thread is not yet started */
	bool m_bHighPriority;			/**< This download is needed now (by validation process, not by CRL service) */
	unsigned long m_ulDownloadingDelay;	/**< The delay to download the file */
	std::string m_Validity;			/**< Validity of the download */
};	

/******************************************************************************//**
  * Line in the Crl Downloading Cache
  *
  * This class is used by the APL_CrlDownloadingCache
  *********************************************************************************/
class APL_CdcLine
{
public:
	/**
	  * Constructor
	  *
	  * Take a line from the file cache and split it into field
	  */
	APL_CdcLine(const char *line);

	/**
	  * Constructor
	  *
	  * Create a new line with validity = downloading
	  */
	APL_CdcLine(const std::string &path);

	/**
	  * Destructor
	  */
	virtual ~APL_CdcLine(void);

	/**
	  * Two lines are equal if the path are equal
	  */
	bool operator== (const APL_CdcLine& line);

	/**
	  * Compare internal UniqueID AND Flags to the params
	  *
	  * @return true if booth equal
	  */
	bool isEqual (const std::string &path);

	/**
	  * Update the validity with the next update date of the crl
	  */
	void updateValidity(const std::string &cachedirpath);

	/**
	  * Return true if the line in the cache is still valid (Validity > now)
	  */
	bool checkValidity();

	/**
	  * Return true there is no validity
	  */
	bool isValidityEmpty()
		{ return m_Validity.empty();}


	/**
	  * Set DownloadEndBefore timestamp in format YYYYMMDDThhmmss by default
	  */
	void setDownloadEndBefore(const char *downloadValidity);

	/**
	  * Return true if the line in the cache is not time out (DownloadEndBefore > now)
	  */
	bool checkDownloadEndBefore();

	/**
	  * Write the line in the file (f)
	  */
	void writeLine(FILE *f);

	/**
	  * Return the path of the file
	  */
	const char *getRelativePath() 
		{return m_RelativePath.c_str();}

	/**
	  * Return true if the flag m_fileExist has been set
	  */
	bool getFileExist() 
		{return m_fileExist;}

	/**
	  * Set the flag m_fileExist
	  */
	void setFileExist(bool bFileExist) 
		{m_fileExist=bFileExist;}

	/**
	  * Return true if the flag WaitingDownload has been set
	  */
	bool isWaitingDownload() 
		{return m_WaitingDownload;}

	/**
	  * Set the flag WaitingDownload
	  */
	void setWaitingDownload(bool bWaiting) 
		{m_WaitingDownload=bWaiting;}

	/**
	  * Return true if we have to wait for the download
	  *	Either the download already started
	  * or the download has been requested
	  */
	bool isDownloadNeedToWait() 
		{return isDownloadStarted() || isDownloadRequest();}

	/**
	  * Return true if download already started
	  */
	bool isDownloadStarted() 
		{return !m_DownloadEndBefore.empty();}

	/**
	  * Return true if the flag DownloadRequest has been set
	  */
	bool isDownloadRequest() 
		{return m_DownloadRequest;}

	/**
	  * Set the flag DownloadRequest
	  */
	void setDownloadRequest(bool bRequest) 
		{m_DownloadRequest=bRequest;}

	/**
	  * Return true if the flag HighPriority has been set
	  */
	bool isHighPriority() 
		{return m_HighPriority;}

	/**
	  * Set the flag HighPriority
	  */
	void setHighPriority(bool bHighPriority) 
		{m_HighPriority=bHighPriority;}

	/**
	  * Return the Error Code
	  */
	tDownloadStatus getErrorCode() 
		{return m_ErrorCode;}

	/**
	  * Set the Error Code
	  */
	void setErrorCode(tDownloadStatus ulErrorCode) 
		{m_ErrorCode=ulErrorCode;}

	/**
	  * Define the cache
	  */
	static void setCache(APL_CrlDownloadingCache *cache)
		{m_cache=cache;}

	/**
	  * Define the crypto framework
	  */
	static void setCryptoFwk(APL_CryptoFwk *cryptoFwk)
		{m_cryptoFwk=cryptoFwk;}

private:
	APL_CdcLine(const APL_CdcLine& cvc);				/**< Copy not allowed - not implemented */
	APL_CdcLine& operator= (const APL_CdcLine& cvc);	/**< Copy not allowed - not implemented */

	static APL_CrlDownloadingCache *m_cache;
	static APL_CryptoFwk *m_cryptoFwk;

	std::string m_RelativePath;			/**<  Path of the line in the cache */
	std::string m_Validity;				/**<  Validity of the line in the cache */
	bool m_WaitingDownload;				/**<  The download has been asked */
	std::string m_DownloadEndBefore;	/**<  The download started and must end before */
	bool m_DownloadRequest;				/**<  The download has been requested */
	bool m_HighPriority;				/**<  The download is a High Priority */
	tDownloadStatus m_ErrorCode;			/**<  The error code of the download */

	bool m_fileExist;					/**< Use to keep cache synchro - not save in the cache */


};

}

#endif //__CRLSERVICE_H__
