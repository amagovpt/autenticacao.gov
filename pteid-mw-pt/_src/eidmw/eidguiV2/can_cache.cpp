#include <string>

#include "eidlib.h"
//Declarations for the following functions here
#include "gapi.h"

void saveCAN(const char * serial, const char *can) {
	std::string config_name("can_");
	config_name += serial;
	PTEID_Config config(config_name.c_str(), L"can_cache", L"");
	config.setString(can);
}

bool deleteCAN() {
	std::string config_name("can_");
	PTEID_Config config(config_name.c_str(), L"can_cache", L"");
	config.DeleteKeysByPrefix();
	return true;
}

std::string getCANFromCache(const char * serial) {
	std::string config_name("can_");
	config_name += serial;
	PTEID_Config config(config_name.c_str(), L"can_cache", L"");
	return config.getString();
}