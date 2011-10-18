//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to catch the BEID_XXX exceptions in a Java application
// calling the BEID Java interface.
//
// compile:
//    javac -classpath <path_to>\beid[35]libJava.jar get_exception.java
//
// run (windows):
//    set PATH=<path_to_dll's>;%PATH%
//    java -cp <path_to>\beidlibJava.jar;. main
//*****************************************************************************

import java.lang.*;
import be.belgium.eid.*;

public class get_exception
{
	public static boolean test_BEID_ExParamRange_1()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate BEID_ExParamRange exception...");
			//-------------------------------------------
			// force a BEID_ExParamRange exception
			//-------------------------------------------
			BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByNum(10);
			System.out.println("[Info] Nothing caught...");
		}
		catch (BEID_ExParamRange e)
		{
			System.out.println("[Catch] BEID_ExParamRange: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
		return testPassed;
	}
	public static boolean test_BEID_ExParamRange_2()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate BEID_ExParamRange exception...");
			//-------------------------------------------
			// force a BEID_ExParamRange exception
			//-------------------------------------------
			BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByName("Madam I'm Adam");
			System.out.println("[Error] Nothing caught...");
		}
		catch (BEID_ExParamRange e)
		{
			System.out.println("[Catch] BEID_ExParamRange: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
		return testPassed;
	}
	static boolean test_BEID_ExCardBadType()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate BEID_ExCardBadType exception...");
			//-------------------------------------------
			// force a BEID_ExCardBadType exception
			//-------------------------------------------
			System.out.println("[Info]  Getting BEID_ReaderContext");
			BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReader();
			String name = readerContext.getName();
			System.out.println("[Info]  Getting BEID_CardType");
			if (readerContext.isCardPresent())
			{
				System.out.println("[Info]  Card present in reader: " + name);
			}
			else
			{
				System.out.println("[Info]  Card NOT present in reader: " + name);
			}
			BEID_Card card = readerContext.getEIDCard();
			System.out.println("[Info]  Nothing caught...");
			testPassed = true;
		}
		catch (BEID_ExCardBadType e)
		{
			System.out.println("[Catch] BEID_ExCardBadType: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (BEID_ExNoCardPresent e)
		{
			System.out.println("[Catch] BEID_ExNoCardPresent: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
		return testPassed;
	}
	//*****************************************************************************
	// Main entry point
	// Different methods will force exceptions. When these exceptions are generated,
	// the test is considered as sucessful.
	// If the test is not successful, the program will exit and as such, no release
	// of the SDK is done, generating also an exception.
	//*****************************************************************************
	public static void main(String args[])
	{
		int	 Retval = 0;
		String 	osName  = System.getProperty("os.name");

		if (-1 != osName.indexOf("Windows"))
		{
			System.out.println("[Info]  Windows system!!");
			System.loadLibrary("beid35libJava_Wrapper");
		}
		else
		{
			System.loadLibrary("beidlibJava_Wrapper");
		}

		System.out.println("[Info]  eID SDK sample program: get_exception");

		if (!test_BEID_ExParamRange_1())
		{
			System.exit(-1);
		}

		if (!test_BEID_ExParamRange_2())
		{
			System.exit(-1);
		}

		if (!test_BEID_ExCardBadType())
		{
			System.exit(-1);
		}

		try
		{
			BEID_ReaderSet.releaseSDK();
		}
		catch (BEID_Exception e)
		{
			System.out.println("[Catch] BEID_Exception: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			System.exit(-1);
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
			System.exit(-1);
		}
	}
}
