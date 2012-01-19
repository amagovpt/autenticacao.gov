//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to read a Belgian eID card.
// The program will dump the data to the standard output for each card reader
// it finds.
//*****************************************************************************
#include <iostream>
#include "eidlib.h"

using namespace eIDMW;

//*****************************************************************************
// UTF8 to Ansi
//*****************************************************************************
std::string utf8ToAnsi( const char* pUtf8)
{
int i=0;
char ansi[128];
char* pAnsi = ansi;

	while (pUtf8[i]) 
	{
		if ((pUtf8[i] & 0x80) == 0) 
		{ // ASCII : 0000 0000-0000 007F 0xxxxxxx
			*pAnsi = pUtf8[i];
			++i;
		} 
		else if ((pUtf8[i] & 0xE0) == 0xC0) 
		{ // 0000 0080-0000 07FF 110xxxxx 10xxxxxx
			*pAnsi = (pUtf8[i] & 0x1F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 1] & 0x3F);
			i += 2;
		} 
		else if ((pUtf8[i] & 0xF0) == 0xE0) 
		{ // 0000 0800-0000 FFFF 1110xxxx 10xxxxxx 10xxxxxx
			*pAnsi = (pUtf8[i] & 0x0F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 1] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 2] & 0x3F);
			i += 3;
		} 
		else if ((pUtf8[i] & 0xF8) == 0xF0) 
		{ // 0001 0000-001F FFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			*pAnsi = (pUtf8[i] & 0x07);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 1] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 2] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 3] & 0x3F);
			i += 4;
		} 
		else if ((pUtf8[i] & 0xFC) == 0xF8) 
		{ // 0020 0000-03FF FFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			*pAnsi = (pUtf8[i] & 0x03);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 1] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 2] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 3] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 4] & 0x3F);
			i += 5;
		} 
		else if ((pUtf8[i] & 0xFE) == 0xFC) 
		{ // 0400 0000-7FFF FFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			*pAnsi = (pUtf8[i] & 0x01);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 1] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 2] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 3] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 4] & 0x3F);
			*pAnsi <<= 6;
			*pAnsi += (pUtf8[i + 5] & 0x3F);
			i += 6;
		} 
		else 
		{
			*pAnsi = '?';
		}
		pAnsi++;
	}
	*pAnsi = 0;
	std::string strAnsi(ansi);
	return strAnsi;
}

//*****************************************************************************
// Get the data and dump to the screen
// Beware: The data coming from the cards is encoded in UTF8!
//*****************************************************************************
void getSISData( PTEID_SISCard& card )
{
	PTEID_SisId& sisId = card.getID();

	std::cout << "\tPersonal data:"		     << std::endl;
	std::cout << "\t-------------"		     << std::endl;
	std::cout << "\tName                 : " << utf8ToAnsi(sisId.getName())		<< std::endl;
	std::cout << "\tSurname              : " << sisId.getSurname()              << std::endl;
	std::cout << "\tInitials             : " << sisId.getInitials()             << std::endl;
	std::cout << "\tGender               : " << sisId.getGender()               << std::endl;
	std::cout << "\tDateOfBirth          : " << sisId.getDateOfBirth()          << std::endl;
	std::cout << "\tSocialSecurityNumber : " << sisId.getSocialSecurityNumber() << std::endl;

	std::cout << "\tCard data:"		         << std::endl;
	std::cout << "\t----------"		         << std::endl;
	std::cout << "\tLogicalNumber        : " << sisId.getLogicalNumber()        << std::endl;
	std::cout << "\tDateOfIssue          : " << sisId.getDateOfIssue()          << std::endl;
	std::cout << "\tValidityBeginDate    : " << sisId.getValidityBeginDate()    << std::endl;
	std::cout << "\tValidityEndDate      : " << sisId.getValidityEndDate()      << std::endl;

}

//*****************************************************************************
// Get the data from a Belgian SIS card
//*****************************************************************************
void getSISCardData( PTEID_ReaderContext& readerContext )
{
	PTEID_SISCard& card = readerContext.getSISCard();
	getSISData( card );
}

