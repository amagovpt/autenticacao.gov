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
#ifndef _WIN32
#include <stdint.h>
#endif
#include "eidlibcompat.h"
#include "eidlibdefines.h"

namespace eIDMW
{

// XMLUSERDATA - BEGIN
#define XML_ROOT_ELEMENT 					"ccpt"
#define XML_ROOT_ELEMENT_ATTR_TIMESTAMP		"timestamp"
#define XML_ROOT_ELEMENT_ATTR_SERVERNAME	"servername"
#define XML_ROOT_ELEMENT_ATTR_SERVERADDRESS	"serveraddress"
#define XML_ROOT_ELEMENT_ATTR_TOKENID		"tokenid"
#define XML_PHOTO_ELEMENT 					"photo"
#define XML_BASIC_INFO_ELEMENT				"basicInformation"
#define XML_NAME_ELEMENT					"name"
#define XML_GIVEN_NAME_ELEMENT				"givenName"
#define XML_SURNAME_ELEMENT					"surname"
#define XML_NIC_ELEMENT						"nic"
#define XML_EXPIRY_DATE_ELEMENT				"expiryDate"
#define XML_CIVIL_INFO_ELEMENT				"CivilInformation"
#define XML_GENDER_ELEMENT					"sex"
#define XML_HEIGHT_ELEMENT					"height"
#define XML_NATIONALITY_ELEMENT				"nationality"
#define XML_DATE_OF_BIRTH_ELEMENT			"dateOfBirth"
#define XML_GIVEN_NAME_FATHER_ELEMENT		"givenNameFather"
#define XML_SURNAME_FATHER_ELEMENT			"surnameFather"
#define XML_GIVEN_NAME_MOTHER_ELEMENT		"givenNameMother"
#define XML_SURNAME_MOTHER_ELEMENT			"surnameMother"
#define XML_ACCIDENTAL_INDICATIONS_ELEMENT	"notes"
#define XML_IDENTIFICATION_NUMBERS_ELEMENT	"IdentificationNumbers"
#define XML_DOCUMENT_NO_ELEMENT				"DocumentNo"
#define XML_TAX_NO_ELEMENT					"TaxNo"
#define XML_SOCIAL_SECURITY_NO_ELEMENT		"SocialSecurityNo"
#define XML_HEALTH_NO_ELEMENT				"HealthNo"
#define XML_MRZ1_ELEMENT					"mrz1"
#define XML_MRZ2_ELEMENT					"mrz2"
#define XML_MRZ3_ELEMENT					"mrz3"
#define XML_CARD_VALUES_ELEMENT				"CardValues"
#define XML_CARD_VERSION_ELEMENT			"cardVersion"
#define XML_CARD_NUMBER_PAN_ELEMENT			"cardNumberPAN"
#define XML_ISSUING_DATE_ELEMENT			"issuingDate"
#define XML_ISSUING_ENTITY_ELEMENT			"issuingEntity"
#define XML_DOCUMENT_TYPE_ELEMENT			"documentType"
#define XML_LOCAL_OF_REQUEST_ELEMENT		"localOfRequest"
#define XML_VERSION_ELEMENT					"version"
#define XML_ADDRESS_ELEMENT					"Address"
#define XML_DISTRICT_ELEMENT				"district"
#define XML_MUNICIPALITY_ELEMENT			"municipality"
#define XML_CIVIL_PARISH_ELEMENT			"civilParish"
#define XML_ABBR_STREET_TYPE_ELEMENT		"abrStreetType"
#define XML_STREET_TYPE_ELEMENT				"streetType"
#define XML_STREET_NAME_ELEMENT				"streetName"
#define XML_ABBR_BUILDING_TYPE_ELEMENT		"abrBuildingType"
#define XML_BUILDING_TYPE_ELEMENT			"buildingType"
#define XML_DOOR_NO_ELEMENT					"doorNo"
#define XML_FLOOR_ELEMENT					"floor"
#define XML_SIDE_ELEMENT					"side"
#define XML_PLACE_ELEMENT					"place"
#define XML_LOCALITY_ELEMENT				"locality"
#define XML_ZIP4_ELEMENT					"zip4"
#define XML_ZIP3_ELEMENT					"zip3"
#define XML_POSTAL_LOCALITY_ELEMENT			"postalLocality"
#define XML_PERSONAL_NOTES_ELEMENT			"userNotes"

#define XML_FOREIGN_COUNTRY_ELEMENT			"foreignCountry"
#define XML_FOREIGN_ADDRESS_ELEMENT			"foreignAddress"
#define XML_FOREIGN_CITY_ELEMENT			"foreignCity"
#define XML_FOREIGN_REGION_ELEMENT			"foreignRegion"
#define XML_FOREIGN_LOCALITY_ELEMENT		"foreignLocality"
#define XML_FOREIGN_POSTAL_CODE_ELEMENT		"foreignPostalCode"

#define XML_OPEN_TAG(tag) "<" tag ">"
#define XML_OPEN_TAG_NEWLINE(tag) "<" tag ">\n"
#define XML_CLOSE_TAG(tag) "</" tag ">\n"

#define XML_ATTRIBUTE(container,attributename, data) \
	container+=attributename; \
	container+="=\""; \
	container+=data; \
	container+="\" ";
#define XML_ROOT_S "<" XML_ROOT_ELEMENT " "
#define XML_ROOT_E ">\n"

#define BUILD_XML_ELEMENT(container, tag, data) \
	container+=XML_OPEN_TAG(tag); \
	container+=data; \
	container+=XML_CLOSE_TAG(tag);

#define BUILD_XML_ELEMENT_NEWLINE(container, tag, data) \
	container+=XML_OPEN_TAG_NEWLINE(tag); \
	container+=data; \
	container+=XML_CLOSE_TAG(tag);

#define XML_ESCAPE_QUOTE	"\"", "&quot;"
#define XML_ESCAPE_APOS		"'", "&apos;"
#define XML_ESCAPE_LT		"<", "&lt;"
#define XML_ESCAPE_GT		">", "&gt;"
#define XML_ESCAPE_AMP		"&", "&amp;"

enum XMLUserData
{
	XML_PHOTO=0,
	XML_NAME,
	XML_GIVEN_NAME,
	XML_SURNAME,
	XML_NIC,
	XML_EXPIRY_DATE,
	XML_GENDER,
	XML_HEIGHT,
	XML_NATIONALITY,
	XML_DATE_OF_BIRTH,
	XML_GIVEN_NAME_FATHER,
	XML_SURNAME_FATHER,
	XML_GIVEN_NAME_MOTHER,
	XML_SURNAME_MOTHER,
	XML_ACCIDENTAL_INDICATIONS,
	XML_DOCUMENT_NO,
	XML_TAX_NO,
	XML_SOCIAL_SECURITY_NO,
	XML_HEALTH_NO,
	XML_MRZ1,
	XML_MRZ2,
	XML_MRZ3,
	XML_CARD_VERSION,
	XML_CARD_NUMBER_PAN,
	XML_ISSUING_DATE,
	XML_ISSUING_ENTITY,
	XML_DOCUMENT_TYPE,
	XML_LOCAL_OF_REQUEST,
	XML_VERSION,
	XML_DISTRICT,
	XML_MUNICIPALITY,
	XML_CIVIL_PARISH,
	XML_ABBR_STREET_TYPE,
	XML_STREET_TYPE,
	XML_STREET_NAME,
	XML_ABBR_BUILDING_TYPE,
	XML_BUILDING_TYPE,
	XML_DOOR_NO,
	XML_FLOOR,
	XML_SIDE,
	XML_PLACE,
	XML_LOCALITY,
	XML_ZIP4,
	XML_ZIP3,
	XML_POSTAL_LOCALITY,
	XML_PERSONAL_NOTES,
	XML_FOREIGN_COUNTRY,
	XML_FOREIGN_ADDRESS,
	XML_FOREIGN_CITY,
	XML_FOREIGN_REGION,
	XML_FOREIGN_LOCALITY,
	XML_FOREIGN_POSTAL_CODE
};
// XMLUSERDATA - END




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
		PTEIDSDK_API PTEID_ByteArray();	/**< Default constructor */
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
		 * Get a string from buffer at ulOffset position and with ulLen size.
		 */
		const char *GetStringAt(unsigned long ulOffset, unsigned long ulLen) const;

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


class APLPublicKey;

class PTEID_PublicKey : public PTEID_Object
{
public:
	PTEIDSDK_API virtual ~PTEID_PublicKey();												/**< Destructor */

