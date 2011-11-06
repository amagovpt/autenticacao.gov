package be.eid.eidtestinfra.pcsccontrol;

import java.io.File;
import java.io.FileOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import javax.xml.XMLConstants;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;

import be.eid.eidtestinfra.pcsccontrol.gui.Toolkit;

/**
 * ControlModel contains methods for loading and saving the {@link Control} instance from and to
 * file. First an instance needs to be made with a file path then {@link #reload()} is called to
 * parse the control file and create the Control instance.
 *  
 * @author Rogier Taal
 * 
 */
public class ControlModel {
	
	/** Control that receives changes until setSaved is called */
	private Control activeControl;
	/** Control that is cached to allow to spot any changes */
	private Control originalControl;

	private File controlFile;
	
	private static final String DEFAULT_FILE_NAME = "PteidTestCtrl.xml";
	
	public static final String DEFAULT_FILE_PATH = System.getProperty("ALLUSERSPROFILE") != null ?
			new File(System.getProperty("ALLUSERSPROFILE"), DEFAULT_FILE_NAME).getAbsolutePath() : null;
	
	/** List of allowed visibility modes */
	public static final List<String> VISB_MODES = Arrays.asList(new String[]{
			Visibility.HIDE_REAL.getValue(),
			Visibility.REAL_FIRST.getValue(),
			Visibility.REAL_LAST.getValue(),
			Visibility.HIDE_VIRTUAL.getValue() });
	
	private static Unmarshaller cardUnmarshaller, controlUnmarshaller;
	private static Marshaller cardMarshaller, controlMarshaller;
	
	static {
		try {
			JAXBContext context = JAXBContext.newInstance("be.eid.eidtestinfra.pcsccontrol");
			SchemaFactory schemaFactory = SchemaFactory
					.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
			Schema scardSchema = schemaFactory.newSchema(ControlModel.class.getClassLoader ().getResource("scard.xsd"));
	 		Schema controlSchema = schemaFactory.newSchema(ControlModel.class.getClassLoader ().getResource("control.xsd"));
			cardUnmarshaller = context.createUnmarshaller();
			cardUnmarshaller.setSchema(scardSchema);
			controlUnmarshaller = context.createUnmarshaller();
			controlUnmarshaller.setSchema(controlSchema);
			cardMarshaller = context.createMarshaller();
			cardMarshaller.setSchema(scardSchema);	
			controlMarshaller = context.createMarshaller();
			controlMarshaller.setSchema(controlSchema);
			controlMarshaller.setProperty("jaxb.formatted.output", Boolean.TRUE);
		} catch(Throwable t) { //cannot happen unless the jar was not packaged correctly 
			if (Log.logger != null)
				Log.logger.error("Startup error", t);
			t.printStackTrace();
		}
	}

	/**
	 * 
	 * @param filePath a path to a control file or null
	 */
	public ControlModel(String filePath) {
		//set defaults
		activeControl = new Control();
		activeControl.show = Visibility.HIDE_REAL.getValue();
		//to make comparing easier later on make sure that the virtualCard array is not null
		//causes the array to be created
		activeControl.getVirtualcard();
		originalControl = copyOf(activeControl);
		setFilePath(filePath);
	}
	
	/**
	 * Parse the control file xml and create a Control instance. 
	 * @throws FileNotFoundException when the control file is not readable or
	 * cannot be parsed because it does not comply to the control file xml format.
	 */
	public void reload() throws FileNotFoundException {
		if(controlFile != null && controlFile.exists()) {
			if (!controlFile.canRead()) {
				if (Log.logger != null)
					Log.logger.error("Can't read control file (access denied)");
				throw new FileNotFoundException(controlFile + " (Access is denied)");
			}
			if (controlFile.length() == 0) {
				//set defaults
				activeControl = new Control();
				activeControl.show = Visibility.HIDE_REAL.getValue();
			} else {
				try {
						byte[] contents = readFile(this.controlFile);
						contents = replaceXmlElement(contents, "control",
							"control xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://env.dev.eid.portugal.be/eidtestinfra http://env.dev.eid.portugal.be/schemas/control.xsd\"");
					activeControl = (Control) controlUnmarshaller.unmarshal(new ByteArrayInputStream(contents));
				} catch (IOException ex) {
					if (Log.logger != null)
						Log.logger.error("Can't read control file", ex);
					throw new FileNotFoundException(controlFile + " (can't read file)");
				} catch (JAXBException jaxbe) {
					if (Log.logger != null)
						Log.logger.error("Corrupt control file", jaxbe);
					throw new FileNotFoundException(controlFile + " (File is corrupt)");
				}
			}
		}
		//to make comparing easier later on make sure that the virtualCard array is not null
		//causes the array to be created
		activeControl.getVirtualcard();
		originalControl = copyOf(activeControl);
	}
	
