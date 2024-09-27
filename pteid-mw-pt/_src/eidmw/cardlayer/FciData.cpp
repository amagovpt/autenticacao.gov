/* ****************************************************************************

 * PT eID Middleware Project.
 * Copyright (C) 2024 André Guerreiro - <aguerreiro1985@gmail.com>
 */

/* ntohl function in different system headers */
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <cstring>

#include "Util.h"
#include "Log.h"
#include "FciData.h"

namespace eIDMW {

/* Extract EF file length value from smartcard FCI data:
   There are two possible tags for file size attribute in FCI data
   0x80 and 0x81, both are mentioned in ISO 7816-4 standard */
int extractEFSize(CByteArray &fci_data) {
	long tag_size = 0;
	int ef_length = 0;
	// Tag 80: Number of data bytes in the file, excluding structural information
	const unsigned char *tag_value = findASN1Object(fci_data, tag_size, 0x80);
	if (tag_value == NULL) {
		tag_value = findASN1Object(fci_data, tag_size, 0x81);
		if (tag_value == NULL) {
			MWLOG(LEV_ERROR, MOD_CAL, "Broken FCI data! Tags 80 and 81 not found!");
			return 0;
		}
	}

	if (tag_size > 4) {
		MWLOG(LEV_ERROR, MOD_CAL, "FCI length tag larger than 4 bytes!");
		return 0;
	}
	int offset = sizeof(int) - tag_size;
	char *dest_ptr = (char *)&ef_length + offset;
	memcpy(dest_ptr, tag_value, tag_size);

	// The ASN.1 tags in FCI contain big-endian integers
	return ntohl(ef_length);
}

} // namespace eIDMW

#ifdef UNIT_TEST

int main() {
	int ef_len = 0;
	// FCI example from PEP document
	const unsigned char fci1[] = {0x62, 0x3d, 0x82, 0x02, 0x01, 0x21, 0x80, 0x04, 0x00, 0x00, 0x10, 0x00, 0x8a,
								  0x01, 0x07, 0x83, 0x02, 0x01, 0x1d, 0x81, 0x04, 0x00, 0x00, 0x10, 0x00, 0x86,
								  0x24, 0xd0, 0x00, 0x02, 0x00, 0xff, 0xff, 0xd6, 0x00, 0x02, 0x00, 0xff, 0xff,
								  0xb0, 0x00, 0x02, 0x00, 0xff, 0xff, 0xb0, 0x00, 0x00, 0x00, 0x1b, 0x1b, 0xa4,
								  0x00, 0x02, 0x00, 0xff, 0xff, 0xa4, 0x00, 0x00, 0x00, 0x1b, 0x1b};
	// FCI example from CC2 document
	const unsigned char fci2[] = {0x6f, 0x11, 0x80, 0x03, 0x00, 0x02, 0x1d, 0x82, 0x01, 0x01,
								  0x83, 0x02, 0x01, 0x0e, 0x86, 0x03, 0x40, 0x01, 0xff};
	// FCI example from CC1 document
	const unsigned char fci3[] = {0x6f, 0x15, 0x81, 0x02, 0x3c, 0x8c, 0x82, 0x01, 0x01, 0x83, 0x02, 0xef,
								  0x02, 0x8a, 0x01, 0x05, 0x8c, 0x05, 0x1b, 0xff, 0xff, 0xff, 0x00};
	// Another FCI example from CC2 document
	const unsigned char fci4[] = {0x6f, 0x15, 0x81, 0x02, 0x0a, 0x8c, 0x82, 0x01, 0x01, 0x83, 0x02, 0xef,
								  0x02, 0x8a, 0x01, 0x05, 0x8c, 0x05, 0x1b, 0xff, 0xff, 0xf1, 0x00};

	eIDMW::CByteArray fci_test(fci1, sizeof(fci1));
	ef_len = extractEFSize(fci_test);
	printf("EF Length: %d (%#X)\n", ef_len, ef_len);

	eIDMW::CByteArray fci_test2(fci2, sizeof(fci2));
	ef_len = extractEFSize(fci_test2);
	printf("EF Length: %d (%#X)\n", ef_len, ef_len);

	eIDMW::CByteArray fci_test3(fci3, sizeof(fci3));
	ef_len = extractEFSize(fci_test3);
	printf("EF Length: %d (%#X)\n", ef_len, ef_len);

	eIDMW::CByteArray fci_test4(fci4, sizeof(fci4));
	ef_len = extractEFSize(fci_test4);
	printf("EF Length: %d (%#X)\n", ef_len, ef_len);
}

#endif