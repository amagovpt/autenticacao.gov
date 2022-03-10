#include "credentials.h"
#include "Util.h"
#include "Config.h"

using namespace eIDMW;

std::string CMDCredentials::getCMDBasicAuthAppId(std::string defaultValue) {
    std::string regAppId = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_APPID));
    if (regAppId != "default_value") {
        return regAppId;
    }
    return defaultValue;
}
std::string CMDCredentials::getCMDBasicAuthUserId(std::string defaultValue) {
    std::string regUserId = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_USERID));
    if (regUserId != "default_value") {
        return regUserId;
    }
    return defaultValue;
}
std::string CMDCredentials::getCMDBasicAuthPassword(std::string defaultValue) {
    std::string regPassword = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_PASSWORD));
    if (regPassword != "default_value") {
        return regPassword;
    }
    return defaultValue;
}