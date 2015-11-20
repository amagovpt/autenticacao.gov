package eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard;

import com.jgoodies.binding.beans.Model;
import eu.europa.ec.markt.dss.applet.controller.DSSAppletController;
import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.applet.view.WaitingGlassPanel;

import javax.swing.*;
import java.util.Map;
import java.util.logging.Level;

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
public abstract class WizardController<M extends Model> extends DSSAppletController<M> {

    private final Map<Class<? extends WizardStep<M, ? extends WizardController<M>>>, ? extends WizardStep<M, ? extends WizardController<M>>> wizardSteps;

    private WizardStep<M, ? extends WizardController<M>> currentStep;

    private WizardView<M, ? extends WizardController<M>> currentView;

    private final int maxStep;

    /**
     * 
     * The default constructor for WizardController.
     * 
     * @param core
     * @param model
     */
    protected WizardController(final DSSAppletCore core, final M model) {
        super(core, model);

        registerViews();
        wizardSteps = registerWizardStep();
        if (wizardSteps.size() == 0) {
            throw new RuntimeException("Please register at least one step");
        }

        int max = 0;

        for (final WizardStep<M, ? extends WizardController<M>> step : wizardSteps.values()) {
            max = step.getStepProgression() > max ? step.getStepProgression() : max;
        }

        maxStep = max;

    }

    /**
     * 
     * @throws Exception
     */
    public void doBack() {
        if (!hasBack()) {
            return;
        }
        final WizardStep<M, ? extends WizardController<M>> step = wizardSteps.get(this.currentStep.getBackStep());
        execute(step);
    }

    /**
     * 
     */
    protected abstract void doCancel();

    /**
     * 
     * @throws Exception
     */
    public void doNext() {

        if (!hasNext()) {
            return;
        }
        
        final WizardStep<M, ? extends WizardController<M>> currentStep = this.currentStep;
        final WizardStep<M, ? extends WizardController<M>> nextStep = wizardSteps.get(this.currentStep.getNextStep());

        final SwingWorker<Object, Object> worker = new SwingWorker<Object, Object>() {
            /*
             * (non-Javadoc)
             * 
             * @see javax.swing.SwingWorker#doInBackground()
             */
            @Override
            protected Object doInBackground() throws Exception {
                try {

                    // Finish current wizard step
                    LOG.log(Level.INFO, "Finish wizard step {0}", new Object[] { currentStep });
                    final JPanel glassPanel = new WaitingGlassPanel();
                    getCore().setGlassPane(glassPanel);
                    getCore().getGlassPane().setVisible(true);
                    currentStep.finish();

                    // Init the next wizard step
                    LOG.log(Level.INFO, "Init wizard step {0}", new Object[] { nextStep });
                    setCurrentWizardStep(nextStep);
                    currentView = nextStep.getView();
                    nextStep.init();
                    currentView.show();
                    getCore().getGlassPane().setVisible(false);

                    return null;
                } catch (final Exception e) {
                    LOG.log(Level.SEVERE, "Execute fail", e);
                    JOptionPane.showMessageDialog(getCore(), e);
                    doBack();
                    return null;
                }
            }

        };

        worker.execute();

    }

    /**
     * 
     * @return
     */
    protected abstract Class<? extends WizardStep<M, ? extends WizardController<M>>> doStart();

    /**
     * 
     * @param wizardStep
     * @throws Exception
     */
    private void execute(final WizardStep<M, ? extends WizardController<M>> wizardStep) {

        final SwingWorker<Object, Object> worker = new SwingWorker<Object, Object>() {
            /*
             * (non-Javadoc)
             * 
             * @see javax.swing.SwingWorker#doInBackground()
             */
            @Override
            protected Object doInBackground() throws Exception {
                try {
                    LOG.log(Level.INFO, "Execute step {0}", new Object[] { wizardStep });
                    final JPanel glassPanel = new WaitingGlassPanel();
                    getCore().setGlassPane(glassPanel);
                    getCore().getGlassPane().setVisible(true);
                    setCurrentWizardStep(wizardStep);
                    currentView = wizardStep.getView();
                    wizardStep.init();
                    currentView.show();
                    getCore().getGlassPane().setVisible(false);
                    return null;
                } catch (final Exception e) {
                    LOG.log(Level.SEVERE, "Execute fail", e);
                    JOptionPane.showMessageDialog(getCore(), e);
                    doBack();
                    return null;
                }
            }

        };

        worker.execute();
    }

    /**
     * 
     * @return
     */
    public int getStepNumber() {
        return this.currentStep.getStepProgression();
    }

    /**
     * 
     * @return
     */
    public int getStepTotals() {
        return maxStep;
    }

    /**
     * 
     * @return
     */
    public boolean hasBack() {
        final Class<?> stepClass = this.currentStep.getBackStep();
        return stepClass != null && wizardSteps.containsKey(stepClass);
    }

    /**
     * 
     * @return
     */
    public boolean hasNext() {
        final Class<?> stepClass = this.currentStep.getNextStep();
        return stepClass != null && wizardSteps.containsKey(stepClass);
    }

    /**
     * 
     * @return
     */
    public boolean isLast() {
        return (getStepTotals() - 1) == getStepNumber();
    }

    /**
     * 
     * @return
     */
    public boolean isValid() {
        return this.currentStep.isValid();
    }

    protected abstract void registerViews();

    /**
     * 
     * @return
     */
    protected abstract Map<Class<? extends WizardStep<M, ? extends WizardController<M>>>, ? extends WizardStep<M, ? extends WizardController<M>>> registerWizardStep();

    /**
     * 
     * @param wizardStep
     */
    private void setCurrentWizardStep(final WizardStep<M, ? extends WizardController<M>> wizardStep) {
        this.currentStep = wizardStep;
    }

    /**
     * /**
     * 
     * @throws Exception
     */
    public void start() {
        final WizardStep<M, ? extends WizardController<M>> step = wizardSteps.get(doStart());
        execute(step);
    }

}
