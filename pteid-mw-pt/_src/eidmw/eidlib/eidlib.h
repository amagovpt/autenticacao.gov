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

#ifndef __PTEIDLIB_H__
#define __PTEIDLIB_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include "eidlibdefines.h"
#include "xmlUserData.h"

namespace eIDMW
{

struct SDK_Context;

class CMutex;
class PTEID_Exception;

/******************************************************************************//**
  * Base class for the object of PTEID SDK (Can not be instantiated).
  *********************************************************************************/
class PTEID_Object
{
public:
	PTEIDSDK_API virtual ~PTEID_Object()=0;		/**< Destructor */


	NOEXPORT_PTEIDSDK void Init(const SDK_Context *context,void *impl);	/**< For internal use : Initialize pimpl */

	/**
	  * Release the objects instantiated within this PTEID_Object.
	  * Automatically call in the destructor.
 	  */
	NOEXPORT_PTEIDSDK void Release();

protected:
	PTEID_Object(const SDK_Context *context,void *impl);	/**< For internal use : Constructor */
	void addObject(PTEID_Object *impl);			/**< For internal use : Add linked object */
	void backupObject(unsigned long idx);		/**< For internal use : Backup linked object */
	PTEID_Object *getObject(unsigned long idx);	/**< For internal use : Return an object by its index */
	PTEID_Object *getObject(void *impl);			/**< For internal use : Return an object by its impl */
	void delObject(unsigned long idx);			/**< For internal use : Delete an object by its index */
	void delObject(void *impl);					/**< For internal use : Delete an object by its impl */

	void checkContextStillOk() const;			/**< For internal use : check if the Context is still correct (the card hasn't changed) */

	PTEID_Object(const PTEID_Object& obj);				/**< Copy not allowed - not implemented */
	PTEID_Object& operator= (const PTEID_Object& obj);	/**< Copy not allowed - not implemented */

	bool m_delimpl;							/**< For internal use : m_impl object must be deleted */
	void *m_impl;							/**< For internal use : pimpl pointer */
	unsigned long m_ulIndexExtAdd;			/**< For internal use : extended add object */
	std::map<unsigned long,PTEID_Object *> m_objects;	/**< For internal use : Map of object instantiated within this PTEID_Object */

	SDK_Context *m_context;					/**< For internal use : context structure */

	//CMutex *m_mutex;
};

class PTEID_Card;

class CByteArray;

/******************************************************************************//**
  * This class is used to receive/pass bytes array from/to different method in the SDK.
  *********************************************************************************/
class PTEID_ByteArray : public PTEID_Object
{
public:
    PTEIDSDK_API PTEID_ByteArray();								/**< Default constructor */
	PTEIDSDK_API PTEID_ByteArray(const PTEID_ByteArray &bytearray); /**< Copy constructor */

	 /**
	  * Constructor - initialize a byte array with an array of unsigned char.
	  *
	  * @param pucData is the byte array
	  * @param ulSize is the size of the array
	  */
    PTEIDSDK_API PTEID_ByteArray(const unsigned char * pucData, unsigned long ulSize);

	PTEIDSDK_API virtual ~PTEID_ByteArray();				/**< Destructor */

	 /**
	  * Append data to the byte array.
	  *
	  * @param pucData is the byte array
	  * @param ulSize is the size of the array
	  */
    PTEIDSDK_API void Append(const unsigned char * pucData, unsigned long ulSize);

	 /**
	  * Append data to the byte array.
	  */
    PTEIDSDK_API void Append(const PTEID_ByteArray &data);

	 /**
	  * Remove the data from the byte array.
	  */
    PTEIDSDK_API void Clear();

	 /**
	  * Return true if the content of data is the same as this.
	  */
    PTEIDSDK_API bool Equals(const PTEID_ByteArray &data) const;

 	 /**
	  * Return the number of bytes in the array.
	  */
    PTEIDSDK_API unsigned long Size() const;

 	/**
	  * Return the array of bytes in the object.
	  * If Size() == 0, then NULL is returned.
	  */
	PTEIDSDK_API const unsigned char *GetBytes() const;

	/**
	  * Writing the binary content to a file.
	  */
	PTEIDSDK_API bool writeToFile(const char * csFilePath);

	/**
	  * Copy content of bytearray.
	  */
	PTEIDSDK_API PTEID_ByteArray &operator=(const PTEID_ByteArray &bytearray);

	NOEXPORT_PTEIDSDK PTEID_ByteArray(const SDK_Context *context,const CByteArray &impl);	/**< For internal use : construct from lower level object*/
	NOEXPORT_PTEIDSDK PTEID_ByteArray &operator=(const CByteArray &bytearray);			/**< For internal use : copy from lower level object*/
};

class PhotoPteid;

class PTEID_Photo : public PTEID_Object
{
public:
	PTEIDSDK_API virtual ~PTEID_Photo();												/**< Destructor */

	PTEIDSDK_API PTEID_ByteArray& getphotoRAW();
	PTEIDSDK_API PTEID_ByteArray& getphoto();
	PTEIDSDK_API PTEID_ByteArray& getphotoCbeff();
	PTEIDSDK_API PTEID_ByteArray& getphotoFacialrechdr();
	PTEIDSDK_API PTEID_ByteArray& getphotoFacialinfo();
	PTEIDSDK_API PTEID_ByteArray& getphotoImageinfo();

	NOEXPORT_PTEIDSDK PTEID_Photo(const SDK_Context *context,const PhotoPteid &impl);

private:
	PTEID_Photo(const PTEID_Photo& photo);				/**< Copy not allowed - not implemented */
	PTEID_Photo& operator= (const PTEID_Photo& photo);	/**< Copy not allowed - not implemented */
};


/******************************************************************************//**
  * These structure are used for compatibility with old C sdk.
  *********************************************************************************/
struct PTEID_RawData_Eid
{
    PTEID_ByteArray idData;
    PTEID_ByteArray idSigData;
    PTEID_ByteArray addrData;
    PTEID_ByteArray addrSigData;
    PTEID_ByteArray sodData;
    PTEID_ByteArray cardData;
    PTEID_ByteArray tokenInfo;
    PTEID_ByteArray certRN;
    PTEID_ByteArray challenge;
    PTEID_ByteArray response;
    PTEID_ByteArray persoData;
    PTEID_ByteArray trace;
};

/**
  * This define give an easy access to singleton (no declaration/instantiation is needed).
  *
  * Usage : ReaderSet.SomeMethod().
  */
#define ReaderSet PTEID_ReaderSet::instance()

/**
  * Init the SDK (Optional).
  */
#define PTEID_InitSDK() PTEID_ReaderSet::initSDK()

/**
  * Release the SDK.
  * THIS MUST BE CALLED WHEN THE SDK IS NOT NEEDED ANYMORE AND BEFORE THE APPLICATION IS CALLED.
  * IF NOT RELEASE PROPERLY, AN EXCEPTION PTEID_ExReleaseNeeded IS THROWN.
  */
#define PTEID_ReleaseSDK() PTEID_ReaderSet::releaseSDK()

class PTEID_ReaderContext;
class APL_ReaderContext;

/******************************************************************************//**
  * This is a singleton class that is the starting point to get all other objects.
  * You get an instance from the static instance() method (or using the define ReaderSet).
  * Then you get a READER							(PTEID_ReaderContext)
  *		-> from this reader, you a CARD				(PTEID_Card or derived class)
  *			-> from this card, you get DOCUMENT		(PTEID_XMLDoc or derived class)
  *				-> ...
  *********************************************************************************/
class PTEID_ReaderSet : public PTEID_Object
{
public:
    PTEIDSDK_API static PTEID_ReaderSet &instance();		/**< Return the singleton object (create it at first use) */

	/**
	  * Init the SDK (Optional).
	  * @param bManageTestCard If true the applayer must ask if test cards are allowed (used for compatibility with old C API).
	  * @param bManageTestCard If false other applications (ex. gui) take that into their scope
	  */
	PTEIDSDK_API static void initSDK(bool bManageTestCard=false);
	PTEIDSDK_API static void releaseSDK();				/**< Release the SDK */

	PTEIDSDK_API virtual ~PTEID_ReaderSet();				/**< Destructor */

	/**
	  * Release the readers (Useful if readers had changed).
	  *
	  * @param bAllReference If true all the invalid reference/pointer are destroyed.
	  * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	  * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	  */
	PTEIDSDK_API void releaseReaders(bool bAllReference=false);

	/**
	  * Return true if readers has been added or removed
	  */
	PTEIDSDK_API bool isReadersChanged() const;

	/**
	  * Get the list of the reader.
	  * Return an array of const char *
	  * The last pointer is NULL
	  * Usage : const char * const *ppList=PTEID_ReaderSet::readerList();
	  *         for(const char * const *ppName=ppList;*ppName!=NULL;ppName++) {...}
	  *
	  * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	  */
    PTEIDSDK_API const char * const *readerList(bool bForceRefresh=false);

	/**
	  * Return the first readercontext with a card.
	  *		If no card is present, return the firs reader.
	  *		If no reader exist, throw an exception PTEID_ExNoReader.
	  */
	PTEIDSDK_API PTEID_ReaderContext &getReader();

	/**
	  * Get the reader by its name.
 	  */
	PTEIDSDK_API PTEID_ReaderContext &getReaderByName(const char *readerName);

