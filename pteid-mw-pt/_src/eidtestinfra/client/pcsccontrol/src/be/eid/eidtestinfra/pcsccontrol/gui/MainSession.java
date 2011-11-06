package be.eid.eidtestinfra.pcsccontrol.gui;

import java.util.List;
import java.util.Vector;

import javax.swing.SwingUtilities;

import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlModel;
import be.eid.eidtestinfra.pcsccontrol.PropertyModel;

/**
 * This class contains methods that target the {@link ControlModel} and the {@link PropertyModel}. Calling
 * these methods on this class will take care of synchronization, wait cursor and error dialogs.
 * 
 * @author Rogier Taal
 * 
 */
public class MainSession {
	
	private Main main;
	private DefaultActionMap actionMap;
	private Integer requestCount = 0;
	private PropertyModel pm;
	private ControlModel cm;
	private List<ControlCardHolder> items = new Vector<ControlCardHolder>();
	private CardReaderMonitor crmon;
	private Boolean itemCacheLoaded = false;
	
	public MainSession(Main main, PropertyModel pm, ControlModel cm) {
		this.main = main;
		actionMap = new DefaultActionMap();
		this.pm = pm;
		this.cm = cm;
		crmon = CardReaderMonitorFactory.createReaderMonitor();
	}
	
	public CardReaderMonitor getCardReaderMonitor() {
		return crmon;
	}
		
	public ControlModel getControlModel() {
		return cm;
	}
	
	public PropertyModel getPropertyModel() {
		return pm;
	}
	
	public DefaultActionMap getActionMap() {
		return actionMap;
	}
	
	public boolean itemCacheLoaded() {
		synchronized (this.itemCacheLoaded) {
			return this.itemCacheLoaded;
		}
	}
	
	public synchronized void loadItemCache() {
		List<ControlCardHolder> itemsNew = cm.getItems(pm.getIncludeDirs());
		this.items.clear();
		this.items.addAll(itemsNew);
		synchronized (itemCacheLoaded) {
			itemCacheLoaded = true;
		}
	}
	
	public List<ControlCardHolder> getItems() {
		return items;			
	}
	
	public void runWithBusyCursor(Runnable r) {
		requestStarted();
		try {
			r.run();
		} finally {
			requestFinished();
		}
	}
	
	private void requestStarted() {
		synchronized (requestCount) {
			if(requestCount++ == 0)
				SwingUtilities.invokeLater(new Runnable() {
					public void run() {
						Toolkit.startWaitCursor(main.getFrame());
					}
				});
		}
	}
	
	private void requestFinished() {
		synchronized (requestCount) {
			if(--requestCount == 0)
				SwingUtilities.invokeLater(new Runnable() {
					public void run() {
						Toolkit.stopWaitCursor(main.getFrame());
					}
				});
		}
	}
	
}
