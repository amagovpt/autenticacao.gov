package be.belgium.eid;

/******************************************************************************//**
  * Class to be used to wrap any kind of long value.
  * This class can be used when calling: BEID_ReaderContext.isCardChanged()
  *********************************************************************************/
public class BEID_ulwrapper
{
	public BEID_ulwrapper(long lLong)
	{
		m_long = lLong;
	}
	public long m_long;
}
