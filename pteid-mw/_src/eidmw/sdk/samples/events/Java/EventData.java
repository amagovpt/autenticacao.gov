//*****************************************************************************
// Event data class
// This class contains user defined items. The object
// is passed around through the callback mechanism.
//*****************************************************************************
import java.lang.*;
import be.belgium.eid.*;

public class EventData
{
	EventData(String readerName)
	{
		m_readerName = readerName;
	}
	public String m_readerName;
}
