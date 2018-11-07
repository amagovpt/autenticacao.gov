package eu.europa.ec.markt.dss.applet.controller;

import com.jgoodies.binding.beans.Model;
import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardController;

/**
 * 
 * TODO
 * 
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 * 
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @param <M>
 */
public abstract class DSSWizardController<M extends Model> extends WizardController<M> {
    /**
     * 
     * The default constructor for DSSWizardController.
     * 
     * @param core
     * @param model
     */
    protected DSSWizardController(final DSSAppletCore core, final M model) {
        super(core, model);
    }

}
