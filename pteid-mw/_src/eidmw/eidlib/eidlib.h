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

#ifndef __BEIDLIB_H__
#define __BEIDLIB_H__

#include <string>
#include <vector>
#include <map>
#include "eidlibdefines.h"

namespace eIDMW
{

struct SDK_Context;

class CMutex;
class BEID_Exception;

/******************************************************************************//**
  * Base class for the object of BEID SDK (Can not be instantiated).
  *********************************************************************************/
class BEID_Object
{
public:
	BEIDSDK_API virtual ~BEID_Object()=0;		/**< Destructor */


	NOEXPORT_BEIDSDK void Init(const SDK_Context *context,void *impl);	/**< For internal use : Initialize pimpl */

	/**
	  * Release the objects instantiated within this BEID_Object.
	  * Automatically call in the destructor.
 	  */
	NOEXPORT_BEIDSDK void Release();

protected:
	BEID_Object(const SDK_Context *context,void *impl);	/**< For internal use : Constructor */
	void addObject(BEID_Object *impl);			/**< For internal use : Add linked object */
	void backupObject(unsigned long idx);		/**< For internal use : Backup linked object */
	BEID_Object *getObject(unsigned long idx);	/**< For internal use : Return an object by its index */
	BEID_Object *getObject(void *impl);			/**< For internal use : Return an object by its impl */
	void delObject(unsigned long idx);			/**< For internal use : Delete an object by its index */
	void delObject(void *impl);					/**< For internal use : Delete an object by its impl */

	void checkContextStillOk() const;			/**< For internal use : check if the Context is still correct (the card hasn't changed) */

	BEID_Object(const BEID_Object& obj);				/**< Copy not allowed - not implemented */
	BEID_Object& operator= (const BEID_Object& obj);	/**< Copy not allowed - not implemented */

	bool m_delimpl;							/**< For internal use : m_impl object must be deleted */
	void *m_impl;							/**< For internal use : pimpl pointer */
	unsigned long m_ulIndexExtAdd;			/**< For internal use : extended add object */
	std::map<unsigned long,BEID_Object *> m_objects;	/**< For internal use : Map of object instantiated within this BEID_Object */

	SDK_Context *m_context;					/**< For internal use : context structure */

	//CMutex *m_mutex;
};

class BEID_Card;

class CByteArray;

/******************************************************************************//**
  * This class is used to receive/pass bytes array from/to different method in the SDK.
  *********************************************************************************/
class BEID_ByteArray : public BEID_Object
{
public:
    BEIDSDK_API BEID_ByteArray();								/**< Default constructor */
	BEIDSDK_API BEID_ByteArray(const BEID_ByteArray &bytearray); /**< Copy constructor */

	 /**
	  * Constructor - initialize a byte array with an array of unsigned char.
	  *
	  * @param pucData is the byte array
	  * @param ulSize is the size of the array
	  */
    BEIDSDK_API BEID_ByteArray(const unsigned char * pucData, unsigned long ulSize);

	BEIDSDK_API virtual ~BEID_ByteArray();				/**< Destructor */

	 /**
	  * Append data to the byte array.
	  *
	  * @param pucData is the byte array
	  * @param ulSize is the size of the array
	  */
    BEIDSDK_API void Append(const unsigned char * pucData, unsigned long ulSize);

	 /**
	  * Append data to the byte array.
	  */
    BEIDSDK_API void Append(const BEID_ByteArray &data);

	 /**
	  * Remove the data from the byte array.
	  */
    BEIDSDK_API void Clear();

	 /**
	  * Return true if the content of data is the same as this.
	  */
    BEIDSDK_API bool Equals(const BEID_ByteArray &data) const;

 	 /**
	  * Return the number of bytes in the array.
	  */
    BEIDSDK_API unsigned long Size() const;

 	/**
	  * Return the array of bytes in the object.
	  * If Size() == 0, then NULL is returned.
	  */
	BEIDSDK_API const unsigned char *GetBytes() const;

	/**
	  * Writing the binary content to a file.
	  */
	BEIDSDK_API bool writeToFile(const char * csFilePath);

	/**
	  * Copy content of bytearray.
	  */
	BEIDSDK_API BEID_ByteArray &operator=(const BEID_ByteArray &bytearray);

	NOEXPORT_BEIDSDK BEID_ByteArray(const SDK_Context *context,const CByteArray &impl);	/**< For internal use : construct from lower level object*/
	NOEXPORT_BEIDSDK BEID_ByteArray &operator=(const CByteArray &bytearray);			/**< For internal use : copy from lower level object*/
};

/******************************************************************************//**
  * These structure are used for compatibility with old C sdk.
  *********************************************************************************/
struct BEID_RawData_Eid
{
    BEID_ByteArray idData;
    BEID_ByteArray idSigData;
    BEID_ByteArray addrData;
    BEID_ByteArray addrSigData;
    BEID_ByteArray sodData;
    BEID_ByteArray cardData;
    BEID_ByteArray tokenInfo;
    BEID_ByteArray certRN;
    BEID_ByteArray challenge;
    BEID_ByteArray response;
    BEID_ByteArray persoData;
    BEID_ByteArray trace;
};

struct BEID_RawData_Sis
{
    BEID_ByteArray idData;
};

/**
  * This define give an easy access to singleton (no declaration/instantiation is needed).
  *
  * Usage : ReaderSet.SomeMethod().
  */
#define ReaderSet BEID_ReaderSet::instance()

/**
  * Init the SDK (Optional).
  */
#define BEID_InitSDK() BEID_ReaderSet::initSDK()

/**
  * Release the SDK.
  * THIS MUST BE CALLED WHEN THE SDK IS NOT NEEDED ANYMORE AND BEFORE THE APPLICATION IS CALLED.
  * IF NOT RELEASE PROPERLY, AN EXCEPTION BEID_ExReleaseNeeded IS THROWN.
  */
#define BEID_ReleaseSDK() BEID_ReaderSet::releaseSDK()

class BEID_ReaderContext;
class APL_ReaderContext;

/******************************************************************************//**
  * This is a singleton class that is the starting point to get all other objects.
  * You get an instance from the static instance() method (or using the define ReaderSet).
  * Then you get a READER							(BEID_ReaderContext)
  *		-> from this reader, you a CARD				(BEID_Card or derived class)
  *			-> from this card, you get DOCUMENT		(BEID_XMLDoc or derived class)
  *				-> ...
  *********************************************************************************/
class BEID_ReaderSet : public BEID_Object
{
public:
    BEIDSDK_API static BEID_ReaderSet &instance();		/**< Return the singleton object (create it at first use) */

	/**
	  * Init the SDK (Optional).
	  * @param bManageTestCard If true the applayer must ask if test cards are allowed (used for compatibility with old C API).
	  * @param bManageTestCard If false other applications (ex. gui) take that into their scope
	  */
	BEIDSDK_API static void initSDK(bool bManageTestCard=false);
	BEIDSDK_API static void releaseSDK();				/**< Release the SDK */

	BEIDSDK_API virtual ~BEID_ReaderSet();				/**< Destructor */

	/**
	  * Release the readers (Useful if readers had changed).
	  *
	  * @param bAllReference If true all the invalid reference/pointer are destroyed.
	  * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	  * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	  */
	BEIDSDK_API void releaseReaders(bool bAllReference=false);

