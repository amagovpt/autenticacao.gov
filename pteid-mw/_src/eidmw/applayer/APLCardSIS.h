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

#ifndef __APLCARDSIS_H__
#define __APLCARDSIS_H__

#include <string>
#include "Export.h"
#include "APLReader.h"
#include "APLCard.h"
#include "APLDoc.h"

namespace eIDMW
{

class APL_SisFile_ID;
class APL_DocSisId;
class APL_SisFullDoc;

/******************************************************************************//**
  * Class that represent a SIS card
  *
  * To get APL_SISCard object, we have to ask it from APL_ReaderContext 
 *********************************************************************************/
class APL_SISCard : public APL_MemoryCard
{
public:
	/**
	  * Destructor
	  *
	  * Destroy m_docid and m_FileID
	  */
	EIDMW_APL_API virtual ~APL_SISCard();

	/**
	  * Return the type of the card (PTEID_CARDTYPE_SIS)
	  */
	EIDMW_APL_API virtual APL_CardType getType() const;

 	/** 
	 * Return a document from the card
	 */
	EIDMW_APL_API virtual APL_XMLDoc& getDocument(APL_DocumentType type);

	/**
	  * Return a pointer to the document FULL
	  */
	EIDMW_APL_API APL_SisFullDoc& getFullDoc();

	/**
	  * Return a pointer to the document ID
	  *
	  * It is based upon the ID file	
	  */
    EIDMW_APL_API APL_DocSisId& getID();

 	/** 
	 * Return rawdata from the card
	 */
	EIDMW_APL_API virtual const CByteArray& getRawData(APL_RawDataType type);

 	EIDMW_APL_API const CByteArray& getRawData_Id();					/**< Get the id RawData */

	APL_SisFile_ID *getFileID();						/**< Return a pointer to the file ID (NOT EXPORTED) */

protected:
	/**
	  * Constructor
	  *		Used only in APL_ReaderContext::connectCard
	  */    
	APL_SISCard(APL_ReaderContext *reader);

	virtual bool initVirtualReader();
	virtual bool isCardForbidden();

	/**
	  * Read a file from a virtual reader
	  *
	  * @param fileID : is the name/path of the file
	  * @param in : will return the content of the file
	  */
	static unsigned long readVirtualFileRAW(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx);
	static unsigned long readVirtualFileTLV(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx);
	static unsigned long readVirtualFileCSV(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx);
	static unsigned long readVirtualFileXML(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx);

private:
	APL_SISCard(const APL_SISCard& card);				/**< Copy not allowed - not implemented */
	APL_SISCard &operator= (const APL_SISCard& card);	/**< Copy not allowed - not implemented */

	APL_SisFullDoc *m_docfull;								/**< Pointer to the document FULL */
	APL_DocSisId *m_docid;								/**< Pointer to the document ID */

	APL_SisFile_ID *m_FileID;							/**< Pointer to the file ID */

friend bool APL_ReaderContext::connectCard();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the FULL document on a SIS card
  * To get APL_DocSisFull object, we have to ask it from APL_EIDCard 
  *********************************************************************************/
class APL_SisFullDoc : public APL_XMLDoc
{
public:
	/**
	  * Destructor
	  */
	EIDMW_APL_API virtual ~APL_SisFullDoc();

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/

	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	/**< Build the XML document */
	EIDMW_APL_API virtual CByteArray getCSV();						/**< Build the CSV document */
	EIDMW_APL_API virtual CByteArray getTLV();						/**< Build the TLV document */

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getDocFull()
	  */    
	APL_SisFullDoc(APL_SISCard *card);

private:
	APL_SisFullDoc(const APL_SisFullDoc& doc);				/**< Copy not allowed - not implemented */
	APL_SisFullDoc &operator= (const APL_SisFullDoc& doc);	/**< Copy not allowed - not implemented */

	APL_SISCard *m_card;							/**< Pointer to the card that construct this object*/

friend APL_SisFullDoc& APL_SISCard::getFullDoc();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the document ID on a SIS card
  *
  * This class show id informations to APL_SISCard user
  *
  * To get APL_DocSisId object, we have to ask it from APL_SISCard 
  *********************************************************************************/
class APL_DocSisId : public APL_XMLDoc
{
public:
	/**
	  * Destructor
	  */
	EIDMW_APL_API virtual ~APL_DocSisId();

	EIDMW_APL_API virtual bool isAllowed();							/**< The document is allowed*/

	EIDMW_APL_API virtual CByteArray getXML(bool bNoHeader=false);	/**< Build the XML document */
	EIDMW_APL_API virtual CByteArray getCSV();						/**< Build the CSV document */
	EIDMW_APL_API virtual CByteArray getTLV();						/**< Build the TLV document */

	EIDMW_APL_API const char *getName();						/**< Return field Name from the ID file */
	EIDMW_APL_API const char *getSurname();					/**< Return field Surname from the ID file */
	EIDMW_APL_API const char *getInitials();					/**< Return field Initials from the ID file */
	EIDMW_APL_API const char *getGender();					/**< Return field Gender from the ID file */
	EIDMW_APL_API const char *getDateOfBirth();				/**< Return field DateOfBirth from the ID file */
	EIDMW_APL_API const char *getSocialSecurityNumber();		/**< Return field SocialSecurityNumber from the ID file */
	EIDMW_APL_API const char *getLogicalNumber();				/**< Return field LogicalNumber from the ID file */
	EIDMW_APL_API const char *getDateOfIssue();				/**< Return field DateOfIssue from the ID file */
	EIDMW_APL_API const char *getValidityBeginDate();			/**< Return field ValidityBeginDate from the ID file */
	EIDMW_APL_API const char *getValidityEndDate();			/**< Return field ValidityEndDate from the ID file */

protected:
	/**
	  * Constructor
	  *		Used only in APL_SISCard::getID()
	  */    
	APL_DocSisId(APL_Card *card);	

private:
	APL_DocSisId(const APL_DocSisId& doc);				/**< Copy not allowed - not implemented */
	APL_DocSisId &operator= (const APL_DocSisId& doc);	/**< Copy not allowed - not implemented */

	APL_SISCard *m_card;								/**< Pointer to the card that construct this object*/

friend APL_DocSisId& APL_SISCard::getID();	/**< This method must access protected constructor */

};

}

#endif //__CARDSIS_H__
