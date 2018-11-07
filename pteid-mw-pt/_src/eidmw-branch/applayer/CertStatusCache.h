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

#ifndef __CERTSTATUSCACHE_H__
#define __CERTSTATUSCACHE_H__

#include <string>
#include <memory>
#include <vector>
#include "Mutex.h"
#include "APLCertif.h"
#include "cryptoFwkPteid.h"

#ifndef WIN32
#include <fcntl.h>
#endif

namespace eIDMW
{

#define CSC_VALIDITY_FORMAT "%Y%m%dT%H%M%S" //YYYYMMDDThhmmss

#define CSC_MAX_LINE_LENGHT 50

typedef enum 
{
	CSC_VALIDATION_NONE=0,	/**< No CRL nor OCSP validation */
	CSC_VALIDATION_CRL=1,	/**< CRL validation */
	CSC_VALIDATION_OCSP=2	/**< OCSP validation */
} CSC_Validation;

#define CSC_VALIDATION_FLAG_TESTROOT	8
#define CSC_VALIDATION_FLAG_BADDATE		16

typedef enum 
{
	CSC_STATUS_WAIT=0,		/**< Being validated by another process */
	CSC_STATUS_NONE,		/**< No validation yet */
	CSC_STATUS_VALID_SIGN,	/**< Valid certificate */
	CSC_STATUS_VALID_FULL,	/**< Valid certificate with CRL or OCSP */
	CSC_STATUS_TEST,		/**< Valid but test certificate */
	CSC_STATUS_DATE,		/**< Valid except Date validity */
	CSC_STATUS_CONNECT,		/**< Connection problem      */
	CSC_STATUS_ISSUER,		/**< An issuer is missing in the chain */
	CSC_STATUS_REVOKED,		/**< Certificate not valid   */
	CSC_STATUS_UNKNOWN,		/**< Responder return unknow */
	CSC_STATUS_ERROR,		/**< Another error occured   */
	CSC_STATUS_SUSPENDED    /** Suspended certificate    */
} CSC_Status;

/******************************************************************************//**
  * Line in the Certificate Status Cache
  *
  * This class is used by the APL_CertStatusCache
  *********************************************************************************/
class APL_CscLine
{
public:
	/**
	  * Constructor
	  *
	  * Take a line from the file cache and split it into field
	  */
	APL_CscLine(const char *line);

	/**
	  * Constructor
	  *
	  * Create a new line with status = CSC_STATUS_NONE
	  */
	APL_CscLine(unsigned long ulUniqueID,CSC_Validation validationType);

	/**
	  * Constructor
	  *
	  * Create a new line with status = CSC_STATUS_NONE
	  */
	APL_CscLine(unsigned long ulUniqueID,unsigned long ulFlags);

	/**
	  * Destructor
	  */
	virtual ~APL_CscLine(void);

	/**
	  * Two lines are equal if the UniqueID AND the Flags are booth equal
	  */
	bool operator== (const APL_CscLine& line);

	/**
	  * Compare internal UniqueID AND Flags to the params
	  *
	  * @return true if booth equal
	  */
	bool isEqual (unsigned long ulUniqueID,unsigned long ulFlags);

	/**
	  * Set Validity timestamp in format YYYYMMDDThhmmss by default
	  */
	void setValidity(unsigned long delay);

	/**
	  * Return true if the line in the cache is still valid (Validity > now)
	  */
	bool checkValidity();

	/**
	  * Write the line in the file (f)
	  */
	void writeLine(FILE *f);

	unsigned long getID() const					
		{return m_ulUniqueID;}								/**< Return the UniqueId of the line */

	unsigned long getFlags() const					
		{return m_ulFlags;}									/**< Return the Flags */

	static CSC_Validation getValidationType(unsigned long ulFlags)	
		{return (CSC_Validation)(ulFlags % 4);}				/**< Convert the Flags into validation type */

	CSC_Validation getValidationType() const	
		{return getValidationType(m_ulFlags);}				/**< Return the Validation type of the line */

