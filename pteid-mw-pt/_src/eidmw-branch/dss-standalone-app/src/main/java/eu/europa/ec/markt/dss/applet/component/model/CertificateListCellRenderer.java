package eu.europa.ec.markt.dss.applet.component.model;

import java.awt.Component;
import java.security.cert.X509Certificate;

import javax.swing.DefaultListCellRenderer;
import javax.swing.JList;

import eu.europa.ec.markt.dss.signature.token.DSSPrivateKeyEntry;

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
public class CertificateListCellRenderer extends DefaultListCellRenderer {
    /*
     * (non-Javadoc)
     * 
     * @see javax.swing.DefaultListCellRenderer#getListCellRendererComponent(javax.swing.JList, java.lang.Object, int,
     * boolean, boolean)
     */
    @Override
    public Component getListCellRendererComponent(final JList list, final Object value, final int index, final boolean isSelected, final boolean cellHasFocus) {
        final X509Certificate cert = ((DSSPrivateKeyEntry) value).getCertificate();
        String subjectDN = cert.getSubjectDN().getName();
        final int dnStartIndex = subjectDN.indexOf("CN=") + 3;
        if (dnStartIndex > 0 && subjectDN.indexOf(",", dnStartIndex) > 0) {
            subjectDN = subjectDN.substring(dnStartIndex, subjectDN.indexOf(",", dnStartIndex)) + " (SN:" + cert.getSerialNumber() + ")";
        }
        return super.getListCellRendererComponent(list, subjectDN, index, isSelected, cellHasFocus);
    }
}
