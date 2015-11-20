package eu.europa.ec.markt.dss.applet.view;

import com.jgoodies.binding.PresentationModel;
import com.jgoodies.binding.value.ValueModel;
import com.jgoodies.forms.builder.PanelBuilder;
import com.jgoodies.forms.layout.CellConstraints;
import eu.europa.ec.markt.dss.applet.controller.ActivityController;
import eu.europa.ec.markt.dss.applet.main.Parameters.AppletUsage;
import eu.europa.ec.markt.dss.applet.model.ActivityModel;
import eu.europa.ec.markt.dss.applet.model.ActivityModel.ActivityAction;
import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

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
public class ActivityView extends DSSAppletView<ActivityModel, ActivityController> {

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
    private final class NextActionListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(ActionEvent e) {
            if (getModel().getAction() != null) {
                getController().startValidationAction();
            }
        }

    }
    private static final String I18N_VERIFY_DOCUMENT = ResourceUtils.getI18n("VERIFY_DOCUMENT_SIGNATURE");


//    private final JRadioButton choice1;
    private final JRadioButton choice2;

    private final JButton button;

    private final PresentationModel<ActivityModel> presentationModel;

    private final List<JRadioButton> choices = new ArrayList<JRadioButton>();

    /**
     * 
     * The default constructor for ActivityView.
     * 
     * @param core
     * @param controller
     * @param model
     */
    public ActivityView(final ApplicationCore core, final ActivityController controller, final ActivityModel model) {
        super(core, controller, model);

        this.presentationModel = new PresentationModel<ActivityModel>(getModel());
        final ValueModel activityValue = presentationModel.getModel(ActivityModel.PROPERTY_ACTIVITY);
        choice2 = ComponentFactory.createRadioButton(I18N_VERIFY_DOCUMENT, activityValue, ActivityAction.VERIFY);
        choice2.setName("validate");
        button = ComponentFactory.createNextButton(true, new NextActionListener());
        button.setName("next");

        choices.add(choice2);

     }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#doLayout()
     */
    @Override
    protected Container doLayout() {

        final JPanel panel = ComponentFactory.createPanel(new BorderLayout());

        final String[] colSpecs = new String[] { "5dlu", "pref", "5dlu", "pref:grow,5dlu" };
        final String[] rowSpecs = new String[] { "5dlu", "pref", "5dlu", "pref", "5dlu", "pref", "5dlu", "pref", "5dlu", "pref", "5dlu" };

        final PanelBuilder builder = ComponentFactory.createBuilder(colSpecs, rowSpecs);
        final CellConstraints cc = new CellConstraints();

        builder.addSeparator(ResourceUtils.getI18n("CHOOSE_AN_ACTIVITY"), cc.xyw(2, 2, 3));

        int i = 4;
        for (JRadioButton choice : choices) {
            builder.add(choice, cc.xy(2, i));
            i += 2;
        }

        panel.add(ComponentFactory.createPanel(builder), BorderLayout.CENTER);
        panel.add(ComponentFactory.actionPanel(button), BorderLayout.SOUTH);

        return panel;
    }
}
