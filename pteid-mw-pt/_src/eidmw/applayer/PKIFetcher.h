/*-****************************************************************************

 * Copyright (C) 2014 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include "ByteArray.h"

namespace eIDMW {
class PKIFetcher {
public:
	CByteArray fetch_PKI_file(const char *url);

private:
	static size_t curl_write_data(char *, size_t, size_t, void *);
	static CByteArray received_data;
};

} // namespace eIDMW
