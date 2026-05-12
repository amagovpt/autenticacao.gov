#pragma once

#include <eidlib.h>
#include <string>

namespace ccapp {

std::string readAddressInfo(eIDMW::PTEID_EIDCard& eidCard, const std::string& jsonPath = "cc_address.json");

}