	/**
	  * Return true if readers has been added or removed
	  */
	BEIDSDK_API bool isReadersChanged() const;

	/**
	  * Get the list of the reader.
	  * Return an array of const char *
	  * The last pointer is NULL
	  * Usage : const char * const *ppList=BEID_ReaderSet::readerList();
	  *         for(const char * const *ppName=ppList;*ppName!=NULL;ppName++) {...}
	  *
	  * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	  */
    BEIDSDK_API const char * const *readerList(bool bForceRefresh=false);

	/**
	  * Return the first readercontext with a card.
	  *		If no card is present, return the firs reader.
	  *		If no reader exist, throw an exception BEID_ExNoReader.
	  */
	BEIDSDK_API BEID_ReaderContext &getReader();

	/**
	  * Get the reader by its name.
 	  */
	BEIDSDK_API BEID_ReaderContext &getReaderByName(const char *readerName);

 	/**
	  * Return the number of card readers connected to the computer.
	  *
	  * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	  */
    BEIDSDK_API unsigned long readerCount(bool bForceRefresh=false);

	/**
	  * Get the name of the reader by its Index.
	  * Throw BEID_ExParamRange exception if the index is out of range.
 	  */
	BEIDSDK_API const char *getReaderName(unsigned long ulIndex);

	/**
	  * Get the reader by its Index.
	  * Throw BEID_ExParamRange exception if the index is out of range.
 	  */
	BEIDSDK_API BEID_ReaderContext &getReaderByNum(unsigned long ulIndex);

	/**
	  * Return the reader containing the card with this SN.
	  *		If no card with this SN is found, throw an exception BEID_ExParamRange.
	  */
	BEIDSDK_API BEID_ReaderContext &getReaderByCardSerialNumber(const char *cardSerialNumber);

	/**
	* Flush the cached files.
	* Return if any files were flushed (T/F).
	*/;
	BEIDSDK_API bool flushCache();			/**< Flush the cache */

	NOEXPORT_BEIDSDK BEID_ReaderContext &getReader(APL_ReaderContext *pAplReader); /**< For internal use - Not exported*/

private:
	BEID_ReaderSet();											/**< For internal use : Constructor */

	BEID_ReaderSet(const BEID_ReaderSet& reader);				/**< Copy not allowed - not implemented */
	BEID_ReaderSet& operator= (const BEID_ReaderSet& reader);	/**< Copy not allowed - not implemented */

};

class BEID_Card;
class BEID_EIDCard;
class BEID_KidsCard;
class BEID_ForeignerCard;
class BEID_SISCard;

/******************************************************************************//**
  * This class represent a reader.
  * You get reader object from the ReaderSet
  *		either by its index (getReaderByNum) or by its name (getReaderByName).
  * Once you have a reader object, you can check if a card is present (isCardPresent).
  * Then you can ask which type of card is in the reader with getCardType()
  *		and then get a card object using one of this method :
  *			getCard, getEIDCard, getKidsCard, getForeignerCard or getSISCard.
  *********************************************************************************/
class BEID_ReaderContext : public BEID_Object
{
public:
	/**
	  * Construct using a fileType and fileName.
	  *		No physical reader are connected (m_reader=NULL)
	  */
	BEIDSDK_API BEID_ReaderContext(BEID_FileType fileType,const char *fileName);

	/**
	  * Construct using a fileType and its content (for compatibility with SetRawFile).
	  *		No physical reader are connected (m_reader=NULL)
	  */
	BEIDSDK_API BEID_ReaderContext(BEID_FileType fileType,const BEID_ByteArray &data);

	/**
	  * Construct using Raw data for Eid.
	  *		No physical reader are connected (m_reader=NULL)
	  */
	BEIDSDK_API BEID_ReaderContext(const BEID_RawData_Eid &data);

	/**
	  * Construct using Raw data for Sis.
	  *		No physical reader are connected (m_reader=NULL)
	  */
	BEIDSDK_API BEID_ReaderContext(const BEID_RawData_Sis &data);

	BEIDSDK_API virtual ~BEID_ReaderContext();	/**< Destructor */

	/**
	  * Return the name of the reader.
	  */
	BEIDSDK_API const char *getName();

	/**
	  * Return true if a card is present and false otherwise.
	  */
    BEIDSDK_API bool isCardPresent();

	/**
	  * Release the card.
	  *
	  * @param bAllReference If true all the invalid reference/pointer are destroyed.
	  * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	  * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	  */
	BEIDSDK_API void releaseCard(bool bAllReference=false);

	/**
	  * Return true if a card has changed since the last called (with the same ulOldId parameter).
	  */
	BEIDSDK_API bool isCardChanged(unsigned long &ulOldId);

	/**
	 * Return the type of the card in the reader.
	 *
	 * Throw BEID_ExNoCardPresent exception if no card is present.
	 */
	BEIDSDK_API BEID_CardType getCardType();

	/**
	 * Get the card in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(BEID_EIDCard, BEID_KidsCard, BEID_ForeignerCard or BEID_SISCard).
	 *
	 * If no card is present in the reader, exception BEID_ExNoCardPresent is thrown.
	 * If the card type is not supported, exception BEID_ExCardTypeUnknown is thrown.
	 */
    BEIDSDK_API BEID_Card &getCard();

	/**
	 * Get the EIDcard in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(BEID_EIDCard, BEID_KidsCard, BEID_ForeignerCard).
	 *
	 * If no card is present in the reader, exception BEID_ExNoCardPresent is thrown.
	 * If the card is not an EIDcard, exception BEID_ExCardBadType is thrown.
	 */
    BEIDSDK_API BEID_EIDCard &getEIDCard();

 	/**
	 * Get the KidsCard in the reader.
	 *
	 * If no card is present in the reader, exception BEID_ExNoCardPresent is thrown.
	 * If the card is not a KidsCard, exception BEID_ExCardBadType is thrown.
	 */
	BEIDSDK_API BEID_KidsCard &getKidsCard();

 	/**
	 * Get the ForeignerCard in the reader.
	 *
	 * If no card is present in the reader, exception BEID_ExNoCardPresent is thrown
	 * If the card is not a ForeignerCard, exception BEID_ExCardBadType is thrown.
	 */
	BEIDSDK_API BEID_ForeignerCard &getForeignerCard();

 	/**
	 * Get the SISCard in the reader.
	 *
	 * If no card is present in the reader, exception BEID_ExNoCardPresent is thrown.
	 * If the card is not a SISCard, exception BEID_ExCardBadType is thrown.
	 */
	BEIDSDK_API BEID_SISCard &getSISCard();

	/**
	 * Specify a callback function to be called each time a
	 * card is inserted/remove in/from this reader.
	 *
	 * @return A handle can be used to stop the callbacks when they are no longer needed.
	 */
    BEIDSDK_API unsigned long SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef);

	/**
	  * To tell that the callbacks are not longer needed.
	  * @param ulHandle is the handle return by SetEventCallback
	  */
	BEIDSDK_API void StopEventCallback(unsigned long ulHandle);

	BEIDSDK_API void BeginTransaction();								/**< Begin a transaction with the reader */
	BEIDSDK_API void EndTransaction();								/**< End the transaction */

