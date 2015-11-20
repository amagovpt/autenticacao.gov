package eu.europa.ec.markt.dss.commons.swing.mvc.applet;

/**
 * TODO
 * 
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 * 
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 */
@SuppressWarnings("serial")
public class ControllerException extends Exception {
    /**
     * 
     * The default constructor for ControllerException.
     */
    public ControllerException() {
        super();
    }

    /**
     * 
     * The default constructor for ControllerException.
     * 
     * @param message
     */
    public ControllerException(final String message) {
        super(message);
        // TODO Auto-generated constructor stub
    }

    /**
     * 
     * The default constructor for ControllerException.
     * 
     * @param message
     * @param cause
     */
    public ControllerException(final String message, final Throwable cause) {
        super(message, cause);
    }

    /**
     * 
     * The default constructor for ControllerException.
     * 
     * @param cause
     */
    public ControllerException(final Throwable cause) {
        super(cause);
    }

}
