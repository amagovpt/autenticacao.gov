package eu.europa.ec.markt.dss.applet.view.validation;

import com.jgoodies.forms.builder.PanelBuilder;
import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;

import eu.europa.ec.markt.dss.applet.model.ValidationModel;
import eu.europa.ec.markt.dss.applet.util.ComponentFactory;
import eu.europa.ec.markt.dss.applet.util.ResourceUtils;
import eu.europa.ec.markt.dss.applet.wizard.validation.ValidationWizardController;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.ApplicationCore;
import eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardView;

import java.util.Locale;
import java.util.logging.Level;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.beans.PropertyChangeEvent;
import java.io.File;

/**
 * TODO
 * <p/>
 * <p/>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 */
public class ValidationView extends WizardView<ValidationModel, ValidationWizardController> {

//    private static final boolean DISPLAY_LEGACY_VALIDATION = false;

    /**
     * TODO
     * <p/>
     * <p/>
     * DISCLAIMER: Project owner DG-MARKT.
     *
     * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
     * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
     */
    private class ClearEventListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(final ActionEvent e) {
            final ValidationModel model = getModel();
            model.setOriginalFile(null);
            model.setSignedFile(null);
            model.clearSimpleReports();
            model.clearDetailedReports();
        }
    }

    /**
     * TODO
     * <p/>
     * <p/>
     * DISCLAIMER: Project owner DG-MARKT.
     *
     * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
     * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
     */
    private class SelectFileAEventListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
        @Override
        public void actionPerformed(final ActionEvent e) {
        	
//        	FileDialog chooser =  new FileDialog(new Frame(), ResourceUtils.getI18n("LOAD_SIGNATURE"), FileDialog.LOAD);
//        	chooser.setDirectory(System.getProperty("user.home"));
//
//        	chooser.setVisible(true);
//        	String file = chooser.getFile();
//        	if (file != null)
//        	{
//        		String complete_path = chooser.getDirectory()+java.io.File.separatorChar+file;
//        		getModel().setSignedFile(new java.io.File(complete_path));
//        	}
        	
        	JFileChooser chooser = new JFileChooser(System.getProperty("user.home"));
        	FileNameExtensionFilter filter = new FileNameExtensionFilter(
        	        "Signature Files", "ccsigned", "pdf");
        	    chooser.setFileFilter(filter);
        	    chooser.setMultiSelectionEnabled(true);
        	    
        	    int returnVal = chooser.showDialog(getCore(), I18N_PARAM_APPROVE_LABEL);
        	    if (returnVal == JFileChooser.APPROVE_OPTION) {
        	    	File[] selected_files = chooser.getSelectedFiles();
                	
                	for (File f: selected_files)
                	{

                		getModel().setSignedFile(f);
                	}
        	    }
        
        }
    }
   

    /**
     * TODO
     * <p/>
     * <p/>
     * DISCLAIMER: Project owner DG-MARKT.
     *
     * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
     * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
     */
    private class SelectFileBEventListener implements ActionListener {
        /*
         * (non-Javadoc)
         * 
         * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
         */
    	
        @Override
        public void actionPerformed(final ActionEvent e) {
        	FileDialog chooser =  new FileDialog(new Frame(), ResourceUtils.getI18n("LOAD_SIGNED_FILE"), FileDialog.LOAD);
        	chooser.setVisible(true);
        	String file = chooser.getFile();
        	if (file != null)
        	{
        		getModel().setOriginalFile(new java.io.File(file));
        	}
        }
    }
    
    private class CheckBoxListener implements ItemListener
    {

		@Override
		public void itemStateChanged(ItemEvent e) {
			Object source = e.getItemSelectable();
			
//			if (source == param_qualified)
//				getModel().setQualifiedCert(e.getStateChange() == ItemEvent.SELECTED);
			if (source == param_timestamp)
				getModel().setTimestamp(e.getStateChange() == ItemEvent.SELECTED);
			else if (source == param_ltv)
				getModel().setLong_term_validation(e.getStateChange() == ItemEvent.SELECTED);
			
		}
    	
    }
    
    private static final String I18N_MORE_OPTIONS = ResourceUtils.getI18n("MORE_OPTIONS");

    private static final String I18N_NO_FILE_SELECTED = ResourceUtils.getI18n("NO_FILE_SELECTED");
    private static final String I18N_BROWSE_SIGNED = ResourceUtils.getI18n("BROWSE_SIGNED");
    private static final String I18N_BROWSE_ORIGINAL = ResourceUtils.getI18n("BROWSE_ORIGINAL");
    private static final String I18N_SIGNED_FILE_TO_VALIDATE = ResourceUtils.getI18n("SIGNED_FILE_TO_VALIDATE");
    private static final String I18N_ORIGINAL_FILE = ResourceUtils.getI18n("ORIGINAL_FILE") + " " + ResourceUtils.getI18n("ONLY_IF_DETACHED");
