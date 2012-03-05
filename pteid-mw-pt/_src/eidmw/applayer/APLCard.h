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

#ifndef __SCCARDUTIL_H__
#define __SCCARDUTIL_H__

#include <string>
#include "Export.h"
#include "ByteArray.h"
#include "P15Objects.h"
#include "APLReader.h"
#include "APLPublicKey.h"

#define BEGIN_CAL_OPERATION(obj) \
	obj->CalLock();\
	try \
	{

#define END_CAL_OPERATION(obj) \
	} \
	catch(...) \
	{ \
		obj->CalUnlock(); \
		throw; \
	} \
	obj->CalUnlock();

namespace eIDMW
{

EIDMW_APL_API bool APLVerifySignature(const char *container_path, char * errors, unsigned long* error_len);
enum APL_DocumentType
{
	APL_DOCTYPE_ID=0,
	APL_DOCTYPE_ADDRESS,
	APL_DOCTYPE_SOD,
	APL_DOCTYPE_INFO,
	APL_DOCTYPE_PINS,
	APL_DOCTYPE_CERTIFICATES
};

enum APL_RawDataType
{
	APL_RAWDATA_ID=0,
	APL_RAWDATA_ID_SIG,
	APL_RAWDATA_TRACE,
	APL_RAWDATA_ADDR,
	APL_RAWDATA_ADDR_SIG,
	APL_RAWDATA_SOD,
	APL_RAWDATA_CARD_INFO,
	APL_RAWDATA_TOKEN_INFO,
	APL_RAWDATA_CHALLENGE,
	APL_RAWDATA_RESPONSE,
	APL_RAWDATA_PERSO_DATA
};


class CReader;
class APL_CryptoFwk;
class APL_ReaderContext;
class APL_Pin;
class APL_XMLDoc;
/******************************************************************************//**
  * Abstract base class for all type of card
  *********************************************************************************/
class APL_Card
{
public: 
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_Card()=0;

	/** 
	 * Return the type of the card
	 */
	EIDMW_APL_API virtual APL_CardType getType() const;

 	/** 
	 * Return a document from the card
	 */
	EIDMW_APL_API virtual APL_XMLDoc& getDocument(APL_DocumentType type)=0;

  	/** 
	 * Return rawdata from the card
	 */
	EIDMW_APL_API virtual const CByteArray& getRawData(APL_RawDataType type)=0;

        EIDMW_APL_API virtual CByteArray sendAPDU(const CByteArray& cmd);

        EIDMW_APL_API virtual CByteArray Sign(const CByteArray & oData, bool signatureKey=false);

	/* XADeS Signature Methods  */

	EIDMW_APL_API CByteArray &SignXades(CByteArray ba, const char *URL);

	EIDMW_APL_API CByteArray &SignXadesT(CByteArray ba, const char *URL);

	EIDMW_APL_API CByteArray &SignXades(const char ** path, unsigned int n_paths, const char *output_path);

	EIDMW_APL_API CByteArray &SignXadesT(const char ** path, unsigned int n_paths, const char *output_path);

	EIDMW_APL_API void SignXadesIndividual(const char**, unsigned int, const char*);
	
	EIDMW_APL_API void SignXadesTIndividual(const char**, unsigned int, const char*);

	/**
	  * Read a file on the card 
	  *
	  * @param csPath is the path of the file to be read
	  * @param bytearray will contain the content of the file 
	  */
 	EIDMW_APL_API virtual unsigned long readFile(const char *csPath, CByteArray &oData, unsigned long  ulOffset=0, unsigned long  ulMaxLength=0);

	/**
	  * Write a file to the card 
	  *
	  * @param csPath is the path of the file to be written
	  * @param oData will contain the content of the file 
	  */
    EIDMW_APL_API virtual bool writeFile(const char *csPath, const CByteArray& oData,unsigned long ulOffset=0);

	EIDMW_APL_API virtual bool isVirtualCard() const;