//*****************************************************************************
// Get the data and dump to the screen
// Beware: The data coming from the cards is encoded in UTF8!
//*****************************************************************************
void getEIDData( PTEID_EIDCard& card )
{
	PTEID_EId&	  eid  = card.getID();

	if ( card.isTestCard() )
	{
		std::cout << std::endl;
		std::cout << "Error: This is a test card. Can not read data..." << std::endl;
		return;
	}

	std::cout << "\tDocumentVersion    : " << eid.getDocumentVersion() << std::endl;
	std::cout << "\tDocumentType       : " << eid.getDocumentType() << std::endl;

	std::cout << std::endl;

	std::cout << "\tPersonal data:"		   << std::endl;
	std::cout << "\t-------------"		   << std::endl;
	std::cout << "\tFirstName          : " << eid.getFirstName()			<< std::endl;
	std::cout << "\tSurname            : " << eid.getSurname()				<< std::endl;
	std::cout << "\tGender             : " << eid.getGender()				<< std::endl;
	std::cout << "\tDateOfBirth        : " << eid.getDateOfBirth()			<< std::endl;
	std::cout << "\tLocationOfBirth    : " << eid.getLocationOfBirth()		<< std::endl;
	std::cout << "\tNationality        : " << eid.getNationality()			<< std::endl;
	std::cout << "\tNationalNumber     : " << eid.getCivilianIdNumber()		<< std::endl;
	std::cout << "\tSpecialOrganization: " << eid.getSpecialOrganization()	<< std::endl;
	std::cout << "\tMemberOfFamily     : " << eid.getMemberOfFamily()		<< std::endl;
	std::cout << "\tAddressVersion     : " << eid.getAddressVersion()		<< std::endl;
	std::cout << "\tStreet             : " << eid.getStreet()				<< std::endl;
	std::cout << "\tZipCode            : " << eid.getZipCode()				<< std::endl;
	std::cout << "\tMunicipality       : " << eid.getMunicipality()			<< std::endl;
	std::cout << "\tCountry            : " << eid.getCountry()				<< std::endl;
	std::cout << "\tSpecialStatus      : " << eid.getSpecialStatus()		<< std::endl;


	std::cout << std::endl;

	std::cout << "\tCard data:"		       << std::endl;
	std::cout << "\t----------"		       << std::endl;
	std::cout << "\tLogicalNumber      : " << eid.getLogicalNumber()		<< std::endl;
	std::cout << "\tChipNumber         : " << eid.getDocumentPAN()			<< std::endl;
	std::cout << "\tValidityBeginDate  : " << eid.getValidityBeginDate()	<< std::endl;
	std::cout << "\tValidityEndDate    : " << eid.getValidityEndDate()		<< std::endl;
	std::cout << "\tIssuingMunicipality: " << eid.getIssuingMunicipality()	<< std::endl;
}

//*****************************************************************************
// Get the data from a Belgian kids EID card
//*****************************************************************************
void getKidsCardData( PTEID_ReaderContext& readerContext )
{
	PTEID_KidsCard& card = readerContext.getKidsCard();
	getEIDData(card);
}

//*****************************************************************************
// Get the data from a Belgian foreigner EID card
//*****************************************************************************
void getForeignerCardData( PTEID_ReaderContext& readerContext )
{
	PTEID_ForeignerCard& card = readerContext.getForeignerCard();
	getEIDData(card);
}
//*****************************************************************************
// Get the data from a Belgian EID card
//*****************************************************************************
void getEidCardData( PTEID_ReaderContext& readerContext )
{
	PTEID_EIDCard& card = readerContext.getEIDCard();
	getEIDData(card);
}

//*****************************************************************************
// get a string representation of the card type
//*****************************************************************************
std::string getCardTypeStr( PTEID_ReaderContext& readerContext )
{
	std::string		strCardType;
	PTEID_CardType	cardType = readerContext.getCardType();

	switch(cardType)
	{
	case PTEID_CARDTYPE_IAS07:
		strCardType = "PTEID_CARDTYPE_IAS07";
		break;
	case PTEID_CARDTYPE_IAS101:
		strCardType = "PTEID_CARDTYPE_IAS101";
		break;
	case PTEID_CARDTYPE_KIDS:
		strCardType = "PTEID_CARDTYPE_KIDS";
		break;
	case PTEID_CARDTYPE_FOREIGNER:
		strCardType = "PTEID_CARDTYPE_FOREIGNER";
		break;
	case PTEID_CARDTYPE_SIS:
		strCardType = "PTEID_CARDTYPE_SIS";
		break;
	case PTEID_CARDTYPE_UNKNOWN:
	default:
		strCardType = "PTEID_CARDTYPE_UNKNOWN";
		break;
	}
	return strCardType;
}

//*****************************************************************************
// Show the info of the card in the reader
//*****************************************************************************
void showCardInfo( const char* readerName )
{
	PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName( readerName );
	if ( readerContext.isCardPresent() )
	{
		std::cout << "\tType               : " << getCardTypeStr( readerContext ) << std::endl;

		switch(readerContext.getCardType())
		{
		case PTEID_CARDTYPE_IAS07:
		case PTEID_CARDTYPE_IAS101:
			getEidCardData( readerContext );
			break;
		case PTEID_CARDTYPE_KIDS:
			getKidsCardData( readerContext );
			break;
		case PTEID_CARDTYPE_FOREIGNER:
			getForeignerCardData( readerContext );
			break;
		case PTEID_CARDTYPE_SIS:
			getSISCardData( readerContext );
			break;
		case PTEID_CARDTYPE_UNKNOWN:
		default:
			break;
		}
	}
}

//*****************************************************************************
// Show the reader info an get the data of the card if present
//*****************************************************************************
void showReaderCardInfo( const char* readerName )
{
	PTEID_ReaderContext& readerContext = ReaderSet.getReaderByName( readerName );

	std::cout << "Reader: "			<< readerName << std::endl;
	std::cout << "\tCard present: " << (readerContext.isCardPresent()? "yes" :"no") << std::endl;
	
	showCardInfo( readerName );
	
	std::cout << std::endl;
}

//*****************************************************************************
// scan all the card readers and if a card is present, show the content of the
// card.
//*****************************************************************************
void scanReaders( void )
{
	unsigned long		nrReaders  = ReaderSet.readerCount();
	const char* const*	readerList = ReaderSet.readerList();

	std::cout << "Nr of card readers detected: " << nrReaders << std::endl;

	for ( size_t readerIdx=0; readerIdx<nrReaders; readerIdx++)
	{
		showReaderCardInfo( readerList[readerIdx] );
	}
}

//*****************************************************************************
// Main entry point
//*****************************************************************************
int main( int argc, char* argv[] )
{
	std::cout << "eID SDK sample program: read_eid" << std::endl;

	scanReaders();

	PTEID_ReleaseSDK();

}

