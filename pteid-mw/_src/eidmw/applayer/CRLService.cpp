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
#include "CRLService.h"
#include "eidErrors.h"
#include "MWException.h"
#include "cryptoFwkBeid.h"
#include "MiscUtil.h"
#include "Log.h"
#include "APLConfig.h"
#include "Config.h"
#include "Util.h"

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


namespace eIDMW
{

/*****************************************************************************************
------------------------------------ APL_CrlService ---------------------------------------
*****************************************************************************************/
APL_CrlService::APL_CrlService(APL_CrlDownloadingCache *cache,APL_CryptoFwk *cryptoFwk)
{
	MWLOG(LEV_INFO, MOD_CRL, L"Create CrlService object");
	m_cache=cache;
	m_cryptoFwk=cryptoFwk;

	APL_Config conf_file(CConfig::EIDMW_CONFIG_PARAM_CRL_LOCKFILE);     
	m_flagfilename=CPathUtil::getFullPath(cache->getCacheDirPath(),conf_file.getString());

	m_flagService=NULL;
	m_control=NULL;
}

APL_CrlService::~APL_CrlService()
{
	MWLOG(LEV_INFO, MOD_CRL, L"Delete CrlService object");
}

//we loop until the main process end
void APL_CrlService::Run()
{
	try
	{
		//We try to catch the flag service to define the mode
		startControl();

		if(m_flagService)
		{
			//DOWNLOAD MODE
			//First we synchronize
			if(!m_bStopRequest)
				m_cache->keepCacheSynchro(m_bStopRequest);

			//Then we refresh the cache from pending download
			if(!m_bStopRequest)
				m_cache->refreshCache(m_bStopRequest,true);
		}

		while(!m_bStopRequest)
		{
			if(m_flagService)
			{
				//DOWNLOAD MODE
				//This thread runs the controle of the download
				if(!m_bStopRequest)
					m_cache->checkForDownload(m_bStopRequest);

				SleepMillisecs(1000);
			}
			else
			{
				//QUIET MODE
				//We wait until the Download mode is release by another process
				//The service swith between process, so it's important not to do a cleanup and keep the pending download
				if(startControl())
					m_cache->refreshCache(m_bStopRequest,false);
				else
					SleepMillisecs(1000);
			}
		}

		stopControl();

	}
	catch (...)
	{
		stopControl();
		m_isRunning = false;
		throw;
	}
}

int APL_CrlService::Start()
{
	return CThread::Start();
}

bool APL_CrlService::startControl()
{
	if(catchFlagFile())
	{
		//Start the control thread
		if(!m_control)
		{
			m_control = new APL_DownloadControl(m_cache,m_cryptoFwk);
			m_control->Start();
		}
		return true;
	}

	return false;
}

void APL_CrlService::stopControl()
{
	if(m_control)
	{
		m_control->Stop();

		delete m_control;
		m_control=NULL;
	}

	releaseFlagFile();
}

bool APL_CrlService::pushNewDownload(const char *uri, const char *file, bool bHighPriority,APL_CrlDownloadingCache *cache)
{
	return m_control->pushNewDownload(uri,file,bHighPriority,cache);
}
bool APL_CrlService::catchFlagFile()
{
	if(m_flagService)
		return false;

	int err=0;

#ifndef WIN32
	m_tFlFlag.l_type   = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	m_tFlFlag.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	m_tFlFlag.l_start  = 0;        /* Offset from l_whence         */
	m_tFlFlag.l_len    = 0;        /* length, 0 = to EOF           */
	m_tFlFlag.l_pid    = getpid(); /* our PID                      */
#endif

#ifdef WIN32
		//As the file is set with hidden attribute, we try to remove it
		remove(m_flagfilename.c_str());
		err = fopen_s(&m_flagService, m_flagfilename.c_str(), "w");
		SetFileAttributesA(m_flagfilename.c_str(),FILE_ATTRIBUTE_HIDDEN);
#else
		m_flagService = fopen(m_flagfilename.c_str(), "w");
		err = (m_flagService == NULL) ? errno : 0;
#endif
		if (err != 0 && err != EACCES) return false;							// Added for unit testing

		if(err==EACCES) 
		{
			m_flagService=NULL;
			return false;
		}

	if(m_flagService==NULL)
		return false;
	
	
#ifdef WIN32
	_lock_file(m_flagService);		//Lock the file to avoid other process to access it
#else 
	// on Linux/Mac we set an advisory lock, i.e. it prevents
	// other processes from using the file only if they are collaborative 
	// and check for the lock, otherwise they can do whatever they like ..
	
	if( fcntl(fileno(m_flagService), F_SETLK, &m_tFlFlag) == -1){  /* set the lock */
	  fclose(m_flagService);
	  m_flagService=NULL;
	  return false;
	}
#endif

	return true;
}

bool APL_CrlService::releaseFlagFile()
{
	//If the file is not open, it's an implementation problem
	if(!m_flagService)
		return false;

	
#ifdef WIN32
	_unlock_file(m_flagService);
#else
	m_tFlFlag.l_type   = F_UNLCK;  /* tell it to unlock the region */
		
	if( fcntl(fileno(m_flagService), F_SETLK, &m_tFlFlag) == -1){ /* set the region to unlocked */
		 printf("APL_CrlDownloadingCache::releaseFlagFile fcntl: %s\n",strerror(errno));
		 exit(1);
	}
#endif
	fclose(m_flagService);

	m_flagService=NULL;

	return true;
}

/*****************************************************************************************
------------------------------------ APL_CrlDownload ---------------------------------------
*****************************************************************************************/
APL_CrlDownloadingCache *APL_CrlDownload::m_cache=NULL;
APL_CryptoFwk *APL_CrlDownload::m_cryptoFwk=NULL;

APL_CrlDownload::APL_CrlDownload(const char *uri, const char *file, bool bHighPriority,unsigned long ulDelay)
{
	MWLOG(LEV_DEBUG, MOD_CRL, L"Create CrlDownload object");
	m_uri=uri;
	m_file=file;
	m_bHighPriority=bHighPriority;
	m_bStarted=false;
	m_Validity="";
	m_ulDownloadingDelay=ulDelay;
}

APL_CrlDownload::~APL_CrlDownload()
{
	//If the download has not starting, we notify the cache
	if(!isStarted())
		if(m_cache)
			m_cache->notifyDownload(m_uri.c_str(),DOWNLOAD_NOTIFY_END_INTERRUPT,m_Validity.c_str());

	MWLOG(LEV_DEBUG, MOD_CRL, L"Delete CrlDownload object");
}

int APL_CrlDownload::Start()
{
	//If the thread start, the flag m_bWaitingStart is set to false
	int iRet=CThread::Start();

	if(!iRet)
	{
		m_bStarted=true;
	}

	return iRet;
}

void APL_CrlDownload::Run()
{
	try
	{
		if(!m_bStopRequest)
		{
			setValidity();
			if(m_cache)
				m_cache->notifyDownload(m_uri.c_str(),DOWNLOAD_NOTIFY_BEGIN,m_Validity.c_str());

			if(download())
			{
			//When the download is finished, we notify the cache
				if(m_cache)
					m_cache->notifyDownload(m_uri.c_str(),DOWNLOAD_NOTIFY_END_OK,m_Validity.c_str());
			}
			else
			{
				if(m_cache)
					m_cache->notifyDownload(m_uri.c_str(),DOWNLOAD_NOTIFY_ERR_CONNECT,m_Validity.c_str());
			}
		}
		resetValidity();

	}
	catch (...)
	{
		if(m_cache)
			m_cache->notifyDownload(m_uri.c_str(),DOWNLOAD_NOTIFY_END_INTERRUPT,m_Validity.c_str());

		m_isRunning = false;
		throw;
	}

}
void APL_CrlDownload::Stop()
{
	if(m_bHighPriority)		//If it's high priority, we wait until download ends
	{
		CThread::WaitTillStopped();
	}
	else
	{
		CThread::Stop();			//We ask for to stop to exit from the run loop
	}
}

bool APL_CrlDownload::isHighPriority()
{
	return m_bHighPriority;
}

bool APL_CrlDownload::isStarted()
{
	return m_bStarted;
}

bool APL_CrlDownload::isFinished()
{
	if(!isStarted())
		return false;

	if(IsRunning())
		return false;

	return true;
}

bool APL_CrlDownload::download()
{
	bool bRet = false;
	CByteArray baData;
    FILE *pf = NULL;  

    if (m_cryptoFwk->downloadFile(m_uri.c_str(), baData,m_bStopRequest))
    {

		//First get the direcory and check if it exist
		std::string directory=CPathUtil::getDir(m_file.c_str());
		CPathUtil::checkDir(directory.c_str());

        // Then save the file
#ifdef WIN32
        fopen_s(&pf,m_file.c_str(), "wb");
#else
        pf = fopen(m_file.c_str(), "wb");
#endif
        if(NULL != pf)
        {
            size_t ulNumWritten = 0;
            ulNumWritten = fwrite(baData.GetBytes(), sizeof(unsigned char), baData.Size(), pf);
            fclose(pf);
            bRet = true;
        }
	}

    return bRet;

}

//Set Downloading timestamp in format YYYYMMDDThhmmss by default
void APL_CrlDownload::setValidity()
{
	CTimestampUtil::getTimestamp(m_Validity,m_ulDownloadingDelay,CDC_VALIDITY_FORMAT);
}

//reset the validity timestamp
void APL_CrlDownload::resetValidity()
{
	m_Validity="";
}

//Return true is Line Validity > now
bool APL_CrlDownload::checkValidity()
{
	if(m_Validity.empty())
		return true;

	return CTimestampUtil::checkTimestamp(m_Validity,CDC_VALIDITY_FORMAT);
}

/*****************************************************************************************
------------------------------------ APL_DownloadControl ---------------------------------------
*****************************************************************************************/
APL_DownloadControl::APL_DownloadControl(APL_CrlDownloadingCache *cache,APL_CryptoFwk *cryptoFwk)
{
	MWLOG(LEV_INFO, MOD_CRL, L"Create DownloadControl object");

	m_cache=cache;
	m_cryptoFwk=cryptoFwk;

	APL_Config conf_MaxNumber(CConfig::EIDMW_CONFIG_PARAM_CRL_SERVDOWNLOADNR);     
	m_ulMaxNumber = conf_MaxNumber.getLong();

	APL_Config conf_DownloadingDelay(CConfig::EIDMW_CONFIG_PARAM_CRL_TIMEOUT);     
	m_ulDownloadingDelay = conf_DownloadingDelay.getLong();

	m_ulCurrentlyRunning=0;

	APL_CrlDownload::setCache(m_cache);
	APL_CrlDownload::setCryptoFwk(m_cryptoFwk);

	srand((int)time(NULL));
}

APL_DownloadControl::~APL_DownloadControl()
{
	MWLOG(LEV_INFO, MOD_CRL, L"Delete DownloadControl object");
}

void APL_DownloadControl::Run()
{

	try
	{
		while(!m_bStopRequest)
		{
			if(!m_bStopRequest)
				deleteOldThreads(false);
			if(!m_bStopRequest)
				startNewThreads();

			if(!m_bStopRequest)
				SleepMillisecs(100);
		}
	}
	catch (...)
	{
		m_isRunning = false;
		throw;
	}
}

bool APL_DownloadControl::pushNewDownload(const char *uri, const char *file, bool bHighPriority,APL_CrlDownloadingCache *cache)
{
	CAutoMutex autoMutex(&m_Mutex);			//To prevent multi-access to vector m_threads

	bool bFound=false;

	std::vector<APL_CrlDownload *>::iterator itr;
	APL_CrlDownload *pThread;

	itr = m_threads.begin();
	while(itr!=m_threads.end())
	{
		pThread=*itr;

		if(strcmp(pThread->getUri(),uri)==0)
		{
			bFound=true;
			break;
		}

		itr++;
	}

	if(!bFound)
	{
		//Create the thread and add it to the vector
		pThread = new APL_CrlDownload(uri,file,bHighPriority,m_ulDownloadingDelay);
		m_threads.push_back(pThread);
	}

	if(bHighPriority)
	{
		pThread->setHighPriority(true);
	}

	return true;
}

void APL_DownloadControl::deleteOldThreads(bool bForceAll)
{
	CAutoMutex autoMutex(&m_Mutex);			//To prevent multi-access to vector m_threads

	std::vector<APL_CrlDownload *>::iterator itr;
	APL_CrlDownload *pThread;

	bool bStillValid;
	bool bDelete;

	itr = m_threads.begin();
	while(itr!=m_threads.end())
	{
		pThread=*itr;

		bStillValid=pThread->checkValidity();
		bDelete=false;

		if(bForceAll)						//If bForceAll	
		{
			pThread->Stop();					//We asked the thread to stop
			bDelete=true;						//Then, we delete it
		}
		else if(pThread->isFinished())		//If the thread finished
		{
			bDelete=true;						//We delete it
		}
		else if(!bStillValid)				//If it not valid any more
		{			
			pThread->Stop();					//We asked the thread to stop
			if(!pThread->isHighPriority())		//and, if it's not a high priority, we delete it
				bDelete=true;					//Else, download will retry soon
		}

		if(bDelete)
		{
			delete pThread;
			itr=m_threads.erase(itr);
			m_ulCurrentlyRunning--;
		}
		else	//If we delete the threads, me don't need to iterate 
		{				//because it has already done by the erase
			itr++;
		}
	} 
}

void APL_DownloadControl::startNewThreads()
{
	CAutoMutex autoMutex(&m_Mutex);			//To prevent multi-access to vector m_threads

	std::vector<APL_CrlDownload *>::iterator itr;
	APL_CrlDownload *pThread;
	
	//We loop 2 times through the vector.
	//The fisrt time we only run all the HighPriority threads
	itr = m_threads.begin();
	while(itr!=m_threads.end())
	{
		pThread=*itr;
		
		//If the thread is not yet started
		if(!pThread->isStarted())
		{
			if(pThread->isHighPriority()) 
			{
				pThread->Start();			//Start the new thread
				m_ulCurrentlyRunning++;
			}
		}

		itr++;
	} 

	//Then if there is place for other threads
	//we made another vector for random selection
	if(m_ulCurrentlyRunning<m_ulMaxNumber)
	{
		std::vector<APL_CrlDownload *> listDownload;

		itr = m_threads.begin();
		while(itr!=m_threads.end())
		{
			pThread=*itr;
			
			//If the thread is not yet started
			if(!pThread->isStarted())
			{
				listDownload.push_back(pThread);
			}
			if(m_ulCurrentlyRunning>=m_ulMaxNumber)
				break;

			itr++;
		} 
		
		while(listDownload.size()>0 && m_ulCurrentlyRunning<m_ulMaxNumber) 
		{
			int idx=(int)((double)listDownload.size()*(double)rand()/(double)RAND_MAX);

			itr = listDownload.begin();
			for(int i=0;i<idx;i++)
			{
				itr++;
			}
			pThread=*itr;

			pThread->Start();			//Start the new thread
			m_ulCurrentlyRunning++;

			listDownload.erase(itr);
		}

	}
}

void APL_DownloadControl::Stop()
{
	CThread::Stop();			//We ask for to stop to exit from the run loop
	deleteOldThreads(true);		//Then we delete all threads
}

/*****************************************************************************************
------------------------------------ APL_CrlDownloadingCache ---------------------------------------
*****************************************************************************************/
APL_CrlDownloadingCache::APL_CrlDownloadingCache(APL_CryptoFwk *cryptoFwk)
{
	MWLOG(LEV_INFO, MOD_CRL, L"Create CrlDownloadingCache object");

	m_cryptoFwk=cryptoFwk;

	APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEDIR);     
	m_cachedirpath = conf_dir.getString();