	BEIDSDK_API bool isVirtualReader();								/**< Return true if this is a virtual reader (create from a file) */

private:
	BEID_ReaderContext(const BEID_ReaderContext& reader);				/**< Copy not allowed - not implemented */
	BEID_ReaderContext& operator= (const BEID_ReaderContext& reader);	/**< Copy not allowed - not implemented */

	BEID_ReaderContext(const SDK_Context *context,APL_ReaderContext *impl);						/**< For internal use : Constructor */

	unsigned long m_cardid;
	//CMutex *m_mutex;

friend BEID_ReaderContext &BEID_ReaderSet::getReader(APL_ReaderContext *pAplReader); /**< For internal use : This method must access protected constructor */
};

class BEID_XMLDoc;
class APL_Card;

/******************************************************************************//**
  * Abstract base class for all the card type supported.
  * The BEID_ReaderContext::getCard() method will return such an object.
  *********************************************************************************/
class BEID_Card : public BEID_Object
{
public:
	BEIDSDK_API virtual ~BEID_Card()=0;				/**< Destructor */

 	/**
	 * Return the type of the card
	 */
	BEIDSDK_API virtual BEID_CardType getType();

 	/**
	 * Return a document from the card.
	 * Throw BEID_ExDocTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual BEID_XMLDoc& getDocument(BEID_DocumentType type)=0;

 	/**
	 * Return a raw data from the card.
	 * Throw BEID_ExFileTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual const BEID_ByteArray& getRawData(BEID_RawDataType type)=0;

 	/**
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @return a BEID_ByteArray containing the result
	 */
    BEIDSDK_API virtual BEID_ByteArray sendAPDU(const BEID_ByteArray& cmd);

    BEIDSDK_API virtual BEID_ByteArray Sign(const BEID_ByteArray& oData);

 	/**
	 * Read a File from the card.
	 * @param fileID is the path of the file
	 * @param ulOffset is the offset to begins the reading
	 * @param ulMaxLength is the maximum length of bytes to read
	 * @return A BEID_ByteArray with the content of the file
	 */
	BEIDSDK_API virtual BEID_ByteArray readFile(const char *fileID, unsigned long  ulOffset=0, unsigned long  ulMaxLength=0);

 	/**
	 * Write a file to the card.
	 * @param fileID is the path of the file
	 * @param oData contents the bytes to write
	 * @param ulOffset is the offset to begins the writing
	 */
	BEIDSDK_API virtual bool writeFile(const char *fileID, const BEID_ByteArray& oData,unsigned long ulOffset=0);

protected:
	BEID_Card(const SDK_Context *context,APL_Card *impl);/**< For internal use : Constructor */

private:
	BEID_Card(const BEID_Card& card);					/**< Copy not allowed - not implemented */
	BEID_Card& operator= (const BEID_Card& card);		/**< Copy not allowed - not implemented */

};

class APL_MemoryCard;

/******************************************************************************//**
  * Abstract base class for Memory card.
  *********************************************************************************/
class BEID_MemoryCard : public BEID_Card
{
public:
	BEIDSDK_API virtual ~BEID_MemoryCard()=0;				/**< Destructor */

protected:
	BEID_MemoryCard(const SDK_Context *context,APL_Card *impl);/**< For internal use : Constructor */

private:
	BEID_MemoryCard(const BEID_MemoryCard& card);				/**< Copy not allowed - not implemented */
	BEID_MemoryCard& operator= (const BEID_MemoryCard& card);	/**< Copy not allowed - not implemented */
};

class BEID_Pin;
class BEID_Pins;
class BEID_Certificates;
class APL_SmartCard;

/******************************************************************************//**
  * Abstract base class for Smart card.
  *********************************************************************************/
class BEID_SmartCard : public BEID_Card
{
public:
	BEIDSDK_API virtual ~BEID_SmartCard()=0;				/**< Destructor */

 	/**
	 * Select an application from the card
	 */
	BEIDSDK_API void selectApplication(const BEID_ByteArray &applicationId);

 	/**
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @param pin is the pin to ask for writing
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 * @return a BEID_ByteArray containing the result
	 */
    BEIDSDK_API virtual BEID_ByteArray sendAPDU(const BEID_ByteArray& cmd,BEID_Pin *pin=NULL,const char *csPinCode="");

 	/**
	 * Read a File from the card.
	 *
	 * If no pin is passed and a pin is needed by the card, the pin code will be asked anyway.
	 *
	 * @param fileID is the path of the file
	 * @param in return the file
	 * @param pin is the pin to ask for reading
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 */
	BEIDSDK_API virtual long readFile(const char *fileID, BEID_ByteArray &in,BEID_Pin *pin=NULL,const char *csPinCode="");

 	/**
	 * Write a file to the card.
	 * Throw BEID_ExCmdNotAllowed exception you're not allowed to write the file.
	 *
	 * If no pin is passed and a pin is needed by the card, the pin code will be asked anyway.
	 *
	 * @param fileID is the path of the file
	 * @param out contents the bytes to write
	 * @param pin is the pin to ask for writing
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 */
	BEIDSDK_API virtual bool writeFile(const char *fileID,const BEID_ByteArray &out,BEID_Pin *pin=NULL,const char *csPinCode="");

 	/**
	 * Return the number of pin on the card.
	 */
	BEIDSDK_API virtual unsigned long pinCount();

 	/**
	 * Return an object to access all the pins on the card.
	 */
	BEIDSDK_API virtual BEID_Pins& getPins();

 	/**
	 * Return the number of certificate on the card.
	 */
	BEIDSDK_API virtual unsigned long certificateCount();

	/**
	 * Return an object to access all the certificates on the card.
	 */
	BEIDSDK_API virtual BEID_Certificates& getCertificates();

	/**
	  * Return the challenge.
	  *
	  * @param bForceNewInit force a new initialization of the couple challenge/response
	  */
	BEIDSDK_API virtual const BEID_ByteArray &getChallenge(bool bForceNewInit = false);

	/**
	  * Return the response to the challenge.
	  */
	BEIDSDK_API virtual const BEID_ByteArray &getChallengeResponse();

	/**
	  * Return true if the response of the card to the given challenge is the same as the response expected.
	  * For virtual card (from file), always return false.
	  *
	  * @param challenge is the challenge to check
	  * @param response is the response expected from the card
	  */
	BEIDSDK_API virtual bool verifyChallengeResponse(const BEID_ByteArray &challenge, const BEID_ByteArray &response) const;

protected:
	BEID_SmartCard(const SDK_Context *context,APL_Card *impl);	/**< For internal use : Constructor */

private:
	BEID_SmartCard(const BEID_SmartCard& card);						/**< Copy not allowed - not implemented */
	BEID_SmartCard& operator= (const BEID_SmartCard& card);			/**< Copy not allowed - not implemented */
};

class BEID_SisFullDoc;
class BEID_SisId;
class APL_SISCard;

/******************************************************************************//**
  * This class represents a SIS card.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class BEID_SISCard : public BEID_MemoryCard
{
public:
	BEIDSDK_API virtual ~BEID_SISCard();				/**< Destructor */

 	/**
	 * Return a document from the card.
	 * Throw BEID_ExDocTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual BEID_XMLDoc& getDocument(BEID_DocumentType type);

 	/**
	 * Get the full document.
	 */
	BEIDSDK_API BEID_SisFullDoc& getFullDoc();

 	/**
	 * Get the id document.
	 */
 	BEIDSDK_API BEID_SisId& getID();

 	/**
	 * Return a raw data from the card.
	 * Throw BEID_ExFileTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual const BEID_ByteArray& getRawData(BEID_RawDataType type);

 	/**
	 * Get the id RawData.
	 */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Id();

