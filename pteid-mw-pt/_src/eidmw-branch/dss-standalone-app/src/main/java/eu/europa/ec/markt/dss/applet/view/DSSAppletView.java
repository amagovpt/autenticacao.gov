package eu.europa.ec.markt.dss.applet.view;

import java.awt.Container;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import com.jgoodies.binding.beans.BeanAdapter;

import eu.europa.ec.markt.dss.applet.controller.DSSAppletController;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.AppletView;

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
public abstract class DSSAppletView<M, C extends DSSAppletController<M>> extends AppletView<M, C> implements PropertyChangeListener {
    /**
     * 
     * The default constructor for DSSAppletView.
     * 
     * @param core
     * @param controller
     * @param model
     */
    public DSSAppletView(final ApplicationCore core, final C controller, final M model) {
        super(core, controller, model);
        final BeanAdapter<M> beanAdapter = new BeanAdapter<M>(model);
        beanAdapter.addBeanPropertyChangeListener(this);
    }

    public void doInit() {
    }

    protected abstract Container doLayout();

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.commons.swing.mvc.applet.AppletView#layout()
     */
    @Override
    protected Container layout() {
        doInit();
        return doLayout();
    }

    /**
     * 
     * @param evt
     */
    public void modelChanged(final PropertyChangeEvent evt) {
    };

    /*
     * (non-Javadoc)
     * 
     * @see java.beans.PropertyChangeListener#propertyChange(java.beans.PropertyChangeEvent)
     */
    @Override
    public void propertyChange(final PropertyChangeEvent evt) {
        modelChanged(evt);
    }

}
