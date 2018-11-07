/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pteidlib;

import java.util.logging.Level;
import java.util.logging.Logger;
import pt.gov.cartaodecidadao.PTEID_EId;

/**
 *
 * @author ruim
 */
public class PTEID_ID {
    
    public short version;
    public String deliveryEntity;
    public String country;
    public String documentType;
    public String cardNumber;
    public String cardNumberPAN;
    public String cardVersion;
    public String deliveryDate;
    public String locale;
    public String validityDate;
    public String name;
    public String firstname;
    public String sex;
    public String nationality;
    public String birthDate;
    public String height;
    public String numBI;
    public String nameFather;
    public String firstnameFather;
    public String nameMother;
    public String firstnameMother;
    public String numNIF;
    public String numSS;
    public String numSNS;
    public String notes;
    public String mrz1;
    public String mrz2;
    public String mrz3;
    
    protected PTEID_ID(PTEID_EId id){
        try {
            version = 0;
            deliveryEntity = id.getIssuingEntity();
            country = id.getCountry();
            documentType = id.getDocumentType();
            cardNumber = id.getDocumentNumber();
            cardNumberPAN = id.getDocumentPAN();
            cardVersion = id.getDocumentVersion();
            deliveryDate = id.getValidityBeginDate();
            locale = id.getLocalofRequest();
            validityDate = id.getValidityEndDate();
            name = id.getSurname();
            firstname = id.getGivenName();
            sex = id.getGender();
            nationality = id.getNationality();
            birthDate = id.getDateOfBirth();
            height = id.getHeight();
            numBI = id.getCivilianIdNumber();
            nameFather = id.getSurnameFather();
            firstnameFather = id.getGivenNameFather();
            nameMother = id.getSurnameMother();
            firstnameMother = id.getGivenNameMother();
            numNIF = id.getTaxNo();
            numSS = id.getSocialSecurityNumber();
            numSNS = id.getHealthNumber();
            notes = id.getAccidentalIndications();
            mrz1 = id.getMRZ1();
            mrz2 = id.getMRZ2();
            mrz3 = id.getMRZ3();
        } catch (Exception ex) {
            Logger.getLogger(PTEID_ID.class.getName()).log(Level.SEVERE, null, ex);
        }
        
    }
    
}
