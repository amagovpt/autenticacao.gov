package be.eid.eidtestinfra.pcsccontrol;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Properties;

import be.eid.eidtestinfra.pcsccontrol.gui.Toolkit;

/**
 * PropertyModel contains methods for loading and saving the {@link Properties} instance from and to
 * file. First an instance needs to be made with a file path then {@link #reload()} is called to
 * parse the properties file and create the PropertyModel instance.
 * 
 * @author Rogier Taal
 * 
 * */
public class PropertyModel {
	
	private File propFile;
	private Properties activeProps = new Properties();
	private Properties originalProps = new Properties();
	
	private static final DateFormat DATEFORMAT = new SimpleDateFormat("dd.MM.yyyy HH.mm.ss");
	
	public static final String DEFAULT_PROPFILENAME = ".eidtestinfa.dirs";
	
	/**
	 * 
	 * @param filePath a path to a properties file or null
	 */
	public PropertyModel() {
		String homedir = System.getProperty("user.home");
		File propsdir = new File(homedir);

		File tmp = null;
		if (File.pathSeparatorChar == ';') {
			// For Windows: use ~/ because ~/Application Data
			// doesn't seem to be accessible in Vista (??)
				tmp = propsdir;			
		}
		else {
			String osName = System.getProperty("os.name");
			if (osName.startsWith("Mac") || osName.startsWith("mac")) {
				// For Mac: use ~/Library/Preferences if it exists
				tmp = new File(propsdir, "Library/Preferences");
			}
			else {
				// For Linux: use ~/.conf if it exists
				tmp = new File(propsdir, ".conf");
			}
		}
		if (tmp.exists())
			propsdir = tmp;

		propFile = new File(propsdir, DEFAULT_PROPFILENAME);

		if (Log.logger != null)
			Log.logger.info("Per user config file: " + propFile.getAbsolutePath());
	}
	
	/**
	 * If the property file give in the constructor or in {@link #setControlFile(String)} exists
	 * then load the properties from it.
	 * @throws IOException
	 */
	public void reload() throws IOException {
		if(propFile.exists()) {
			activeProps.loadFromXML(new FileInputStream(this.propFile));
		}
		originalProps.putAll(activeProps);
		
		if(getControlFile() == null) {
			setControlFile(ControlModel.DEFAULT_FILE_PATH);
		}
	}
	
	/**
	 * 
	 * @return the absolute path of the property file
	 */
	public String getFilePath() {
		return propFile.getAbsolutePath();
	}
	
	/**
	 * 
	 * @return
	 */
	public Properties getProperties() {
		return activeProps;
	}
	
	/**
	 * This methods compares the changes made to the properties since the last reload or save
	 * was made.
	 * @return true when changes have been made and need to be saved
	 */
	public boolean isDirty() {
		return !activeProps.equals(originalProps);
	}
	
	/**
	 * 
	 * @throws IOException
	 */
	public synchronized void save() throws IOException {		
		File tmpFile = new File(propFile.getAbsolutePath()+"~");
		//backup control file
		if(propFile.exists()) {
			try {
				Toolkit.copyFile(propFile, tmpFile);
			} catch(Exception ignored) {				
			}
		}
		
		try {
			String comment = "Last saved by " + System.getProperty("user.name") + " on " + DATEFORMAT.format(new Date());
			activeProps.storeToXML(new FileOutputStream(propFile), comment);
		} catch(IOException ioe) {
			//copy back
			if(tmpFile.exists()) {
				try {
					Toolkit.copyFile(tmpFile, propFile);
				} catch(Exception ignored) {					
				}
			}
			throw ioe;
		}
		
		originalProps.clear();
		originalProps.putAll(activeProps);

		if (Log.logger != null)
			Log.logger.debug("Saved file " + propFile.getAbsolutePath());
	}
	
	/**
	 * 
	 * @return the control file path
	 */
	public String getControlFile() {
		return activeProps.getProperty("ControlFile");
	}
	
