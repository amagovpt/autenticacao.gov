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
    PaceAuthentication(CContext *poContext);
    ~PaceAuthentication();

    void initPaceAuthentication(SCARDHANDLE &hCard, const void *param_structure);

    CByteArray sendAPDU(const CByteArray& plainAPDU, SCARDHANDLE &hCard, long &lRetVal, const void * param_structure);

private:
    std::unique_ptr<PaceAuthenticationImpl> m_impl;
};

}

#endif // PACEAUTHENTICATION_H
