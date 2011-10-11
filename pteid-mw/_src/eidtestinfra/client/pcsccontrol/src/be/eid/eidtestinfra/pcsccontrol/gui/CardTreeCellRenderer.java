package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Color;
import java.awt.Component;

import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.JTree;
import javax.swing.UIManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;

import be.eid.eidtestinfra.pcsccontrol.CardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;

/**
 * Renders the tree nodes as JLables with ID Card icon and physical chip number text. The leaves are JLabels also but
 * with a black bullet or green bullet as icon and friendly name text. A green bullet indicates that the virtual card
 * is the active one set in the control file.
 * 
 * @author Rogier Taal
 * 
 */
public class CardTreeCellRenderer extends DefaultTreeCellRenderer {
	private static final long serialVersionUID = 1L;
	
	// The constants here save calling a getter inside getTreeCellRendererComponent
	// which must respond very fast as it gets called many times repeatedly
	private static final Icon SELECTED_LEAF = UIManager.getIcon("SELECTED_JTREE_LEAF_16x13");
	private static final Icon DEFAULT_LEAF = UIManager.getIcon("DEFAULT_JTREE_LEAF_16x13");
	private static final Icon VIRT_CARD_NODE = UIManager.getIcon("ICO_CARD_EID_PLAIN_16x16");
	private static final Color VALID_LEAF_FOREGROUND = UIManager.getColor("Label.foreground");
	
	public Component getTreeCellRendererComponent(
		JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus) {
		JLabel lbl = (JLabel) super.getTreeCellRendererComponent(tree, value, sel, expanded, leaf, row, hasFocus);
		DefaultMutableTreeNode leafNode = (DefaultMutableTreeNode) value;
		Object obj = leafNode.getUserObject();
		if(obj instanceof CardHolder) {
			CardHolder ch = (CardHolder) obj;
			if(ch.containsValidCard()) {
				lbl.setForeground(VALID_LEAF_FOREGROUND);
				lbl.setText(ch.getCard().getFriendlyName());
			} else {
				lbl.setForeground(Color.LIGHT_GRAY);
				lbl.setText(ch.getFile());
			}
			if(leafContainsActiveFile(leafNode)) {
				lbl.setIcon(SELECTED_LEAF);
			} else {
				lbl.setIcon(DEFAULT_LEAF);
			}
		} else if (obj instanceof ControlCardHolder) {
			ControlCardHolder cch = (ControlCardHolder) obj;
			lbl.setIcon(VIRT_CARD_NODE);
			lbl.setText(cch.getHardchipnr());
		}
		return lbl;
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
}