private:
	BEID_SISCard(const BEID_SISCard& card);				/**< Copy not allowed - not implemented */
	BEID_SISCard& operator= (const BEID_SISCard& card);	/**< Copy not allowed - not implemented */

	BEID_SISCard(const SDK_Context *context,APL_Card *impl);	/**< For internal use : Constructor */

friend BEID_Card &BEID_ReaderContext::getCard();		/**< For internal use : This method must access protected constructor */

};

class BEID_EIdFullDoc;
class BEID_EId;
class BEID_Address;
class BEID_Sod;
class BEID_CardVersionInfo;
class BEID_Certificate;
class APL_EIDCard;

/******************************************************************************//**
  * This class represents a Belgium EID card.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class BEID_EIDCard : public BEID_SmartCard
{
public:
	BEIDSDK_API virtual ~BEID_EIDCard();						/**< Destructor */

 	/**
	  * Return true if the user allow the application.
	  */
	BEIDSDK_API static bool isApplicationAllowed();

 	/**
	  * Return true this is a test card.
	  */
	BEIDSDK_API virtual bool isTestCard();

	/**
	  * Return true if test card are allowed.
	  */
	BEIDSDK_API virtual bool getAllowTestCard();

	/**
	  * Set the flag to allow the test cards.
	  */
	BEIDSDK_API virtual void setAllowTestCard(bool allow);

	/**
	 * Return a document from the card.
	 * Throw BEID_ExDocTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual BEID_XMLDoc& getDocument(BEID_DocumentType type);

	BEIDSDK_API BEID_EIdFullDoc& getFullDoc();				/**< Get the full document */
	BEIDSDK_API BEID_EId& getID();							/**< Get the id document */
	BEIDSDK_API BEID_Address& getAddr();					/**< Get the Address document */
	BEIDSDK_API BEID_Sod& getSod();							/**< Get the sod document */
	BEIDSDK_API BEID_CardVersionInfo& getVersionInfo();		/**< Get the info document  */

	BEIDSDK_API BEID_Certificate &getCert(BEID_CertifType type);/**< Return certificate by type from the card */
	BEIDSDK_API BEID_Certificate &getRoot();				/**< Return the root certificate from the card */
	BEIDSDK_API BEID_Certificate &getCA();					/**< Return the ca certificate from the card */
	BEIDSDK_API BEID_Certificate &getSignature();			/**< Return the signature certificate from the card */
	BEIDSDK_API BEID_Certificate &getAuthentication();		/**< Return the authentication certificate from the card */

	/**
	 * Return a raw data from the card.
	 * Throw BEID_ExFileTypeUnknown exception if the document doesn't exist for this card.
	 */
	BEIDSDK_API virtual const BEID_ByteArray& getRawData(BEID_RawDataType type);

 	BEIDSDK_API const BEID_ByteArray& getRawData_Id();				/**< Get the Id RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_IdSig();			/**< Get the IdSig RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Trace();			/**< Get the Trace RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Addr();			/**< Get the Addr RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_AddrSig();			/**< Get the AddrSig RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Sod();				/**< Get the Sod RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_CardInfo();		/**< Get the Card Info RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_TokenInfo();		/**< Get the Token Info RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Challenge();		/**< Get the challenge RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_Response();		/**< Get the response RawData */
 	BEIDSDK_API const BEID_ByteArray& getRawData_PersoData();		/**< Get the response RawData */

protected:
	BEID_EIDCard(const SDK_Context *context,APL_Card *impl);		/**< For internal use : Constructor */

private:
	BEID_EIDCard(const BEID_EIDCard& card);						/**< Copy not allowed - not implemented */
	BEID_EIDCard& operator= (const BEID_EIDCard& card);			/**< Copy not allowed - not implemented */

friend BEID_Card &BEID_ReaderContext::getCard();				/**< For internal use : This method must access protected constructor */
};

class APL_KidsCard;

/******************************************************************************//**
  * This class represents a Kids card which is a particular BEID_EIDCard.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class BEID_KidsCard : public BEID_EIDCard
{
public:
	BEIDSDK_API virtual ~BEID_KidsCard();				/**< Destructor */

private:
	BEID_KidsCard(const BEID_KidsCard& card);				/**< Copy not allowed - not implemented */
	BEID_KidsCard& operator= (const BEID_KidsCard& card);	/**< Copy not allowed - not implemented */

	BEID_KidsCard(const SDK_Context *context,APL_Card *impl);/**< For internal use : Constructor */

friend BEID_Card &BEID_ReaderContext::getCard();			/**< For internal use : This method must access protected constructor */
};

class APL_ForeignerCard;

/******************************************************************************//**
  * This class represents a Foreigner card which is a particular BEID_EIDCard.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class BEID_ForeignerCard : public BEID_EIDCard
{
public:
	BEIDSDK_API virtual ~BEID_ForeignerCard();					/**< Destructor */

private:
	BEID_ForeignerCard(const BEID_ForeignerCard& card);				/**< Copy not allowed - not implemented */
	BEID_ForeignerCard& operator= (const BEID_ForeignerCard& card);	/**< Copy not allowed - not implemented */

	BEID_ForeignerCard(const SDK_Context *context,APL_Card *impl);	/**< For internal use : Constructor */

friend BEID_Card &BEID_ReaderContext::getCard();	/**< For internal use : This method must access protected constructor */
};

class APL_XMLDoc;

/******************************************************************************//**
  * Abstract base class for all the documents.
  *********************************************************************************/
class BEID_XMLDoc : public BEID_Object
{
public:
	BEIDSDK_API virtual ~BEID_XMLDoc()=0;				/**< Destructor */

	BEIDSDK_API virtual bool isAllowed();						/**< The document is allowed */

	BEIDSDK_API virtual BEID_ByteArray getXML();			/**< Return the document in an XML format */
	BEIDSDK_API virtual BEID_ByteArray getCSV();			/**< Return the document in an CSV format */
	BEIDSDK_API virtual BEID_ByteArray getTLV();			/**< Return the document in an TLV format */

	/**
	  * Write the xml document into the file csFilePath.
	  * @return true if succeeded
	  */
	BEIDSDK_API virtual bool writeXmlToFile(const char * csFilePath);

	/**
	  * Write the csv document into the file csFilePath.
	  * @return true if succeeded
	  */
	BEIDSDK_API virtual bool writeCsvToFile(const char * csFilePath);

	/**
	  * Write the tlv document into the file csFilePath.
	  * @return true if succeeded
	  */
	BEIDSDK_API virtual bool writeTlvToFile(const char * csFilePath);

protected:
	BEID_XMLDoc(const SDK_Context *context,APL_XMLDoc *impl);		/**< For internal use : Constructor */

private:
	BEID_XMLDoc(const BEID_XMLDoc& doc);					/**< Copy not allowed - not implemented */
	BEID_XMLDoc& operator= (const BEID_XMLDoc& doc);		/**< Copy not allowed - not implemented */

};

class APL_Biometric;

/******************************************************************************//**
  * Abstract base class for the biometric documents.
  *********************************************************************************/