	void CalLock();							/**< Lock the reader for exclusive atomic access */
	void CalUnlock();						/**< Unlock the reader */

	/**
	  * Give access to the CAL
	  * NOT FOR EXPORT
	  */
	CReader *getCalReader() const;

	virtual bool initVirtualReader()=0;

protected:
	/**
	  * Constructor
	  *
	  * @param reader is the reader from the card layer (CAL) in which the card is plugged
	  */
	APL_Card(APL_ReaderContext *reader);


	virtual bool isCardForbidden()=0;

	void SignIndividual(const char**, unsigned int, const char*, bool);

	static APL_CryptoFwk *m_cryptoFwk;			/**< Pointer to the crypto framework */
	APL_ReaderContext *m_reader;				/**< Pointer to CAL reader (came from constructor) */	

	CMutex m_Mutex;								/**< Mutex */

private:
	APL_Card(const APL_Card &card);				/**< Copy not allowed - not implemented */
	APL_Card &operator=(const APL_Card &card);	/**< Copy not allowed - not implemented */

friend bool APL_ReaderContext::connectCard();	/**< This method must access protected constructor */
};

class APL_Pins;
class APL_Certif;
class APL_Certifs;
class APL_CardFile_Info;
/******************************************************************************//**
  * Abstract base class for smart card (like eid card)
  *********************************************************************************/
class APL_SmartCard : public APL_Card
{
public:
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_SmartCard()=0;

	EIDMW_APL_API void getInfo(CByteArray &info);		/**< Call the GetInfo from the CAL */

	/**
	  * Tell the card to select an application on the card
	  */
	EIDMW_APL_API void selectApplication(const CByteArray &applicationId) const;

    EIDMW_APL_API virtual CByteArray sendAPDU(const CByteArray& cmd,APL_Pin *pin=NULL,const char *csPinCode="");

	/**
	  * Read a file on the card 
	  *
	  * @param csPath is the path of the file to be read
	  * @param bytearray will contain the content of the file 
	  */
 	EIDMW_APL_API virtual unsigned long readFile(const char *csPath, CByteArray &oData, unsigned long  ulOffset=0, unsigned long  ulMaxLength=0);

	/**
	  * Read a file on the card
	  *
	  * @param fileID : is the name/path of the file
	  * @param in : will return the content of the file
	  * @param pin : is the pin to verify before reading (if null the code will be asked)
	  * @param csPinCode : is the pin code (if empty the code will be asked)
	  */
	EIDMW_APL_API virtual unsigned long readFile(const char *fileID, CByteArray &in,APL_Pin *pin=NULL,const char *csPinCode="");

	/**
	  * Write a file to the card
	  *
	  * @param fileID : is the name/path of the file
	  * @param out : the content of the file
	  * @param pin : is the pin to verify before writing (if null the code will be asked)
	  * @param csPinCode : is the pin code (if empty the code will be asked)
	  */
	EIDMW_APL_API virtual bool writeFile(const char *fileID,const CByteArray &out,APL_Pin *pin=NULL,const char *csPinCode="");

	/**
	  * Return the number of pin on the card
	  */
	EIDMW_APL_API virtual unsigned long pinCount();

	/**
	  * Return an object to access all the pins on the card
	  */
	EIDMW_APL_API virtual APL_Pins *getPins();

	/**
	  * Return structure containing the PKCS15 object from the CAL
	  */
	EIDMW_APL_API virtual tPin getPin(unsigned long ulIndex);

	/**
	  * Return pinStatus from the CAL
	  */
	EIDMW_APL_API virtual unsigned long pinStatus(const tPin & Pin);

	/* Get the CVC CA public key that
	 * this card uses to verify the CVC key; */
	EIDMW_APL_API virtual APLPublicKey *getRootCAPubKey()=0;

 	/**
	  * Execute a pin command from the CAL
	  */
	EIDMW_APL_API virtual bool pinCmd(tPinOperation operation, const tPin &Pin,const char *csPin1, const char *csPin2,unsigned long &ulRemaining,bool bShowDlg=true);

