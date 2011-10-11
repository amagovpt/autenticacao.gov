package be.eid.eidtestinfra.pcsccontrol.gui;

/**
 * This project uses JPCSC under Java 5 and smartcardio under Java 6 and higher. This 
 * interface hides that underlying library used.
 * 
 * @author Rogier Taal
 * 
 */
public interface CardSession {
	/**
	 * Connect to reader.
	 */
	void connect() throws Exception;
	
	/**
	 * Transmit data.
	 * @param apdu
	 * @return
	 * @throws Exception
	 */
	byte[] transmit(byte[] apdu) throws Exception;
	
	/**
	 * Disconnect from card.
	 * @throws Exception
	 */
	void disconnect() throws Exception;
}
