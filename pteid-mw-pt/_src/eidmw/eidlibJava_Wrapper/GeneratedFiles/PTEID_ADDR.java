/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pt.gov.cartaodecidadao;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ruim
 */
public class PTEID_ADDR {

    public short version;
    public String addrType = "";
    public String country = "";
    public String district = "";
    public String districtDesc = "";
    public String municipality = "";
    public String municipalityDesc = "";
    public String freguesia = "";
    public String freguesiaDesc = "";
    public String streettypeAbbr = "";
    public String streettype = "";
    public String street = "";
    public String buildingAbbr = "";
    public String building = "";
    public String door = "";
    public String floor = "";
    public String side = "";
    public String place = "";
    public String locality = "";
    public String cp4 = "";
    public String cp3 = "";
    public String postal = "";
    public String numMor = "";
    public String countryDescF = "";
    public String addressF = "";
    public String cityF = "";
    public String regioF = "";
    public String localityF = "";
    public String postalF = "";
    public String numMorF = "";

    protected PTEID_ADDR(PTEID_Address addr){
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
        } catch (PTEID_Exception ex) {
            Logger.getLogger(PTEID_ADDR.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
