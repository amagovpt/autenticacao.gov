package be.eid.eidtestinfra.pcsccontrol.gui;

import java.util.EventListener;

/**
 * EventListener for Smart Card Reader status changes.
 * 
 * @author Rogier Taal
 * 
 */
public interface CardReaderChangeListener extends EventListener {
	void cardAdded(String readerName);
	void cardRemoved(String readerName);
	void readerAdded(String readerName);
	void readerRemoved(String readerName);
}
