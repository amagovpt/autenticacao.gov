#pragma once

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