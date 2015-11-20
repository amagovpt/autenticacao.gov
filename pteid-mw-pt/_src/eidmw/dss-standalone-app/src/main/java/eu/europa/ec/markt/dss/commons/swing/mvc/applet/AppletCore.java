package eu.europa.ec.markt.dss.commons.swing.mvc.applet;

import javax.swing.*;
import java.applet.Applet;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;

/**
 *
 * TODO
 *
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 */
public abstract class AppletCore extends JApplet {

    private static final long serialVersionUID = 6721104284268815739L;

    protected static final Logger LOG = Logger.getLogger(AppletCore.class.getName());

    private final Map<Class<? extends AppletController>, AppletController> controllers = new HashMap<Class<? extends AppletController>, AppletController>();

    private ParameterProvider parameterProvider;

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

    /*
     * (non-Javadoc)
     * 
     * @see java.applet.Applet#init()
     */
    @Override
    public void init() {
	    super.init();

	    try {
		    for (final UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
			    if (info.getName().equals("Nimbus")) {
				    UIManager.setLookAndFeel(info.getClassName());
				    SwingUtilities.updateComponentTreeUI(this);
			    }
		    }

	    } catch (final Exception exception) {
		    LOG.warning("Look and feel Nimbus cannot be installed");
	    }


	    if (parameterProvider == null) {
		    parameterProvider = new AppletParameterProvider(this);
	    }
	    registerParameters(parameterProvider);
	    registerControllers();
	    layout(this);
    }

    public void setParameterProvider(ParameterProvider parameterProvider) {
        this.parameterProvider = parameterProvider;
    }

    protected abstract void layout(final AppletCore core);

    protected abstract void registerControllers();

    protected abstract void registerParameters(ParameterProvider parameterProvider);

    public interface ParameterProvider {
        public String getParameter(String parameterName);
    }

    private static class AppletParameterProvider implements ParameterProvider {

        private final Applet applet;

        public AppletParameterProvider(Applet applet) {
            this.applet = applet;
        }

        public String getParameter(String parameterName) {
            return applet.getParameter(parameterName);
        }

    }

}
