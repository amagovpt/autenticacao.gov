package eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard;

import org.apache.commons.lang.builder.ReflectionToStringBuilder;

import com.jgoodies.binding.beans.Model;

import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ControllerException;

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
 * @param <C>
 */
public abstract class WizardStep<M extends Model, C extends WizardController<M>> {

    private final M model;
    private final WizardView<M, C> view;
    private final C controller;

    /**
     * 
     * The default constructor for DSSWizardStep.
     * 
     * @param model
     * @param view
     * @param controller
     */
    public WizardStep(final M model, final WizardView<M, C> view, final C controller) {
        this.model = model;
        this.view = view;
        this.controller = controller;
    }

    protected abstract void finish() throws ControllerException;

    /**
     * 
     * @return
     */
    protected abstract Class<? extends WizardStep<M, C>> getBackStep();

    /**
     * @return the controller
     */
    public C getController() {
        return controller;
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
    protected abstract Class<? extends WizardStep<M, C>> getNextStep();

    /**
     * 
     * @return
     */
    protected abstract int getStepProgression();

    /**
     * @return the view
     */
    public WizardView<M, C> getView() {
        return view;
    }

    /**
     * @throws Exception
     * 
     */
    protected abstract void init() throws ControllerException;

    /**
     * 
     * @return
     */
    protected abstract boolean isValid();

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return ReflectionToStringBuilder.reflectionToString(this);
    }

}
