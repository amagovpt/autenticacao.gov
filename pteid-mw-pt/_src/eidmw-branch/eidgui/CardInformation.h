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

#ifndef _CARDINFORMATION_H_
#define _CARDINFORMATION_H_

#include <iostream>
#include <string.h>
#include <QtGui>

#include "eidlib.h"
#include "eidlibException.h"

using namespace eIDMW;

typedef QMap<QString,QString> tFieldMap;

//**************************************************
// Card information
// This class contains all the card specific data
//**************************************************
class CardInfo
{
public:

#define CHIP_NUMBER			"chip_number"
#define CARD_NUMBER			"card_number"
#define CARD_TYPE			"card_type"
#define CARD_DATEOFISSUE	"card_dateofissue"
#define CARD_PLACEOFISSUE	"card_placeofissue"
#define CARD_ISSUING		"card_issuing"
#define CARD_VALIDFROM		"card_validfrom"
#define CARD_VALIDUNTIL		"card_validuntil"

	//----------------------------------------------
	// ctor
	//----------------------------------------------
	CardInfo( void ) {}

	//----------------------------------------------
	// Reset: reset all the members to default values
	//----------------------------------------------
	void Reset( void )
	{
		m_Fields.clear();
	}

	//----------------------------------------------
	// Retrieve the card specific data
	//----------------------------------------------
	bool RetrieveData(PTEID_EIDCard& Card)
	{
		bool bRetVal = false;
		PTEID_EId& pteid_eid	= Card.getID();

		m_Fields[CARD_TYPE]			=  pteid_eid.getDocumentType();
		m_Fields[CHIP_NUMBER]		=  pteid_eid.getDocumentPAN();
		//m_Fields[CARD_PLACEOFISSUE]	=  pteid_eid.getIssuingMunicipality());
		m_Fields[CARD_VALIDFROM]	=  pteid_eid.getValidityBeginDate();
		m_Fields[CARD_VALIDUNTIL]	=  pteid_eid.getValidityEndDate();

// 		qDebug() << "CardInfo::RetrieveData()";
// 		for (tFieldMap::iterator it=m_Fields.begin(); it!=m_Fields.end(); it++)
// 		{
// 			qDebug() << "[" << it.key() << "] = " << "Value: " << it.value();
// 		}

		bRetVal = true;
		return bRetVal;
	}

	//----------------------------------------------
	// get reference to all fields
	//----------------------------------------------
	tFieldMap& getFields( void )
	{
		return m_Fields;
	}


	static QString formatCardNumber(QString const& card_number)
	{
		QString formatted="";

		if (card_number.length()==0)
		{
			return formatted;
		}

		return formatted;
	}

private:
	tFieldMap m_Fields;
};

//**************************************************
// Address information
// This class contains all the card specific data
//**************************************************
class AddressInfo
{
public:
#define ADDRESS_STREET			"address_street"
#define ADDRESS_ZIPCODE			"address_zipcode"
#define	ADDRESS_CITY			"address_city"
#define ADDRESS_COUNTRY			"address_country"
#define ADDRESS_DISTRICT		"address_district"
#define ADDRESS_MUNICIPALITY 	"address_municipality"
#define ADDRESS_CIVILPARISH		"address_parish"
#define ADDRESS_ABBRSTREETTYPE	"address_abbrstreettype"
#define ADDRESS_STREETTYPE		"address_streettype"
#define ADDRESS_STREETNAME		"address_streetname"
#define ADDRESS_ABBRBUILDINGTYPE	"address_abbrbuildingtype"
#define ADDRESS_BUILDINGTYPE	"address_buildingtype"
#define ADDRESS_DOORNO			"address_doorno"
#define ADDRESS_FLOOR			"address_floor"
#define ADDRESS_SIDE			"address_side"
#define ADDRESS_PLACE			"address_place"
#define ADDRESS_LOCALITY		"address_locality"
#define ADDRESS_ZIP4			"address_zip4"
#define ADDRESS_ZIP3			"address_zip3"
#define ADDRESS_POSTALLOCALITY	"address_postallocality"

#define FOREIGN_COUNTRY "foreign_country"
#define FOREIGN_ADDRESS "foreign_address"
#define FOREIGN_CITY "foreign_city"
#define FOREIGN_REGION "foreign_region"
#define FOREIGN_LOCALITY "foreign_locality"
#define FOREIGN_POSTALCODE "foreign_postalcode"

