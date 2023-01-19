#pragma once

#include <string>
#include <curl/curl.h>

#include "Export.h"

namespace eIDMW {
	//Apply MW proxy settings to a libcurl handle with a certain URL
	EIDMW_APL_API bool applyProxyConfigToCurl(CURL * curl_handle, const std::string &url_to_access);
}