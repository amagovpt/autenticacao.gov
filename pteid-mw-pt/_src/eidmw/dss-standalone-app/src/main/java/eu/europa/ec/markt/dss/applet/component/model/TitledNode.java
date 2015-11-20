package eu.europa.ec.markt.dss.applet.component.model;

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
public class TitledNode {

    private String title;
    private Object value;
    private boolean inline = false;

    /**
     * 
     * The default constructor for TitledNode.
     * 
     * @param title
     * @param value
     */
    public TitledNode(final String title, final Object value) {
        this(title, value, false);
    }

    /**
     * 
     * The default constructor for TitledNode.
     * 
     * @param title
     * @param value
     * @param inline
     */
    public TitledNode(final String title, final Object value, final boolean inline) {
        this.title = title;
        this.value = value;
        this.inline = inline;
    }

    String getTitle() {
        return title;
    }

    Object getValue() {
        return value;
    }

    /**
     * 
     * @return
     */
    public boolean isInline() {
        return inline;
    }

}
