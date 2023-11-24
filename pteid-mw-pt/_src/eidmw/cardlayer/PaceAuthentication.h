#ifndef PACEAUTHENTICATION_H
#define PACEAUTHENTICATION_H

#include "Context.h"

#include <memory>

namespace eIDMW
{
class PaceAuthenticationImpl;
class PaceAuthentication
{
public:
    PaceAuthentication(CContext *poContext, SCARDHANDLE* hCard, const void *param_structure);
    ~PaceAuthentication();

    CByteArray formatAPDU(const CByteArray &plainAPDU);

private:
    std::unique_ptr<PaceAuthenticationImpl> m_impl;
};

}

#endif // PACEAUTHENTICATION_H
