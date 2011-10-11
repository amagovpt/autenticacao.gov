package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.Vector;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;
import javax.swing.table.DefaultTableModel;

/**
 * This panel shows a single column JTable, with to its right, the buttons: "Add..." and "Remove".
 * Pressing the "Add..." button displays a file chooser dialog from which a directory can be
 * selected. "Remove" removes the selected rows inside the tree.
 * 
 * @author Rogier Taal
 * 
 */
public class DirectoryListPane extends JComponent {

	private static final long serialVersionUID = 1L;	
	private JPanel tablePanel, buttonPanel;
	private JTable table;
	private Action addAction, removeAction, okAction, cancelAction;
	private JDialog dialog;
	private DirectoryListPane instance = this;
	/** User pressed OK */
	public static final Integer OK = 1;
	/** User pressed Cancel (default) */
	public static final Integer CANCEL = 0;
	private Object selectedValue = CANCEL;
	
	/**
	 * Creates a modal dialog containing this DirectoryListPane. The caller
	 * needs to call show on the returned dialog.
	 * @param owner
	 * @param title
	 * @return
	 */
	public JDialog createDialog(Frame owner, String title) {		
		dialog = new JDialog(owner, true);
		dialog.setTitle(title);		
		dialog.getContentPane().setLayout(new BorderLayout());
		dialog.getContentPane().add(this, BorderLayout.CENTER);		
		dialog.pack();
		Dimension prefSize = new Dimension(400,240);
		dialog.setMinimumSize(prefSize);
		dialog.setPreferredSize(prefSize);
		dialog.setSize(prefSize);
		dialog.setResizable(true);
		return dialog;
	}
	
	/**
	 * 
	 */
	public DirectoryListPane() {
		createTablePanel();
		okAction = new OkAction();
		cancelAction = new CancelAction();
		setLayout(new BorderLayout());
		add(tablePanel, BorderLayout.CENTER);
		
		createButtonPanel();
		JPanel separatorPanel = new JPanel(new BorderLayout());
		separatorPanel.add(new JSeparator(), BorderLayout.NORTH);
		separatorPanel.add(buttonPanel, BorderLayout.CENTER);
		add(separatorPanel, BorderLayout.SOUTH);
	}
	
	/**
	 * 
	 * @return one of {@link #OK} or {@link #CANCEL}
	 */
	public Object getSelectedValue() {
		return selectedValue;
	}
	
	/**
	 * Set the directories to display in the tree.
	 * @param dirs
	 */
	@SuppressWarnings("unchecked")
	public void setDirectories(String[] dirs) {
		DefaultTableModel model = (DefaultTableModel)table.getModel();
		Vector datavec = model.getDataVector();
		datavec.clear();
		for(String dir : dirs) {
			Vector inner = new Vector(1);
			inner.add(dir);
			datavec.add(inner);
		}
		model.fireTableDataChanged();
	}
	
	/**
	 * @return the directories currently inside the tree. There is no guarantee that the
	 * directories are valid directories.
	 */
	public String[] getDirectories() {
		int rowCnt = table.getRowCount();
		String[] ret = new String[rowCnt];
		for(int i=0; i<rowCnt; i++) {
			ret[i] = (String)table.getModel().getValueAt(i, 0);
		}
		return ret;
	}
	
