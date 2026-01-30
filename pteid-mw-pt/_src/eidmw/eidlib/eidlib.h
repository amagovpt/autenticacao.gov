/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2011-2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012-2014, 2016-2024 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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

#include <cstdint>
#ifndef __PTEIDLIB_H__
#define __PTEIDLIB_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#ifndef _WIN32
#include <stdint.h>
#endif
#include "eidlibdefines.h"

#if !defined SWIG
#include "CardCallbacks.h"
#endif

namespace eIDMW {

enum XMLUserData {
	XML_PHOTO = 0,
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

enum PTEID_SigningDeviceType { CC, CMD };

/** @brief RSA padding type
 *  Which type of padding to be used while signing with RSA keys
 */
enum PTEID_RSAPaddingType {
	PADDING_TYPE_RSA_PKCS,
	PADDING_TYPE_RSA_PSS,
};

struct SDK_Context;

class CMutex;
class PTEID_Exception;

/**
 * Base class for the object of PTEID SDK (Can not be instantiated).
 */

class PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Object() = 0; /**< Destructor */

	NOEXPORT_PTEIDSDK void Init(const SDK_Context *context, void *impl); /**< For internal use : Initialize pimpl */

	/**
	 * Release the objects instantiated within this PTEID_Object.
	 * Automatically call in the destructor.
	 */
	NOEXPORT_PTEIDSDK void Release();

protected:
	PTEID_Object(const SDK_Context *context, void *impl); /**< For internal use : Constructor */
	void addObject(PTEID_Object *impl);					  /**< For internal use : Add linked object */
	void backupObject(unsigned long idx);				  /**< For internal use : Backup linked object */
	PTEID_Object *getObject(unsigned long idx);			  /**< For internal use : Return an object by its index */
	PTEID_Object *getObject(void *impl);				  /**< For internal use : Return an object by its impl */
	void delObject(unsigned long idx);					  /**< For internal use : Delete an object by its index */
	void delObject(void *impl);							  /**< For internal use : Delete an object by its impl */
	/** For internal use : check if the Context is still correct (the card hasn't changed) */
	void checkContextStillOk() const;

	PTEID_Object(const PTEID_Object &obj);			  /**< Copy not allowed - not implemented */
	PTEID_Object &operator=(const PTEID_Object &obj); /**< Copy not allowed - not implemented */

	bool m_delimpl;				   /**< For internal use : m_impl object must be deleted */
	void *m_impl;				   /**< For internal use : pimpl pointer */
	unsigned long m_ulIndexExtAdd; /**< For internal use : extended add object */
	/** For internal use : Map of object instantiated within this PTEID_Object */
	std::map<unsigned long, PTEID_Object *> m_objects;

	SDK_Context *m_context; /**< For internal use : context structure */

	// CMutex *m_mutex;
};

class PTEID_Card;

class CByteArray;

/**
 * Class used to receive and supply an array of bytes from/to different methods in the SDK.
 *
 *  Memory management note - instances of this class will not own the char arrays that are supplied to
 *  it in the constructor or Append() methods as this class will perform copies of the input arrays
 */
class PTEID_ByteArray : public PTEID_Object {
public:
	PTEIDSDK_MULTIPASS_API PTEID_ByteArray();									/**< Default constructor */
	PTEIDSDK_MULTIPASS_API PTEID_ByteArray(const PTEID_ByteArray &bytearray); /**< Copy constructor */

	/**
	 * Constructor - initialize a byte array with an array of unsigned char.
	 *
	 * @param pucData is the byte array
	 * @param ulSize is the size of the array
	 **/
	PTEIDSDK_MULTIPASS_API PTEID_ByteArray(const unsigned char *pucData, unsigned long ulSize);

	PTEIDSDK_MULTIPASS_API virtual ~PTEID_ByteArray(); /**< Destructor */

	/**
	 * Append data to the byte array.
	 *
	 * @param pucData is the byte array
	 * @param ulSize is the size of the array
	 **/
	PTEIDSDK_MULTIPASS_API void Append(const unsigned char *pucData, unsigned long ulSize);

	/**
	 * Append data to the byte array.
	 */
	PTEIDSDK_MULTIPASS_API void Append(const PTEID_ByteArray &data);

	/**
	 * Remove the data from the byte array.
	 */
	PTEIDSDK_MULTIPASS_API void Clear();

	/**
	 * Return true if the content of data is the same as this.
	 */
	PTEIDSDK_MULTIPASS_API bool Equals(const PTEID_ByteArray &data) const;

	/**
	 * Return the number of bytes in the array.
	 */
	PTEIDSDK_MULTIPASS_API unsigned long Size() const;

	/**
	 * Return the array of bytes in the object.
	 * If Size() == 0, then NULL is returned.
	 */
	PTEIDSDK_MULTIPASS_API const unsigned char *GetBytes() const;

	/**
	 * Get a string from buffer at ulOffset position and with ulLen size.
	 */
	const char *GetStringAt(unsigned long ulOffset, unsigned long ulLen) const;

	/**
	 * Writing the binary content to a file.
	 */
	PTEIDSDK_MULTIPASS_API bool writeToFile(const char *csFilePath);

	/**
	 * Copy content of bytearray.
	 */
	PTEIDSDK_MULTIPASS_API PTEID_ByteArray &operator=(const PTEID_ByteArray &bytearray);

#if !defined SWIG
	/**
	 * For internal use: construct from lower level object
	 */
	PTEID_ByteArray(const SDK_Context *context, const CByteArray &impl);
	/**
	 * For internal use: copy from lower level object
	 */
	PTEID_ByteArray &operator=(const CByteArray &bytearray);
#endif
};

class PhotoPteid;

/**
   Class that represents the Citizen photograph stored in a pteid card
*/

class PTEID_Photo : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Photo(); /**< Destructor */

	PTEIDSDK_API PTEID_ByteArray & getphotoRAW(); /**< Retrieve the byte contents of the photo as stored in the card in JPEG-2000 format */
	PTEIDSDK_API PTEID_ByteArray & getphoto(); /**< Retrieve the byte contents of the photo converted to PNG format for maximum compatibility */
	PTEIDSDK_API PTEID_ByteArray &getphotoCbeff();
	PTEIDSDK_API PTEID_ByteArray &getphotoFacialrechdr();
	PTEIDSDK_API PTEID_ByteArray &getphotoFacialinfo();
	PTEIDSDK_API PTEID_ByteArray &getphotoImageinfo();

	NOEXPORT_PTEIDSDK PTEID_Photo(const SDK_Context *context, const PhotoPteid &impl);

private:
	PTEID_Photo(const PTEID_Photo &photo);			  /**< Copy not allowed - not implemented */
	PTEID_Photo &operator=(const PTEID_Photo &photo); /**< Copy not allowed - not implemented */
};

class APLPublicKey;

enum class PTEID_ECC_CurveIdentifier { NOT_ECC_CURVE, NIST_P256, NIST_P384, NIST_P521 };

/**
   Class that represents the card authentication public key - to be used in a mutual authentication process (1024 bit
   RSA key) This key is totally unrelated to the citizen authentication key which is present in the authentication
   certificate
*/

class PTEID_PublicKey : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_PublicKey(); /**< Destructor */

	PTEIDSDK_API PTEID_ByteArray &
	getCardAuthKeyModulus(); /**< Return the RSA public key module if this object contains an RSA public key */
	PTEIDSDK_API PTEID_ByteArray &
	getCardAuthKeyExponent(); /**< Return the RSA public key exponent if this object contains an RSA public key */
	PTEIDSDK_API bool isECCPublicKey(); /**< Return true if this object contains an Elliptic Curve public key instead of RSA */
    /** Return the EC public key data in uncompressed format. This is only applicable for objects coming from PTEID_CARDTYPE_IAS5 cards. */
	PTEIDSDK_API PTEID_ByteArray &getCardAuthECCKey(); 	
    PTEIDSDK_API PTEID_ECC_CurveIdentifier getECCCurveIdentifier();

	NOEXPORT_PTEIDSDK PTEID_PublicKey(const SDK_Context *context, const APLPublicKey &impl);

private:
	PTEID_PublicKey(const PTEID_PublicKey &cardKey);			/**< Copy not allowed - not implemented */
	PTEID_PublicKey &operator=(const PTEID_PublicKey &cardKey); /**< Copy not allowed - not implemented */
};

/**
 * This define give an easy access to the PTEID_ReaderSet singleton (no declaration/instantiation is needed).
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
 * THIS MUST BE CALLED WHEN THE SDK IS NOT NEEDED ANYMORE AND BEFORE THE CALLING APPLICATION EXITS.
 * IF NOT RELEASE PROPERLY, AN EXCEPTION PTEID_ExReleaseNeeded IS THROWN.
 */
#define PTEID_ReleaseSDK() PTEID_ReaderSet::releaseSDK()

class PTEID_ReaderContext;
class APL_ReaderContext;

/**
 * This is a singleton class that is the starting point to get all other objects.
 * You get an instance from the static instance() method (or using the define ReaderSet).
 * Then you get a READER							(PTEID_ReaderContext)
 *		-> from this reader, you a CARD				(PTEID_Card or derived class)
 *			-> from this card, you get identity or address objects	(PTEID_EId or PTEID_Address)
 *				-> ...
 */
class PTEID_ReaderSet : public PTEID_Object {
public:
	PTEIDSDK_MULTIPASS_API static PTEID_ReaderSet &instance(); /**< Return the singleton object (create it at first use) */

	/**
	 * Init the SDK (Optional).
	 * @param bManageTestCard If true the applayer must ask if test cards are allowed (used for compatibility with old C
	 *API).
	 * @param bManageTestCard If false other applications (ex. gui) take that into their scope
	 **/
	PTEIDSDK_MULTIPASS_API static void initSDK(bool bManageTestCard = false);
#if !defined SWIG
	PTEIDSDK_MULTIPASS_API static void initSDKWithCallbacks(const PTEID_CardInterfaceCallbacks &callbacks, bool bManageTestCard = false);
#endif
	PTEIDSDK_MULTIPASS_API static void releaseSDK(); /**< Release the SDK */

	PTEIDSDK_MULTIPASS_API virtual ~PTEID_ReaderSet(); /**< Destructor */

	/**
	 * Release the readers (Useful if readers had changed).
	 *
	 * @param bAllReference If true all the invalid reference/pointer are destroyed.
	 * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	 * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	 */
	PTEIDSDK_MULTIPASS_API void releaseReaders(bool bAllReference = false);

	/**
	 * Return true if readers has been added or removed
	 */
	PTEIDSDK_MULTIPASS_API bool isReadersChanged() const;

	/**
	 * Get the list of the reader.
	 * Return an array of const char *
	 * The last pointer is NULL
	 * Usage : const char * const *ppList=PTEID_ReaderSet::readerList();
	 *         for(const char * const *ppName=ppList;*ppName!=NULL;ppName++) {...}
	 *
	 * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	 **/
	PTEIDSDK_MULTIPASS_API const char *const *readerList(bool bForceRefresh = false);

	/**
	 * Return the first readercontext with a card.
	 *		If no card is present, return the firs reader.
	 *		If no reader exist, throw an exception PTEID_ExNoReader.
	 **/
	PTEIDSDK_MULTIPASS_API PTEID_ReaderContext &getReader();

	/**
	 * Get the reader by its name.
	 */
	PTEIDSDK_MULTIPASS_API PTEID_ReaderContext &getReaderByName(const char *readerName);

