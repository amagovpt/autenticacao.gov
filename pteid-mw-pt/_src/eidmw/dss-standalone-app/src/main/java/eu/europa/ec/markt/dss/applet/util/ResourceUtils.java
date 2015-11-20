package eu.europa.ec.markt.dss.applet.util;

import java.io.IOException;
import java.net.URI;
import java.util.Locale;
import java.util.ResourceBundle;

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
public final class ResourceUtils {

    private static final ResourceBundle BUNDLE_PT, BUNDLE_EN;

    static {
        BUNDLE_PT = ResourceBundle.getBundle("eu/europa/ec/markt/dss/applet/i18n", new Locale("pt"));
        BUNDLE_EN = ResourceBundle.getBundle("eu/europa/ec/markt/dss/applet/i18n", new Locale("en"));
    }

    /**
     * 
     * @param key
     * @return
     */
    public static String getI18n(final String key) {
        String locale = Globals.getInstance().getLocale().getLanguage();
    	if (locale.equals("pt"))
    		return BUNDLE_PT.getString(key);
    	else 
    		return BUNDLE_EN.getString(key);
        
    }

    /**
     * 
     * @param uri
     * @throws IOException
     */
//    public static void openFile(final URI uri) throws IOException {
//        Runtime.getRuntime().exec("rundll32 url.dll,FileProtocolHandler " + uri.toString());
//    }

    private ResourceUtils() {
    }

}