class BEID_Biometric : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_Biometric()=0;				/**< Destructor */

protected:
	BEID_Biometric(const SDK_Context *context,APL_Biometric *impl);	/**< For internal use : Constructor */

private:
	BEID_Biometric(const BEID_Biometric& doc);				/**< Copy not allowed - not implemented */
	BEID_Biometric& operator= (const BEID_Biometric& doc);	/**< Copy not allowed - not implemented */
};

class APL_Crypto;

/******************************************************************************//**
  * Abstract base class for the cryptographic documents.
  *********************************************************************************/
class BEID_Crypto : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_Crypto()=0;			/**< Destructor */

protected:
	BEID_Crypto(const SDK_Context *context,APL_Crypto *impl);	/**< For internal use : Constructor */

private:
	BEID_Crypto(const BEID_Crypto& doc);				/**< Copy not allowed - not implemented */
	BEID_Crypto& operator= (const BEID_Crypto& doc);	/**< Copy not allowed - not implemented */
};

class APL_DocVersionInfo;

/******************************************************************************//**
  * Class for the info document.
  * You can get such an object from BEID_EIDCard::getVersionInfo() (or getDocument).
  *********************************************************************************/
class BEID_CardVersionInfo : public BEID_XMLDoc
{
public:
	BEIDSDK_API  virtual ~BEID_CardVersionInfo();		/**< Destructor */

    BEIDSDK_API const char *getSerialNumber();			/**< Return the Serial Number of the card */
	BEIDSDK_API const char *getComponentCode();			/**< Return the ComponenCode of the card  */
	BEIDSDK_API const char *getOsNumber();				/**< Return the OS Number of the card */
	BEIDSDK_API const char *getOsVersion();				/**< Return the OS Version of the card */
	BEIDSDK_API const char *getSoftmaskNumber();			/**< Return the Softmask Number of the card */
	BEIDSDK_API const char *getSoftmaskVersion();		/**< Return the Softmask Version of the card */
	BEIDSDK_API const char *getAppletVersion();			/**< Return the Applet Version of the card */
	BEIDSDK_API const char *getGlobalOsVersion();		/**< Return the Global Os Version of the card */
	BEIDSDK_API const char *getAppletInterfaceVersion();	/**< Return the Applet Interface Version of the card */
	BEIDSDK_API const char *getPKCS1Support();			/**< Return the PKCS#1 Support of the card */
	BEIDSDK_API const char *getKeyExchangeVersion();		/**< Return the Key Exchange Version of the card */
	BEIDSDK_API const char *getAppletLifeCycle();		/**< Return the Applet Life Cycle of the card */
	BEIDSDK_API const char *getGraphicalPersonalisation();			/**< Return field GraphicalPersonalisation from the Info file */
	BEIDSDK_API const char *getElectricalPersonalisation();			/**< Return field ElectricalPersonalisation from the TokenInfo file */
	BEIDSDK_API const char *getElectricalPersonalisationInterface();	/**< Return field ElectricalPersonalisationInterface from the TokenInfo file */
	BEIDSDK_API const BEID_ByteArray &getSignature();		/**< Return the signature of the card info */

private:
	BEID_CardVersionInfo(const BEID_CardVersionInfo& doc);				/**< Copy not allowed - not implemented */
	BEID_CardVersionInfo& operator= (const BEID_CardVersionInfo& doc);	/**< Copy not allowed - not implemented */

	BEID_CardVersionInfo(const SDK_Context *context,APL_DocVersionInfo *impl);	/**< For internal use : Constructor */

friend BEID_CardVersionInfo& BEID_EIDCard::getVersionInfo();	/**< For internal use : This method must access protected constructor */
};

class APL_SodEid;

/******************************************************************************//**
  * Class for the sod document on a EID Card.
  * You can get such an object from BEID_EIDCard::getSod()	(or getDocument).
  *********************************************************************************/
class BEID_Sod : public BEID_Biometric
{
public:
	BEIDSDK_API virtual ~BEID_Sod();				/**< Destructor */

	BEIDSDK_API const BEID_ByteArray& getData();		/**< Return the sod itself */
	BEIDSDK_API const BEID_ByteArray& getHash();		/**< Return the hash of the sod */

private:
	BEID_Sod(const BEID_Sod& doc);				/**< Copy not allowed - not implemented */
	BEID_Sod& operator= (const BEID_Sod& doc);	/**< Copy not allowed - not implemented */

	BEID_Sod(const SDK_Context *context,APL_SodEid *impl);/**< For internal use : Constructor */

friend BEID_Sod& BEID_EIDCard::getSod();		/**< For internal use : This method must access protected constructor */
};

class APL_DocSisId;

/******************************************************************************//**
  * Class for the id document on a SIS Card.
  * You can get such an object from BEID_SISCard::getID()	(or getDocument).
  *********************************************************************************/
class BEID_SisId : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_SisId();					/**< Destructor */

    BEIDSDK_API const char *getName();					/**< Return Name field */
	BEIDSDK_API const char *getSurname();				/**< Return Surname field */
	BEIDSDK_API const char *getInitials();				/**< Return Initials field */
	BEIDSDK_API const char *getGender();					/**< Return Gender field */
	BEIDSDK_API const char *getDateOfBirth();			/**< Return Date Of Birth field */
	BEIDSDK_API const char *getSocialSecurityNumber();	/**< Return Social Security Number field */
	BEIDSDK_API const char *getLogicalNumber();			/**< Return Logical Number field */
	BEIDSDK_API const char *getDateOfIssue();			/**< Return Date Of Issue field */
	BEIDSDK_API const char *getValidityBeginDate();		/**< Return Validity Begin Date field */
	BEIDSDK_API const char *getValidityEndDate();		/**< Return Validity End Date field */

private:
	BEID_SisId(const BEID_SisId& doc);						/**< Copy not allowed - not implemented */
	BEID_SisId& operator= (const BEID_SisId& doc);			/**< Copy not allowed - not implemented */

	BEID_SisId(const SDK_Context *context,APL_DocSisId *impl);		/**< For internal use : Constructor */

friend BEID_SisId& BEID_SISCard::getID();					/**< For internal use : This method must access protected constructor */
};

class APL_DocEId;

/******************************************************************************//**
  * Class for the id document on a EID Card.
  * You can get such an object from BEID_EIDCard::getID()	(or getDocument).
  *********************************************************************************/
