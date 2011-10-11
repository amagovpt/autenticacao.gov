package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;
import java.io.File;
import java.util.concurrent.CountDownLatch;

import javax.swing.AbstractAction;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import be.eid.eidtestinfra.pcsccontrol.ControlModel;

/**
 * Upon {@link #actionPerformed(ActionEvent)} the user is allowed to select a control file using a file
 * chooser.
 * 
 * @author Rogier Taal
 * 
 */
public class SelectControlFileAction extends AbstractAction implements Runnable {
	
	public static final String NAME = "ACTION_SELECTCONTROLFILE";
	private Main main;
	
	public SelectControlFileAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;

	public void actionPerformed(ActionEvent actionevent) {
		final Runnable selectFile = this;
		Runnable r = new Runnable() {
			public void run() {
				//controlFile before select file
				String ctrlFile1 = main.getSession().getControlModel().getFilePath();
				
				//allow the user to select a file
				selectFile.run();
				
				//controlFile after select file
				String ctrlFile2 = main.getSession().getControlModel().getFilePath();
				
				//if before == after then return
				if(ctrlFile1 == null && ctrlFile2 == null) {
					return;
				}
				//if before equals after then return
				if(!(ctrlFile1 == null && ctrlFile2 == null)) {
					boolean fileEquals = (Toolkit.isWindows ? ctrlFile1.equalsIgnoreCase(ctrlFile2)
							: ctrlFile1.equals(ctrlFile2));
					if(fileEquals) {
						return;
					}
				}
				//file selected has changed, now ask for reload
				final CountDownLatch l1 = new CountDownLatch(1);
				final boolean[] optionYES = new boolean[1];
				SwingUtilities.invokeLater(new Runnable() {
					public void run() {
						optionYES[0] = (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(main.getFrame(), 
								"Reload settings from control file?", "Reload", JOptionPane.YES_NO_OPTION));
						l1.countDown();
					}
				});
				try {
					l1.await();
				} catch(InterruptedException ignored) {
				}
				if(optionYES[0]) {
					Runnable reload = (Runnable)main.getSession().getActionMap().get(ReloadAction.NAME);
					reload.run();
				}
			}
		};
		new Thread(r).start();
	}

	public synchronized void run() {
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
	    	final File selectedFile = chooser.getSelectedFile();
	    	if(prevControlFile == null || !prevControlFile.equals(selectedFile)) {
				try {
					cm.setFilePath(selectedFile.getAbsolutePath());
		    		session.getPropertyModel().setControlFile(cm.getFilePath());
				} catch(final Exception e) {
					Toolkit.showErrorDialog(main.getFrame(), e);
				}
			}
		}		
	}
}