	/**
	 * Sets the control file path.
	 * @param file
	 */
	public void setControlFile(String file) {
		if(file == null) {
			activeProps.remove("ControlFile");
		} else {
			activeProps.setProperty("ControlFile", file);
		}
	}

	/**
	 * Add an include directory in which to look for Card xml files.
	 * @param dir
	 */
	public void addIncludeDir(String dir) {
		if(Toolkit.isWindows) {
			String[] currDirs = getIncludeDirs();
			for(String d1 : currDirs) {
				if(dir.equalsIgnoreCase(d1)) {
					return;
				}
			}
		}		
		int i = 1;
		for (;;) {
			String key = "VirtFileDir." + i++;
			String val = activeProps.getProperty(key);
			if (val == null) {
				activeProps.put(key, dir);
				return;
			}
		}
	}
	
	/**
	 * Set the include directories in which to look for Card xml files.
	 * @param dirs array of directory paths which replaces all directories
	 * present
	 * @return true when after this method the include directories where
	 * different before this method was called. False when nothing really
	 * changed.  
	 */
	public boolean replaceIncludeDirs(String[] dirs) {
		String[] dirsBefore = getIncludeDirs();
		
		//remove all dirs from properties
		Iterator<Object> iter = activeProps.keySet().iterator();
		while(iter.hasNext()) {
			Object key = iter.next();
			if(((String)key).startsWith("VirtFileDir")) {
				//workingProps.remove(key);
				iter.remove();
			}
		}
		List<String> dirL = new ArrayList<String>(dirs.length);
		//add dirs
		int i = 1;
		for (String dir : dirs) {
			String tmp = (Toolkit.isWindows ? dir.toLowerCase() : dir);
			if(!dirL.contains(tmp)) {
				dirL.add(tmp);
				String key = "VirtFileDir." + i++;
				activeProps.put(key, dir);
			}
		}
		
		String[] dirsAfter = getIncludeDirs();

		return !Toolkit.arrayEquals(dirsBefore, dirsAfter, Toolkit.isWindows);
	}

	/**
	 * Remove a directory from the list of include directories in which to look for
	 * Card xml files. 
	 * @param dir
	 */
	public void removeIncludeDir(String dir) {
		int i = 1;
		boolean removed = false;
		for (;;) {
			String key = "VirtFileDir." + i++;
			String val = activeProps.getProperty(key);
			if (val == null) {
				break;
			}
			boolean dirFound = (Toolkit.isWindows ? val.equalsIgnoreCase(dir) : val.equals(dir)); 
			if (dirFound) {
				activeProps.remove(key);
				removed = true;
				break;
			}
		}
		// re-order
		if (removed) {
			for (;;) {
				String key = "VirtFileDir." + i;
				String val = activeProps.getProperty(key);
				if (val != null) {
					activeProps.remove(key);
					key = key.substring(0, key.indexOf('.') + 1) + (i - 1);
					activeProps.put(key, val);
				}
				if (val == null) {
					break;
				}
				i++;
			}
		}
	}

	/**
	 * 
	 * @return the include directories in which to look for Card xml files.
	 */
	public String[] getIncludeDirs() {
		return getIncludeDirs(activeProps);
	}
	
	/**
	 * List all include directories contained by the given Properties file.
	 * @param p
	 * @return
	 */
	private static String[] getIncludeDirs(Properties p) {
		List<String> ret = new ArrayList<String>();
		List<String> dirL = new ArrayList<String>();

		int i = 1;
		for (;;) {
			String val = p.getProperty("VirtFileDir." + i++);
			if (val == null) {
				break;
			}
			String tmp = (Toolkit.isWindows ? val.toLowerCase() : val);
			if(!dirL.contains(tmp)) {
				dirL.add(tmp);
				ret.add(val);
			}
		}
		Collections.sort(ret);
		return ret.toArray(new String[0]);
	}
}
