package be.eid.eidtestinfra.pcsccontrol.gui;

/**
 * This project uses JPCSC under Java 5 and smartcardio under Java 6 and higher. This 
 * interface hides that underlying library used.
 * 
 * The CardReaderMonitor tracks insertion and removal of smart card readers smart cards. When
 * such a change occurs, the corresponding method on all ReaderChancedListeners is called.
 *  
 * @author Rogier Taal
 * 
 */
public interface CardReaderMonitor {
	/**
	 * Create a {@link CardSession}.
	 * @param rdr
	 * @return
	 */
	CardSession createCardSession(String rdr);
	
	/**
	 * 
	 * @param pcl
	 */
	void addReaderChangedListener(CardReaderChangeListener pcl);
	
	/**
	 * 
	 * @param pcl
	 */
	void removeReaderChangedListener(CardReaderChangeListener pcl);
	
	/**
	 * Start this monitor.
	 */
	void start();
	
	/**
	 * Stop this monitor. Smart card reader changes are no longer monitored. All
	 * cached smart card reader states are removed. No listeners will be called.
	 */
	void stop();
	
	/**
	 * Pause this monitor. Smart card reader changes are no longer monitored but
	 * may hold on to cached smart card reader states that allow the monitor to be resumed.
	 * No listeners will be called.
	 */
	void pause();
	
	/**
	 * Resume a paused monitor.
	 */
	void resume();
}
