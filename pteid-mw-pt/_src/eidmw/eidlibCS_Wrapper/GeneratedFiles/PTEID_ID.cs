using System;
using System.Text;

namespace pt.portugal.eid
{
    public class PTEID_ID {
    
    public short version { get; private set; }
    public String deliveryEntity { get; private set; }
    public String country { get; private set; }
    public String documentType { get; private set; }
    public String cardNumber { get; private set; }
    public String cardNumberPAN { get; private set; }
    public String cardVersion { get; private set; }
    public String deliveryDate { get; private set; }
    public String locale { get; private set; }
    public String validityDate { get; private set; }
    public String name { get; private set; }
    public String firstname { get; private set; }
    public String sex { get; private set; }
    public String nationality { get; private set; }
    public String birthDate { get; private set; }
    public String height { get; private set; }
    public String numBI { get; private set; }
    public String nameFather { get; private set; }
    public String firstnameFather { get; private set; }
    public String nameMother { get; private set; }
    public String firstnameMother { get; private set; }
    public String numNIF { get; private set; }
    public String numSS { get; private set; }
    public String numSNS { get; private set; }
    public String notes { get; private set; }
    public String mrz1 { get; private set; }
    public String mrz2 { get; private set; }
    public String mrz3 { get; private set; }

    internal PTEID_ID(PTEID_EId id)
    {
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
        } catch (Exception) {
            throw new PteidException();
        }
    }
}

}
