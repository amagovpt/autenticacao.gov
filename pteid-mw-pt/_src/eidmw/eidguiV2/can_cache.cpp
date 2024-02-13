#include <string>

#include "eidlib.h"
//Declarations for the following functions here
#include "gapi.h"

void saveCAN(const char *can) {
	std::string config_name("can");
	PTEID_Config config(config_name.c_str(), L"can_cache", L"");
	config.setString(can);
}

bool deleteCAN() {
	std::string config_name("can");
	try {
		PTEID_Config config(config_name.c_str(), L"can_cache", L"");
		config.DeleteKeysByPrefix();
	}
	catch (PTEID_Exception& e) {
		PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_WARNING, "eidgui", "deleteCAN: Failed to delete cached CAN! Error code: %08x", e.GetError());
		return false;
	}
	return true;
}

std::string getCANFromCache() {
	std::string config_name("can");

	PTEID_Config config(config_name.c_str(), L"can_cache", L"");
	return config.getString();
}