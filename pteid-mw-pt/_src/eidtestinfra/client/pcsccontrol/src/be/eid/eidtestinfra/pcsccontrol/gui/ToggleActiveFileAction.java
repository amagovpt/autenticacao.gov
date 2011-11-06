package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.event.ActionEvent;

import javax.swing.Action;
import javax.swing.AbstractAction;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import be.eid.eidtestinfra.pcsccontrol.CardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;

/**
 * Upon {@link #actionPerformed(ActionEvent)} and for the currently selected leaf node inside the
 * JTree contained by the instance of {@link Main}, the active virtual file is deactivated when currently
 * activated or activated when currently not activated according to the control file.
 * 
 * @author Rogier Taal
 * 
 */
public class ToggleActiveFileAction extends AbstractAction {
	
	public static final String NAME = "TOGGLEACTIVEFILE_ACTION";
	private Main main;
	
	public ToggleActiveFileAction(Main main) {
		super(NAME);
		this.main = main;
	}

	private static final long serialVersionUID = 1L;

	public void actionPerformed(ActionEvent actionevent) {
		TreePath selPath = main.getTree().getSelectionPaths()[0];
		DefaultMutableTreeNode leafNode = (DefaultMutableTreeNode) selPath.getLastPathComponent();
		DefaultMutableTreeNode parent = (DefaultMutableTreeNode) leafNode.getParent();		
		CardHolder ch = (CardHolder) leafNode.getUserObject();
		ControlCardHolder cch = (ControlCardHolder) parent.getUserObject();
		if(cch.getFile() != null && cch.getFile().equals(ch.getFile())) {
			cch.setFile(null);
		} else {
			cch.setFile(ch.getFile());
		}
		nodeChanged(selPath);

		Action action = this.main.getSession().getActionMap().get(SaveAction.NAME);
		if (action != null && action instanceof SaveAction)
			((SaveAction) action).actionPerformed(actionevent);
	}
	
	private void nodeChanged(TreePath path) {
		TreeModelListener[] listeners = ((DefaultTreeModel)main.getTree().getModel()).getTreeModelListeners();
		TreeModelEvent e = new TreeModelEvent(this, path);
		for(TreeModelListener lst : listeners) {			
			lst.treeNodesChanged(e);
		}
	}
}
