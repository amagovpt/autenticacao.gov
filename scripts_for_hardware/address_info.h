#pragma once

#include <eidlib.h>
#include <string>

namespace ccapp {

void readAddressInfo(eIDMW::PTEID_EIDCard& eidCard, const std::string& jsonPath = "cc_address.json");

}