 	/**
	  * Return the number of card readers connected to the computer.
	  *
	  * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	  */
    PTEIDSDK_API unsigned long readerCount(bool bForceRefresh=false);

	/**
	  * Get the name of the reader by its Index.
	  * Throw PTEID_ExParamRange exception if the index is out of range.
 	  */
	PTEIDSDK_API const char *getReaderName(unsigned long ulIndex);

	/**
	  * Get the reader by its Index.
	  * Throw PTEID_ExParamRange exception if the index is out of range.
 	  */
	PTEIDSDK_API PTEID_ReaderContext &getReaderByNum(unsigned long ulIndex);

	/**
	  * Return the reader containing the card with this SN.
	  *		If no card with this SN is found, throw an exception PTEID_ExParamRange.
	  */
	PTEIDSDK_API PTEID_ReaderContext &getReaderByCardSerialNumber(const char *cardSerialNumber);

	/**
	* Flush the cached files.
	* Return if any files were flushed (T/F).
	*/;
	PTEIDSDK_API bool flushCache();			/**< Flush the cache */

	NOEXPORT_PTEIDSDK PTEID_ReaderContext &getReader(APL_ReaderContext *pAplReader); /**< For internal use - Not exported*/

private:
	PTEID_ReaderSet();											/**< For internal use : Constructor */

	PTEID_ReaderSet(const PTEID_ReaderSet& reader);				/**< Copy not allowed - not implemented */
	PTEID_ReaderSet& operator= (const PTEID_ReaderSet& reader);	/**< Copy not allowed - not implemented */

};

class PTEID_Card;
class PTEID_EIDCard;

/******************************************************************************//**
  * This class represent a reader.
  * You get reader object from the ReaderSet
  *		either by its index (getReaderByNum) or by its name (getReaderByName).
  * Once you have a reader object, you can check if a card is present (isCardPresent).
  * Then you can ask which type of card is in the reader with getCardType()
  *		and then get a card object using one of this method :
  *			getCard or getEIDCard.
  *********************************************************************************/
class PTEID_ReaderContext : public PTEID_Object
{
public:
	/**
	  * Construct using a fileType and fileName.
	  *		No physical reader are connected (m_reader=NULL)
	  */
	PTEIDSDK_API PTEID_ReaderContext(PTEID_FileType fileType,const char *fileName);

	/**
	  * Construct using a fileType and its content (for compatibility with SetRawFile).
	  *		No physical reader are connected (m_reader=NULL)
	  */
	PTEIDSDK_API PTEID_ReaderContext(PTEID_FileType fileType,const PTEID_ByteArray &data);

	/**
	  * Construct using Raw data for Eid.
	  *		No physical reader are connected (m_reader=NULL)
	  */
	PTEIDSDK_API PTEID_ReaderContext(const PTEID_RawData_Eid &data);

	PTEIDSDK_API virtual ~PTEID_ReaderContext();	/**< Destructor */

	/**
	  * Return the name of the reader.
	  */
	PTEIDSDK_API const char *getName();

	/**
	  * Return true if a card is present and false otherwise.
	  */
    PTEIDSDK_API bool isCardPresent();

	/**
	  * Release the card.
	  *
	  * @param bAllReference If true all the invalid reference/pointer are destroyed.
	  * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	  * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	  */
	PTEIDSDK_API void releaseCard(bool bAllReference=false);

	/**
	  * Return true if a card has changed since the last called (with the same ulOldId parameter).
	  */
	PTEIDSDK_API bool isCardChanged(unsigned long &ulOldId);

	/**
	 * Return the type of the card in the reader.
	 *
	 * Throw PTEID_ExNoCardPresent exception if no card is present.
	 */
	PTEIDSDK_API PTEID_CardType getCardType();

	/**
	 * Get the card in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(PTEID_EIDCard).
	 *
	 * If no card is present in the reader, exception PTEID_ExNoCardPresent is thrown.
	 * If the card type is not supported, exception PTEID_ExCardTypeUnknown is thrown.
	 */
    PTEIDSDK_API PTEID_Card &getCard();

	/**
	 * Get the EIDcard in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(PTEID_EIDCard).
	 *
	 * If no card is present in the reader, exception PTEID_ExNoCardPresent is thrown.
	 * If the card is not an EIDcard, exception PTEID_ExCardBadType is thrown.
	 */
    PTEIDSDK_API PTEID_EIDCard &getEIDCard();

	/**
	 * Specify a callback function to be called each time a
	 * card is inserted/remove in/from this reader.
	 *
	 * @return A handle can be used to stop the callbacks when they are no longer needed.
	 */
    PTEIDSDK_API unsigned long SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef);

	/**
	  * To tell that the callbacks are not longer needed.
	  * @param ulHandle is the handle return by SetEventCallback
	  */
	PTEIDSDK_API void StopEventCallback(unsigned long ulHandle);

	PTEIDSDK_API void BeginTransaction();								/**< Begin a transaction with the reader */
	PTEIDSDK_API void EndTransaction();								/**< End the transaction */

	PTEIDSDK_API bool isVirtualReader();								/**< Return true if this is a virtual reader (create from a file) */

private:
	PTEID_ReaderContext(const PTEID_ReaderContext& reader);				/**< Copy not allowed - not implemented */
	PTEID_ReaderContext& operator= (const PTEID_ReaderContext& reader);	/**< Copy not allowed - not implemented */

	PTEID_ReaderContext(const SDK_Context *context,APL_ReaderContext *impl);						/**< For internal use : Constructor */

	unsigned long m_cardid;
	//CMutex *m_mutex;

friend PTEID_ReaderContext &PTEID_ReaderSet::getReader(APL_ReaderContext *pAplReader); /**< For internal use : This method must access protected constructor */
};

class PTEID_XMLDoc;
class APL_Card;

/******************************************************************************//**
  * Abstract base class for all the card type supported.
  * The PTEID_ReaderContext::getCard() method will return such an object.
  *********************************************************************************/
class PTEID_Card : public PTEID_Object
{
public:
	PTEIDSDK_API virtual ~PTEID_Card()=0;				/**< Destructor */

 	/**
	 * Return the type of the card
	 */
	PTEIDSDK_API virtual PTEID_CardType getType();

 	/**
	 * Return a document from the card.
	 * Throw PTEID_ExDocTypeUnknown exception if the document doesn't exist for this card.
	 */
	PTEIDSDK_API virtual PTEID_XMLDoc& getDocument(PTEID_DocumentType type)=0;

 	/**
	 * Return a raw data from the card.
	 * Throw PTEID_ExFileTypeUnknown exception if the document doesn't exist for this card.
	 */
	PTEIDSDK_API virtual const PTEID_ByteArray& getRawData(PTEID_RawDataType type)=0;

 	/**
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @return a PTEID_ByteArray containing the result
	 */
    PTEIDSDK_API virtual PTEID_ByteArray sendAPDU(const PTEID_ByteArray& cmd);

    PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray& oData);

 	/**
	 * Read a File from the card.
	 * @param fileID is the path of the file
	 * @param ulOffset is the offset to begins the reading
	 * @param ulMaxLength is the maximum length of bytes to read
	 * @return A PTEID_ByteArray with the content of the file
	 */
	PTEIDSDK_API virtual PTEID_ByteArray readFile(const char *fileID, unsigned long  ulOffset=0, unsigned long  ulMaxLength=0);

 	/**
	 * Write a file to the card.
	 * @param fileID is the path of the file
	 * @param oData contents the bytes to write
	 * @param ulOffset is the offset to begins the writing
	 */
	PTEIDSDK_API virtual bool writeFile(const char *fileID, const PTEID_ByteArray& oData,unsigned long ulOffset=0);

protected:
	PTEID_Card(const SDK_Context *context,APL_Card *impl);/**< For internal use : Constructor */

private:
	PTEID_Card(const PTEID_Card& card);					/**< Copy not allowed - not implemented */
	PTEID_Card& operator= (const PTEID_Card& card);		/**< Copy not allowed - not implemented */

};

class APL_MemoryCard;

/******************************************************************************//**
  * Abstract base class for Memory card.
  *********************************************************************************/
class PTEID_MemoryCard : public PTEID_Card
{
public:
	PTEIDSDK_API virtual ~PTEID_MemoryCard()=0;				/**< Destructor */

protected:
	PTEID_MemoryCard(const SDK_Context *context,APL_Card *impl);/**< For internal use : Constructor */

private:
	PTEID_MemoryCard(const PTEID_MemoryCard& card);				/**< Copy not allowed - not implemented */
	PTEID_MemoryCard& operator= (const PTEID_MemoryCard& card);	/**< Copy not allowed - not implemented */
};

class PTEID_Pin;
class PTEID_Pins;
class PTEID_Certificates;
class APL_SmartCard;

/******************************************************************************//**
  * Abstract base class for Smart card.
  *********************************************************************************/
class PTEID_SmartCard : public PTEID_Card
{
public:
	PTEIDSDK_API virtual ~PTEID_SmartCard()=0;				/**< Destructor */

 	/**
	 * Select an application from the card
	 */
	PTEIDSDK_API void selectApplication(const PTEID_ByteArray &applicationId);

 	/**
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @param pin is the pin to ask for writing
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 * @return a PTEID_ByteArray containing the result
	 */
    PTEIDSDK_API virtual PTEID_ByteArray sendAPDU(const PTEID_ByteArray& cmd,PTEID_Pin *pin=NULL,const char *csPinCode="");

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
	PTEIDSDK_API virtual long readFile(const char *fileID, PTEID_ByteArray &in,PTEID_Pin *pin=NULL,const char *csPinCode="");

