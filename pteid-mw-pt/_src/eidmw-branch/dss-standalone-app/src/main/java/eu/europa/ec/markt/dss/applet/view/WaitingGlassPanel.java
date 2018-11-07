package eu.europa.ec.markt.dss.applet.view;

import java.awt.Cursor;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JLabel;
import javax.swing.JPanel;

import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;

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
public class WaitingGlassPanel extends JPanel {

    /**
     * 
     * The default constructor for WaitingGlassPanel.
     */
    public WaitingGlassPanel() {
        final JLabel label = ComponentFactory.createLabel(ResourceUtils.getI18n("PLEASE_WAIT"), ComponentFactory.iconWait());
        this.setCursor(new Cursor(Cursor.WAIT_CURSOR));
        this.setOpaque(false);

        this.setLayout(new GridBagLayout());
        this.add(label, new GridBagConstraints());
    }

    /*
     * (non-Javadoc)
     * 
     * @see javax.swing.JComponent#paintComponent(java.awt.Graphics)
     */
    @Override
    protected void paintComponent(final Graphics g) {
        g.setColor(new java.awt.Color(255, 255, 255, 150));
        g.fillRect(0, 0, getWidth() - 1, getHeight() - 1);
        super.paintComponent(g);
    }
}
