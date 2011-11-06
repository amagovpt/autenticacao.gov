//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to read a Belgian eID card.
// The program will dump the data to the standard output for each card reader
// it finds.
//
// compile:
//    javac -classpath <path_to>\pteid[35]libJava.jar main.java
//
// run (windows):
//    set PATH=<path_to_dll's>;%PATH%
//    java -cp <path_to>\pteid35libJava.jar;.  main
//*****************************************************************************

import java.lang.*;
import be.portugal.eid.*;

public class main
{
	//*****************************************************************************
	// Get the data and dump to the screen
	// Beware: The data coming from the cards is encoded in UTF8!
	//*****************************************************************************
	private static void getSISData(PTEID_SISCard card) throws Exception
	{
		PTEID_SisId sisId = card.getID();

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
	private static void getSISCardData(PTEID_ReaderContext readerContext) throws Exception
	{
		PTEID_SISCard card = readerContext.getSISCard();
		getSISData( card );
	}

	//*****************************************************************************
	// Get the data and dump to the screen
	// Beware: The data coming from the cards is encoded in UTF8!
	//*****************************************************************************
	private static void getEIDData(PTEID_EIDCard card) throws Exception
	{
		PTEID_EId	  eid  = card.getID();

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
	private static void getKidsCardData(PTEID_ReaderContext readerContext) throws Exception
	{
		PTEID_KidsCard card = readerContext.getKidsCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// Get the data from a Belgian foreigner EID card
	//*****************************************************************************
	private static void getForeignerCardData(PTEID_ReaderContext readerContext) throws Exception
	{
		PTEID_ForeignerCard card = readerContext.getForeignerCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// Get the data from a Belgian EID card
	//*****************************************************************************
	private static void getEidCardData(PTEID_ReaderContext readerContext) throws Exception
	{
		PTEID_EIDCard card = readerContext.getEIDCard();
		getEIDData(card);
	}

	//*****************************************************************************
	// get a string representation of the card type
	//*****************************************************************************
	private static String getCardTypeStr(PTEID_ReaderContext readerContext) throws Exception
	{
		String		strCardType="UNKNOWN";
		PTEID_CardType	cardType = readerContext.getCardType();

		if (cardType == PTEID_CardType.PTEID_CARDTYPE_EID)
		{
			strCardType = "PTEID_CARDTYPE_EID";
		}
		else if (cardType == PTEID_CardType.PTEID_CARDTYPE_KIDS)
		{
			strCardType = "PTEID_CARDTYPE_KIDS";
		}
		else if (cardType == PTEID_CardType.PTEID_CARDTYPE_FOREIGNER)
		{
			strCardType = "PTEID_CARDTYPE_FOREIGNER";
		}
		else if (cardType == PTEID_CardType.PTEID_CARDTYPE_SIS)
		{
			strCardType = "PTEID_CARDTYPE_SIS";
		}
		else
		{
			strCardType = "PTEID_CARDTYPE_UNKNOWN";
		}
		return strCardType;
	}

	//*****************************************************************************
	// Show the info of the card in the reader
	//*****************************************************************************
	private static void showCardInfo(String readerName) throws Exception
	{
		PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReaderByName( readerName );
		if ( readerContext.isCardPresent() )
		{
			System.out.println("\tType               : " + getCardTypeStr(readerContext));

			PTEID_CardType cardType = readerContext.getCardType();

			if (cardType == PTEID_CardType.PTEID_CARDTYPE_EID)
			{
				getEidCardData(readerContext);
			}
			else if (cardType == PTEID_CardType.PTEID_CARDTYPE_KIDS)
			{
				getKidsCardData(readerContext);
			}
			else if (cardType == PTEID_CardType.PTEID_CARDTYPE_FOREIGNER)
			{
				getForeignerCardData(readerContext);
			}
			else if (cardType == PTEID_CardType.PTEID_CARDTYPE_SIS)
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
		PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReaderByName( readerName );

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
		long nrReaders  = PTEID_ReaderSet.instance().readerCount();
		System.out.println("Nr of card readers detected: "+nrReaders);

		for ( int readerIdx=0; readerIdx<nrReaders; readerIdx++)
		{
			String readerName = PTEID_ReaderSet.instance().getReaderName(readerIdx);
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
			System.loadLibrary("pteid35libJava_Wrapper");
		}
		else
		{
			System.loadLibrary("pteidlibJava_Wrapper");
		}

		try
		{
			PTEID_ReaderSet.initSDK();
			scanReaders();
		}
		catch( PTEID_Exception e)
		{
			System.out.println("[Catch] PTEID_Exception:" + e.GetError());
		}
		catch( Exception e)
		{
			System.out.println("[Catch] Exception:" + e.getMessage());
		}


		try
		{
			PTEID_ReaderSet.releaseSDK();
		}
		catch (PTEID_Exception e)
		{
			System.out.println("[Catch] PTEID_Exception:" + e.GetError());
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception:" + e.getMessage());
		}
	}
}
