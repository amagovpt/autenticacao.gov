package eu.europa.ec.markt.dss.applet.util;

import org.apache.commons.lang.StringUtils;

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
public final class DSSStringUtils {
    /**
     * 
     * @param source
     * @param values
     * @return
     */
    public static boolean contains(final String source, final String... values) {

        if (StringUtils.isEmpty(source)) {
            return false;
        }

        for (final String value : values) {
            if (source.equals(value)) {
                return true;
            }
        }

        return false;
    }

    private DSSStringUtils() {

    }

}