	PTEIDSDK_API PTEID_ByteArray& getCardAuthKeyModulus();
	PTEIDSDK_API PTEID_ByteArray& getCardAuthKeyExponent();

	NOEXPORT_PTEIDSDK PTEID_PublicKey(const SDK_Context *context,const APLPublicKey &impl);

private:
	PTEID_PublicKey(const PTEID_PublicKey& cardKey);				/**< Copy not allowed - not implemented */
	PTEID_PublicKey& operator= (const PTEID_PublicKey& cardKey);	/**< Copy not allowed - not implemented */
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

	PTEIDSDK_API virtual ~PTEID_ReaderContext();	/**< Destructor */

	/**
	  * Return the name of the reader.
	  */
	PTEIDSDK_API const char *getName();

	/**
	  * Return true if a card is present and false otherwise.
	  */
    PTEIDSDK_API bool isCardPresent();

    PTEIDSDK_API bool isPinpad();

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
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @return a PTEID_ByteArray containing the result
	 */
    PTEIDSDK_API virtual PTEID_ByteArray sendAPDU(const PTEID_ByteArray& cmd);

    PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray& data, bool signatureKey=false);

    PTEIDSDK_API virtual PTEID_ByteArray SignSHA256(const PTEID_ByteArray& data, bool signatureKey=false);

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
	 * @param inOffset is the offset of the data to be written to the file
	 */
	PTEIDSDK_API virtual bool writeFile(const char *fileID,const PTEID_ByteArray &out, PTEID_Pin *pin=NULL,const char *csPinCode="",unsigned long inOffset=0);

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

protected:
	PTEID_SmartCard(const SDK_Context *context,APL_Card *impl);	/**< For internal use : Constructor */

private:
	PTEID_SmartCard(const PTEID_SmartCard& card);						/**< Copy not allowed - not implemented */
	PTEID_SmartCard& operator= (const PTEID_SmartCard& card);			/**< Copy not allowed - not implemented */
};

