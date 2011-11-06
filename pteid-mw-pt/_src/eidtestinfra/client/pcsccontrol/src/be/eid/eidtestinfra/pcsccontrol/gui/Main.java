package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.KeyEvent;
import java.awt.event.MouseListener;
import java.util.Enumeration;
import java.util.List;
import java.util.Properties;
import java.util.jar.Manifest;

import javax.swing.AbstractButton;
import javax.swing.Action;
import javax.swing.ActionMap;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTree;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.WindowConstants;
import javax.swing.border.Border;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import be.eid.eidtestinfra.pcsccontrol.CardHolder;
import be.eid.eidtestinfra.pcsccontrol.Log;
import be.eid.eidtestinfra.pcsccontrol.ControlCardHolder;
import be.eid.eidtestinfra.pcsccontrol.ControlModel;
import be.eid.eidtestinfra.pcsccontrol.PropertyModel;
import be.eid.eidtestinfra.pcsccontrol.Visibility;

/**
 * Main is the entry point for this GUI. This class shows a JTree on the left and a {@link CardHolderPanel}
 * on the right. When a leaf node is selected the right side panel is filled with data.
 *  
 * @author Rogier Taal
 * 
 */
public class Main implements TreeSelectionListener {
	
	private Manifest manifest;
	private MainSession session;
	private JFrame frame;
	private JSplitPane splitpane;
	private JTree tree;
	private JMenuBar topMenuBar;
	private ButtonGroup visbBtnGroup;
	private JPanel leftCards;
	private boolean firstLoad;
	private static final String TREEPANEL = "Card with JTree";
	private static final String NOITEMSPANEL = "Card with no items";
	