	static bool allowTestRoot(unsigned long ulFlags)				
		{return (ulFlags & CSC_VALIDATION_FLAG_TESTROOT)!=0;}	/**< Convert the flags into bool for test root allow */

	bool allowTestRoot() const					
		{return (allowTestRoot(m_ulFlags))!=0;}					/**< Return true if test root are allow */

	CSC_Status getStatus() const				
		{return m_Status;}									/**< Return the Status of the line */

	void setStatus(CSC_Status status)			
		{m_Status=status;}									/**< Set the Status of the line line */

	/**
	  * Compare the validity time stamp of two lines
	  *
	  * @return	- <0 if validity of line1 <  validity of line2
	  *	@return	-  0 if validity of line1 == validity of line2
	  *	@return	- >0 if validity of line1 >  validity of line2
	  */
	static int compareValidity(const APL_CscLine &line1,const APL_CscLine &line2);
		
private:
	APL_CscLine(const APL_CscLine& csc);				/**< Copy not allowed - not implemented */
	APL_CscLine& operator= (const APL_CscLine& csc);	/**< Copy not allowed - not implemented */

	unsigned long m_ulUniqueID;			/**<  Unique ID of the certificate */
	unsigned long m_ulFlags;			/**<  Parameter flags (validation type, allow test root, allow wrong date) */
	CSC_Status m_Status;				/**<  Status of the line */
	std::string m_Validity;			/**<  Validity of the line in the cache */
};

/******************************************************************************//**
  * Class caching the status for the latest certificate in a file
  *
  * - We keep the status by Certificate UniqueId and Flags (validation type, allow test Root...)
  *   This way we can have different status depending the parameters 
  *   (for example one status if CRL validation is wanted and another for OCSP validation)
  * - Each line in the cache has a validity timestamp (for example valid for 15 minutes - see CSC_MAX_VALIDITY_SECOND)
  *   After this delay the line is not valid anymore and the status must be check again
  * - The cache is multi-process safe.
  *   Each access to the file is protected by Mutex.
  *   The file is written back as soon as possible.
  * - If process has to calculate the status, he writes the status WAIT in the file ,
  *	  so another process has to wait until the status is available (or the validity is passed)
  *   The wait validity delay is shorter than the normal delay 
  *   to avoid waiting to long in case of crash of the calculating process
  *
  * Usage CSCache.getCertStatus(ulUniqueID,validationType,pStore,bAllowTestRoot)
  *********************************************************************************/
class APL_CertStatusCache
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_CertStatusCache(void);

	/**
	  * To initialise : 
	  *		- the max number of line in the file (Default = CSC_MAX_LINE_NUMBER)
	  *		- the delay for line validity (Default = CSC_MAX_VALIDITY_SECOND)
	  *		- the delay for wait line validity (Default = CSC_MAX_WAITVALIDITY_SECOND)
	  *		- the file name (Default = comes from config)
	  */
	void Init(unsigned long ulMaxNbrLine,unsigned long ulNormalDelay=0,unsigned long ulWaitDelay=0,std::string cachefilename="");

	/**
	  * Return the Status of a certificate
	  *
	  * - First look in the cache if the status is there and still valid 
	  *	- If the status is being validate by an other process, wait for this validation
	  *	- Else call the checkCertValidation method and add the result to the cache
	  *
	  * @param ulUniqueID : The unique id of the certificate to validate
	  * @param validationType : type of validation wanted (NONE, CRL, OCSP)
	  * @param pStore : Pointer to the certificte store in which the unique ID must be and all the issuers
	  * @param bAllowTestRoot : if true, no error is return for Root test
	  * @param bAllowBadDate : if true, no error is return for bad date
	  *
	  * @return The status of the certificate
	  * @return		could not be NONE nor WAIT
	  */
	CSC_Status getCertStatus(unsigned long ulUniqueID,CSC_Validation validationType,APL_Certifs *pStore);

