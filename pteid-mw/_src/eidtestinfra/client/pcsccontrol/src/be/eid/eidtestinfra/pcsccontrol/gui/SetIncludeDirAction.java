package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;
import javax.swing.JDialog;

/**
 * Upon {@link #actionPerformed(ActionEvent)} a dialog is shown containing {@link DirectoryListPane} by which
 * the user is allowed to select or remove include directories.
 * 
 * @author Rogier Taal
 * 
 */
public class SetIncludeDirAction extends AbstractAction {
	
	public static final String NAME = "ACTION_SHOWCONFIGURATION";
	private Main main;
	
	public SetIncludeDirAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;
	
	//@Override
	public void actionPerformed(ActionEvent actionevent) {
		MainSession session = main.getSession();
		DirectoryListPane dirListPane = new DirectoryListPane();
		JDialog dialog = dirListPane.createDialog(main.getFrame(), "Virtual Card Directories");
		dirListPane.setDirectories(session.getPropertyModel().getIncludeDirs());
		Toolkit.centerComponent(dialog, null);
		dialog.setVisible(true);
		
		if(dirListPane.getSelectedValue() == DirectoryListPane.OK) {
			boolean virtDirsChanged = session.getPropertyModel().replaceIncludeDirs(dirListPane.getDirectories());
			if(virtDirsChanged) {
				Runnable reload = (Runnable) session.getActionMap().get(ReloadAction.NAME);
				new Thread(reload).start();
			}
		}	
	}

}
