#include "eidErrors.h"
#include "SigContainer.h"

namespace eIDMW
{
	enum SigVerifyErrorCode
	{

		XADES_ERROR_OK,	
		XADES_ERROR_NOSIG,
		XADES_ERROR_BROKENSIG,
		XADES_ERROR_EXTERNAL_REFS,
		XADES_ERROR_INVALID_RSA,
		XADES_ERROR_INVALID_CERTIFICATE,
		XADES_ERROR_INVALID_TIMESTAMP

	};

	class SignatureImpl;

	class SignatureVerifier
	{

	public:
		EIDMW_APL_API SignatureVerifier(const char *sig_container_path);
		EIDMW_APL_API ~SignatureVerifier();

		EIDMW_APL_API SigVerifyErrorCode Verify();
		EIDMW_APL_API char *GetSigner();
		EIDMW_APL_API char *GetTimestampString();
		EIDMW_APL_API time_t GetUnixTimestamp();
		
		//getTimestamp() //TODO: Create a custom class struct that expresses the timestamp in all its glorious detail

	private:
		void terminateXMLUtils();
		void initXMLUtils();
		SigVerifyErrorCode ValidateXades(CByteArray, tHashedFile **);
		SigVerifyErrorCode ValidateTimestamp(CByteArray signature, CByteArray timestamp);
		CByteArray do_post_validate_timestamp(char *input, long input_len, char *sha1_string);
		bool grep_validation_result (CByteArray validate_data);
		bool ValidateCert(const char *certificate);
		
		char *parseSubjectFromCert();
		void initXerces();
		void base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len);
		char * m_sigcontainer_path;
		char * m_time_and_date;
		X509 *m_cert;
		SignatureImpl *pimpl;

	};


}
