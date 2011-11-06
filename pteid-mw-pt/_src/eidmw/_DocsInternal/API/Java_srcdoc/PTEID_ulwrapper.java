package be.portugal.eid;

/******************************************************************************//**
  * Class to be used to wrap any kind of long value.
  * This class can be used when calling: PTEID_ReaderContext.isCardChanged()
  *********************************************************************************/
public class PTEID_ulwrapper
{
	public PTEID_ulwrapper(long lLong)
	{
		m_long = lLong;
	}
	public long m_long;
}
