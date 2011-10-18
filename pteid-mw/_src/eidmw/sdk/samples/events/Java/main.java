//*****************************************************************************
// eID SDK sample code.
// This sample demonstrates how to use the eID SDK to add events on a card reader
//*****************************************************************************
import java.lang.*;
import java.util.*;
import java.io.*;
import be.belgium.eid.*;

public class main
{
	static HashMap MyReadersSet = new HashMap();

	public static void main(String[] args)
	{
		String osName = System.getProperty("os.name");

		if (-1 != osName.indexOf("Windows"))
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
			System.out.println("eID SDK sample program: events_eid");
			System.out.println("   Insert/Remove card to call event callback");
			System.out.println("   Press 'q<CR>' to quit");

			System.out.println("[Info] Setting event callbacks:");

			//--------------------------------------------
			// install the callbacks for all card readers present
			//--------------------------------------------
			for (int readerIdx = 0; readerIdx < BEID_ReaderSet.instance().readerCount(); readerIdx++)
			{
				BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByNum(readerIdx);
				String readerName = readerContext.getName();
				EventCallback eventCallBack = new EventCallback();
				EventData eventData = new EventData(readerName);

				long handle = readerContext.SetEventCallback(eventCallBack, eventData);
				ReaderRef nh = new ReaderRef(readerName, handle);
				MyReadersSet.put(readerName, nh);
			}

			//--------------------------------------------
			// wait until 'q' is pressed to stop the application
			//--------------------------------------------
			String CurLine = "";
			InputStreamReader converter = new InputStreamReader(System.in);
			BufferedReader in = new BufferedReader(converter);
			while (!(CurLine.equals("q")))
			{
				CurLine = in.readLine();
			}
		}
		catch (BEID_ExParamRange e)
		{
			System.out.println("BEID_ExParamRange caught");
		}
		catch (UnsupportedOperationException e)
		{
			System.out.println("UnsupportedOperationException caught");
		}
		catch (ClassCastException e)
		{
			System.out.println("ClassCastException caught");
		}
		catch (IllegalArgumentException e)
		{
			System.out.println("IllegalArgumentException caught");
		}
		catch (NullPointerException e)
		{
			System.out.println("NullPointerException caught");
		}
		catch (Exception e)
		{
			System.out.println("Exception caught" + e.getMessage());
		}

		try
		{
			//--------------------------------------------
			// stop the callbacks
			//--------------------------------------------
			Set readers = MyReadersSet.keySet();
			Iterator itr = readers.iterator();
			while (itr.hasNext())
			{
				String readerName = (String)itr.next();
				System.out.println("[Info] Reader: " + readerName);
				ReaderRef nh = (ReaderRef)MyReadersSet.get(readerName);
				BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByName(readerName);
				System.out.println("[Info] StopEventCallback(" + nh.m_handle + ")");
				readerContext.StopEventCallback(nh.m_handle);
			}

			System.out.println("[Info] Releasing SDK");
			BEID_ReaderSet.releaseSDK();
			System.out.println("[Info] Done...");
		}
		catch (BEID_Exception e)
		{
			System.exit(-1);
		}
		catch (Exception e)
		{
			System.exit(-1);
		}
	}
}
