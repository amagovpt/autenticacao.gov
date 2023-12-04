#include "APDU.h"

#include <iomanip>
#include <sstream>

#define MAX_LC_LE_LENGTH 0xFFFF
#define MAX_LC_LE_LENGTH_SHORT 255

eIDMW::APDU::APDU()
{
    m_cls = m_ins = m_p1 = m_p2 = 0x00;
    m_le = -1;
    m_isExtended = m_forceExtended = false;
}

eIDMW::APDU::~APDU()
{
}

unsigned char &eIDMW::APDU::cls()
{
    return m_cls;
}

unsigned char &eIDMW::APDU::ins()
{
    return m_ins;
}

unsigned char &eIDMW::APDU::p1()
{
    return m_p1;
}

unsigned char &eIDMW::APDU::p2()
{
    return m_p2;
}

unsigned char eIDMW::APDU::cls() const
{
    return m_cls;
}

unsigned char eIDMW::APDU::ins() const
{
    return m_ins;
}

unsigned char eIDMW::APDU::p1() const
{
    return m_p1;
}

unsigned char eIDMW::APDU::p2() const
{
    return m_p2;
}

eIDMW::CByteArray eIDMW::APDU::getLe(bool onlyLe) const
{
    bool isExtended = m_isExtended || m_forceExtended;
    CByteArray result;
    if(m_le >= 0)
    {
        if(isExtended) {
            CByteArray formatLe;
            if(m_data.Size() > 0 || onlyLe) {
                formatLe = formatExtended(m_le, 2);
            }
            else {
                formatLe = formatExtended(m_le, 3);
            }
            result.Append(formatLe);
        }
        else {
            result.Append(m_le);
        }
    }

    return result;
}

eIDMW::CByteArray eIDMW::APDU::getLc(bool onlyLc) const
{
    CByteArray result;
    bool isExtended = m_isExtended || m_forceExtended;
    size_t lc = m_data.Size();
    if(lc > 0) {
        result.Append(m_data);
        if(isExtended) {
            if(onlyLc)
                result.Append(formatExtended(lc, 2));
            else
                result.Append(formatExtended(lc, 3));
        }
        else {
            result.Append(lc);
        }
    }

    return result;
}

eIDMW::CByteArray eIDMW::APDU::data() const
{
    return m_data;
}

bool eIDMW::APDU::dataExists() const
{
    return m_data.Size() > 0;
}

bool &eIDMW::APDU::forceExtended()
{
    return m_forceExtended;
}

bool eIDMW::APDU::setData(const CByteArray &data)
{
    if(data.Size() > MAX_LC_LE_LENGTH)
        return false;
    if(data.Size() > MAX_LC_LE_LENGTH_SHORT) {
        m_isExtended = true;
    }
    m_data = data;
    return true;
}

bool eIDMW::APDU::setLe(const long &le)
{
    if(le > MAX_LC_LE_LENGTH)
        return false;

    if(le > MAX_LC_LE_LENGTH_SHORT) {
        m_isExtended = true;
    }

    m_le = le;
    return true;
}

bool eIDMW::APDU::canBeShort() const
{
    return !m_isExtended;
}

bool eIDMW::APDU::isExtended() const
{
    return m_isExtended || m_forceExtended;
}

eIDMW::CByteArray eIDMW::APDU::ToByteArray() const
{
    size_t sizeOfAPDU = 2;
    size_t lc = m_data.Size();
    bool isExtended = m_isExtended || m_forceExtended;
    if(lc > 0) {
        sizeOfAPDU += isExtended ? 3 : 1; // lc size
        sizeOfAPDU += lc;
    }
    sizeOfAPDU += isExtended ? lc > 0 ? 2 : 3 : 1; // le size
    CByteArray result(sizeOfAPDU);
    result.Append(m_cls);
    result.Append(m_ins);
    result.Append(m_p1);
    result.Append(m_p2);

    if(lc > 0) {
        result.Append(m_data);
        if(isExtended) {
            result.Append(formatExtended(lc, 3));
        }
        else {
            result.Append(lc);
        }
    }
    if(m_le >= 0)
    {
        if(isExtended) {
            CByteArray formatLe;
            if(lc > 0) {
                formatLe = formatExtended(m_le, 2);
            }
            else {
                formatLe = formatExtended(m_le, 3);
            }
            result.Append(formatLe);
        }
        else {
            result.Append(m_le);
        }
    }
    return result;
}

eIDMW::CByteArray eIDMW::APDU::getHeader() const
{
    CByteArray header(4);
    header.Append(m_cls);
    header.Append(m_ins);
    header.Append(m_p1);
    header.Append(m_p2);

    return header;
}

eIDMW::CByteArray eIDMW::APDU::formatExtended(size_t lengthData, size_t byteSize) const //bytesize needed
{
    std::stringstream stream;
    stream << std::hex << lengthData;
    std::string formatLength = stream.str();

    size_t fillLength = (byteSize * 2) - formatLength.length();
    std::string filler(fillLength, '0');
    formatLength.insert(0, filler);
    CByteArray array;

    for (size_t i = 0; i < byteSize * 2; i += 2) {
        std::string byteStr = formatLength.substr(i, 2);
        unsigned char byteValue = std::stoi(byteStr, nullptr, 16);
        array.Append(byteValue);
    }
    return array;
}