	/**
	 * 
	 * @param propFilePath
	 * @param manifest
	 */
	public Main(Manifest manifest) {
		this.manifest = manifest;
		PropertyModel pm = null;
		ControlModel cm = null;

		pm = new PropertyModel();
		try {
			pm.reload();			
		}
		catch(final Exception e) {
			if (Log.logger != null)
				Log.logger.error("", e);
			Toolkit.showErrorDialog(null, e);
			System.exit(0);
		}

		cm = new ControlModel(pm.getControlFile());
		
		session = new MainSession(this, pm, cm);
		UIManager.getDefaults().putAll(Toolkit.icons);
		createGuiNotRealize();
		
		/////////////
		// realize //
		/////////////
		frame.pack();

		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				Properties props = session.getPropertyModel().getProperties();
				String size = props.getProperty("main.size");
				String location = props.getProperty("main.location");
			    frame.setLocation(Toolkit.toPoint(location));
				if(size != null) {
					if(size.equals("00,00"))
						frame.setExtendedState(Frame.MAXIMIZED_BOTH);
					else
						frame.setSize(Toolkit.toDimension(size));
				}
				frame.setVisible(true);				
			}
		});
		
		Runnable r;

		if(cm.getFilePath() == null) {
			r = new MultiRunnable(new Runnable[] {
				(Runnable) session.getActionMap().get(SelectControlFileAction.NAME),
				(Runnable) session.getActionMap().get(ReloadAction.NAME)
			});
		} else {
			r = (Runnable) session.getActionMap().get(ReloadAction.NAME);
		}
		
		firstLoad = true;
		
		new Thread(r).start();
		
		CardReaderMonitor cmon = session.getCardReaderMonitor(); 
		if(cmon != null) {
			cmon.start();
		}
	}
	
	/**
	 * 
	 * @return
	 */
	public MainSession getSession() {
		return session;
	}
	
	/**
	 * 
	 * @return
	 */
	public Manifest getManifest() {
		return manifest;
	}

	public void valueChanged(TreeSelectionEvent arg0) {
		Object selected = null;
		if(!(arg0 == null || arg0.getNewLeadSelectionPath() == null)) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode) arg0.getNewLeadSelectionPath().getLastPathComponent();
			selected = node.getUserObject();
		}
		if( selected instanceof CardHolder ) {
			((CardHolderPanel)splitpane.getRightComponent()).setModel((CardHolder)selected);
		} else {
			((CardHolderPanel)splitpane.getRightComponent()).setModel(null);
		}
	}
	
	/**
	 * This method gets the items from the MainSession and recreates all nodes and leaves.
	 * The visibility is set according to the value inside the control model.
	 */
	void reload() {
		////////////////////////////////
		// Remember the selected path //
		////////////////////////////////
		String[] previousSelected = null;
		if(firstLoad) {
			firstLoad = false;
			String savedSelected = session.getPropertyModel().getProperties().getProperty("tree.selected");
			if(savedSelected != null) {
				previousSelected = savedSelected.split(",");
			}
		} else {
			previousSelected = getSelected();
		}
		
		//////////////////////
		// Rebuild the tree //
		//////////////////////
		List<ControlCardHolder> items = session.getItems();
		DefaultMutableTreeNode rootNode = (DefaultMutableTreeNode)tree.getModel().getRoot();
		rootNode.removeAllChildren();
		for(ControlCardHolder cch : items) {
	    	DefaultMutableTreeNode cchNode = new DefaultMutableTreeNode(cch);
	    	rootNode.add(cchNode);
	    	for(int i=0; i<cch.size(); i++) {
	    		DefaultMutableTreeNode chNode = new DefaultMutableTreeNode(cch.get(i));
	    		cchNode.add(chNode);
	    	}
	    }
		((DefaultTreeModel)tree.getModel()).reload();
		
		/////////////////////////////////////
		// Select path previously selected //
		/////////////////////////////////////
		TreePath selectedUserObjectPath = null;
		if(previousSelected != null) {
			for(int i=0; i<rootNode.getChildCount(); i++) {
				DefaultMutableTreeNode n1 = (DefaultMutableTreeNode) rootNode.getChildAt(i);
				String postHardChipnr = ((ControlCardHolder)n1.getUserObject()).getHardchipnr();
				String prevHardChipnr = previousSelected[0];
				if(postHardChipnr.equals(prevHardChipnr)) {
					if(previousSelected.length > 1) {
						for(int j=0; j<n1.getChildCount(); j++) {
							DefaultMutableTreeNode n2 = (DefaultMutableTreeNode) n1.getChildAt(j);
							String postFile = ((CardHolder)n2.getUserObject()).getFile();
							String prevFile = previousSelected[1];
							if(postFile.equals(prevFile)) {
								selectedUserObjectPath = new TreePath(n2.getPath());
							}
						}
					} else {
						selectedUserObjectPath = new TreePath(n1.getPath());
					}
					break;
				}
			}
		}

		tree.getSelectionModel().setSelectionPath(selectedUserObjectPath);
		Toolkit.expandAll(tree);
		
		///////////////////////////////////////////
		// Set the correct card on the left side //
		///////////////////////////////////////////
		if(items.size() > 0) {
			((CardLayout)leftCards.getLayout()).show(leftCards, TREEPANEL);
		} else {
			((CardLayout)leftCards.getLayout()).show(leftCards, NOITEMSPANEL);
		}
		
		////////////////////
		// set visibility //
		////////////////////		
		Visibility visb = session.getControlModel().getVisibility();
		Enumeration<AbstractButton> e = visbBtnGroup.getElements();
		while(e.hasMoreElements()) {
			AbstractButton btn = e.nextElement();
			boolean select = btn.getModel().getActionCommand().equals(visb.getValue());
			if(select) {
				visbBtnGroup.setSelected(btn.getModel(), select);
				btn.setSelected(select);
				break;
			}
		}
	}
	
	/**
	 * 
	 * @return
	 */
	JTree getTree() {
		return tree;
	}
	
	/**
	 * 
	 * @return
	 */
	public JFrame getFrame() {
		return frame;
	}	
	
	/**
	 * Sets the properties related to how the interface appears on the screen so that
	 * at the next launch of the gui things like size and location are the same.
	 */
	void saveProperties() {
		Properties props = session.getPropertyModel().getProperties();
		props.setProperty("main.location", Toolkit.toString(frame.getLocation()));
		if( (frame.getExtendedState()&Frame.MAXIMIZED_BOTH) !=0 ) {
			props.setProperty("main.size", "00,00");
		} else {
			props.setProperty("main.size", Toolkit.toString(frame.getSize()));	
		}
		String[] selected = getSelected();
		if(selected != null) {
			String val = null;
			if(selected.length >= 1)
				val = selected[0];
			if(selected.length > 1)
				val += ","+selected[1];
			
			props.setProperty("tree.selected", val);
		} else {
			props.remove("tree.selected");
		}
	}
	
	private String[] getSelected() {
		String[] previousSelected = null;
		TreePath treePath[] = tree.getSelectionPaths();
		if(treePath != null) {
			Object[] userObjectPath = ((DefaultMutableTreeNode) treePath[0].getLastPathComponent()).getUserObjectPath();
			if(userObjectPath.length > 1) {
				previousSelected = new String[userObjectPath.length-1];
				previousSelected[0] = ((ControlCardHolder)userObjectPath[1]).getHardchipnr();
			}
			if(userObjectPath.length > 2) {
				previousSelected[1] = ((CardHolder)userObjectPath[2]).getFile();
			}
		}
		return previousSelected;
	}
	
	private void createGuiNotRealize() {
		createActions();
		createTopMenuBar();
		frame = new JFrame("Pcsc Control");
		frame.setJMenuBar(topMenuBar);
		ImageIcon icon = (ImageIcon)UIManager.getIcon("ICO_CARD_EID_PLAIN_16x16");
		frame.setIconImage(icon.getImage());
		frame.addWindowListener((ExitAction)session.getActionMap().get(ExitAction.NAME));
		frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		
	    tree = new JTree(new DefaultMutableTreeNode("root"));
	    tree.setRootVisible(false);
	    tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
	    tree.setCellRenderer(new CardTreeCellRenderer());
	    tree.addTreeSelectionListener(this);
	    tree.addMouseListener((MouseListener) session.getActionMap().get(CardTreePopupAction.NAME));
	    SwingUtilities.getUIActionMap(tree).setParent(session.getActionMap());
		tree.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_F5, 0), ReloadAction.NAME);
		tree.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_S, KeyEvent.CTRL_DOWN_MASK), SaveAction.NAME);
		tree.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_F10, KeyEvent.SHIFT_DOWN_MASK), CardTreePopupAction.NAME);

	    JScrollPane treeView = new JScrollPane(tree);
	    treeView.setBorder(BorderFactory.createEmptyBorder());

		Border emptyBorderWithInsets = BorderFactory.createEmptyBorder(4, 6, 4, 6);
		tree.setBorder(emptyBorderWithInsets);
	    JPanel noItemsPanel = new NoItemsPanel();
	    noItemsPanel.setBorder(emptyBorderWithInsets);
	    noItemsPanel.setBackground(Color.WHITE);
	    
		leftCards = new JPanel(new CardLayout());
		leftCards.add(treeView, TREEPANEL);
		leftCards.add(noItemsPanel, NOITEMSPANEL);
	    
	    splitpane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
		splitpane.setBorder(BorderFactory.createEmptyBorder());
		splitpane.add(leftCards, JSplitPane.LEFT);
		splitpane.add(new CardHolderPanel(), JSplitPane.RIGHT);
		splitpane.setDividerSize(4);
		splitpane.setResizeWeight(1.0);
		
	    frame.getContentPane().add(splitpane);
	    Dimension prefSize = frame.getPreferredSize();
	    frame.setMinimumSize(new Dimension(prefSize.width+200,prefSize.height+40));
	}
	
	private void createTopMenuBar() {
		topMenuBar = new JMenuBar();
		topMenuBar.setBorder(BorderFactory.createMatteBorder(0,0,1,0,topMenuBar.getBackground().darker()));
		JMenu fileMenu = new JMenu("File");
		JMenu optionsMenu = new JMenu("Options");
		JMenu cardMenu = new JMenu("Card");
		JMenu helpMenu = new JMenu("Help");
	    
		JMenuItem reloadItm = fileMenu.add(session.getActionMap().get(ReloadAction.NAME));
		reloadItm.setText("Reload");
		reloadItm.setIcon(UIManager.getIcon("ManageAPI_16x16"));
		reloadItm.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F5, 0));
		fileMenu.addSeparator();
		JMenuItem saveItm = fileMenu.add(session.getActionMap().get(SaveAction.NAME));
		saveItm.setText("Save");
		saveItm.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, KeyEvent.CTRL_DOWN_MASK));
		fileMenu.add(session.getActionMap().get(ExitAction.NAME)).setText("Exit");
		
