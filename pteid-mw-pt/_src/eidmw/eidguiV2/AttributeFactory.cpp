#include "AttributeFactory.h"

namespace eIDMW
{
    CitizenAttribute uriToAttribute(std::string uri) {
        if (uri == ATTR_URI_NIC)
            return NIC;
        else if (uri == ATTR_URI_NAME)
            return NAME;
        else if (uri == ATTR_URI_SURNAME)
            return SURNAME;
        else if (uri == ATTR_URI_COMPLETENAME)
            return COMPLETENAME;        
        return NONE;
    }

    std::string attributeToUri(CitizenAttribute attribute) {
        switch (attribute)
        {
        case NIC:
            return ATTR_URI_NIC;
        case NAME:
            return ATTR_URI_NAME;
        case SURNAME:
            return ATTR_URI_SURNAME;
        case COMPLETENAME:
            return ATTR_URI_COMPLETENAME;
        default:
            return "";
        }
    }
}