 	/**
	 * Write a file to the card.
	 * Throw PTEID_ExCmdNotAllowed exception you're not allowed to write the file.
	 *
	 * If no pin is passed and a pin is needed by the card, the pin code will be asked anyway.
	 *
	 * @param fileID is the path of the file
	 * @param out contents the bytes to write
	 * @param pin is the pin to ask for writing
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 */
	PTEIDSDK_API virtual bool writeFile(const char *fileID,const PTEID_ByteArray &out,PTEID_Pin *pin=NULL,const char *csPinCode="");

 	/**
	 * Return the number of pin on the card.
	 */
	PTEIDSDK_API virtual unsigned long pinCount();

 	/**
	 * Return an object to access all the pins on the card.
	 */
	PTEIDSDK_API virtual PTEID_Pins& getPins();

 	/**
	 * Return the number of certificate on the card.
	 */
	PTEIDSDK_API virtual unsigned long certificateCount();

	/**
	 * Return an object to access all the certificates on the card.
	 */
	PTEIDSDK_API virtual PTEID_Certificates& getCertificates();

	/**
	  * Return the challenge.
	  *
	  * @param bForceNewInit force a new initialization of the couple challenge/response
	  */
	PTEIDSDK_API virtual const PTEID_ByteArray &getChallenge(bool bForceNewInit = false);

	/**
	  * Return the response to the challenge.
	  */
	PTEIDSDK_API virtual const PTEID_ByteArray &getChallengeResponse();

	/**
	  * Return true if the response of the card to the given challenge is the same as the response expected.
	  * For virtual card (from file), always return false.
	  *
	  * @param challenge is the challenge to check
	  * @param response is the response expected from the card
	  */
	PTEIDSDK_API virtual bool verifyChallengeResponse(const PTEID_ByteArray &challenge, const PTEID_ByteArray &response) const;

protected:
	PTEID_SmartCard(const SDK_Context *context,APL_Card *impl);	/**< For internal use : Constructor */

private:
	PTEID_SmartCard(const PTEID_SmartCard& card);						/**< Copy not allowed - not implemented */
	PTEID_SmartCard& operator= (const PTEID_SmartCard& card);			/**< Copy not allowed - not implemented */
};

class PTEID_EIdFullDoc;
class PTEID_EId;
class PTEID_Address;
class PTEID_Sod;
class PTEID_CardVersionInfo;
class PTEID_Certificate;
class APL_EIDCard;
class PTEID_XmlUserRequestedInfo;
class PTEID_CCXML_Doc;

/******************************************************************************//**
  * This class represents a Portugal EID card.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class PTEID_EIDCard : public PTEID_SmartCard
{
public:
	PTEIDSDK_API virtual ~PTEID_EIDCard();						/**< Destructor */

 	/**
	  * Return true if the user allow the application.
	  */
	PTEIDSDK_API static bool isApplicationAllowed();

 	/**
	  * Return true this is a test card.
	  */
	PTEIDSDK_API virtual bool isTestCard();

	/**
	  * Return true if test card are allowed.
	  */
	PTEIDSDK_API virtual bool getAllowTestCard();

	/**
	  * Set the flag to allow the test cards.
	  */
	PTEIDSDK_API virtual void setAllowTestCard(bool allow);

	/**
	 * Return a document from the card.
	 * Throw PTEID_ExDocTypeUnknown exception if the document doesn't exist for this card.
	 */
	PTEIDSDK_API virtual PTEID_XMLDoc& getDocument(PTEID_DocumentType type);

	PTEIDSDK_API PTEID_EIdFullDoc& getFullDoc();				/**< Get the full document */
	PTEIDSDK_API PTEID_CCXML_Doc& getXmlCCDoc(PTEID_XmlUserRequestedInfo& userRequestedInfo);
	PTEIDSDK_API PTEID_EId& getID();							/**< Get the id document */
	PTEIDSDK_API PTEID_Address& getAddr();					/**< Get the Address document */
	PTEIDSDK_API PTEID_Sod& getSod();							/**< Get the sod document */
	PTEIDSDK_API PTEID_CardVersionInfo& getVersionInfo();		/**< Get the info document  */

	PTEIDSDK_API PTEID_Certificate &getCert(PTEID_CertifType type);/**< Return certificate by type from the card */
	PTEIDSDK_API PTEID_Certificate &getRoot();				/**< Return the root certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getCA();					/**< Return the ca certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getSignature();			/**< Return the signature certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getAuthentication();		/**< Return the authentication certificate from the card */

	/**
	 * Return a raw data from the card.
	 * Throw PTEID_ExFileTypeUnknown exception if the document doesn't exist for this card.
	 */
	PTEIDSDK_API virtual const PTEID_ByteArray& getRawData(PTEID_RawDataType type);

 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Id();				/**< Get the Id RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_IdSig();			/**< Get the IdSig RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Trace();			/**< Get the Trace RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Addr();			/**< Get the Addr RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_AddrSig();			/**< Get the AddrSig RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Sod();				/**< Get the Sod RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_CardInfo();		/**< Get the Card Info RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_TokenInfo();		/**< Get the Token Info RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Challenge();		/**< Get the challenge RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_Response();		/**< Get the response RawData */
 	PTEIDSDK_API const PTEID_ByteArray& getRawData_PersoData();		/**< Get the response RawData */

protected:
	PTEID_EIDCard(const SDK_Context *context,APL_Card *impl);		/**< For internal use : Constructor */

private:
	PTEID_EIDCard(const PTEID_EIDCard& card);						/**< Copy not allowed - not implemented */
	PTEID_EIDCard& operator= (const PTEID_EIDCard& card);			/**< Copy not allowed - not implemented */

friend PTEID_Card &PTEID_ReaderContext::getCard();				/**< For internal use : This method must access protected constructor */
};

class APL_XmlUserRequestedInfo;
/******************************************************************************//**
  * This class will contain information about the data to be returned in the xml file.
  *********************************************************************************/
class PTEID_XmlUserRequestedInfo : public PTEID_Object
{
public:
    PTEIDSDK_API PTEID_XmlUserRequestedInfo();
    PTEIDSDK_API PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress);
	PTEIDSDK_API virtual ~PTEID_XmlUserRequestedInfo();			/**< Destructor */
	PTEIDSDK_API void add(XMLUserData xmlUData);		/**< add string */

protected:
	APL_XmlUserRequestedInfo *customXml;

private:
	PTEID_XmlUserRequestedInfo(const PTEID_XmlUserRequestedInfo& info); /**< Copy not allowed - not implemented */
	PTEID_XmlUserRequestedInfo& operator= (const PTEID_XmlUserRequestedInfo& xmlUserRequestedInfo);	/**< Copy not allowed - not implemented */
friend PTEID_CCXML_Doc& PTEID_EIDCard::getXmlCCDoc(PTEID_XmlUserRequestedInfo& userRequestedInfo);
};

class APL_XMLDoc;

/******************************************************************************//**
  * Abstract base class for all the documents.
  *********************************************************************************/
class PTEID_XMLDoc : public PTEID_Object
{
public:
	PTEIDSDK_API virtual ~PTEID_XMLDoc()=0;				/**< Destructor */

	PTEIDSDK_API virtual bool isAllowed();						/**< The document is allowed */

	PTEIDSDK_API virtual PTEID_ByteArray getXML();			/**< Return the document in an XML format */
	PTEIDSDK_API virtual PTEID_ByteArray getCSV();			/**< Return the document in an CSV format */
	PTEIDSDK_API virtual PTEID_ByteArray getTLV();			/**< Return the document in an TLV format */

	/**
	  * Write the xml document into the file csFilePath.
	  * @return true if succeeded
	  */
	PTEIDSDK_API virtual bool writeXmlToFile(const char * csFilePath);

	/**
	  * Write the csv document into the file csFilePath.
	  * @return true if succeeded
	  */
	PTEIDSDK_API virtual bool writeCsvToFile(const char * csFilePath);

	/**
	  * Write the tlv document into the file csFilePath.
	  * @return true if succeeded
	  */
	PTEIDSDK_API virtual bool writeTlvToFile(const char * csFilePath);

protected:
	PTEID_XMLDoc(const SDK_Context *context,APL_XMLDoc *impl);		/**< For internal use : Constructor */

private:
	PTEID_XMLDoc(const PTEID_XMLDoc& doc);					/**< Copy not allowed - not implemented */
	PTEID_XMLDoc& operator= (const PTEID_XMLDoc& doc);		/**< Copy not allowed - not implemented */

};

class APL_Biometric;

/******************************************************************************//**
  * Abstract base class for the biometric documents.
  *********************************************************************************/
class PTEID_Biometric : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_Biometric()=0;				/**< Destructor */

protected:
	PTEID_Biometric(const SDK_Context *context,APL_Biometric *impl);	/**< For internal use : Constructor */

private:
	PTEID_Biometric(const PTEID_Biometric& doc);				/**< Copy not allowed - not implemented */
	PTEID_Biometric& operator= (const PTEID_Biometric& doc);	/**< Copy not allowed - not implemented */
};

class APL_Crypto;

