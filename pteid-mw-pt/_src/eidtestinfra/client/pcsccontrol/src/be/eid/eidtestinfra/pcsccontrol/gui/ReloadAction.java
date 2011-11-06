package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlModel;

/**
 * Upon {@link #actionPerformed(ActionEvent)} reload is called on the {@link ControlModel}, then
 * {@link MainSession#loadItemCache()} is called and then {@link Main#reload()} is called.
 * 
 * @author Rogier Taal
 * 
 */
public class ReloadAction extends AbstractAction implements Runnable {
	
	public static final String NAME = "ACTION_RELOAD";
	private Main main;
	
	public ReloadAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;

	public void actionPerformed(ActionEvent actionevent) {
		new Thread(this).start();		
	}

	public synchronized void run() {		
		MainSession session = main.getSession();
		
		try {
			session.getControlModel().reload();
		} catch (Exception e) {
			Toolkit.showErrorDialog(main.getFrame(), e);
		}
		session.runWithBusyCursor(new LoadItemCache());
		
		List<File> dirL = new ArrayList<File>();
		
		//Add the already present directories to the list
		String[] currDirs = session.getPropertyModel().getIncludeDirs();
		for(String fileStr : currDirs) {
			dirL.add(new File(fileStr));
		}
		
		//Add the missing directories to the list
		List<ControlCardHolder> cchL = session.getItems();
		for(ControlCardHolder cch : cchL) {
			if(cch.getFile() != null) {
				File f = new File(cch.getFile());
				File dir = f.getParentFile();
				if(!dirL.contains(dir)) {
					dirL.add(dir);
				}
			}
		}
				
		//If the list size is bigger than the number of current directories
		//then ask user if directories should be automatically added
		if(dirL.size() != currDirs.length) {
			final CountDownLatch l1 = new CountDownLatch(1);
			final boolean[] optionYES = new boolean[1];
			SwingUtilities.invokeLater(new Runnable() {
				public void run() {
					optionYES[0] = (JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(main.getFrame(), 
							"The include directory for one or more active virtual cards is missing. Automatically add missing directories?", "Add missing directories", JOptionPane.YES_NO_OPTION));
					l1.countDown();
				}
			});
			try {
				l1.await();
			} catch(InterruptedException ignored) {
			}
			
			if(optionYES[0]) {
				String[] dirs = new String[dirL.size()];
				for(int i=0; i<dirL.size(); i++) {
					dirs[i] = dirL.get(i).getAbsolutePath();
				}
				session.getPropertyModel().replaceIncludeDirs(dirs);
				session.runWithBusyCursor(new LoadItemCache());
			}
		}
		
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				main.reload();
			}
		});
	}
	
	private class LoadItemCache implements Runnable {
		Exception e;
		public void run() {
			try {
				main.getSession().loadItemCache();
			} catch(Exception e) {
				this.e = e;
			}
		}		
	}
}