	/**
	 * Return the number of card readers connected to the computer.
	 *
	 * @param bForceRefresh force the reconnection to the lower layer to see if reader list have changed
	 **/
	PTEIDSDK_MULTIPASS_API unsigned long readerCount(bool bForceRefresh = false);

	/**
	 * Get the name of the reader by its Index.
	 * Throw PTEID_ExParamRange exception if the index is out of range.
	 **/
	PTEIDSDK_MULTIPASS_API const char *getReaderName(unsigned long ulIndex);

	/**
	 * Get the reader by its Index.
	 * Throw PTEID_ExParamRange exception if the index is out of range.
	 **/
	PTEIDSDK_MULTIPASS_API PTEID_ReaderContext &getReaderByNum(unsigned long ulIndex);

	/**
	 * Return the reader containing the card with this SN.
	 *		If no card with this SN is found, throw an exception PTEID_ExParamRange.
	 **/
	PTEIDSDK_MULTIPASS_API PTEID_ReaderContext &getReaderByCardSerialNumber(const char *cardSerialNumber);

	/**
	 * Flush the cached files.
	 * Return if any files were flushed (T/F).
	 */
	;
	PTEIDSDK_API bool flushCache(); /**< Flush the cache */

	/**
	 * For internal use - Not exported
	 */
	PTEIDSDK_MULTIPASS_API PTEID_ReaderContext &getReader(APL_ReaderContext *pAplReader);

private:
	PTEID_ReaderSet(); /**< For internal use : Constructor */

	PTEID_ReaderSet(const PTEID_ReaderSet &reader);			   /**< Copy not allowed - not implemented */
	PTEID_ReaderSet &operator=(const PTEID_ReaderSet &reader); /**< Copy not allowed - not implemented */
};

class PTEID_Card;
class PTEID_EIDCard;
class ICAO_Card;

/**
 * This class represent a reader.
 * You get a reader object from the ReaderSet
 *		either by its index (getReaderByNum) or by its name (getReaderByName).
 * Once you have a reader object, you can check if a card is present (isCardPresent).
 * Then you can ask which type of card is in the reader with getCardType()
 *	and then get a card object using one of this methods :
 *	getCard or getEIDCard.
 */
class PTEID_ReaderContext : public PTEID_Object {
public:
	PTEIDSDK_MULTIPASS_API virtual ~PTEID_ReaderContext(); /**< Destructor */

	/**
	 * Return the name of the reader.
	 */
	PTEIDSDK_MULTIPASS_API const char *getName();

	/**
	 * Return true if a card is present and false otherwise.
	 */
	PTEIDSDK_MULTIPASS_API bool isCardPresent();

	PTEIDSDK_API bool isPinpad();

	/**
	 * Release the card.
	 *
	 * @param bAllReference If true all the invalid reference/pointer are destroyed.
	 * @param bAllReference	PUT THIS PARAMETER TO TRUE IS THREAD UNSAFE.
	 * @param bAllReference	You have to be sure that you will not use any old reference/pointer after this release
	 */
	PTEIDSDK_API void releaseCard(bool bAllReference = false);

	/**
	 * Return true if a card has changed since the last called (with the same ulOldId parameter).
	 */
	PTEIDSDK_API bool isCardChanged(unsigned long &ulOldId);

	/**
	 * Return the type of the card in the reader.
	 *
	 * Throw PTEID_ExNoCardPresent exception if no card is present.
	 **/
	PTEIDSDK_API PTEID_CardType getCardType();

	/**
	 * Get the card in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(PTEID_EIDCard).
	 *
	 * If no card is present in the reader, exception PTEID_ExNoCardPresent is thrown.
	 * If the card type is not supported, exception PTEID_ExCardTypeUnknown is thrown.
	 **/
	PTEIDSDK_API PTEID_Card &getCard();

	/**
	 * Get the card interface used on the current used card
	 *
	 * Returns the interface of the current card
	 **/
	PTEIDSDK_API PTEID_CardContactInterface getCardContactInterface();

	/**
	 * Get the EIDcard in the reader.
	 *		Instantiation is made regarding the type of the card
	 *			(PTEID_EIDCard).
	 *
	 * If no card is present in the reader, exception PTEID_ExNoCardPresent is thrown.
	 * If the card type is not supported, exception PTEID_ExCardTypeUnknown is thrown.
	 **/
	PTEIDSDK_API PTEID_EIDCard &getEIDCard();

	PTEIDSDK_API ICAO_Card &getICAOCard();

	/**
     * Read the Multipass token information from the card present in the reader
     *
     * This token is a free access unique value, not related to personal citizen data.
     * Its purpose is to be used in some physical access control scenarios.
     * This is the only useful information available on IAS v5 cards without access control on contactless interface.
     *
     * The implementation uses Passive and Chip Authentication mechanisms defined in ICAO Doc 9303 Part 11 to counter the risk of receiving untrusted or cloned information.
     *
     * @return A PTEID_ByteArray object containing the Multipass token which is a 16-byte value
     */ 
	PTEIDSDK_MULTIPASS_API PTEID_ByteArray getMultiPassToken();

	/**
	 * Specify a callback function to be called each time a
	 * card is inserted/remove in/from this reader.
	 *
	 * @return A handle can be used to stop the callbacks when they are no longer needed.
	 **/
	PTEIDSDK_API unsigned long SetEventCallback(void (*callback)(long lRet, unsigned long ulState, void *pvRef),
												void *pvRef);

	/**
	 * To tell that the callbacks are not longer needed.
	 * @param ulHandle is the handle return by SetEventCallback
	 **/
	PTEIDSDK_API void StopEventCallback(unsigned long ulHandle);

	PTEIDSDK_API void BeginTransaction(); /**< Begin a transaction with the reader */
	PTEIDSDK_API void EndTransaction();	  /**< End the transaction */

private:
	PTEID_ReaderContext(const PTEID_ReaderContext &reader);			   /**< Copy not allowed - not implemented */
	PTEID_ReaderContext &operator=(const PTEID_ReaderContext &reader); /**< Copy not allowed - not implemented */

	PTEID_ReaderContext(const SDK_Context *context, APL_ReaderContext *impl); /**< For internal use : Constructor */

	unsigned long m_cardid;
	unsigned long m_cardIcaoId;
	// CMutex *m_mutex;

	friend PTEID_ReaderContext &PTEID_ReaderSet::getReader(
		APL_ReaderContext *pAplReader); /**< For internal use : This method must access protected constructor */
};

class PTEID_PDFSignature;
class APL_Card;
class APL_ICAO;
class PTEID_Certificate;
class PTEID_Certificates;

/**
 * Interface class for objects with Signing capabilities.
 * @since 3.8.0
 */
class PTEID_SigningDevice {
public:
	/**
	 * Raw RSA signature with PKCS #1 padding.
	 * @param data holds the data to be signed, at most 32 bytes. This data should be hashed using sha256.
	 * @param signatureKey whether to use the 'Signature key'. By default, it uses the 'Authentication private key'. To
	 * sign with the 'Signature private key' set the parameter signatureKey to @b true.
	 * @return A PTEID_ByteArray containing the signed data.
	 */
	PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray &data, bool signatureKey = false) = 0;

	/**
	 * Raw RSA signature with PKCS #1 padding (applied to a SHA256 hash).
	 * @param data holds the data to be signed, it should be 32 bytes.
	 * @param signatureKey whether to use the 'Signature key'. By default, it uses the 'Authentication private key'. To
	 * sign with the 'Signature private key' set the parameter signatureKey to @b true.
	 * @return A PTEID_ByteArray containing the signed data.
	 * @deprecated This method is now deprecated. Use PTEID_SigningDevice::Sign method instead, which already supports
	 * SHA256.
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignSHA256(const PTEID_ByteArray &data, bool signatureKey = false) = 0;

	/**
	 * Produce a XAdES Signature of the files indicated by the parameter @e paths and stores the results in one ASiC
	 *container in a zip format. The location of the resulting ASiC container is indicated by the parameter @e
	 *output_path.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 *signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path points to the resulting container
	 * @param level is an enum used to set the XAdES signature level/profile: XAdES-B (PTEID_LEVEL_BASIC), XAdES-T
	 *(PTEID_LEVEL_T), XAdES-LTA (PTEID_LEVEL_LTV). Note: Setting parameter level to PTEID_LEVEL_LT will throw
	 *PTEID_Exception(EIDMW_ERR_PARAM_BAD), as it is not supported yet.
	 **/
	PTEIDSDK_API virtual PTEID_ByteArray SignXades(const char *output_path, const char *const *paths,
												   unsigned int n_paths,
												   PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) = 0;

	/**
	 * Produce a XAdES-T Signature of the files indicated by the parameter @e paths and stores the results in one ASiC
	 * container in a zip format. The location of the resulting ASiC container is indicated by the parameter @e
	 * output_path. If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the resulting file is a XAdES-B signature.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 * signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path points to the resulting container
	 * @deprecated use the SignXades function and set signature level with level (PTEID_SignatureLevel) parameter
	 * instead
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesT(const char *output_path, const char *const *paths,
													unsigned int n_paths) = 0;

	/**
	 * Produce a XAdES-LTA Signature of the files indicated by the parameter @e paths and stores the results in one ASiC
	 * container in a zip format. The location of the resulting ASiC container is indicated by the parameter @e
	 * output_path. If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the resulting file is a XAdES-B signature. If
	 * PTEID_Exception(EIDMW_LTV_ERROR) is thrown, the resulting file is a XAdES-LT or XAdES-LTA depending where the
	 * timestamping fails.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 * signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path points to the resulting container
	 * @deprecated use the SignXades function and set signature level with level (PTEID_SignatureLevel) parameter
	 * instead
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesA(const char *output_path, const char *const *paths,
													unsigned int n_paths) = 0;

	/**
	 * Produce XAdES-B Signatures of the files indicated by the parameter @e paths and stores each of the results in an
	 *individual ASiC container in a zip format.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 *signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path directory of the created ASiC containers
	 **/
	PTEIDSDK_API virtual void SignXadesIndividual(const char *output_path, const char *const *paths,
												  unsigned int n_paths) = 0;

	/**
	 * Produce XAdES-T Signatures of the files indicated by the parameter @e paths and stores each of the results in an
	 *individual ASiC container in a zip format. If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the resulting file
	 *is a XAdES-B signature.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 *signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path directory of the created ASiC containers
	 **/
	PTEIDSDK_API virtual void SignXadesTIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) = 0;

	/**
	 * Produce XAdES-A Signatures of the files indicated by the parameter @e paths and stores each of the results in an
	 *individual ASiC container in a zip format. If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the resulting file
	 *is a XAdES-B signature. If PTEID_Exception(EIDMW_LTV_ERROR) is thrown, the resulting file is a XAdES-LT or
	 *XAdES-LTA depending where the timestamping fails.
	 *
	 * @param paths is an array of null-terminated strings representing absolute paths in
	 * the local filesystem. Those files content (hashed with SHA-256 algorithm) will be the input data for the RSA
	 *signature
	 * @param n_paths is the number of elements in the @e paths array
	 * @param output_path directory of the created ASiC containers
	 **/
	PTEIDSDK_API virtual void SignXadesAIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) = 0;

	/*
	 * Add a XAdES signature to the ASIC container at @path. The signature profile is specified in @level parameter
	 */
	PTEIDSDK_API virtual void SignASiC(const char *path, PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) = 0;

	/**
	 * PDF Signature with location by page sector (the portrait A4 page is split into 18 cells: 6 lines and 3 columns)
	 * If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the resulting file is a PAdES-B signature.
	 * If PTEID_Exception(EIDMW_LTV_ERROR) is thrown, the resulting file is a PAdES-LT or PAdES-LTA depending where the
	 *timestamping fails.
	 *
	 * @param sig_handler: this defines the input file and some signature options
	 * @param page: in case of visible signature it defines the page where the signature will appear
	 * @param page_sector: position in the signature grid, between 1 to 18 for Portrait documents and 1 to 20 for
	 *Landscape ones
	 * @param is_landscape: is unused parameter, the SDK now detects document orientation automatically
	 * @param location: Signature metadata field
	 * @param reason: Signature metadata field
	 * @param outfile_path: Native Filesystem path of the ouput file
	 * @deprecated use the SignPDF function with location coordinates instead
	 **/
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape,
									 const char *location, const char *reason, const char *outfile_path) = 0;

	/**
	 * PDF Signature with location by coordinates (expressed in percentage of page height/width). The coordinate system
	 *has its origin in the top left corner of the page If PTEID_Exception(EIDMW_TIMESTAMP_ERROR) is thrown, the
	 *resulting file is a PAdES-B signature. If PTEID_Exception(EIDMW_LTV_ERROR) is thrown, the resulting file is a
	 *PAdES-LT or PAdES-LTA depending where the timestamping fails.
	 *
	 * @param sig_handler: this defines the input file and some signature options
	 * @param page: in case of visible signature it defines the page where the signature will appear
	 * @param coord_x: X coordinate of the signature location (percentage of page width)
	 * @param coord_y: Y coordinate of the signature location (percentage of page height)
	 * @param location: Location field in the added signature metadata
	 * @param reason: Signature metadata field
	 * @param outfile_path: Native Filesystem path of the ouput file
	 **/
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y,
									 const char *location, const char *reason, const char *outfile_path) = 0;

	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificates &getCertificates() = 0;
	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificate &getCert(PTEID_CertifType type) = 0;
	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificate &getRoot() = 0;
	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificate &getCA() = 0;
	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificate &getSignature() = 0;
	/** @deprecated Use getCertificates() to obtain the certificates and obtain the desired certificate/build the chain
	 * manually.*/
	PTEIDSDK_API virtual PTEID_Certificate &getAuthentication() = 0;

	/**
	 * Get type of SigningDeviceType.
	 */
	PTEIDSDK_API virtual PTEID_SigningDeviceType getDeviceType() = 0;
};