/*
STH: no use in selecting another control file
		optionsMenu.add(session.getActionMap().get(SelectControlFileAction.NAME)).setText("Select Control File...");
*/
		optionsMenu.add(session.getActionMap().get(SetIncludeDirAction.NAME)).setText("Virtual Card Include Directories...");
		
		JMenu visbMenu = (JMenu) optionsMenu.add(new JMenu("Card Reader Visibility"));
	
		Action visibilitySetAction = session.getActionMap().get(SetVisibilityAction.NAME);
		visbBtnGroup = new ButtonGroup();
	    JRadioButtonMenuItem menuItem = new JRadioButtonMenuItem(visibilitySetAction);
	    menuItem.setText("Hide physical card reader");
	    menuItem.getModel().setActionCommand(Visibility.HIDE_REAL.getValue());
	    visbBtnGroup.setSelected(menuItem.getModel(), true);
	    visbBtnGroup.add(menuItem);
	    visbMenu.add(menuItem);
	    menuItem = new JRadioButtonMenuItem(visibilitySetAction);
	    menuItem.setText("Hide virtual card reader");
	    menuItem.setActionCommand(Visibility.HIDE_VIRTUAL.getValue());
	    visbBtnGroup.add(menuItem);
	    visbMenu.add(menuItem);
	    menuItem = new JRadioButtonMenuItem(visibilitySetAction);
	    menuItem.setText("List physical card reader first");
	    menuItem.setActionCommand(Visibility.REAL_FIRST.getValue());
	    visbBtnGroup.add(menuItem);
	    visbMenu.add(menuItem);
	    menuItem = new JRadioButtonMenuItem(visibilitySetAction);
	    menuItem.setText("List virtual card reader first");
	    menuItem.setActionCommand(Visibility.REAL_LAST.getValue());
	    visbBtnGroup.add(menuItem);
	    visbMenu.add(menuItem);
	    
	    if(session.getCardReaderMonitor() != null) {
		    JMenu unblockMenu = new JMenu("Unblock");
		    cardMenu.add(unblockMenu);
		    CardReaderMenuUpdator.install(this, unblockMenu);
	    }
	    
	    JMenuItem aboutItem = helpMenu.add(session.getActionMap().get(ShowAboutAction.NAME));
		aboutItem.setText("About");
				
		topMenuBar.add(fileMenu);
		topMenuBar.add(optionsMenu);
		if(session.getCardReaderMonitor() != null) {
	    	topMenuBar.add(cardMenu);
	    }
		topMenuBar.add(Box.createHorizontalGlue());
		topMenuBar.add(helpMenu);
	}
	
	private void createActions() {
		ActionMap actionMap = session.getActionMap();
		actionMap.put(ExitAction.NAME, new ExitAction(this));
		actionMap.put(ToggleActiveFileAction.NAME, new ToggleActiveFileAction(this));
		actionMap.put(SetIncludeDirAction.NAME, new SetIncludeDirAction(this));
		actionMap.put(ReloadAction.NAME, new ReloadAction(this));
		actionMap.put(SaveAction.NAME, new SaveAction(this));
		actionMap.put(SetVisibilityAction.NAME, new SetVisibilityAction(this));
		actionMap.put(SelectControlFileAction.NAME, new SelectControlFileAction(this));
		actionMap.put(ShowAboutAction.NAME, new ShowAboutAction(this));
		actionMap.put(CardTreePopupAction.NAME, new CardTreePopupAction(this));
		if(session.getCardReaderMonitor() != null) {
			actionMap.put(CardUnblockAction.NAME, new CardUnblockAction(this));
		}
	}
	
	/**
	 * Panel shows a hyperlink to the {@link SetIncludeDirAction}. This panel is
	 * shown when there are no items in the tree.
	 * @author Rogier Taal
	 */
	private class NoItemsPanel extends JPanel {
		private static final long serialVersionUID = 1L;
		public NoItemsPanel() {
			setBackground(Color.WHITE);
			GridBagLayout gridbag = new GridBagLayout();
			setLayout(gridbag);
			GridBagConstraints c = new GridBagConstraints();
			c.anchor = GridBagConstraints.NORTH;
			c.insets = new Insets(0,0,2,0);
			c.weighty = 0;			
			JLabel lbl1 = new JLabel("There are no Virtual Cards to show");
			lbl1.setFont(lbl1.getFont().deriveFont(Font.ITALIC));
			LinkButton btn1 = new LinkButton(session.getActionMap().get(SetIncludeDirAction.NAME));
			btn1.setText("Add include directories...");
			btn1.setFont(btn1.getFont().deriveFont(Font.ITALIC));
			btn1.setAlwaysUnderlined(true);
			c.gridy=0; gridbag.setConstraints(lbl1, c); add(lbl1);
			c.weighty = 1.0;
			c.gridy=1; gridbag.setConstraints(btn1, c); add(btn1);
		}
	}
}
