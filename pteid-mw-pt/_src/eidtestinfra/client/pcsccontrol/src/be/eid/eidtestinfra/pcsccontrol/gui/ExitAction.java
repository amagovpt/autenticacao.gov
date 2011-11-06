package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.util.concurrent.CountDownLatch;

import javax.swing.AbstractAction;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import be.eid.eidtestinfra.pcsccontrol.ControlModel;
import be.eid.eidtestinfra.pcsccontrol.PropertyModel;

/**
 * Upon {@link #actionPerformed(ActionEvent)} when the {@link ControlModel} is dirty, the user
 * is asked if he wishes to save the control model. If the control model is to be saved but the control file path
 * is not set, the user is requested to select a path using a file chooser. The {@link PropertyModel}
 * is always saved when dirty without needing to ask the user.
 * 
 * @author Rogier Taal
 * 
 */
public class ExitAction extends AbstractAction implements Runnable, WindowListener {
	
	public static final String NAME = "ACTION_EXIT";
	private Main main;
	private boolean doExit;
	
	public ExitAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;

	public void actionPerformed(ActionEvent actionevent) {
		final Runnable exit = this;
		new Thread(new Runnable() {
			public void run() {
				main.getSession().getActionMap().setEnabled(DefaultActionMap.SAVE_PROFILE, false);
				try {
					exit.run();
				} finally {
					main.getSession().getActionMap().setEnabled(DefaultActionMap.SAVE_PROFILE, true);
				}
			}
		}).start();
	}
	
	public void windowClosing(WindowEvent arg0) {
		if(enabled)
			actionPerformed(null);
	}

	public void windowActivated(WindowEvent arg0) {}

	public void windowClosed(WindowEvent arg0) { }

	public void windowDeactivated(WindowEvent arg0) { }

	public void windowDeiconified(WindowEvent arg0) {}

	public void windowIconified(WindowEvent arg0) {}

	public void windowOpened(WindowEvent arg0) {}

	public synchronized void run() {
		MainSession session = main.getSession();
		ControlModel cm = session.getControlModel();
		PropertyModel pm = session.getPropertyModel();
		doExit = true;
		
		if(session.itemCacheLoaded()) {
			cm.replaceItems(session.getItems());
		}
		main.saveProperties();

		if((!Toolkit.fileExists(cm.getFilePath())) || cm.isDirty()) {
			final CountDownLatch l1 = new CountDownLatch(1);
			final boolean[] optionYES = new boolean[1];
			SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					optionYES[0] = (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(main.getFrame(), 
							"Control file has been modified. Save changes?", "Save Control File", JOptionPane.YES_NO_OPTION));
					l1.countDown();
				}
			});
			try {
				l1.await();
			} catch(InterruptedException ignored) {
			}
			
			if(optionYES[0]) {
				if(cm.getFilePath() == null) {
					if(!waitForSelectControlFile()) {
						doExit = false;
					}
				}
				if(doExit) {
					SaveControl saveControl = new SaveControl();
					session.runWithBusyCursor(saveControl);
					if(saveControl.e != null) {
						doExit = false;
						Toolkit.showErrorDialog(main.getFrame(), saveControl.e);						
					}
				}
			}
		}
				
		if(doExit) {
			if((!Toolkit.fileExists(pm.getFilePath())) || pm.isDirty()) {
				SaveProperties saveProperties = new SaveProperties();
				session.runWithBusyCursor(saveProperties);
				if(saveProperties.e != null) {
					final String msg = (saveProperties.e.getCause() != null
							? saveProperties.e.getCause().getMessage() : saveProperties.e.getMessage());
					final CountDownLatch l1 = new CountDownLatch(1);
					final boolean[] optionYES = new boolean[1];
					SwingUtilities.invokeLater(new Runnable() {
						public void run() {
							optionYES[0] = (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(main.getFrame(), 
									msg+"\nFailed to save program settings. Quit anyway?", "Warning",
									JOptionPane.YES_NO_OPTION,  JOptionPane.WARNING_MESSAGE));
							l1.countDown();
						}
					});
					try {
						l1.await();
					} catch(InterruptedException ignored) {
					}					
					doExit = optionYES[0];
				}
			}
		}
		
		if(doExit) {
			if(session.getCardReaderMonitor() != null) {
				session.runWithBusyCursor(new StopCardReaderMonitor());
			}
			System.exit(0);
		}
	}
	
	private class SaveControl implements Runnable {
		Exception e;
		public void run () {
			try {
				main.getSession().getControlModel().save();
			} catch(Exception e) {
				this.e = e;
			}			
		}
	}
	
	private class SaveProperties implements Runnable {
		Exception e;
		public void run () {
			try {
				main.getSession().getPropertyModel().save();
			} catch(Exception e) {
				this.e = e;
			}			
		}
	}
	
	private class StopCardReaderMonitor implements Runnable {
		Exception e;
		public void run() {
			try {
				main.getSession().getCardReaderMonitor().stop();
			} catch (Exception e) {
				this.e = e;
			}
		}		
	}
	
	private boolean waitForSelectControlFile() {
		final CountDownLatch l1 = new CountDownLatch(1);
		final boolean[] optionAPPROVE = new boolean[1];
		
		MainSession session = main.getSession();
		ControlModel cm = session.getControlModel();
		File prevControlFile = (cm.getFilePath() != null ? new File(cm.getFilePath()) : null);
		final JFileChooser chooser = new JFileChooser();
		chooser.setDialogTitle("Select Control File");
		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		chooser.setSelectedFile(prevControlFile);
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				optionAPPROVE[0] = (JFileChooser.APPROVE_OPTION == chooser.showOpenDialog(main.getFrame()));
				l1.countDown();
			}
		});
		try {
			l1.await();
		} catch(InterruptedException ignored) {
		}

	    if(optionAPPROVE[0]) {
			cm.setFilePath(chooser.getSelectedFile().getAbsolutePath());
		    session.getPropertyModel().setControlFile(cm.getFilePath());
		}
		
		boolean fileSelected = (cm.getFilePath() != null);
		return fileSelected;
	}
}
