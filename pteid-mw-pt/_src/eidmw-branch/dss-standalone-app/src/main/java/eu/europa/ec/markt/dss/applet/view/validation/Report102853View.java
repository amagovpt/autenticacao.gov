/*
 * DSS - Digital Signature Services
 *
 * Copyright (C) 2013 European Commission, Directorate-General Internal Market and Services (DG MARKT), B-1049 Bruxelles/Brussel
 *
 * Developed by: 2013 ARHS Developments S.A. (rue Nicolas Bové 2B, L-1253 Luxembourg) http://www.arhs-developments.com
 *
 * This file is part of the "DSS - Digital Signature Services" project.
 *
 * "DSS - Digital Signature Services" is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * DSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * "DSS - Digital Signature Services".  If not, see <http://www.gnu.org/licenses/>.
 */

package eu.europa.ec.markt.dss.applet.view.validation;

import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.swing.JFileChooser;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;

import org.apache.commons.io.FilenameUtils;
import org.apache.commons.io.output.ByteArrayOutputStream;
import org.w3c.dom.Document;
import org.xhtmlrenderer.layout.SharedContext;
import org.xhtmlrenderer.pdf.ITextRenderer;
import org.xhtmlrenderer.simple.FSScrollPane;
import org.xhtmlrenderer.simple.XHTMLPanel;
import org.xhtmlrenderer.swing.SelectionHighlighter;
import org.xhtmlrenderer.swing.SelectionHighlighter.CopyAction;

import com.jgoodies.forms.builder.PanelBuilder;
import com.jgoodies.forms.layout.CellConstraints;
import com.lowagie.text.DocumentException;

import eu.europa.ec.markt.dss.DSSXMLUtils;
import eu.europa.ec.markt.dss.applet.model.ValidationModel;
import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;
import eu.europa.ec.markt.dss.applet.wizard.validation.ValidationWizardController;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardView;

/**
 * TODO
 * <p/>
 * <p/>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 */
public class Report102853View extends WizardView<ValidationModel, ValidationWizardController> {

//    private final JTextArea detailedReportText;

//    private JTree diagnostic;
//    private final JTextArea diagnosticText;

//    private final ValueHolder detailledReportValueHolder;
//    private final ValueHolder diagnositcValueHolder;

    private final XHTMLPanel simpleReportHtmlPanel;
    private final FSScrollPane simpleReportScrollPane;

//    private final XHTMLPanel detailedReportHtmlPanel;
//    private final FSScrollPane detailedReportScrollPane;
    
    private List<Document> detailedReport;
    private ValidationModel my_validation_model;
  