class BEID_EId : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_EId();						/**< Destructor */

	BEIDSDK_API const char *getDocumentVersion();		/**< Return Document Version field */
	BEIDSDK_API const char *getDocumentType();			/**< Return Document Type field */
	BEIDSDK_API const char *getCountry();				/**< Return Country field */
	BEIDSDK_API const char *getFirstName1();			/**< Return First Name part 1 (2 first given name) */
	BEIDSDK_API const char *getSurname();				/**< Return Surname field */
	BEIDSDK_API const char *getGender();				/**< Return Gender field */
	BEIDSDK_API const char *getDateOfBirth();			/**< Return Date Of Birth field */
	BEIDSDK_API const char *getLocationOfBirth();		/**< Return Location Of Birth field */
	BEIDSDK_API const char *getNationality();			/**< Return Nationality field */
	BEIDSDK_API const char *getNationalNumber();		/**< Return National Number field */
	BEIDSDK_API const char *getDuplicata();				/**< Return Duplicata field */
	BEIDSDK_API const char *getSpecialOrganization();	/**< Return Special Organization field */
	BEIDSDK_API const char *getMemberOfFamily();		/**< Return Member Of Family field */
	BEIDSDK_API const char *getLogicalNumber();			/**< Return Logical Number field */
	BEIDSDK_API const char *getChipNumber();			/**< Return Chip Number field */
	BEIDSDK_API const char *getValidityBeginDate();		/**< Return Validity Begin Date field */
	BEIDSDK_API const char *getValidityEndDate();		/**< Return Validity End Date field */
	//BEIDSDK_API const char *getAddressVersion();		/**< Return Address Version field */
	//BEIDSDK_API const char *getStreet();				/**< Return Street field */
	//BEIDSDK_API const char *getZipCode();				/**< Return Zip Code field */
	BEIDSDK_API const char *getSpecialStatus();			/**< Return Special Status field */
	/*New status for PTeid-ng */
	BEIDSDK_API	const char *getHeight();						/**< Return field Height */
	BEIDSDK_API	const char *getDocumentNumber();				/**< Return field DocumentNumber */
	BEIDSDK_API	const char *getCivilianIdNumber();				/**< Return field CivlianIdNumber */
	BEIDSDK_API	const char *getTaxNo();							/**< Return field TaxNo */
	BEIDSDK_API	const char *getSocialSecurityNo();				/**< Return field SocialSecurityNo */
	BEIDSDK_API	const char *getHealthNo();						/**< Return field HealthNo */
	BEIDSDK_API	const char *getIssuingEntity();					/**< Return field IssuingEntity */
	BEIDSDK_API const char *getLocalofRequest();				/**< Return field LocalofRequest*/
	BEIDSDK_API	const char *getGivenNameFather();				/**< Return field GivenNameFather */
	BEIDSDK_API	const char *getSurnameFather();					/**< Return field SurnameFather */
	BEIDSDK_API	const char *getGivenNameMother();				/**< Return field GivenNameMother */
	BEIDSDK_API	const char *getSurnameMother();					/**< Return field SurnameMother */
	BEIDSDK_API	const char *getParents();						/**< Return field Parents */
	BEIDSDK_API	const BEID_ByteArray& getPhoto();							/**< Return field Photo */
	BEIDSDK_API	const char *getPersoData();						/**< Return field PersoData */
	BEIDSDK_API	const char *getValidation();					/**< Return field Validation */
	BEIDSDK_API const char *getMRZ1();							/**< Return field MRZ block 1 */
	BEIDSDK_API const char *getMRZ2();							/**< Return field MRZ block 2 */
	BEIDSDK_API const char *getMRZ3();							/**< Return field MRZ block 3 */
	BEIDSDK_API const char *getAccidentalIndications();			/**< Return field AccidentalIndications */

private:
	BEID_EId(const BEID_EId& doc);							/**< Copy not allowed - not implemented */
	BEID_EId& operator= (const BEID_EId& doc);				/**< Copy not allowed - not implemented */

	BEID_EId(const SDK_Context *context,APL_DocEId *impl);			/**< For internal use : Constructor */

friend BEID_EId& BEID_EIDCard::getID();						/**< For internal use : This method must access protected constructor */
};

class APL_AddrEId;

/******************************************************************************//**
  * Class for the Address document on a EID Card.
  * You can get such an object from BEID_EIDCard::getAddr()	(or getDocument).
  *********************************************************************************/
class BEID_Address : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_Address();						/**< Destructor */

	//BEIDSDK_API const char *getAddressVersion();		/**< Return Address Version field */
	//BEIDSDK_API const char *getStreet();				/**< Return Street field */
	//BEIDSDK_API const char *getZipCode();				/**< Return Zip Code field */
	BEIDSDK_API const char *getMunicipality();			/**< Return Municipality field */
	BEIDSDK_API	const char *getDistrict();						/**< Return field District */
	BEIDSDK_API	const char *getStreetName();					/**< Return field StreetName */
	BEIDSDK_API	const char *getCivilParish();					/**< Return field CivilParish */
	BEIDSDK_API	const char *getStreetType1();					/**< Return field StreetType1 */
	BEIDSDK_API	const char *getStreetType2();					/**< Return field StreetType2 */
	BEIDSDK_API	const char *getBuildingType1();					/**< Return field BuildingType1 */
	BEIDSDK_API	const char *getBuildingType2();					/**< Return field BuildingType2 */
	BEIDSDK_API	const char *getDoorNo();						/**< Return field DoorNo */
	BEIDSDK_API	const char *getFloor();							/**< Return field Floor */
	BEIDSDK_API	const char *getSide();							/**< Return field Side */
	BEIDSDK_API	const char *getLocality();						/**< Return field Locality */
	BEIDSDK_API	const char *getPlace();						/**< Return field Locality */
	BEIDSDK_API	const char *getZip4();							/**< Return field Zip4 */
	BEIDSDK_API	const char *getZip3();							/**< Return field Zip3 */
	BEIDSDK_API	const char *getPostalLocality();				/**< Return field PostalLocality */

private:
	BEID_Address(const BEID_Address& doc);							/**< Copy not allowed - not implemented */
	BEID_Address& operator= (const BEID_Address& doc);				/**< Copy not allowed - not implemented */

	BEID_Address(const SDK_Context *context,APL_AddrEId *impl);			/**< For internal use : Constructor */

friend BEID_Address& BEID_EIDCard::getAddr();						/**< For internal use : This method must access protected constructor */
};

class APL_SisFullDoc;

/******************************************************************************//**
  * Class for the full document Sis.
  *********************************************************************************/
class BEID_SisFullDoc : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_SisFullDoc();					/**< Destructor */

protected:
	BEID_SisFullDoc(const SDK_Context *context,APL_SisFullDoc *impl);	/**< For internal use : Constructor */

private:
	BEID_SisFullDoc(const BEID_SisFullDoc& doc);				/**< Copy not allowed - not implemented */
	BEID_SisFullDoc& operator= (const BEID_SisFullDoc& doc);	/**< Copy not allowed - not implemented */

friend BEID_SisFullDoc& BEID_SISCard::getFullDoc();				/**< For internal use : This method must access protected constructor */
};

class APL_EIdFullDoc;

/******************************************************************************//**
  * Class for the full document Eid.
  *********************************************************************************/
class BEID_EIdFullDoc : public BEID_XMLDoc
{
public:
	BEIDSDK_API virtual ~BEID_EIdFullDoc();					/**< Destructor */

protected:
	BEID_EIdFullDoc(const SDK_Context *context,APL_EIdFullDoc *impl);	/**< For internal use : Constructor */

private:
	BEID_EIdFullDoc(const BEID_EIdFullDoc& doc);				/**< Copy not allowed - not implemented */
	BEID_EIdFullDoc& operator= (const BEID_EIdFullDoc& doc);	/**< Copy not allowed - not implemented */

friend BEID_EIdFullDoc& BEID_EIDCard::getFullDoc();				/**< For internal use : This method must access protected constructor */
};
class BEID_Pin;
class APL_Pins;

/******************************************************************************//**
  * Container class for all pins on the card.
  *********************************************************************************/
class BEID_Pins : public BEID_Crypto
{
public:
	BEIDSDK_API virtual ~BEID_Pins();				/**< Destructor */

	BEIDSDK_API unsigned long count();						/**< The number of pins on the card */

