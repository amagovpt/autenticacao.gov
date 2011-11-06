package be.eid.eidtestinfra.pcsccontrol;

import java.io.ByteArrayInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.jar.Attributes;
import java.util.jar.Manifest;

import javax.swing.UIManager;
import javax.xml.bind.JAXBException;

import be.eid.eidtestinfra.pcsccontrol.gui.Toolkit;

/**
 * This class does the command line functionality.
 * 
 * @author Rogier Taal
 * 
 */
public class Main {

	private ControlModel cm;
	private PropertyModel pm;
	private boolean verbose;

	public static void main(String args[]) {		
		try {
			doMain(args);
		}
		catch (Throwable t) {
			if (Log.logger != null)
				Log.logger.error("", t);
			t.printStackTrace();
		}
	}

	public static void doMain(String args[]) {		
		{
			List<String> argsL = Arrays.asList(args);
			if (argsL.contains("-h")) {
				showUsageAndExit();
			}			
			if (argsL.contains("-version")) {
				showVersionAndExit();
			}
		}
		
		// search for verbose
		int vIndex;
		for(vIndex=0; vIndex<args.length; vIndex++) {
			if(args[vIndex].equals("-verbose")) {
				break;
			}
		}
		boolean verbose = (vIndex < args.length);
		if (verbose) { //remove -v from argument list
			String[] tmp = new String[args.length-1];
			System.arraycopy(args, 0, tmp, 0, vIndex);
			System.arraycopy(args, vIndex+1, tmp, vIndex, tmp.length-vIndex);
			args = tmp;
		}
		
		Main m = new Main(verbose);
		if(args.length == 0) {
			m.guiInit();
			return;
		}
		
		try {
			m.cmdInit();
		} catch (Exception e) {
			showExceptionAndExit(e);
		}
		
		if (args[0].equalsIgnoreCase("-showvisb")) { // -showvisb
			if (args.length != 1)
				showUsageAndExit();
			m.showVisb();
		} else if(args[0].equalsIgnoreCase("-showControlFile")) { // -showControlFile
			if (args.length != 1)
				showUsageAndExit();
			m.showControlFile();			
/*
STH: no use in selecting another control file

		} else if (args[0].equalsIgnoreCase("-setControlFile")) { // -setControlFile <path>
			if (args.length != 2)
				showUsageAndExit();
			m.setControlFile(args);
*/
		} else if (args[0].equalsIgnoreCase("-setvisb")) { // -setvisb <mode> (HIDE_REAL, HIDE_VIRTUAL, REAL_FIRST, REAL_LAST)
			if (args.length != 2)
				showUsageAndExit();
			m.setVisb(args);
		} else if (args[0].equalsIgnoreCase("-adddir")) { // -adddir <path>
			if (args.length != 2)
				showUsageAndExit();
			m.addDir(args);
		} else if (args[0].equalsIgnoreCase("-listdirs")) { // -listdir
			if (args.length != 1)
				showUsageAndExit();
			m.listDirs();
		} else if (args[0].equalsIgnoreCase("-listchipnrs")) { // -listchipnrs
			if (args.length != 1)
				showUsageAndExit();
			m.listChipNrs();
		} else if (args[0].equalsIgnoreCase("-listcards")) { // -listcards <chipnr> (physical chipnr)
			if (args.length != 2)
				showUsageAndExit();
			m.listCards(args);
		} else if (args[0].equalsIgnoreCase("-deldir")) { // -deldir <path>
			if (args.length != 2)
				showUsageAndExit();
			m.delDir(args);
		} else if (args[0].equalsIgnoreCase("-setcard")) { // -setCard <physical chipnr> <path>
			if (args.length != 3)
				showUsageAndExit();
			m.setCard(args, true);
		} else if (args[0].equalsIgnoreCase("-unsetcard")) { // -unSetCard <physical chipnr>
			if (args.length != 2)
				showUsageAndExit();
			m.setCard(args, false);
		} else if (args[0].equalsIgnoreCase("-unsetall")) { // -unSetAll
			if (args.length != 1)
				showUsageAndExit();
			m.setCard(null, false);
		} else if (args[0].equalsIgnoreCase("-showCard")) { // -showCard <physical chipnr>
			if (args.length != 2)
				showUsageAndExit();
			m.showCard(args);
		}

		showUsageAndExit();
	}
	
