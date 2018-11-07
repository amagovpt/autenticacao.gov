package eu.europa.ec.markt.dss.applet.wizard.validation;

import java.io.File;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.output.ByteArrayOutputStream;

import eu.europa.ec.markt.dss.CertificateIdentifier;
import eu.europa.ec.markt.dss.DSSXMLUtils;
import eu.europa.ec.markt.dss.applet.controller.ActivityController;
import eu.europa.ec.markt.dss.applet.controller.DSSWizardController;
import eu.europa.ec.markt.dss.applet.main.DSSAppletCore;
import eu.europa.ec.markt.dss.applet.main.Parameters;
import eu.europa.ec.markt.dss.applet.model.ValidationModel;
import eu.europa.ec.markt.dss.applet.util.Globals;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;
import eu.europa.ec.markt.dss.applet.util.SimpleReportConverter;
import eu.europa.ec.markt.dss.applet.util.ValidationReportConverter;
import eu.europa.ec.markt.dss.applet.view.validation.Report102853View;
import eu.europa.ec.markt.dss.applet.view.validation.ValidationView;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardController;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardStep;
import eu.europa.ec.markt.dss.exception.DSSException;
import eu.europa.ec.markt.dss.signature.DSSDocument;
import eu.europa.ec.markt.dss.signature.FileDocument;
import eu.europa.ec.markt.dss.validation102853.report.ValidationReport;
import eu.europa.ec.markt.dss.validation102853.CommonCertificateVerifier;
import eu.europa.ec.markt.dss.validation102853.engine.function.XmlDom;
import eu.europa.ec.markt.dss.validation102853.engine.rules.wrapper.constraint.ValidationPolicyDao;
import eu.europa.ec.markt.dss.validation102853.report.SimpleReport;

/**
 * TODO
 * <p/>
 * <p/>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 */

public class ValidationWizardController extends DSSWizardController<ValidationModel> {

    private Report102853View report102853View;

    private ValidationView formView;
    
    boolean skip_first_step = false;

    /**
     * The default constructor for ValidationWizardController.
     *
     * @param core
     * @param model
     */
    public ValidationWizardController(final DSSAppletCore core, final ValidationModel model) {

        super(core, model);
        final Parameters parameters = core.getParameters();
        if (parameters.getSignedFile() != null)
        {
        	model.setSignedFileNoEvent(new File(parameters.getSignedFile()));
        	skip_first_step = true;
        }
    }

