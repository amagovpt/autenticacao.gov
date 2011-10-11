//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to catch the PTEID_XXX exceptions in a Java application
// calling the PTEID Java interface.
//
// compile:
//    javac -classpath <path_to>\pteid[35]libJava.jar get_exception.java
//
// run (windows):
//    set PATH=<path_to_dll's>;%PATH%
//    java -cp <path_to>\pteidlibJava.jar;. main
//*****************************************************************************

import java.lang.*;
import be.portugal.eid.*;

public class get_exception
{
	public static boolean test_PTEID_ExParamRange_1()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate PTEID_ExParamRange exception...");
			//-------------------------------------------
			// force a PTEID_ExParamRange exception
			//-------------------------------------------
			PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReaderByNum(10);
			System.out.println("[Info] Nothing caught...");
		}
		catch (PTEID_ExParamRange e)
		{
			System.out.println("[Catch] PTEID_ExParamRange: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
		return testPassed;
	}
	public static boolean test_PTEID_ExParamRange_2()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate PTEID_ExParamRange exception...");
			//-------------------------------------------
			// force a PTEID_ExParamRange exception
			//-------------------------------------------
			PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReaderByName("Madam I'm Adam");
			System.out.println("[Error] Nothing caught...");
		}
		catch (PTEID_ExParamRange e)
		{
			System.out.println("[Catch] PTEID_ExParamRange: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
		}
		return testPassed;
	}
	static boolean test_PTEID_ExCardBadType()
	{
		boolean testPassed = false;
		try
		{
			System.out.println("[Info]  Trying to generate PTEID_ExCardBadType exception...");
			//-------------------------------------------
			// force a PTEID_ExCardBadType exception
			//-------------------------------------------
			System.out.println("[Info]  Getting PTEID_ReaderContext");
			PTEID_ReaderContext readerContext = PTEID_ReaderSet.instance().getReader();
			String name = readerContext.getName();
			System.out.println("[Info]  Getting PTEID_CardType");
			if (readerContext.isCardPresent())
			{
				System.out.println("[Info]  Card present in reader: " + name);
			}
			else
			{
				System.out.println("[Info]  Card NOT present in reader: " + name);
			}
			PTEID_Card card = readerContext.getEIDCard();
			System.out.println("[Info]  Nothing caught...");
			testPassed = true;
		}
		catch (PTEID_ExCardBadType e)
		{
			System.out.println("[Catch] PTEID_ExCardBadType: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			testPassed = true;
		}
		catch (PTEID_ExNoCardPresent e)
		{
			System.out.println("[Catch] PTEID_ExNoCardPresent: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
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
			System.loadLibrary("pteid35libJava_Wrapper");
		}
		else
		{
			System.loadLibrary("pteidlibJava_Wrapper");
		}

		System.out.println("[Info]  eID SDK sample program: get_exception");

		if (!test_PTEID_ExParamRange_1())
		{
			System.exit(-1);
		}

		if (!test_PTEID_ExParamRange_2())
		{
			System.exit(-1);
		}

		if (!test_PTEID_ExCardBadType())
		{
			System.exit(-1);
		}

		try
		{
			PTEID_ReaderSet.releaseSDK();
		}
		catch (PTEID_Exception e)
		{
			System.out.println("[Catch] PTEID_Exception: code: 0x" + java.lang.Integer.toHexString(e.GetError()));
			System.exit(-1);
		}
		catch (Exception e)
		{
			System.out.println("[Catch] Exception: " + e.getMessage());
			System.exit(-1);
		}
	}
}
