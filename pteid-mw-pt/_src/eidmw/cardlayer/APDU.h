#ifndef APDU_H
#define APDU_H

#include "ByteArray.h"

#include <stdio.h>

namespace eIDMW
{
class APDU
{
public:
    APDU();
    ~APDU();
    unsigned char &cls();
    unsigned char &ins();
    unsigned char &p1();
    unsigned char &p2();
    bool &forceExtended();

    unsigned char cls() const;
    unsigned char ins() const;
    unsigned char p1() const;
    unsigned char p2() const;
    CByteArray getLe(bool onlyLe = false) const;
    CByteArray getLc(bool onlyLc = false) const;
    CByteArray data() const;
    bool dataExists() const;

    bool setData(const CByteArray &data);
    bool setLe(const long &le);
    bool canBeShort() const;
    bool isExtended() const;

    CByteArray ToByteArray() const;
    CByteArray getHeader() const;

private:
    eIDMW::CByteArray formatExtended(size_t lengthData, size_t byteSize) const;

private:
    unsigned char m_cls;
    unsigned char m_ins;
    unsigned char m_p1, m_p2;
    CByteArray m_data;  // size of data is lc
    long m_le;
    unsigned char *m_resp;
    size_t m_resLen;
    bool m_forceExtended;
    bool m_isExtended;

};
}

#endif // APDU_H