class EIDMW_DocumentReport;
struct EIDMW_ActiveAuthenticationReport;
struct EIDMW_ChipAuthenticationReport;
struct EIDMW_SodReport;
struct EIDMW_DataGroupReport;

class PTEID_ActiveAuthenticationReport : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray GetDG14() const;
	PTEIDSDK_API PTEID_ByteArray GetDG14StoredHash() const;
	PTEIDSDK_API PTEID_ByteArray GetDG14ComputedHash() const;

	PTEIDSDK_API PTEID_ByteArray GetDG15() const;
	PTEIDSDK_API PTEID_ByteArray GetDG15StoredHash() const;
	PTEIDSDK_API PTEID_ByteArray GetDG15ComputedHash() const;

	PTEIDSDK_API const char *GetOID() const;

	PTEIDSDK_API long GetStatus() const;
	PTEIDSDK_API const char *GetStatusMessage() const;

private:
	const EIDMW_ActiveAuthenticationReport &m_impl;
	mutable std::string m_statusMessage;
	friend class ICAO_Card;
	friend class PTEID_DocumentReport;

	PTEID_ActiveAuthenticationReport(const SDK_Context *context, const EIDMW_ActiveAuthenticationReport &report);
	PTEID_ActiveAuthenticationReport &
	operator=(const PTEID_ActiveAuthenticationReport &) = delete; /**< Copy not allowed - not implemented */
};

class PTEID_ChipAuthenticationReport : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray GetPublicKey() const;
	PTEIDSDK_API const char *GetOID() const;

	PTEIDSDK_API long GetStatus() const;
	PTEIDSDK_API const char *GetStatusMessage() const;

private:
	const EIDMW_ChipAuthenticationReport &m_impl;
	mutable std::string m_statusMessage;
	friend class ICAO_Card;
	friend class PTEID_DocumentReport;

	PTEID_ChipAuthenticationReport(const SDK_Context *context, const EIDMW_ChipAuthenticationReport &report);
	PTEID_ChipAuthenticationReport &
	operator=(const PTEID_ChipAuthenticationReport &) = delete; /**< Copy not allowed - not implemented */
};

class PTEID_SodReport : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray GetSigner() const;

	PTEIDSDK_API long GetStatus() const;
	PTEIDSDK_API const char *GetStatusMessage() const;

private:
	const EIDMW_SodReport &m_impl;
	mutable std::string m_statusMessage;
	friend class ICAO_Card;
	friend class PTEID_DocumentReport;

	PTEID_SodReport(const SDK_Context *context, const EIDMW_SodReport &report);
	PTEID_SodReport &operator=(const PTEID_SodReport &) = delete;
};

class PTEID_DataGroupReport : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray GetStoredHash() const;
	PTEIDSDK_API PTEID_ByteArray GetComputedHash() const;
	PTEIDSDK_API long GetStatus() const;
	PTEIDSDK_API const char *GetStatusMessage() const;

private:
	const EIDMW_DataGroupReport &m_impl;
	mutable std::string m_statusMessage;

	friend class ICAO_Card;
	friend class PTEID_BaseDGReport;
	friend class PTEID_DocumentReport;

	PTEID_DataGroupReport(const SDK_Context *context, const EIDMW_DataGroupReport &report);
	PTEID_DataGroupReport(const PTEID_DataGroupReport &) = delete;
	PTEID_DataGroupReport &operator=(const PTEID_DataGroupReport &) = delete;
};

class PTEID_DocumentReport : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ActiveAuthenticationReport *GetActiveAuthenticationReport() const;
	PTEIDSDK_API PTEID_ChipAuthenticationReport *GetChipAuthenticationReport() const;
	PTEIDSDK_API PTEID_SodReport *GetSodReport() const;
	PTEIDSDK_API PTEID_DataGroupReport *GetDataGroupReport(PTEID_DataGroupID tag) const;

private:
	const EIDMW_DocumentReport &m_impl;
	friend class ICAO_Card;
	friend class PTEID_BaseDGReport;

	PTEID_DocumentReport(const SDK_Context *context, const EIDMW_DocumentReport &reports);
	PTEID_DocumentReport(const PTEID_DocumentReport &) = delete;			/**< Copy not allowed - not implemented */
	PTEID_DocumentReport &operator=(const PTEID_DocumentReport &) = delete; /**< Copy not allowed - not implemented */
};

class PTEID_BaseDGReport : public PTEID_Object {
public:
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const = 0;
	PTEIDSDK_API PTEID_DocumentReport *GetDocumentReport() const;

protected:
	PTEIDSDK_API const PTEID_DataGroupReport *GetReportByID(PTEID_DataGroupID id) const;

	const EIDMW_DocumentReport &m_documentReport;
	PTEID_BaseDGReport(const SDK_Context *context, const EIDMW_DocumentReport &report);
	PTEID_BaseDGReport(const PTEID_BaseDGReport &) = delete;
	PTEID_BaseDGReport &operator=(const PTEID_BaseDGReport &) = delete;
};

class PTEID_RawDataGroup : public PTEID_BaseDGReport {
public:
	PTEIDSDK_API PTEID_ByteArray GetData() const;
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const;

private:
	friend class ICAO_Card;

	PTEID_ByteArray m_data;
	PTEID_DataGroupID m_id;

	PTEID_RawDataGroup(const SDK_Context *context, PTEID_DataGroupID id, PTEID_ByteArray data,
					   const EIDMW_DocumentReport &report);
	PTEID_RawDataGroup(const PTEID_RawDataGroup &) = delete;
	PTEID_RawDataGroup &operator=(const PTEID_RawDataGroup &) = delete;
};

class IcaoDg1;
class PTEID_ICAO_DG1 : public PTEID_BaseDGReport {
public:
	PTEIDSDK_API const char *documentCode() const;
	PTEIDSDK_API const char *issuingState() const;
	PTEIDSDK_API const char *documentNumber() const;
	PTEIDSDK_API int serialNumberCheckDigit() const;
	PTEIDSDK_API const char *optionalDataLine1() const;
	PTEIDSDK_API const char *dateOfBirth() const;
	PTEIDSDK_API char sex() const;
	PTEIDSDK_API const char *dateOfExpiry() const;
	PTEIDSDK_API const char *nationality() const;
	PTEIDSDK_API const char *optionalDataLine2() const;
	PTEIDSDK_API const char *primaryIdentifier() const;
	PTEIDSDK_API const char *secondaryIdentifier() const;
	PTEIDSDK_API bool isPassport() const;
	PTEIDSDK_API ~PTEID_ICAO_DG1();
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const;

private:
	const IcaoDg1 &m_impl;
	friend class ICAO_Card;
	PTEID_ICAO_DG1(const SDK_Context *context, const IcaoDg1 &dg1, const EIDMW_DocumentReport &documentReport);
	PTEID_ICAO_DG1(const PTEID_ICAO_DG1 &);						/**< Copy not allowed - not implemented */
	PTEID_ICAO_DG1 &operator=(const PTEID_ICAO_DG1 &) = delete; /**< Copy not allowed - not implemented */
};

class IcaoDg2;
class BiometricInformation;
class FaceInfo;
class FaceInfoData;
class FeaturePoint;
class PTEID_FeaturePoint {
public:
	PTEIDSDK_API unsigned char type() const;
	PTEIDSDK_API unsigned char featurePoint() const;
	PTEIDSDK_API unsigned char majorCode() const;
	PTEIDSDK_API unsigned char minorCode() const;
	PTEIDSDK_API unsigned short x_coord() const;
	PTEIDSDK_API unsigned short y_coord() const;
	PTEIDSDK_API unsigned short reserved() const;

private:
	friend class PTEID_FaceInfoData;
	PTEID_FeaturePoint(FeaturePoint &featurePoint);
	PTEID_FeaturePoint &operator=(const PTEID_FeaturePoint &) = delete; /**< Copy not allowed - not implemented */
	FeaturePoint &m_impl;
};

class PTEID_FaceInfoData : public PTEID_Object {
public:
	PTEIDSDK_API long facialRecordDataLength() const;
	PTEIDSDK_API unsigned short numberOfFeaturePoints() const;
	PTEIDSDK_API unsigned char gender() const;
	PTEIDSDK_API PTEID_Gender genderDecode() const;
	PTEIDSDK_API unsigned char eyeColor() const;
	PTEIDSDK_API PTEID_EyeColor eyeColorDecode() const;
	PTEIDSDK_API unsigned char hairColour() const;
	PTEIDSDK_API PTEID_HairColour hairColourDecode() const;
	PTEIDSDK_API long featureMask() const;
	PTEIDSDK_API long expression() const;
	PTEIDSDK_API long poseAngle() const;
	PTEIDSDK_API long poseAngleUncertainty() const;
	PTEIDSDK_API std::vector<PTEID_FeaturePoint *> featurePoints() const;
	PTEIDSDK_API unsigned char faceImgType() const;
	PTEIDSDK_API PTEID_FaceImageType faceImgTypeDecode() const;
	PTEIDSDK_API unsigned char imgDataType() const;
	PTEIDSDK_API PTEID_ImageDataType imgDataTypeDecode() const;
	PTEIDSDK_API unsigned short imgWidth() const;
	PTEIDSDK_API unsigned short imgHeight() const;
	PTEIDSDK_API unsigned char colourSpace() const;
	PTEIDSDK_API PTEID_ImageColourSpace colourSpaceDecode() const;
	PTEIDSDK_API unsigned char sourceType() const;
	PTEIDSDK_API PTEID_SourceType sourceTypeDecode() const;
	PTEIDSDK_API unsigned short deviceType() const;
	PTEIDSDK_API unsigned short quality() const;
	PTEIDSDK_API PTEID_ByteArray photoRawData() const;
	PTEIDSDK_API PTEID_ByteArray photoRawDataPNG() const;

private:
	friend class PTEID_FaceInfo;
	PTEID_FaceInfoData(const SDK_Context *context, FaceInfoData &data);
	~PTEID_FaceInfoData();
	PTEID_FaceInfoData &operator=(const PTEID_FaceInfoData &) = delete; /**< Copy not allowed - not implemented */
	FaceInfoData &m_impl;
	std::vector<PTEID_FeaturePoint *> m_featurePoints;
};

