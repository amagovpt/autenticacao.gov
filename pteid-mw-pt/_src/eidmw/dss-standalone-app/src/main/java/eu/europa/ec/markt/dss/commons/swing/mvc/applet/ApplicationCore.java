package eu.europa.ec.markt.dss.commons.swing.mvc.applet;

import javax.swing.*;

import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.Globals;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.logging.Logger;

//OSX Integration
import com.apple.eawt.AppEvent.OpenFilesEvent;
import com.apple.eawt.Application;
import com.apple.eawt.OpenFilesHandler;

/**
 *
 *
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 */
public abstract class ApplicationCore extends JFrame implements OpenFilesHandler {

    private static final long serialVersionUID = 6721104284268815739L;
    
    private static String signedFile = null;

    protected static final Logger LOG = Logger.getLogger(ApplicationCore.class.getName());

    private final Map<Class<? extends AppletController>, AppletController> controllers = new HashMap<Class<? extends AppletController>, AppletController>();

    /**
     *
     * @param controllerClass
     * @return a controller
     */
    @SuppressWarnings("unchecked")
    public <C extends AppletController> C getController(final Class<C> controllerClass) {
        if (!controllers.containsKey(controllerClass)) {
            throw new RuntimeException("The class controller " + controllerClass.getName() + " cannot be find , please register it");
        }
        return (C) controllers.get(controllerClass);
    }

    /**
     * @return the controllers
     */
    public Map<Class<? extends AppletController>, AppletController> getControllers() {
        return controllers;
    }
    
    public interface ParameterProvider {
        public String getParameter(String parameterName);
    }
    
    public static void showLanguageDialog()
    {
    	
    }
    
    public void openFiles(OpenFilesEvent event) {
        System.err.println("Received Apple event openFiles()" + ((File)event.getFiles().get(0)).getAbsolutePath());
        signedFile = ((File)event.getFiles().get(0)).getAbsolutePath();
    }
    
    private static void loadLookAndFeel()
    {
    	String os = System.getProperty("os.name").toLowerCase();
    	try {

    		if (os.equals("windows"))
    		{
    			UIManager.setLookAndFeel(
    					UIManager.getSystemLookAndFeelClassName());

    		}

    		else
    		{

    			for (final UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
    					if (info.getName().equals("Nimbus")) {
    						UIManager.setLookAndFeel(info.getClassName());
    						//				    SwingUtilities.updateComponentTreeUI(this);
    					}
    			}
    		}

    	} catch (final Exception exception) {
    		LOG.warning("Look and feel Nimbus cannot be installed");
    	}
    }
    
    static int wait_count = 0;

    public static void main(String[] args) throws InterruptedException {
    	
    	//Not using this app as a handler for XAdES/ASIC containers for now...
    	if (args.length > 1)
    		signedFile = args[1];
    	
    	loadLookAndFeel();
	    
    	String language = "en";
    	
	    if (args.length > 0) {
	       language = args[0];
	    }
	    
	    Globals.getInstance().setLocale(language);
	    
	    DSSAppletCore core_app = new DSSAppletCore();	
	    
		AppParameterProvider parameterProvider = new AppParameterProvider();
	    
		if (System.getProperty("os.name").toLowerCase().contains("mac"))
		{
			Application.getApplication().setOpenFileHandler(core_app);

			//core_app.registerParameters(parameterProvider, signedFile);

			while(signedFile == null && wait_count < 3)
			{
				Thread.sleep(100);
				wait_count++;
			}
		}
	    
	    core_app.registerParameters(parameterProvider, signedFile);
	    core_app.registerControllers();
	    core_app.setTitle(ResourceUtils.getI18n("APPLICATION_TITLE"));
	    core_app.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	    core_app.setIconImage(ComponentFactory.createWindowIcon());
	    core_app.executeLayout();
	    
	    core_app.pack();
	    core_app.setSize(800,600);
	    core_app.setResizable(false);
	    core_app.setLocationRelativeTo(null);
	    
	    core_app.setVisible(true);
    }

    public abstract void executeLayout();

    protected abstract void registerControllers();
    

    private static class AppParameterProvider implements ParameterProvider {
    	
    	Map<String, String> actual_parameters = new HashMap<String, String>();
    	
        public AppParameterProvider() {
        	Properties props = new Properties();
        	try {
        		File config_file = new File(ApplicationCore.class.getProtectionDomain().getCodeSource().getLocation().toURI());
				props.load(new FileReader(config_file.getParent() + "/config.properties"));
				
			} catch (IOException e) {
				e.printStackTrace();
			} catch (URISyntaxException e) {
				e.printStackTrace();
			}
        	actual_parameters.put("service_url", props.getProperty("service_url"));
            
        }

        public String getParameter(String parameterName) {
            String p = actual_parameters.get(parameterName);
            return p != null ? p : ""; 
        }

    }

}