class PTEID_EId;
class PTEID_Address;
class PTEID_Sod;
class PTEID_CardVersionInfo;
class PTEID_Certificate;
class APL_EIDCard;
class PTEID_XmlUserRequestedInfo;
class PTEID_CCXML_Doc;
class PDFSignature;


class PTEID_PDFSignature
{
	public:
		PTEIDSDK_API PTEID_PDFSignature();
		PTEIDSDK_API PTEID_PDFSignature(const char *input_path);
		PTEIDSDK_API ~PTEID_PDFSignature();

        PTEIDSDK_API void setFileSigning(char *input_path);
		PTEIDSDK_API void addToBatchSigning(char *input_path);
		PTEIDSDK_API void addToBatchSigning(char *input_path, bool last_page);
		PTEIDSDK_API int getPageCount();
		PTEIDSDK_API int getOtherPageCount(const char *input_path);
		PTEIDSDK_API void enableTimestamp();
		PTEIDSDK_API void enableSmallSignatureFormat();
		PTEIDSDK_API bool isLandscapeFormat();
		PTEIDSDK_API char *getOccupiedSectors(int page);
		PTEIDSDK_API void setCustomImage(unsigned char *image_data, unsigned long image_length);
		PTEIDSDK_API void setCustomImage(const PTEID_ByteArray &image_data);

		PTEIDSDK_API PDFSignature *getPdfSignature();

	private:
	//The applayer object that actually implements the signature
		PDFSignature *mp_signature;

	friend class PTEID_EIDCard;

};


//Fwd declaration
class ScapSSLConnection;

class PTEID_ScapConnection
{
	public:
		PTEIDSDK_API PTEID_ScapConnection(char *host, char *port);
		PTEIDSDK_API ~PTEID_ScapConnection();
		PTEIDSDK_API char *postSoapRequest(char *endpoint, char *soapAction, char *soapBody);

	private:
		ScapSSLConnection *m_connection;
};

class SecurityContext;


/******************************************************************************//**
  * This class represents a Portugal EID card.
  * To get such an object you have to ask it from the ReaderContext.
  *********************************************************************************/
class PTEID_EIDCard : public PTEID_SmartCard
{
public:
	PTEIDSDK_API virtual ~PTEID_EIDCard();						/**< Destructor */