	/**
	  * Return the delay of line validity
	  */
	unsigned long getNormalDelay() {return m_ulNormalDelay;}

	/**
	  * Return the delay of wait line validity
	  */
	unsigned long getWaitDelay() {return m_ulWaitDelay;}

private:
	/**
	  * Constructor
	  */
	APL_CertStatusCache(APL_CryptoFwk *cryptoFwk);

	APL_CertStatusCache(const APL_CertStatusCache& csc);				/**< Copy not allowed - not implemented */
	APL_CertStatusCache& operator= (const APL_CertStatusCache& csc);	/**< Copy not allowed - not implemented */

	/**
	  * Delete the object in the vector (m_lines) and empty it.
	  */
	void resetLines();

	/**
	  * Load the cache file,
	  * create the APL_CscLine objects and add it to the vector (m_lines)
	  */
	bool loadFile();

	/**
	  * Write the APL_CscLine objects in the vector (m_lines) into the cache file
	  */
	bool writeFile();

	/**
	  * Close the cache file
	  */
	bool closeFile();

	/**
	  * Look in the cache for the status of the certificate
	  *
	  * - Load the cache file 
	  * - Find the line
	  * - If found check the validity of the line.
	  * - If unvalid, delete the line
	  * - Create the line with status=CSC_STATUS_WAIT to avoid other process to do the validation
	  * - Write all the lines into cache file
	  *
	  * @param ulUniqueID : The unique id of the certificate to validate
	  * @param ulFlags : type of validation wanted (NONE, CRL, OCSP), allow test root, allow wrong date
	  *
	  * @return The status of the certificate
	  * @return		- NONE means that the status has to be calculated
	  * @return		- WAIT means that we have to wait until un other process made the validation
	  * @return		- other status means the status was valid in the cache
	  */
	CSC_Status getStatusFromCache(unsigned long ulUniqueID,unsigned long ulFlags);

	/**
	  * Make the validation of the certificate and return the status
	  *
	  * @param ulUniqueID : The unique id of the certificate to validate
	  * @param ulFlags : type of validation wanted (NONE, CRL, OCSP), allow test root, allow wrong date
	  * @param pStore : Pointer to the certificte store in which the unique ID must be and all the issuers
	  *
	  * @return The status of the certificate
	  * @return		could not be NONE nor WAIT
	  */
	CSC_Status checkCertValidation(unsigned long ulUniqueID,unsigned long ulFlags,APL_Certifs *pStore);

	/**
	  * Add the status to the cache
	  *
	  * - Load the cache file
	  * - Find the line... if unfound, create it
	  * - Set the status and the line validity
	  * - Write all the lines into cache file
	  *
	  * @param ulUniqueID : The unique id of the certificate to validate
	  * @param validationType : type of validation wanted (NONE, CRL, OCSP)
	  * @param status : The status to store in the cache
	  */
	void addStatusToCache(unsigned long ulUniqueID,unsigned long ulFlags,CSC_Status status);

	/**
	  * Convert CryptoCertStatus into CSC_Status
	  */
	CSC_Status convertStatus(APL_CertifStatus status);

	CMutex m_Mutex;									/**< Mutex for exclusive access */

	APL_CryptoFwk *m_cryptoFwk;						/**< Pointer to the crypto framework */

	std::string m_cachefilename;					/**< The name of the cache file */
	unsigned long m_ulMaxNbrLine;					/**< The maximum number of line that the file can contain */
	unsigned long m_ulNormalDelay;					/**< The delay of lines validity in the cache  */
	unsigned long m_ulWaitDelay;					/**< The delay of wait lines validity in the cache = the delay for validating process */

	std::vector <APL_CscLine*> m_lines;				/**< List of the line in the cache */

	FILE *m_f;										/**< Pointer to the cache file */

friend void CAppLayer::startAllServices();			/**< This method must access private constructor */
 
#ifndef WIN32
	struct flock m_tFl;
#endif
};

}

#endif //__CERTSTATUSCACHE_H__
