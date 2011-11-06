package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Component;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;

import be.eid.eidtestinfra.pcsccontrol.CardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;

/**
 * CardTreePopupListener is added to the JTree in Main. It shows the JPopupMenu given inside
 * the constructor and sets the single option either to "Set Active" or "UnSet Active".
 * 
 * @author Rogier Taal
 * 
 */
public class CardTreePopupAction extends AbstractAction implements MouseListener {
	
	private static final String SET_ACTIVE = "Set Active";
	private static final String UNSET_ACTIVE = "UnSet Active";
	private static final long serialVersionUID = 1L;
	
	public static final String NAME = "ACTION_CardTreePopupAction";
	
	private Main main;
	
	public CardTreePopupAction(Main main) {
		super(NAME);
		this.main = main;
	}
	
	public void actionPerformed(ActionEvent e) {
		JTree tree = main.getTree();
		TreePath selPath = tree.getSelectionPath();
		if(selPath != null) {
			Rectangle rect = tree.getPathBounds(selPath);
			MouseEvent event = new MouseEvent(tree, MouseEvent.MOUSE_PRESSED, System.currentTimeMillis(), 0,
					(int)rect.getCenterX() ,(int)rect.getCenterY(), 1, true);
			maybeShowPopup(event);
		}
	}
		
	public void mousePressed(MouseEvent e) {
		if(e.isPopupTrigger() && ((Component)e.getSource()).isEnabled())
			maybeShowPopup(e);
    }

	public void mouseReleased(MouseEvent e) {
    	if(e.isPopupTrigger() && ((Component)e.getSource()).isEnabled())
    		maybeShowPopup(e);
    }
	
    private void maybeShowPopup(MouseEvent e) {
    	JTree tree = ((JTree)e.getComponent());
    	TreePath selPath = tree.getPathForLocation(e.getX(), e.getY());
    	if(selPath != null) {
    		DefaultMutableTreeNode node = (DefaultMutableTreeNode) selPath.getLastPathComponent();
    		Object obj = node.getUserObject();
    		if(obj instanceof CardHolder) {
    			tree.getSelectionModel().setSelectionPath(selPath);
    			JPopupMenu popup = new JPopupMenu();
    			Action toggleActiveFileAction = main.getSession().getActionMap().get(ToggleActiveFileAction.NAME);
    			if(!leafContainsActiveFile(node)) {
    				popup.add(toggleActiveFileAction).setText(SET_ACTIVE);	        	            
    			} else {
    				popup.add(toggleActiveFileAction).setText(UNSET_ACTIVE);
    			}
    			popup.show(e.getComponent(), e.getX(), e.getY());
    		}
    	}
    }
    
	/**
	 * @param node leaf node that contains a CardHolder user object.
	 * @return true when the virtual card is the active one set in the control file.  
	 */
    private static boolean leafContainsActiveFile(DefaultMutableTreeNode node) {
		Object obj = node.getUserObject();
		if(obj instanceof CardHolder) {
			CardHolder ch = (CardHolder) obj;
			DefaultMutableTreeNode parent = (DefaultMutableTreeNode) node.getParent();	        			
			ControlCardHolder cch = (ControlCardHolder) parent.getUserObject();
			String controlPath = cch.getFile();
			String virtCardPath = ch.getFile();
			return (controlPath != null
					&& (Toolkit.isWindows ? controlPath.equalsIgnoreCase(virtCardPath) : controlPath.equals(virtCardPath)));
		}
		return false;
	}

	public void mouseClicked(MouseEvent e) {
	}

	public void mouseEntered(MouseEvent e) {
	}

	public void mouseExited(MouseEvent e) {
	}
}
