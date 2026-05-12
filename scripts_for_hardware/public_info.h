#pragma once

#include <string>

#include <eidlib.h>

namespace ccapp {

void printPublicInfoAndSavePhoto(eIDMW::PTEID_EIDCard& eidCard, 
                                 const std::string& photoPath = "cc_photo",
                                 const std::string& jsonPath = "cc_info.json");

}