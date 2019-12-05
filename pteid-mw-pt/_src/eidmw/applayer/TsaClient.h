/*-****************************************************************************

 * Copyright (C) 2013-2014 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

#include "ByteArray.h"

namespace eIDMW
{
	class TSAClient
	{
		public:
			TSAClient();
			void timestamp_data(const unsigned char *input, unsigned int data_len);
			CByteArray getResponse();


		private:
			static size_t curl_write_data(char *, size_t, size_t, void *);
			void generate_asn1_request_struct(const unsigned char *, bool);
			static CByteArray received_data;

	};

}