class PTEID_FaceInfo : public PTEID_Object {
public:
	PTEIDSDK_API const char *version() const;
	PTEIDSDK_API unsigned short encodingBytes() const;
	PTEIDSDK_API long sizeOfRecord() const;
	PTEIDSDK_API long numberOfFacialImages() const;
	PTEIDSDK_API std::vector<PTEID_FaceInfoData *> faceInfoData() const;

private:
	friend class PTEID_ICAO_DG2;
	friend class PTEID_BiometricInfomation;
	PTEID_FaceInfo(const SDK_Context *context, FaceInfo &face);
	~PTEID_FaceInfo();
	PTEID_FaceInfo &operator=(const PTEID_FaceInfo &) = delete; /**< Copy not allowed - not implemented */
	FaceInfo &m_impl;
	std::vector<PTEID_FaceInfoData *> m_faceInfoDataVec;
};

class PTEID_BiometricInfomation : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray icaoHeaderVersion() const;
	PTEIDSDK_API PTEID_ByteArray type() const;
	PTEIDSDK_API PTEID_ByteArray subType() const;
	PTEIDSDK_API PTEID_ByteArray creationDateAndtime() const;
	PTEIDSDK_API PTEID_ByteArray validPeriod() const;
	PTEIDSDK_API PTEID_ByteArray creatorOfBiometricRefData() const;
	PTEIDSDK_API PTEID_ByteArray formatOwner() const;
	PTEIDSDK_API PTEID_ByteArray formatType() const;
	PTEIDSDK_API PTEID_FaceInfo *faceInfo() const;

private:
	friend class PTEID_ICAO_DG2;
	PTEID_BiometricInfomation(const SDK_Context *context, BiometricInformation &bioInfo);
	~PTEID_BiometricInfomation();
	PTEID_FaceInfo *m_faceInfo;
	PTEID_BiometricInfomation &
	operator=(const PTEID_BiometricInfomation &) = delete; /**< Copy not allowed - not implemented */
	BiometricInformation &m_impl;
};

class PTEID_ICAO_DG2 : public PTEID_BaseDGReport {
public:
	PTEIDSDK_API unsigned int numberOfBiometrics() const;
	PTEIDSDK_API std::vector<PTEID_BiometricInfomation *> biometricInstances();
	PTEIDSDK_API ~PTEID_ICAO_DG2();
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const;

private:
	const IcaoDg2 &m_impl;
	std::vector<PTEID_BiometricInfomation *> m_biometricInstances; // delete this on destructor
	friend class ICAO_Card;
	PTEID_ICAO_DG2(const SDK_Context *context, const IcaoDg2 &dg2, const EIDMW_DocumentReport &report);
	PTEID_ICAO_DG2(const PTEID_ICAO_DG2 &);						/**< Copy not allowed - not implemented */
	PTEID_ICAO_DG2 &operator=(const PTEID_ICAO_DG2 &) = delete; /**< Copy not allowed - not implemented */
};
class IcaoDg3;
class BiometricInfoImage;
class PTEID_BiometricInfoFingerImage : public PTEID_Object {
public:
	PTEIDSDK_API unsigned int length() const;
	PTEIDSDK_API unsigned int fingerPalmPosition() const;
	PTEIDSDK_API unsigned int countOfViews() const;
	PTEIDSDK_API unsigned int viewMumber() const;
	PTEIDSDK_API unsigned int quality() const;
	PTEIDSDK_API unsigned int impressionType() const;
	PTEIDSDK_API unsigned short horizontalLineLength() const;
	PTEIDSDK_API unsigned short verticalLineLength() const;
	PTEIDSDK_API unsigned char reserved() const;
	PTEIDSDK_API PTEID_ByteArray imageData() const;

private:
	friend class PTEID_BiometricInfomationDg3;
	const BiometricInfoImage &m_impl;
	PTEID_BiometricInfoFingerImage(const SDK_Context *context, const BiometricInfoImage &bioInfo);
	PTEID_BiometricInfoFingerImage(const PTEID_BiometricInfoFingerImage &);
	PTEID_BiometricInfoFingerImage &operator=(const PTEID_BiometricInfoFingerImage &) = delete;
};

class BiometricInfoDG3;
class PTEID_BiometricInfomationDg3 : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_ByteArray icaoHeaderVersion() const;
	PTEIDSDK_API PTEID_ByteArray type() const;
	PTEIDSDK_API PTEID_ByteArray subType() const;
	PTEIDSDK_API PTEID_ByteArray creationDateAndtime() const;
	PTEIDSDK_API PTEID_ByteArray validPeriod() const;
	PTEIDSDK_API PTEID_ByteArray creatorOfBiometricRefData() const;
	PTEIDSDK_API PTEID_ByteArray formatOwner() const;
	PTEIDSDK_API PTEID_ByteArray formatType() const;

	PTEIDSDK_API const char *specVersion() const;
	PTEIDSDK_API unsigned long long recordLength() const;
	PTEIDSDK_API unsigned short scannerId() const;
	PTEIDSDK_API unsigned short imageAcquisitionLevel() const;
	PTEIDSDK_API unsigned int numFingersOrPalmImages() const;
	PTEIDSDK_API unsigned int scaleUnits() const;
	PTEIDSDK_API unsigned short xScanResolution() const;
	PTEIDSDK_API unsigned short yScanResolution() const;
	PTEIDSDK_API unsigned short xImageResolution() const;
	PTEIDSDK_API unsigned short yImageResolution() const;
	PTEIDSDK_API unsigned int pixelDepth() const;
	PTEIDSDK_API unsigned int imageCompressionAlgorithm() const;
	PTEIDSDK_API unsigned short reserved() const;

private:
	const BiometricInfoDG3 &m_impl;
	friend class PTEID_ICAO_DG3;
	PTEID_BiometricInfomationDg3(const SDK_Context *context, const BiometricInfoDG3 &bioInfo);
	PTEID_BiometricInfomationDg3(const PTEID_BiometricInfomationDg3 &);
	~PTEID_BiometricInfomationDg3();
	PTEID_BiometricInfomationDg3 &operator=(const PTEID_BiometricInfomationDg3 &) = delete;
	std::vector<PTEID_BiometricInfoFingerImage *> m_bioFingerImageVec;
};

class PTEID_ICAO_DG3 : public PTEID_BaseDGReport {
public:
	PTEIDSDK_API ~PTEID_ICAO_DG3();
	PTEIDSDK_API unsigned int numberOfbiometrics() const;
	PTEIDSDK_API std::vector<PTEID_BiometricInfomationDg3 *> biometricInformation() const;
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const;

private:
	const IcaoDg3 &m_impl;

	friend class ICAO_Card;
	PTEID_ICAO_DG3(const SDK_Context *context, const IcaoDg3 &dg3, const EIDMW_DocumentReport &report);
	PTEID_ICAO_DG3(const PTEID_ICAO_DG3 &);						/**< Copy not allowed - not implemented */
	PTEID_ICAO_DG3 &operator=(const PTEID_ICAO_DG3 &) = delete; /**< Copy not allowed - not implemented */
	std::vector<PTEID_BiometricInfomationDg3 *> m_biometricInformation;
};

class IcaoDg11;
class PTEID_ICAO_DG11 : public PTEID_BaseDGReport {
public:
	PTEIDSDK_API PTEID_ByteArray listOfTags() const;
	PTEIDSDK_API const char *fullName() const;
	PTEIDSDK_API const char *personalNumber() const;
	PTEIDSDK_API const char *fullDateOfBirth() const;
	PTEIDSDK_API const char *placeOfBirth() const;
	PTEIDSDK_API const char *permanentAddress() const;
	PTEIDSDK_API const char *telephone() const;
	PTEIDSDK_API const char *profession() const;
	PTEIDSDK_API const char *title() const;
	PTEIDSDK_API const char *personalSummary() const;
	PTEIDSDK_API const char *proofOfCitizenship() const;
	PTEIDSDK_API const char *otherValidTDNumbers() const;
	PTEIDSDK_API const char *custodyInformation() const;
	PTEIDSDK_API int numberOfOtherNames() const;
	PTEIDSDK_API const char *otherNames() const;
	PTEIDSDK_API virtual ~PTEID_ICAO_DG11();
	PTEIDSDK_API virtual const PTEID_DataGroupReport *GetReport() const;

private:
	const IcaoDg11 &m_impl;
	friend class ICAO_Card;
	PTEID_ICAO_DG11(const SDK_Context *context, const IcaoDg11 &dg11, const EIDMW_DocumentReport &report);
	PTEID_ICAO_DG11(const PTEID_ICAO_DG11 &);					  /**< Copy not allowed - not implemented */
	PTEID_ICAO_DG11 &operator=(const PTEID_ICAO_DG11 &) = delete; /**< Copy not allowed - not implemented */
};

class ICAO_Card : public PTEID_Object {
public:
	PTEIDSDK_API virtual std::vector<PTEID_DataGroupID> getAvailableDatagroups();

	PTEIDSDK_API virtual PTEID_DocumentReport *GetDocumentReport() const;

	PTEIDSDK_API virtual void initPaceAuthentication(const char *secret, size_t length,
													 PTEID_CardPaceSecretType secretType);

	/* @param: mrz_info must contain the full MRZInfo needed for BAC authentication:
	           i.e. document number + date of birth + expiry date, all 3 fields including their respective check digits */
	PTEIDSDK_API virtual void initBACAuthentication(const char *mrz_info);

	/**
	 * Read raw data from datagroup specified in @tag parameter
	 */
	PTEIDSDK_API virtual PTEID_RawDataGroup *readDatagroupRaw(PTEID_DataGroupID tag);

	PTEIDSDK_API virtual PTEID_ICAO_DG1 *readDataGroup1();
	PTEIDSDK_API virtual PTEID_ICAO_DG2 *readDataGroup2();
	PTEIDSDK_API virtual PTEID_ICAO_DG3 *readDataGroup3();

	PTEIDSDK_API virtual PTEID_ICAO_DG11 *readDataGroup11();

