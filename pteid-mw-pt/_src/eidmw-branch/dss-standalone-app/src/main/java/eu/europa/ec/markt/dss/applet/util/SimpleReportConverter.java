package eu.europa.ec.markt.dss.applet.util;

import java.io.InputStream;
import java.util.Locale;

import eu.europa.ec.markt.dss.validation102853.report.SimpleReport;

/**
 * Takes a XmlDom simpleReport from the ProcessExecutor and generates a HTML document
 * <p/>
 * <p/>
 * <p/>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 */
public class SimpleReportConverter extends XsltConverter<SimpleReport> {

    
	InputStream getXsltFileClasspathResource() {
        return getClass().getResourceAsStream("/eu/europa/ec/markt/dss/applet/util/simpleReport.xslt");
    }

}