	/**
	 * 
	 * @return the control file path or null
	 */
	public String getFilePath() {
		return (controlFile != null ? controlFile.getAbsolutePath() : null);
	}
	
	/**
	 * 
	 * @param filePath or null
	 */
	public void setFilePath(String filePath) {
		if(filePath != null) {
			File controlFile = new File(filePath);
			if(!controlFile.isDirectory()) {
				if(this.controlFile == null || !this.controlFile.equals(controlFile)) {
					this.controlFile = controlFile;
				}
			}
		} else {
			this.controlFile = null;
		}

		if (Log.logger != null)
			Log.logger.info("Control file: " + (this.controlFile == null ?
				"<null>" : this.controlFile.getAbsolutePath()));
	}
	
	/**
	 * This methods compares the changes made to the Control instance since the last reload or save
	 * was made.
	 * @return true when changes have been made and need to be saved
	 */
	public boolean isDirty() {
		return !controlEquals(activeControl, originalControl);
	}
	
	/**
	 * Override List of ControlCardHolders with the given list.
	 * @param items
	 */
	public void replaceItems(List<ControlCardHolder> items) {
		saveToControl(activeControl, items);
	}

	/**
	 * Replace and XML element by something else.
	 * Used to modify the control file before it's written to disk and
	 * after it's read from disk; an also to modify the virtual card
	 * files after they are read from disk.
	 */
	private byte[] replaceXmlElement(byte[] xml, String elem, String replaceBy)
	{
		String str = new String(xml);
		int idx1 = str.indexOf("<" + elem + " ");
		if (idx1 != -1) {
				int idx2 = str.indexOf(">", idx1 + 5);
				if (idx2 != -1) {
					str = str.substring(0, idx1 + 1) + replaceBy + str.substring(idx2);
					xml = str.getBytes();
				}
		}

		return xml;
	}

	byte[] readFile(File f) throws IOException
	{
		byte[] buf = new byte[2048];
		ByteArrayOutputStream baos = new ByteArrayOutputStream(30000);
		FileInputStream fis = new FileInputStream(f);
		int len = fis.read(buf);
		while (len != -1) {
			baos.write(buf, 0, len);
			len = fis.read(buf);
		}
		return baos.toByteArray();
	}
	
	/**
	 * This methods saves changes made to the control since the last reload or save
	 * was made.
	 * @param items or null
	 * @throws IOException
	 */
	public synchronized void save() throws IOException {		
		File tmpFile = new File(controlFile.getAbsolutePath()+"~");
		//backup control file
		if(controlFile.exists()) {
			try {
				Toolkit.copyFile(controlFile, tmpFile);
			} catch(Exception ignored) {				
			}
		}
		
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream(10000);
			controlMarshaller.setProperty("jaxb.schemaLocation",
				"http://env.dev.eid.portugal.be/eidtestinfra http://env.dev.eid.portugal.be/schemas/control.xsd");
			controlMarshaller.marshal(activeControl, baos);

			byte[] contents = replaceXmlElement(baos.toByteArray(), "control",
				"control xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:be:fedict:eid:dev:control:1.0 http://env.dev.eid.portugal.be/schemas/control.xsd\" xmlns=\"urn:be:fedict:eid:dev:control:1.0\"");

			FileOutputStream fos = new FileOutputStream(controlFile);
			fos.write(contents);
			fos.close();

			if (Log.logger != null)
				Log.logger.debug("Saved file " + controlFile.getAbsolutePath());
		} catch(JAXBException jaxbe) {
			if (Log.logger != null)
				Log.logger.error("", jaxbe);
			//copy back
			if(tmpFile.exists()) {
				try {
					Toolkit.copyFile(tmpFile, controlFile);
				} catch(Exception ignored) {					
				}
			}
			//cannot happen unless wrong schema file inside jar file
			//or cannot write, therefore throw IOException
			throw new IOException(jaxbe.getLinkedException().getMessage());
		}
		
