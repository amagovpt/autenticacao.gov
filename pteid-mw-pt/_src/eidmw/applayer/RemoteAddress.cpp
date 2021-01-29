 /* RemoteAddress helper functions - JSON parsing, object building and card interactions
 
  * Copyright (C) 2020-2021 André Guerreiro - <aguerreiro1985@gmail.com>
  */

#include "ByteArray.h"
#include "APLCardPteid.h"
#include "CardPteid.h"
#include "cJSON.h"
#include "Log.h"
#include "RemoteAddress.h"

namespace eIDMW
{

CByteArray getSodData(APL_EIDCard *card) {

    const CByteArray sod_data = card->getSod().getData();
    const unsigned char *data_p = sod_data.GetBytes();

    //Trim the padding 0x00 bytes parsing the length from the top-level ASN.1 object 77 82 XX YY
    int sodLen = int((data_p[2] << 8) + data_p[3] + 4);

    return CByteArray(data_p, sodLen);
}


CByteArray getAuthCert(APL_EIDCard *card) {
    APL_Certifs * certificates = card->getCertificates();
    const CByteArray &ba = certificates->getAuthentication()->getData();

    const unsigned char *data_p = ba.GetBytes();

    //Trim the padding 0x00 bytes parsing the length from the top-level ASN.1 object 30 82 XX YY
    int sodLen = int((data_p[2] << 8) + data_p[3] + 4);

    return CByteArray(data_p, sodLen);
}


/* JSON parsing functions */

RA_SignChallengeResponse parseSignChallengeResponse(const char * json_str) {
    RA_SignChallengeResponse resp;

    cJSON *signed_challenge = cJSON_Parse(json_str);
    cJSON *item = NULL;
    if (signed_challenge == NULL) {
        //MWLOG(LEV_ERROR, MOD_APL, "Failed to parse JSON for parseSignChallengeResponse!");
        fprintf(stderr, "JSON parsing error in parseSignChallengeResponse! Input: %s\n", json_str);
        return resp;
    }

    item = cJSON_GetObjectItem(signed_challenge, "ChallengeResponse");

    if (!cJSON_IsObject(item)) {
        item = cJSON_GetObjectItem(signed_challenge, "ErrorStatus");
        if (cJSON_IsObject(item)) {
            cJSON * error_code = cJSON_GetObjectItem(item, "code");
            resp.error_code = error_code->valueint;

        }
        cJSON_Delete(signed_challenge);
        return resp;
    }

    if (!cJSON_IsObject(item)) {
        item = cJSON_GetObjectItem(signed_challenge, "ErrorStatus");
        if (cJSON_IsObject(item)) {
            cJSON * error_code = cJSON_GetObjectItem(item, "code");
            resp.error_code = error_code->valueint;

        }
        cJSON_Delete(signed_challenge);
        return resp;
    }

    cJSON * item2 = cJSON_GetObjectItem(item, "signedChallenge");

    if (cJSON_IsString(item2) && (item2->valuestring != NULL)) {
        resp.signed_challenge.append(item2->valuestring);
    }

    item2 = cJSON_GetObjectItem(item, "SetSECommand");
    if (cJSON_IsString(item2) && (item2->valuestring != NULL)) {
        resp.set_se_command.append(item2->valuestring);
    }

    item2 = cJSON_GetObjectItem(item, "InternalAuthenticateCommand");
    if (cJSON_IsString(item2) && (item2->valuestring != NULL)) {
        resp.internal_auth_command.append(item2->valuestring);
    }

    cJSON_Delete(signed_challenge);

    return resp;

}


RA_DHParamsResponse parseDHParamsResponse(const char * json_str) {

    RA_DHParamsResponse resp;

    cJSON *dh_params = cJSON_Parse(json_str);
    cJSON *item = NULL;
    if (dh_params == NULL)
    {
        MWLOG(LEV_ERROR, MOD_APL, "Failed to parse JSON for DHparamsResponse! Input: %s", json_str);
        return resp;
    }

    item = cJSON_GetObjectItem(dh_params, "DHParamsResponse");

    if (!cJSON_IsObject(item)) {
        //Error response: get Error Status and write it to resp
        item = cJSON_GetObjectItem(dh_params, "ErrorStatus");
        if (cJSON_IsObject(item)) {
            cJSON * error_code = cJSON_GetObjectItem(item, "code");
            resp.error_code = error_code->valueint;

        }
        cJSON_Delete(dh_params);
        return resp;
    }

    cJSON * item2 = cJSON_GetObjectItem(item, "kifd");

    if (cJSON_IsString(item2) && (item2->valuestring != NULL)) {
        resp.kifd.append(item2->valuestring);
    }

    item2 = cJSON_GetObjectItem(item, "c_cv_ifd_aut");
    if (cJSON_IsString(item2) && (item2->valuestring != NULL)) {
        resp.cv_ifd_cert.append(item2->valuestring);
    }

    cJSON_Delete(dh_params);

    return resp;
}

RA_GetAddressResponse * validateReadAddressResponse(const char * json_str) {

    RA_GetAddressResponse *resp = new RA_GetAddressResponse();

    cJSON *cjson_obj = cJSON_Parse(json_str);
    cJSON *item = NULL;
    if (cjson_obj == NULL)
    {
        MWLOG(LEV_ERROR, MOD_APL, "Failed to parse JSON for ReadAddressResponse! Input: %s", json_str);
        resp->error_code = -1;
        return resp;
    }

    item = cJSON_GetObjectItem(cjson_obj, "readAddressResponse");

    if (!cJSON_IsObject(item)) {
        //Error response: get Error Status and write it to resp
        item = cJSON_GetObjectItem(cjson_obj, "ErrorStatus");
        if (cJSON_IsObject(item)) {
            cJSON * error_code = cJSON_GetObjectItem(item, "code");
            resp->error_code = error_code->valueint;

            cJSON * description = cJSON_GetObjectItem(item, "description");
            if (cJSON_IsString(description))
                resp->error_msg = description->valuestring;

        }
        cJSON_Delete(cjson_obj);
        return resp;
    }

    cJSON * item2 = cJSON_GetObjectItem(item, "addr");
    if (cJSON_IsObject(item2)) {
        //This is a national address
        resp->is_foreign_address = false;
        resp->address_obj = item2;
        resp->parent_json = cjson_obj;
    }
    else {
        item2 = cJSON_GetObjectItem(item, "foreign_addr");

        if (cJSON_IsObject(item2)) {
            resp->is_foreign_address = true;
            resp->address_obj = item2;
            resp->parent_json = cjson_obj;
        }
        else {
            MWLOG(LEV_ERROR, MOD_APL, "Invalid ReadAddressResponse: both addr and foreign_addr are NULL!");

        }
    }
    return resp;
}

cJSON * buildIDObject(APL_EidFile_ID &id_file) {
    cJSON *parent = cJSON_CreateObject();

    cJSON_AddItemToObject(parent,"Surname", cJSON_CreateString(id_file.getSurname()));
    cJSON_AddItemToObject(parent,"Givenname", cJSON_CreateString(id_file.getGivenName()));
    cJSON_AddItemToObject(parent,"Sex", cJSON_CreateString(id_file.getGender()));
    cJSON_AddItemToObject(parent,"Height", cJSON_CreateString(id_file.getHeight()));
    cJSON_AddItemToObject(parent,"Country", cJSON_CreateString(id_file.getCountry()));
    cJSON_AddItemToObject(parent,"Birthdate", cJSON_CreateString(id_file.getDateOfBirth()));
    cJSON_AddItemToObject(parent,"GivenNameFather", cJSON_CreateString(id_file.getGivenNameFather()));
    cJSON_AddItemToObject(parent,"SurnameFather", cJSON_CreateString(id_file.getSurnameFather()));
    cJSON_AddItemToObject(parent,"GivenNameMother", cJSON_CreateString(id_file.getGivenNameMother()));
    cJSON_AddItemToObject(parent,"SurnameMother", cJSON_CreateString(id_file.getSurnameMother()));
    cJSON_AddItemToObject(parent,"Documenttype", cJSON_CreateString(id_file.getDocumentType()));
    cJSON_AddItemToObject(parent,"Documentnum", cJSON_CreateString(id_file.getDocumentNumber()));
    cJSON_AddItemToObject(parent,"CivilianIdNumber", cJSON_CreateString(id_file.getCivilianIdNumber()));
    cJSON_AddItemToObject(parent,"Documentversion", cJSON_CreateString(id_file.getDocumentVersion()));
    cJSON_AddItemToObject(parent,"DocumentPAN", cJSON_CreateString(id_file.getDocumentPAN()));
    cJSON_AddItemToObject(parent,"Nationality", cJSON_CreateString(id_file.getNationality()));
    cJSON_AddItemToObject(parent,"Validityenddate", cJSON_CreateString(id_file.getValidityEndDate()));
    cJSON_AddItemToObject(parent,"Validitybegindate", cJSON_CreateString(id_file.getValidityBeginDate()));
    cJSON_AddItemToObject(parent,"PlaceOfRequest", cJSON_CreateString(id_file.getLocalofRequest()));
    cJSON_AddItemToObject(parent,"IssuingEntity", cJSON_CreateString(id_file.getIssuingEntity()));
    cJSON_AddItemToObject(parent,"NISS", cJSON_CreateString(id_file.getSocialSecurityNumber()));
    cJSON_AddItemToObject(parent,"NSNS", cJSON_CreateString(id_file.getHealthNumber()));
    cJSON_AddItemToObject(parent,"NIF", cJSON_CreateString(id_file.getTaxNo()));
    cJSON_AddItemToObject(parent,"MRZ1", cJSON_CreateString(id_file.getMRZ1()));
    cJSON_AddItemToObject(parent,"MRZ2", cJSON_CreateString(id_file.getMRZ2()));
    cJSON_AddItemToObject(parent,"MRZ3", cJSON_CreateString(id_file.getMRZ3()));
    cJSON_AddItemToObject(parent,"AccidentalIndications", cJSON_CreateString(id_file.getAccidentalIndications()));

    return parent;
}

cJSON * buildAddressObject(APL_EidFile_Address &addr) {
    cJSON *parent = cJSON_CreateObject();

    cJSON_AddItemToObject(parent, "countryCode", cJSON_CreateString(addr.getCountryCode()));
    cJSON_AddItemToObject(parent, "district", cJSON_CreateString(addr.getDistrict()));
    cJSON_AddItemToObject(parent, "districtCode", cJSON_CreateString(addr.getDistrictCode()));
    cJSON_AddItemToObject(parent, "municipality", cJSON_CreateString(addr.getMunicipality()));
    cJSON_AddItemToObject(parent, "municipalityCode", cJSON_CreateString(addr.getMunicipalityCode()));
    cJSON_AddItemToObject(parent, "parish", cJSON_CreateString(addr.getCivilParish()));
    cJSON_AddItemToObject(parent, "parishCode", cJSON_CreateString(addr.getCivilParishCode()));
    cJSON_AddItemToObject(parent, "streetType", cJSON_CreateString(addr.getStreetType()));
    cJSON_AddItemToObject(parent, "abbrStreetType", cJSON_CreateString(addr.getAbbrStreetType()));
    cJSON_AddItemToObject(parent, "streetName", cJSON_CreateString(addr.getStreetName()));
    cJSON_AddItemToObject(parent, "buildingType", cJSON_CreateString(addr.getBuildingType()));
    cJSON_AddItemToObject(parent, "abbrBuildingType", cJSON_CreateString(addr.getAbbrBuildingType()));
    cJSON_AddItemToObject(parent, "doorNo", cJSON_CreateString(addr.getDoorNo()));
    cJSON_AddItemToObject(parent, "floor", cJSON_CreateString(addr.getFloor()));
    cJSON_AddItemToObject(parent, "place", cJSON_CreateString(addr.getPlace()));
    cJSON_AddItemToObject(parent, "side", cJSON_CreateString(addr.getSide()));
    cJSON_AddItemToObject(parent, "locality", cJSON_CreateString(addr.getLocality()));
    cJSON_AddItemToObject(parent, "zip4", cJSON_CreateString(addr.getZip4()));
    cJSON_AddItemToObject(parent, "zip3", cJSON_CreateString(addr.getZip3()));
    cJSON_AddItemToObject(parent, "postalLocality", cJSON_CreateString(addr.getPostalLocality()));
    cJSON_AddItemToObject(parent, "generatedAddressCode", cJSON_CreateString(addr.getGeneratedAddressCode()));

    return parent;
}

cJSON * buildForeignAddressObject(APL_EidFile_Address &addr) {
    cJSON *parent = cJSON_CreateObject();

    cJSON_AddItemToObject(parent, "countryCode", cJSON_CreateString(addr.getCountryCode()));
    cJSON_AddItemToObject(parent, "foreignCountry",        cJSON_CreateString(addr.getForeignCountry()));
    cJSON_AddItemToObject(parent, "foreignGenericAddress", cJSON_CreateString(addr.getForeignAddress()));
    cJSON_AddItemToObject(parent, "foreignCity",           cJSON_CreateString(addr.getForeignCity()));
    cJSON_AddItemToObject(parent, "foreignRegion",         cJSON_CreateString(addr.getForeignRegion()));
    cJSON_AddItemToObject(parent, "foreignLocality",       cJSON_CreateString(addr.getForeignLocality()));
    cJSON_AddItemToObject(parent, "foreignPostalCode",     cJSON_CreateString(addr.getForeignPostalCode()));
    cJSON_AddItemToObject(parent, "generatedAddressCode",  cJSON_CreateString(addr.getGeneratedAddressCode()));


    return parent;
}

char * build_json_obj_dhparams(DHParams &dh, APL_EidFile_ID *id_file, APL_EidFile_Address *addr,
                   CByteArray &sod, CByteArray &auth_cert)
{

    if (id_file == NULL) {
        MWLOG(LEV_ERROR, MOD_APL, "%s: Illegal NULL parameter id_file", __FUNCTION__);
        return NULL;

    }
    else if (addr == NULL) {
        MWLOG(LEV_ERROR, MOD_APL, "%s: Illegal NULL parameter addr", __FUNCTION__);
        return NULL;
    }

    cJSON *parent = cJSON_CreateObject();
    cJSON *root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "P", cJSON_CreateString(dh.dh_p));
    cJSON_AddItemToObject(root, "Q", cJSON_CreateString(dh.dh_q));
    cJSON_AddItemToObject(root, "G", cJSON_CreateString(dh.dh_g));

