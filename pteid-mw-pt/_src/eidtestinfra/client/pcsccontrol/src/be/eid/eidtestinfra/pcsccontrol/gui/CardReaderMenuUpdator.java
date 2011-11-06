package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Component;

import javax.swing.Action;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

/**
 * The CardReaderMenuUpdator listens to CardReaderChangeEvents and updates
 * the given menu accordingly. The menu's popup is changed dynamically even
 * while showing.
 * 
 * @author Rogier Taal
 * 
 */
public class CardReaderMenuUpdator implements CardReaderChangeListener {
	
	private JMenu menu;
	private Action unblockAction;
	
	private CardReaderMenuUpdator(JMenu menu, Action unblockAction) {
		this.menu = menu;
		this.unblockAction = unblockAction;
	}

	/**
	 * Adds a CardReaderChangeListener to the CardReaderMonitor retrieved from
	 * the MainSession. Changes are directly reflected in the given menu.
	 * @param main
	 * @param menu
	 */
	public static void install(final Main main, final JMenu menu) {
		CardReaderMonitor monitor = main.getSession().getCardReaderMonitor();
		Action unblockAction = main.getSession().getActionMap().get(CardUnblockAction.NAME);
		monitor.addReaderChangedListener(new CardReaderMenuUpdator(menu, unblockAction));
		menu.setEnabled(false);
	}

	public void cardAdded(final String name) {
		//System.out.println("cardAdded "+name);
		//if reader exists then enable and return
		for(int i=0; i<menu.getItemCount(); i++) {
			if(menu.getItem(i).getName().equals(name)) {
				menu.getItem(i).setEnabled(true);
				return;
			}
		}
		//if no reader then create one 
		JMenuItem itm = menu.add(unblockAction);
		itm.setName(name);
		itm.setText(name);
		itm.setActionCommand(name);					
		menu.setEnabled(true);
		refreshMenu(menu);		
	}

	public void cardRemoved(final String name) {
		//System.out.println("cardRemoved "+name);
		//if reader exists then disable and return
		for(int i=0; i<menu.getItemCount(); i++) {
			if(menu.getItem(i).getName().equals(name)) {
				menu.getItem(i).setEnabled(false);
				return;
			}
		}
	}

	public void readerAdded(final String name) {
		//System.out.println("readerAdded "+name);
		//if reader exists then return
		for(int i=0; i<menu.getItemCount(); i++) {
			if(menu.getItem(i).getName().equals(name)) {
				return;
			}
		}
		//if reader not exist then create reader
		JMenuItem itm = menu.add(unblockAction);
		itm.setName(name);
		itm.setText(name);
		itm.setActionCommand(name);
		itm.setEnabled(false);					
		menu.setEnabled(true);
		refreshMenu(menu);
	}

	public void readerRemoved(final String name) {
		//System.out.println("readerRemoved "+name);
		if(name == null) {
			menu.removeAll();
		} else {
			for(int i=0; i<menu.getItemCount(); i++) {
				if(menu.getItem(i).getName().equals(name)) {
					menu.remove(i);
					break;
				}
			}
		}
		menu.setEnabled(menu.getItemCount() > 0);
		refreshMenu(menu);
	}
	
	/** @see CardReaderMenuUpdator#refreshPopup(JPopupMenu) */
	private static void refreshMenu(JMenu menu) {
		if(menu.isSelected()) {
			JPopupMenu popup = menu.getPopupMenu();
			if(popup.isShowing()) {
				refreshPopup(popup);
			}
		}
	}
	
	/** Mysterious calls to pack(), invalidate() and validate() ;-) */
    private static void refreshPopup(JPopupMenu popup) {
        popup.pack ();
        popup.invalidate ();
        Component c = popup.getParent ();
        if (c != null) {
        	c.validate ();
        }
    }
}