	public Main(boolean verbose) {
		this.verbose = verbose;
	}

	private static void showUsageAndExit() {
		System.err.println("Usage: pcsccontrol <options>"
						+ "\n       pcsccontrol [no options starts graphical mode]"
						+ "\n\nOptions:"
/*
STH: no use in selecting another control file
						+ "\n    Set control file path            : -setControlFile <path>"
*/
						+ "\n    Show control file path           : -showControlFile"
						+ "\n    Set visibility                   : -setVisb <mode>"
						+ "\n      (mode is one of: [HIDE_REAL, HIDE_VIRTUAL, REAL_FIRST, REAL_LAST])"
						+ "\n    Show visibility                  : -showVisb"
						+ "\n    List available virtual card dirs : -listDirs"
						+ "\n    Add virtual card dir             : -addDir <path>"
						+ "\n    Delete virtual card dir          : -delDir <path>"
						+ "\n    List available virtual cards     : -listCards <physical chipnr>"
						+ "\n    List available physical chip nrs : -listChipnrs"
						+ "\n    Set virtual card path            : -setCard <physical chipnr> <path>"
						+ "\n    Unset virtual card path          : -unSetCard <physical chipnr>"
						+ "\n    Unset all virtual card paths     : -unSetAll"
						+ "\n    Show active virtual card         : -showCard <physical chipnr>"
						+ "\n    Verbose (only text mode)         : -verbose"
						+ "\n    Show version and exit            : -version"
						+ "\n    Show this help                   : -h"
						+ "\n");
		System.exit(1);
	}
	
	private void setControlFile(String[] args) {
		pm.setControlFile(args[1]);
		saveAndExit();
	}
	
	private void showControlFile() {
		System.out.println(pm.getControlFile());
		System.exit(0);
	}
	
	private void setVisb(String[] args) {
		if (!ControlModel.VISB_MODES.contains(args[1])) {
			showUsageAndExit();
		}
		cm.setVisibility(args[1]);			
		saveAndExit();
	}
	
	private void showVisb() {
		System.out.println(cm.getVisibility().getValue());
		System.exit(0);
	}
	
	private void listDirs() {
		String[] dirs = pm.getIncludeDirs();
		System.out.println(listStrings(dirs, "\n"));
		System.exit(0);
	}
	
	private void addDir(String[] args) {
		pm.addIncludeDir(args[1]);
		saveAndExit();
	}
	
	private void listChipNrs() {
		String[] dirs = pm.getIncludeDirs();
		List<ControlCardHolder> ctcholders = cm.getItems(dirs);
		System.out.println(listPhysicalChipNrs(ctcholders, "\n"));
		System.exit(0);
	}
	
	private void listCards(String[] args) {
		String[] dirs = pm.getIncludeDirs();
		List<ControlCardHolder> ctcholders = cm.getItems(dirs);
		System.out.println(listFileNames(ctcholders, args[1], "\n"));
		System.exit(0);
	}
	
	private void delDir(String[] args) {
		pm.removeIncludeDir(args[1]);
		saveAndExit();
	}
	
	private void setCard(String[] args, boolean set) {
		if(set && args[2].trim().length() == 0) {
			System.err.println("Invalid path given");
			System.exit(1);
		}
		if(args == null) { //unsetAll
			List<ControlCardHolder> items = Collections.emptyList(); 
			cm.replaceItems(items);
			saveAndExit();
		}
		String[] dirs = pm.getIncludeDirs();
		List<ControlCardHolder> items = cm.getItems(dirs);
		for(ControlCardHolder cch : items) {
			if(cch.getHardchipnr().equals(args[1])) {
				cch.setFile((set ? args[2] : null));
				cm.replaceItems(items);
				saveAndExit();			
			}
		}
		System.out.println("No match found for chip number: "+args[1]);
		System.exit(1);
	}
	
	private void showCard(String[] args) {
		String[] dirs = pm.getIncludeDirs();
		List<ControlCardHolder> items = cm.getItems(dirs);
		for(ControlCardHolder cch : items) {
			if(cch.getHardchipnr().equals(args[1])) {
				if(cch.getFile() != null) {
					System.out.println(cch.getFile());
				}
				break;
			}
		}
		System.exit(0);
	}