	APL_Config conf_file(CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEFILE);     

	m_crlextension = "crl";
	m_cachefilename = CPathUtil::getFullPath(m_cachedirpath.c_str(),conf_file.getString());

#ifdef WIN32
	m_cachefilelock=m_cachefilename;
	m_cachefilelock.append(".lck");
	m_flock=NULL;
#endif

	m_crlService=NULL;

	m_f=NULL;

	//Test if the directory exist
	CPathUtil::checkDir(m_cachedirpath.c_str());

	APL_CdcLine::setCache(this);
	APL_CdcLine::setCryptoFwk(m_cryptoFwk);

	//Start the threads
	startAllThreads();
}

APL_CrlDownloadingCache::~APL_CrlDownloadingCache()
{
	if(m_f)				//Should not happen
		closeCacheFile();

	stopAllThreads();

	resetLines();

	MWLOG(LEV_INFO, MOD_CRL, L"Delete CrlDownloadingCache object");
}

void APL_CrlDownloadingCache::startAllThreads()
{
	if(!m_crlService)
	{
		m_crlService=new APL_CrlService(this,m_cryptoFwk);
		m_crlService->Start();
	}

}

void APL_CrlDownloadingCache::stopAllThreads()
{

	if(m_crlService)
	{
		m_crlService->Stop();

		delete m_crlService;
		m_crlService=NULL;
	}
}

