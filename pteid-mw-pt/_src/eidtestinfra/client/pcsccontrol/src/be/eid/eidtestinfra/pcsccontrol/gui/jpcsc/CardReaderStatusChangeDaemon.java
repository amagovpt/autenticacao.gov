package be.eid.eidtestinfra.pcsccontrol.gui.jpcsc;

import java.util.ArrayList;
import java.util.List;

import javax.swing.SwingUtilities;
import javax.swing.event.EventListenerList;

import be.eid.eidtestinfra.pcsccontrol.gui.CardReaderChangeListener;

import com.linuxnet.jpcsc.Context;
import com.linuxnet.jpcsc.PCSC;
import com.linuxnet.jpcsc.State;

/**
 * This class is a child of {@link CardReaderMonitorImpl}. GetStatusChange is repeatedly called with
 * a timeout. In case of an actual change the corresponding method is called on the CardReaderChangeListeners.
 * 
 * @author Rogier Taal
 */
class CardReaderStatusChangeDaemon implements Runnable {
	
	private Context ctx;
	private static final int STATUSCHANGE_INTERVAL = new Integer(3000);
	
	private EventListenerList listenerList; 
	private List<State> rdrStates;
	
	private boolean run, exit;
	
	public CardReaderStatusChangeDaemon(Context ctx, EventListenerList listenerList) {
		this.ctx = ctx;
		this.listenerList = listenerList;
		rdrStates = new ArrayList<State>();
	}
	
	public void start() {
		Thread daemon = new Thread(this);
		daemon.setDaemon(true);
		daemon.start();
	}
	
	public void stop() {
		run = false;
		try {
			//if Cancel ever hangs then place the call inside
			//a thread and use a wait timeout
			ctx.Cancel();
		} catch(Exception ignored) {				
		}
		synchronized (rdrStates) {
			rdrStates.notifyAll();		
		}		
		synchronized (this) {
			if(!exit) {
				try {
					this.wait(10);
				} catch(InterruptedException ignored) {
				}				
			}
		}
	}
	
	public void addReader(final String[] rdrs) {
		synchronized (rdrStates) {
			boolean added = false;
			for(String rdr : rdrs) {
				if(!(contains(rdrStates, rdr))) {
					rdrStates.add(new State(rdr));
					added = true;
				}
			}
			if(added) {
				rdrStates.notifyAll();
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
	
	private static boolean contains(List<State> rdrL, String rdr) {
		for(State s : rdrL) {
			if(s.szReader.equals(rdr))
				return true;
		}
		return false;
	}

	public void run() {
		run = true;
		exit = false;
		while(run) {
			getStatusChange();
		}			
    	synchronized (this) {
    		exit = true;
    		notify();
    	}
	}
	
	private void getStatusChange() {
		synchronized (rdrStates) {
			while(run && rdrStates.size() == 0) {
				try {
					rdrStates.wait(1000);
				} catch(InterruptedException ie) {
					run = false;
				}
			}
			if(!run) {
				return;
			}
		}
		State[] states = null;
		try {
			while(run) {
				synchronized (rdrStates) {
					states = rdrStates.toArray(new State[0]);
				}
				if(!ctx.GetStatusChange(STATUSCHANGE_INTERVAL, states))
					break;
			}
		} catch(Exception e) {
			if(!run) {
				return;
			}
			synchronized (rdrStates) {
				if(rdrStates.size() != 0) {
					rdrStates.clear();
					SwingUtilities.invokeLater(new Runnable() { public void run() {
						fireReaderRemoved(null);
					}});
				}
			}
			return;
		}
		
		if(run && states != null) {
			for(int i=0; i<states.length; i++) {
				if((states[i].dwEventState & PCSC.STATE_CHANGED) == PCSC.STATE_CHANGED) {
					
					boolean cardEject = ((states[i].dwEventState&PCSC.STATE_EMPTY) == PCSC.STATE_EMPTY)
						&& ((states[i].dwCurrentState&PCSC.STATE_EMPTY) != PCSC.STATE_EMPTY);
					
					boolean cardInsert = ((states[i].dwEventState&PCSC.STATE_PRESENT) == PCSC.STATE_PRESENT)
						&& ((states[i].dwCurrentState&PCSC.STATE_PRESENT) != PCSC.STATE_PRESENT);
					
					boolean cardPresent = ((states[i].dwEventState&PCSC.PRESENT) == PCSC.PRESENT)
						&& ((states[i].dwCurrentState&PCSC.PRESENT) != PCSC.PRESENT);
					
					//ignoreReader is used on Windows
					boolean ignoreReader = ((states[i].dwEventState&PCSC.STATE_IGNORE)==PCSC.STATE_IGNORE);
					
					//unknown is used on Linux
					boolean unknownReader = ((states[i].dwEventState&PCSC.STATE_UNKNOWN)==PCSC.STATE_UNKNOWN);
									
					final String rdr = states[i].szReader;					
					states[i].dwCurrentState = states[i].dwEventState;

//					System.out.println(Integer.toHexString((states[i].dwEventState&0xFF))+" "+rdr+" : cardEject="+cardEject+",cardInsert="+cardInsert+",cardPresent="
//							+cardPresent+",ignoreReader="+ignoreReader+",unknown="+unknownReader);
					
					if(cardInsert) {
						SwingUtilities.invokeLater(new Runnable() { public void run() {
							fireCardAdded(rdr);
						}});
					}
					else if(cardEject&&!cardPresent) {
						SwingUtilities.invokeLater(new Runnable() { public void run() {
							fireCardRemoved(rdr);
						}});
					}
					
					if(ignoreReader||unknownReader) {
						synchronized (rdrStates) {
							rdrStates.remove(states[i]);
						}
						SwingUtilities.invokeLater(new Runnable() { public void run() {
							fireReaderRemoved(rdr);
						}});						
					} else {
						SwingUtilities.invokeLater(new Runnable() { public void run() {
							fireReaderAdded(rdr);
						}});
					}		
				}
			}
		}
	}
}
