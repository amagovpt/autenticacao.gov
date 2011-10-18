//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to read a Belgian eID card.
// The program will dump the data to the standard output for each card reader
// it finds.
//
// compile:
//    javac -classpath <path_to>\beid[35]libJava.jar main.java
//
// run (windows):
//    set PATH=<path_to_dll's>;%PATH%
//    java -cp <path_to>\beid35libJava.jar;.  main
//*****************************************************************************

import java.lang.*;
import be.belgium.eid.*;

public class main
{
	//*****************************************************************************
	// Get the data and dump to the screen
	// Beware: The data coming from the cards is encoded in UTF8!
	//*****************************************************************************
	private static void getSISData(BEID_SISCard card) throws Exception
	{
		BEID_SisId sisId = card.getID();

		System.out.println();

		System.out.println("\tPeronal data:");
		System.out.println( "\t-------------"		     );
		System.out.println( "\tName                 : " + sisId.getName());
		System.out.println( "\tSurname              : " + sisId.getSurname());
		System.out.println( "\tInitials             : " + sisId.getInitials());
		System.out.println( "\tGender               : " + sisId.getGender());
		System.out.println( "\tDateOfBirth          : " + sisId.getDateOfBirth());
		System.out.println( "\tSocialSecurityNumber : " + sisId.getSocialSecurityNumber());

		System.out.println();

		System.out.println( "\tCard data:");
		System.out.println( "\t----------");
		System.out.println( "\tLogicalNumber        : " + sisId.getLogicalNumber());
		System.out.println( "\tDateOfIssue          : " + sisId.getDateOfIssue());
		System.out.println( "\tValidityBeginDate    : " + sisId.getValidityBeginDate());
		System.out.println( "\tValidityEndDate      : " + sisId.getValidityEndDate());
	}

	//*****************************************************************************
	// Get the data from a Belgian SIS card
	//*****************************************************************************
	private static void getSISCardData(BEID_ReaderContext readerContext) throws Exception
	{
		BEID_SISCard card = readerContext.getSISCard();
		getSISData( card );
	}

	//*****************************************************************************
	// Get the data and dump to the screen
	// Beware: The data coming from the cards is encoded in UTF8!
	//*****************************************************************************
	private static void getEIDData(BEID_EIDCard card) throws Exception
	{
		BEID_EId	  eid  = card.getID();

		if ( card.isTestCard() )
		{
			card.setAllowTestCard(true);
			System.out.println( "" );
			System.out.println( "Warning: This is a test card.");
		}

		System.out.println( "\tDocumentVersion    : " + eid.getDocumentVersion() );
		System.out.println( "\tDocumentType       : " + eid.getDocumentType() );

		System.out.println( );

		System.out.println( "\tPeronal data:" );
		System.out.println( "\t-------------" );
		System.out.println( "\tFirstName1          : " + eid.getFirstName1()			);
		System.out.println( "\tSurname            : " + eid.getSurname()				);
		System.out.println( "\tGender             : " + eid.getGender()				);
		System.out.println( "\tDateOfBirth        : " + eid.getDateOfBirth()			);
		System.out.println( "\tLocationOfBirth    : " + eid.getLocationOfBirth()		);
		System.out.println( "\tNobility           : " + eid.getNobility()				);
		System.out.println( "\tNationality        : " + eid.getNationality()			);
		System.out.println( "\tNationalNumber     : " + eid.getNationalNumber()		);
		System.out.println( "\tSpecialOrganization: " + eid.getSpecialOrganization()	);
		System.out.println( "\tMemberOfFamily     : " + eid.getMemberOfFamily()		);
		System.out.println( "\tAddressVersion     : " + eid.getAddressVersion()		);
		System.out.println( "\tStreet             : " + eid.getStreet()				);
		System.out.println( "\tZipCode            : " + eid.getZipCode()				);
		System.out.println( "\tMunicipality       : " + eid.getMunicipality()			);
		System.out.println( "\tCountry            : " + eid.getCountry()				);
		System.out.println( "\tSpecialStatus      : " + eid.getSpecialStatus()		);

		System.out.println( "" );

		System.out.println( "\tCard data:"		       );
		System.out.println( "\t----------"		       );
		System.out.println( "\tLogicalNumber      : " + eid.getLogicalNumber()		);
		System.out.println( "\tChipNumber         : " + eid.getChipNumber()			);
		System.out.println( "\tValidityBeginDate  : " + eid.getValidityBeginDate()	);
		System.out.println( "\tValidityEndDate    : " + eid.getValidityEndDate()		);
		System.out.println( "\tIssuingMunicipality: " + eid.getIssuingMunicipality()	);
	}