    /**
     *
     */
    public void displayFormView() {
        formView.show();
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.ecodex.dss.commons.swing.mvc.applet.WizardController#doCancel()
     */
    @Override
    protected void doCancel() {
        getCore().getController(ActivityController.class).display();
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardController#doStart()
     */
    @Override
    protected Class<? extends WizardStep<ValidationModel, ? extends WizardController<ValidationModel>>> doStart() {
    	 return skip_first_step ? Report102853Step.class : FormStep.class;
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardController#registerViews()
     */
    @Override
    protected void registerViews() {
        formView = new ValidationView(getCore(), this, getModel());
        report102853View = new Report102853View(getCore(), this, getModel());
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardController#registerWizardStep()
     */
    @Override
    protected Map<Class<? extends WizardStep<ValidationModel, ? extends WizardController<ValidationModel>>>, ? extends WizardStep<ValidationModel, ? extends WizardController<ValidationModel>>> registerWizardStep() {
        final Map steps = new HashMap();
        steps.put(FormStep.class, new FormStep(getModel(), formView, this));
        steps.put(Report102853Step.class, new Report102853Step(getModel(), report102853View, this));
        return steps;
    }

    private CommonCertificateVerifier trustedListCertificateVerifier;
    
    private File signed_file;
    
    public void setTrustedListCertificateVerifier(CommonCertificateVerifier trustedListCertificateVerifier) {
        this.trustedListCertificateVerifier = trustedListCertificateVerifier;
    }


    /**
     * Validate the document with the 102853 validation policy
     *
     * @throws IOException
     */
    public void validate102853Document() throws IOException {

        final ValidationModel model = getModel();
        final List<File> signed = model.getSignedFiles();
        
        //Delete existing reports
        model.clearDetailedReports();
        model.clearSimpleReports();
        
        
        for (File signed_file: signed)
        {

        	final DSSDocument signedDocument = new FileDocument(signed_file);
        	final File externalSignatureFile = model.getOriginalFile();

        	CertificateIdentifier.clear();

        	eu.europa.ec.markt.dss.validation102853.SignedDocumentValidator validator = eu.europa.ec.markt.dss.validation102853.SignedDocumentValidator
        			.fromDocument(signedDocument);
        	
        	//agrr: Added new method to define the language to use in the validation report
        	validator.setReportLanguage(Globals.getInstance().getLocale().getLanguage());
        	
        	validator.setCertificateVerifier(trustedListCertificateVerifier);

        	// In case of detached signature, the signature file path from JFileChooser
        	if (externalSignatureFile != null && externalSignatureFile.exists()) {
        		final DSSDocument externalSignatureDocument = new FileDocument(externalSignatureFile);
        		validator.setExternalContent(externalSignatureDocument);
        	}

        	URL validationPolicyURL = getParameter().getDefaultPolicyUrl();

        	assertValidationPolicyFileValid(validationPolicyURL);

        	eu.europa.ec.markt.dss.validation102853.report.ValidationReport report = validator.validateDocument(validationPolicyURL);

        	//TODO: this should be lists now that we're dealing with batch validation
        	model.setDiagnosticData102853(validator.getDiagnosticData());
        	model.setValidation102853Report(report);

        	model.setSimpleReport102853(validator.getSimpleReport());
        }
    }

    private void assertValidationPolicyFileValid(URL validationPolicyURL) {
        try {
            new ValidationPolicyDao().load(validationPolicyURL);
        } catch (Exception e) {
            throw new DSSException("The selected Validation Policy is not valid.");
        }
    }
    
//    private final String VALID = "<span class=\"indication-icon\">V</span>VALID";
    private final String INVALID = "INVALID";
//    private final String INDETERMINATE = "<span class=\"indication-icon\">?</span>INDETERMINATE";
    
    private void hideParam(Document html, String xpath_query)
    {
    	final String NOT_SELECTED = ResourceUtils.getI18n("UNSELECTED_PARAM");
    	NodeList nodes = null;

    	XPathFactory factory = XPathFactory.newInstance();
    	XPath xpath = factory.newXPath();
    	try 
    	{
    		nodes = (NodeList) xpath.evaluate(xpath_query, html, XPathConstants.NODESET);
    	}
    	catch (XPathExpressionException e) {

    		e.printStackTrace();
    	}

    	if (nodes != null)
    	{
    		for (int i=0; i!= nodes.getLength(); i++)
    		{

    			Element tr = ((Element)nodes.item(i));

    			NodeList children = tr.getChildNodes();
    			int j = 0;
    			while(j != children.getLength())
    			{
    				if (children.item(j).getNodeName().equals("td"))
    				{
    					children.item(j).setTextContent(NOT_SELECTED);
    				}
    				j++;
    			}
    		}

    	}

    }
    
    private String getTrustedResult(String subindication, String signature_info)
    {
    	if (signature_info != null && signature_info.equals(DSSException.NETWORK_ERROR))
    		return "INDETERMINATE";
    	if (subindication.equals("OUT_OF_BOUNDS_NO_POE") || subindication.equals("NO_CERTIFICATE_CHAIN_FOUND") ||
    			subindication.equals("NO_SIGNER_CERTIFICATE_FOUND") || subindication.equals("REVOKED_NO_POE") || subindication.equals("SIGNED_DATA_NOT_FOUND"))
    		return "INVALID";
    	else return "VALID";
    }
    
    private String[] STRINGS_EN = {"Validation Parameters", "Trusted Certificate:", "Timestamp:", "Long-term Validation:",
    		"Signed By:", "On Claimed time:"
    		};
    private String[] STRINGS_PT = {"Parâmetros de Validação", "Certificado Confiável:",
    		"Selo Temporal:", "Validação a longo prazo:", "Assinado Por:", "Assinatura efectuada em:" 
    		};
    
    private String[] TAGGED_STRINGS_EN = {"Document Name", "Failed to validate certificate: please check your Internet connection", "Validation Policy:", "QES AdESQC TL based", "Validate electronic signatures and indicates whether they are Advanced electronic Signatures (AdES), AdES supported by a Qualified Certificate (AdES/QC) or a Qualified electronic Signature (QES). All certificates and their related chains supporting the signatures are validated against the EU Member State Trusted Lists (this includes signer's certificate and certificates used to validate certificate validity status services - CRLs, OCSP, and time-stamps)."
    		};
    
    private String[] TAGGED_STRINGS_PT = { "Nome do Documento", "Erro na validação de certificado: por favor verifique a sua ligação à Internet", "Política de validação", "QES AdESQC TL based",
	"Assinatura Eletrónica Qualificada com base em certificados emitidos por Entidades Certificadoras que estão registadas nas TSLs (Trusted Service List) dos Estados Membros da UE em conformidade com o n.º 4 do artigo 2° da Decisão 2009/767/CE da Comissão, de 16 de Outubro de 2009."
    		};
    
    private String[] DETAILED_REPORT_STRINGS_EN = {
    	"Validation Report", "Basic Building Blocks", "Basic Validation Data", "Timestamp Validation Data", "AdES-T Validation Data", "Long Term Validation Data",
    	"Identification of the signing certificate", "Validation Context Initialization", "Cryptographic Verification", "Signature Acceptance Validation",
    	"X509 Certificate Validation", "Signature", "Timestamp"
    };
    
    private String[] DETAILED_REPORT_STRINGS_PT = {
        	"Relatório Detalhado", "Componentes Básicos", "Dados Básicos de Validação", "Validação de Selos temporais", "Validação AdES-T", "Validação a Longo prazo",
        	"Identificação do certificado do assinante", "Inicialização do Contexto de Validação", "Verificação Criptográfica", "Validação da aceitação da assinatura",
        	"Validação de certificado X509", "Assinatura", "Selo temporal"
        };
    
    private String documentToString(Document doc)
    {
    	ByteArrayOutputStream out = new ByteArrayOutputStream();
    	DSSXMLUtils.printDocument(doc.getFirstChild(), out);

    	try {
			return out.toString("UTF-8");
		} catch (UnsupportedEncodingException e) {

			e.printStackTrace();
		}
    	return null;
    }
    		
    
    private Document replaceI18nStrings(Document html)
    {
    	String tmp = documentToString(html);
    	
    	String[] translated_strings = null;

    	if (Globals.getInstance().getLocale().getLanguage().equals("pt"))
    	{
    		translated_strings = STRINGS_PT;
    		tmp = translateStatusValues(tmp, false);
    	}
    	else
    		translated_strings = STRINGS_EN;

    	Pattern p = Pattern.compile("STRING");
    	Matcher m = p.matcher(tmp);
    	StringBuffer sb = new StringBuffer();

    	int i = 0;
    	int str_index = 0;

    	while (m.find()) {

    		//Wrap-around the string match supporting a sequence of the same string replacements
    		str_index = i % translated_strings.length;

    		m.appendReplacement(sb, translated_strings[str_index]);
    		i++;
    	}
    	m.appendTail(sb);

    	try {
    		return DSSXMLUtils.buildDOM(sb.toString());
    	} catch (ParserConfigurationException e) {
    		e.printStackTrace();
    	} catch (IOException e) {

    		e.printStackTrace();
    	} catch (SAXException e) {

    		e.printStackTrace();
    	}

    	return null;
    }
    
    private Document replaceI18nStringsDetailed(Document doc)
    {

    	if (Globals.getInstance().getLocale().getLanguage().equals("pt"))
    	{
    		String tmp = documentToString(doc);
    		tmp = translateStatusValues(tmp, true);
        	
    		int i = 0;
    		for(String en_string: DETAILED_REPORT_STRINGS_EN)
    		{
    			Pattern p = Pattern.compile(en_string);
    			Matcher m = p.matcher(tmp);
    			StringBuffer sb = new StringBuffer();
    		
    			while (m.find()) {
    		
    				m.appendReplacement(sb, DETAILED_REPORT_STRINGS_PT[i]);
    			}
    			m.appendTail(sb);
    			tmp = sb.toString();
    			
    			i++;
    		}
    		
    		try {
        		return DSSXMLUtils.buildDOM(tmp);
        	} catch (ParserConfigurationException e) {
        		e.printStackTrace();
        	} catch (IOException e) {

        		e.printStackTrace();
        	} catch (SAXException e) {

        		e.printStackTrace();
        	}

        	return null;
    		

    	}
    	else 
    		return doc;

    }
    
    boolean isLocalePt()
    {
    	return Globals.getInstance().getLocale().getLanguage().equals("pt");
    }
    
    private Document replaceTaggedStrings(Document html)
    {
    	String[] tagged_strings = null;
    	String tmp = documentToString(html);
    	
    	if (isLocalePt())
    	{
    		tagged_strings = TAGGED_STRINGS_PT;
    	}
    	else
    		tagged_strings = TAGGED_STRINGS_EN;
    	
    	tmp = tmp.replaceAll("STR_DOCNAME", tagged_strings[0]);
    	
    	tmp = tmp.replaceAll("STR_NETWORK_ERROR", tagged_strings[1]);
    	
    	for (int i=0; i != 3; i++)
    	{
    		tmp = tmp.replace("STR_POL"+i, tagged_strings[i+2]);
    	}
    	
    	try {
    		return DSSXMLUtils.buildDOM(tmp);
    	}
    	catch (ParserConfigurationException e) {
    		e.printStackTrace();
    	}
    	catch (IOException e) {

    		e.printStackTrace();
    	} catch (SAXException e) {

    		e.printStackTrace();
    	}

    	return null;
    }
    
    
    private Document indentMultipleReports(Document doc)
    {
    	String tmp = documentToString(doc);

    	String indented_html = tmp.replaceFirst("padding-left: 0px;", "padding-left: 20px;");

    	try {
    		return DSSXMLUtils.buildDOM(indented_html);
    	}
    	catch (ParserConfigurationException e) {
    		e.printStackTrace();
    	} catch (IOException e) {

    		e.printStackTrace();
    	} catch (SAXException e) {

    		e.printStackTrace();
    	}
    	return null;
    }
    
    private String translateStatusValues(String tmp, boolean avoid_mismatch) {
    	if (avoid_mismatch)
    	{
    		tmp = tmp.replace("VALID</h4>", "VÁLIDO</h4>");
    		tmp = tmp.replace("VALID\n", "VÁLIDO\n");
    		
    	}
    	else
    		tmp = tmp.replace("VALID", "VÁLIDO");
    	tmp = tmp.replace("INVALID", "INVÁLIDO");
    	tmp = tmp.replace("INDETERMINATE", "INDETERMINADO");
		
    	return tmp;
	}
    
    /*
     */

	private void showTrustedCertResult(Document generated_html)
    {
    	
    	NodeList nodes = null;
    	
    	SimpleReport report = getModel().getMergedReport();
    	List<String> sig_ids = report.getSignatureIds();
    	
    	int signature_index = 0;

    	for (String id: sig_ids)
    	{
    		List<XmlDom> signature_info = report.getElements("/SimpleReport/Signature[@Id='%s']/Info", id);
    		String sig_info = null;
    		
    		if (signature_info.size() >= 2)
    			sig_info = signature_info.get(1).getText();
    		
    		String trustedResult = getTrustedResult(report.getSubIndication(id), sig_info);
    		
//    		System.out.println("DEBUG-> Signature ID: "+ id+ " TrustedResult: "+trustedResult);
  
    		XPathFactory factory = XPathFactory.newInstance();
    		XPath xpath = factory.newXPath();
    		try
    		{
    			nodes = (NodeList) xpath.evaluate("//*[local-name()='tr'][@class='paramTrusted']", 
    					generated_html, XPathConstants.NODESET);
    		}
    		catch (XPathExpressionException e) {

    			e.printStackTrace();
    		}

    		if (nodes != null)
    		{
    			Node elem = nodes.item(signature_index);
    			NodeList nl = elem.getChildNodes();
    			
    			int j = 0;
    			
    			while(j != nl.getLength())
    			{
    				if (nl.item(j).getNodeName().equals("td"))
    					nl.item(j).setTextContent(trustedResult);
    				j++;
    			}
    		}
    		
    		signature_index++;

    	}
    }
    
    /*
     * Apply the validation parameters on the global result for each signature
     * and also turn INDETERMINATE results to INVALID if the signing cert is
     */
    private List<String> fixGlobalResult()
    {
    	SimpleReport report = getModel().getMergedReport();
    	
    	//Map (SIG_ID, GlobalResult)
    	List <String> sig_results = new ArrayList<String>();
    	List<String> sig_ids = report.getSignatureIds();
    	
    	for (String id: sig_ids)
    	{
//    		System.out.println("DEBUG: fixGlobalResult() Signature ID: "+ id);
    		String result = report.getIndication(id);
    		
    		List<XmlDom> signature_info = report.getElements("/SimpleReport/Signature[@Id='%s']/Info", id);
    		String sig_info = null;
    		
    		if (signature_info.size() >= 2)
    			sig_info = signature_info.get(1).getText();
    		
    		//Reflect on the global result cases of Revoked/Expired certificate
    		if (result.equals("INDETERMINATE"))
    			result = getTrustedResult(report.getSubIndication(id), sig_info);
    		
    		if (getModel().isTimestamp() && report.getTimestamp(id).equals("INVALID"))
    		{
    			result = INVALID;
    		}
    		
    		if (getModel().isLong_term_validation() && report.getLTV(id).equals("INVALID"))
    		{
    			result = INVALID;
    		}
    		
    		sig_results.add(result);
    	}
    	
    	return sig_results;
    }
    
    /*
     */
    private void hideIgnoredParameters(Document generated_html)
    {

    	boolean show = getModel().isQualifiedCert();

    	if (!show)
    	{
    		hideParam(generated_html, "//*[local-name()='tr'][@class='paramQC']");
    	}

    	show = getModel().isTimestamp();

    	if (!show)
    	{
    	
    		hideParam(generated_html, "//*[local-name()='tr'][@class='paramTS']");
    	}

    	show = getModel().isLong_term_validation();

    	if (!show)
    	{
    		hideParam(generated_html, "//*[local-name()='tr'][@class='paramLTV']");
    	}
  
    }
    
    private SimpleReport mergeReports(SimpleReport report1, SimpleReport report2)
    {

    	DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    	dbf.setNamespaceAware(true);
    	Document document1 = report1.getRootElement().getOwnerDocument();
    	
    	final String namespaceURI = "http://dss.markt.ec.europa.eu/validation/diagnostic";

    	Element root = report2.getRootElement();

    	NodeList signaturesSecondFile = root.getElementsByTagNameNS(namespaceURI, "Signature");
    	NodeList docNameSecondFile = root.getElementsByTagNameNS(namespaceURI, "DocumentName");
    	NodeList validationTimeSecondFile = root.getElementsByTagNameNS(namespaceURI, "ValidationTime");

    	int i = 0;
    	Node imported = null;
    	//Only one DocumentName and ValidationTime per report
    	imported =  document1.importNode(validationTimeSecondFile.item(0), true);
    	report1.getRootElement().appendChild(imported);
    	
    	imported =  document1.importNode(docNameSecondFile.item(0), true);
    	report1.getRootElement().appendChild(imported);

    	while (i != signaturesSecondFile.getLength())
    	{
//    		System.err.println("DEBUG: mergeReports importing signature...");
    		imported = document1.importNode(signaturesSecondFile.item(i), true);
    		report1.getRootElement().appendChild(imported);
    		i++;

    	}
    	
    	return report1;

    }
    
    /*
     */

    private void replaceResult(Document html, List<String> results) {

    	XPathFactory factory = XPathFactory.newInstance();
    	XPath xpath = factory.newXPath();
    	NodeList nodes = null;
    	
    	String[] result_set = results.toArray(new String[0]);
//    	for (String s: result_set)
//    	{
//    		System.err.println("DEBUG: Result Set: "+s);
//    	}
    	
    	try
    	{
    		nodes = (NodeList)
    				xpath.evaluate("//*[local-name()='tr'][@class='signature-start']", html, XPathConstants.NODESET);

    	}
    	catch (XPathExpressionException e) {

    		e.printStackTrace();
    	}
		
    	for (int i=0; i!= results.size(); i++)
    	{

    		Element tr = ((Element) nodes.item(i));
    		NodeList nl = tr.getChildNodes();
    		int j = 0;
    		while(j != nl.getLength())
    		{
    			if (nl.item(j).getNodeName().equals("th"))
    			{

    				nl.item(j).setTextContent(result_set[i]);
    				((Element)nl.item(j)).setAttribute("class", result_set[i]);
    			}
    			j++;
    		}

    	}
		
	}
    
    /* Workaround for bug in dss-document regarding the document name of ASIC signed files... */
    private void replaceFileName(Document generated_html)
    {
    	List<File> signed_files = getModel().getSignedFiles();
    	
    	XPathFactory factory = XPathFactory.newInstance();
    	XPath xpath = factory.newXPath();
    	NodeList nodes = null;
    	String nodeName = null, nodeContent = null;
    	
    	try
    	{
    		nodes = (NodeList)
    				xpath.evaluate("//*[local-name()='tr'][@class='documentName']", generated_html, XPathConstants.NODESET);

    		for (int i=0; i!= nodes.getLength(); i++)
    		{
    			Element tr = ((Element) nodes.item(i));
    			NodeList nl = tr.getChildNodes();
    			int j = 0;

    			while(j != nl.getLength())
    			{
    				nodeName = nl.item(j).getNodeName();
    				if (nodeName.equals("td"))
    				{
    					nodeContent = nl.item(j).getTextContent();  
    					if (nodeContent.equals("CMSSignedDocument"))
    						nl.item(j).setTextContent(signed_files.get(i).getName());
    				}

    				j++;
    			}
    		}
    	}
    	catch (XPathExpressionException e) {

    		e.printStackTrace();
    	}
    	
    }
    
    //Quite hackish way of detecting unsigned documents and inserting the appropriate warning... 
    private void addWarningUnsignedDocument(Document generated_html)
    {
    	XPathFactory factory = XPathFactory.newInstance();
    	XPath xpath = factory.newXPath();
    	NodeList nodes = null;
    	try
    	{
    		nodes = (NodeList)
    				xpath.evaluate("//*[local-name()='tr'][@class='documentName']", generated_html, XPathConstants.NODESET);

    	}
    	catch (XPathExpressionException e) {

    		e.printStackTrace();
    	}
    	
    	int i = 0;
    	Element next_elem = null;
    	Node tmp = null;
    	while (i != nodes.getLength())
    	{
    		tmp = nodes.item(i).getNextSibling();
    		//Skip over text nodes
    		while (tmp != null && tmp.getNodeType() == Node.TEXT_NODE)
    			tmp = tmp.getNextSibling();

    		if (tmp == null)
    		{
    			System.err.println("Probably broken HTML, unexpected end of document...");
    			break;
    		}
    		
    		next_elem = (Element)tmp;
    		//This is the pattern for unsigned documents, a documentName row NOT followed by a signature-start row... 
    		if (!next_elem.getAttribute("class").equals("signature-start"))
    		{

    			Element elem = next_elem;
    			Element parent = (Element) elem.getParentNode();

    			Element warning_elem = elem.getOwnerDocument().createElement("tr");
    			warning_elem.setAttribute("class", "signature-start");

    			Element warning_elem2 = elem.getOwnerDocument().createElement("th");
    			warning_elem2.setTextContent(ResourceUtils.getI18n("UNSIGNED_WARNING"));
    			warning_elem2.setAttribute("class", INVALID);

    			warning_elem.appendChild(warning_elem2);

    			parent.insertBefore(warning_elem, elem);
    		}
    		
    		i++;
    	}
    }
    
    private void removeDuplicateSignatureIDs(SimpleReport merged)
    {
    	Element root = merged.getRootElement();
    	final String namespaceURI = "http://dss.markt.ec.europa.eu/validation/diagnostic";

    	NodeList signatures = root.getElementsByTagNameNS(namespaceURI, "Signature");
    	int i = 0;
    	Random r = new Random();
    	Set<String> sig_ids = new HashSet<String>();
    	String id_string = null;
    	
    	while (i != signatures.getLength())
    	{
    		id_string = ((Element)signatures.item(i)).getAttribute("Id");
    		
    		if (sig_ids.contains(id_string))
    		{
    			id_string = id_string + "_" + r.nextLong();
    			((Element)signatures.item(i)).setAttribute("Id", id_string);
    		
    		}
    			
    		sig_ids.add(id_string);
    		i++;
    	}
    	
    }

    public Document renderSimpleReportAsHtml()
    {
    	
        final List<SimpleReport> reports = getModel().getSimpleReport102853();
        SimpleReport merged = null;
        final SimpleReportConverter simpleReportConverter = new SimpleReportConverter();
        int reports_count = reports.size();
        
        if (reports_count == 1)
        {
        	merged = reports.get(0);
        	getModel().setMergedReport(merged);
        }
        else
        {
        	merged = reports.get(0);
        	for (int i = 0; i != reports.size() -1 ; i++)
        	{
        	   merged = mergeReports(merged, reports.get(i+1));
        	
        	}
        	
        	removeDuplicateSignatureIDs(merged);
        	getModel().setMergedReport(merged);
        }
        
        Document doc = simpleReportConverter.renderAsHtml(merged);
        
        //Apply the validation parameters selected by the user
        List<String> results = fixGlobalResult();
        replaceResult(doc, results);
        hideIgnoredParameters(doc);
        showTrustedCertResult(doc);
        
        //Workaround filename bug
        replaceFileName(doc);
        
        //Add a warning when the document doesn't contain signatures...
        addWarningUnsignedDocument(doc);
        
        doc = replaceI18nStrings(doc);
        doc = replaceTaggedStrings(doc);
        
        if (reports_count > 1)
        	doc = indentMultipleReports(doc);
                
        return doc;
    }


	public List<Document> renderValidationReportAsHtml() {

        final List<ValidationReport> validationReports = getModel().getValidation102853Report();
        final ValidationReportConverter validationReportConverter = new ValidationReportConverter();
        
        List <Document> htmlReports = new ArrayList<Document>();
        for (ValidationReport v: validationReports)
        {
        	Document english_version = validationReportConverter.renderAsHtml(v);
        	Document translated_doc = replaceI18nStringsDetailed(english_version);
        	htmlReports.add(translated_doc);
        }
        return htmlReports;
    }

}