void APL_CrlDownloadingCache::resetLines()
{
	std::map<std::string,APL_CdcLine *>::const_iterator itr;
	
	itr = m_lines.begin();
	while(itr!=m_lines.end())
	{
		delete itr->second;
		m_lines.erase(itr->first);
		itr = m_lines.begin();
	} 
}

void APL_CrlDownloadingCache::refreshCache(bool &bStopRequest, bool bCleanUp)
{
	CAutoMutex autoMutex(&m_Mutex);				//To prevent multi-access to cache file and vector m_lines
	
	//We load the cache
	if(!loadCacheFile())
	{
		closeCacheFile();
		return;
	}

	std::map<std::string,APL_CdcLine *>::iterator itr;
	APL_CdcLine *pLine;
	
	//Refresh all the lines
	itr = m_lines.begin();
	while(itr!=m_lines.end())
	{
		pLine=itr->second;

		//If the download has been started by the process that crash, it become High priority
		if(!bCleanUp && (pLine->isDownloadStarted() || pLine->isHighPriority() || pLine->isDownloadRequest()))
			pLine->setDownloadRequest(true);
		else
			pLine->setDownloadRequest(false);

		//No download currently exist
		pLine->setWaitingDownload(false);
		pLine->setHighPriority(false);
		pLine->setDownloadEndBefore("");
		pLine->setErrorCode(DOWNLOAD_STATUS_OK);	

		if(pLine->isValidityEmpty() && pLine->getFileExist())
			pLine->updateValidity(getCacheDirPath());
		
		itr++;

		if(bStopRequest)
			break;
	} 

	//Write the lines to the cache the unused lined are not saved
	writeCacheFile();

}