/******************************************************************************//**
  * Abstract base class for the cryptographic documents.
  *********************************************************************************/
class PTEID_Crypto : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_Crypto()=0;			/**< Destructor */

protected:
	PTEID_Crypto(const SDK_Context *context,APL_Crypto *impl);	/**< For internal use : Constructor */

private:
	PTEID_Crypto(const PTEID_Crypto& doc);				/**< Copy not allowed - not implemented */
	PTEID_Crypto& operator= (const PTEID_Crypto& doc);	/**< Copy not allowed - not implemented */
};

class APL_DocVersionInfo;

/******************************************************************************//**
  * Class for the info document.
  * You can get such an object from PTEID_EIDCard::getVersionInfo() (or getDocument).
  *********************************************************************************/
class PTEID_CardVersionInfo : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API  virtual ~PTEID_CardVersionInfo();		/**< Destructor */

	PTEIDSDK_API bool isActive();						/**< Returns the card status (true = active) */
	PTEIDSDK_API const char *getSerialNumber();			/**< Return the Serial Number of the card */
    PTEIDSDK_API const char *getTokenLabel();			/**< Return the Token Label (EFCIA 5032) */
    PTEIDSDK_API const char *getComponentCode();			/**< Return the ComponenCode of the card  */
	PTEIDSDK_API const char *getOsNumber();				/**< Return the OS Number of the card */
	PTEIDSDK_API const char *getOsVersion();				/**< Return the OS Version of the card */
	PTEIDSDK_API const char *getSoftmaskNumber();			/**< Return the Softmask Number of the card */
	PTEIDSDK_API const char *getSoftmaskVersion();		/**< Return the Softmask Version of the card */
	PTEIDSDK_API const char *getAppletVersion();			/**< Return the Applet Version of the card */
	PTEIDSDK_API const char *getGlobalOsVersion();		/**< Return the Global Os Version of the card */
	PTEIDSDK_API const char *getAppletInterfaceVersion();	/**< Return the Applet Interface Version of the card */
	PTEIDSDK_API const char *getPKCS1Support();			/**< Return the PKCS#1 Support of the card */
	PTEIDSDK_API const char *getKeyExchangeVersion();		/**< Return the Key Exchange Version of the card */
	PTEIDSDK_API const char *getAppletLifeCycle();		/**< Return the Applet Life Cycle of the card */
	PTEIDSDK_API const char *getGraphicalPersonalisation();			/**< Return field GraphicalPersonalisation from the Info file */
	PTEIDSDK_API const char *getElectricalPersonalisation();			/**< Return field ElectricalPersonalisation from the TokenInfo file */
	PTEIDSDK_API const char *getElectricalPersonalisationInterface();	/**< Return field ElectricalPersonalisationInterface from the TokenInfo file */
	PTEIDSDK_API const PTEID_ByteArray &getSignature();		/**< Return the signature of the card info */

private:
	PTEID_CardVersionInfo(const PTEID_CardVersionInfo& doc);				/**< Copy not allowed - not implemented */
	PTEID_CardVersionInfo& operator= (const PTEID_CardVersionInfo& doc);	/**< Copy not allowed - not implemented */

	PTEID_CardVersionInfo(const SDK_Context *context,APL_DocVersionInfo *impl);	/**< For internal use : Constructor */

friend PTEID_CardVersionInfo& PTEID_EIDCard::getVersionInfo();	/**< For internal use : This method must access protected constructor */
};

class APL_SodEid;

/******************************************************************************//**
  * Class for the sod document on a EID Card.
  * You can get such an object from PTEID_EIDCard::getSod()	(or getDocument).
  *********************************************************************************/
class PTEID_Sod : public PTEID_Biometric
{
public:
	PTEIDSDK_API virtual ~PTEID_Sod();				/**< Destructor */

	PTEIDSDK_API const PTEID_ByteArray& getData();		/**< Return the sod itself */
	PTEIDSDK_API const PTEID_ByteArray& getHash();		/**< Return the hash of the sod */

private:
	PTEID_Sod(const PTEID_Sod& doc);				/**< Copy not allowed - not implemented */
	PTEID_Sod& operator= (const PTEID_Sod& doc);	/**< Copy not allowed - not implemented */

	PTEID_Sod(const SDK_Context *context,APL_SodEid *impl);/**< For internal use : Constructor */

friend PTEID_Sod& PTEID_EIDCard::getSod();		/**< For internal use : This method must access protected constructor */
};

class APL_DocEId;

/******************************************************************************//**
  * Class for the id document on a EID Card.
  * You can get such an object from PTEID_EIDCard::getID()	(or getDocument).
  *********************************************************************************/
class PTEID_EId : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_EId();						/**< Destructor */

	PTEIDSDK_API const char *getDocumentVersion();			/**< Return Document Version field */
	PTEIDSDK_API const char *getDocumentType();				/**< Return Document Type field */
	PTEIDSDK_API const char *getCountry();					/**< Return Country field */
	PTEIDSDK_API const char *getGivenName();				/**< Return GivenName (2 first given name) */
	PTEIDSDK_API const char *getSurname();					/**< Return Surname field */
	PTEIDSDK_API const char *getGender();					/**< Return Gender field */
	PTEIDSDK_API const char *getDateOfBirth();				/**< Return Date Of Birth field */
	PTEIDSDK_API const char *getLocationOfBirth();			/**< Return Location Of Birth field */
	PTEIDSDK_API const char *getNationality();				/**< Return Nationality field */
	PTEIDSDK_API const char *getDuplicata();				/**< Return Duplicata field */
	PTEIDSDK_API const char *getSpecialOrganization();		/**< Return Special Organization field */
	PTEIDSDK_API const char *getMemberOfFamily();			/**< Return Member Of Family field */
	PTEIDSDK_API const char *getLogicalNumber();			/**< Return Logical Number field */
	PTEIDSDK_API const char *getDocumentPAN();				/**< Return Document PAN field */
	PTEIDSDK_API const char *getValidityBeginDate();		/**< Return Validity Begin Date field */
	PTEIDSDK_API const char *getValidityEndDate();			/**< Return Validity End Date field */
	PTEIDSDK_API const char *getSpecialStatus();			/**< Return Special Status field */
	PTEIDSDK_API const char *getHeight();					/**< Return field Height */
	PTEIDSDK_API const char *getDocumentNumber();			/**< Return field DocumentNumber */
	PTEIDSDK_API const char *getCivilianIdNumber();			/**< Return field CivlianIdNumber */
	PTEIDSDK_API const char *getTaxNo();					/**< Return field TaxNo */
	PTEIDSDK_API const char *getSocialSecurityNumber();		/**< Return field SocialSecurityNumber */
	PTEIDSDK_API const char *getHealthNumber();				/**< Return field HealthNo */
	PTEIDSDK_API const char *getIssuingEntity();			/**< Return field IssuingEntity */
	PTEIDSDK_API const char *getLocalofRequest();			/**< Return field LocalofRequest*/
	PTEIDSDK_API const char *getGivenNameFather();			/**< Return field GivenNameFather */
	PTEIDSDK_API const char *getSurnameFather();			/**< Return field SurnameFather */
	PTEIDSDK_API const char *getGivenNameMother();			/**< Return field GivenNameMother */
	PTEIDSDK_API const char *getSurnameMother();			/**< Return field SurnameMother */
	PTEIDSDK_API const char *getParents();					/**< Return field Parents */
	PTEIDSDK_API PTEID_Photo& getPhotoObj();				/**< Return object Photo */
	PTEIDSDK_API const char *getPersoData();				/**< Return field PersoData */
	PTEIDSDK_API const char *getValidation();				/**< Return field Validation */
	PTEIDSDK_API const char *getMRZ1();						/**< Return field MRZ block 1 */
	PTEIDSDK_API const char *getMRZ2();						/**< Return field MRZ block 2 */
	PTEIDSDK_API const char *getMRZ3();					 	/**< Return field MRZ block 3 */
	PTEIDSDK_API const char *getAccidentalIndications(); 	/**< Return field AccidentalIndications */

private:
	PTEID_EId(const PTEID_EId& doc);						/**< Copy not allowed - not implemented */
	PTEID_EId& operator= (const PTEID_EId& doc);			/**< Copy not allowed - not implemented */

	PTEID_EId(const SDK_Context *context,APL_DocEId *impl);	/**< For internal use : Constructor */

friend PTEID_EId& PTEID_EIDCard::getID();					/**< For internal use : This method must access protected constructor */
};

class APL_AddrEId;

/******************************************************************************//**
  * Class for the Address document on a EID Card.
  * You can get such an object from PTEID_EIDCard::getAddr()	(or getDocument).
  *********************************************************************************/