	/**
	 * Load a certificate MasterList containing CSCA certificates. These are needed for certificate validation during
	 Passive Authentication This method needs to be called before readDataGroupRaw() or any other readDataGroup* method
	 */
	PTEIDSDK_API virtual void loadMasterList(const char *filePath);
	/**
	 * Reset the card state, discarding any Secure Messaging session. This is useful for a multi-application card like
	 * PT eID v2 to be able to access other applications afterwards Other methods of ICAO_Card will fail after this
	 * call!
	 */
	PTEIDSDK_API virtual void resetCardState();

protected:
	ICAO_Card(const SDK_Context *context, APL_ICAO *impl); /**< For internal use : Constructor */

private:
	ICAO_Card(const ICAO_Card &card) = delete;			  /**< Copy not allowed - not implemented */
	ICAO_Card &operator=(const ICAO_Card &card) = delete; /**< Copy not allowed - not implemented */

	friend ICAO_Card &
	PTEID_ReaderContext::getICAOCard(); /**< For internal use : This method must access protected constructor */
};

/**
 * Abstract base class for all the card types supported.
 * PTEID_ReaderContext::getCard() method
 * will return such an object.
 */
class PTEID_Card : public PTEID_Object, public PTEID_SigningDevice {
public:
	PTEIDSDK_API virtual ~PTEID_Card() = 0; /**< Destructor */

	/**
	 * Return the type of the card
	 */
	PTEIDSDK_API virtual PTEID_CardType getType();

	/**
	 * Send an APDU command to the card and get the result.
	 * @param cmd is the apdu command
	 * @return A PTEID_ByteArray containing the result
	 */
	PTEIDSDK_API virtual PTEID_ByteArray sendAPDU(const PTEID_ByteArray &cmd);

	/**
	 * Signs a block of data using RSA-PKCS#1 using cards of type PTEID_CARDTYPE_IAS07 or ECDSA using newer cards of type PTEID_CARDTYPE_IAS5
	 * In the ECDSA case the signature is returned in IEEE P1363 format, i.e. the signature is the concatenation of r and s values generated by the algorithm.
	 * In some application contexts encoding the signature into the ASN.1 format described in RFC-5480 may be needed.
	 *
	 * @param data block of data to be signed. Has to be hashed using either sha1, sha256, sha384 or sha512. Therefore,
	 * size of data array has to be at max 64 bytes.
	 * @param signatureKey by default uses the 'Authentication private key' to sign message. Setting this to @b true
	 * makes use of 'Signature private key' instead.
	 * @return PTEID_ByteArray containing the signature
	 */
	PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray &data, bool signatureKey = false) override;

	/**
	 * @copydoc PTEID_SigningDevice::SignSHA256
	 * @deprecated Use PTEID_Card::Sign instead.
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignSHA256(const PTEID_ByteArray &data, bool signatureKey = false) override;

	/**
	 * Signs a block of data using RSA-PKCS#1, RSA-PSS or ECDSA. This method can be used to select the RSA padding algorithm to use with cards
	 * of type PTEID_CARDTYPE_IAS07. When using newer cards of type PTEID_CARDTYPE_IAS5 @paddingType parameter is ignored and an ECDSA signature is returned.
	 *
	 * @param data block of data to be signed. Has to be hashed using either sha1, sha256, sha384 or sha512. Therefore,
	 * data has to be at max 64 bytes.
	 * @param paddingType either RSA-PSS or RSA-PKCS#1
	 * @param signatureKey by default uses the 'Authentication private key' to sign message. Setting this to @b true
	 * makes use of 'Signature private key' instead.
	 * @return PTEID_ByteArray containing the signature
	 */
	PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray &data, PTEID_RSAPaddingType paddingType,
											  bool signatureKey = false);

	/**
	 * @copydoc PTEID_SigningDevice::SignXades
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXades(const char *output_path, const char *const *paths,
												   unsigned int n_paths,
												   PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) override;

	/**
	 * @copydoc PTEID_SigningDevice::SignXadesT
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesT(const char *output_path, const char *const *paths,
													unsigned int n_paths) override;

	/**
	 * @copydoc PTEID_SigningDevice::SignXadesA
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesA(const char *output_path, const char *const *paths,
													unsigned int n_paths) override;
	PTEIDSDK_API virtual void SignXadesIndividual(const char *output_path, const char *const *paths,
												  unsigned int n_paths) override;
	PTEIDSDK_API virtual void SignXadesTIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) override;
	PTEIDSDK_API virtual void SignXadesAIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) override;
	PTEIDSDK_API virtual void SignASiC(const char *path, PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) override;
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape,
									 const char *location, const char *reason, const char *outfile_path) override;
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y,
									 const char *location, const char *reason, const char *outfile_path) override;

	/**
	 * @return an object to access all the certificates on the card.
	 **/
	PTEIDSDK_API virtual PTEID_Certificates &getCertificates() override;
	PTEIDSDK_API virtual PTEID_Certificate &getCert(PTEID_CertifType type) override;
	PTEIDSDK_API virtual PTEID_Certificate &getRoot() override;
	PTEIDSDK_API virtual PTEID_Certificate &getCA() override;
	PTEIDSDK_API virtual PTEID_Certificate &getSignature() override;
	PTEIDSDK_API virtual PTEID_Certificate &getAuthentication() override;

	PTEIDSDK_API virtual PTEID_SigningDeviceType getDeviceType() override { return PTEID_SigningDeviceType::CC; }

	/**
	 * Read a File from the card.
	 * @param fileID is the path of the file
	 * @param ulOffset is the offset to begin the reading
	 * @param ulMaxLength is the maximum length of bytes to read
	 * @return A PTEID_ByteArray with the content of the file
	 **/
	PTEIDSDK_API virtual PTEID_ByteArray readFile(const char *fileID, unsigned long ulOffset = 0,
												  unsigned long ulMaxLength = 0);

	/**
	 * Write a file to the card.
	 * @param fileID is the path of the file
	 * @param oData contents the bytes to write
	 * @param ulOffset is the offset to begin the writing
	 **/
	PTEIDSDK_API virtual bool writeFile(const char *fileID, const PTEID_ByteArray &oData, unsigned long ulOffset = 0);

	/**
	 * Init the Pace authentication process
	 * @param secret string that holds the secret to make the PACE process
	 * @param length size of the secret string
	 * @param secretType type of the secret used to make the PACE process
	 **/

	PTEIDSDK_API virtual void initPaceAuthentication(const char *secret, size_t length,
													 PTEID_CardPaceSecretType secretType);

protected:
	PTEID_Card(const SDK_Context *context, APL_Card *impl); /**< For internal use : Constructor */

private:
	PTEID_Card(const PTEID_Card &card);			   /**< Copy not allowed - not implemented */
	PTEID_Card &operator=(const PTEID_Card &card); /**< Copy not allowed - not implemented */
};

class PTEID_Pin;
class PTEID_Pins;
class PTEID_Certificates;
class APL_SmartCard;

/**
 * Abstract base class for Smart card.
 */
class PTEID_SmartCard : public PTEID_Card {
public:
	PTEIDSDK_API virtual ~PTEID_SmartCard() = 0; /**< Destructor */

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
	 **/
	PTEIDSDK_API virtual PTEID_ByteArray sendAPDU(const PTEID_ByteArray &cmd, PTEID_Pin *pin = NULL,
												  const char *csPinCode = "");

	/**
	 * Read a File from the card.
	 *
	 * If no pin is passed and a pin is needed by the card, the pin code will be asked anyway.
	 *
	 * @param fileID is the path of the file
	 * @param in return the file
	 * @param pin is the pin to ask for reading
	 * @param csPinCode is the code of the pin (it will be asked if needed and not set)
	 **/
	PTEIDSDK_API virtual long readFile(const char *fileID, PTEID_ByteArray &in, PTEID_Pin *pin = NULL,
									   const char *csPinCode = "");

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
	 **/
	PTEIDSDK_API virtual bool writeFile(const char *fileID, const PTEID_ByteArray &out, PTEID_Pin *pin = NULL,
										const char *csPinCode = "", unsigned long inOffset = 0);

	/**
	 * @return the number of pins on the card.
	 **/
	PTEIDSDK_API virtual unsigned long pinCount();

	/**
	 * @return an object to access all the pins on the card.
	 **/
	PTEIDSDK_API virtual PTEID_Pins &getPins();

	/**
	 * @return the number of certificate on the card.
	 **/
	PTEIDSDK_API virtual unsigned long certificateCount();

protected:
	PTEID_SmartCard(const SDK_Context *context, APL_Card *impl); /**< For internal use : Constructor */

private:
	PTEID_SmartCard(const PTEID_SmartCard &card);			 /**< Copy not allowed - not implemented */
	PTEID_SmartCard &operator=(const PTEID_SmartCard &card); /**< Copy not allowed - not implemented */
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

/**
 * Helper class to deal with ASIC signature containers used by pteid-mw to deliver XAdES signatures
 * "attached to their input files"
 */
class PTEID_ASICContainer {
public:
	PTEIDSDK_API PTEID_ASICContainer(const char *input_path);
	PTEIDSDK_API ~PTEID_ASICContainer();

	PTEIDSDK_API long countInputFiles();
	PTEIDSDK_API const char *getInputFile(long file_n);

	PTEIDSDK_API void extract(const char *filename, const char *out_dir);
	// PTEIDSDK_API void extractSignature(const char *output_dir, long signature_index);

private:
	// Internal SigContainer object
	void *m_impl;
	std::vector<std::string> m_files;
};

/**
 * Helper class for the PTEID_EIDCard::SignPDF() methods - it defines multiple options that affect the signature
 * operation e.g Signature in batch mode, adding a cryptographic timestamp, small format visible signature and custom
 * image in the visible signature
 */
class PTEID_PDFSignature {
public:
	PTEIDSDK_API PTEID_PDFSignature();
	PTEIDSDK_API PTEID_PDFSignature(const char *input_path);
	PTEIDSDK_API ~PTEID_PDFSignature();

	/**
	 *  Set the PDF file to sign - it can be supplied either with this method or through the class constructor
	 **/
	PTEIDSDK_API void setFileSigning(char *input_path);
	/**
	 * Add a file to a batch mode signature.
	 *
	 * This means the signature PIN introduced when the first signature is performed
	 * is cached in memory for the minimum time needed to sign the complete set of files
	 *
	 * @param input_path: Absolute path for a PDF file to be signed in batch mode
	 **/
	PTEIDSDK_API void addToBatchSigning(char *input_path);
	PTEIDSDK_API void addToBatchSigning(char *input_path, bool last_page);
	PTEIDSDK_API int getPageCount();
	PTEIDSDK_API int getOtherPageCount(const char *input_path);
	/**
	 * Apply a timestamp after signing the document.
	 *
	 * Apply a cryptographic timestamp on the signature(s) - Internet connection is required.
	 * By default the implementation will use the official Cartão de Cidadão TSA at ts.cartaodecidadao.pt.
	 * Information about request limits described at https://pki.cartaodecidadao.pt - in section "Serviço de Selos
	 *Temporais". Using @ref PTEID_Config with parameter PTEID_PARAM_XSIGN_TSAURL a different TSA may be specified
	 **/
	PTEIDSDK_API void enableTimestamp();

	PTEIDSDK_API void setSignatureLevel(PTEID_SignatureLevel);