//    private static final String I18N_PARAM_QUALIFIED = ResourceUtils.getI18n("PARAM_QUALIFIED");
//    private static final String I18N_PARAM_QUALIFIED_TOOLTIP = ResourceUtils.getI18n("PARAM_QUALIFIED_TOOLTIP");
    private static final String I18N_PARAM_TIMESTAMP = ResourceUtils.getI18n("PARAM_TIMESTAMP");
    private static final String I18N_PARAM_TIMESTAMP_TOOLTIP = ResourceUtils.getI18n("PARAM_TIMESTAMP_TOOLTIP");
    private static final String I18N_PARAM_LTV = ResourceUtils.getI18n("PARAM_LTV");
    private static final String I18N_PARAM_LTV_TOOLTIP = ResourceUtils.getI18n("PARAM_LTV_TOOLTIP");
    private static final String I18N_PARAM_APPROVE_LABEL = ResourceUtils.getI18n("SELECT_FILES");
    

    private final JLabel fileB;

    private final JLabel fileA;
    private final JLabel fileBLabel;

    private final JButton selectFileA;

    private final JButton selectFileB;
    private final JButton clear;
    private final JButton moreOptions;
    
    private final JCheckBox param_timestamp, param_ltv;

//    private final JRadioButton validationLegacy;
//    private final JRadioButton validation102853;

    private String I18N_VALIDATION_PARAMETERS = ResourceUtils.getI18n("VALIDATION_PARAMETERS");

//    private final JRadioButton defaultValidation102853;
//    private final JRadioButton customValidation102853;
//
//    private final JLabel filePolicyLabel;
//    private final JButton selectFilePolicy;

    /**
     * The default constructor for ValidationView.
     *
     * @param core
     * @param controller
     * @param model
     */
    public ValidationView(final ApplicationCore core, final ValidationWizardController controller, final ValidationModel model) {
        super(core, controller, model);

        selectFileA = ComponentFactory.createFileChooser(I18N_BROWSE_SIGNED, true, new SelectFileAEventListener());
        selectFileA.setName("fileA");
        selectFileB = ComponentFactory.createFileChooser(I18N_BROWSE_ORIGINAL, true, new SelectFileBEventListener());
        selectFileB.setName("fileB");
        clear = ComponentFactory.createClearButton(true, new ClearEventListener());
        clear.setName("clear");
        
        moreOptions = ComponentFactory.createClearButton(true, new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				selectFileB.setVisible(!selectFileB.isVisible());
				fileB.setVisible(!fileB.isVisible());
				fileBLabel.setVisible(!fileBLabel.isVisible());
				
			}
		});
        moreOptions.setText(I18N_MORE_OPTIONS);
        fileA = ComponentFactory.createLabel(I18N_NO_FILE_SELECTED);
        fileB = ComponentFactory.createLabel(I18N_NO_FILE_SELECTED);
        fileBLabel = ComponentFactory.createLabel(I18N_ORIGINAL_FILE);
        selectFileB.setVisible(false);
        fileB.setVisible(false);
        
        Font font = fileBLabel.getFont();
        // same font but bold
        Font boldFont = new Font(font.getFontName(), Font.BOLD, font.getSize());
        fileBLabel.setFont(boldFont);
        fileBLabel.setVisible(false);
        