	PTEIDSDK_API PTEID_CCXML_Doc& getXmlCCDoc(PTEID_XmlUserRequestedInfo& userRequestedInfo);
	PTEIDSDK_API PTEID_EId& getID();							/**< Get the id document */
	PTEIDSDK_API PTEID_Address& getAddr();					/**< Get the Address document */
	PTEIDSDK_API PTEID_Sod& getSod();							/**< Get the sod document */
	PTEIDSDK_API PTEID_CardVersionInfo& getVersionInfo();		/**< Get the info document  */
	PTEIDSDK_API bool writePersonalNotes(const PTEID_ByteArray &out,PTEID_Pin *pin=NULL,const char *csPinCode="");
	PTEIDSDK_API bool clearPersonalNotes(PTEID_Pin *pin=NULL,const char *csPinCode="");
	PTEIDSDK_API const char *readPersonalNotes();

	PTEIDSDK_API PTEID_Certificate &getCert(PTEID_CertifType type);/**< Return certificate by type from the card */
	PTEIDSDK_API PTEID_Certificate &getRoot();				/**< Return the root certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getCA();					/**< Return the ca certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getSignature();			/**< Return the signature certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getAuthentication();		/**< Return the authentication certificate from the card */

	PTEIDSDK_API PTEID_PublicKey& getRootCAPubKey();		/**< Get the CVC CA public key that this card uses to verify the CVC key */
	PTEIDSDK_API bool isActive();
	PTEIDSDK_API void doSODCheck(bool check);			/**< enable/disable the checking of the data against the sod*/
	PTEIDSDK_API bool Activate(const char *pinCode, PTEID_ByteArray &BCDDate, bool blockActivationPIN); 	/**< Activate the pteid card */


	    /** Produce Xades Signature of Arbitrary Contents (from memory or local files)
	    *
	    *  The next 4 Methods return an UTF-8 encoded byte array containing a full XAdES or XAdES-T
	    *  signature of the content supplied (either as pointer to a memory buffer
	    *  or as local file paths) signed by the Citizen Signature Key.
	    *  These method perform interactive PIN authentication if needed
	    *
	    *  @param IN paths is an array of null-terminated strings representing absolute paths in
	    *  the local filesystem. Those files content (hashed with SHA-1 algorithm) will be the input data for the RSA signature
	    *  @param IN n_paths is the number of elements in the paths array
	    */
	     PTEIDSDK_API PTEID_ByteArray SignXades(const char *output_path, const char * const* paths, unsigned int n_paths); /** Return a Xades signature as a UTF-8 string (supports multiple files)*/

	     PTEIDSDK_API PTEID_ByteArray SignXadesT(const char *output_path, const char * const* path, unsigned int n_paths); /** Return a Xades-T signature as a UTF-8 string (supports multiple files)*/
	     PTEIDSDK_API PTEID_ByteArray SignXadesA(const char *output_path, const char * const* path, unsigned int n_paths); /** Return a Xades-T signature as a UTF-8 string (supports multiple files)*/

	     PTEIDSDK_API void SignXadesIndividual(const char *output_path,  const char * const* paths, unsigned int n_paths); /** Store the XAdes signature in individual zip containers  */
	     PTEIDSDK_API void SignXadesTIndividual(const char *output_path, const char * const* paths, unsigned int n_paths); /** Store the Xades-T signature in individual zip containers  */
		 PTEIDSDK_API void SignXadesAIndividual(const char *output_path, const char * const* paths, unsigned int n_paths);

	     //PDF Signature with location by page sector (the portrait A4 page is split into 18 cells: 6 lines and 3 columns)
	     PTEIDSDK_API int SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape, const char *location, const char *reason,
			const char *outfile_path);

	     /*PDF Signature with location by coordinates (expressed in percentage of page height/width). The coordinate system has its origin in the top left corner
	     of the page
		 * @param sig_handler: this defines the input file and some signature options
		 * @param page: in case of visible signature it defines the page where the signature will appear
		 * @param coord_x: X coordinate of the signature location (percentage of page width)
		 * @param coord_y: Y coordinate of the signature location (percentage of page height)
		 * @param location: Signature metadata field
		 * @param reason: Signature metadata field
		 * @param outfile_path: Native Filesystem path of the ouput file
		 */
	    PTEIDSDK_API int SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y, const char *location, const char *reason,
			const char *outfile_path);

	     /* Change the OTP/EMV-CAP PIN through interaction with the appropriate HTTPS server
	      * Note: This method SHOULD be always called before any change to the Authentication PIN
	      * because the two PINs are the same from the user's perspective
	      *
	      */
	     //PTEIDSDK_API bool ChangeCapPin(const char *new_pin);

	     typedef void (*t_address_change_callback)(void *, int);

	     PTEIDSDK_API void ChangeAddress(char *secretCode, char *process, t_address_change_callback callback, void *callback_data);