	/**
	  * Get the pin by its Index.
	  * Throw BEID_ExParamRange exception if the index is out of range.
 	  */
	BEIDSDK_API BEID_Pin &getPinByNumber(unsigned long ulIndex);

private:
	BEID_Pins(const BEID_Pins& pins);					/**< Copy not allowed - not implemented */
	BEID_Pins& operator= (const BEID_Pins& pins);		/**< Copy not allowed - not implemented */

	BEID_Pins(const SDK_Context *context,APL_Pins *impl);		/**< For internal use : Constructor */

friend BEID_Pins& BEID_SmartCard::getPins();			/**< For internal use : This method must access protected constructor */
};

class APL_Pin;

/******************************************************************************//**
  * Class that represent one Pin.
  *********************************************************************************/
class BEID_Pin : public BEID_Crypto
{
public:
	BEIDSDK_API virtual ~BEID_Pin();				/**< Destructor */

	BEIDSDK_API unsigned long getIndex();		/**< Get the index of the pin */
	BEIDSDK_API unsigned long getType();			/**< Get the type of the pin */
	BEIDSDK_API unsigned long getId();			/**< Get the id of the pin */
	BEIDSDK_API BEID_PinUsage getUsageCode();	/**< Get the usage code of the pin */
	BEIDSDK_API unsigned long getFlags();		/**< Get the flags of the pin */
	BEIDSDK_API const char *getLabel();			/**< Get the label of the pin */

	BEIDSDK_API const BEID_ByteArray &getSignature();	/**< Return the signature of the pin */

	/**
	  * Return the remaining tries for giving the good pin.
	  *
	  * This opperation is not supported by all card.
	  *
	  * @return -1 if not supported
	  * @return the number of remaining tries in the other case
	  */
	BEIDSDK_API long getTriesLeft();

	/**
	  * Ask the card to verify the pin.
	  * A popup will ask for the code.
	  * @return true if success and false if failed
	  */
	BEIDSDK_API bool verifyPin();

	/**
	  * Ask the card to verify the pin.
	  *
	  * @param csPin is the pin code to verify (if csPin is empty, a popup will ask for the code)
	  * @param ulRemaining return the remaining tries (only if verifying failed)
	  *
	  * @return true if success and false if failed
	  */
	BEIDSDK_API bool verifyPin(const char *csPin,unsigned long &ulRemaining);

	/**
	  * Ask the card to change the pin.
	  * A popup will ask for the codes
	  * @return true if success and false if failed
	  */
	BEIDSDK_API bool changePin();

	/**
	  * Ask the card to change the pin.
	  *
	  * if csPin1 or csPin2 are empty, a popup will ask for the codes.
	  *
	  * @param csPin1 is the old pin code
	  * @param csPin2 is the new pin code
	  * @param ulRemaining return the remaining tries (only when operation failed)
	  *
	  * @return true if success and false if failed
	  */
	BEIDSDK_API bool changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName);

private:
	BEID_Pin(const BEID_Pin& pin);									/**< Copy not allowed - not implemented */
	BEID_Pin& operator= (const BEID_Pin& pin);						/**< Copy not allowed - not implemented */

	BEID_Pin(const SDK_Context *context,APL_Pin *impl);						/**< For internal use : Constructor */

friend BEID_Pin &BEID_Pins::getPinByNumber(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
};

class APL_Crl;

/******************************************************************************//**
  * Class that represents one CRL.
  *********************************************************************************/
class BEID_Crl : public BEID_Object
{
public:
	/**
	  * Create a Crl from its uri (without any certificate link).
	  * As there is no issuer, this CRL can't be verify and some method are not allowed
	  * (ex. getIssuer).
	  * These methods throw BEID_ExBadUsage exception.
	  */
	BEIDSDK_API BEID_Crl(const char *uri);

	BEIDSDK_API virtual ~BEID_Crl(void);				/**< Destructor */

	BEIDSDK_API const char *getUri();				/**< Return the uri of the CRL */

	BEIDSDK_API const char *getIssuerName();			/**< Return the name of the issuer of the certificate */

	/**
	  * Return the issuer certificate.
	  *
	  * if there is no issuer (root), BEID_ExCertNoIssuer exception is thrown.
	  */
	BEIDSDK_API BEID_Certificate &getIssuer();

	/**
	  * Return the CRL as a byte array.
	  * If it comes from a Certif we verify the signature.
	  * If it's created from the URL only we DON'T verify the signature.
	  * @param crl will content the crl
	  * @param bForceDownload : if true the CRL in the cache is not valid anymore and we force a new download
	  */
	BEIDSDK_API BEID_CrlStatus getData(BEID_ByteArray &crl,bool bForceDownload=false);

	NOEXPORT_BEIDSDK BEID_Crl(const SDK_Context *context,APL_Crl *impl);	/**< For internal use : Constructor */

private:
	BEID_Crl(const BEID_Crl& crl);				/**< Copy not allowed - not implemented */
	BEID_Crl &operator= (const BEID_Crl& crl);	/**< Copy not allowed - not implemented */
};

class APL_OcspResponse;

/******************************************************************************//**
  * Class that represents one OCSP Response.
  *********************************************************************************/
class BEID_OcspResponse : public BEID_Object
{
public:
	/**
	  * Create an OcspResponse object from the URI only and CertID.
	  * This OCSP Response is not link to any certificate so some methods could not be used.
	  * These methods throw BEID_ExBadUsage exception.
	  */
	BEIDSDK_API BEID_OcspResponse(const char *uri,BEID_HashAlgo hashAlgorithm,const BEID_ByteArray &issuerNameHash,const BEID_ByteArray &issuerKeyHash,const BEID_ByteArray &serialNumber);

	BEIDSDK_API virtual ~BEID_OcspResponse(void);		/**< Destructor */

	BEIDSDK_API const char *getUri();					/**< Return the uri of the responder */

	/**
	  * Return the response.
	  */
	BEIDSDK_API BEID_CertifStatus getResponse(BEID_ByteArray &response);

	NOEXPORT_BEIDSDK BEID_OcspResponse(const SDK_Context *context,APL_OcspResponse *impl);	/**< For internal use : Constructor */

private:
	BEID_OcspResponse(const BEID_OcspResponse& ocsp);				/**< Copy not allowed - not implemented */
	BEID_OcspResponse &operator= (const BEID_OcspResponse& ocsp);	/**< Copy not allowed - not implemented */
};

class APL_Certifs;

/******************************************************************************//**
  * Container class for all certificates on the card.
  *********************************************************************************/
class BEID_Certificates : public BEID_Crypto
{
public:
	/**
	  * Create an BEID_Certificates store without any link to a card.
	  * This store is not link to any Card, so some methods could not be used.
	  * These methods throw BEID_ExBadUsage exception.
	  */
	BEIDSDK_API BEID_Certificates();

	BEIDSDK_API virtual ~BEID_Certificates();					/**< Destructor */

	BEIDSDK_API unsigned long countFromCard();					/**< The number of certificates on the card */
	BEIDSDK_API unsigned long countAll();						/**< The number of certificates (on the card or not) */

	/**
	  * Get the ulIndex certificate from the card.
	  * Throw BEID_ExParamRange exception if the index is out of range.
	  */
	BEIDSDK_API BEID_Certificate &getCertFromCard(unsigned long ulIndexCard);

