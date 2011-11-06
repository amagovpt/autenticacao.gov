package be.eid.eidtestinfra.pcsccontrol.gui;

import java.util.ArrayList;
import java.util.List;

import javax.smartcardio.CardException;
import javax.smartcardio.CardTerminal;
import javax.smartcardio.CardTerminals;
import javax.smartcardio.TerminalFactory;
import javax.swing.SwingUtilities;
import javax.swing.event.EventListenerList;

import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * Smartcardio version of {@link CardReaderMonitor}.
 * 
 * @author Rogier Taal
 * 
 */
public class CardReaderMonitorImpl implements CardReaderMonitor, Runnable {
	
	private static final Long LISTREADER_INTERVAL = new Long(4000);
	private EventListenerList listenerList;
	private TerminalFactory factory;
	private List<CardTerminal> terms;
	private List<Boolean> termsCardPresent;
	private CardTerminals terminals;
	private boolean run, exit;
	
	public CardReaderMonitorImpl() {
		listenerList = new EventListenerList();
	}
	
	public void start() {		
		factory = TerminalFactory.getDefault();
		terms = new ArrayList<CardTerminal>();
		termsCardPresent = new ArrayList<Boolean>();
		new Thread(this).start();
	}
	
	public void resume() {
		new Thread(this).start();
	}
	
	public void stop() {
		pause();
		factory = null;
		terms = null;
		termsCardPresent = null;
	}
	
	public void pause() {
		run = false;
		synchronized (LISTREADER_INTERVAL) {
			LISTREADER_INTERVAL.notify();
		}
		synchronized (this) {
			while(!exit) {
				try {
					wait(10);
				} catch(InterruptedException ignored) {
				}
			}
		}		
	}
	
	public CardSession createCardSession(String rdr) {
		CardTerminal term = null;
		synchronized (terms) {
			for(CardTerminal t : terms) {
				if(t.getName().equals(rdr))
					term = t;
			}			
		}
		return new CardSessionImpl(term, rdr);
	}

	public void addReaderChangedListener(CardReaderChangeListener pcl) {
		listenerList.add(CardReaderChangeListener.class, pcl);
	}
	
	public void removeReaderChangedListener(CardReaderChangeListener pcl) {
		listenerList.remove(CardReaderChangeListener.class, pcl);
	}
	
	protected void fireReaderAdded(String reader) {
	    Object[] listeners = listenerList.getListenerList();
	    for (int i = listeners.length-2; i>=0; i-=2) {
	        if (listeners[i]==CardReaderChangeListener.class) {
        		((CardReaderChangeListener)listeners[i+1]).readerAdded(reader);
	        }
	    }
	}
	
	protected void fireReaderRemoved(String reader) {
	    Object[] listeners = listenerList.getListenerList();
	    for (int i = listeners.length-2; i>=0; i-=2) {
	        if (listeners[i]==CardReaderChangeListener.class) {
        		((CardReaderChangeListener)listeners[i+1]).readerRemoved(reader);
	        }
	    }
	}
	
	protected void fireCardAdded(String reader) {
	    Object[] listeners = listenerList.getListenerList();
	    for (int i = listeners.length-2; i>=0; i-=2) {
	        if (listeners[i]==CardReaderChangeListener.class) {
        		((CardReaderChangeListener)listeners[i+1]).cardAdded(reader);
	        }
	    }
	}
	
	protected void fireCardRemoved(String reader) {
	    Object[] listeners = listenerList.getListenerList();
	    for (int i = listeners.length-2; i>=0; i-=2) {
	        if (listeners[i]==CardReaderChangeListener.class) {
        		((CardReaderChangeListener)listeners[i+1]).cardRemoved(reader);
	        }
	    }
	}
	
	public void run() {
		if (Log.logger != null)
			Log.logger.debug("Started CardReaderMonitorImpl thread");
		run = true;
		exit = false;
		boolean changed;
		boolean noReaders;
		terminals = factory.terminals();
		
		while (run) {
			synchronized (terms) {
				getStatusChange();				
			}
			terminals = factory.terminals();
			changed = false;
			while(run && !changed) {
				noReaders = false;
				try {
					changed = terminals.waitForChange(LISTREADER_INTERVAL);
				} catch(Exception e) {
					noReaders = true;
				}
				if(run && noReaders) {
					synchronized (LISTREADER_INTERVAL) {
						try {
							LISTREADER_INTERVAL.wait(LISTREADER_INTERVAL);
						} catch(InterruptedException ie) {
							run = false;
						}
					}
				}
			}
		}
    	synchronized (this) {
    		exit = true;
    		this.notify();
    	}
		if (Log.logger != null)
			Log.logger.debug("Ending CardReaderMonitorImpl thread");
	}
	
	private void getStatusChange() {
		List<CardTerminal> old = new ArrayList<CardTerminal>(terms.size());
		List<Boolean> oldPresent = new ArrayList<Boolean>(terms.size());
		old.addAll(terms);
		oldPresent.addAll(termsCardPresent);
		
		try {
			terms.clear();
			termsCardPresent.clear();
			terms.addAll(terminals.list());
		} catch(CardException ce) {
			if(!run) {
				return;
			}
			SwingUtilities.invokeLater(new Runnable() { public void run() {
				fireReaderRemoved(null);
			}});
			return;
		}
		
		for(int i=0; i<terms.size(); i++) {
			try {
				termsCardPresent.add(terms.get(i).isCardPresent());
			} catch(CardException ce) {
				termsCardPresent.add(false);
			}
		}
		
		//Check for reader removed
		for(int i=0; i<old.size(); i++) {
			if(!terms.contains(old.get(i))) {
				final CardTerminal term = old.get(i);
				SwingUtilities.invokeLater(new Runnable() { public void run() {
					fireReaderRemoved(term.getName());
				}});
			}
		}
		
		//Check for reader added, card added, card removed
		for(int index=0; index<terms.size(); index++) {
			final CardTerminal newTerm = terms.get(index);
			int oldIndex = old.indexOf(newTerm);
			if(oldIndex == -1) {
				final boolean cardPresent = termsCardPresent.get(index);
				SwingUtilities.invokeLater(new Runnable() { public void run() {						
					if(cardPresent)
						fireCardAdded(newTerm.getName());
					else
						fireReaderAdded(newTerm.getName());		
				}});
			} else {
				boolean presentBefore = oldPresent.get(oldIndex);
				boolean presentAfter = termsCardPresent.get(index);			
				if(presentBefore && !presentAfter) {
					SwingUtilities.invokeLater(new Runnable() { public void run() {
						fireCardRemoved(newTerm.getName());
					}});
				} else if(!presentBefore && presentAfter) {
					SwingUtilities.invokeLater(new Runnable() { public void run() {
						fireCardAdded(newTerm.getName());
					}});						
				}
			}
		}		
	}

}
