#include "eidErrors.h"
#include "SigContainer.h"
#include <xsec/dsig/DSIGReferenceList.hpp>

#include <xercesc/dom/DOMNode.hpp>

#define XERCES_NS XERCES_CPP_NAMESPACE_QUALIFIER
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

#define SHA1_LEN 20

	class SignatureVerifier
	{

	public:
		SignatureVerifier(const char *sig_container_path);
		SigVerifyErrorCode Verify();
		char *GetSigner();
		char *GetTimestampString();
		//Maybe we should return a specific object/struct for this
		time_t GetUnixTimestamp();
		//getTimestamp() //TODO: Create a custom class struct that expresses the timestamp in all its glorious detail

	private:
		SigVerifyErrorCode ValidateXades(CByteArray, tHashedFile **);
		SigVerifyErrorCode ValidateTimestamp(CByteArray signature, CByteArray timestamp);
		CByteArray do_post_validate_timestamp(char *input, long input_len, char *sha1_string);
		bool grep_validation_result (CByteArray validate_data);
		bool ValidateCert(const char *certificate);
		bool checkExternalRefs(DSIGReferenceList *refs, tHashedFile **hashes);
		char *parseSubjectFromCert();
		void initXerces();
		void base64Decode(const char *array, unsigned int inlen, unsigned char *&decoded, unsigned int &decoded_len);
		const char * m_sigcontainer_path;
		char * m_time_and_date;
		X509 *m_cert;
		XERCES_NS DOMNode * m_signature_dom;

	};


}