	/**
	 * Use a smaller format for the visible signature which is 50% smaller in height but as wide as the regular format.
	 * If using a smaller format and a custom seal size at the same time the final size is 50% smaller then the current
	 *configured size. The default "Cartão de Cidadão" logo or a custom image supplied via setCustomImage() will not be
	 *included.
	 **/
	PTEIDSDK_API void enableSmallSignatureFormat();
	PTEIDSDK_API bool isLandscapeFormat();
	/**
	 * @deprecated - method related to visible signature using sectors
	 **/
	PTEIDSDK_API char *getOccupiedSectors(int page);
	PTEIDSDK_API void setCustomImage(unsigned char *image_data, unsigned long image_length);
	/**
	 * Change the size of the visible signature (Minimum size: 120x35 px)
	 * Visible signature never will be smaller than page
	 * Except if the page is smaller then minimum size
	 * @param width - width of the visible signature   (Minimum size: 120 px)
	 * @param height - height of the visible signature (Minimum size: 35 px)
	 **/
	PTEIDSDK_API void setCustomSealSize(unsigned int width, unsigned int height);
	/**
	 * Change the image that's used on the bottom of the visible signature
	 * This image will replace the default "Cartão de Cidadão" logo
	 * @param image_data - image data in JPEG format (mandatory size: 351x77 px)
	 **/
	PTEIDSDK_API void setCustomImage(const PTEID_ByteArray &image_data);

	PTEIDSDK_API PDFSignature *getPdfSignature();

	PTEIDSDK_API char *getCertificateCitizenName();
	PTEIDSDK_API char *getCertificateCitizenID();

private:
	// The applayer object that actually implements the signature
	PDFSignature *mp_signature;

	friend class PTEID_Card;
};

class SSLConnection;

/**
 * This class represents a Portugal EID card.
 * To get such an object you have to ask it from the ReaderContext.
 */
class PTEID_EIDCard : public PTEID_SmartCard {
public:
	PTEIDSDK_API virtual ~PTEID_EIDCard(); /**< Destructor */

	PTEIDSDK_API PTEID_CCXML_Doc &getXmlCCDoc(PTEID_XmlUserRequestedInfo &userRequestedInfo);
	PTEIDSDK_API PTEID_EId &getID();					  /**< Get the id document */
	PTEIDSDK_API PTEID_Address &getAddr();				  /**< Get the Address document */
	PTEIDSDK_API PTEID_Sod &getSod();					  /**< Get the SOD document */
	PTEIDSDK_API PTEID_CardVersionInfo &getVersionInfo(); /**< Get the info document  */
	PTEIDSDK_API bool writePersonalNotes(const PTEID_ByteArray &out, PTEID_Pin *pin = NULL, const char *csPinCode = "");
	PTEIDSDK_API bool clearPersonalNotes(PTEID_Pin *pin = NULL, const char *csPinCode = "");
	PTEIDSDK_API const char *readPersonalNotes();
	/** Get the CVC CA public key that this card uses to verify the CVC key */
	PTEIDSDK_API PTEID_PublicKey &getRootCAPubKey();
	PTEIDSDK_API bool isActive();

	/** Enable/disable the verification of ID and address data against the SOD file */
	PTEIDSDK_API void doSODCheck(bool check);

#if !defined SWIG
	PTEIDSDK_API SSLConnection *buildSSLConnection();

#endif

	/**
	Activate the Pteid card by writing the contents of the supplied PTEID_ByteArray in the card TRACE file
		@param BCDDate content to be written in the activation flag file (limited to 8 bytes) - recommended way to use
	is to supply the current date in BCD format (Binary coded decimal)
		@param pinCode Activation PIN value
		@param blockActivationPIN Boolean flag used in test mode to avoid blocking the Activation PIN after first use
	*/
	PTEIDSDK_API bool Activate(const char *pinCode, PTEID_ByteArray &BCDDate, bool blockActivationPIN);

protected:
	PTEID_EIDCard(const SDK_Context *context, APL_Card *impl); /**< For internal use : Constructor */

private:
	PTEID_EIDCard(const PTEID_EIDCard &card);			 /**< Copy not allowed - not implemented */
	PTEID_EIDCard &operator=(const PTEID_EIDCard &card); /**< Copy not allowed - not implemented */
	bool persoNotesDirty;

	friend PTEID_Card & PTEID_ReaderContext::getCard(); /**< For internal use : This method must access protected constructor */
};

/**
 * This class is a client for signing with Chave Móvel Digital.
 * @since 3.8.0
 */
class PTEID_CMDSignatureClient : public PTEID_Object, public PTEID_SigningDevice {
public:
	PTEIDSDK_API PTEID_CMDSignatureClient();		  /**< Default constructor */
	PTEIDSDK_API virtual ~PTEID_CMDSignatureClient(); /**< Destructor */

	/**
	 * If setMobileNumberCaching is enabled, the mobile number used in a previous signature with this instance is
	 * cached and displayed in the signature dialog as a placeholder.
	 **/
	PTEIDSDK_API void setMobileNumberCaching(bool enabled);

	/**
	 * @copydoc PTEID_SigningDevice::Sign
	 * \attention The PTEID_CMDSignatureClient class only supports signature key. For that reason, signatureKey must be
	 * set to true!
	 */
	PTEIDSDK_API virtual PTEID_ByteArray Sign(const PTEID_ByteArray &data, bool signatureKey = false) override;
	/**
	 * @copydoc PTEID_SigningDevice::SignSHA256
	 * @deprecated Use PTEID_CMDSignatureClient::Sign instead.
	 * \attention The PTEID_CMDSignatureClient class only supports signature key. For that reason, signatureKey must be
	 * set to true!
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignSHA256(const PTEID_ByteArray &data, bool signatureKey = false) override;
	/**
	 * @copydoc PTEID_SigningDevice::SignXades
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXades(const char *output_path, const char *const *paths,
												   unsigned int n_paths,
												   PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) override;
	/**
	 * @warning This method is not implemented for PTEID_CMDSignatureClient.
	 * @throws PTEID_Exception(EIDMW_ERR_NOT_IMPLEMENTED) the method is not implemented.
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesT(const char *output_path, const char *const *paths,
													unsigned int n_paths) override;
	/**
	 * @warning This method is not implemented for PTEID_CMDSignatureClient.
	 * @throws PTEID_Exception(EIDMW_ERR_NOT_IMPLEMENTED) the method is not implemented.
	 */
	PTEIDSDK_API virtual PTEID_ByteArray SignXadesA(const char *output_path, const char *const *paths,
													unsigned int n_paths) override;
	/**
	 * @warning This method is not implemented for PTEID_CMDSignatureClient.
	 * @throws PTEID_Exception(EIDMW_ERR_NOT_IMPLEMENTED) the method is not implemented.
	 */
	PTEIDSDK_API virtual void SignXadesIndividual(const char *output_path, const char *const *paths,
												  unsigned int n_paths) override;
	/**
	 * @warning This method is not implemented for PTEID_CMDSignatureClient.
	 * @throws PTEID_Exception(EIDMW_ERR_NOT_IMPLEMENTED) the method is not implemented.
	 */
	PTEIDSDK_API virtual void SignXadesTIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) override;
	/**
	 * @warning This method is not implemented for PTEID_CMDSignatureClient.
	 * @throws PTEID_Exception(EIDMW_ERR_NOT_IMPLEMENTED) the method is not implemented.
	 */
	PTEIDSDK_API virtual void SignXadesAIndividual(const char *output_path, const char *const *paths,
												   unsigned int n_paths) override;
	PTEIDSDK_API virtual void SignASiC(const char *path, PTEID_SignatureLevel level = PTEID_LEVEL_BASIC) override;
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, int page_sector, bool is_landscape,
									 const char *location, const char *reason, const char *outfile_path) override;
	PTEIDSDK_API virtual int SignPDF(PTEID_PDFSignature &sig_handler, int page, double coord_x, double coord_y,
									 const char *location, const char *reason, const char *outfile_path) override;

	/**
	 * This method returns the certificates for the account used in the last signature performed by this client.
	 * If no signature was made, it will open a dialog to introduce the CMD credentials for the account of the
	 *certificates to be obtained.
	 * @return an object to access the certificate chain of a CMD account.
	 * @warning The PTEID_CMDSignatureClient instance manages the allocation and deletion of the PTEID_Certificates
	 *returned by getCertificates(). This means that the PTEID_Certificates can only be used while the
	 *PTEID_CMDSignatureClient exists (in Java/C# be sure to keep a reference to PTEID_CMDSignatureClient!).
	 **/
	PTEIDSDK_API virtual PTEID_Certificates &getCertificates() override;
	PTEIDSDK_API virtual PTEID_Certificate &getCert(PTEID_CertifType type) override;
	PTEIDSDK_API virtual PTEID_Certificate &getRoot() override;
	PTEIDSDK_API virtual PTEID_Certificate &getCA() override;
	PTEIDSDK_API virtual PTEID_Certificate &getSignature() override;
	PTEIDSDK_API virtual PTEID_Certificate &getAuthentication() override;

	PTEIDSDK_API virtual PTEID_SigningDeviceType getDeviceType() override { return PTEID_SigningDeviceType::CMD; }

	/**
	 * Set the credentials for the CMD services.
	 * The credentials must be set once before using the CMD services and will be used for every instance of
	 *PTEID_CMDSignatureClient.
	 **/
	PTEIDSDK_API static void setCredentials(const char *basicAuthUser, const char *basicAuthPassword,
											const char *applicationId);

private:
	std::vector<PTEID_Certificates *>
		m_certificates; /**< For internal use : certificates to be returned by getCertificates. */
};

/**
 * Singleton class - Factory used to obtain signing devices.
 * Optionally, opens a dialog to let user choose signing device type.
 * @since 3.8.0
 */
class PTEID_SigningDeviceFactory {
public:
	PTEIDSDK_API static PTEID_SigningDeviceFactory &instance(); /**< Return the singleton object */

	/**
	 * Obtain a signing device.
	 *
	 * At least one of the parameters must be true. Otherwise, PTEID_ExBadUsage is thrown.
	 * If multiple types were allowed, this method opens a dialog to choose the signing device type.
	 *
	 * @param allowCC allow to retrieve handle to PTEID_EIDCard
	 * @param allowCMD allow to retrieve handle to PTEID_CMDSignatureDevice
	 **/
	PTEIDSDK_API PTEID_SigningDevice &getSigningDevice(bool allowCC = true, bool allowCMD = true);

private:
	PTEID_SigningDeviceFactory() {};
	~PTEID_SigningDeviceFactory();

	PTEID_SigningDeviceFactory(const PTEID_SigningDeviceFactory &factory); /**< Copy not allowed - not implemented */
	PTEID_SigningDeviceFactory &
	operator=(const PTEID_SigningDeviceFactory &factory); /**< Copy not allowed - not implemented */

	std::vector<PTEID_CMDSignatureClient *>
		m_cmdClients; /**< For internal use : PTEID_CMDSignatureClient returned by getSigningDevice to be deleted. */
};

class APL_XmlUserRequestedInfo;

/**
 * This class will contain information about the data to be included in the XML file returned by
 * PTEID_EIDCard::getXmlCCDoc
 */
class PTEID_XmlUserRequestedInfo : public PTEID_Object {
public:
	PTEIDSDK_API PTEID_XmlUserRequestedInfo();
	PTEIDSDK_API PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress);
	PTEIDSDK_API PTEID_XmlUserRequestedInfo(const char *timeStamp, const char *serverName, const char *serverAddress,
											const char *tokenID);
	PTEIDSDK_API virtual ~PTEID_XmlUserRequestedInfo(); /**< Destructor */
	PTEIDSDK_API void add(XMLUserData xmlUData);		/**< add string */

protected:
	APL_XmlUserRequestedInfo *customXml;

