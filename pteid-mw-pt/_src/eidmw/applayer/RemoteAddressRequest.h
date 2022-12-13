#pragma once

#include <string>

namespace eIDMW
{

    class PostResponse {
    public:
        long http_code;
        std::string http_response;
        std::string http_headers;
    };


    std::string parseCookieFromHeaders(std::string headers);

    PostResponse post_json_remoteaddress(const char *endpoint, char * json_str, const char *cookie);

}