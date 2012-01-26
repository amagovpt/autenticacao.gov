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

#ifndef __CARDEID_H__
#define __CARDEID_H__

#include <string>
#include "Export.h"
#include "APLReader.h"
#include "APLDoc.h"
#include "APLCardPteid.h"
#include "CardFile.h"
#include "ByteArray.h"
#include "PhotoPteid.h"
#include "APLCardAuthenticationKey.h"

namespace eIDMW
{

class APL_EIDCard;

/******************************************************************************//**
  * Class that represent the file containing Trace informations on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_Trace object, we have to ask it from APL_EIDCard
 *********************************************************************************/
class APL_EidFile_Trace : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_Trace();
	const char *getValidation();				/**< Return field Validation */
	bool isActive();							/**< Return the card activation state (true = active) */

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileID()
	  */
	APL_EidFile_Trace(APL_EIDCard *card);

private:
	APL_EidFile_Trace(const APL_EidFile_Trace& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_Trace &operator= (const APL_EidFile_Trace& file);	/**< Copy not allowed - not implemented */

	/**
	  * Check if the ID signature correspond to this file (using RRN certificate)
	  * @return - if bad RRN file => status of RRN file
	  * @return - if bad signature file => status of signature file
	  * @return - if signature error => CARDFILESTATUS_ERROR_SIGNATURE
	  * @return - else => CARDFILESTATUS_ERROR_OK
	  */
	virtual tCardFileStatus VerifyFile();

	/**
	  * Map all the fields with the content of the file (APL_CardFile::m_data)
	  */
	virtual bool MapFields();

	/**
	  * Empty all fields
	  */
	virtual void EmptyFields();

	/**
	  * Return true if data can be showned
	  */
	virtual bool ShowData();

	std::string m_Validation;							/**< Field Validation */
	bool isCardActive;

	friend 	APL_EidFile_Trace *APL_EIDCard::getFileTrace();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the file containing ID informations on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_ID object, we have to ask it from APL_EIDCard 
 *********************************************************************************/
class APL_EidFile_ID : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_ID();

	const char *getDocumentVersion();				/**< Return field DocumentVersion */
	const char *getCountry();						/**< Return field Country */
	const char *getDocumentType();					/**< Return field DocumentType */
	const char *getGivenName();						/**< Return field GivenName */
	const char *getSurname();						/**< Return field Surname */
	const char *getGender();						/**< Return field Gender */
	const char *getDateOfBirth();					/**< Return field DateOfBirth */
	const char *getLocationOfBirth();				/**< Return field LocationOfBirth */
	const char *getNationality();					/**< Return field Nationality */
	const char *getDuplicata();						/**< Return field Duplicata */
	const char *getSpecialOrganization();			/**< Return field SpecialOrganization */
	const char *getMemberOfFamily();				/**< Return field MemberOfFamily */
	const char *getLogicalNumber();					/**< Return field LogicalNumber */
	const char *getDocumentPAN();					/**< Return field DocumentPAN */
	const char *getValidityBeginDate();				/**< Return field ValidityBeginDate */
	const char *getValidityEndDate();				/**< Return field ValidityEndDate */
	const char *getLocalofRequest();				/**< Return field LocalofRequest */
	const char *getSpecialStatus();					/**< Return field SpecialStatus */
	/*New status for PTeid-ng */
	const char *getHeight();						/**< Return field Height */
	const char *getDocumentNumber();				/**< Return field DocumentNumber */
	const char *getCivilianIdNumber();				/**< Return field CivilianIdNumber */
	const char *getTaxNo();							/**< Return field TaxNo */
	const char *getSocialSecurityNumber();			/**< Return field SocialSecurityNumber */
	const char *getHealthNumber();					/**< Return field HealthNumber */
	const char *getIssuingEntity();					/**< Return field IssuingEntity */
	const char *getGivenNameFather();				/**< Return field GivenNameFather */
	const char *getSurnameFather();					/**< Return field SurnameFather */
	const char *getGivenNameMother();				/**< Return field GivenNameMother */
	const char *getSurnameMother();					/**< Return field SurnameMother */
	const char *getParents();						/**< Return field Parents */
	PhotoPteid *getPhotoObj();						/**< Return object Photo */
	APLCardAuthenticationKey *getCardAuthKeyObj();	/**< Return object Authentication Key */
	const char *getMRZ1();							/**< Return field MRZ block 1*/
	const char *getMRZ2();							/**< Return field MRZ block 2*/
	const char *getMRZ3();							/**< Return field MRZ block 3*/
	const char *getAccidentalIndications();			/**< Return field AccidentalIndications */
	const CByteArray& getPhotoHash();				/**< Return field PhotoHash */

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileID()
	  */    
	APL_EidFile_ID(APL_EIDCard *card);

private:
	APL_EidFile_ID(const APL_EidFile_ID& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_ID &operator= (const APL_EidFile_ID& file);	/**< Copy not allowed - not implemented */

	/**
	  * Check if the ID signature correspond to this file (using RRN certificate)
	  * @return - if bad RRN file => status of RRN file
	  * @return - if bad signature file => status of signature file
	  * @return - if signature error => CARDFILESTATUS_ERROR_SIGNATURE
	  * @return - else => CARDFILESTATUS_ERROR_OK
	  */
	virtual tCardFileStatus VerifyFile();

	/**
	  * Map all the fields with the content of the file (APL_CardFile::m_data)
	  */
	virtual bool MapFields();

	/**
	  * Empty all fields
	  */
	virtual void EmptyFields();

	/**
	  * Return true if data can be showned 
	  */
	virtual bool ShowData();

	std::string m_IDFile;									/**< Whole ID Field */
	std::string m_DocumentVersion;							/**< Field DocumentVersion */
	std::string m_Country;									/**< Field Country */
	std::string m_DocumentType;								/**< Field DocumentType */
	std::string m_GivenName;								/**< Field GivenName */
	std::string m_Surname;									/**< Field Surname */
	std::string m_Gender;									/**< Field Gender */
	std::string m_DateOfBirth;								/**< Field DateOfBirth */
	std::string m_LocationOfBirth;							/**< Field LocationOfBirth */
	std::string m_Nationality;								/**< Field Nationality */
	std::string m_CivilianIdNumber;							/**< Field NationalNumber */
	std::string m_Duplicata;								/**< Field Duplicata */
	std::string m_SpecialOrganization;						/**< Field SpecialOrganization */
	std::string m_MemberOfFamily;							/**< Field MemberOfFamily */
	std::string m_LogicalNumber;							/**< Field LogicalNumber */
	std::string m_ValidityBeginDate;						/**< Field ValidityBeginDate */
	std::string m_ValidityEndDate;							/**< Field ValidityEndDate */
	std::string m_LocalofRequest;							/**< Field IssuingMunicipality */
	std::string m_SpecialStatus;							/**< Field SpecialStatus */
	std::string m_Height;									/**< Field Height */
	std::string m_ChipNumber;								/**< Field ChipNumber */
	std::string m_DocumentNumber;							/**< Field DocumentNumber */
	std::string m_TaxNo;									/**< Field TaxNo */
	std::string m_SocialSecurityNo;							/**< Field SocialSecurityNo */
	std::string m_HealthNo;									/**< Field HealthNo */
	std::string m_IssuingEntity;							/**< Field IssuingEntity */
	std::string m_GivenNameFather;							/**< Field GivenNameFather */
	std::string m_SurnameFather;							/**< Field SurnameFather */
	std::string m_GivenNameMother;							/**< Field GivenNameMother */
	std::string m_SurnameMother;							/**< Field SurnameMother */
	std::string m_AccidentalIndications;					/**< Field AccidentalIndications */
	PhotoPteid* photo;										/**< object photo */
	std::string m_MRZ1;										/**< Field MRZ block 1 */
	std::string m_MRZ2;										/**< Field MRZ block 2 */
	std::string m_MRZ3;										/**< Field MRZ block 3 */
	CByteArray m_PhotoHash;									/**< Field PhotoHash */	
	APLCardAuthenticationKey* cardKey;

friend 	APL_EidFile_ID *APL_EIDCard::getFileID();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the file containing ID signature on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_IDSign object, we have to ask it from APL_EIDCard 
  *********************************************************************************/
class APL_EidFile_IDSign : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_IDSign();

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileIDSign()
	  */    
	APL_EidFile_IDSign(APL_EIDCard *card);
	
private:
	APL_EidFile_IDSign(const APL_EidFile_IDSign& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_IDSign &operator= (const APL_EidFile_IDSign& file);	/**< Copy not allowed - not implemented */

	/**
	  * No verification for this file
	  * Always return CARDFILESTATUS_OK
	  */
	virtual tCardFileStatus VerifyFile();
	virtual bool MapFields() {return true;}							/**< Nothing to do m_data contains the file */
	virtual void EmptyFields() {}									/**< Nothing to do m_data contains the file */


friend 	APL_EidFile_IDSign *APL_EIDCard::getFileIDSign();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the file containing address informations on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_Address object, we have to ask it from APL_EIDCard 
  *********************************************************************************/
class APL_EidFile_Address : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_Address();

	/*New status for PTeid-ng */
	bool isNationalAddress();							/**<is the address a portuguese address? */
	const char *getCountryCode();						/**<residence country */

	const char *getDistrict();
	const char *getDistrictCode();
	const char *getMunicipality();						/**< Return field Municipality */
	const char *getMunicipalityCode();
	const char *getCivilParish();
	const char *getCivilParishCode();
	const char *getAbbrStreetType();
	const char *getStreetType();
	const char *getStreetName();
	const char *getAbbrBuildingType();
	const char *getBuildingType();
	const char *getDoorNo();
	const char *getFloor();
	const char *getSide();
	const char *getLocality();
	const char *getZip4();
	const char *getZip3();
	const char *getPlace();
	const char *getPostalLocality();
	const char *getGeneratedAddressCode();

	const char *getForeignCountry();
	const char *getForeignAddress();
	const char *getForeignCity();
	const char *getForeignRegion();
	const char *getForeignLocality();
	const char *getForeignPostalCode();

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileAddress()
	  */    
	APL_EidFile_Address(APL_EIDCard *card);

private:
	APL_EidFile_Address(const APL_EidFile_Address& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_Address &operator= (const APL_EidFile_Address& file);	/**< Copy not allowed - not implemented */

	/**
	  * Check if the Address signature correspond to this file (using RRN certificate)
	  * @return - if bad RRN file => status of RRN file
	  * @return - if bad signature file => status of signature file
	  * @return - if signature error => CARDFILESTATUS_ERROR_SIGNATURE
	  * @return - else => CARDFILESTATUS_ERROR_OK
	  */
	virtual tCardFileStatus VerifyFile();

	/**
	  * Map all the fields with the content of the file (APL_CardFile::m_data)
	  */
	virtual void AddressFields();
	virtual void ForeignerAddressFields();
	virtual bool MapFields();
	virtual void EmptyFields();									/**< Empty all fields */

	/**
	  * Return true if data can be showned 
	  */
	virtual bool ShowData();


	std::string m_AddressFile;

	std::string m_AddressType;								/**< Field Address type */
	std::string m_CountryCode;								/**< Field Country code*/

	std::string m_DistrictCode;								/**< Field District Code*/
	std::string m_DistrictDescription;						/**< Field District Description*/
	std::string m_MunicipalityCode;							/**< Field Municipality Code*/
	std::string m_MunicipalityDescription;					/**< Field Municipality Description*/
	std::string m_CivilParishCode;							/**< Field Civil Parish Code*/
	std::string m_CivilParishDescription;					/**< Field Civil Parish Description*/
	std::string m_AbbrStreetType;							/**< Field Abbreviated Street Type*/
	std::string m_StreetType;								/**< Field Street Type*/
	std::string m_StreetName;								/**< Field Street Name*/
	std::string m_AbbrBuildingType;
	std::string m_BuildingType;
	std::string m_DoorNo;
	std::string m_Floor;
	std::string m_Side;
	std::string m_Locality;
	std::string m_Zip4;
	std::string m_Zip3;
	std::string m_PostalLocality;
	std::string m_Place;

	std::string m_Foreign_Country;
	std::string m_Foreign_Generic_Address;
	std::string m_Foreign_City;
	std::string m_Foreign_Region;
	std::string m_Foreign_Locality;
	std::string m_Foreign_Postal_Code;

	std::string m_Generated_Address_Code;

	static const std::string m_NATIONAL;			/**< portuguese addresses 'N' in the address type field*/
	static const std::string m_FOREIGN;				/**< foreign addresses have 'I' in the address type field*/

friend 	APL_EidFile_Address *APL_EIDCard::getFileAddress();	/**< This method must access protected constructor */

};

/******************************************************************************//**
  * Class that represent the file containing address signature on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_AddressSign object, we have to ask it from APL_EIDCard 
  *********************************************************************************/
class APL_EidFile_AddressSign : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_AddressSign();

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileAddressSign()
	  */    
	APL_EidFile_AddressSign(APL_EIDCard *card);

private:
	APL_EidFile_AddressSign(const APL_EidFile_AddressSign& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_AddressSign &operator= (const APL_EidFile_AddressSign& file);	/**< Copy not allowed - not implemented */

	/**
	  * No verification for this file
	  * Always return CARDFILESTATUS_OK
	  */
	virtual tCardFileStatus VerifyFile();
	virtual bool MapFields() {return true;}							/**< Nothing to do m_data contains the file */
	virtual void EmptyFields() {}									/**< Nothing to do m_data contains the file */

friend 	APL_EidFile_AddressSign *APL_EIDCard::getFileAddressSign();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the file containing the SOD on a PTID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_Sod object, we have to ask it from APL_EIDCard
  *********************************************************************************/
class APL_EidFile_Sod : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_Sod();

	const char *getSod(); /* Get Sod */

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileSod()
	  */    
	APL_EidFile_Sod(APL_EIDCard *card);

private:
	APL_EidFile_Sod(const APL_EidFile_Sod& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_Sod &operator= (const APL_EidFile_Sod& file);	/**< Copy not allowed - not implemented */

	/**
	  * Check if the hash in the ID file correspond to this file
	  * @return - if bad ID file => status of ID file
	  * @return - if hash error => CARDFILESTATUS_ERROR_HASH
	  * @return - else => CARDFILESTATUS_ERROR_OK
	  */
	virtual tCardFileStatus VerifyFile();
	virtual bool MapFields();									/**< Nothing to do m_data contains the file */
	virtual void EmptyFields();									/**< Nothing to do m_data contains the file */

	/**
	  * Return true if data can be showned
	  */
	virtual bool ShowData();

	std::string m_Sod;                                            	/**< Field Sod */
	std::string m_SodFile;											/**< Whole Sod Field */

friend 	APL_EidFile_Sod *APL_EIDCard::getFileSod();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the file containing the Personal Data on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_PersoData object, we have to ask it from APL_EIDCard
  *********************************************************************************/
class APL_EidFile_PersoData : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_PersoData();

	const char *getPersoData(); /* Get PersonalData */

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFilePersoData()
	  */
	APL_EidFile_PersoData(APL_EIDCard *card);

private:
	APL_EidFile_PersoData(const APL_EidFile_PersoData& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_PersoData &operator= (const APL_EidFile_PersoData& file);	/**< Copy not allowed - not implemented */

	/**
	  * Check if the hash in the ID file correspond to this file
	  * @return - if bad ID file => status of ID file
	  * @return - if hash error => CARDFILESTATUS_ERROR_HASH
	  * @return - else => CARDFILESTATUS_ERROR_OK
	  */
	virtual tCardFileStatus VerifyFile();
	virtual bool MapFields();										/**< Nothing to do m_data contains the file */
	virtual void EmptyFields();										/**< Nothing to do m_data contains the file */

	/**
	  * Return true if data can be showned
	  */
	virtual bool ShowData();

	std::string m_PersoData;                                            	/**< Field PersoData */


friend 	APL_EidFile_PersoData *APL_EIDCard::getFilePersoData();	/**< This method must access protected constructor */
};

/******************************************************************************//**
  * Class that represent the Token info file on a PTEID card
  *
  * This class is for internal use in APL_EIDCard
  *
  * To get APL_EidFile_TokenInfo object, we have to ask it from APL_EIDCard 
  *********************************************************************************/
class APL_EidFile_TokenInfo : public APL_CardFile
{
public:
	/**
	  * Destructor
	  */
	virtual ~APL_EidFile_TokenInfo();

	const char *getGraphicalPersonalisation();				/**< Return the Graphical Personalisation of the file */
	const char *getElectricalPersonalisation();				/**< Return the Electrical Personalisation of the file */
	const char *getElectricalPersonalisationInterface();	/**< Return the Electrical Personalisation Interface of the file */
	const char *getTokenLabel();
	const char *getTokenSerialNumber();

protected:
	/**
	  * Constructor
	  *		Used only in APL_EIDCard::getFileTokenInfo()
	  */    
	APL_EidFile_TokenInfo(APL_EIDCard *card);

private:
	APL_EidFile_TokenInfo(const APL_EidFile_TokenInfo& file);				/**< Copy not allowed - not implemented */
	APL_EidFile_TokenInfo &operator= (const APL_EidFile_TokenInfo& file);	/**< Copy not allowed - not implemented */

	virtual tCardFileStatus VerifyFile();	/**< Always return CARDFILESTATUS_OK */
	virtual bool MapFields();				/**< Map the fields with the content of the file */
	virtual void EmptyFields();				/**< Empty all the fields */

	std::string m_GraphicalPersonalisation;				/**< The Graphical Personalisation of the file */
	std::string m_ElectricalPersonalisation;			/**< The Electrical Personalisation of the file */
	std::string m_ElectricalPersonalisationInterface;	/**< The Electrical Personalisation Interface of the file */
	std::string m_label;
	std::string m_TokenSerialNumber;

friend 	APL_EidFile_TokenInfo *APL_EIDCard::getFileTokenInfo();	/**< This method must access protected constructor */
};
}

#endif //__CARDEID_H__
