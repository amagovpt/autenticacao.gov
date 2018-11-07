package eu.europa.ec.markt.dss.commons.swing.mvc.applet;

import java.awt.Container;
import java.util.logging.Logger;

import eu.europa.ec.markt.dss.applet.util.ComponentFactory;

/**
 * 
 * TODO
 * 
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 * 
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @param <M> model
 * @param <C> controller
 * @param <JC> Component
 */
public abstract class AppletView<M, C extends AppletController<? extends ApplicationCore, M>> {

    protected static final Logger LOG = Logger.getLogger(AppletView.class.getName());

    private final ApplicationCore core;
    private final M model;
    private final C controller;

    /**
     * 
     * The default constructor for AbstractView.
     * 
     * @param core
     * @param controller
     * @param model
     */
    public AppletView(final ApplicationCore core, final C controller, final M model) {
        this.core = core;
        this.controller = controller;
        this.model = model;
    }

    /**
     * @return the controller
     */
    public C getController() {
        return controller;
    }

    /**
     * 
     * @return
     */
    protected ApplicationCore getCore() {
        return core;
    }

    /**
     * @return the model
     */
    public M getModel() {
        return model;
    }

    /**
     * 
     * @return
     */
    protected abstract Container layout();

    /**
     * 
     */
    public void show() {

        final Container content = layout();
        ComponentFactory.updateDisplay(getCore(), content);
    }

}
