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
#include <time.h>
#include <stdio.h>

#include "CertStatusCache.h"
#include "APLConfig.h"
#include "MiscUtil.h"
#include "Thread.h"
#include "MWException.h"
#include "eidErrors.h"
#include "Log.h"
#include <errno.h>

#ifndef WIN32
#include "Util.h"
#include <fcntl.h>
#endif

namespace eIDMW 
{

/* ****************
*** APL_CscLine ***
***************** */

//Take a line from the file cache and split it into field
APL_CscLine::APL_CscLine(const char *lineIn)
{
	char line[CSC_MAX_LINE_LENGHT];

	int i;
	int sep=0;
	char *field[4];
	char *stop;

	field[sep++]=&line[0];

	for(i=0;i<CSC_MAX_LINE_LENGHT-1;i++)
	{
		if(lineIn[i]=='\n' || lineIn[i]=='\0')
		{
			break;
		}
		else if(lineIn[i]=='|')
		{
			if(sep>3)
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

	if(sep==4)
	{
		m_ulUniqueID=strtoul(field[0],&stop,10);
		m_ulFlags=strtoul(field[1],&stop,10);
		m_Status=(CSC_Status)atoi(field[2]);
		m_Validity.assign(field[3]); 
	}
	else
	{	
		m_ulUniqueID=0;
		m_ulFlags=0;
		m_Status=CSC_STATUS_NONE;
		m_Validity=""; 
	}
}

APL_CscLine::APL_CscLine(unsigned long ulUniqueID,CSC_Validation validationType)
{
	m_ulUniqueID=ulUniqueID;
	m_ulFlags =	validationType;
				
	m_Status=CSC_STATUS_NONE;
	m_Validity=""; 
}

APL_CscLine::APL_CscLine(unsigned long ulUniqueID,unsigned long ulFlags)
{
	m_ulUniqueID=ulUniqueID;
	m_ulFlags=ulFlags;
	m_Status=CSC_STATUS_NONE;
	m_Validity=""; 
}

APL_CscLine::~APL_CscLine()
{
}

//Two lines are equal if the UniqueID AND the ValidationType are booth equal
bool APL_CscLine::operator== (const APL_CscLine& cscline)
{
	return isEqual(cscline.m_ulUniqueID,cscline.m_ulFlags);
}

bool APL_CscLine::isEqual (unsigned long ulUniqueID,unsigned long ulFlags)
{
	if(m_ulUniqueID==ulUniqueID 
		&& m_ulFlags==ulFlags)
		return true;
	else 
		return false;
}

//Set Validity timestamp in format YYYYMMDDThhmmss by default
void APL_CscLine::setValidity(unsigned long delay)
{
	CTimestampUtil::getTimestamp(m_Validity,delay,CSC_VALIDITY_FORMAT);
}

//PRIVATE: return true is Line Validity > now
bool APL_CscLine::checkValidity()
{
	return CTimestampUtil::checkTimestamp(m_Validity,CSC_VALIDITY_FORMAT);
}

void APL_CscLine::writeLine(FILE *f)
{
#ifdef WIN32
	fprintf_s(f,"%lu|%lu|%d|%s\n",m_ulUniqueID,m_ulFlags,m_Status,m_Validity.c_str());
#else
	fprintf(f,"%lu|%lu|%d|%s\n",m_ulUniqueID,m_ulFlags,m_Status,m_Validity.c_str());
#endif
}

int APL_CscLine::compareValidity(const APL_CscLine &line1,const APL_CscLine &line2)
{
	return line1.m_Validity.compare(line2.m_Validity);
}

/* *********************
*** APL_CertStatusCache ***
********************** */
APL_CertStatusCache::APL_CertStatusCache(APL_CryptoFwk *cryptoFwk)
{
	MWLOG(LEV_INFO, MOD_APL, L"Create CertStatusCache object");

	m_cryptoFwk=cryptoFwk;

	APL_Config conf_file(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_CACHEFILE);     
	m_cachefilename = conf_file.getString();

	APL_Config conf_NbrLine(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_LINENUMB);     
	m_ulMaxNbrLine = conf_NbrLine.getLong();

	APL_Config conf_NormalDelay(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY);     
	m_ulNormalDelay = conf_NormalDelay.getLong();
	
	APL_Config conf_WaitDelay(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_WAITDELAY);     
	m_ulWaitDelay = conf_WaitDelay.getLong();

	m_f=NULL;
}

APL_CertStatusCache::~APL_CertStatusCache(void)
{
	resetLines();

	if(m_f)				//Should not happen
		closeFile();

	MWLOG(LEV_INFO, MOD_APL, L"Delete CertStatusCache object");
}

void APL_CertStatusCache::Init(unsigned long ulMaxNbrLine,unsigned long ulNormalDelay,unsigned long ulWaitDelay,std::string cachefilename)
{
	m_ulMaxNbrLine=ulMaxNbrLine;

	if(ulNormalDelay>0)
		m_ulNormalDelay=ulNormalDelay;

	if(ulWaitDelay>0)
		m_ulWaitDelay=ulWaitDelay;

	if(cachefilename!="")
		m_cachefilename=cachefilename;
}


//Get the certificate status
CSC_Status APL_CertStatusCache::getCertStatus(unsigned long ulUniqueID,const CSC_Validation validationType,APL_Certifs *certStore)
{
	if(certStore==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	CSC_Status status;

	APL_CscLine line(ulUniqueID,validationType);
	unsigned long ulFlags=line.getFlags();

	//Check if the certificate is in the cache and the status still valid
	do
	{
		//If another the status is being validate
		//	=> Wait and re-enter the function
		status=getStatusFromCache(ulUniqueID,ulFlags);

		if(status==CSC_STATUS_WAIT)
			CThread::SleepMillisecs(100);

	} while(status==CSC_STATUS_WAIT);

	//IF NOT YET IN THE CACHE
	if(status==CSC_STATUS_NONE)
	{

		//Run the validation process
		status=checkCertValidation(ulUniqueID,ulFlags,certStore);

		//Add the status to the cache.
		addStatusToCache(ulUniqueID,ulFlags,status);
	}
	return status;
}

//PRIVATE: Check if the certificate is in the cache and still valid
CSC_Status APL_CertStatusCache::getStatusFromCache(unsigned long ulUniqueID,unsigned long ulFlags)
{
	CAutoMutex autoMutex(&m_Mutex);
	APL_CscLine *pLine=NULL;
	int find=-1;

	//Load the file
	loadFile();

	//Find if the certificate is in the cache
	for(unsigned int i=0;i<m_lines.size();i++)
	{
		if(m_lines[i]->isEqual(ulUniqueID,ulFlags))
		{
			pLine=m_lines[i];
			find=i;
			break;
		}
	}

	//If found => check the validity
	//if not valid => erase
	// or if connection problem (in this case, we must try again) => erase
	// or if it was an issuer problem, we try again in the case of the issuer has been added => erase
	// or if it was another error, we try again => erase
	if(pLine)
	{
		if(!pLine->checkValidity() 
			|| pLine->getStatus()==CSC_STATUS_CONNECT
			|| pLine->getStatus()==CSC_STATUS_ISSUER 
			|| pLine->getStatus()==CSC_STATUS_ERROR)
		{
			delete pLine;
			m_lines.erase(m_lines.begin()+find);
			pLine=NULL;
		}
	}

	//If found return the Status
	if(pLine)
	{
		CSC_Status status=pLine->getStatus();
		closeFile();
		return status;
	}

	//IF NOT YET IN THE CACHE

	//Create the line in the file with status=CSC_STATUS_WAIT
	//   and write the file to avoid other process to do the validation
	pLine=new APL_CscLine(ulUniqueID,ulFlags);
	pLine->setStatus(CSC_STATUS_WAIT);
	pLine->setValidity(m_ulWaitDelay);
	m_lines.push_back(pLine);

	writeFile();

	return CSC_STATUS_NONE;
}

CSC_Status APL_CertStatusCache::convertStatus(APL_CertifStatus status)
{
	//Convert the APL CertifStatus into a status for the cache

	switch(status)
	{
	case APL_CERTIF_STATUS_VALID:
		return CSC_STATUS_VALID_SIGN;

	case APL_CERTIF_STATUS_VALID_CRL:
	case APL_CERTIF_STATUS_VALID_OCSP:
		return CSC_STATUS_VALID_FULL;

	case APL_CERTIF_STATUS_CONNECT:
		return CSC_STATUS_CONNECT;

	case APL_CERTIF_STATUS_ISSUER:
		return CSC_STATUS_ISSUER;

	case APL_CERTIF_STATUS_SUSPENDED:
		return CSC_STATUS_SUSPENDED; 

	case APL_CERTIF_STATUS_REVOKED:
		return CSC_STATUS_REVOKED;

	case APL_CERTIF_STATUS_UNKNOWN:
	default:
		return CSC_STATUS_UNKNOWN;
	}
}

//PRIVATE : Do the CRL/OCSP validation
CSC_Status APL_CertStatusCache::checkCertValidation(unsigned long ulUniqueID,unsigned long ulFlags,APL_Certifs *certStore)
{
	CSC_Status issuerstatus;
	CSC_Status certstatus;

	APL_Certif *cert=certStore->getCertUniqueId(ulUniqueID);

	bool bRoot=cert->isRoot();

	//If this is not a root
	if (bRoot)
	{
		return CSC_STATUS_VALID_SIGN;
	}
	else
	{
		//We check the issuer
		APL_Certif *issuer=NULL;
		if(NULL == (issuer = cert->getIssuer()))
			return CSC_STATUS_ISSUER;

		issuerstatus=checkCertValidation(issuer->getUniqueId(),ulFlags,certStore);
		//If the issuer is not valid we return this status
		if(issuerstatus!=CSC_STATUS_VALID_SIGN && issuerstatus!=CSC_STATUS_VALID_FULL)	
			return issuerstatus;
	}

	bool bDateOk=m_cryptoFwk->VerifyDateValidity(cert->getData());

	//Check date validity
	if(!bDateOk)
		return CSC_STATUS_DATE;

	certstatus = convertStatus(cert->validationOCSP());

	//In case the OCSP query failed fallback to CRL
	if (certstatus != CSC_STATUS_SUSPENDED && certstatus != CSC_STATUS_REVOKED &&
	      certstatus != CSC_STATUS_VALID_SIGN)
	{
		MWLOG(LEV_DEBUG, MOD_APL, "DEBUG: falling back to CRL validation for certificate %s: OCSP return code %d",
			 cert->getOwnerName(), certstatus);
		certstatus = convertStatus(cert->validationCRL());
	}


	return certstatus;
}

//PRIVATE : Add the certificate and its status to cache
void APL_CertStatusCache::addStatusToCache(unsigned long ulUniqueID,unsigned long ulFlags,CSC_Status status)
{
	CAutoMutex autoMutex(&m_Mutex);
	APL_CscLine *pLine=NULL;

	//To be sure that the cache has not be modified by another process, we reload it
	loadFile();

	//find the line
	for(unsigned int i=0;i<m_lines.size();i++)
	{
		if(m_lines[i]->isEqual(ulUniqueID,ulFlags))
			pLine=m_lines[i];

	}

	//if not find, we add it
	if(!pLine)
	{
		pLine=new APL_CscLine(ulUniqueID,ulFlags);
		m_lines.push_back(pLine);
	}

	pLine->setStatus(status);
	pLine->setValidity(m_ulNormalDelay);

	//Write the file
	writeFile();
}

void APL_CertStatusCache::resetLines()
{
	while(m_lines.size()>0)
	{
		delete m_lines[m_lines.size()-1];
		m_lines.pop_back();
	}
}

bool APL_CertStatusCache::loadFile()
{
	//If the file is already open, it's an implementation problem
	if(m_f)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	resetLines();

	char line[CSC_MAX_LINE_LENGHT];
	APL_CscLine *pLine;
	int err=0;
	int iLoop;

#ifndef WIN32
	m_tFl.l_type   = F_RDLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK    */
	m_tFl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
	m_tFl.l_start  = 0;        /* Offset from l_whence         */
	m_tFl.l_len    = 0;        /* length, 0 = to EOF           */
	m_tFl.l_pid    = getpid(); /* our PID                      */
#endif

	iLoop=0;
	do
	{
#ifdef WIN32
		err = fopen_s(&m_f, m_cachefilename.c_str(), "r");
#else
		m_f = fopen(m_cachefilename.c_str(), "r");
		if (m_f == NULL) err=errno;		
#endif
		if (err != 0 && err != EACCES && err != ENOENT ) return false;							// Added for unit testing

		if(err==EACCES) 
			CThread::SleepMillisecs(50);

		iLoop++;
	} while(err==EACCES && iLoop<100);

	//If the file doesn't exist, we create it
	if(m_f==NULL)
	{
#ifdef WIN32
		err = fopen_s(&m_f, m_cachefilename.c_str(), "w");
		SetFileAttributesA(m_cachefilename.c_str(),FILE_ATTRIBUTE_HIDDEN);
#else
		m_f = fopen(m_cachefilename.c_str(), "w");
		if (m_f == NULL) err=errno;		
#endif
		if (err != 0) return false;							// Added for unit testing	
#ifndef WIN32
		m_tFl.l_type   = F_WRLCK; // has to be the same type as the mode used to open the file
#endif
	}

	if(m_f==NULL)
		return false;

#ifdef WIN32
	_lock_file(m_f);		//Lock the file to avoid other process to access it
#else
	// on Linux/Mac we set an advisory lock, i.e. it prevents
	// other processes from using the file only if they are collaborative 
	// and check for the lock, otherwise they can do whatever they like ..
	
	if( fcntl(fileno(m_f), F_SETLKW, &m_tFl) == -1){  /* set the lock, waiting if necessary */
	  printf("APL_CertStatusCache::loadFile: fcntl %s\n",strerror(errno));
	  exit(1);
	}
#endif

	while( fgets (line, CSC_MAX_LINE_LENGHT, m_f) != NULL ) 
	{
		pLine=new APL_CscLine(line);
		m_lines.push_back(pLine);
	}

	return true;
}

bool APL_CertStatusCache::writeFile()
{

#ifdef WIN32
	_unlock_file(m_f);
	fclose(m_f);	
	int err = 0;
	int iLoop=0;
	do
	{
		err = 0;

		//As the file is set with hidden attribute, we need to remove it before open it in write mode
		if(0 != remove(m_cachefilename.c_str()))
			err = errno;

		//If the remove succeeded or the file does not exist, we open it in write mode
		if(err == 0 || err == ENOENT)
			err = fopen_s(&m_f, m_cachefilename.c_str(), "w");

		//If this is an unknown error, we quit
		if (err != 0 && err != EACCES && err != ENOENT ) return false;

		//If something failed, we wait and retry
		if(err != 0) 
			CThread::SleepMillisecs(50);

		iLoop++;
	} while(err != 0 && iLoop<100);

	if (err != 0) 
	{
		closeFile();
		return false; // Added for unit testing
	}							
	_lock_file(m_f);
	SetFileAttributesA(m_cachefilename.c_str(),FILE_ATTRIBUTE_HIDDEN);
#else
	m_f = freopen(m_cachefilename.c_str(), "w",m_f);
	if (m_f == NULL) return false;		
#endif

	//If there is to much line, we have to delete the oldest
	while(m_lines.size()>m_ulMaxNbrLine)
	{
		APL_CscLine *lineOldest=m_lines[0];
		int find=0;
		for(unsigned int i=1;i<m_lines.size();i++)
		{
			if(APL_CscLine::compareValidity(*lineOldest,*m_lines[i])>0)
			{
				lineOldest=m_lines[i];
				find=i;
			}
		}
		delete lineOldest;
		m_lines.erase(m_lines.begin()+find);
	}

	for(unsigned int i=0;i<m_lines.size();i++)
	{
		//if the line is not valid anymore it is not written in the file
		if(m_lines[i]->checkValidity())
			m_lines[i]->writeLine(m_f);
	}

	return closeFile();
}

bool APL_CertStatusCache::closeFile()
{
	//If the file is not open, it's an implementation problem
	if(m_f)
	{
#ifdef WIN32
		_unlock_file(m_f);
#else	
		m_tFl.l_type   = F_UNLCK;  /* tell it to unlock the region */
		if( fcntl(fileno(m_f), F_SETLKW, &m_tFl) == -1){  /* set the lock, waiting if necessary */
		  printf("APL_CertStatusCache::closeFile: fcntl %s\n",strerror(errno));
		  exit(1);
		}
#endif
		fclose(m_f);
	}

	m_f=NULL;

	return true;
}

}