	//*****************************************************************************
	// Get the data from a Belgian kids EID card
	//*****************************************************************************
	private static void getKidsCardData(BEID_ReaderContext readerContext) throws Exception
	{
		BEID_KidsCard card = readerContext.getKidsCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// Get the data from a Belgian foreigner EID card
	//*****************************************************************************
	private static void getForeignerCardData(BEID_ReaderContext readerContext) throws Exception
	{
		BEID_ForeignerCard card = readerContext.getForeignerCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// Get the data from a Belgian EID card
	//*****************************************************************************
	private static void getEidCardData(BEID_ReaderContext readerContext) throws Exception
	{
		BEID_EIDCard card = readerContext.getEIDCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// get a string representation of the card type
	//*****************************************************************************
	private static String getCardTypeStr(BEID_ReaderContext readerContext) throws Exception
	{
		String		strCardType="UNKNOWN";
		BEID_CardType	cardType = readerContext.getCardType();

		if (cardType == BEID_CardType.BEID_CARDTYPE_EID)
		{
			strCardType = "BEID_CARDTYPE_EID";
		}
		else if (cardType == BEID_CardType.BEID_CARDTYPE_KIDS)
		{
			strCardType = "BEID_CARDTYPE_KIDS";
		}
		else if (cardType == BEID_CardType.BEID_CARDTYPE_FOREIGNER)
		{
			strCardType = "BEID_CARDTYPE_FOREIGNER";
		}
		else if (cardType == BEID_CardType.BEID_CARDTYPE_SIS)
		{
			strCardType = "BEID_CARDTYPE_SIS";
		}
		else
		{
			strCardType = "BEID_CARDTYPE_UNKNOWN";
		}
		return strCardType;
	}

	//*****************************************************************************
	// Show the info of the card in the reader
	//*****************************************************************************
	private static void showCardInfo(String readerName) throws Exception
	{
		BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByName( readerName );
		if ( readerContext.isCardPresent() )
		{
			System.out.println("\tType               : " + getCardTypeStr(readerContext));

			BEID_CardType cardType = readerContext.getCardType();

			if (cardType == BEID_CardType.BEID_CARDTYPE_EID)
			{
				getEidCardData(readerContext);
			}
			else if (cardType == BEID_CardType.BEID_CARDTYPE_KIDS)
			{
				getKidsCardData(readerContext);
			}
			else if (cardType == BEID_CardType.BEID_CARDTYPE_FOREIGNER)
			{
				getForeignerCardData(readerContext);
			}
			else if (cardType == BEID_CardType.BEID_CARDTYPE_SIS)
			{
				getSISCardData(readerContext);
			}
			else
			{

			}
		}
	}

	//*****************************************************************************
	// Show the reader info an get the data of the card if present
	//*****************************************************************************
	private static void showReaderCardInfo(String readerName) throws Exception
	{
		BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByName( readerName );

		System.out.println("Reader: "+readerName);
		System.out.println("\tCard present: " + (readerContext.isCardPresent()? "yes" :"no"));

		showCardInfo( readerName );

		System.out.println("");
	}

	//*****************************************************************************
	// scan all the card readers and if a card is present, show the content of the
	// card.
	//*****************************************************************************
	private static void scanReaders() throws Exception
	{
		long nrReaders  = BEID_ReaderSet.instance().readerCount();
		System.out.println("Nr of card readers detected: "+nrReaders);

		for ( int readerIdx=0; readerIdx<nrReaders; readerIdx++)
		{
			String readerName = BEID_ReaderSet.instance().getReaderName(readerIdx);
			showReaderCardInfo(readerName);
		}
	}

	//*****************************************************************************
	// Main entry point
	//*****************************************************************************
	public static void main(String argv[])
	{
		System.out.println("[Info]  eID SDK sample program: read_eid");

		String osName = System.getProperty("os.name");

		if ( -1 != osName.indexOf("Windows") )
		{
			System.out.println("[Info]  Windows system!!");
			System.loadLibrary("beid35libJava_Wrapper");
		}
		else
		{
			System.loadLibrary("beidlibJava_Wrapper");
		}

		try
		{
			BEID_ReaderSet.initSDK();
			scanReaders();
		}
		catch( BEID_Exception e)
		{
			System.out.println("[Catch] BEID_Exception:" + e.GetError());
		}
		catch( Exception e)
		{
			System.out.println("[Catch] Exception:" + e.getMessage());
		}


		try
		{
			BEID_ReaderSet.releaseSDK();
		}
		catch (BEID_Exception e)
		{
			System.out.println("[Catch] BEID_Exception:" + e.GetError());
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception:" + e.getMessage());
		}
	}
}
