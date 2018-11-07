package eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard;

import com.jgoodies.binding.beans.Model;

import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;
import eu.europa.ec.markt.dss.applet.view.DSSAppletView;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;

import javax.swing.*;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;

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

public abstract class WizardView<M extends Model, C extends WizardController<M>> extends DSSAppletView<M, C> {

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
    private class BackActionListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(final ActionEvent event) {
            getController().doBack();
        }

    }

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
    private class CancelActionListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(final ActionEvent event) {
            getController().doCancel();
        }
    }

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
    private class NextActionListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(final ActionEvent event) {
            getController().doNext();
        }

    }

    private final JButton backButton;
    private final JButton nextButton;
//    private final JButton cancelButton;

    /**
     * 
     * The default constructor for WizardView.
     * 
     * @param core
     * @param controller
     * @param model
     */
    public WizardView(final ApplicationCore core, final C controller, final M model) {
        super(core, controller, model);

        backButton = ComponentFactory.createBackButton(false, new BackActionListener());
        backButton.setName("back");
        nextButton = ComponentFactory.createNextButton(false, new NextActionListener());
        nextButton.setName("next");
//        cancelButton = ComponentFactory.createCancelButton(true, new CancelActionListener());
//        cancelButton.setName("cancel");
    }

    private JPanel doActionLayout() {
        if (getController().hasNext()) {
            return ComponentFactory.actionPanel(backButton, nextButton);
        } else {
            return ComponentFactory.actionPanel(backButton);
        }

    }

    private JPanel doStepLayout() {
        final int currentStep = getController().getStepNumber();
        final int maxStep = getController().getStepTotals();
        return ComponentFactory.createWizardStepPanel(currentStep, maxStep);

    }

    private void doWizardInit() {
        final boolean backEnabled = getController().hasBack();
        final boolean nextEnabled = getController().hasNext() && getController().isValid();
        backButton.setEnabled(backEnabled);
        nextButton.setEnabled(nextEnabled);

        if (getController().isLast()) {
            nextButton.setText(ResourceUtils.getI18n("VALIDATE"));
        } else {
            nextButton.setText(ResourceUtils.getI18n("NEXT"));
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#layout()
     */
    @Override
    protected Container layout() {

        doWizardInit();

        final JPanel panel = ComponentFactory.createPanel(new BorderLayout());
//        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

        panel.add(super.layout(), BorderLayout.CENTER);
        panel.add(doActionLayout(), BorderLayout.SOUTH);

        return panel;
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#modelChange(java.beans.PropertyChangeEvent)
     */
    @Override
    public void modelChanged(final PropertyChangeEvent evt) {

        final boolean enabled = getController().hasNext() && getController().isValid();

        this.nextButton.setEnabled(enabled);

        wizardModelChange(evt);
    }

    public void wizardModelChange(final PropertyChangeEvent evt) {
    };

}
