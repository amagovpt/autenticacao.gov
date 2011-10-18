//*****************************************************************************
// Event callback implementation
// The eidlib will call the method getEvent()
//*****************************************************************************
import java.lang.*;
import java.util.*;
import be.belgium.eid.*;

public class EventCallback implements Callback
{
	//--------------------------------------------
	// keep a vector with all incoming callbacks
	//--------------------------------------------
	//static public Vector callbacks = new Vector();

	public EventCallback()
	{
	}

	//--------------------------------------------
	// implementation of the callback method
	// the data object of this method is the object set at initialization
	// of the callbacks (see setEventCallback(...) in main)
	//--------------------------------------------
	public void getEvent(long lRet, long ulState, Object data)
	{
		try
		{
			//--------------------------------------------
			// get the reader name that sent the event
			//--------------------------------------------
			EventData eventData = (EventData)data;
			String readerName = eventData.m_readerName;

			BEID_ReaderContext readerContext = BEID_ReaderSet.instance().getReaderByName(readerName);

			//--------------------------------------------
			// is a card present?
			//--------------------------------------------
			if (!readerContext.isCardPresent())
			{
				System.out.println("[CBack] Card removed: " + readerName);
				return;
			}

			//--------------------------------------------
			// find in the callbackHandles the corresponding card ID for this reader
			//--------------------------------------------
			ReaderRef nh = (ReaderRef)main.MyReadersSet.get(readerName);
			BEID_ulwrapper wrapCardID = new BEID_ulwrapper(nh.m_cardID);

			//--------------------------------------------
			// check if card is changed and store new ID
			//--------------------------------------------
			if (readerContext.isCardChanged(wrapCardID))
			{
				System.out.println("[CBack] Card changed: " + readerName + " card_id:" + wrapCardID.m_long);
				nh.m_cardID = wrapCardID.m_long;
			}
		}
		catch (Exception ex)
		{
			System.out.println("[Error] Exception caught");
		}
	}
}