protected:
	PTEID_EIDCard(const SDK_Context *context,APL_Card *impl);		/**< For internal use : Constructor */

private:
	PTEID_EIDCard(const PTEID_EIDCard& card);						/**< Copy not allowed - not implemented */
	PTEID_EIDCard& operator= (const PTEID_EIDCard& card);			/**< Copy not allowed - not implemented */
	bool persoNotesDirty;

friend PTEID_Card &PTEID_ReaderContext::getCard();				/**< For internal use : This method must access protected constructor */
friend PTEIDSDK_API long ::PTEID_CVC_Init(const unsigned char *pucCert, int iCertLen, unsigned char *pucChallenge, int iChallengeLen);

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
    PTEIDSDK_API PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress, const char *tokenID);
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
  * You can get such an object from PTEID_EIDCard::getVersionInfo().
  *********************************************************************************/
class PTEID_CardVersionInfo : public PTEID_XMLDoc
{
public:
	PTEIDSDK_API  virtual ~PTEID_CardVersionInfo();		/**< Destructor */

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

private:
	PTEID_CardVersionInfo(const PTEID_CardVersionInfo& doc);				/**< Copy not allowed - not implemented */
	PTEID_CardVersionInfo& operator= (const PTEID_CardVersionInfo& doc);	/**< Copy not allowed - not implemented */

	PTEID_CardVersionInfo(const SDK_Context *context,APL_DocVersionInfo *impl);	/**< For internal use : Constructor */

friend PTEID_CardVersionInfo& PTEID_EIDCard::getVersionInfo();	/**< For internal use : This method must access protected constructor */
};

class APL_SodEid;

/******************************************************************************//**
  * Class for the sod document on a EID Card.
  * You can get such an object from PTEID_EIDCard::getSod().
  *********************************************************************************/
class PTEID_Sod : public PTEID_Biometric
{
public:
	PTEIDSDK_API virtual ~PTEID_Sod();				/**< Destructor */

	PTEIDSDK_API const PTEID_ByteArray& getData();		/**< Return the sod itself */

private:
	PTEID_Sod(const PTEID_Sod& doc);				/**< Copy not allowed - not implemented */
	PTEID_Sod& operator= (const PTEID_Sod& doc);	/**< Copy not allowed - not implemented */

	PTEID_Sod(const SDK_Context *context,APL_SodEid *impl);/**< For internal use : Constructor */

friend PTEID_Sod& PTEID_EIDCard::getSod();		/**< For internal use : This method must access protected constructor */
};

class APL_DocEId;

/******************************************************************************//**
  * Class for the id document on a EID Card.
  * You can get such an object from PTEID_EIDCard::getID().
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
	PTEIDSDK_API const char *getNationality();				/**< Return Nationality field */
	PTEIDSDK_API const char *getDocumentPAN();				/**< Return Document PAN field */
	PTEIDSDK_API const char *getValidityBeginDate();		/**< Return Validity Begin Date field */
	PTEIDSDK_API const char *getValidityEndDate();			/**< Return Validity End Date field */
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
	PTEIDSDK_API PTEID_PublicKey& getCardAuthKeyObj();	/**< Return object CardAuthKey */
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
  * You can get such an object from PTEID_EIDCard::getAddr().
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

	/**
	  * Get the pin by its pin reference.
	  * Throw PTEID_ExParamRange exception if the index is out of range.
	  */
	PTEIDSDK_API PTEID_Pin &getPinByPinRef(unsigned long pinRef);

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

	static const unsigned long AUTH_PIN = 129;
	static const unsigned long SIGN_PIN = 130;
	static const unsigned long ADDR_PIN = 131;

	PTEIDSDK_API virtual ~PTEID_Pin();				/**< Destructor */

	PTEIDSDK_API unsigned long getIndex();		/**< Get the index of the pin */
	PTEIDSDK_API unsigned long getType();			/**< Get the type of the pin */
	PTEIDSDK_API unsigned long getId();			/**< Get the id of the pin */
	PTEIDSDK_API unsigned long getPinRef();		/**< Get the pinref value of the pin */
	PTEIDSDK_API PTEID_PinUsage getUsageCode();	/**< Get the usage code of the pin */
	PTEIDSDK_API unsigned long getFlags();		               /**< Get the flags of the pin */
	PTEIDSDK_API const char *getLabel();			           /**< Get the label of the pin */
	PTEIDSDK_API const char *getLabelById( unsigned long id ); /**< Get the label of the pin by Id */
	PTEIDSDK_API bool unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long flags);

	/**
	  * Return the remaining tries for entering the correct pin.
	  *
	  *
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
	  * @param bShowDlg flag used to either show or not a dialog where the user inserts the pin (default=true)
	  * @param wndGeometry DEPRECATED (default=0)
	  * @return true if success and false if failed
	  */
	PTEIDSDK_API bool verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg=true, void *wndGeometry = 0 );

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
	PTEIDSDK_API bool changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg=true, void *wndGeometry = 0 );

