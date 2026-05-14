#ifndef SIGN_DOCUMENTS_H
#define SIGN_DOCUMENTS_H

#include <eidlib.h>
#include <string>

namespace ccapp {
    int signDocuments(eIDMW::PTEID_EIDCard &eidCard, const std::string &baseDir);
}

#endif // SIGN_DOCUMENTS_H