void APL_CrlDownloadingCache::keepCacheSynchro(bool &bStopRequest)
{
//The cache file and the directories have to stay synchro
//So before checking we loop through directories to see if there is new file
	CAutoMutex autoMutex(&m_Mutex);				//To prevent multi-access to cache file and vector m_lines

	//We load the cache
	if(!loadCacheFile())
	{
		closeCacheFile();
		return;
	}

	//Scan all the file in the cache and create the line if missing
	CPathUtil::scanDir(m_cachedirpath.c_str(),"",m_crlextension.c_str(),bStopRequest,this,&APL_CrlDownloadingCache::setFileExist);
 
	//Write the lines to the cache the unused lined are not saved
	writeCacheFile(true);
}

void APL_CrlDownloadingCache::checkForDownload(bool &bStopRequest)
{

	CAutoMutex autoMutex(&m_Mutex);					//To prevent multi-access to cache file and vector m_lines

	//Load the cache
	if(!loadCacheFile())
	{
		closeCacheFile();
		return;
	}

	std::map<std::string,APL_CdcLine *>::iterator itr;
	APL_CdcLine *pLine;
	bool bPush;
	bool bHighPriority;
	
	//Loop into the map and ask for download if validity expired
	itr = m_lines.begin();
	while(itr!=m_lines.end())
	{
		pLine=itr->second;

		bPush=false;
		bHighPriority=false;

		//If the flag download requested is set,
		//that means that someone is waiting for this crl
		//so we push the download with high priority
		if(pLine->isDownloadRequest())
		{
			bPush=true;
			bHighPriority=true;
		}
		else if(!pLine->isWaitingDownload())	//if download is already pushed => just wait
		{
			if(!pLine->checkValidity())			//if validity is out of date => push
			{
				bPush=true;
			}
		}

		if(bPush)
		{
			//Ask for download
			std::string relativePath=pLine->getRelativePath();
			std::string fullPath=CPathUtil::getFullPath(m_cachedirpath.c_str(),relativePath.c_str());
			std::string uri=CPathUtil::getUri(relativePath.c_str());

			if(m_crlService->pushNewDownload(uri.c_str(),fullPath.c_str(),bHighPriority,this))
			{
				pLine->setWaitingDownload(true);
				pLine->setHighPriority(bHighPriority);
				pLine->setDownloadRequest(false);
				pLine->setErrorCode(DOWNLOAD_STATUS_OK);		
			}
		}

		itr++;

		if(bStopRequest)
			break;
	} 

	writeCacheFile();
}
void APL_CrlDownloadingCache::notifyDownload(const char *uri,tDownloadNotify notify,const char *downloadValidity)
{
	CAutoMutex autoMutex(&m_Mutex);						//To prevent multi-access to cache file and vector m_lines

	if(!loadCacheFile())
	{
		closeCacheFile();
		return;
	}

	APL_CdcLine *pLine=findLineInCache(uri);

	switch(notify)
	{
	case DOWNLOAD_NOTIFY_BEGIN:
		pLine->setWaitingDownload(false);					//Remove the downloading flag
		pLine->setDownloadEndBefore(downloadValidity);		//Download begins => put the flag for the download
		pLine->setErrorCode(DOWNLOAD_STATUS_OK);		
		break;

	case DOWNLOAD_NOTIFY_END_INTERRUPT:
		pLine->setWaitingDownload(false);					//Remove the downloading flag
		pLine->setDownloadEndBefore("");		
		pLine->setErrorCode(DOWNLOAD_STATUS_OK);		
		break;

	case DOWNLOAD_NOTIFY_END_OK:
		pLine->setWaitingDownload(false);					//Remove the downloading flag
		pLine->setHighPriority(false);						//Remove the HighPriority flag
		pLine->setDownloadEndBefore("");		
		pLine->setErrorCode(DOWNLOAD_STATUS_OK);		
		pLine->updateValidity(getCacheDirPath());			//Download ok => get the validity of the crl file
		break;

	case DOWNLOAD_NOTIFY_ERR_CONNECT:
		pLine->setWaitingDownload(false);					//Remove the downloading flag
		pLine->setDownloadEndBefore("");		
		pLine->setErrorCode(DOWNLOAD_STATUS_ERR_CONNECT);	//Set ErrorCode to 1
		break;
	}
	writeCacheFile();
}

