package eu.europa.ec.markt.dss.applet.model;

import com.jgoodies.binding.beans.Model;

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
@SuppressWarnings("serial")
public class ActivityModel extends Model {
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
    public enum ActivityAction {
        VERIFY, EDIT_VALIDATION_POLICY, EXTEND
    }

    public static final String PROPERTY_ACTIVITY = "action";
    private ActivityAction action;

    /**
     * @return the action
     */
    public ActivityAction getAction() {
        return action;
    }

    /**
     * @param action the action to set
     */
    public void setAction(final ActivityAction action) {
        final ActivityAction oldValue = this.action;
        final ActivityAction newValue = action;
        this.action = newValue;
        firePropertyChange(PROPERTY_ACTIVITY, oldValue, newValue);
    }

}