	/**
	  * Return the number of certificate on the card
	  */
	EIDMW_APL_API virtual unsigned long certificateCount();

	/**
	  * Return an object to access all the certificates on the card
	  */
	EIDMW_APL_API virtual APL_Certifs *getCertificates();

	/**
	  * Return structure containing the PKCS15 object from the CAL
	  */
	EIDMW_APL_API virtual tCert getP15Cert(unsigned long ulIndex);

	/**
	  * Return true this is a test card
	  */
	EIDMW_APL_API virtual bool isTestCard();

	/**
	  * Return true if test card are allowed
	  */
	EIDMW_APL_API virtual bool getAllowTestCard();

	/**
	  * Set the flag to allow the test cards
	  */
	EIDMW_APL_API virtual void setAllowTestCard(bool allow);

	/**
	  * Return true if bad date card are allowed
	  */
	EIDMW_APL_API virtual bool getAllowBadDate();

	/**
	  * Set the flag to allow the bad date cards
	  */
	EIDMW_APL_API virtual void setAllowBadDate(bool allow);

	/**
	  * Return the challenge
	  *
	  * @param bForceNewInit force a new initialization of the couple challenge/response
	  */
	EIDMW_APL_API virtual const CByteArray &getChallenge(bool bForceNewInit = false);
	EIDMW_APL_API virtual const CByteArray &getChallengeResponse();		/**< Return the response to the challenge */

	/**
	  * Return true if the response of the card to the given challenge is the same as the response expected
	  * For virtual card (from file), always return false
	  *
	  * @param challenge is the challenge to check
	  * @param response is the response expected from the card
	  */
	EIDMW_APL_API virtual bool verifyChallengeResponse(const CByteArray &challenge, const CByteArray &response) const;
	
	APL_CardFile_Info *getFileInfo();		/**< Return a pointer to the pseudo file info */

	virtual const char *getTokenSerialNumber() = 0;	/**< Return the token serial number (pkcs15 parse) (NOT EXPORTED) */
	virtual	const char *getTokenLabel() = 0;	/**< Return the token label (pkcs15 parse) (NOT EXPORTED) */

protected:
	/**
	  * Constructor
	  *
	  * @param reader is the reader from the card layer (CAL) in which the card is plugged
	  */
	APL_SmartCard(APL_ReaderContext *reader);

	virtual void initChallengeResponse();	/**< Initialize Challenge/Response */
	virtual CByteArray getChallengeResponse(const CByteArray &challenge) const;	/**< Return the response to the specified challenge */

	APL_Pins *m_pins;				/**< Pointer to the collection of pins */
	APL_Certifs *m_certs;			/**< Pointer to the certificate store */
		
	APL_CardFile_Info *m_fileinfo;	/**< Pointer to the "pseudo file" Info */

	bool m_allowTestParam;			/**< Allow test card (from config) */
	bool m_allowTestAnswer;			/**< User's answer to allow test card */
	bool m_allowTestAsked;			/**< Already asked for allowing test card */

	bool m_allowBadDate;			/**< Allow bad date card */

	unsigned long m_certificateCount;
	unsigned long m_pinCount;

	CByteArray *m_challenge;
	CByteArray *m_challengeResponse;
	APLPublicKey *m_RootCAPubKey;
	std::string *m_tokenSerial;
	std::string *m_tokenLabel;
};

/******************************************************************************//**
  * Abstract base class for memory card (like sis card)
  *********************************************************************************/
class APL_MemoryCard : public APL_Card
{
public:
	/**
	  * Pur virtual destructor
	  */
	EIDMW_APL_API virtual ~APL_MemoryCard()=0;

protected:
	/**
	  * Constructor
	  *
	  * @param reader is the reader from the card layer (CAL) in which the card is plugged
	  */
	APL_MemoryCard(APL_ReaderContext *reader);
};

}

#endif //__SCCARDUTIL_H__