	private void createTablePanel() {
		addAction = new AddAction();
		removeAction = new RemoveAction();

		tablePanel = new JPanel();
			
		table = new JTable(0, 1);
		table.setRowSelectionAllowed(true);
		table.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
	    table.setTableHeader(null);
	    table.setShowGrid(true);
	    table.addKeyListener(new SelectedRowDeleter());
	    table.setBorder(BorderFactory.createEmptyBorder());
	    
		JScrollPane tableScrollpane = new JScrollPane(table,
				JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		tableScrollpane.getViewport().setBackground(UIManager.getColor("Table.background"));
	    
		JPanel buttonPanel = new JPanel();
		GridLayout gl = new GridLayout(2,1);
		gl.setVgap(2);
		buttonPanel.setLayout(gl);
		JButton addBtn = new JButton(addAction);
		JButton removeBtn = new JButton(removeAction);
		addBtn.setText("Add...");
		removeBtn.setText("Remove");
		buttonPanel.add(addBtn);
		buttonPanel.add(removeBtn);
		
		GridBagLayout gridbag = new GridBagLayout();
		tablePanel.setLayout(gridbag);
		GridBagConstraints c = new GridBagConstraints();
		c.anchor = GridBagConstraints.NORTHWEST;
		c.insets = new Insets(8,8,8,8);
		c.weightx = 1.0;
		c.weighty = 1.0;

		c.gridy=0;
		c.fill = GridBagConstraints.BOTH;
		c.gridx=0; gridbag.setConstraints(tableScrollpane, c); tablePanel.add(tableScrollpane);
		c.fill = GridBagConstraints.NONE;
		c.weightx = 0;
		c.insets = new Insets(8,0,8,8);
		c.gridx=1; gridbag.setConstraints(buttonPanel, c); tablePanel.add(buttonPanel);
	}
	
	private void createButtonPanel() {
		buttonPanel = new JPanel();
		buttonPanel.setLayout(new FlowLayout(FlowLayout.RIGHT));		
		JButton okBut = new JButton(okAction);
		JButton cancelBut = new JButton(cancelAction);	
		okBut.setText("OK");
		cancelBut.setText("Cancel");
		buttonPanel.add(okBut);
		buttonPanel.add(cancelBut);
	}
	
	/**
	 * Action to be executed when "Add..." is pressed.
	 * @author Rogier Taal
	 */
	private class AddAction extends AbstractAction {
		private static final long serialVersionUID = 1L;
		public void actionPerformed(ActionEvent arg0) {
			JFileChooser chooser = new JFileChooser();
			chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
		    int returnVal = chooser.showOpenDialog(instance);
		    if(returnVal == JFileChooser.APPROVE_OPTION) {
		    	((DefaultTableModel)table.getModel()).addRow(new Object[]{chooser.getSelectedFile().getAbsolutePath()});
		    }
		}
	}

	/**
	 * Action to be executed when "Remove" is pressed.
	 * @author Rogier Taal
	 */
	private class RemoveAction extends AbstractAction {
		private static final long serialVersionUID = 1L;
		public void actionPerformed(ActionEvent arg0) {
			deleteSelectedRows();
		}
	}
	
	/**
	 * Action to be executed when "OK" is pressed.
	 * @author Rogier Taal
	 */
	private class OkAction extends AbstractAction {
		private static final long serialVersionUID = 1L;
		public void actionPerformed(ActionEvent arg0) {
			selectedValue = OK;
			dialog.dispose();
		}
	}

	/**
	 * Action to be executed when "Cancel" is pressed.
	 * @author Rogier Taal
	 */
	private class CancelAction extends AbstractAction {
		private static final long serialVersionUID = 1L;
		public void actionPerformed(ActionEvent arg0) {
			selectedValue = CANCEL;
			dialog.dispose();
		}
	}

	/**
	 * Calls deleteSelectedRows upon the "DELETE" key pressed event.
	 * @author Rogier Taal
	 */
	private class SelectedRowDeleter extends KeyAdapter {
		public void keyPressed(KeyEvent keyevent) {
			if(keyevent.getKeyCode() == KeyEvent.VK_DELETE) {
				deleteSelectedRows();
			}
		}
	}
		
	private void deleteSelectedRows() {
		int[] rows = table.getSelectedRows();
		if(rows.length == 0) {
			return;
		}
		DefaultTableModel model = (DefaultTableModel)table.getModel();
		
		if(rows.length == 1) {
			model.removeRow(rows[0]);
			return;
		}
		if(rows.length == model.getDataVector().size()) {
			model.getDataVector().clear();
			model.fireTableDataChanged();
			return;
		}
		int removedRows = 0;
		for(int i=0; i<rows.length; i++) {
			model.getDataVector().removeElementAt(rows[i] - removedRows++);
		}
		model.fireTableDataChanged();
	}
}