		originalControl = copyOf(activeControl);
	}

	/**
	 * 
	 * @param val
	 */
	public void setVisibility(String val) {
		Visibility v = Visibility.get(val);
		if (v == null) {
			throw new IllegalArgumentException("\"" + val
					+ " not allowed. Value must be one of ["
					+ Visibility.HIDE_REAL.getValue() + ", "
					+ Visibility.REAL_FIRST.getValue() + ", "
					+ Visibility.REAL_LAST.getValue() + ", "
					+ Visibility.HIDE_VIRTUAL.getValue() + "]");
		}
		setVisibility(v);
	}

	/**
	 * 
	 * @param v
	 */
	public void setVisibility(Visibility v) {
		activeControl.setShow(v.getValue());
	}
	
	/**
	 * 
	 * @return
	 */
	public Visibility getVisibility() {
		return Visibility.get(activeControl.getShow());
	}
	
	/**
	 * Search the give directories for files with .xml extension. All xml files are parsed
	 * but those that do not comply with the Card schema are discarded.
	 * @param dirs
	 * @return a list of ControlCardHolders of which one hard chip number occurs only once.
	 */
	public List<ControlCardHolder> getItems(String[] dirs) {		
		List<ControlCardHolder> ctcholders = new ArrayList<ControlCardHolder>();				
		FilenameFilter filter = new VirtualCardFileNameFilter();
		
		//////////////////////////////////////////////
		// Lookup all possible VirtualCards on disk //
		//////////////////////////////////////////////
		for (String dirStr : dirs) {
			File dir = new File(dirStr);
			if(dir.exists() && dir.canRead()) {
				String[] files = dir.list(filter);
				for (String s : files) {
					File f = new File(dir, s);
					Card card = null;
					try {
						byte[] contents = readFile(f);
						contents = replaceXmlElement(contents, "card",
							"card xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://env.dev.eid.portugal.be/eidtestinfra http://env.dev.eid.portugal.be/schemas/card.xsd\"");
						card = (Card) cardUnmarshaller.unmarshal(new ByteArrayInputStream(contents));
					} catch (IOException ignored) {						
						if (Log.logger != null)
							Log.logger.debug("Can't read \"" + f.getAbsolutePath() + "\": " + ignored.toString());
					} catch (JAXBException ignored) {						
						if (Log.logger != null)
							Log.logger.debug("Can't parse \"" + f.getAbsolutePath() + "\": " + ignored.toString());
					}
					if(card != null) {
						ControlCardHolder ctcholder = new ControlCardHolder(card.getPhysicalCardChipNumber());
						
						int index = Collections.binarySearch(ctcholders, ctcholder, ControlCardHolder.HARDCHIPNR_COMPARATOR);
						if (index < 0) { //does not exist
							ctcholders.add(-index - 1, ctcholder);						
						} else { //set the card on the existing instance
							ctcholder = ctcholders.get(index);
						}
	
						ctcholder.add(new CardHolder(card, f.getAbsolutePath()));					
					}
				}
			}
		}
		
		///////////////////////////////////////////////////////////////////////////
		// Add entries from Control file that are not in the include directories //
		///////////////////////////////////////////////////////////////////////////
		List<Control.Virtualcard> controlEntries = activeControl.getVirtualcard();
		for(Control.Virtualcard controlEntry : controlEntries) {
			ControlCardHolder ctcholder = new ControlCardHolder(controlEntry.hardchipnr);
			int index = Collections.binarySearch(ctcholders, ctcholder, ControlCardHolder.HARDCHIPNR_COMPARATOR);
			if (index < 0) { //does not exist
				ctcholders.add(-index - 1, ctcholder);						
			} else { //set the card on the existing instance
				ctcholder = ctcholders.get(index);
			}
			ctcholder.setFile(controlEntry.file);
			
			CardHolder ch = null;
			for(int i=0; i<ctcholder.size(); i++) {
				String file = ctcholder.get(i).getFile();
				boolean fileEquals = (Toolkit.isWindows ? file.equalsIgnoreCase(controlEntry.file)
							: file.equals(controlEntry.file)); 
				if(fileEquals) {
					ch = ctcholder.get(i);
					break;
				}
			}
			
			if(ch == null) {
				ctcholder.add(new CardHolder(null, controlEntry.file));				
			}			
		}
		
		return ctcholders;
	}
	
	/**
	 * 
	 * @param c1
	 * @param c2
	 * @return
	 */
	private static boolean controlEquals(Control c1, Control c2) {
		if (c1 == c2)
			return true;
		if (!c1.show.equals(c2.show))
			return false;
		if (c1.virtualcard == null && c2.virtualcard != null)
			return false;
		if (c1.virtualcard != null && c2.virtualcard == null)
			return false;
		if (c1.virtualcard != null && c2.virtualcard != null) {
			if(c1.virtualcard.size() != c2.virtualcard.size())
				return false;
			for(int i=0; i<c1.virtualcard.size(); i++) {
				Control.Virtualcard vc1 = c1.virtualcard.get(i);
				Control.Virtualcard vc2 = c2.virtualcard.get(i);
				if(!vc1.file.equals(vc2.file))
					return false;
				if(!vc1.hardchipnr.equals(vc2.hardchipnr))
					return false;
			}
		}
		return true;
	}
	
	/**
	 * 
	 * @param c
	 * @return
	 */
	private static Control copyOf(Control c) {
		Control c2 = new Control();
		c2.show = c.show;
		if(c.virtualcard != null) {
			c2.virtualcard = new ArrayList<Control.Virtualcard>(c.virtualcard.size());			
			for(Control.Virtualcard vc : c.virtualcard) {
				Control.Virtualcard vc2 = new Control.Virtualcard();
				vc2.file = vc.file;
				vc2.hardchipnr = vc.hardchipnr;
				c2.virtualcard.add(vc2);
			}
		}
		return c2;
	}
	
	/**
	 * Replace current items on the control file with the given items. Changes
	 * will be finalized on calling the save method.
	 * @param control
	 * @param items
	 */
	private static void saveToControl(Control control, List<ControlCardHolder> items) {
		if(items.size() == 0) {
			control.getVirtualcard().clear();
		} else {
			for(ControlCardHolder saveItem : items) {
				boolean exist = false;
				Iterator<Control.Virtualcard> controlEntryIter = control.getVirtualcard().iterator();
				while(controlEntryIter.hasNext()) {
					Control.Virtualcard controlEntry = controlEntryIter.next();
					if(controlEntry.hardchipnr.equals(saveItem.getHardchipnr())) {
						exist = true;
						if(saveItem.getFile() == null) {
							controlEntryIter.remove();
						} else if(!controlEntry.file.equals(saveItem.getFile())) {
							controlEntry.file = saveItem.getFile();						
						}
						break;
					}
				}
				if(!exist && saveItem.getFile() != null) {
					Control.Virtualcard vc = new Control.Virtualcard();
					vc.file = saveItem.getFile();
					vc.hardchipnr = saveItem.getHardchipnr();
					control.getVirtualcard().add(vc);
				}
			}
		}
	}
	
	/**
	 * 
	 * @author Rogier Taal
	 *
	 */
	private static class VirtualCardFileNameFilter implements FilenameFilter {
		public boolean accept(File arg0, String arg1) {
			File f = new File(arg0, arg1);
			return f.isFile()
					&& !f.isHidden()
					&& f.canRead()
					&& arg1.endsWith(".xml");
		}
	}
}