	/**
	  * Return the certificate with the number ulIndexAll.
	  *
	  * ATTENTION ulIndexAll and ulIndexCard are two different index.
	  * Index will change if new certificates are added with addCert().
	  */
	BEIDSDK_API BEID_Certificate &getCert(unsigned long ulIndexAll);

	/**
	  * Return the certificate by type.
	  */
	BEIDSDK_API BEID_Certificate &getCert(BEID_CertifType type);

	BEIDSDK_API BEID_Certificate &getRoot();					/**< Return the root certificate from the card */
	BEIDSDK_API BEID_Certificate &getCA();						/**< Return the ca certificate from the card */
	BEIDSDK_API BEID_Certificate &getSignature();				/**< Return the signature certificate from the card */
	BEIDSDK_API BEID_Certificate &getAuthentication();			/**< Return the authentication certificate from the card */

	/**
	  * Add a new certificate to the store.
	  */
	BEIDSDK_API BEID_Certificate &addCertificate(BEID_ByteArray &cert);


private:
	BEID_Certificates(const BEID_Certificates& certifs);			/**< Copy not allowed - not implemented */
	BEID_Certificates& operator= (const BEID_Certificates& certifs);/**< Copy not allowed - not implemented */

	BEID_Certificates(const SDK_Context *context,APL_Certifs *impl);/**< For internal use : Constructor */

friend BEID_Certificates& BEID_SmartCard::getCertificates();		/**< For internal use : This method must access protected constructor */
};

class APL_Certif;

/******************************************************************************//**
  * Class that represent one certificate.
  *********************************************************************************/
class BEID_Certificate : public BEID_Crypto
{
public:
	BEIDSDK_API virtual ~BEID_Certificate();			/**< Destructor */

	BEIDSDK_API const char *getLabel();				/**< Return the label of the certificate */
	BEIDSDK_API unsigned long getID();				/**< Return the id of the certificate */

	/**
	  * Return the status of the certificate using default validation level (from config).
	  */
	BEIDSDK_API BEID_CertifStatus getStatus();

	/**
	  * Return the status of the certificate.
	  */
	BEIDSDK_API BEID_CertifStatus getStatus(BEID_ValidationLevel crl, BEID_ValidationLevel ocsp);

	BEIDSDK_API BEID_CertifType getType();			/**< Return the type of the certificate */

	BEIDSDK_API const BEID_ByteArray &getCertData();/**< Return the content of the certificate */
	BEIDSDK_API void getFormattedData(BEID_ByteArray &data);	/**< Return the content of the certificate without ending zero */
	BEIDSDK_API const char *getSerialNumber();		/**< Return the serial number of the certificate */
	BEIDSDK_API const char *getOwnerName();			/**< Return the name of the owner of the certificate */
	BEIDSDK_API const char *getIssuerName();			/**< Return the name of the issuer of the certificate */
	BEIDSDK_API const char *getValidityBegin();		/**< Return the validity begin date of the certificate */
	BEIDSDK_API const char *getValidityEnd();		/**< Return the validity end date of the certificate */
	BEIDSDK_API unsigned long getKeyLength();		/**< Return the length of public/private key on the certificate */

	/**
	  * Return true if this is a root certificate.
	  */
	BEIDSDK_API bool isRoot();

	/**
	  * Return the test status.
	  *
	  * @return true if test certificate
	  * @return false if good one
	  */
	BEIDSDK_API bool isTest();

	/**
	  * Return true if the certificate chain end by the one of beid root.
	  */
	BEIDSDK_API bool isFromBeidValidChain();

	/**
	  * This certificate comes from the card.
	  */
	BEIDSDK_API bool isFromCard();

	/**
	  * Return the issuer certificate.
	  *
	  * if there is no issuer (root), BEID_ExCertNoIssuer exception is thrown
	  */
	BEIDSDK_API BEID_Certificate &getIssuer();

	/**
	  * Return the number of children for this certificate.
	  */
	BEIDSDK_API unsigned long countChildren();

	/**
	  * Return the children certificate (certificate that has been issued by this one).
	  *
	  * @param ulIndex is the children index (the index for the first child is 0)
	  * Throw BEID_ExParamRange exception if the index is out of range
	  */
	BEIDSDK_API BEID_Certificate &getChildren(unsigned long ulIndex);

	/**
	  * Return the crl of the certificate.
	  */
	BEIDSDK_API BEID_Crl &getCRL();

	/**
	  * Return the ocsp response object of the certificate.
	  */
	BEIDSDK_API BEID_OcspResponse &getOcspResponse();


	BEIDSDK_API BEID_CertifStatus verifyCRL(bool forceDownload=false);		/**< Verify the certificate trough CRL validation */
	BEIDSDK_API BEID_CertifStatus verifyOCSP();								/**< Verify the certificate trough OCSP validation */

private:
	BEID_Certificate(const BEID_Certificate& certif);				/**< Copy not allowed - not implemented */
	BEID_Certificate& operator= (const BEID_Certificate& certif);	/**< Copy not allowed - not implemented */

	BEID_Certificate(const SDK_Context *context,APL_Certif *impl);	/**< For internal use : Constructor */

friend BEID_Certificate &BEID_Certificates::getCert(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
friend BEID_Certificate &BEID_Certificates::getCertFromCard(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
friend BEID_Certificate &BEID_Certificates::getCert(BEID_CertifType type);			/**< For internal use : This method must access protected constructor */
friend BEID_Certificate &BEID_Crl::getIssuer();										/**< For internal use : This method must access protected constructor */
friend BEID_Certificate &BEID_Certificates::addCertificate(BEID_ByteArray &cert);	/**< For internal use : This method must access protected constructor */
};

class APL_Config;

/******************************************************************************//**
  * Class to access the config parameters.
  *********************************************************************************/
class BEID_Config : public BEID_Object
{
public:
	/**
	  * Create object to access parameter Param.
	  */
    BEIDSDK_API BEID_Config(BEID_Param Param);

 	/**
	  * Create object to access a string parameter.
	  */
    BEIDSDK_API BEID_Config(const char *csName, const char *czSection, const char *csDefaultValue);

	/**
	  * Create object to access a numerical parameter.
	  */
    BEIDSDK_API BEID_Config(const char *csName, const char *czSection, long lDefaultValue);

	BEIDSDK_API virtual ~BEID_Config();			/**< Destructor */

    BEIDSDK_API const char *getString();		/**< Return the string value (Throw exception for numerical parameter) */
    BEIDSDK_API long getLong();					/**< Return the numerical value (Throw exception for string parameter) */

    BEIDSDK_API void setString(const char *csValue);	/**< Set the string value (Throw exception for numerical parameter) */
	BEIDSDK_API void setLong(long lValue);				/**< Set the numerical value (Throw exception for string parameter) */

private:
	BEID_Config(const BEID_Config& config);				/**< Copy not allowed - not implemented */
	BEID_Config& operator= (const BEID_Config& config);	/**< Copy not allowed - not implemented */

	BEID_Config(APL_Config *impl);	/**< For internal use : Constructor */
};

/******************************************************************************//**
  * Function for Logging.
  *********************************************************************************/
BEIDSDK_API void BEID_LOG(BEID_LogLevel level, const char *module_name, const char *format, ...);

}

#endif //__BEIDLIB_H__
