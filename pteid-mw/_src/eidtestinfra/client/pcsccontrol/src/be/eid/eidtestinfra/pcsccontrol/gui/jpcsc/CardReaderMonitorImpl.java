package be.eid.eidtestinfra.pcsccontrol.gui.jpcsc;

import javax.swing.event.EventListenerList;

import be.eid.eidtestinfra.pcsccontrol.gui.CardReaderChangeListener;
import be.eid.eidtestinfra.pcsccontrol.gui.CardReaderMonitor;
import be.eid.eidtestinfra.pcsccontrol.gui.CardSession;
import be.eid.eidtestinfra.pcsccontrol.Log;

import com.linuxnet.jpcsc.Context;
import com.linuxnet.jpcsc.PCSC;

/**
 * JPCSC implementation of the CardReaderMonitor. Start method will start a thread
 * that at regular interval calls {@link Context#ListReaders()}. The readers are
 * passed to the CardReaderStatusChangeDaemon which at regular interval calls
 * GetStatusChange. The result is that all CardReaderChangeListeners are informed when a reader or
 * card is added or removed. 
 *  
 * @author Rogier Taal
 */
public class CardReaderMonitorImpl implements CardReaderMonitor, Runnable {

	private Context ctx;
	private EventListenerList listenerList; 
	private CardReaderStatusChangeDaemon statusChangeDaemon;
	private static final Long LISTREADER_INTERVAL = new Long(4000);
	private boolean ctxEstablished, run, exit;	

	public CardReaderMonitorImpl() {
		listenerList = new EventListenerList();
	}
	
	public static boolean jpcscEnabled() {
		try {
			Class.forName("com.linuxnet.jpcsc.Context");
			return true;
		} catch(ClassNotFoundException cnfe) {
			return false;
		}
	}
	
	public void start() {
		ctx = new Context();
		statusChangeDaemon = new CardReaderStatusChangeDaemon(ctx, listenerList);
		new Thread(this).start();
	}
	
	public void resume() {
		new Thread(this).start();
	}
	
	public void pause() {
		run = false;
		synchronized (LISTREADER_INTERVAL) {
			LISTREADER_INTERVAL.notify();
		}
		synchronized (this) {
			if(!exit) {
				try {
					wait(10);
				} catch(InterruptedException ignored) {
				}
			}
		}
		statusChangeDaemon.stop();
	}
	
	public void stop() {
		pause();
		try {
			//if ReleaseContext ever hangs then place the call inside
			//a thread and use a wait timeout
			ctx.ReleaseContext(); 
		} catch(Exception ignored) {				
		}
		ctxEstablished = false;
		ctx = null;
		statusChangeDaemon = null;		
	}
	
	public CardSession createCardSession(String rdr) {
		return new CardSessionImpl(ctx, rdr);
	}
	
	public void addReaderChangedListener(CardReaderChangeListener pcl) {
		listenerList.add(CardReaderChangeListener.class, pcl);
	}
	
	public void removeReaderChangedListener(CardReaderChangeListener pcl) {
		listenerList.remove(CardReaderChangeListener.class, pcl);
	}
	
	public void run() {
		if (Log.logger != null)
			Log.logger.debug("Started CardReaderMonitorImpl thread");
		statusChangeDaemon.start();
		run = true;
    	while(run) {
    		if(!ctxEstablished) {
    			try {
    				ctx.EstablishContext(PCSC.SCOPE_SYSTEM, null, null);
    				ctxEstablished = true;
    			} catch(Exception ignored) {    				
    			}
    		}
    		if(ctxEstablished) {
			    String[] sa = null; 
			    try {
			    	sa = ctx.ListReaders();
			    } catch (Exception ignored) {
			    }
			    if(!run) {
			    	break;
			    }
			    if(sa != null) {
		    		statusChangeDaemon.addReader(sa);
			    }
    		}
		    synchronized (LISTREADER_INTERVAL) {
		    	try {
		    		LISTREADER_INTERVAL.wait(LISTREADER_INTERVAL);
		    	} catch(InterruptedException ie) {
		    		run = false;
		    	}				
			}
    	}
    	
    	synchronized (this) {
    		exit = true;
    		notify();
    	}
		if (Log.logger != null)
			Log.logger.debug("Ending CardReaderMonitorImpl thread");
	}
}