	PTEID_Card*              m_pCard;
	//----------------------------------------------
	// ctor
	//----------------------------------------------
	AddressInfo( void )
	{
	}

	//----------------------------------------------
	// dtor
	//----------------------------------------------
	virtual ~AddressInfo( void )
	{
	}

	//----------------------------------------------
	// Reset data to default values
	//----------------------------------------------
	void Reset( void )
	{
		m_Fields.clear();
	}

	//----------------------------------------------
	// Retrieve data from EID card
	//----------------------------------------------
	bool RetrieveDataAddress(PTEID_EIDCard& Card)
	{
		bool		bRetVal = false;
		//TODO marker set
		PTEID_Address&	pteid_eid = Card.getAddr();

		if (!pteid_eid.isNationalAddress())
		{
			m_foreign = true;

			m_Fields[FOREIGN_COUNTRY] = pteid_eid.getForeignCountry();
			m_Fields[FOREIGN_ADDRESS] = pteid_eid.getForeignAddress();
			m_Fields[FOREIGN_CITY] = pteid_eid.getForeignCity();
			m_Fields[FOREIGN_REGION] = pteid_eid.getForeignRegion();
			m_Fields[FOREIGN_LOCALITY] = pteid_eid.getForeignLocality();
			m_Fields[FOREIGN_POSTALCODE] = pteid_eid.getForeignPostalCode();
		}
		else
		{
			m_foreign = false;

			m_Fields[ADDRESS_DISTRICT]			=  pteid_eid.getDistrict();
			m_Fields[ADDRESS_MUNICIPALITY] 		=  pteid_eid.getMunicipality();
			m_Fields[ADDRESS_CIVILPARISH]  		=  pteid_eid.getCivilParish();
			m_Fields[ADDRESS_ABBRSTREETTYPE]  	=  pteid_eid.getAbbrStreetType();
			m_Fields[ADDRESS_STREETTYPE]  		=  pteid_eid.getStreetType();
			m_Fields[ADDRESS_STREETNAME]   		=  pteid_eid.getStreetName();
			m_Fields[ADDRESS_ABBRBUILDINGTYPE] 	=  pteid_eid.getAbbrBuildingType();
			m_Fields[ADDRESS_BUILDINGTYPE] 		=  pteid_eid.getBuildingType();
			m_Fields[ADDRESS_DOORNO] 			=  pteid_eid.getDoorNo();
			m_Fields[ADDRESS_FLOOR] 			=  pteid_eid.getFloor();
			m_Fields[ADDRESS_SIDE] 				=  pteid_eid.getSide();
			m_Fields[ADDRESS_PLACE] 			=  pteid_eid.getPlace();
			m_Fields[ADDRESS_LOCALITY] 			=  pteid_eid.getLocality();
			m_Fields[ADDRESS_ZIP4] 				=  pteid_eid.getZip4();
			m_Fields[ADDRESS_ZIP3] 				=  pteid_eid.getZip3();
			m_Fields[ADDRESS_POSTALLOCALITY]	=  pteid_eid.getPostalLocality();

		}

// 		qDebug() << "AddressInfo::RetrieveData()";
// 		for (tFieldMap::iterator it=m_Fields.begin(); it!=m_Fields.end(); it++)
// 		{
// 			qDebug() << "[" << it.key() << "] = " << "Value: " << it.value();
// 		}
		//bRetVal = true;
		m_pCard = &Card;
		return bRetVal;
	};

	//----------------------------------------------
	// get reference to all fields
	//----------------------------------------------
	tFieldMap& getFields( void )
	{
		return m_Fields;
	}

	bool isForeign() { return m_foreign; };

	bool isDataLoaded() {return (m_pCard!=NULL);}

private:
	tFieldMap m_Fields;
	bool m_foreign;
};


//**************************************************
// Person biometric information
// This class contains all biometric data
//**************************************************
class BiometricInfo
{
public:
	//----------------------------------------------
	// ctor
	//----------------------------------------------
	BiometricInfo( void )
	{
	}

	//----------------------------------------------
	// dtor
	//----------------------------------------------
	virtual ~BiometricInfo( void )
	{
	}

