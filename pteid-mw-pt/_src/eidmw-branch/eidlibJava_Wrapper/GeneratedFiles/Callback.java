package pt.gov.cartaodecidadao;

/**
  * Java callback interface
  * This interface must be implemented at the application side by a class that should receive
  * the callback(s).
  */
public interface Callback
{
	/**
	  * Get a callback event.
	  * This method is the callback from the Java SDK to the application.
	  * A class at the application side must implement this method to receive the callbacks from
	  * the Java interface. This callback method will give the dataobject back that was set by the
	  * PTEID_ReaderContext.SetEventCallback(...) method.
	  *
	  * @param lRet:    		internal use only
	  * @param ulState: 		internal use only
	  * @param callbackData: 	the callback data object as passed by the function SetEventCallback
	  */
	public void getEvent(long lRet, long ulState, Object callbackData);
}

class CallbackHelper
{
	CallbackHelper()
	{
	}
	CallbackHelper(CallbackHelper callbackHelper)
	{
		m_callbackObject = callbackHelper.m_callbackObject;
		m_callbackData = callbackHelper.m_callbackData;
	}
	CallbackHelper(Callback callback, Object callbackData)
	{
		m_callbackObject = callback;
		m_callbackData = callbackData;
		m_handle = -1;
	}
	public Callback m_callbackObject;
	public Object m_callbackData;
	public long m_handle;
}