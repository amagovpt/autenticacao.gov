#ifndef APDU_H
#define APDU_H

#include "ByteArray.h"

#include <stdio.h>

namespace eIDMW {
class EIDMW_CAL_API APDU {
public:
	APDU();
	~APDU();
	unsigned char &cla();
	unsigned char &ins();
	unsigned char &p1();
	unsigned char &p2();
	bool &forceExtended();

	unsigned char cla() const;
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

	CByteArray ToByteArray(bool usingPace = false) const;
	CByteArray getHeader() const;

    static CByteArray formatExtended(size_t lengthData, size_t byteSize);

private:
	unsigned char m_cla;
	unsigned char m_ins;
	unsigned char m_p1, m_p2;
	CByteArray m_data; // size of data is lc
	long m_le;
	unsigned char *m_resp;
	size_t m_resLen;
	bool m_forceExtended;
	bool m_isExtended;
};
} // namespace eIDMW

#endif // APDU_H
