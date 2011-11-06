package be.eid.eidtestinfra.pcsccontrol.gui;

import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * A Runnable that contains Runnables. {@link #run()} simply calls the Runnables given in the
 * constructor.
 * 
 * @author Rogier Taal
 * 
 */
public class MultiRunnable implements Runnable {
	private Runnable [] run;
	
	public MultiRunnable(Runnable [] run) {
		this.run = run;
	}

	public void run() {
		if (Log.logger != null)
			Log.logger.debug("Started MultRunnable thread, " + run.length + " threads to be started");
		for(Runnable r : run) {
			r.run();
		if (Log.logger != null)
			Log.logger.debug("Ended MultRunnable thread");
		}
	}

}
