package eu.europa.ec.markt.dss.applet.controller;

import eu.europa.ec.markt.dss.applet.io.RemoteCRLSource;
import eu.europa.ec.markt.dss.applet.io.RemoteOCSPSource;
import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.applet.main.Parameters;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.AppletController;
import eu.europa.ec.markt.dss.validation.TrustedListCertificateVerifier;
import eu.europa.ec.markt.dss.validation.tsp.TSPSource;

import org.bouncycastle.jce.provider.BouncyCastleProvider;

import java.security.Security;

/**
 * 
 * TODO
 * 
 * <p>
 * DISCLAIMER: Project owner DG-MARKT.
 * 
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @param <M>
 */
public abstract class DSSAppletController<M> extends AppletController<DSSAppletCore, M> {

    protected final String serviceURL;


    protected TrustedListCertificateVerifier certificateVerifier;
    
	protected TSPSource tspSource;

    protected RemoteOCSPSource ocspSource;

	protected RemoteCRLSource crlSource;

   
	/**
     * 
     * The default constructor for DSSAppletController.
     * 
     * @param core
     * @param model
     */
    protected DSSAppletController(final DSSAppletCore core, final M model) {
        super(core, model);

        Security.addProvider(new BouncyCastleProvider());

        final Parameters parameters = core.getParameters();

        serviceURL = parameters.getServiceURL();

    }
    
    public void setCertificateVerifier(TrustedListCertificateVerifier certificateVerifier) 
    {
		this.certificateVerifier = certificateVerifier;
	}
    
    public void setTspSource(TSPSource tsp)
    {
    	this.tspSource = tsp;
    }
    
    public void setOcspSource(RemoteOCSPSource ocspSource) {
		this.ocspSource = ocspSource;
	}
    
    public void setCrlSource(RemoteCRLSource crlSource) {
		this.crlSource = crlSource;
	}
    

    /**
     * 
     * @return
     */
    public Parameters getParameter() {
        return getCore().getParameters();
    }

}
