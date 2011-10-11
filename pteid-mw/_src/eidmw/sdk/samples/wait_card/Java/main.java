//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to add events on a card reader
//*****************************************************************************

import java.lang.*;
import java.awt.Toolkit;
import be.portugal.eid.*;

public class main
{

	//*****************************************************************************
	// WaitForCardPresent
	// This function waits until a card has been inserted into a reader
	// Or for a maximum of Timeout seconds
	// It returns the reader context or throw an exception on timeout
	//*****************************************************************************
	private static PTEID_ReaderContext WaitForCardPresent(int Timeout) throws PTEID_ExNoReader,PTEID_Exception,Exception
	{
		int 	Count	  = 0;
		boolean bContinue = true;
		boolean bRetVal	  = false;

		PTEID_ReaderContext reader = null;

		while(bContinue)
		{
			reader = PTEID_ReaderSet.instance().getReader();
			if(reader.isCardPresent())
			{
				bRetVal = true;
				break;
			}
			else if (Count > Timeout)
			{
				bRetVal = false;
				break;
			}
			else
			{
				Thread.currentThread().sleep(1000);
				Count++;
			}
		}

		if( bRetVal )
		{
			return reader;
		}
		Exception e = new Exception("Timeout");
		throw(e);
	}
	//*****************************************************************************
	// WaitForCardAbsent Function
	// This function hangs until the card is retracted from the cardreader
	//*****************************************************************************
	private static boolean WaitForCardAbsent(int Timeout) throws PTEID_ExNoReader,PTEID_Exception,Exception
	{
		PTEID_ReaderContext reader = null;
		int Count = 0;

		while (true)
		{
			reader = PTEID_ReaderSet.instance().getReader();
			if (!reader.isCardPresent())
			{
				return true;
			}
			else if (Count > Timeout)
			{
				return false;
			}
			else
			{
				Thread.currentThread().sleep(1000);
				Count++;
			}
		}
	}

	//*****************************************************************************
	// Main entry point
	//*****************************************************************************
	public static void main(String argv[])
	{
		System.out.println("[Info]  eID SDK sample program: wait_card");

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

			System.out.println("Please insert your card");

	        PTEID_ReaderContext 	reader 	= WaitForCardPresent(5);

	        if( reader == null)
	        {
				System.out.println( "Timeout...");
			}
			else
			{
				PTEID_EIDCard card 	= reader.getEIDCard();
				card.setAllowTestCard(true);

				String sName = card.getID().getFirstName();
				sName += " ";
				sName += card.getID().getSurname();

				System.out.println( sName + ", please remove your card.");

				if (!WaitForCardAbsent(5))
				{
					while (!WaitForCardAbsent(1))
					{
						System.out.println( sName + ", don't forget to remove your card!");
						Toolkit.getDefaultToolkit().beep();
					}
				}
				System.out.println( "Thanks!");
			}

		}
		catch (PTEID_ExNoReader e)
		{
			System.out.println("[Catch] PTEID_ExNoReader: no card reader");
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
		catch( Exception e)
		{
			System.out.println("[Catch] Exception:" + e.getMessage());
		}

	}
}