class PTEID_Address : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_Address();							/**< Destructor */

	//PTEIDSDK_API const char *getAddressVersion();					/**< Return Address Version field */
	//PTEIDSDK_API const char *getStreet();							/**< Return Street field */
	//PTEIDSDK_API const char *getZipCode();						/**< Return Zip Code field */
	PTEIDSDK_API	bool isNationalAddress();						/**< is the address a portuguese address? */
	PTEIDSDK_API	const char *getCountryCode();						/**<residence country */

	PTEIDSDK_API	const char *getDistrict();						/**< Return field District */
	PTEIDSDK_API	const char *getDistrictCode();					/**< Return field District Code*/
	PTEIDSDK_API 	const char *getMunicipality();					/**< Return Municipality field */
	PTEIDSDK_API 	const char *getMunicipalityCode();				/**< Return Municipality Code field */
	PTEIDSDK_API	const char *getCivilParish();					/**< Return field CivilParish */
	PTEIDSDK_API	const char *getCivilParishCode();				/**< Return field CivilParish Code */
	PTEIDSDK_API	const char *getAbbrStreetType();				/**< Return field AbbrStreetType */
	PTEIDSDK_API	const char *getStreetType();					/**< Return field StreetType */
	PTEIDSDK_API	const char *getStreetName();					/**< Return field StreetName */
	PTEIDSDK_API	const char *getAbbrBuildingType();				/**< Return field AbbrBuildingType */
	PTEIDSDK_API	const char *getBuildingType();					/**< Return field BuildingType */
	PTEIDSDK_API	const char *getDoorNo();						/**< Return field DoorNo */
	PTEIDSDK_API	const char *getFloor();							/**< Return field Floor */
	PTEIDSDK_API	const char *getSide();							/**< Return field Side */
	PTEIDSDK_API	const char *getLocality();						/**< Return field Locality */
	PTEIDSDK_API	const char *getPlace();							/**< Return field Locality */
	PTEIDSDK_API	const char *getZip4();							/**< Return field Zip4 */
	PTEIDSDK_API	const char *getZip3();							/**< Return field Zip3 */
	PTEIDSDK_API	const char *getPostalLocality();				/**< Return field PostalLocality */
	PTEIDSDK_API	const char *getGeneratedAddressCode();			/**< Return field Address Code */

	PTEIDSDK_API 	const char *getForeignCountry();
	PTEIDSDK_API 	const char *getForeignAddress();
	PTEIDSDK_API 	const char *getForeignCity();
	PTEIDSDK_API 	const char *getForeignRegion();
	PTEIDSDK_API 	const char *getForeignLocality();
	PTEIDSDK_API 	const char *getForeignPostalCode();

private:
	PTEID_Address(const PTEID_Address& doc);							/**< Copy not allowed - not implemented */
	PTEID_Address& operator= (const PTEID_Address& doc);				/**< Copy not allowed - not implemented */

	PTEID_Address(const SDK_Context *context,APL_AddrEId *impl);			/**< For internal use : Constructor */

friend PTEID_Address& PTEID_EIDCard::getAddr();						/**< For internal use : This method must access protected constructor */
};

class APL_EIdFullDoc;

/******************************************************************************//**
  * Class for the full document Eid.
  *********************************************************************************/
class PTEID_EIdFullDoc : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_EIdFullDoc();					/**< Destructor */

protected:
	PTEID_EIdFullDoc(const SDK_Context *context,APL_EIdFullDoc *impl);	/**< For internal use : Constructor */

private:
	PTEID_EIdFullDoc(const PTEID_EIdFullDoc& doc);				/**< Copy not allowed - not implemented */
	PTEID_EIdFullDoc& operator= (const PTEID_EIdFullDoc& doc);	/**< Copy not allowed - not implemented */

friend PTEID_EIdFullDoc& PTEID_EIDCard::getFullDoc();				/**< For internal use : This method must access protected constructor */
};

class APL_CCXML_Doc;

class PTEID_CCXML_Doc : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API virtual ~PTEID_CCXML_Doc();					/**< Destructor */
	PTEIDSDK_API const char *getCCXML();

protected:
	PTEID_CCXML_Doc(const SDK_Context *context,APL_CCXML_Doc *impl);	/**< For internal use : Constructor */

private:
	PTEID_CCXML_Doc(const PTEID_CCXML_Doc& doc);				/**< Copy not allowed - not implemented */
	PTEID_CCXML_Doc& operator= (const PTEID_CCXML_Doc& doc);	/**< Copy not allowed - not implemented */
	std::string *xmltemp;
	friend PTEID_CCXML_Doc& PTEID_EIDCard::getXmlCCDoc(PTEID_XmlUserRequestedInfo& userRequestedInfo);			/**< For internal use : This method must access protected constructor */
};


class PTEID_Pin;
class APL_Pins;

/******************************************************************************//**
  * Container class for all pins on the card.
  *********************************************************************************/
class PTEID_Pins : public PTEID_Crypto
{
public:
	PTEIDSDK_API virtual ~PTEID_Pins();				/**< Destructor */

	PTEIDSDK_API unsigned long count();						/**< The number of pins on the card */

	/**
	  * Get the pin by its Index.
	  * Throw PTEID_ExParamRange exception if the index is out of range.
 	  */
	PTEIDSDK_API PTEID_Pin &getPinByNumber(unsigned long ulIndex);

private:
	PTEID_Pins(const PTEID_Pins& pins);					/**< Copy not allowed - not implemented */
	PTEID_Pins& operator= (const PTEID_Pins& pins);		/**< Copy not allowed - not implemented */

	PTEID_Pins(const SDK_Context *context,APL_Pins *impl);		/**< For internal use : Constructor */

friend PTEID_Pins& PTEID_SmartCard::getPins();			/**< For internal use : This method must access protected constructor */
};

class APL_Pin;

/******************************************************************************//**
  * Class that represent one Pin.
  *********************************************************************************/
class PTEID_Pin : public PTEID_Crypto
{
public:
	PTEIDSDK_API virtual ~PTEID_Pin();				/**< Destructor */

	PTEIDSDK_API unsigned long getIndex();		/**< Get the index of the pin */
	PTEIDSDK_API unsigned long getType();			/**< Get the type of the pin */
	PTEIDSDK_API unsigned long getId();			/**< Get the id of the pin */
	PTEIDSDK_API PTEID_PinUsage getUsageCode();	/**< Get the usage code of the pin */
	PTEIDSDK_API unsigned long getFlags();		/**< Get the flags of the pin */
	PTEIDSDK_API const char *getLabel();			/**< Get the label of the pin */

	PTEIDSDK_API const PTEID_ByteArray &getSignature();	/**< Return the signature of the pin */

	/**
	  * Return the remaining tries for giving the good pin.
	  *
	  * This opperation is not supported by all card.
	  *
	  * @return -1 if not supported
	  * @return the number of remaining tries in the other case
	  */
	PTEIDSDK_API long getTriesLeft();

	/**
	  * Ask the card to verify the pin.
	  * A popup will ask for the code.
	  * @return true if success and false if failed
	  */
	PTEIDSDK_API bool verifyPin();

	/**
	  * Ask the card to verify the pin.
	  *
	  * @param csPin is the pin code to verify (if csPin is empty, a popup will ask for the code)
	  * @param ulRemaining return the remaining tries (only if verifying failed)
	  *
	  * @return true if success and false if failed
	  */
	PTEIDSDK_API bool verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg=true);

	/**
	  * Ask the card to change the pin.
	  * A popup will ask for the codes
	  * @return true if success and false if failed
	  */
	PTEIDSDK_API bool changePin();

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
	PTEIDSDK_API bool changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg=true);

private:
	PTEID_Pin(const PTEID_Pin& pin);									/**< Copy not allowed - not implemented */
	PTEID_Pin& operator= (const PTEID_Pin& pin);						/**< Copy not allowed - not implemented */

	PTEID_Pin(const SDK_Context *context,APL_Pin *impl);						/**< For internal use : Constructor */

friend PTEID_Pin &PTEID_Pins::getPinByNumber(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
};

class APL_Certifs;

/******************************************************************************//**
  * Container class for all certificates on the card.
  *********************************************************************************/
class PTEID_Certificates : public PTEID_Crypto
{
public:
	/**
	  * Create an PTEID_Certificates store without any link to a card.
	  * This store is not link to any Card, so some methods could not be used.
	  * These methods throw PTEID_ExBadUsage exception.
	  */
	PTEIDSDK_API PTEID_Certificates();

	PTEIDSDK_API virtual ~PTEID_Certificates();					/**< Destructor */

	PTEIDSDK_API unsigned long countFromCard();					/**< The number of certificates on the card */
	PTEIDSDK_API unsigned long countAll();						/**< The number of certificates (on the card or not) */

	/**
	  * Get the ulIndex certificate from the card.
	  * Throw PTEID_ExParamRange exception if the index is out of range.
	  */
	PTEIDSDK_API PTEID_Certificate &getCertFromCard(unsigned long ulIndexCard);

	/**
	  * Return the certificate with the number ulIndexAll.
	  *
	  * ATTENTION ulIndexAll and ulIndexCard are two different index.
	  * Index will change if new certificates are added with addCert().
	  */
	PTEIDSDK_API PTEID_Certificate &getCert(unsigned long ulIndexAll);

	PTEIDSDK_API const char *getExternalCertData(int cert);
	PTEIDSDK_API int getExternalCertDataSize(int cert);
	PTEIDSDK_API const char *getExternalCertSubject(int cert);
	PTEIDSDK_API const char *getExternalCertIssuer(int cert);
	PTEIDSDK_API const char *getExternalCertNotBefore(int cert);
	PTEIDSDK_API const char *getExternalCertNotAfter(int cert);
	PTEIDSDK_API unsigned long getExternalCertKeylenght(int cert);

	/**
	  * Return the certificate by type.
	  */
	PTEIDSDK_API PTEID_Certificate &getCert(PTEID_CertifType type);

	PTEIDSDK_API PTEID_Certificate &getRoot();					/**< Return the root certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getCA();						/**< Return the ca certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getSignature();				/**< Return the signature certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getAuthentication();			/**< Return the authentication certificate from the card */