	//----------------------------------------------
	// Reset the biometric data
	//----------------------------------------------
	void Reset( void )
	{
		m_pPictureData.clear();
	}
	//----------------------------------------------
	// retrieve data from EID card
	//----------------------------------------------
	bool RetrieveData( PTEID_EIDCard& Card	)
	{
		bool					bRetVal = false;

		PTEID_EId& pteid_eid = Card.getID();
		PTEID_ByteArray & p = pteid_eid.getPhotoObj().getphoto();
		m_pPictureData = QByteArray((const char *)p.GetBytes(),p.Size());
		bRetVal = true;

		//delete(&p);//LL
		return bRetVal;
	}

public:
	QByteArray m_pPictureData;			//!< picture on card in png format
};

class PersoDataInfo
{
public:
#define PERSODATA_INFO                  "persodata_info"

    //----------------------------------------------
    // ctor
    //----------------------------------------------
    PersoDataInfo( void )
    {
    }

    //----------------------------------------------
    // dtor
    //----------------------------------------------
    virtual ~PersoDataInfo( void )
    {
    }

    //----------------------------------------------
    // Reset data to default values
    //----------------------------------------------
    void Reset( void )
    {
            m_Fields.clear();
    }

    //----------------------------------------------
    // Retrieve data from EID card
    //----------------------------------------------
    bool RetrieveData(PTEID_EIDCard& Card)
    {
    	bool            bRetVal = false;

    	m_Fields[PERSODATA_INFO]	=  Card.readPersonalNotes();
    	return bRetVal;
	}
    //----------------------------------------------
    // get reference to all fields
    //----------------------------------------------
    tFieldMap& getFields( void )
    {
    	return m_Fields;
    }

private:
		tFieldMap m_Fields;
};

//**************************************************
// Person information
// This class contains all the person specific data
//**************************************************
class PersonInfo
{
public:
#define NAME				"name"
#define GIVENNAME			"givenname"
#define BIRTHPLACE			"birthplace"
#define BIRTHDATE			"birthdate"
#define COUNTRY				"country"
#define SEX					"sex"
#define TITLE				"title"
#define NATIONALITY			"nationality"
#define NATIONALNUMBER		"nationalnumber"
#define HEIGHT				"height"
#define DOCUMENTNUMBER  	"documentnumber"
#define PARENTS				"parents"
#define TAXNO				"taxno"
#define HEALTHNO			"healthno"
#define SOCIALSECURITYNO 	"socialsecurityno"
#define CARDVERSION			"cardversion"
#define DOCUMENTTYPE		"documenttype"
#define ISSUINGENTITY		"issuingentity"
#define PHOTO				"photo"
#define PARENTS				"parents"
#define MOTHER				"mother"
#define FATHER				"father"
#define LOCALOFREQUEST		"localofrequest"
#define VALIDATION			"validation"
#define MRZ1				"MRZ1"
#define MRZ2				"MRZ2"
#define MRZ3				"MRZ3"
#define ACCIDENTALINDICATIONS "ACCIDENTALINDICATIONS"
#define LINK_TO_CERT        "LINK_TO_CERT"


#define INITIALS		"initials"

	//----------------------------------------------
	// ctor
	//----------------------------------------------
	PersonInfo( void )
	{
	}

	//----------------------------------------------
	// dtor
	//----------------------------------------------
	virtual ~PersonInfo( void )
	{
	}

	//----------------------------------------------
	// reset all data to default
	//----------------------------------------------
	void Reset( void )
	{
		m_Fields.clear();
		//m_AddressInfo.Reset();
        //m_RelativesInfo.Reset();
		m_BiometricInfo.Reset();
		m_PersoDataInfo.Reset();
	}

	bool isExpiredDate(const char *in_strDate) {
        if ( in_strDate == NULL ) return false;

        QDate qDate = QDate::fromString( in_strDate, "dd MM yyyy" );
        QDate curDate = QDate::currentDate();

        if ( curDate > qDate ) return true;
        return false;
    }

