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

#ifndef __SCREADERUTIL_H__
#define __SCREADERUTIL_H__

#include <string>
#include <vector>
#include "Export.h"
#include "Mutex.h"
#include "ByteArray.h"
#include "CardLayerConst.h"

extern std::string m_InitSerialNumber;								/**< Field ChipNumber */

namespace eIDMW
{

#define COUNT_UNDEF 0xFFFFFFFF

enum APL_CardType
{
	APL_CARDTYPE_UNKNOWN=0,
	APL_CARDTYPE_PTEID_IAS07,
	APL_CARDTYPE_PTEID_IAS101
};


struct APL_RawData_Eid
{
	unsigned long version;
    CByteArray idData;
    CByteArray idSigData;
    CByteArray traceData;
    CByteArray addrData;
    CByteArray addrSigData;
    CByteArray sodData;
    CByteArray cardData;
    CByteArray tokenInfo;
    CByteArray certRN;
    CByteArray challenge;
    CByteArray response;
    CByteArray persodata;
};

/**
  * Easy access to singleton (no declaration/instantiation is needed)
  * 
  * Usage : AppLayer.SomeMethod()
  */
#define AppLayer CAppLayer::instance()

class CCardLayer;
class CReader;

class APL_ReaderContext;
class APL_CryptoFwkPteid;
class APL_CertStatusCache;

/******************************************************************************//**
  * Singleton class that represent the application layer
  *		-Singleton object is create at the first instance call
  *		-Encapsulate a CCardLayer object for access to CardLayer 
  *********************************************************************************/
class CAppLayer
{
public:
	/**
	  * Return the singleton object (create it at first use)
	  */
    EIDMW_APL_API static CAppLayer &instance();

	/**
	  * Init the applayer
	  */
	EIDMW_APL_API static void init(bool bAskForTestCard=false);

	/**
	  * Release the whole applayer
	  */
    EIDMW_APL_API static void release();
	
	/**
	  * Release the readers (Needed if isReadersChanged)
	  */
	EIDMW_APL_API void releaseReaders();

	/**
	  * Return true if readers has been added or removed
	  */
	EIDMW_APL_API bool isReadersChanged() const;

	/**
	  * Return the internal context id
	  */    
	EIDMW_APL_API unsigned long getContextId(bool bForceRefresh=false);

	/**
	  * Return reader list from the cardlayer
	  */
	EIDMW_APL_API const char * const *readerList(bool bForceRefresh=false);

	/**
	  * Return the number of reader
	  */
	EIDMW_APL_API unsigned long readerCount(bool bForceRefresh=false);

	/**
	  * Return a reader object by its name
	  *		First look in the reader vector if an object already exist
	  *		If no, look in the reader list if the name exist
	  *			Yes => create the object and add to the vector
	  *			No => throw an exception EIDMW_ERR_NO_READER
	  */
	EIDMW_APL_API APL_ReaderContext &getReader(const char *readerName);

	/**
	  * Return a reader object by its index
	  *		First get the reader name
	  *		The call the getReader by name
	  *	Throw an exception EIDMW_ERR_PARAM_RANGE if index out of range
	  */
	EIDMW_APL_API APL_ReaderContext &getReader(unsigned long ulIndex);

	/**
	  * Return the first readercontext with a card
	  *		If no card is present, return the firs reader.
	  *		If no reader exist, throw an exception EIDMW_ERR_NO_READER
	  */
	EIDMW_APL_API APL_ReaderContext &getReader();

	/**
	  * Return the reader containing the card with this SN
	  *		If no card with this SN is found, throw an exception EIDMW_ERR_PARAM_RANGE
	  */
	EIDMW_APL_API APL_ReaderContext &getReaderByCardSN(const char *cardSerialNumber);

	/**
	  * Return the reader name by its index
	  *	Throw an exception EIDMW_ERR_PARAM_RANGE if index out of range
	  */
	EIDMW_APL_API const char *getReaderName(unsigned long ulIndex);

 	/** 
	  * Flush the CAL cache 
	  */
	EIDMW_APL_API bool flushCache() const;

	/**
	  * Return the cardlayer object
	  */
	CCardLayer *getCardLayer() const;


	/**
	  * Return the crypto framework object
	  *
	  * NOT for using outside the library (NO EXPORT)
	  */
	APL_CryptoFwkPteid *getCryptoFwk() const;

	/**
	  * Return the Certificate status cache object
	  *
	  * NOT for using outside the library (NO EXPORT)
	  */
	APL_CertStatusCache *getCertStatusCache() const; 

	/**
	  * Start card layer, crypto Fwk, caches, CRL service,...
	  *
	  * NOT for using outside the library (NO EXPORT)
	  * Must be public to be friend of class that are instanciated
	  */
	 void startAllServices();

	 bool getAskForTestCard();

private:
	/**
	  * Constructor - used within "instance"
	  */
	CAppLayer();

	/**
	  * Destructor - used within release
	  */
	virtual ~CAppLayer(void);

	CAppLayer(const CAppLayer& app);				/**< Copy not allowed - not implemented */
	CAppLayer& operator= (const CAppLayer& app);	/**< Copy not allowed - not implemented */

	/**
	  * Place the flag for asking if test card could be read
	  * If true the applayer must ask. 
	  * If false other application (ex. gui) take that into their scope
	  * Default value is false.
	  * Use in init
	  */
	void setAskForTestCard(bool bAskForTestCard);

	/**
	  * Update the version for this user
	  */
	 void updateVersion();

	/**
	  * Stop all the service
	  *
	  * Delete card layer, crypto Fwk, caches, CRL service,...
	  */
	 void stopAllServices();

	/**
	  * Release the m_readerList
	  */
	void readerListRelease();

