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

#ifndef __CARDSIS_H__
#define __CARDSIS_H__

#include <string>
#include "APLReader.h"
#include "APLCardSIS.h"
#include "APLDoc.h"
#include "CardFile.h"

namespace eIDMW
{

class APL_Card;

/******************************************************************************//**
  * Class that represent the file containing ID information on a SIS card
  *
  * This class is for internal use in APL_SISCard
  *
  * To get APL_SisFile_ID object, we have to ask it from APL_SISCard 
  *********************************************************************************/
class APL_SisFile_ID : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_SisFile_ID();

    const char *getName();									/**< Return field Name */
	const char *getSurname();								/**< Return field Surname */
	const char *getInitials();								/**< Return field Initials */
	const char *getGender();								/**< Return field Gender */
	const char *getDateOfBirth();							/**< Return field DateOfBirth */
	const char *getSocialSecurityNumber();					/**< Return field SocialSecurityNumber */
	const char *getLogicalNumber();							/**< Return field LogicalNumber */
	const char *getDateOfIssue();							/**< Return field DateOfIssue */
	const char *getValidityBeginDate();						/**< Return field ValidityBeginDate */
	const char *getValidityEndDate();						/**< Return field ValidityEndDate */

protected:
	/**
	  * Constructor
	  *		Used only in APL_SISCard::getFileID()
	  */    
	APL_SisFile_ID(APL_Card *card);		

private:
	APL_SisFile_ID(const APL_SisFile_ID& file);				/**< Copy not allowed - not implemented */
	APL_SisFile_ID &operator= (const APL_SisFile_ID& file);	/**< Copy not allowed - not implemented */

	/**
	  * No verification for this file
	  * Always return CARDFILESTATUS_OK
	  */
	virtual tCardFileStatus VerifyFile();

	/**
	  * Map all the fields with the content of the file (APL_CardFile::m_data)
	  */
	virtual bool MapFields();
	virtual void EmptyFields();							/**< Empty all fields */

    std::string m_Name;									/**< Field Name */
	std::string m_Surname;								/**< Field Surname */
	std::string m_Initials;								/**< Field Initials */
	std::string m_Gender;								/**< Field Gender */
	std::string m_DateOfBirth;							/**< Field DateOfBirth */
	std::string m_SocialSecurityNumber;					/**< Field SocialSecurityNumber */
	std::string m_LogicalNumber;						/**< Field LogicalNumber */
	std::string m_DateOfIssue;							/**< Field DateOfIssue */
	std::string m_ValidityBeginDate;					/**< Field ValidityBeginDate */
	std::string m_ValidityEndDate;						/**< Field ValidityEndDate */

friend APL_SisFile_ID *APL_SISCard::getFileID();	/**< This method must access protected constructor */
};

}

#endif //__CARDSIS_H__
