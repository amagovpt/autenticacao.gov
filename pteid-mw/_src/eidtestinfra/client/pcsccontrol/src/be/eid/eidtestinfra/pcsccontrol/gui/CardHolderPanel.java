package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Image;
import java.awt.Insets;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.Box;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import be.eid.eidtestinfra.pcsccontrol.Card;
import be.eid.eidtestinfra.pcsccontrol.CardHolder;
import be.eid.eidtestinfra.pcsccontrol.Card.File;

/**
 * CardHolderPanel is the view for the {@link CardHolder} object (the model). The file path is shown,
 * a number of attributes from {@link Card} including the picture.
 * 
 * @author Rogier Taal
 * 
 */
public class CardHolderPanel extends JPanel {
	
	private static final long serialVersionUID = 1L;
	private JTextField pathFld = new JTextField(40);
	private JTextField typeFld = new JTextField(30);
	private JTextField testCardVersionFld = new JTextField(30);
	private JTextField friendlyNameFld = new JTextField(30);
	private JTextField chipNumberFld = new JTextField(30);
	private JTextField physicalCardChipNumberFld = new JTextField(30);
	private JTextField versionNumberFld = new JTextField(30);
	private JTextField atrFld = new JTextField(30);
	private ImagePanel imgPanel = new ImagePanel();
	
	private static final String BEID_IMAGE_FILE = "3F00DF014035";
	
	/**
	 * 
	 */
	public CardHolderPanel() {
		super();
		
		pathFld.setEditable(false);
		typeFld.setEditable(false);
		testCardVersionFld.setEditable(false);
		friendlyNameFld.setEditable(false);
		chipNumberFld.setEditable(false);
		physicalCardChipNumberFld.setEditable(false);
		versionNumberFld.setEditable(false);
		atrFld.setEditable(false);
		
		JLabel pathLbl = new JLabel("Path:");
		JLabel typeLbl = new JLabel("Type:");
		JLabel testCardVersionLbl = new JLabel("Test Card Version:");
		JLabel friendlyNameLbl = new JLabel("Friendly Name:");
		JLabel chipNumberLbl = new JLabel("Chip Number:");
		JLabel physicalCardChipNumberLbl = new JLabel("Physical Card Chip Number:");
		JLabel versionNumberLbl = new JLabel("Version Number:");
		JLabel atrLbl = new JLabel("Atr:");
		
		GridBagLayout gridbag = new GridBagLayout();
		setLayout(gridbag);
		GridBagConstraints c = new GridBagConstraints();
		c.anchor = GridBagConstraints.NORTHWEST;
		c.insets = new Insets(8,8,2,8);
		
		c.gridx=0;
		c.gridy=0; gridbag.setConstraints(pathLbl, c); add(pathLbl);
		c.gridy++; gridbag.setConstraints(typeLbl, c); add(typeLbl);
		c.gridy++; gridbag.setConstraints(testCardVersionLbl, c); add(testCardVersionLbl);
		c.gridy++; gridbag.setConstraints(friendlyNameLbl, c); add(friendlyNameLbl);
		c.gridy++; gridbag.setConstraints(chipNumberLbl, c); add(chipNumberLbl);
		c.gridy++; gridbag.setConstraints(physicalCardChipNumberLbl, c); add(physicalCardChipNumberLbl);
		c.gridy++; gridbag.setConstraints(versionNumberLbl, c); add(versionNumberLbl);
		c.gridy++; gridbag.setConstraints(atrLbl, c); add(atrLbl);
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.gridheight = GridBagConstraints.REMAINDER;
		c.insets = new Insets(16,8,8,8);
		c.gridy++; gridbag.setConstraints(imgPanel, c); add(imgPanel);
		
		c.weightx = 1.0;
		c.weighty = 1.0;
		Component glue = Box.createGlue();
		c.gridy++; gridbag.setConstraints(glue, c); add(glue);
		
		c.insets = new Insets(8,8,2,8);
		c.weightx = 0;
		c.weighty = 0;
		c.gridx=1;
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.gridheight = GridBagConstraints.RELATIVE;
		c.gridy=0; gridbag.setConstraints(pathFld, c); add(pathFld);
		c.gridy++; gridbag.setConstraints(typeFld, c); add(typeFld);
		c.gridy++; gridbag.setConstraints(testCardVersionFld, c); add(testCardVersionFld);
		c.gridy++; gridbag.setConstraints(friendlyNameFld, c); add(friendlyNameFld);
		c.gridy++; gridbag.setConstraints(chipNumberFld, c); add(chipNumberFld);
		c.gridy++; gridbag.setConstraints(physicalCardChipNumberFld, c); add(physicalCardChipNumberFld);
		c.gridy++; gridbag.setConstraints(versionNumberFld, c); add(versionNumberFld);
		c.gridy++; gridbag.setConstraints(atrFld, c); add(atrFld);
		
		       	       	
       	setMinimumSize(getPreferredSize());
	}
	