private:
	PTEID_Pin(const PTEID_Pin& pin);									/**< Copy not allowed - not implemented */
	PTEID_Pin& operator= (const PTEID_Pin& pin);						/**< Copy not allowed - not implemented */

	PTEID_Pin(const SDK_Context *context,APL_Pin *impl);						/**< For internal use : Constructor */

friend PTEID_Pin &PTEID_Pins::getPinByNumber(unsigned long ulIndex);	/**< For internal use : This method must access protected constructor */
friend PTEID_Pin &PTEID_Pins::getPinByPinRef(unsigned long pinRef);		/**< For internal use : This method must access protected constructor */
};

class APL_Certifs;

/******************************************************************************//**
  * Container class for all certificates on the card.
  *********************************************************************************/
class PTEID_Certificates : public PTEID_Crypto
{
public:

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

	PTEIDSDK_API void addToSODCAs(PTEID_ByteArray &cert);

	PTEIDSDK_API void resetSODCAs();


private:
	PTEID_Certificates(const PTEID_Certificates& certifs);			/**< Copy not allowed - not implemented */
	PTEID_Certificates& operator= (const PTEID_Certificates& certifs);/**< Copy not allowed - not implemented */

	PTEID_Certificates(const SDK_Context *context,APL_Certifs *impl);/**< For internal use : Constructor */

friend PTEID_Certificates& PTEID_SmartCard::getCertificates();		/**< For internal use : This method must access protected constructor */
};

class APL_Certif;

/******************************************************************************//**
  * Class that represents one certificate.
  *********************************************************************************/
class PTEID_Certificate : public PTEID_Crypto
{
public:
	static const unsigned long CITIZEN_AUTH = 0;
	static const unsigned long CITIZEN_SIGN = 1;
	static const unsigned long SUB_CA_SIGN = 2;
	static const unsigned long SUB_CA_AUTH = 3;

	PTEIDSDK_API virtual ~PTEID_Certificate();			/**< Destructor */

	PTEIDSDK_API const char *getLabel();				/**< Return the label of the certificate */
	PTEIDSDK_API unsigned long getID();				/**< Return the id of the certificate */
	PTEIDSDK_API PTEID_CertifStatus getStatus();			/** OCSP/CRL validation status */

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

/**********************************************************************************
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
        * Create object to access a string parameter.
        */
    PTEIDSDK_API PTEID_Config(const char *csName, const wchar_t *czSection, const wchar_t *csDefaultValue);

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


#if !defined SWIG
/* Function to get System Proxy to access supplied host */
PTEIDSDK_API void PTEID_GetProxyFromPac(const char *pacFile, const char *url, std::string *proxy_host, std::string *proxy_port);
#endif


PTEIDSDK_API void setCompatReaderContext(eIDMW::PTEID_ReaderContext *ctx);

PTEIDSDK_API eIDMW::PTEID_ByteArray PTEID_CVC_Init(eIDMW::PTEID_ByteArray cvc_cert);

PTEIDSDK_API void PTEID_CVC_Authenticate(eIDMW::PTEID_ByteArray cvc_cert);

PTEIDSDK_API eIDMW::PTEID_ByteArray PTEID_CVC_ReadFile(eIDMW::PTEID_ByteArray fileID);


} //Namespace eIDMW

#endif //__PTEIDLIB_H__