	/**
	  * Add a new certificate to the store.
	  */
	PTEIDSDK_API PTEID_Certificate &addCertificate(PTEID_ByteArray &cert);


private:
	PTEID_Certificates(const PTEID_Certificates& certifs);			/**< Copy not allowed - not implemented */
	PTEID_Certificates& operator= (const PTEID_Certificates& certifs);/**< Copy not allowed - not implemented */

	PTEID_Certificates(const SDK_Context *context,APL_Certifs *impl);/**< For internal use : Constructor */

friend PTEID_Certificates& PTEID_SmartCard::getCertificates();		/**< For internal use : This method must access protected constructor */
};

class APL_Certif;

/******************************************************************************//**
  * Class that represent one certificate.
  *********************************************************************************/
class PTEID_Certificate : public PTEID_Crypto
{
public:
	PTEIDSDK_API virtual ~PTEID_Certificate();			/**< Destructor */

	PTEIDSDK_API const char *getLabel();				/**< Return the label of the certificate */
	PTEIDSDK_API unsigned long getID();				/**< Return the id of the certificate */

	PTEIDSDK_API PTEID_CertifType getType();			/**< Return the type of the certificate */

	PTEIDSDK_API const PTEID_ByteArray &getCertData();/**< Return the content of the certificate */
	PTEIDSDK_API void getFormattedData(PTEID_ByteArray &data);	/**< Return the content of the certificate without ending zero */
	PTEIDSDK_API const char *getSerialNumber();		/**< Return the serial number of the certificate */
	PTEIDSDK_API const char *getOwnerName();			/**< Return the name of the owner of the certificate */
	PTEIDSDK_API const char *getIssuerName();			/**< Return the name of the issuer of the certificate */
	PTEIDSDK_API const char *getValidityBegin();		/**< Return the validity begin date of the certificate */
	PTEIDSDK_API const char *getValidityEnd();		/**< Return the validity end date of the certificate */
	PTEIDSDK_API unsigned long getKeyLength();		/**< Return the length of public/private key on the certificate */

	/**
	  * Return true if this is a root certificate.
	  */
	PTEIDSDK_API bool isRoot();

	/**
	  * Return the test status.
	  *
	  * @return true if test certificate
	  * @return false if good one
	  */
	PTEIDSDK_API bool isTest();

	/**
	  * Return true if the certificate chain end by the one of pteid root.
	  */
	PTEIDSDK_API bool isFromPteidValidChain();

	/**
	  * This certificate comes from the card.
	  */
	PTEIDSDK_API bool isFromCard();

	/**
	  * Return the issuer certificate.
	  *
	  * if there is no issuer (root), PTEID_ExCertNoIssuer exception is thrown
	  */
	PTEIDSDK_API PTEID_Certificate &getIssuer();

	/**
	  * Return the number of children for this certificate.
	  */
	PTEIDSDK_API unsigned long countChildren();

	/**
	  * Return the children certificate (certificate that has been issued by this one).
	  *
	  * @param ulIndex is the children index (the index for the first child is 0)
	  * Throw PTEID_ExParamRange exception if the index is out of range
	  */
	PTEIDSDK_API PTEID_Certificate &getChildren(unsigned long ulIndex);

private:
	PTEID_Certificate(const PTEID_Certificate& certif);				/**< Copy not allowed - not implemented */
	PTEID_Certificate& operator= (const PTEID_Certificate& certif);	/**< Copy not allowed - not implemented */

