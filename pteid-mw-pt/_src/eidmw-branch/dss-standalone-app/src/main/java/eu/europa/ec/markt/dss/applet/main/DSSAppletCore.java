package eu.europa.ec.markt.dss.applet.main;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.logging.Level;

import org.apache.commons.lang.StringUtils;

import eu.europa.ec.markt.dss.applet.controller.ActivityController;
import eu.europa.ec.markt.dss.applet.main.Parameters.AppletUsage;
import eu.europa.ec.markt.dss.applet.model.ActivityModel;
import eu.europa.ec.markt.dss.applet.model.ValidationModel;
import eu.europa.ec.markt.dss.applet.util.DSSStringUtils;
import eu.europa.ec.markt.dss.applet.wizard.validation.ValidationWizardController;
import eu.europa.ec.markt.dss.validation.TrustedListCertificateVerifier;
import eu.europa.ec.markt.dss.common.SignatureTokenType;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;
import eu.europa.ec.markt.dss.signature.SignaturePackaging;

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
public class DSSAppletCore extends ApplicationCore {

    private static final String PARAM_APPLET_USAGE = "usage";

    private static final String PARAM_SERVICE_URL = "service_url";

    private static final String PARAM_SIGNATURE_POLICY_ALGO = "signature_policy_algo";
    private static final String PARAM_SIGNATURE_POLICY_HASH = "signature_policy_hash";

    private static final String PARAM_STRICT_RFC3370 = "strict_rfc3370";

    private static final String PARAM_SIGNATURE_PACKAGING = "signature_packaging";
    private static final String PARAM_SIGNATURE_LEVEL = "signature_level";

    private static final String PARAM_DEFAULT_POLICY_URL = "default_policy_url";

    private Parameters parameters;

    private AppletModule injector;

    /**
     * Default constructor
     */
    public DSSAppletCore() {
    	super();
        injector = new AppletModule(this);
        
    }


    /**
     * @return the parameters
     */
    public Parameters getParameters() {
        return parameters;
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.ecodex.dss.commons.swing.mvc.AbstractApplet#layout(javax.swing.JApplet)
     */
    @Override
    public void executeLayout() {
        getController(ActivityController.class).display();
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.ecodex.dss.commons.swing.mvc.AbstractApplet#registerControllers()
     */
    @Override
    public void registerControllers() {

    	ActivityController c1 = new ActivityController(this, new ActivityModel());
    	c1.setCertificateVerifier((TrustedListCertificateVerifier)injector.getCertificateVerifier(injector.getCRLSource(),
    			injector.getOSCPSource(), injector.getCertificateSource()));

    	ValidationWizardController c2 = new ValidationWizardController(this, new ValidationModel());
    	c2.setTrustedListCertificateVerifier(injector.getTrustedListCertificateVerifier102853(
    			injector.getCRLSource(),
    			injector.getOSCPSource(),
    			injector.getCertificateSource102853()));

    	getControllers().put(ActivityController.class, c1);
    	getControllers().put(ValidationWizardController.class, c2);

    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.ecodex.dss.commons.swing.mvc.applet.AppletCore#registerParameters()
     */
    public void registerParameters(ParameterProvider parameterProvider, String signedFile) {

        LOG.log(Level.INFO, "Register applet parameters ");

        final Parameters parameters = new Parameters();
        
        if (signedFile != null)
        	parameters.setSignedFile(signedFile);

        final String appletUsageParam = parameterProvider.getParameter(PARAM_APPLET_USAGE);
        if (StringUtils.isNotEmpty(appletUsageParam)) {
            parameters.setAppletUsage(AppletUsage.valueOf(appletUsageParam.toUpperCase()));
        }
    

        // Service URL
        final String serviceParam = parameterProvider.getParameter(PARAM_SERVICE_URL);
        if (StringUtils.isEmpty(serviceParam)) {
            throw new IllegalArgumentException(PARAM_SERVICE_URL + "cannot be empty");
        }
        parameters.setServiceURL(serviceParam);

        // RFC3370
        final String rfc3370Param = parameterProvider.getParameter(PARAM_STRICT_RFC3370);
        if (StringUtils.isNotEmpty(rfc3370Param)) {
            try {
                parameters.setStrictRFC3370(Boolean.parseBoolean(rfc3370Param));
            } catch (final Exception e) {
                LOG.log(Level.WARNING, "Invalid value of " + PARAM_STRICT_RFC3370 + " parameter: {0}", rfc3370Param);
            }
        }

//        final String signaturePolicyValueParam = parameterProvider.getParameter(PARAM_SIGNATURE_POLICY_HASH);
//        parameters.setSignaturePolicyValue(Base64.decodeBase64(signaturePolicyValueParam));

        // Default policy URL
        final String defaultPolicyUrl = parameterProvider.getParameter(PARAM_DEFAULT_POLICY_URL);
        if (StringUtils.isNotEmpty(defaultPolicyUrl)) {
            try {
                parameters.setDefaultPolicyUrl(new URL(defaultPolicyUrl));
            } catch (IOException e) {
                throw new IllegalArgumentException(PARAM_DEFAULT_POLICY_URL + " cannot be opened", e);
            }
        }

        this.parameters = parameters;

        LOG.log(Level.INFO, "Parameters - {0}", parameters);

    }
}