APL_CdcLine *APL_CrlDownloadingCache::findLineInCache(const char *uri)
{
	if(!m_f)	//The cache file MUST be open
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	std::map<std::string,APL_CdcLine *>::iterator itr;
	APL_CdcLine *pLine=NULL;
	std::string relativePath=CPathUtil::getRelativePath(uri);

	itr = m_lines.find(relativePath);
	if(itr==m_lines.end())
	{
		//If not yet in the cache, we create the line
        pLine=new APL_CdcLine(relativePath);
        m_lines[relativePath]=pLine;
	}
	else
	{
		pLine=itr->second;
	}

	return pLine;
}

//The validation of the crl goes wrong (date or signature problem)
//so the download is request
void APL_CrlDownloadingCache::forceCacheUpdate(const char *uri,bool bHighPriority)
{
	CAutoMutex autoMutex(&m_Mutex);						//To prevent multi-access to cache file and vector m_lines

	if(!loadCacheFile())
	{
		closeCacheFile();
		return;
	}

	APL_CdcLine *pLine=findLineInCache(uri);

	//if download already start => Do nothing
	//Else =>	Set the download request flag
	if(!pLine->isDownloadStarted())
	{
		pLine->setDownloadRequest(true);
	}

	writeCacheFile();
}

tDownloadStatus APL_CrlDownloadingCache::getCrlDataFromUri(const char *uri, CByteArray &data)
{
	tDownloadStatus eDownloadStatus=DOWNLOAD_STATUS_ERR_UNKNOWN;
	bool bWait=true;
	bool bWrite;
	bool bRequested=false;

	MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: CRL asked URI=%ls",utilStringWiden(uri).c_str());

	std::string file=CPathUtil::getFullPathFromUri(m_cachedirpath.c_str(),uri);

	while(bWait)
	{
		{	//MUTEX SECTION
			CAutoMutex autoMutex(&m_Mutex);

			bWrite=false;

			if(loadCacheFile())
			{
				MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Load cache file status ok");

				APL_CdcLine *pLine=findLineInCache(uri);

				if(bRequested && pLine->getErrorCode()!=DOWNLOAD_STATUS_OK)	//If request but error =>return the error
				{
					MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Already requested but error occured");
					eDownloadStatus=pLine->getErrorCode();
					bWait=false;
				}
				else if(pLine->isDownloadNeedToWait())		//If the download is request or already started => we just need to wait
				{
					MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Wait while downloading");
				}
				else if(pLine->isWaitingDownload())		//If the download is waiting => we force the request (High priority)	
				{
					if(!pLine->isHighPriority())
					{
						MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: High priority => Force download request");
						pLine->setDownloadRequest(true);
						pLine->setErrorCode(DOWNLOAD_STATUS_OK);	
						bWrite=true;
					}
				}
				else									//Else the file in the directory is up to date
				{
					MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Get file from cache directory");
					if(!getCrlDataFromFile(file.c_str(),data))
					{
						MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: File out of date => Force download request");
						pLine->setDownloadRequest(true);
						pLine->setErrorCode(DOWNLOAD_STATUS_OK);	
						bWrite=true;
					}
					else
					{
						eDownloadStatus=DOWNLOAD_STATUS_OK;
						bWait=false;
					}
				}

				if(bWrite)
				{
					writeCacheFile();
					MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Write cache file status");
				}
				else
				{
					closeCacheFile();
					MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Close cache file status");
				}
			}
			else
			{
				closeCacheFile();
			}

		}

		if(bWait)
			CThread::SleepMillisecs(100);

		bRequested=true;
	}

	MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromUri: Return status=%ld",eDownloadStatus);
	return eDownloadStatus;
}