	PTEID_Certificate(const SDK_Context *context,APL_Certif *impl);	/**< For internal use : Constructor */

friend PTEID_Certificate &PTEID_Certificates::getCert(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
friend PTEID_Certificate &PTEID_Certificates::getCertFromCard(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
friend PTEID_Certificate &PTEID_Certificates::getCert(PTEID_CertifType type);			/**< For internal use : This method must access protected constructor */
friend PTEID_Certificate &PTEID_Certificates::addCertificate(PTEID_ByteArray &cert);	/**< For internal use : This method must access protected constructor */
};

class APL_Config;

/******************************************************************************//**
  * Class to access the config parameters.
  *********************************************************************************/
class PTEID_Config : public PTEID_Object
{
public:
	/**
	  * Create object to access parameter Param.
	  */
    PTEIDSDK_API PTEID_Config(PTEID_Param Param);

 	/**
	  * Create object to access a string parameter.
	  */
    PTEIDSDK_API PTEID_Config(const char *csName, const char *czSection, const char *csDefaultValue);

	/**
	  * Create object to access a numerical parameter.
	  */
    PTEIDSDK_API PTEID_Config(const char *csName, const char *czSection, long lDefaultValue);

	PTEIDSDK_API virtual ~PTEID_Config();			/**< Destructor */

    PTEIDSDK_API const char *getString();		/**< Return the string value (Throw exception for numerical parameter) */
    PTEIDSDK_API long getLong();					/**< Return the numerical value (Throw exception for string parameter) */

    PTEIDSDK_API void setString(const char *csValue);	/**< Set the string value (Throw exception for numerical parameter) */
	PTEIDSDK_API void setLong(long lValue);				/**< Set the numerical value (Throw exception for string parameter) */

private:
	PTEID_Config(const PTEID_Config& config);				/**< Copy not allowed - not implemented */
	PTEID_Config& operator= (const PTEID_Config& config);	/**< Copy not allowed - not implemented */

	PTEID_Config(APL_Config *impl);	/**< For internal use : Constructor */
};

/******************************************************************************//**
  * Function for Logging.
  *********************************************************************************/
PTEIDSDK_API void PTEID_LOG(PTEID_LogLevel level, const char *module_name, const char *format, ...);


/******************************************************************************//**
  * Compatibility layer
  *********************************************************************************/
#define COMP_LAYER_NATIONAL_ADDRESS "N"
#define COMP_LAYER_FOREIGN_ADDRESS "I"

#define PTEID_DELIVERY_ENTITY_LEN               40
#define PTEID_COUNTRY_LEN                       80
#define PTEID_DOCUMENT_TYPE_LEN                 34
#define PTEID_CARDNUMBER_LEN                    28
#define PTEID_CARDNUMBER_PAN_LEN                32
#define PTEID_CARDVERSION_LEN                   16
#define PTEID_DATE_LEN                          20
#define PTEID_LOCALE_LEN                        60
#define PTEID_NAME_LEN                          120
#define PTEID_SEX_LEN                           2
#define PTEID_NATIONALITY_LEN                   6
#define PTEID_HEIGHT_LEN                        8
#define PTEID_NUMBI_LEN                         18
#define PTEID_NUMNIF_LEN                        18
#define PTEID_NUMSS_LEN                         22
#define PTEID_NUMSNS_LEN                        18
#define PTEID_INDICATIONEV_LEN                  120
#define PTEID_MRZ_LEN							30

#define PTEID_MAX_DELIVERY_ENTITY_LEN               PTEID_DELIVERY_ENTITY_LEN+2
#define PTEID_MAX_COUNTRY_LEN                       PTEID_COUNTRY_LEN+2
#define PTEID_MAX_DOCUMENT_TYPE_LEN                 PTEID_DOCUMENT_TYPE_LEN+2
#define PTEID_MAX_CARDNUMBER_LEN                    PTEID_CARDNUMBER_LEN+2
#define PTEID_MAX_CARDNUMBER_PAN_LEN                PTEID_CARDNUMBER_PAN_LEN+2
#define PTEID_MAX_CARDVERSION_LEN                   PTEID_CARDVERSION_LEN+2
#define PTEID_MAX_DATE_LEN                          PTEID_DATE_LEN+2
#define PTEID_MAX_LOCALE_LEN                        PTEID_LOCALE_LEN+2
#define PTEID_MAX_NAME_LEN                          PTEID_NAME_LEN+2
#define PTEID_MAX_SEX_LEN                           PTEID_SEX_LEN+2
#define PTEID_MAX_NATIONALITY_LEN                   PTEID_NATIONALITY_LEN+2
#define PTEID_MAX_HEIGHT_LEN                        PTEID_HEIGHT_LEN+2
#define PTEID_MAX_NUMBI_LEN                         PTEID_NUMBI_LEN+2
#define PTEID_MAX_NUMNIF_LEN                        PTEID_NUMNIF_LEN+2
#define PTEID_MAX_NUMSS_LEN                         PTEID_NUMSS_LEN+2
#define PTEID_MAX_NUMSNS_LEN                        PTEID_NUMSNS_LEN+2
#define PTEID_MAX_INDICATIONEV_LEN                  PTEID_INDICATIONEV_LEN+2
#define PTEID_MAX_MRZ_LEN                           PTEID_MRZ_LEN+2

#define PTEID_ADDR_TYPE_LEN                     2
#define PTEID_ADDR_COUNTRY_LEN                  4
#define PTEID_DISTRICT_LEN                      4
#define PTEID_DISTRICT_DESC_LEN                 100
#define PTEID_DISTRICT_CON_LEN                  8
#define PTEID_DISTRICT_CON_DESC_LEN             100
#define PTEID_DISTRICT_FREG_LEN                 12
#define PTEID_DISTRICT_FREG_DESC_LEN            100
#define PTEID_ROAD_ABBR_LEN                     20
#define PTEID_ROAD_LEN                          100
#define PTEID_ROAD_DESIG_LEN                    200
#define PTEID_HOUSE_ABBR_LEN                    20
#define PTEID_HOUSE_LEN                         100
#define PTEID_NUMDOOR_LEN                       20
#define PTEID_FLOOR_LEN                         40
#define PTEID_SIDE_LEN                          40
#define PTEID_PLACE_LEN                         100
#define PTEID_LOCALITY_LEN                      100
#define PTEID_CP4_LEN                           8
#define PTEID_CP3_LEN                           6
#define PTEID_POSTAL_LEN                        50
#define PTEID_NUMMOR_LEN                        12
#define PTEID_ADDR_COUNTRYF_DESC_LEN            100
#define PTEID_ADDRF_LEN                         300
#define PTEID_CITYF_LEN                         100
#define PTEID_REGIOF_LEN                        100
#define PTEID_LOCALITYF_LEN                     100
#define PTEID_POSTALF_LEN                       100
#define PTEID_NUMMORF_LEN                       12

#define PTEID_MAX_ADDR_TYPE_LEN                     PTEID_ADDR_TYPE_LEN+2
#define PTEID_MAX_ADDR_COUNTRY_LEN                  PTEID_ADDR_COUNTRY_LEN+2
#define PTEID_MAX_DISTRICT_LEN                      PTEID_DISTRICT_LEN+2
#define PTEID_MAX_DISTRICT_DESC_LEN                 PTEID_DISTRICT_DESC_LEN+2
#define PTEID_MAX_DISTRICT_CON_LEN                  PTEID_DISTRICT_CON_LEN+2
#define PTEID_MAX_DISTRICT_CON_DESC_LEN             PTEID_DISTRICT_CON_DESC_LEN+2
#define PTEID_MAX_DISTRICT_FREG_LEN                 PTEID_DISTRICT_FREG_LEN+2
#define PTEID_MAX_DISTRICT_FREG_DESC_LEN            PTEID_DISTRICT_FREG_DESC_LEN+2
#define PTEID_MAX_ROAD_ABBR_LEN                     PTEID_ROAD_ABBR_LEN+2
#define PTEID_MAX_ROAD_LEN                          PTEID_ROAD_LEN+2
#define PTEID_MAX_ROAD_DESIG_LEN                    PTEID_ROAD_DESIG_LEN+2
#define PTEID_MAX_HOUSE_ABBR_LEN                    PTEID_HOUSE_ABBR_LEN+2
#define PTEID_MAX_HOUSE_LEN                         PTEID_HOUSE_LEN+2
#define PTEID_MAX_NUMDOOR_LEN                       PTEID_NUMDOOR_LEN+2
#define PTEID_MAX_FLOOR_LEN                         PTEID_FLOOR_LEN+2
#define PTEID_MAX_SIDE_LEN                          PTEID_SIDE_LEN+2
#define PTEID_MAX_PLACE_LEN                         PTEID_PLACE_LEN+2
#define PTEID_MAX_LOCALITY_LEN                      PTEID_LOCALITY_LEN+2
#define PTEID_MAX_CP4_LEN                           PTEID_CP4_LEN+2
#define PTEID_MAX_CP3_LEN                           PTEID_CP3_LEN+2
#define PTEID_MAX_POSTAL_LEN                        PTEID_POSTAL_LEN+2
#define PTEID_MAX_NUMMOR_LEN                        PTEID_NUMMOR_LEN+2
#define PTEID_MAX_ADDR_COUNTRYF_DESC_LEN            PTEID_ADDR_COUNTRYF_DESC_LEN+2
#define PTEID_MAX_ADDRF_LEN                         PTEID_ADDRF_LEN+2
#define PTEID_MAX_CITYF_LEN                         PTEID_CITYF_LEN+2
#define PTEID_MAX_REGIOF_LEN                        PTEID_REGIOF_LEN+2
#define PTEID_MAX_LOCALITYF_LEN                     PTEID_LOCALITYF_LEN+2
#define PTEID_MAX_POSTALF_LEN                       PTEID_POSTALF_LEN+2
#define PTEID_MAX_NUMMORF_LEN                       PTEID_NUMMORF_LEN+2

#define PTEID_MAX_PICTURE_LEN			14128
#define PTEID_MAX_PICTURE_LEN_HEADER	111
#define PTEID_MAX_PICTUREH_LEN			(PTEID_MAX_PICTURE_LEN+PTEID_MAX_PICTURE_LEN_HEADER)
#define PTEID_MAX_CBEFF_LEN				34
#define PTEID_MAX_FACRECH_LEN			14
#define PTEID_MAX_FACINFO_LEN			20
#define PTEID_MAX_IMAGEINFO_LEN			12
#define PTEID_MAX_IMAGEHEADER_LEN		(PTEID_MAX_CBEFF_LEN+PTEID_MAX_FACRECH_LEN+PTEID_MAX_FACINFO_LEN+PTEID_MAX_IMAGEINFO_LEN)

#define PTEID_MAX_CERT_LEN				2500
#define PTEID_MAX_CERT_NUMBER			10
#define PTEID_MAX_CERT_LABEL_LEN		256

#define PTEID_MAX_PINS					8
#define PTEID_MAX_PIN_LABEL_LEN			256

#define PTEID_MAX_ID_NUMBER_LEN			64

#define PTEID_SOD_FILE					"3F005F00EF06"

#define PTEID_ACTIVE_CARD				1
#define PTEID_INACTIVE_CARD 			0

#define PTEID_ADDRESS_PIN_ID			131
#define PTEID_NO_PIN_NEEDED				0

#define MODE_ACTIVATE_BLOCK_PIN   1

typedef enum {
	COMP_CARD_TYPE_ERR = 0, // Something went wrong, or unknown card type
	COMP_CARD_TYPE_IAS07,   // IAS 0.7 card
	COMP_CARD_TYPE_IAS101,  // IAS 1.01 card
}tCompCardType;

typedef struct
{
    short version;
    char deliveryEntity[PTEID_MAX_DELIVERY_ENTITY_LEN];
    char country[PTEID_MAX_COUNTRY_LEN];
    char documentType[PTEID_MAX_DOCUMENT_TYPE_LEN];
    char cardNumber[PTEID_MAX_CARDNUMBER_LEN];
    char cardNumberPAN[PTEID_MAX_CARDNUMBER_PAN_LEN];
    char cardVersion[PTEID_MAX_CARDVERSION_LEN];
    char deliveryDate[PTEID_MAX_DATE_LEN];
    char locale[PTEID_MAX_LOCALE_LEN];
    char validityDate[PTEID_MAX_DATE_LEN];
    char name[PTEID_MAX_NAME_LEN];
    char firstname[PTEID_MAX_NAME_LEN];
    char sex[PTEID_MAX_SEX_LEN];
    char nationality[PTEID_MAX_NATIONALITY_LEN];
    char birthDate[PTEID_MAX_DATE_LEN];
    char height[PTEID_MAX_HEIGHT_LEN];
    char numBI[PTEID_MAX_NUMBI_LEN];
    char nameFather[PTEID_MAX_NAME_LEN];
    char firstnameFather[PTEID_MAX_NAME_LEN];
    char nameMother[PTEID_MAX_NAME_LEN];
    char firstnameMother[PTEID_MAX_NAME_LEN];
    char numNIF[PTEID_MAX_NUMNIF_LEN];
    char numSS[PTEID_MAX_NUMSS_LEN];
    char numSNS[PTEID_MAX_NUMSNS_LEN];
    char notes[PTEID_MAX_INDICATIONEV_LEN];
    char mrz1[PTEID_MAX_MRZ_LEN];
    char mrz2[PTEID_MAX_MRZ_LEN];
    char mrz3[PTEID_MAX_MRZ_LEN];
} PTEID_ID;

typedef struct
{
    short version;
    char addrType[PTEID_MAX_ADDR_TYPE_LEN];
    char country[PTEID_MAX_ADDR_COUNTRY_LEN];
    char district[PTEID_MAX_DISTRICT_LEN];
    char districtDesc[PTEID_MAX_DISTRICT_DESC_LEN];
    char municipality[PTEID_MAX_DISTRICT_CON_LEN];
    char municipalityDesc[PTEID_MAX_DISTRICT_CON_DESC_LEN];
    char freguesia[PTEID_MAX_DISTRICT_FREG_LEN];
    char freguesiaDesc[PTEID_MAX_DISTRICT_FREG_DESC_LEN];
    char streettypeAbbr[PTEID_MAX_ROAD_ABBR_LEN];
    char streettype[PTEID_MAX_ROAD_LEN];
    char street[PTEID_MAX_ROAD_DESIG_LEN];
    char buildingAbbr[PTEID_MAX_HOUSE_ABBR_LEN];
    char building[PTEID_MAX_HOUSE_LEN];
    char door[PTEID_MAX_NUMDOOR_LEN];
    char floor[PTEID_MAX_FLOOR_LEN];
    char side[PTEID_MAX_SIDE_LEN];
    char place[PTEID_MAX_PLACE_LEN];
    char locality[PTEID_MAX_LOCALITY_LEN];
    char cp4[PTEID_MAX_CP4_LEN];
    char cp3[PTEID_MAX_CP3_LEN];
    char postal[PTEID_MAX_POSTAL_LEN];
    char numMor[PTEID_MAX_NUMMOR_LEN];
    char countryDescF[PTEID_MAX_ADDR_COUNTRYF_DESC_LEN];
    char addressF[PTEID_MAX_ADDRF_LEN];
    char cityF[PTEID_MAX_CITYF_LEN];
    char regioF[PTEID_MAX_REGIOF_LEN];
    char localityF[PTEID_MAX_LOCALITYF_LEN];
    char postalF[PTEID_MAX_POSTALF_LEN];
    char numMorF[PTEID_MAX_NUMMORF_LEN];
} PTEID_ADDR;

typedef struct
{
    short version;
    unsigned char cbeff[PTEID_MAX_CBEFF_LEN];
    unsigned char facialrechdr[PTEID_MAX_FACRECH_LEN];
    unsigned char facialinfo[PTEID_MAX_FACINFO_LEN];
    unsigned char imageinfo[PTEID_MAX_IMAGEINFO_LEN];
    unsigned char picture[PTEID_MAX_PICTUREH_LEN];
    unsigned long piclength;
} PTEID_PIC;

typedef struct
{
	unsigned char certif[PTEID_MAX_CERT_LEN];	/* Byte stream encoded certificate */
	long certifLength;					  /* Size in bytes of the encoded certificate */
	char certifLabel[PTEID_MAX_CERT_LABEL_LEN];     /* Label of the certificate (Authentication, Signature, CA, Root,) */
} PTEID_Certif;

typedef struct
{
	PTEID_Certif certificates[PTEID_MAX_CERT_NUMBER];  /* Array of PTEID_Certif structures */
	long certificatesLength;			/* Number of elements in Array */
} PTEID_Certifs;

typedef struct
{
  long pinType;             // ILEID_PIN_TYPE_PKCS15 or PTEID_PIN_TYPE_OS
  unsigned char id;                    // PIN reference or ID
  long usageCode;       // Usage code (PTEID_USAGE_AUTH, PTEID_USAGE_SIGN, ...)
  long triesLeft;
  long flags;
  char label[PTEID_MAX_PIN_LABEL_LEN];
  char *shortUsage;     // May be NULL for usage known by the middleware
  char *longUsage;      // May be NULL for usage known by the middleware
} PTEIDPin;

typedef struct
{
	PTEIDPin pins[PTEID_MAX_PINS];  /* Array of PTEID_Pin structures */
	long pinsLength;			        /* Number of elements in Array */
} PTEIDPins;

typedef struct
{
	char label[PTEID_MAX_CERT_LABEL_LEN];
	char serial[PTEID_MAX_ID_NUMBER_LEN];
} PTEID_TokenInfo;


PTEIDSDK_API long PTEID_Init(
			char *ReaderName		/**< in: the PCSC reader name (as returned by SCardListReaders()),
								 specify NULL if you want to select the first reader */
);

PTEIDSDK_API long PTEID_Exit(
			unsigned long ulMode	/**< in: exit mode, either PTEID_EXIT_LEAVE_CARD or PTEID_EXIT_UNPOWER */
);

PTEIDSDK_API tCompCardType PTEID_GetCardType();

/**
 * Read the ID data.
 */
PTEIDSDK_API long PTEID_GetID(
	PTEID_ID *IDData		/**< out: the address of a PTEID_ID struct */
);

/**
 * Read the Address data.
 */
PTEIDSDK_API long PTEID_GetAddr(
	PTEID_ADDR *AddrData	/**< out: the address of an PTEID_ADDR struct */
);

/**
 * Read the Picture.
 */
PTEIDSDK_API long PTEID_GetPic(
	PTEID_PIC *PicData		/**< out: the address of a PTEID_PIC struct */
);

/**
 * Read all the user and CA certificates.
 */
PTEIDSDK_API long PTEID_GetCertificates(
	PTEID_Certifs *Certifs	/**< out: the address of a PTEID_Certifs struct */
);

/**
 * Verify a PIN.
 */
PTEIDSDK_API long PTEID_VerifyPIN(
	unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
	char *Pin,				/**< in: the PIN value, if NULL then the user will be prompted for the PIN */
	long *triesLeft			/**< out: the remaining PIN tries */
);

/**
 * Verify a PIN. If this is the signature PIN, do not display an alert message.
 */
PTEIDSDK_API long PTEID_VerifyPIN_No_Alert(
	unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
	char *Pin,				/**< in: the PIN value, if NULL then the user will be prompted for the PIN */
	long *triesLeft			/**< out: the remaining PIN tries */
);

/**
 * Change a PIN.
 */
PTEIDSDK_API long PTEID_ChangePIN(
	unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
	char *pszOldPin,		/**< in: the current PIN value, if NULL then the user will be prompted for the PIN */
	char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
	long *triesLeft			/**< out: the remaining PIN tries */
);

/**
 * Return the PINs (that are listed in the PKCS15 files).
 */
PTEIDSDK_API long PTEID_GetPINs(
	PTEIDPins *Pins		/**< out: the address of a PTEID_Pins struct */
);

/**
 * Return the PKCS15 TokenInfo contents.
 */
PTEIDSDK_API long PTEID_GetTokenInfo(
	PTEID_TokenInfo *tokenData	/**< out: the address of a PTEID_TokenInfo struct */
);

/**
 * Read the contents of the SOD file from the card.
 * This function calls PTEID_ReadFile() with the SOD file as path.
 * If *outlen is less then the file's contents, only *outlen
 * bytes will be read. If *outlen is bigger then the file's
 * contents then the file's contents are returned without error. */
PTEIDSDK_API long PTEID_ReadSOD(
	unsigned char *out,         /**< out: the buffer to hold the file contents */
	unsigned long *outlen		/**< in/out: number of bytes allocated/number of bytes read */
);

/**
 * Unblock PIN with PIN change.
 * If pszPuk == NULL or pszNewPin == NULL, a GUI is shown asking for the PUK and the new PIN
 */
PTEIDSDK_API long PTEID_UnblockPIN(
	unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
	char *pszPuk,			/**< in: the PUK value, if NULL then the user will be prompted for the PUK */
	char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
	long *triesLeft			/**< out: the remaining PUK tries */
);

/**
 * Extended Unblock PIN functionality.
 * E.g. calling PTEID_UnblockPIN_Ext() with ulFlags = UNBLOCK_FLAG_NEW_PIN
 *   is the same as calling PTEID_UnblockPIN(...)
 */
PTEIDSDK_API long PTEID_UnblockPIN_Ext(
	unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
	char *pszPuk,			/**< in: the PUK value, if NULL then the user will be prompted for the PUK */
	char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
	long *triesLeft,		/**< out: the remaining PUK tries */
	unsigned long ulFlags	/**< in: flags: 0, UNBLOCK_FLAG_NEW_PIN, UNBLOCK_FLAG_PUK_MERGE or
									UNBLOCK_FLAG_NEW_PIN | UNBLOCK_FLAG_PUK_MERGE */
);

/**
 * Select an Application Directory File (ADF) by means of the AID (Application ID).
 */
PTEIDSDK_API long PTEID_SelectADF(
	unsigned char *adf,		/**< in: the AID of the ADF */
	long adflen				/**< in: the length */
);

/**
 * Read a file on the card.
 * If a PIN reference is provided and needed to read the file,
 * the PIN will be asked and checked if needed.
 * If *outlen is less then the file's contents, only *outlen
 * bytes will be read. If *outlen is bigger then the file's
 * contents then the file's contents are returned without error.
 */
PTEIDSDK_API long PTEID_ReadFile(
	unsigned char *file,	/**< in: a byte array containing the file path,
								e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x02} for the ID file */
	int filelen,			/**< in: file length */
	unsigned char *out,		/**< out: the buffer to hold the file contents */
	unsigned long *outlen,	/**< in/out: number of bytes allocated/number of bytes read */
	unsigned char PinId		/**< in: the ID of the Address PIN (only needed when reading the Address File) */
);

/**
 * Write data to a file on the card.
 * If a PIN reference is provided, the PIN will be asked and checked
 * if needed (just-in-time checking).
 * This function is only applicable for writing to the Personal Data file.
 */
PTEIDSDK_API long PTEID_WriteFile(
	unsigned char *file,	/**< in: a byte array containing the file path,
								e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x02} for the ID file */
	int filelen,			/**< in: file length */
	unsigned char *in,		/**< in: the data to be written to the file */
	unsigned long inlen,	/**< in: length of the data to be written */
	unsigned char PinId		/**< in: the ID of the Authentication PIN, see the PTEID_Pins struct */
);

/**
 * Get the activation status of the card.
 */
PTEIDSDK_API long PTEID_IsActivated(
	unsigned long *pulStatus	/**< out the activation status: 0 if not activate, 1 if activated */
);

/**
 * Activate the card (= update a specific file on the card).
 * If the card has been activated allready, SC_ERROR_NOT_ALLOWED is returned.
 */
PTEIDSDK_API long PTEID_Activate(
	char *pszPin,			/**< in: the value of the Activation PIN */
	unsigned char *pucDate,	/**< in: the current date in DD MM YY YY format in BCD format (4 bytes),
									e.g. {0x17 0x11 0x20 0x06} for the 17th of Nov. 2006) */
	unsigned long ulMode	/**<in: mode: MODE_ACTIVATE_BLOCK_PIN to block the Activation PIN,
									or to 0 otherwise (this should only to be used for testing). */
);


}

#endif //__PTEIDLIB_H__
