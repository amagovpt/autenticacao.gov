/*
    To be able to build with support for CMD, please contact info.cidadao@ama.pt
*/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

#include <string>
#include "CMDSignature.h" // to define PTEIDCMD_API

namespace eIDMW
{
    class PTEIDCMD_API CMDCredentials {
    public:
        static std::string getCMDBasicAuthAppId(std::string defaultValue);
        static std::string getCMDBasicAuthUserId(std::string defaultValue);
        static std::string getCMDBasicAuthPassword(std::string defaultValue);
    };
}
#endif