private:
	PTEID_XmlUserRequestedInfo(const PTEID_XmlUserRequestedInfo &info); /**< Copy not allowed - not implemented */
	PTEID_XmlUserRequestedInfo &
	operator=(const PTEID_XmlUserRequestedInfo &xmlUserRequestedInfo); /**< Copy not allowed - not implemented */
	friend PTEID_CCXML_Doc &PTEID_EIDCard::getXmlCCDoc(PTEID_XmlUserRequestedInfo &userRequestedInfo);
};

class APL_DocVersionInfo;

/**
 * Class for the info document.
 * You can get such an object from PTEID_EIDCard::getVersionInfo().
 */
class PTEID_CardVersionInfo : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_CardVersionInfo(); /**< Destructor */

	PTEIDSDK_API const char *getSerialNumber();			  /**< Return the Serial Number of the card */
	PTEIDSDK_API const char *getTokenLabel();			  /**< Return the Token Label (EFCIA 5032) */
	PTEIDSDK_API const char *getComponentCode();		  /**< Return the ComponenCode of the card  */
	PTEIDSDK_API const char *getOsNumber();				  /**< Return the OS Number of the card */
	PTEIDSDK_API const char *getOsVersion();			  /**< Return the OS Version of the card */
	PTEIDSDK_API const char *getSoftmaskNumber();		  /**< Return the Softmask Number of the card */
	PTEIDSDK_API const char *getSoftmaskVersion();		  /**< Return the Softmask Version of the card */
	PTEIDSDK_API const char *getAppletVersion();		  /**< Return the Applet Version of the card */
	PTEIDSDK_API const char *getGlobalOsVersion();		  /**< Return the Global Os Version of the card */
	PTEIDSDK_API const char *getAppletInterfaceVersion(); /**< Return the Applet Interface Version of the card */
	PTEIDSDK_API const char *getPKCS1Support();			  /**< Return the PKCS#1 Support of the card */
	PTEIDSDK_API const char *getKeyExchangeVersion();	  /**< Return the Key Exchange Version of the card */
	PTEIDSDK_API const char *getAppletLifeCycle();		  /**< Return the Applet Life Cycle of the card */
	PTEIDSDK_API const char *
	getGraphicalPersonalisation(); /**< Return field GraphicalPersonalisation from the Info file */
	PTEIDSDK_API const char *
	getElectricalPersonalisation(); /**< Return field ElectricalPersonalisation from the TokenInfo file */
	PTEIDSDK_API const char *
	getElectricalPersonalisationInterface(); /**< Return field ElectricalPersonalisationInterface from the TokenInfo
												file */

private:
	PTEID_CardVersionInfo(const PTEID_CardVersionInfo &doc);			/**< Copy not allowed - not implemented */
	PTEID_CardVersionInfo &operator=(const PTEID_CardVersionInfo &doc); /**< Copy not allowed - not implemented */

	PTEID_CardVersionInfo(const SDK_Context *context, APL_DocVersionInfo *impl); /**< For internal use : Constructor */

	friend PTEID_CardVersionInfo &
	PTEID_EIDCard::getVersionInfo(); /**< For internal use : This method must access protected constructor */
};

class APL_SodEid;

/**
 * Class for the SOD document on an EID Card.
 * You can get such an object from PTEID_EIDCard::getSod().
 */
class PTEID_Sod : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Sod(); /**< Destructor */
	/** Return the SOD data - it contains a PKCS#7 signed object as described in ICAO Doc 9303 - "Machine Readable Travel Documents"  */
	PTEIDSDK_API const PTEID_ByteArray &getData();

private:
	PTEID_Sod(const PTEID_Sod &doc);			/**< Copy not allowed - not implemented */
	PTEID_Sod &operator=(const PTEID_Sod &doc); /**< Copy not allowed - not implemented */

	PTEID_Sod(const SDK_Context *context, APL_SodEid *impl); /**< For internal use : Constructor */

	friend PTEID_Sod &PTEID_EIDCard::getSod(); /**< For internal use : This method must access protected constructor */
};

class APL_DocEId;

/**
 * Class for the ID document on a EID Card.
 * You can get such an object from PTEID_EIDCard::getID().
 */
class PTEID_EId : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_EId(); /**< Destructor */

	PTEIDSDK_API const char *getDocumentVersion();		 /**< Return Document Version field */
	PTEIDSDK_API const char *getDocumentType();			 /**< Return Document Type field */
	PTEIDSDK_API const char *getCountry();				 /**< Return Country field */
	PTEIDSDK_API const char *getGivenName();			 /**< Return GivenName (2 first given name) */
	PTEIDSDK_API const char *getSurname();				 /**< Return Surname field */
	PTEIDSDK_API const char *getGender();				 /**< Return Gender field */
	PTEIDSDK_API const char *getDateOfBirth();			 /**< Return Date Of Birth field */
	PTEIDSDK_API const char *getNationality();			 /**< Return Nationality field */
	PTEIDSDK_API const char *getDocumentPAN();			 /**< Return Document PAN field */
	PTEIDSDK_API const char *getValidityBeginDate();	 /**< Return Validity Begin Date field */
	PTEIDSDK_API const char *getValidityEndDate();		 /**< Return Validity End Date field */
	PTEIDSDK_API const char *getHeight();				 /**< Return field Height */
	PTEIDSDK_API const char *getDocumentNumber();		 /**< Return field DocumentNumber */
	PTEIDSDK_API const char *getCivilianIdNumber();		 /**< Return field CivlianIdNumber */
	PTEIDSDK_API const char *getTaxNo();				 /**< Return field TaxNo */
	PTEIDSDK_API const char *getSocialSecurityNumber();	 /**< Return field SocialSecurityNumber */
	PTEIDSDK_API const char *getHealthNumber();			 /**< Return field HealthNo */
	PTEIDSDK_API const char *getIssuingEntity();		 /**< Return field IssuingEntity */
	PTEIDSDK_API const char *getLocalofRequest();		 /**< Return field LocalofRequest*/
	PTEIDSDK_API const char *getGivenNameFather();		 /**< Return field GivenNameFather */
	PTEIDSDK_API const char *getSurnameFather();		 /**< Return field SurnameFather */
	PTEIDSDK_API const char *getGivenNameMother();		 /**< Return field GivenNameMother */
	PTEIDSDK_API const char *getSurnameMother();		 /**< Return field SurnameMother */
	PTEIDSDK_API const char *getParents();				 /**< Return field Parents */
	PTEIDSDK_API PTEID_Photo &getPhotoObj();			 /**< Return object Photo */
	PTEIDSDK_API PTEID_PublicKey &getCardAuthKeyObj();	 /**< Return object CardAuthKey */
	PTEIDSDK_API const char *getValidation();			 /**< Return field Validation */
	PTEIDSDK_API const char *getMRZ1();					 /**< Return field MRZ block 1 */
	PTEIDSDK_API const char *getMRZ2();					 /**< Return field MRZ block 2 */
	PTEIDSDK_API const char *getMRZ3();					 /**< Return field MRZ block 3 */
	PTEIDSDK_API const char *getAccidentalIndications(); /**< Return field AccidentalIndications */

private:
	PTEID_EId(const PTEID_EId &doc);			/**< Copy not allowed - not implemented */
	PTEID_EId &operator=(const PTEID_EId &doc); /**< Copy not allowed - not implemented */

	PTEID_EId(const SDK_Context *context, APL_DocEId *impl); /**< For internal use : Constructor */

	friend PTEID_EId &PTEID_EIDCard::getID(); /**< For internal use : This method must access protected constructor */
};

class APL_AddrEId;

/**
 * Class for the Address information associated to an EID Card.
 * You can get such an object from PTEID_EIDCard::getAddr().
 */
class PTEID_Address : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Address(); /**< Destructor */

	PTEIDSDK_API bool isNationalAddress();	   /**< Is this address a portuguese address? */
	PTEIDSDK_API const char *getCountryCode(); /**< Return field country */
	// The following 20 getter methods return data only for national addresses
	PTEIDSDK_API const char *getDistrict();				/**< Return field District */
	PTEIDSDK_API const char *getDistrictCode();			/**< Return field District Code */
	PTEIDSDK_API const char *getMunicipality();			/**< Return field Municipality */
	PTEIDSDK_API const char *getMunicipalityCode();		/**< Return field Municipality Code */
	PTEIDSDK_API const char *getCivilParish();			/**< Return field Civil Parish */
	PTEIDSDK_API const char *getCivilParishCode();		/**< Return field Civil Parish Code */
	PTEIDSDK_API const char *getAbbrStreetType();		/**< Return field Abbreviated street type */
	PTEIDSDK_API const char *getStreetType();			/**< Return field StreetType */
	PTEIDSDK_API const char *getStreetName();			/**< Return field StreetName */
	PTEIDSDK_API const char *getAbbrBuildingType();		/**< Return field Abbreviated building type */
	PTEIDSDK_API const char *getBuildingType();			/**< Return field Building Type */
	PTEIDSDK_API const char *getDoorNo();				/**< Return field Door Number */
	PTEIDSDK_API const char *getFloor();				/**< Return field Floor */
	PTEIDSDK_API const char *getSide();					/**< Return field Side */
	PTEIDSDK_API const char *getLocality();				/**< Return field Locality */
	PTEIDSDK_API const char *getPlace();				/**< Return field Place */
	PTEIDSDK_API const char *getZip4();					/**< Return field Zip4 */
	PTEIDSDK_API const char *getZip3();					/**< Return field Zip3 */
	PTEIDSDK_API const char *getPostalLocality();		/**< Return field Postal Locality */
	PTEIDSDK_API const char *getGeneratedAddressCode(); /**< Return field Address Code */

	// The following 6 getter methods are only relevant for foreign addresses
	PTEIDSDK_API const char *getForeignCountry(); /**< Return foreign address 2-letter ISO 3166-1 country code */
	PTEIDSDK_API const char *getForeignAddress(); /**< Return foreign address generic address field - may contain street
													 name, door number, or other address elements */
	PTEIDSDK_API const char *getForeignCity();	  /**< Return foreign address city */
	PTEIDSDK_API const char *getForeignRegion();  /**< Return foreign address region */
	PTEIDSDK_API const char *getForeignLocality(); /**< Return foreign address locality */
	/** Return foreign address postal code - format is country-dependent */
	PTEIDSDK_API const char *getForeignPostalCode();

private:
	PTEID_Address(const PTEID_Address &doc);			/**< Copy not allowed - not implemented */
	PTEID_Address &operator=(const PTEID_Address &doc); /**< Copy not allowed - not implemented */

	PTEID_Address(const SDK_Context *context, APL_AddrEId *impl); /**< For internal use : Constructor */

	friend PTEID_Address &
	PTEID_EIDCard::getAddr(); /**< For internal use : This method must access protected constructor */
};

class APL_CCXML_Doc;

class PTEID_CCXML_Doc : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_CCXML_Doc(); /**< Destructor */
	PTEIDSDK_API const char *getCCXML();

protected:
	PTEID_CCXML_Doc(const SDK_Context *context, APL_CCXML_Doc *impl); /**< For internal use : Constructor */

private:
	PTEID_CCXML_Doc(const PTEID_CCXML_Doc &doc);			/**< Copy not allowed - not implemented */
	PTEID_CCXML_Doc &operator=(const PTEID_CCXML_Doc &doc); /**< Copy not allowed - not implemented */
	std::string *xmltemp;
	friend PTEID_CCXML_Doc &PTEID_EIDCard::getXmlCCDoc(
		PTEID_XmlUserRequestedInfo
			&userRequestedInfo); /**< For internal use : This method must access protected constructor */
};

class PTEID_Pin;
class APL_Pins;

