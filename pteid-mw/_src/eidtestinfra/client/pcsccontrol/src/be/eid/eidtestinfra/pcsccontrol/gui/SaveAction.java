package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;

import be.eid.eidtestinfra.pcsccontrol.ControlModel;
import be.eid.eidtestinfra.pcsccontrol.PropertyModel;

/**
 * Upon {@link #actionPerformed(ActionEvent)} save is called on the {@link ControlModel} and on the {@link PropertyModel}
 * but only if they are dirty or the control file, resp. property file does not exist.
 * 
 * @author Rogier Taal
 * 
 */
public class SaveAction extends AbstractAction implements Runnable {
	
	public static final String NAME = "ACTION_SAVE";
	private Main main;
	
	public SaveAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;

	public void actionPerformed(ActionEvent actionevent) {
		final Runnable save = this;
		new Thread(new Runnable() {
			public void run() {
				main.getSession().getActionMap().setEnabled(DefaultActionMap.SAVE_PROFILE, false);
				try {
					save.run();
				} finally {
					main.getSession().getActionMap().setEnabled(DefaultActionMap.SAVE_PROFILE, true);
				}
			}
		}).start();	
	}

	public synchronized void run() {
		MainSession session = main.getSession();
		ControlModel cm = session.getControlModel();
		PropertyModel pm = session.getPropertyModel();

		if(session.itemCacheLoaded()) {
			cm.replaceItems(session.getItems());
		}
		main.saveProperties();

		try {
			if((!Toolkit.fileExists(cm.getFilePath())) || cm.isDirty()) {
				if(cm.getFilePath() == null) {
					SelectControlFileAction selectControlFile = (SelectControlFileAction) session.getActionMap().get(SelectControlFileAction.NAME);
					selectControlFile.run();
				}
				if(cm.getFilePath() == null) {
					return;
				}
				SaveControl saveControl = new SaveControl();
				session.runWithBusyCursor(saveControl);
				if(saveControl.e != null) {
					throw saveControl.e;
				}
			}
			if((!Toolkit.fileExists(pm.getFilePath())) || pm.isDirty()) {
				SaveProperties saveProperties = new SaveProperties();
				session.runWithBusyCursor(saveProperties);
				if(saveProperties.e != null) {
					throw saveProperties.e;
				}
			}
		}
		catch(final Exception e) {
			Toolkit.showErrorDialog(main.getFrame(), e);
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
}
