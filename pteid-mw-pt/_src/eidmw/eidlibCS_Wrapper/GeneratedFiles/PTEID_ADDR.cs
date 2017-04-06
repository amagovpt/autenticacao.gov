using System;
using System.Text;
using pt.portugal.eid;

namespace eidpt
{
    public class PteidAddr {

    private short Version;
    private String AddrType;
    private String Country;
    private String District;
    private String DistrictDesc;
    private String Municipality;
    private String MunicipalityDesc;
    private String Freguesia;
    private String FreguesiaDesc;
    private String StreettypeAbbr;
    private String Streettype;
    private String Street;
    private String BuildingAbbr;
    private String Building;
    private String Door;
    private String Floor;
    private String Side;
    private String Place;
    private String Locality;
    private String Cp4;
    private String Cp3;
    private String Postal;
    private String NumMor;
    private String CountryDescF;
    private String AddressF;
    private String CityF;
    private String RegioF;
    private String LocalityF;
    private String PostalF;
    private String NumMorF;

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
    public string addrType
    {
        get
        {
            return AddrType;
        }
        private set
        {
            AddrType = value;
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
    public string district
    {
        get
        {
            return District;
        }
        private set
        {
            District = value;
        }
    }
    public string districtDesc
    {
        get
        {
            return DistrictDesc;
        }
        private set
        {
            DistrictDesc = value;
        }
    }
    public string municipality
    {
        get
        {
            return Municipality;
        }
        private set
        {
            Municipality = value;
        }
    }
    public string municipalityDesc
    {
        get
        {
            return MunicipalityDesc;
        }
        private set
        {
            MunicipalityDesc = value;
        }
    }
    public string freguesia
    {
        get
        {
            return Freguesia;
        }
        private set
        {
            Freguesia = value;
        }
    }
    public string freguesiaDesc
    {
        get
        {
            return FreguesiaDesc;
        }
        private set
        {
            FreguesiaDesc = value;
        }
    }
    public string streettypeAbbr
    {
        get
        {
            return StreettypeAbbr;
        }
        private set
        {
            StreettypeAbbr = value;
        }
    }
    public string streettype
    {
        get
        {
            return Streettype;
        }
        private set
        {
            Streettype = value;
        }
    }
    public string street
    {
        get
        {
            return Street;
        }
        private set
        {
            Street = value;
        }
    }
    public string buildingAbbr
    {
        get
        {
            return BuildingAbbr;
        }
        private set
        {
            BuildingAbbr = value;
        }
    }
    public string building
    {
        get
        {
            return Building;
        }
        private set
        {
            Building = value;
        }
    }
    public string door
    {
        get
        {
            return Door;
        }
        private set
        {
            Door = value;
        }
    }
    public string floor
    {
        get
        {
            return Floor;
        }
        private set
        {
            Floor = value;
        }
    }
    public string side
    {
        get
        {
            return Side;
        }
        private set
        {
            Side = value;
        }
    }
    public string place
    {
        get
        {
            return Place;
        }
        private set
        {
            Place = value;
        }
    }
    public string locality
    {
        get
        {
            return Locality;
        }
        private set
        {
            Locality = value;
        }
    }
    public string cp4
    {
        get
        {
            return Cp4;
        }
        private set
        {
            Cp4 = value;
        }
    }
    public string cp3
    {
        get
        {
            return Cp3;
        }
        private set
        {
            Cp3 = value;
        }
    }
    public string postal
    {
        get
        {
            return Postal;
        }
        private set
        {
            Postal = value;
        }
    }
    public string numMor
    {
        get
        {
            return NumMor;
        }
        private set
        {
            NumMor = value;
        }
    }
    public string countryDescF
    {
        get
        {
            return CountryDescF;
        }
        private set
        {
            CountryDescF = value;
        }
    }
    public string addressF
    {
        get
        {
            return AddressF;
        }
        private set
        {
            AddressF = value;
        }
    }
    public string cityF
    {
        get
        {
            return CityF;
        }
        private set
        {
            CityF = value;
        }
    }
    public string regioF
    {
        get
        {
            return RegioF;
        }
        private set
        {
            RegioF = value;
        }
    }
    public string localityF
    {
        get
        {
            return LocalityF;
        }
        private set
        {
            LocalityF = value;
        }
    }
    public string postalF
    {
        get
        {
            return PostalF;
        }
        private set
        {
            PostalF = value;
        }
    }
    public string numMorF
    {
        get
        {
            return NumMorF;
        }
        private set
        {
            NumMorF = value;
        }
    }

    internal PteidAddr(PTEID_Address addr)
    {
        try {
            version = 0;
            addrType = addr.isNationalAddress() ? "N" : "I";
            country = addr.getCountryCode();
        if (addr.isNationalAddress()) {
            district = addr.getDistrictCode();
            districtDesc = addr.getDistrict();
            municipality = addr.getMunicipalityCode();
            municipalityDesc = addr.getMunicipality();
            freguesia = addr.getCivilParishCode();
            freguesiaDesc = addr.getCivilParish();
            streettypeAbbr = addr.getAbbrStreetType();
            streettype = addr.getStreetType();
            street = addr.getStreetName();
            buildingAbbr = addr.getAbbrBuildingType();
            building = addr.getBuildingType();
            door = addr.getDoorNo();
            floor = addr.getFloor();
            side = addr.getSide();
            place = addr.getPlace();
            locality = addr.getLocality();
            cp4 = addr.getZip4();
            cp3 = addr.getZip3();
            postal = addr.getPostalLocality();
            numMor = addr.getGeneratedAddressCode();
        } else {
            countryDescF = addr.getForeignCountry();
            addressF = addr.getForeignAddress();
            cityF = addr.getForeignCity();
            regioF = addr.getForeignRegion();
            localityF = addr.getForeignLocality();
            postalF = addr.getForeignPostalCode();
            numMorF = addr.getGeneratedAddressCode();
        }
        } catch (PTEID_Exception) {
            throw new PteidException(0);
        }
    }
}

}