	//----------------------------------------------
	// retrieve data from EID card
	//----------------------------------------------
	bool RetrieveData(PTEID_EIDCard& Card)
	{
		bool bRetVal = false;
		unsigned int fatherlen, motherlen;

		//Enable SOD checking
		Card.doSODCheck(true);

		PTEID_EId& pteid_eid = Card.getID();

		m_Fields[NAME]						= pteid_eid.getSurname();
		m_Fields[GIVENNAME]					= pteid_eid.getGivenName();
		m_Fields[NATIONALITY]				= pteid_eid.getNationality();
		m_Fields[NATIONALNUMBER]			= pteid_eid.getCivilianIdNumber();
		m_Fields[BIRTHDATE]					= pteid_eid.getDateOfBirth();
		m_Fields[COUNTRY]					= pteid_eid.getCountry();
		m_Fields[SEX]						= pteid_eid.getGender();
		m_Fields[HEIGHT]					= pteid_eid.getHeight();
		m_Fields[DOCUMENTNUMBER] 			= pteid_eid.getDocumentNumber();
		m_Fields[TAXNO]						= pteid_eid.getTaxNo();
		m_Fields[HEALTHNO]					= pteid_eid.getHealthNumber();
		m_Fields[SOCIALSECURITYNO]			= pteid_eid.getSocialSecurityNumber();
		m_Fields[CARDVERSION]				= pteid_eid.getDocumentVersion();
		m_Fields[DOCUMENTTYPE]				= pteid_eid.getDocumentType();
		m_Fields[ISSUINGENTITY]				= pteid_eid.getIssuingEntity();

		fatherlen = strlen(pteid_eid.getGivenNameFather()) + strlen(pteid_eid.getSurnameFather()) + 2;

		char* father = new char[fatherlen];
		strcpy(father, pteid_eid.getGivenNameFather());
		strcat(father, " ");
		strcat(father, pteid_eid.getSurnameFather());

		m_Fields[FATHER] = father;

		motherlen = strlen(pteid_eid.getGivenNameMother()) + strlen(pteid_eid.getSurnameMother()) + 2;

		char* mother = new char[motherlen];
		strcpy(mother, pteid_eid.getGivenNameMother());
		strcat(mother, " ");
		strcat(mother, pteid_eid.getSurnameMother());

		m_Fields[MOTHER] = mother;

		m_Fields[LOCALOFREQUEST]			= pteid_eid.getLocalofRequest();

		if ( !Card.isActive() ) {
           m_Fields[VALIDATION]	= "The Citizen Card is not active";
            m_Fields[LINK_TO_CERT] = "";
		}
		else {
            bool bIsExpired = isExpiredDate( pteid_eid.getValidityEndDate() );
            if ( bIsExpired ){
                m_Fields[VALIDATION] = "The Citizen Card is expired";
                m_Fields[LINK_TO_CERT] = "";
            } else{
                m_Fields[VALIDATION] = "The Citizen Card has been activated";
                m_Fields[LINK_TO_CERT] = "To verify that the card is not suspended or revoked,you should validate the certificates on the Certificates tab";
            }
		}

		m_Fields[MRZ1]						= pteid_eid.getMRZ1();
		m_Fields[MRZ2]						= pteid_eid.getMRZ2();
		m_Fields[MRZ3]						= pteid_eid.getMRZ3();
		m_Fields[ACCIDENTALINDICATIONS]		= pteid_eid.getAccidentalIndications();

		m_BiometricInfo.RetrieveData(Card);

		delete[] father;//LL
		delete[] mother;//LL

		return bRetVal;
	}

	//----------------------------------------------
	// get reference to all fields
	//----------------------------------------------
	const tFieldMap getFields( void ) const
	{
		return m_Fields;
	}

	bool isMale( void )
	{
		bool bRetVal = false;

		QString sex = m_Fields[SEX];
		if ( sex != "" )
		{
			if (sex == "1")		bRetVal=true;
			if (sex == "M")		bRetVal=true;
			if (sex == "Male")	bRetVal=true;
		}
		return bRetVal;
	}

	bool isFemale( void )
	{
		bool bRetVal = false;

		QString sex = m_Fields[SEX];
		if ( sex != "" )
		{
			if (sex == "2")	bRetVal=true;
			if (sex == "F")	bRetVal=true;
			if (sex == "V")	bRetVal=true;
			if (sex == "W")	bRetVal=true;
		}
		return bRetVal;
	}
	static QString formatNationalNumber( QString const& number)
	{
		QString formatted="";

		if (number.length()==0)
		{
			return formatted;
		}

		return formatted;
	}

public:
	//AddressInfo		m_AddressInfo;		//!< address info
	//RelativesInfo	m_RelativesInfo;		//!< person relatives info
	BiometricInfo	m_BiometricInfo;		//!< person biometric data
	PersoDataInfo   m_PersoDataInfo;		//!< personal data info

private:
	tFieldMap m_Fields;
};

//**************************************************
// Certificate info on the card
//**************************************************
class CertifInfo
{
public:
	CertifInfo( void )
		: m_pCertificates(NULL)
		//, m_pSignatureCert(NULL)
		//, m_pAuthenticationCert(NULL)
	{
	}