	/**
	  * Initialize the m_readerList
	  *
	  * @param bForceRefresh : if true, always clear the vector
	  * @param bForceRefresh : if false, clear the vector only if reader count has changed
	  */
	void readerListInit(bool bForceRefresh=false);

    static CAppLayer *m_instance;					/**< Pointer to singleton object */
	static CMutex m_Mutex;							/**< Mutex to create the singleton */

	std::vector<APL_ReaderContext *> m_physicalReaders;		/**< Vector of physical readers already called */

	bool m_askfortestcard;							/**< If true the applayer must ask if test card could be read */

	char **m_readerList;							/**< Array of the reader name */
	unsigned long m_readerCount;					/**< Keep the number of reader */

	unsigned long m_contextid;						/**< Incremented for each time a reader is add or remove */

	CCardLayer *m_Cal;								/**< Pointer to cardlayer */
	APL_CryptoFwkPteid *m_cryptoFwk;					/**< Pointer to APL_CryptoFwkPteid */
	APL_CertStatusCache *m_certStatusCache;			/**< Pointer to APL_CertStatusCache */
};

class CReader;

class APL_Card;
class APL_EIDCard;

/******************************************************************************//**
  * Class that represent a reader
  * Contain a CReader from the CardLayer
  *
  * To get APL_ReaderContext object, we have to ask it from CAppLayer 
 *********************************************************************************/
class APL_ReaderContext
{
public:

	/**
	  * Construct using Raw data for Eid
	  *		No reader are connected (m_reader=NULL)
	  */
	EIDMW_APL_API APL_ReaderContext(const APL_RawData_Eid &data);

	/**
	  * Destructor
	  */    
	EIDMW_APL_API ~APL_ReaderContext();

	/**
	  * Return the name of the reader
	  */    
	EIDMW_APL_API const char *getName();

	/**
	  * Instanciated the m_card object
	  *		First delete it if not null
	  *		Then check if a card is present
	  *		If yes, instanciated regarding the card type
	  *			APL_EIDCard
	  *
	  * @return true if a new connection has been made
	  */
	EIDMW_APL_API bool connectCard();

	/**
	  * Check if a card is present
	  * return true if the external status is 
	  *		CARD_STILL_PRESENT, CARD_INSERTED, CARD_OTHER
	  */    
	EIDMW_APL_API bool isCardPresent();

	/**
	  * Check if the card has been changed until the last id we get
	  */    
	EIDMW_APL_API bool isCardChanged(unsigned long &ulOldId);

	/**
	* Check if this is a Pinpad reader
	*/
	EIDMW_APL_API bool isPinpad();

	/**
	  * Return the internal card id of the current connected card (0 if no card is present)
	  */    
	EIDMW_APL_API unsigned long getCardId();
 
 	/**
	  * Return the type of the card
	  *		Connect to the card if the status has changed 
	  *		If no card is present, return PTEID_CARDTYPE_UNKNOWN
	  */    
	EIDMW_APL_API APL_CardType getCardType();

 	/**
	  * Get the card in the reader whatever is type
	  * If there is no card in the reader NULL is return
	  */    
    EIDMW_APL_API APL_Card *getCard();

 	/**
	  * Get the EIdcard in the reader
	  * If there is no EIdcard in the reader NULL is return
	  */    
    EIDMW_APL_API APL_EIDCard *getEIDCard();

	/** 
	 * Specify a callback function to be called each time a
	 * card is inserted/remove in/from this reader.
	 *
	 * @return A handle can be used to stop the callbacks when they are no longer needed. 
	 */
    EIDMW_APL_API unsigned long SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef) const;

	/** 
	  * To tell that the callbacks are not longer needed. 
	  * @param ulHandle is the handle return by SetEventCallback
	  */
	EIDMW_APL_API void StopEventCallback(unsigned long ulHandle) const;

	EIDMW_APL_API void BeginTransaction();				/**< Begin a transaction */
	EIDMW_APL_API void EndTransaction();				/**< End the transaction */

	void CalLock();										/**< Lock the reader for exclusive atomic access */
	void CalUnlock();									/**< Unlock the reader */


	/**
	  * Give access to the CAL
	  * NOT FOR EXPORT
	  */
	CReader *getCalReader() const;

	/**
	  * Give access to the Super Parser
	  * NOT FOR EXPORT
	  */

private:	
	/**
	  * Construct using a reader name
	  *		Instanciated the m_reader with the cardlayer reader
	  */    
	APL_ReaderContext(const char *readerName);

	APL_ReaderContext(const APL_ReaderContext &reader);				/**< Copy not allowed - not implemented */
	APL_ReaderContext &operator=(const APL_ReaderContext &reader);	/**< Copy not allowed - not implemented */

	APL_CardType getPhysicalCardType(); /**< Ask the card type to the CAL */
	
	bool m_cal_lock;			/**< Running an atomic cal lock */
	CMutex m_cal_mutex;			/**< Mutex for cardlayer access */

	bool m_transaction_lock;		/**< Running Transaction */
	CMutex m_transaction_mutex;		/**< Mutex for the transaction  */

	CMutex m_newcardmutex;

	APL_Card *m_card;		/**< Pointer to the card in the reader */
	CReader *m_calreader;		/**< Pointer to the reader object in the cardlayer */
	tCardStatus m_status;	/**< Hold the status of the reader */

	std::string m_name;		/**< The name of the reader */
	unsigned long m_cardid;	/**< Incremented for each new connected card */

friend APL_ReaderContext &CAppLayer::getReader(const char *readerName); /**< This method must access protected constructor */
};


class TLVParser;
class CSVParser;
class EIDMW_EIDMemParser;

}

#endif //__SCREADERUTIL_H__
