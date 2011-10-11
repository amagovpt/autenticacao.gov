package be.eid.eidtestinfra.pcsccontrol.gui;

import java.util.StringTokenizer;
import be.eid.eidtestinfra.pcsccontrol.Log;

/**
 * This factory creates a CardReaderMonitor based on the <i>smartcardio</i> library when
 * the Java version is 1.6 or higher<br>-or-<br>creates a CardReaderMonitor based on the <i>jpcsc</i>
 * library when the Java version is 1.5 and the jpcsc classes can be found on the classpath (jpcsc.jar
 * is on the classpath).
 * 
 * @author Rogier Taal
 * 
 */
public class CardReaderMonitorFactory {
	
	//made private because this class only contains static methods
	private CardReaderMonitorFactory() {		
	}
	
	public static CardReaderMonitor createReaderMonitor() {
		CardReaderMonitor crm = null;
		if(isJava_6_OrHigher()) {
			System.out.println("Using pcsc library: \"javax.smartcardio\"");
			if (Log.logger != null)
				Log.logger.info("Using pcsc library: \"javax.smartcardio\"");
			crm = new CardReaderMonitorImpl(); 
		} else if(be.eid.eidtestinfra.pcsccontrol.gui.jpcsc.CardReaderMonitorImpl.jpcscEnabled()) {
			System.out.println("Using pcsc library: \"com.linuxnet.jpcsc\"");
			if (Log.logger != null)
				Log.logger.info("Using pcsc library: \"com.linuxnet.jpcsc\"");
			crm = new be.eid.eidtestinfra.pcsccontrol.gui.jpcsc.CardReaderMonitorImpl();
		}
		return crm;
	}
	
	public static boolean isJava_6_OrHigher() {
		String javaVersion = System.getProperty("java.version");
		StringTokenizer st = new StringTokenizer(javaVersion, ".");
		int majorVersion = Integer.parseInt(st.nextToken());
		int minorVersion = Integer.parseInt(st.nextToken());		
		return (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 6));
	}
}