    cJSON_AddItemToObject(root, "sod", cJSON_CreateString(byteArrayToHexString(sod)));
    cJSON_AddItemToObject(root, "auth_cert", cJSON_CreateString(byteArrayToHexString(auth_cert)));
    cJSON_AddItemToObject(root, "card_auth_public_key", cJSON_CreateString(dh.card_auth_public_key));
    cJSON_AddItemToObject(root, "cvc_ca_public_key", cJSON_CreateString(dh.cvc_ca_public_key));


    cJSON_AddItemToObject(root, "id", buildIDObject(*id_file));
    if (addr->isNationalAddress()) {
        cJSON_AddItemToObject(root, "addr", buildAddressObject(*addr));
    }
    else {
        cJSON_AddItemToObject(root, "foreign_addr", buildForeignAddressObject(*addr));   
    }
    
    cJSON_AddItemToObject(parent, "DHParams", root);

    char * json_str = cJSON_PrintUnformatted(parent);

    cJSON_Delete(parent);

    return json_str;
}

char * build_json_obj_sign_challenge(char * challenge, char * kicc) {

    cJSON *parent = cJSON_CreateObject();
    cJSON *root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "challenge", cJSON_CreateString(challenge));
    cJSON_AddItemToObject(root, "kicc", cJSON_CreateString(kicc));


    cJSON_AddItemToObject(parent, "Challenge", root);

    char * json_str = cJSON_PrintUnformatted(parent);

    cJSON_Delete(parent);

    return json_str;
}

char * build_json_obj_read_address(CByteArray & set_se_response, CByteArray &internal_authenticate_response) {
    cJSON *parent = cJSON_CreateObject();
    cJSON *root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "SetSEResponse", cJSON_CreateString(byteArrayToHexString(set_se_response)));
    cJSON_AddItemToObject(root, "InternalAuthenticateResponse", cJSON_CreateString(byteArrayToHexString(internal_authenticate_response)));


    cJSON_AddItemToObject(parent, "ReadAddress", root);

    char * json_str = cJSON_PrintUnformatted(parent);

    cJSON_Delete(parent);

    return json_str;
}

}