//        param_qualified = ComponentFactory.createCheckBox(true, I18N_PARAM_QUALIFIED, I18N_PARAM_QUALIFIED_TOOLTIP);
//        param_qualified.addItemListener(new CheckBoxListener());
        param_timestamp = ComponentFactory.createCheckBox(true, I18N_PARAM_TIMESTAMP, I18N_PARAM_TIMESTAMP_TOOLTIP);
        param_timestamp.addItemListener(new CheckBoxListener());
        param_ltv = ComponentFactory.createCheckBox(true, I18N_PARAM_LTV, I18N_PARAM_LTV_TOOLTIP);
        param_ltv.addItemListener(new CheckBoxListener());

    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#doInit()
     */
    @Override
    public void doInit() {

        final ValidationModel model = getModel();
        fileA.setText(model.getSignedFileString() != null ? model.getSignedFileString(): I18N_NO_FILE_SELECTED);
        fileB.setText(model.getOriginalFile() != null ? model.getOriginalFile().getName() : I18N_NO_FILE_SELECTED);
    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.view.DSSAppletView#doLayout()
     */
    @Override
    protected Container doLayout() {
    	
        final FormLayout layout = new FormLayout("5dlu, pref, 5dlu, pref, 5dlu, pref:grow, 5dlu", "5dlu, p, 5dlu, p, 5dlu, pref, 5dlu, p, 5dlu, pref, 5dlu, pref, 5dlu, pref, 5dlu, pref, 5dlu, pref, 5dlu, pref, 5dlu, pref");

        final CellConstraints cc = new CellConstraints();
        final PanelBuilder builder = ComponentFactory.createBuilder(layout);
        int row = 2;
     
        /*

        ComboBoxModel<String> lang_model = new DefaultComboBoxModel( new String[]{"en", "pt"});
        JComboBox combo = ComponentFactory.combo(lang_model, 
        		new ActionListener() {
     
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                String l = (String)cb.getSelectedItem();
                System.out.println("Selected language: "+l);
                Locale.setDefault(new Locale(l));
                moreOptions.setText(ResourceUtils.getLocalizedString("MORE_OPTIONS", l));
                selectFileA.setText(ResourceUtils.getLocalizedString("BROWSE_SIGNED", l));
                selectFileB.setText(ResourceUtils.getLocalizedString("BROWSE_ORIGINAL", l));
                clear.setText(ResourceUtils.getLocalizedString("CLEAR", l));
                
                param_timestamp.setText(ResourceUtils.getLocalizedString("PARAM_TIMESTAMP", l));
                param_ltv.setText(ResourceUtils.getLocalizedString("PARAM_LTV", l));
                
                
            }
        });
        builder.add(new JLabel("Choose your language"),  cc.xy(2, row));
        builder.add(combo, cc.xy(4, row));
        
        */
        
        builder.addSeparator(I18N_SIGNED_FILE_TO_VALIDATE, cc.xyw(2, row=row+2, 6));
        builder.add(selectFileA, cc.xy(2, row=row+2));
        builder.add(fileA, cc.xy(4, row));
        builder.add(clear, cc.xy(2, row=row+2));
        builder.addSeparator(I18N_VALIDATION_PARAMETERS, cc.xyw(2, row=row+2, 6));
//        builder.add(param_qualified, cc.xy(2, row=row+2));
        builder.add(param_timestamp, cc.xy(2, row=row+2));
        builder.add(param_ltv, cc.xy(2, row=row+2));
        builder.add(moreOptions, cc.xy(2, row=row+2));
        
       // builder.addSeparator(I18N_ORIGINAL_FILE, cc.xyw(2, row=row+2, 6));
        builder.add(selectFileB, cc.xy(2, row=row+2));
        builder.add(fileB, cc.xy(4, row));
        builder.add(fileBLabel, cc.xy(2, row=row+2));
        
        

        return ComponentFactory.createPanel(builder);

    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * eu.europa.ec.markt.dss.commons.swing.mvc.applet.wizard.WizardView#wizardModelChange(java.beans.PropertyChangeEvent
     * )
     */
    @Override
    public void wizardModelChange(final PropertyChangeEvent evt) {
        final ValidationModel model = getModel();

        if (evt.getPropertyName().equals(ValidationModel.CHANGE_PROPERTY_ORIGINAL_FILE)) {

            if (model.getOriginalFile() == null) {
                fileB.setText(I18N_NO_FILE_SELECTED);
            } else {
                fileB.setText(model.getOriginalFile().getName());
            }
            return;
        }

        if (evt.getPropertyName().equals(ValidationModel.CHANGE_PROPERTY_SIGNED_FILE)) {
        	String signedFiles = model.getSignedFileString();
            if (signedFiles == null) {
                fileA.setText(I18N_NO_FILE_SELECTED);
            } else {
                fileA.setText(signedFiles);
            }
            return;
        }


    }

}
