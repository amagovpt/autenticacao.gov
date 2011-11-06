//*****************************************************************************
// reader reference 
//*****************************************************************************
import java.lang.*;
import java.util.*;
import java.io.*;
import be.portugal.eid.*;

public class ReaderRef
{
	ReaderRef(String name, long handle)
	{
		m_name = name;		// reader name
		m_handle = handle;	// reader handle
		m_cardID = 0;		// card ID
	}
	String m_name;
	long m_handle;
	long m_cardID;
};
