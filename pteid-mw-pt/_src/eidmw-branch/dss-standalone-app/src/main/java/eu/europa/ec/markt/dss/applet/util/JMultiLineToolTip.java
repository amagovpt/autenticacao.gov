package eu.europa.ec.markt.dss.applet.util;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;

import javax.swing.CellRendererPane;
import javax.swing.JComponent;
import javax.swing.JTextArea;
import javax.swing.JToolTip;
import javax.swing.plaf.ComponentUI;
import javax.swing.plaf.basic.BasicToolTipUI;


/**
 * @author Zafir Anjum
 */


public class JMultiLineToolTip extends JToolTip {
    /** */
    private static final String uiClassID = "ToolTipUI";

    /** */
    private String tipText;
    /** */
    private JComponent component;

    /** */
    public JMultiLineToolTip() {
        updateUI();
    }

    /** */
    public void updateUI() {
        setUI(MultiLineToolTipUI.createUI(this));
    }

    /** @param columns number of columns */
    public void setColumns(final int columns) {
        this.columns = columns;
        this.fixedwidth = 0;
    }

    /** @return columns */
    public int getColumns() {
        return columns;
    }

    /** @param width width */
    public void setFixedWidth(final int width) {
        this.fixedwidth = width;
        this.columns = 0;
    }

    /** @return ret */
    public int getFixedWidth() {
        return fixedwidth;
    }
    /** */
    protected int columns = 0;
    /** */
    protected int fixedwidth = 0;
}


/** */
class MultiLineToolTipUI extends BasicToolTipUI {
    /** */
    private static MultiLineToolTipUI sharedInstance = new MultiLineToolTipUI();
    /** */
    private Font smallFont;
    /** */
    private static JToolTip tip;
    /** */
    protected CellRendererPane rendererPane;
    /** */
    private static JTextArea textArea;

    /** @param c component
     *  @return ret component*/
    public static ComponentUI createUI(final JComponent c) {
        return sharedInstance;
    }
    /** */
    public MultiLineToolTipUI() {
        super();
    }
    /** @param component */
    public void installUI(JComponent c) {
        super.installUI(c);
        tip = (JToolTip) c;
        rendererPane = new CellRendererPane();
        c.add(rendererPane);
    }

    /** @param c component */
    public void uninstallUI(final JComponent c) {
        super.uninstallUI(c);

        c.remove(rendererPane);
        rendererPane = null;
    }

    /** @param g graphics
     *  @param c component
     *   */
    public void paint(final Graphics g, final JComponent c) {
        Dimension size = c.getSize();
        textArea.setBackground(c.getBackground());
        rendererPane.paintComponent(g, textArea, c, 1, 1,
                        size.width - 1, size.height - 1, true);
    }

    /** @param c component
     *  @return ret dimension */
    public Dimension getPreferredSize(final JComponent c) {
        String tipText = ((JToolTip) c).getTipText();
        if (tipText == null) {
            return new Dimension(0, 0);
        }
        textArea = new JTextArea(tipText);
        rendererPane.removeAll();
        rendererPane.add(textArea);
        textArea.setWrapStyleWord(true);
        int width = ((JMultiLineToolTip) c).getFixedWidth();
        int columns = ((JMultiLineToolTip) c).getColumns();

        if (columns > 0) {
            textArea.setColumns(columns);
            textArea.setSize(0, 0);
        textArea.setLineWrap(true);
            textArea.setSize(textArea.getPreferredSize());
        } else if (width > 0) {
        textArea.setLineWrap(true);
            Dimension d = textArea.getPreferredSize();
            d.width = width;
            d.height++;
            textArea.setSize(d);
        } else {
            textArea.setLineWrap(false);
        }


        Dimension dim = textArea.getPreferredSize();

        dim.height += 1;
        dim.width += 1;
        return dim;
    }

    /** @param c component
     *  @return ret dimension */
    public Dimension getMinimumSize(final JComponent c) {
        return getPreferredSize(c);
    }

    /** @param c component
     *  @return ret dimension */
    public Dimension getMaximumSize(final JComponent c) {
        return getPreferredSize(c);
    }
}