    /**
     * The default constructor for Report102853View.
     *
     * @param core
     * @param controller
     * @param model
     */
    public Report102853View(final ApplicationCore core, final ValidationWizardController controller, final ValidationModel model) {
        super(core, controller, model);
        my_validation_model = model;
       
//        detailledReportValueHolder = new ValueHolder("");
//        diagnositcValueHolder = new ValueHolder("");
//
//        detailedReportText = ComponentFactory.createTextArea(detailledReportValueHolder);
//        detailedReportText.setTabSize(2);

//        diagnostic = ComponentFactory.tree("Diagnostic", new DefaultTreeModel(null));
//        diagnosticText = ComponentFactory.createTextArea(diagnositcValueHolder);
//        diagnosticText.setTabSize(2);

        simpleReportHtmlPanel = new XHTMLPanel();
        final SelectionHighlighter caret = new SelectionHighlighter();
        caret.install(simpleReportHtmlPanel);
        CopyAction copyAction = new SelectionHighlighter.CopyAction();
        copyAction.install(caret);
        
        simpleReportScrollPane = new FSScrollPane(simpleReportHtmlPanel);
        SharedContext context = simpleReportHtmlPanel.getSharedContext();
        context.getTextRenderer().setSmoothingThreshold(8.0f);

//        detailedReportHtmlPanel = new XHTMLPanel();
//        detailedReportScrollPane = new FSScrollPane(detailedReportHtmlPanel);
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#doInit()
     */

    @Override
    public void doInit() {
//        final ValidationModel model = getModel();

//        final List<XmlDom> validation102853Report = model.getValidation102853Report();
//        final String reportText = validation102853Report.toString();
//        detailledReportValueHolder.setValue(reportText);

//        final DiagnosticData diagnosticData102853 = model.getDiagnosticData102853();
//        final Document document = ValidationResourceManager.convert(diagnosticData102853);
//        final XMLTreeModel xmlTreeModelDiagnostic = new XMLTreeModel();
//        xmlTreeModelDiagnostic.setDocument(document.getDocumentElement());
//        diagnostic = ComponentFactory.tree("Diagnostic", xmlTreeModelDiagnostic);
//        expandTree(diagnostic);
//
//        diagnositcValueHolder.setValue(new XmlDom(document).toString());
        
        /* SIMPLE REPORT */
        
        final Document simpleReportHtml = getController().renderSimpleReportAsHtml();
//        System.err.println("[DEBUG] SimpleReport HTML: ");
//        DSSXMLUtils.printDocument(simpleReportHtml.getFirstChild(), System.err);
        simpleReportHtmlPanel.setDocument(simpleReportHtml);

        /* DETAILED REPORT */	
        final List<Document> detailedReportsHtml = getController().renderValidationReportAsHtml();
        detailedReport = detailedReportsHtml;
        //detailedReportHtmlPanel.setDocument(detailedReportHtml);
    }

    /*
     * (non-Javadoc)
     *
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#doLayout()
     */
    @Override
    protected Container doLayout() {

        JTabbedPane tabbedPane = new JTabbedPane(SwingConstants.TOP);
        String simple_report_title = ResourceUtils.getI18n("SIMPLE_REPORT");
        tabbedPane.addTab(simple_report_title, getHtmlPanel(simple_report_title, simpleReportScrollPane, simpleReportHtmlPanel));
//        tabbedPane.addTab("Detailed Report", getHtmlPanel("Detailed Report", detailedReportScrollPane, detailedReportHtmlPanel));
//        tabbedPane.addTab("Detailled Report Tree", getDetailledReportPanel());
        // tabbedPane.addTab("Detailled Report XML", getDetailledReportText());
        //tabbedPane.addTab("Diagnostic Tree", getDiagnosticPanel());
        // tabbedPane.addTab("Diagnostic XML", getDiagnosticPanelText());

        return tabbedPane;

    }
    
    class SaveButtonActionListener implements ActionListener
    {
    	Document m_doc = null;
    	String suggestedFileName;
    	public SaveButtonActionListener(Document doc, String suggested_filename) {
    		m_doc = doc;
    		suggestedFileName = suggested_filename;
		}

		@Override
		public void actionPerformed(ActionEvent event) {
			final JFileChooser fileChooser = new JFileChooser();
			fileChooser.setSelectedFile(new java.io.File(suggestedFileName));
			
            int returnValue = fileChooser.showSaveDialog(simpleReportScrollPane);
            if (returnValue == JFileChooser.APPROVE_OPTION) {
                try {
                    OutputStream os = new FileOutputStream(fileChooser.getSelectedFile());
                    ITextRenderer renderer = new ITextRenderer();
                    renderer.setDocument(m_doc, "file:///");
                    renderer.layout();
                    renderer.createPDF(os);

                    os.close();
                } catch (FileNotFoundException e) {
                    throw new RuntimeException(e);
                } catch (DocumentException e) {
                    throw new RuntimeException(e);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }

            }
			
		}
    	
    }
    
    class SaveReportsAsZipListener implements ActionListener
    {
    	List <Document> m_doc = null;
    	public SaveReportsAsZipListener(List<Document> docs) {
    		m_doc = docs;
    	}
    	@Override
    	public void actionPerformed(ActionEvent event) {
    		
    		String basename = null;
    		final JFileChooser fileChooser = new JFileChooser();
    		fileChooser.setSelectedFile(new java.io.File("detailed_reports.zip"));

    		int returnValue = fileChooser.showSaveDialog(simpleReportScrollPane);
    		if (returnValue == JFileChooser.APPROVE_OPTION) {
    			
    			try {
    				ZipOutputStream zos = new ZipOutputStream(new FileOutputStream(fileChooser.getSelectedFile()));
    				
    				for (int index = 0; index != m_doc.size(); index++)
    				{
    					basename = FilenameUtils.getBaseName(getModel().getSignedFiles().get(index).getName());
    					String fileName = "Detailed_Report-" + basename + ".pdf";
    				    ZipEntry zipEntry = new ZipEntry(fileName);
    				    zos.putNextEntry(zipEntry);
    				    
    					ByteArrayOutputStream os = new ByteArrayOutputStream();
    					ITextRenderer renderer = new ITextRenderer();
    					renderer.setDocument(m_doc.get(index), "file:///");
    					renderer.layout();
    					renderer.createPDF(os);
    					
    					zos.write(os.toByteArray());
    					zos.closeEntry();

    				}
    				zos.close();
    			
    		} catch (FileNotFoundException e) {
    			throw new RuntimeException(e);
    		} catch (DocumentException e) {
    			throw new RuntimeException(e);
    		} catch (IOException e) {
    			throw new RuntimeException(e);
    		}
    	}

    }
}

    private JPanel getHtmlPanel(final String textWithMnemonic, final FSScrollPane simpleReportScrollPane, final XHTMLPanel htmlPanel) {
        final String[] columnSpecs = new String[]{"5dlu", "pref", "5dlu", "pref", "fill:default:grow", "5dlu"};
        final String[] rowSpecs = new String[]{"5dlu", "pref", "5dlu", "fill:default:grow", "5dlu", "pref", "5dlu"};
        final PanelBuilder builder = ComponentFactory.createBuilder(columnSpecs, rowSpecs);
        final CellConstraints cc = new CellConstraints();
        

        builder.addSeparator(textWithMnemonic, cc.xyw(2, 2, 3));
        builder.add(ComponentFactory.createScrollPane(simpleReportScrollPane), cc.xyw(2, 4, 4));
        
        List <File> signed_files = my_validation_model.getSignedFiles();
        
        String basename = signed_files.size() == 1 ? FilenameUtils.getBaseName(getModel().getSignedFiles().get(0).getName()) : "multiple_files";
		
        builder.add(ComponentFactory.createSaveButton(ResourceUtils.getI18n("SAVE_THIS"), true, new SaveButtonActionListener(htmlPanel.getDocument(),
        		"SimpleReport-"+basename+".pdf"))
        		, cc.xyw(2, 6, 1));
        
        if (getModel().getSignedFiles().size() > 1)
        	builder.add(ComponentFactory.createSaveButton(ResourceUtils.getI18n("SAVE_DETAILED"), true, new SaveReportsAsZipListener(detailedReport)), cc.xyw(4, 6, 1));
        else 
        	builder.add(ComponentFactory.createSaveButton(ResourceUtils.getI18n("SAVE_DETAILED"), true, new SaveButtonActionListener(detailedReport.get(0), "DetailedReport-"+basename+".pdf")), cc.xyw(4, 6, 1));

        return ComponentFactory.createPanel(builder);
    }

    /**
     * fully expand the tree
     *
     * @param tree
     
    private void expandTree(JTree tree) {
        // expand all
//        for (int i = 0; i < tree.getRowCount(); i++) {
        int i = 0;
        tree.expandRow(i);
//        }
    }
    */

}
