package eu.europa.ec.markt.dss.commons.swing.mvc.applet;

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
 * @param <C>
 * @param <M>
 */
public abstract class AppletController<C extends ApplicationCore, M> {

    protected static final Logger LOG = Logger.getLogger(AppletController.class.getName());

    private final C core;

    private final M model;

    /**
     * 
     * The default constructor for AppletController.
     * 
     * @param core
     * @param model
     */
    protected AppletController(final C core, final M model) {
        this.core = core;
        this.model = model;
    }

    /**
     * 
     * @return
     */
    public C getCore() {
        return this.core;
    }

    /**
     * 
     * @return
     */
    protected M getModel() {
        return this.model;
    }

}
