#include "ByteArray.h"

namespace eIDMW
{
	class CRLFetcher
	{
		public:
			CByteArray fetch_CRL_file(const char *crl_url);

		private:
			static size_t curl_write_data(char *, size_t, size_t, void *);
			static CByteArray received_data;

	};


}