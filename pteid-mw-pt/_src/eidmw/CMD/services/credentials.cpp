#include "credentials.h"
#include "Util.h"
#include "Config.h"

using namespace eIDMW;

std::string CMDCredentials::getCMDBasicAuthAppId() {
    std::string regAppId = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_APPID));
    if (regAppId != "default_value") {
        return regAppId;
    }
    return CMD_BASIC_AUTH_APPID;
}
std::string CMDCredentials::getCMDBasicAuthUserId() {
    std::string regUserId = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_USERID));
    if (regUserId != "default_value") {
        return regUserId;
    }
    return CMD_BASIC_AUTH_USERID;
}
std::string CMDCredentials::getCMDBasicAuthPassword() {
    std::string regPassword = utilStringNarrow(CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_PASSWORD));
    if (regPassword != "default_value") {
        return regPassword;
    }
    return CMD_BASIC_AUTH_PASSWORD;
}