bool APL_CrlDownloadingCache::getCrlDataFromFile(const char *file, CByteArray &crl)
{
	FILE *f=NULL;

	MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: File asked=%ls",utilStringWiden(file).c_str());

#ifdef WIN32
    	errno_t errcode=fopen_s(&f,file, "rb");
	if (errcode != 0) return false;							// Added for unit testing
#else
	f = fopen(file, "rb");
	if (f == NULL) return false;				
#endif

    	if(NULL != f)
    	{
		MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: File open");
        	size_t size = 0;
#ifdef WIN32
        	struct _stat buf = {0};
        	if(0 == _fstat(_fileno(f), &buf))
#else
		struct stat buf = {0};
       	 	if(0 == fstat(fileno(f), &buf))
#endif
        	{
           		unsigned char *pBuffer = (unsigned char *)malloc(buf.st_size);
 			MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: Memory allocated to buffer (%ld bytes)",buf.st_size);

           		size = fread(pBuffer, sizeof(unsigned char), buf.st_size, f);
  			MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: Read data from file (asked for %ld => received %ld)",buf.st_size,size);

            		if(size == (size_t)buf.st_size)
            		{	
				crl.Append(pBuffer,(long)size);
 				MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: Append data to buffer");
           		}
        	}
       		fclose(f);
 		MWLOG(LEV_DEBUG, MOD_CRL, L"APL_CrlDownloadingCache::getCrlDataFromFile: Close file");
	}

	return true;
}

void APL_CrlDownloadingCache::setFileExist(const char *SubDir, const char *File, void *param)
{
  std::map<std::string,APL_CdcLine *>::const_iterator itr;
  APL_CrlDownloadingCache *pCache = static_cast < APL_CrlDownloadingCache * > ( param );
  APL_CdcLine *pLine;
  
  std::string path=SubDir;
  
#ifdef WIN32
  path+=(strlen(SubDir)!=0?"\\":"");
#else
  path+=(strlen(SubDir)!=0 ? "/" : "");
#endif
  path+=File;
  
  if(pCache != NULL){
    itr = pCache->m_lines.find(path);
  
    if(itr==pCache->m_lines.end())
      {
        pLine=new APL_CdcLine(path);
        pCache->m_lines[path]=pLine;
		pLine->updateValidity(pCache->getCacheDirPath());
      }
    else
      {
        pLine=itr->second;
      }
    
    pLine->setFileExist(true);
   } 
}

bool APL_CrlDownloadingCache::loadCacheFile()
{
	//If the file is already open, it's an implementation problem
	if(m_f)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"APL_CrlDownloadingCache::loadCacheFile: File already loaded");
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}

#ifdef WIN32
	//On Windows, we can't reopen an hidden file. We have to remove it then open it again.
	//That make a concurency hole in the write cache file if we put the lock on the cache file it self
	// so we use an other file explicitely for that
	if(m_flock)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"APL_CrlDownloadingCache::loadCacheFile: File already locked");
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
#endif

	resetLines();

	char line[CDC_MAX_LINE_LENGHT];
	APL_CdcLine *pLine;
	int err=0;

	int iLoop;

#ifdef WIN32
	iLoop=0;
	do
	{
		err = 0;

		//As the file is set with hidden attribute, we need to remove it before open it in write mode
		if(0 != remove(m_cachefilelock.c_str()))
			err = errno;

		//If the remove succed or the file does not exist, we open it in write mode
		if(err == 0 || err == ENOENT)
			err = fopen_s(&m_flock, m_cachefilelock.c_str(), "w");

		//If this is an unknown error, we quit
		if (err != 0 && err != EACCES && err != ENOENT ) return false;

		//If something failed, we wait and retry
		if(err != 0) 
			CThread::SleepMillisecs(50);

		iLoop++;
	} while(err != 0 && iLoop<100);

	if(err != 0)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"Could not open the lock file '%s'",utilStringWiden(m_cachefilelock).c_str());
		return false;
	}

	_lock_file(m_flock);
	SetFileAttributesA(m_cachefilelock.c_str(),FILE_ATTRIBUTE_HIDDEN);