/**
 * Container class for all pins on the card.
 */
class PTEID_Pins : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Pins(); /**< Destructor */

	PTEIDSDK_API unsigned long count(); /**< The number of pins on the card */

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
	PTEID_Pins(const PTEID_Pins &pins);			   /**< Copy not allowed - not implemented */
	PTEID_Pins &operator=(const PTEID_Pins &pins); /**< Copy not allowed - not implemented */

	PTEID_Pins(const SDK_Context *context, APL_Pins *impl); /**< For internal use : Constructor */

	friend PTEID_Pins &
	PTEID_SmartCard::getPins(); /**< For internal use : This method must access protected constructor */
};

class APL_Pin;

/**
 * Class that represent one card PIN.
 */
class PTEID_Pin : public PTEID_Object {
public:
	/** PIN reference value for the authentication PIN.
	 *  It can be used as parameter for @ref PTEID_Pins::getPinByPinRef() */
	static const unsigned long AUTH_PIN = 129;
	/** PIN reference value for the signature PIN. It can be used as parameter for @ref PTEID_Pins::getPinByPinRef() */
	static const unsigned long SIGN_PIN = 130;
	/** PIN reference value for the address PIN. It can be used as parameter for @ref PTEID_Pins::getPinByPinRef() */
	static const unsigned long ADDR_PIN = 131;

	/** Flags available for @ref PTEID_Pin::unlockPin */
	static const int UNBLOCK_FLAG_NEW_PIN = 1;
	static const int UNBLOCK_FLAG_PUK_MERGE = 2;

	PTEIDSDK_API virtual ~PTEID_Pin(); /**< Destructor */

	PTEIDSDK_API unsigned long getIndex();					 /**< Get the index of the pin */
	PTEIDSDK_API unsigned long getType();					 /**< Get the type of the pin */
	PTEIDSDK_API unsigned long getId();						 /**< Get the id of the pin */
	PTEIDSDK_API unsigned long getPinRef();					 /**< Get the pinref value of the pin */
	PTEIDSDK_API PTEID_PinUsage getUsageCode();				 /**< Get the usage code of the pin */
	PTEIDSDK_API unsigned long getFlags();					 /**< Get the flags of the pin */
	PTEIDSDK_API const char *getLabel();					 /**< Get the label of the pin */
	PTEIDSDK_API const char *getLabelById(unsigned long id); /**< Get the label of the pin by Id */
	PTEIDSDK_API bool unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft,
								unsigned long flags);

	/**
	 * Return the remaining tries for entering the correct pin.
	 *
	 *
	 * @return the number of remaining tries in the other case
	 */
	PTEIDSDK_API long getTriesLeft();

	/**
	 * Check the current verification status of the PIN
	 *
	 *
	 * @return true if the PIN has already been successfully presented during the current card session or false
	 * otherwise
	 */
	PTEIDSDK_API bool isVerified();

	/**
	 * Ask the card to verify the pin.
	 * A popup will ask for the code.
	 * @return true if success and false if failed
	 **/
	PTEIDSDK_API bool verifyPin();

	/**
	 * Ask the card to verify the pin.
	 *
	 * @param csPin is the pin code to verify (if csPin is empty, a popup will ask for the code)
	 * @param ulRemaining return the remaining tries (only if verifying failed)
	 * @param bShowDlg flag used to either show or not a dialog where the user inserts the pin (default=true)
	 * @param wndGeometry (default=0)
	 * @return true if success and false if failed
	 **/
	PTEIDSDK_API bool verifyPin(const char *csPin, unsigned long &ulRemaining, bool bShowDlg = true,
								void *wndGeometry = 0);

	/**
	 * Ask the card to change the pin.
	 * A popup will ask for the codes
	 * @return true if success and false if failed
	 **/
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
	 **/
	PTEIDSDK_API bool changePin(const char *csPin1, const char *csPin2, unsigned long &ulRemaining, const char *PinName,
								bool bShowDlg = true, void *wndGeometry = 0);

private:
	PTEID_Pin(const PTEID_Pin &pin);			/**< Copy not allowed - not implemented */
	PTEID_Pin &operator=(const PTEID_Pin &pin); /**< Copy not allowed - not implemented */

	PTEID_Pin(const SDK_Context *context, APL_Pin *impl); /**< For internal use : Constructor */

	friend PTEID_Pin &PTEID_Pins::getPinByNumber(
		unsigned long ulIndex); /**< For internal use : This method must access protected constructor */
	friend PTEID_Pin &PTEID_Pins::getPinByPinRef(
		unsigned long pinRef); /**< For internal use : This method must access protected constructor */
};

class APL_Certifs;

/**
 * Container class for all certificates on the card.
 */
class PTEID_Certificates : public PTEID_Object {
public:
	PTEIDSDK_API virtual ~PTEID_Certificates(); /**< Destructor */

	PTEIDSDK_API unsigned long countFromCard(); /**< The number of certificates on the card */
	PTEIDSDK_API unsigned long countAll();		/**< The number of certificates (on the card or not) */

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

	PTEIDSDK_API PTEID_Certificate &getRoot();			 /**< Return the root certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getCA();			 /**< Return the ca certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getSignature();		 /**< Return the signature certificate from the card */
	PTEIDSDK_API PTEID_Certificate &getAuthentication(); /**< Return the authentication certificate from the card */

	/**
	 * Add a new certificate to the store.
	 */
	PTEIDSDK_API PTEID_Certificate &addCertificate(PTEID_ByteArray &cert);

	PTEIDSDK_API void addToSODCAs(PTEID_ByteArray &cert);

	PTEIDSDK_API void resetSODCAs();

private:
	PTEID_Certificates(const PTEID_Certificates &certifs);			  /**< Copy not allowed - not implemented */
	PTEID_Certificates &operator=(const PTEID_Certificates &certifs); /**< Copy not allowed - not implemented */

	PTEID_Certificates(const SDK_Context *context, APL_Certifs *impl); /**< For internal use : Constructor */

	friend PTEID_Certificates &
	PTEID_Card::getCertificates(); /**< For internal use : This method must access protected constructor */
	friend PTEID_Certificates &PTEID_CMDSignatureClient::getCertificates(); /**< For internal use : This method must
																			   access protected constructor */
};

class APL_Certif;

/**
 * Class that represents one certificate.
 */
class PTEID_Certificate : public PTEID_Object {
public:
	static const unsigned long CITIZEN_AUTH = 0;
	static const unsigned long CITIZEN_SIGN = 1;
	static const unsigned long SUB_CA_SIGN = 2;
	static const unsigned long SUB_CA_AUTH = 3;

	PTEIDSDK_API virtual ~PTEID_Certificate(); /**< Destructor */

	PTEIDSDK_API const char *getLabel(); /**< Return the label of the certificate */
	PTEIDSDK_API unsigned long getID();	 /**< Return the id of the certificate */

	PTEIDSDK_API PTEID_CertifType getType(); /**< Return the type of the certificate */

	PTEIDSDK_API const PTEID_ByteArray &getCertData(); /**< Return the content of the certificate */
	PTEIDSDK_API void getFormattedData(PTEID_ByteArray &data);	/**< Return the content of the certificate without padding zeros */
	PTEIDSDK_API const char *getSerialNumber(); /**< Return the serial number of the certificate */
	PTEIDSDK_API const char *getOwnerName();	/**< Return the name of the owner of the certificate */
	PTEIDSDK_API const char * getSubjectSerialNumber();	 /**< Return the serial number of the subject of the certificate */
	PTEIDSDK_API const char *getIssuerName();	 /**< Return the name of the issuer of the certificate */
	PTEIDSDK_API const char *getValidityBegin(); /**< Return the validity begin date of the certificate */
	PTEIDSDK_API const char *getValidityEnd();	 /**< Return the validity end date of the certificate */
	PTEIDSDK_API unsigned long getKeyLength();	 /**< Return the length of public/private key on the certificate */

	PTEIDSDK_API PTEID_CertifStatus getStatus(); /**< Return OCSP/CRL validation status */

	/**
	 * OCSP/CRL validation status
	 *
	 * @param useCache boolean that specifies if cached certificates can be utilized for the validation (default true)
	 * @param validateChain boolean that specifies if the entire certificate chain is to be validated or not (default
	 * true)
	 *
	 */
	PTEIDSDK_API PTEID_CertifStatus getStatus(bool useCache, bool validateChain);

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
	 * Return true if the current time is between the certificate's notBefore and notAfter
	 */
	PTEIDSDK_API bool verifyDateValidity();

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
	 **/
	PTEIDSDK_API PTEID_Certificate &getChildren(unsigned long ulIndex);

private:
	PTEID_Certificate(const PTEID_Certificate &certif);			   /**< Copy not allowed - not implemented */
	PTEID_Certificate &operator=(const PTEID_Certificate &certif); /**< Copy not allowed - not implemented */

	PTEID_Certificate(const SDK_Context *context, APL_Certif *impl); /**< For internal use : Constructor */

	friend PTEID_Certificate &PTEID_Certificates::getCert(
		unsigned long ulIndex); /**< For internal use : This method must access protected constructor */
	friend PTEID_Certificate &PTEID_Certificates::getCertFromCard(
		unsigned long ulIndex); /**< For internal use : This method must access protected constructor */
	friend PTEID_Certificate &PTEID_Certificates::getCert(
		PTEID_CertifType type); /**< For internal use : This method must access protected constructor */
	friend PTEID_Certificate &PTEID_Certificates::addCertificate(
		PTEID_ByteArray &cert); /**< For internal use : This method must access protected constructor */
};

class APL_Config;

/**
  * Class to access and modify the config parameters of pteid-mw.
	These parameters are persisted in the Windows Registry under the HKEY_CURRENT_USER\\Software\\PTEID key
	or in the pteid.conf file of the current user (on Linux and MacOS systems)
  */
class PTEID_Config : public PTEID_Object {
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

	PTEIDSDK_API virtual ~PTEID_Config(); /**< Destructor */

	PTEIDSDK_API const char *getString(); /**< Return the string value (Throw exception for numerical parameter) */
	PTEIDSDK_API long getLong();		  /**< Return the numerical value (Throw exception for string parameter) */

	PTEIDSDK_API void setString(const char *csValue); /**< Set the string value (Throw exception for numerical parameter) */
	PTEIDSDK_API void DeleteKeysByPrefix(); /**< Reset the strings with some prefix (Throw exception for numerical parameter) */
	PTEIDSDK_API unsigned int CountKeysByPrefix(); /**< Count keys with some prefix */
	PTEIDSDK_API void setLong(long lValue); /**< Set the numerical value (Throw exception for string parameter) */
	PTEIDSDK_MULTIPASS_API static void SetTestMode(bool bTestMode); /**< Set the test mode */

private:
	PTEID_Config(const PTEID_Config &config);			 /**< Copy not allowed - not implemented */
	PTEID_Config &operator=(const PTEID_Config &config); /**< Copy not allowed - not implemented */

	PTEID_Config(APL_Config *impl); /**< For internal use : Constructor */
};

/**
 * Function used for Logging
 */
PTEIDSDK_API void PTEID_LOG(PTEID_LogLevel level, const char *module_name, const char *format, ...);

#if !defined SWIG
/* Function to get System Proxy to access supplied host */
PTEIDSDK_API void PTEID_GetProxyFromPac(const char *pacFile, const char *url, std::string *proxy_host,
										std::string *proxy_port);
#endif

} // Namespace eIDMW

#endif //__PTEIDLIB_H__
