/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package pteidlib;

import java.util.logging.Level;
import java.util.logging.Logger;
import pt.gov.cartaodecidadao.pteidlibJava_WrapperConstants;
import pt.gov.cartaodecidadao.PTEID_Address;
import pt.gov.cartaodecidadao.PTEID_Exception;
import pt.gov.cartaodecidadao.PTEID_ByteArray;

/**
 *
 * @author ruim
 */
public class PTEID_ADDR implements pteidlibJava_WrapperConstants {

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
    PTEID_ByteArray m_data;

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

    protected PTEID_ADDR( byte[] addressRaw ){
        try {
            m_data = new PTEID_ByteArray( addressRaw, addressRaw.length );

            version = 0;
            addrType = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_TYPE
                                         , PTEIDNG_FIELD_ADDRESS_LEN_TYPE );
            boolean isNationalAddress = ( addrType == "N" );

            country = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_COUNTRY
                                        , PTEIDNG_FIELD_ADDRESS_LEN_COUNTRY );

            if ( isNationalAddress ){
                getAddressFields();
            } else{
                getForeignerAddressFields();
            }/* if ( isNationalAddress ) */

        } catch (Exception ex) {
            System.err.println("Error in CVC_GetAddr: " + ex.getMessage());
            Logger.getLogger(PTEID_ADDR.class.getName()).log(Level.SEVERE, null, ex);
        }
    }/* PTEID_ADDR() */

    protected void getAddressFields(){
        //District Code
        district = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_DISTRICT
                                     , PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT );

        //District Description
        districtDesc = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_DISTRICT_DESCRIPTION
                                         , PTEIDNG_FIELD_ADDRESS_LEN_DISTRICT_DESCRIPTION );

        //Municipality Code
        municipality = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY
                                         , PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY );

        //Municipality Description
        municipalityDesc = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_MUNICIPALITY_DESCRIPTION
                                             , PTEIDNG_FIELD_ADDRESS_LEN_MUNICIPALITY_DESCRIPTION );

        //CivilParish Code
        freguesia = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH
                                      , PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH );

        //CivilParish Description
        freguesiaDesc = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_CIVILPARISH_DESCRIPTION
                                          , PTEIDNG_FIELD_ADDRESS_LEN_CIVILPARISH_DESCRIPTION );

        //Abbreviated Street Type
        streettypeAbbr = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_ABBR_STREET_TYPE
                                           , PTEIDNG_FIELD_ADDRESS_LEN_ABBR_STREET_TYPE );

        //Street Type
        streettype = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_STREET_TYPE
                                       , PTEIDNG_FIELD_ADDRESS_LEN_STREET_TYPE );

        //Street Name
        street = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_STREETNAME
                                   , PTEIDNG_FIELD_ADDRESS_LEN_STREETNAME );

        //Abbreviated Building Type
        buildingAbbr = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_ABBR_BUILDING_TYPE
                                         , PTEIDNG_FIELD_ADDRESS_LEN_ABBR_BUILDING_TYPE );

        //Building Type
        building = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_BUILDING_TYPE
                                     , PTEIDNG_FIELD_ADDRESS_LEN_BUILDING_TYPE );

        //DoorNo
        door = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_DOORNO
                                 , PTEIDNG_FIELD_ADDRESS_LEN_DOORNO );

        //Floor
        floor = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_FLOOR
                                  , PTEIDNG_FIELD_ADDRESS_LEN_FLOOR );

        //Side
        side = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_SIDE
                                 , PTEIDNG_FIELD_ADDRESS_LEN_SIDE );

        //Place
        place = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_PLACE
                                  , PTEIDNG_FIELD_ADDRESS_LEN_PLACE );

        //Locality
        locality = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_LOCALITY
                                     , PTEIDNG_FIELD_ADDRESS_LEN_LOCALITY );

        //Zip4
        cp4 = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_ZIP4
                                , PTEIDNG_FIELD_ADDRESS_LEN_ZIP4 );

        //Zip3
        cp3 = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_ZIP3
                                , PTEIDNG_FIELD_ADDRESS_LEN_ZIP3 );

        //Postal Locality
        postal = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_POSTALLOCALITY
                                   , PTEIDNG_FIELD_ADDRESS_LEN_POSTALLOCALITY );

        //Generated Address Code
        numMor = m_data.GetStringAt( PTEIDNG_FIELD_ADDRESS_POS_GENADDRESS_NUM
                                   , PTEIDNG_FIELD_ADDRESS_LEN_GENADDRESS_NUM );
    }/* AddressFields() */

    protected void getForeignerAddressFields(){
        //Foreign Country
        //Generated Address Code
        countryDescF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_COUNTRY_DESCRIPTION
                                         , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_COUNTRY_DESCRIPTION );

        //Foreign Generic Address
        addressF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_ADDRESS
                                     , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_ADDRESS );

        //Foreign City
        cityF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_CITY
                                  , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_CITY );

        //Foreign Region
        regioF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_REGION
                                   , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_REGION );

        //Foreign Locality
        localityF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_LOCALITY
                                      , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_LOCALITY );

        //Foreign Postal Code
        postalF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_POSTAL_CODE
                                    , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_POSTAL_CODE );

        //Foreign Generated Address Code
        numMorF = m_data.GetStringAt( PTEIDNG_FIELD_FOREIGN_ADDRESS_POS_GENADDRESS_NUM
                                    , PTEIDNG_FIELD_FOREIGN_ADDRESS_LEN_GENADDRESS_NUM );
    }/* ForeignerAddressFields() */
}