#else
	m_tFlCache.l_type   = F_RDLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	m_tFlCache.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	m_tFlCache.l_start  = 0;        /* Offset from l_whence         */
	m_tFlCache.l_len    = 0;        /* length, 0 = to EOF           */
	m_tFlCache.l_pid    = getpid(); /* our PID                      */
#endif

	iLoop=0;
	do
	{
#ifdef WIN32
		err = fopen_s(&m_f, m_cachefilename.c_str(), "r");
#else
		m_f = fopen(m_cachefilename.c_str(), "r");
		err = m_f == NULL ? errno : 0;
#endif
		if (err != 0 && err != EACCES && err != ENOENT )
		{
			MWLOG(LEV_ERROR, MOD_CRL, L"Could not open CRL downloading cache file for reading '%s'",utilStringWiden(m_cachefilename).c_str());
			return false;
		}

		if(err==EACCES) 
			CThread::SleepMillisecs(50);

		iLoop++;
	} while(err==EACCES && iLoop<100);

	if(err==EACCES)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"Could not access CRL downloading cache file for reading '%s'",utilStringWiden(m_cachefilename).c_str());
		return false;
	}

	//If the file doesn't exist, we create it
	if(m_f==NULL)
	{
#ifdef WIN32
		err = fopen_s(&m_f, m_cachefilename.c_str(), "w");
		SetFileAttributesA(m_cachefilename.c_str(),FILE_ATTRIBUTE_HIDDEN);
#else
		m_f = fopen(m_cachefilename.c_str(), "w");
		err = m_f == NULL ? errno : 0;
#endif
		if (err != 0)
		{
			MWLOG(LEV_ERROR, MOD_CRL, L"Could not create CRL downloading cache file '%s'",utilStringWiden(m_cachefilename).c_str());
			return false;
		}
		
#ifndef WIN32
		m_tFlCache.l_type   = F_WRLCK; // has to be the same type as the mode used to open the file
#endif
	}

	if(m_f==NULL)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"Could not open CRL downloading cache file for reading '%s'",utilStringWiden(m_cachefilename).c_str());
		return false;
	}
	
	
#ifdef WIN32
#else 
	// on Linux/Mac we set an advisory lock, i.e. it prevents
	// other processes from using the file only if they are collaborative 
	// and check for the lock, otherwise they can do whatever they like ..
	
	if( fcntl(fileno(m_f), F_SETLKW, &m_tFlCache) == -1){  /* set the lock, waiting if necessary */
	  printf("APL_CrlDownloadingCache::loadCacheFile file 0x%p fcntl: %s\n",m_f,strerror(errno));
	  exit(1);
	}
#endif

    while( fgets (line, CDC_MAX_LINE_LENGHT, m_f) != NULL ) 
	{
		pLine=new APL_CdcLine(line);
		m_lines[pLine->getRelativePath()]=pLine;
	}

	return true;
}

bool APL_CrlDownloadingCache::writeCacheFile(bool bCleanUp)
{
#ifdef WIN32
	if(!m_flock)
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"APL_CrlDownloadingCache::writeCacheFile: File not locked");
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);
	}
#endif

#ifdef WIN32
	fclose(m_f);	//As the file is set with hidden attribute, we need to remove it before open it in write mode
	int err = 0;
	int iLoop = 0;
	do
	{
		err = 0;

		//As the file is set with hidden attribute, we need to remove it before open it in write mode
		if(0 != remove(m_cachefilename.c_str()))
			err = errno;

		//If the remove succed or the file does not exist, we open it in write mode
		if(err == 0 || err == ENOENT)
			err = fopen_s(&m_f, m_cachefilename.c_str(), "w");

		//If this is an unknown error, we quit
		if (err != 0 && err != EACCES && err != ENOENT ) return false;

		//If something failed, we wait and retry
		if(err != 0) 
			CThread::SleepMillisecs(50);

		iLoop++;
	} while(err != 0 && iLoop<100);

	if (err == 0)
	{
		SetFileAttributesA(m_cachefilename.c_str(),FILE_ATTRIBUTE_HIDDEN);
	}
	else
#else
	m_f = freopen(m_cachefilename.c_str(), "w",m_f);
	if (m_f == NULL)		
#endif
	{
		MWLOG(LEV_ERROR, MOD_CRL, L"Could not open CRL downloading cache file for writing '%s'",utilStringWiden(m_cachefilename).c_str());
		closeCacheFile();
		return false;
	}

	std::map<std::string,APL_CdcLine *>::const_iterator itr;
	
	itr = m_lines.begin();
	while(itr!=m_lines.end())
	{
		if(!bCleanUp || itr->second->getFileExist())
		{
			itr->second->writeLine(m_f);
		}
		itr++;
	} 

	return closeCacheFile();
}

bool APL_CrlDownloadingCache::closeCacheFile()
{
	if(m_f)
	{
#ifndef WIN32
		m_tFlCache.l_type   = F_UNLCK;  /* tell it to unlock the region */
		
		if( fcntl(fileno(m_f), F_SETLK, &m_tFlCache) == -1){ /* set the region to unlocked */
		  printf("APL_CrlDownloadingCache::closeCacheFile fcntl: %s\n",strerror(errno));
		  exit(1);
		}
#endif
		fclose(m_f);
	}

	m_f=NULL;

#ifdef WIN32
	if(m_flock)
	{
		_unlock_file(m_flock);
		fclose(m_flock);
		m_flock=NULL;
	}
#endif

	return true;
}

