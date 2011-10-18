package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;

import javax.swing.Action;
import javax.swing.AbstractAction;

import be.eid.eidtestinfra.pcsccontrol.ControlModel;

/**
 * Upon {@link #actionPerformed(ActionEvent)} the visibility is set on the {@link ControlModel}.
 * 
 * @author Rogier Taal
 * 
 */
public class SetVisibilityAction extends AbstractAction {
	private static final long serialVersionUID = 1L;

	public static final String NAME = "ACTION_SETVISIBILITY";
	
	private Main main;
	
	public SetVisibilityAction(Main main) {
		super(NAME);
		this.main = main;
	}

	public void actionPerformed(ActionEvent arg0) {
		main.getSession().getControlModel().setVisibility(arg0.getActionCommand());

		Action action = this.main.getSession().getActionMap().get(SaveAction.NAME);
		if (action != null && action instanceof SaveAction)
			((SaveAction) action).actionPerformed(arg0);

	}
}
