package eu.europa.ec.markt.dss.applet.model;

import java.io.File;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

import org.apache.commons.lang.StringUtils;
import org.apache.commons.lang.builder.ReflectionToStringBuilder;

import com.jgoodies.binding.beans.Model;

import eu.europa.ec.markt.dss.validation.report.ValidationReport;
import eu.europa.ec.markt.dss.validation102853.data.diagnostic.DiagnosticData;
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
@SuppressWarnings("serial")
public class ValidationModel extends Model {
    /**
     *
     */
    public static final String CHANGE_PROPERTY_SIGNED_FILE = "signedFile";
    private List<File> signedFiles = new ArrayList<File>();
    /**
     *
     */
    public static final String CHANGE_PROPERTY_ORIGINAL_FILE = "originalFile";
    public static final String CHANGE_PROPERTY_PARAMS_QUALIFIED_CERT = "qualifiedCert";
    public static final String CHANGE_PROPERTY_PARAMS_TIMESTAMP = "timestamp";
    public static final String CHANGE_PROPERTY_PARAMS_LTV = "ltv";
    
    private File originalFile;

//    public static final String CHANGE_PROPERTY_VALIDATION_LEGACY_CHOOSEN = "validationLegacyChoosen";

    //private boolean validationLegacyChoosen = false;

    private File selectedPolicyFile;

    private ValidationReport validationReport;

    public static final String CHANGE_PROPERTY_VALIDATION_102853_REPORT = "validation102853Report";
    private List<eu.europa.ec.markt.dss.validation102853.report.ValidationReport> validation102853Report = 
    		new ArrayList<eu.europa.ec.markt.dss.validation102853.report.ValidationReport>();

    public static final String CHANGE_PROPERTY_DIAGNOSTIC_DATA_102853 = "diagnosticData102853";
    private DiagnosticData diagnosticData102853;

    public static final String CHANGE_PROPERTY_SIMPLE_REPORT_102853 = "simpleReport102853";
    private List<SimpleReport> simpleReport102853 = new ArrayList<SimpleReport>();
    
    /* The final merged report which is a union of all the previous ones and is ready to be processed by the XSLT */
    private SimpleReport mergedReport;
    
    public SimpleReport getMergedReport() {
		return mergedReport;
	}

	public void setMergedReport(SimpleReport mergedReport) {
		this.mergedReport = mergedReport;
	}

	private boolean qualifiedCert;

	private boolean timestamp;
	
    private boolean long_term_validation;
    
    
    public boolean isQualifiedCert() {
  		return qualifiedCert;
  	}

  	public void setQualifiedCert(boolean qualifiedCert) {
  		final boolean oldValue = this.qualifiedCert;
  		this.qualifiedCert = qualifiedCert;
  		firePropertyChange(CHANGE_PROPERTY_PARAMS_QUALIFIED_CERT, oldValue, this.qualifiedCert);
  	}
  	
    public boolean isTimestamp() {
		return timestamp;
	}

	public void setTimestamp(boolean timestamp) {
		final boolean oldValue = this.timestamp;
		this.timestamp = timestamp;
		firePropertyChange(CHANGE_PROPERTY_PARAMS_TIMESTAMP, oldValue, this.timestamp);
	}

	public boolean isLong_term_validation() {
		return long_term_validation;
	}

	public void setLong_term_validation(boolean long_term_validation) {
		final boolean oldValue = this.long_term_validation;
		this.long_term_validation = long_term_validation;
		firePropertyChange(CHANGE_PROPERTY_PARAMS_LTV, oldValue, this.long_term_validation);
	}

	/**
     * @return the originalFile
     */
    public File getOriginalFile() {
        return originalFile;
    }

    /**
     * @return the signedFile
     */
    public List<File> getSignedFiles() {
        return signedFiles;
    }

    /**
     * @return the validationReport
     */
    public ValidationReport getValidationReport() {
        return validationReport;

    }

    /**
     * @param originalFile the originalFile to set
     */
    public void setOriginalFile(final File originalFile) {
        final File oldValue = this.originalFile;
        final File newValue = originalFile;
        this.originalFile = newValue;
        firePropertyChange(CHANGE_PROPERTY_ORIGINAL_FILE, oldValue, newValue);
    }

    /**
     * @param signedFile the signedFile to set
     */
    public void setSignedFile(final File signedFile) {
    	if (signedFile == null)
    	{
    		this.signedFiles.clear();
    		firePropertyChange(CHANGE_PROPERTY_SIGNED_FILE, null, null);
    		return;
    	}
    	
        //final File oldValue = this.signedFile;
        final File newValue = signedFile;
        this.signedFiles.add(newValue);
        firePropertyChange(CHANGE_PROPERTY_SIGNED_FILE, null, newValue);
    }
    
    public void setSignedFileNoEvent(final File signedFile)
    {
    	this.signedFiles.add(signedFile);
    }

    /**
     * @param validationReport the validationReport to set
     */
    public void setValidationReport(final ValidationReport validationReport) {
        this.validationReport = validationReport;
    }

    /*
	 *
     */
    @Override
    public String toString() {
        return ReflectionToStringBuilder.reflectionToString(this);
    }

    public File getSelectedPolicyFile() {
        return selectedPolicyFile;
    }

    public List <eu.europa.ec.markt.dss.validation102853.report.ValidationReport> getValidation102853Report() {
        return validation102853Report;
    }

    public void setValidation102853Report(eu.europa.ec.markt.dss.validation102853.report.ValidationReport validation102853Report) 
    {
    	eu.europa.ec.markt.dss.validation102853.report.ValidationReport oldValue = null;
    	if (!this.validation102853Report.isEmpty())
    		oldValue = this.validation102853Report.get(this.validation102853Report.size()-1);
    	
        final eu.europa.ec.markt.dss.validation102853.report.ValidationReport newValue = validation102853Report;
        this.validation102853Report.add(validation102853Report);
        firePropertyChange(CHANGE_PROPERTY_VALIDATION_102853_REPORT, oldValue, newValue);
    }

    public void setDiagnosticData102853(DiagnosticData diagnosticData102853) {
        final DiagnosticData oldValue = this.diagnosticData102853;
        final DiagnosticData newValue = diagnosticData102853;
        this.diagnosticData102853 = diagnosticData102853;
        firePropertyChange(CHANGE_PROPERTY_DIAGNOSTIC_DATA_102853, oldValue, newValue);
    }

    public DiagnosticData getDiagnosticData102853() {
        return diagnosticData102853;
    }

    public List<SimpleReport> getSimpleReport102853() {
        return simpleReport102853;
    }
    
    private String[] getSignedFilenames()
    {
    	String[] filenames = new String[this.signedFiles.size()];
    	int i = 0;
    	for (File f: this.signedFiles)
    	{
    	  filenames[i++] =	f.getName();
    	}
    	
    	return filenames;
    }

    public void setSimpleReport102853(SimpleReport simpleReport102853) {
//        final SimpleReport oldValue = this.simpleReport102853;
        final SimpleReport newValue = simpleReport102853;
        this.simpleReport102853.add(simpleReport102853);
        firePropertyChange(CHANGE_PROPERTY_SIMPLE_REPORT_102853, null, newValue);
    }
    
    public void clearSimpleReports()
    {
    	this.simpleReport102853.clear();
    }
    
    public void clearDetailedReports()
    {
    	this.validation102853Report.clear();
    }

	public String getSignedFileString() {
		if (this.signedFiles.size() == 0) {
			return null;
		}
		else
		{
			return StringUtils.abbreviate(Arrays.toString(getSignedFilenames()),
					50);

		}
	}
}