const char *APL_CrlDownloadingCache::getCacheDirPath()
{
	return m_cachedirpath.c_str();
}

/*****************************************************************************************
------------------------------------ APL_CdcLine ---------------------------------------
*****************************************************************************************/
APL_CrlDownloadingCache *APL_CdcLine::m_cache=NULL;
APL_CryptoFwk *APL_CdcLine::m_cryptoFwk=NULL;

//Take a line from the file cache and split it into field
APL_CdcLine::APL_CdcLine(const char *lineIn)
{
	const int NbrField=7;

	char line[CDC_MAX_LINE_LENGHT];

	int i;
	int sep=0;
	char *field[NbrField];

	field[sep++]=&line[0];

	for(i=0;i<CDC_MAX_LINE_LENGHT-1;i++)
	{
		if(lineIn[i]=='\n' || lineIn[i]=='\0')
		{
			break;
		}
		else if(lineIn[i]=='|')
		{
			if(sep>=NbrField)
				break;
			line[i]='\0';
			field[sep++]=&line[i+1];
		}
		else
		{
			line[i]=lineIn[i];
		}
	}
	line[i]='\0';

	if(sep==NbrField)
	{
		m_RelativePath.assign(field[0]);
		m_Validity.assign(field[1]); 
		m_WaitingDownload=(strcmp(field[2],"1")==0?true:false); 
		m_HighPriority=(strcmp(field[3],"1")==0?true:false); 
		m_DownloadEndBefore.assign(field[4]); 
		m_DownloadRequest=(strcmp(field[5],"1")==0?true:false); 
		m_ErrorCode=(tDownloadStatus)atoi(field[6]);
	}
	else
	{	
		m_RelativePath="";
		m_Validity=""; 
		m_WaitingDownload=false;
		m_HighPriority=false;
		m_DownloadEndBefore="";
		m_DownloadRequest=false;
		m_ErrorCode=DOWNLOAD_STATUS_OK;
	}

	m_fileExist=false;
}

APL_CdcLine::APL_CdcLine(const std::string &relativePath)
{
	m_RelativePath=relativePath;
	m_Validity=""; 
	m_WaitingDownload=false;
	m_HighPriority=false;
	m_DownloadEndBefore="";
	m_DownloadRequest=false;
	m_ErrorCode=DOWNLOAD_STATUS_OK;

	m_fileExist=false;
}

APL_CdcLine::~APL_CdcLine()
{
}

//Two lines are equal if the Path are equal
bool APL_CdcLine::operator== (const APL_CdcLine& cvcline)
{
	return isEqual(cvcline.m_RelativePath);
}

bool APL_CdcLine::isEqual (const std::string &relativePath)
{
	if(m_RelativePath.compare(relativePath))
		return true;
	else 
		return false;
}

void APL_CdcLine::updateValidity(const std::string &cachedirpath)
{
#ifdef WIN32
	std::string fullpath=cachedirpath + "\\" + m_RelativePath;
#else
	std::string fullpath= cachedirpath + "/" + m_RelativePath;
#endif
	std::string validityLastUpdate;
	std::string validityNextUpdate;

	CByteArray crl;

	if(m_cache->getCrlDataFromFile(fullpath.c_str(),crl))
	{
		tCrlInfo info;
		m_cryptoFwk->getCrlInfo(crl,info,CDC_VALIDITY_FORMAT);

		m_Validity=info.validityNextUpdate;
	}
}

//PRIVATE: return true is Line Validity > now
bool APL_CdcLine::checkValidity()
{
	return CTimestampUtil::checkTimestamp(m_Validity,CDC_VALIDITY_FORMAT);
}

//Set DownloadEndBefore timestamp in format YYYYMMDDThhmmss by default
void APL_CdcLine::setDownloadEndBefore(const char *downloadValidity)
{
	m_DownloadEndBefore=downloadValidity;
}

//PRIVATE: return true is DownloadEndBefore > now
bool APL_CdcLine::checkDownloadEndBefore()
{
	return CTimestampUtil::checkTimestamp(m_DownloadEndBefore,CDC_VALIDITY_FORMAT);
}

void APL_CdcLine::writeLine(FILE *f)
{
#ifdef WIN32
	fprintf_s(f,"%s|%s|%d|%d|%s|%d|%lu\n",m_RelativePath.c_str(),m_Validity.c_str(),(m_WaitingDownload?1:0),(m_HighPriority?1:0),m_DownloadEndBefore.c_str(),(m_DownloadRequest?1:0),m_ErrorCode);
#else
	fprintf(f,"%s|%s|%d|%d|%s|%d|%u\n",m_RelativePath.c_str(),m_Validity.c_str(),(m_WaitingDownload?1:0),(m_HighPriority?1:0),m_DownloadEndBefore.c_str(),(m_DownloadRequest?1:0),m_ErrorCode);
#endif
}

}
