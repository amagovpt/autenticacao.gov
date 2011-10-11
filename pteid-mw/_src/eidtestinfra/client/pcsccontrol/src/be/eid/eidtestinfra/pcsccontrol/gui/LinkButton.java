package be.eid.eidtestinfra.pcsccontrol.gui;

import java.awt.Color;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.UIManager;

/**
 * A JButton that looks like a hyperlink on a webpage.
 * 
 * @author Rogier Taal
 * 
 */
public class LinkButton extends JButton {

	private static final long serialVersionUID = 1L;
	boolean underline = false;
	boolean alwaysUnderline = false;
	
	public LinkButton() {
		super();
		init();
	}
	
	public LinkButton(Action a) {
    	super(a);
    	init();
    }

	public LinkButton(Icon icon) {
    	super(icon);
    	init();
    }

	public LinkButton(String text) {
    	super(text);
    	init();
    }

	public LinkButton(String text, Icon icon) {
    	super(text, icon);
    	init();
    }
	
	public void setAlwaysUnderlined(boolean alwaysUnderline) {
		this.alwaysUnderline = alwaysUnderline;
		if(alwaysUnderline) {
			setForeground(Color.BLUE);
		} else {
			setForeground(UIManager.getColor("Button.foreground"));
		}
	}
	
	private void init() {
		setBorder(BorderFactory.createEmptyBorder(0,0,2,0));
		setBorderPainted(false);
		setContentAreaFilled(false);
		setFocusPainted(false);
		addMouseListener(new MouseAdapter() {
			
			public void mouseEntered(MouseEvent mouseevent) {
				underline = true;
				if(!alwaysUnderline) {
					setForeground(Color.BLUE);
				}
			    setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
				repaint();
		    }

		    public void mouseExited(MouseEvent mouseevent) {
		    	underline = false;
		    	if(!alwaysUnderline) {
		    		setForeground(UIManager.getColor("Button.foreground"));
		    	}
		    	setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
		    	repaint();
		    }
			
		});
	}
	
	public void paint (Graphics g)
    {
        super.paint(g);
        if (alwaysUnderline || underline)
        {
            Font f = getFont();
            FontMetrics fm = getFontMetrics(f);
            int strw = fm.stringWidth(getText());
            int inset = (g.getClipBounds().width-strw)/2;
            int y1 = fm.getHeight();
            int x1 = inset;
            int x2 = inset + strw;
            if (getText().length() > 0)
            g.drawLine(x1, y1, x2, y1);
        }
    }

}
