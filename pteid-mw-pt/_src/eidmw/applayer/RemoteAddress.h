/*  RemoteAddress.h */

#pragma once

#include <string>
#include "SAM.h"
#include "ByteArray.h"
#include "cJSON.h"

//Simple data classes to represent webservice responses

namespace eIDMW
{

    class RA_DHParamsResponse {
        public:
            std::string kifd;
            std::string cv_ifd_cert;
            int error_code;

           RA_DHParamsResponse() { error_code = 0; } 
    };

    class RA_SignChallengeResponse {
        public:
            std::string signed_challenge;
            std::string set_se_command;
            std::string internal_auth_command;
            int error_code;

           RA_SignChallengeResponse() { error_code = 0; }

    };

    class RA_GetAddressResponse {
        public:
            cJSON * address_obj;
            cJSON * parent_json;
            bool is_foreign_address;
            int error_code;
            char * error_msg;
            RA_GetAddressResponse() { 
                error_code = 0;
                is_foreign_address = false;
                address_obj = NULL;
                parent_json = NULL;
            }

            ~RA_GetAddressResponse() {
                if (parent_json)
                    cJSON_Delete(parent_json);
            }
    };

    CByteArray getSodData(APL_EIDCard *card);
    CByteArray getAuthCert(APL_EIDCard *card);

    RA_DHParamsResponse parseDHParamsResponse(const char * json_str);
    RA_SignChallengeResponse parseSignChallengeResponse(const char * json_str);
    RA_GetAddressResponse *validateReadAddressResponse(const char * json_str);

    cJSON * buildIDObject(APL_EidFile_ID &id_file);
    cJSON * buildAddressObject(APL_EidFile_Address &addr);

    //JSON object building functions
    char * build_json_obj_dhparams(DHParams &dh_params, APL_EidFile_ID *id_file, APL_EidFile_Address *addr,
                       CByteArray &sod, CByteArray &auth_cert);
    char * build_json_obj_sign_challenge(char *challenge, char * kicc);
    char * build_json_obj_read_address(CByteArray & set_se_response, CByteArray &internal_authenticate_response);


}