	virtual ~CertifInfo( void )
	{
	}

    //void Reset( void )
    //{
    //    m_MainCertifs.clear();
    //}

    bool RetrieveData( PTEID_EIDCard& card )
	{
		m_pCertificates = &card.getCertificates();
		return true;
	}

	PTEID_Certificates* getCertificates( void )
	{
		return m_pCertificates;
	}

private:
	PTEID_Certificates* m_pCertificates;
	//QList<PTEID_Certificate *> m_MainCertifs;

/*
    PTEID_Certificate*  m_pSignatureCert;
    PTEID_Certificate*  m_pAuthenticationCert;
*/
};

//**************************************************
// Pins info on the card
//**************************************************
/*class PinsInfo
{
public:
	PinsInfo( void )
	{
	}
	virtual ~PinsInfo( void )
	{
	}
};*/

//**************************************************
// Base class containing the card information
// The data for the cards is spread over several classes to separate
// the type of data
// - CardInfo	: data of the card itself
// - PersonInfo : data about the person
// - AddressInfo: address data of the person
// - RelativesInfo: info about the persons relatives
// - PersonExtraInfo: additional data on the person
// - MiscInfo	: miscellaneous info
//**************************************************
class CardInformation
{
public:
	//----------------------------------------------
	// ctor
	//----------------------------------------------
	CardInformation( void );

	//----------------------------------------------
	// dtor
	//----------------------------------------------
	virtual ~CardInformation( void );

	//----------------------------------------------
	// Reset to default values
	//----------------------------------------------
	void Reset( void );

	//----------------------------------------------
	// Load the data from an EID card
	//----------------------------------------------
	bool LoadData             (PTEID_EIDCard& Card, QString const& cardReader );
	bool LoadDataAddress      (PTEID_EIDCard& Card, QString const& cardReader );
	bool LoadDataPersoData    (PTEID_EIDCard& Card, QString const& cardReader );
	bool LoadDataCertificates (PTEID_EIDCard& Card, QString const& cardReader );

	//----------------------------------------------
	// Load the data from a memory card
	//----------------------------------------------
	bool LoadData(PTEID_MemoryCard& Card, QString const& cardReader);

	bool isDataLoaded() {return (m_pCard!=NULL);} //return m_bDataLoaded;}

private:
	//----------------------------------------------
	// retrieve data from EID card
	//----------------------------------------------
	bool RetrieveData( PTEID_EIDCard& Card )
	{
		bool bRetVal = false;
		bRetVal |= m_CardInfo.RetrieveData(Card);
		bRetVal |= m_PersonInfo.RetrieveData(Card);
		bRetVal |= m_CertifInfo.RetrieveData(Card);

		m_pCard = &Card;

		return bRetVal;
	}

	bool RetrieveDataAddress (PTEID_EIDCard& Card)
	{
		bool bRetVal = false;
		
		bRetVal |= m_AddressInfo.RetrieveDataAddress(Card);		
		
		m_pCard = &Card;

		return bRetVal;
	}

	bool RetrieveDataPersoData (PTEID_EIDCard& Card)
	{
		bool bRetVal = false;
		
		bRetVal |= m_PersoDataInfo.RetrieveData (Card);


		//bRetVal |= m_PinsInfo.RetrieveData(pCard);
		//bRetVal |= m_CertifInfo.RetrieveDataCertificates(Card);
		m_pCard = &Card;

		return bRetVal;
	}

	bool RetrieveDataCertificates (PTEID_EIDCard& Card)
	{
		bool bRetVal = false;
		
		bRetVal |= m_CertifInfo.RetrieveData (Card);

		bRetVal |= m_CertifInfo.RetrieveData(Card);
		m_pCard = &Card;

		return bRetVal;
	}

public:
	CardInfo		m_CardInfo;				//!< card specific data
	PersonInfo		m_PersonInfo;			//!< person info
	AddressInfo		m_AddressInfo;			//!< Address info
	PersoDataInfo	m_PersoDataInfo;		//!< info on the PersoData

	//PinsInfo		m_PinsInfo;				//!< info on all the pins
	CertifInfo		m_CertifInfo;			//!< info on the certificates

	PTEID_Card*		m_pCard;				//!< pointer to the card this data belongs to
	QString			m_cardReader;			//!< cardreader from which we read the data
};

#endif