	private static void showExceptionAndExit(Exception e) {
		if (Log.logger != null)
			Log.logger.error("", e);
		System.err.println(e.getMessage());
		System.exit(1);
	}
	
	private void saveAndExit() {
		if(cm.getFilePath() == null || cm.isDirty()) {
			try {
				if(cm.getFilePath() == null) {
					throw new FileNotFoundException("Control file is not set");
				}
				cm.save();
			} catch (Exception e) {
				showExceptionAndExit(e);
			}
		}
		if(pm.getFilePath() == null || pm.isDirty()) {
			try {
				pm.save();
			} catch (Exception e) {
				showExceptionAndExit(e);
			}
		}
		System.exit(0);
	}
	
	private void cmdInit() throws JAXBException, IOException {
		pm = new PropertyModel();
		pm.reload();
		cm = new ControlModel(pm.getControlFile());
		cm.reload();
		
		if(verbose) {
			System.out.println("Using properties file: "
					+ (pm.getFilePath() == null ? "" : pm.getFilePath()));
			System.out.println("Using control file: "
					+ (cm.getFilePath() == null ? "" : cm.getFilePath()));
		}
	}
	
	private void guiInit() {
		Manifest manifest = null;
		try {
			manifest = getManifest();
		} catch (Exception e) {
			if (Log.logger != null)
				Log.logger.error("", e);
			Toolkit.showErrorDialog(null, e);
			System.exit(1);
		}

		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {			
		}

		String osName = System.getProperty("os.name"); 
	    if(osName.equals("Windows XP")||osName.equals("Windows 2000")) {
	    	Toolkit.fixWindowsClassicLAF();
	    }
		
		new be.eid.eidtestinfra.pcsccontrol.gui.Main(manifest);
	}
	
	private static void showVersionAndExit() {
		try {
			Manifest m = getManifest();
			String version = m.getMainAttributes().getValue(Attributes.Name.IMPLEMENTATION_VERSION);
			System.out.println(version);
			System.exit(0);
			return;
		} catch (IOException ioe) {
			showExceptionAndExit(ioe);
		}
	}
	
	public static final String listStrings(String[] v, String sep) {
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < v.length; i++) {
			sb.append(v[i]);
			if (i + 1 < v.length)
				sb.append(sep);
		}
		return sb.toString();
	}
	
	public static final String listFileNames(List<ControlCardHolder> v, String hardChipnr, String sep) {
		ControlCardHolder inst = null;
		for(ControlCardHolder cch : v) {
			if(cch.getHardchipnr().equals(hardChipnr)) {
				inst = cch;
				break;
			}
		}
		
		if(inst == null) {
			return "";
		}
		
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < inst.size(); i++) {
			sb.append(inst.get(i).getFile());
			if (i + 1 < inst.size())
				sb.append(sep);
		}
		return sb.toString();
	}

	public static final String listPhysicalChipNrs(List<ControlCardHolder> v, String sep) {
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < v.size(); i++) {
			sb.append(v.get(i).getHardchipnr());
			if (i + 1 < v.size())
				sb.append(sep);
		}
		return sb.toString();
	}
	
	private static Manifest getManifest() throws IOException {
		String className = Main.class.getSimpleName();
		String classFileName = className + ".class";
		String pathToThisClass = Main.class.getResource(classFileName).toString();
		int mark = pathToThisClass.indexOf("!") ;
		String pathToManifest = pathToThisClass.toString().substring(0,mark+1) ;
		pathToManifest += "/META-INF/MANIFEST.MF" ;
		
		byte[] res =  new byte[0];
		InputStream is = new URL(pathToManifest).openStream();
		byte[] buf = new byte[400];
		int len = 0;
		int totalLen = 0;
		while( (len = is.read(buf)) != -1 ) {
			byte[] tmp = new byte[totalLen+len];
			System.arraycopy(res, 0, tmp, 0, totalLen);
			System.arraycopy(buf, 0, tmp, totalLen, len);
			totalLen += len;
			res = tmp;
		}				
		return new Manifest(new ByteArrayInputStream(res));
	}

}
