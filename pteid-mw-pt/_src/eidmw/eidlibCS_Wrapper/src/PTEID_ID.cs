using System;
using System.Text;
using pt.portugal.eid;

namespace eidpt
{
    public class PteidId {
    
    private short Version;
    private String DeliveryEntity;
    private String Country;
    private String DocumentType;
    private String CardNumber;
    private String CardNumberPAN;
    private String CardVersion;
    private String DeliveryDate;
    private String Locale;
    private String ValidityDate;
    private String Name;
    private String Firstname;
    private String Sex;
    private String Nationality;
    private String BirthDate;
    private String Height;
    private String NumBI;
    private String NameFather;
    private String FirstnameFather;
    private String NameMother;
    private String FirstnameMother;
    private String NumNIF;
    private String NumSS;
    private String NumSNS;
    private String Notes;
    private String Mrz1;
    private String Mrz2;
    private String Mrz3;

    public short version
    {
        get
        {
            return Version;
        }
        private set
        {
            Version = value;
        }
    }
    public string deliveryEntity
    {
        get
        {
            return DeliveryEntity;
        }
        private set
        {
            DeliveryEntity = value;
        }
    }
    public string country
    {
        get
        {
            return Country;
        }
        private set
        {
            Country = value;
        }
    }
    public string documentType
    {
        get
        {
            return DocumentType;
        }
        private set
        {
            DocumentType = value;
        }
    }
    public string cardNumber
    {
        get
        {
            return CardNumber;
        }
        private set
        {
            CardNumber = value;
        }
    }
    public string cardNumberPAN
    {
        get
        {
            return CardNumberPAN;
        }
        private set
        {
            CardNumberPAN = value;
        }
    }
    public string cardVersion
    {
        get
        {
            return CardVersion;
        }
        private set
        {
            CardVersion = value;
        }
    }
    public string deliveryDate
    {
        get
        {
            return DeliveryDate;
        }
        private set
        {
            DeliveryDate = value;
        }
    }
    public string locale
    {
        get
        {
            return Locale;
        }
        private set
        {
            Locale = value;
        }
    }
    public string validityDate
    {
        get
        {
            return ValidityDate;
        }
        private set
        {
            ValidityDate = value;
        }
    }
    public string name
    {
        get
        {
            return Name;
        }
        private set
        {
            Name = value;
        }
    }
    public string firstname
    {
        get
        {
            return Firstname;
        }
        private set
        {
            Firstname = value;
        }
    }
    public string sex
    {
        get
        {
            return Sex;
        }
        private set
        {
            Sex = value;
        }
    }
    public string nationality
    {
        get
        {
            return Nationality;
        }
        private set
        {
            Nationality = value;
        }
    }
    public string birthDate
    {
        get
        {
            return BirthDate;
        }
        private set
        {
            BirthDate = value;
        }
    }
    public string height
    {
        get
        {
            return Height;
        }
        private set
        {
            Height = value;
        }
    }
    public string numBI
    {
        get
        {
            return NumBI;
        }
        private set
        {
            NumBI = value;
        }
    }
    public string nameFather
    {
        get
        {
            return NameFather;
        }
        private set
        {
            NameFather = value;
        }
    }
    public string firstnameFather
    {
        get
        {
            return FirstnameFather;
        }
        private set
        {
            FirstnameFather = value;
        }
    }
    public string nameMother
    {
        get
        {
            return NameMother;
        }
        private set
        {
            NameMother = value;
        }
    }
    public string firstnameMother
    {
        get
        {
            return FirstnameMother;
        }
        private set
        {
            FirstnameMother = value;
        }
    }
    public string numNIF
    {
        get
        {
            return NumNIF;
        }
        private set
        {
            NumNIF = value;
        }
    }
    public string numSS
    {
        get
        {
            return NumSS;
        }
        private set
        {
            NumSS = value;
        }
    }
    public string numSNS
    {
        get
        {
            return NumSNS;
        }
        private set
        {
            NumSNS = value;
        }
    }
    public string notes
    {
        get
        {
            return Notes;
        }
        private set
        {
            Notes = value;
        }
    }
    public string mrz1
    {
        get
        {
            return Mrz1;
        }
        private set
        {
            Mrz1 = value;
        }
    }
    public string mrz2
    {
        get
        {
            return Mrz2;
        }
        private set
        {
            Mrz2 = value;
        }
    }
    public string mrz3
    {
        get
        {
            return Mrz3;
        }
        private set
        {
            Mrz3 = value;
        }
    }

    internal PteidId(PTEID_EId id)
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
            throw new PteidException(0);
        }
    }
}

}
