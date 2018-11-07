package eu.europa.ec.markt.dss.applet.controller;

import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.applet.model.ActivityModel;
import eu.europa.ec.markt.dss.applet.model.ActivityModel.ActivityAction;
import eu.europa.ec.markt.dss.applet.view.ActivityView;
import eu.europa.ec.markt.dss.applet.wizard.validation.ValidationWizardController;


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
public class ActivityController extends DSSAppletController<ActivityModel> {

    private ActivityView view;

    /**
     * 
     * The default constructor for ActivityController.
     * 
     * @param core
     * @param model
     */
    public ActivityController(final DSSAppletCore core, final ActivityModel model) {
        super(core, model);
        view = new ActivityView(getCore(), this, getModel());
    }

    /**
     * 
     */
    public void display() {
        //view.show();
        startValidationAction();
    }

    /**
     * 
     */
    public void startValidationAction() {

    	getCore().getController(ValidationWizardController.class).start();

    }
}