	/**
	 * If the given CardHolder is null then all fields are cleared. Otherwise the path field
	 * is set. If ch.containsValidCard() returns true then all other fields are set as well.
	 * @param ch or null
	 */
	public void setModel(CardHolder ch) {
		clear();
		if(ch == null) {
			return;
		}
		pathFld.setText(ch.getFile());
		if(!ch.containsValidCard()) {
			return;
		}
		Card c = ch.getCard();
		pathFld.setText(ch.getFile());
		typeFld.setText(c.getType());
		testCardVersionFld.setText(c.getTestCardVersion());
		friendlyNameFld.setText(c.getFriendlyName());
		chipNumberFld.setText(c.getChipNumber());
		physicalCardChipNumberFld.setText(c.getPhysicalCardChipNumber());
		versionNumberFld.setText(c.getVersionNumber());
		atrFld.setText(c.getAtr());
				
		List<File> fl = c.getFile();
		byte[] imgHex = null;
		for(int i=0; i<fl.size(); i++) {
			if(fl.get(i).getId().equals(BEID_IMAGE_FILE))
				imgHex = hexstring2hex(fl.get(i).getContent());
		}
		if(imgHex != null) {
			try {
				imgPanel.setImage(ImageIO.read(new ByteArrayInputStream(imgHex)));
			} catch(IOException ignored) {
			}
		}
	}
	
	/**
	 * Clear all fields.
	 */
	private void clear() {
		imgPanel.setImage(null);
		pathFld.setText(null);
		typeFld.setText(null);
		testCardVersionFld.setText(null);
		friendlyNameFld.setText(null);
		chipNumberFld.setText(null);
		physicalCardChipNumberFld.setText(null);
		versionNumberFld.setText(null);
		atrFld.setText(null);
	}
	
	/**
	 * Converts hex string to a byte array. For example "A4F1" becomes two bytes [0xA4, 0xF1].
	 * @param hex
	 * @return
	 */
	private static byte[] hexstring2hex(String hex) {		
		if(hex == null || (hex.length() & 1) != 0) { //if size is uneven return null
			return null;
		}
		char[] hexc = hex.toCharArray();
		byte[] hexb = new byte[hexc.length/2];
		for(int i=0; i<hexb.length; i++) {
			hexb[i] = (byte) (16*Character.digit(hexc[2*i], 16)+Character.digit(hexc[2*i+1],16));
		}
		return hexb;
	}
	
	/**
	 * Panel of fixed size 140x200 with a line border.
	 * @author Rogier Taal
	 *
	 */
	private static class ImagePanel extends JPanel {
		private static final long serialVersionUID = 1L;
		private static final Dimension SIZE = new Dimension(140,200);
		private Image img;
		
		public Dimension getPreferredSize() {
			return SIZE;
		}
		
		public void setImage(Image img) {
			this.img = img;
			repaint();
		}
		
		public void paint(Graphics g) {
			if(img != null) {
				g.drawImage(img,0,0,this);				
			} else {
				super.paint(g);
			}
			g.drawRect(0, 0, SIZE.width-1, SIZE.height-1);
		}
	}
}
