#include "eidlib.h"

#include "eidlibException.h"
#include "InternalUtil.h"
#include "MWException.h"
#include "eidErrors.h"

#include "APLCard.h"
#include "ByteArray.h"

namespace eIDMW
{

/** Validates an XML-DSIG or XAdES signature
 *
 *  This method is intended to validate XADES signatures produced with PTEID_EIDCard::SignXades() method
 *  even though any conforming signature should work
 *
 *  Implementation note: External references in the <SignedInfo> element are not checked
 *
 *  @param IN signature is a byte array containing the UTF-8 representation of an XML document
 *  @param OUT error_buffer if not NULL should point to a preallocated char buffer that will be filled with 
 *  a description of eventual validation problems
 *  @param IN/OUT error_size on input it should point to the size of error_buffer while on API return it points to the length of the string written into error_buffer
 */
bool PTEID_SigVerifier::VerifySignature(const char *container_path, char * error_buffer, unsigned long *error_size)
{
	bool res = false;

	res = APLVerifySignature(container_path, error_buffer, error_size);
	return res;

}


PTEID_ByteArray PTEID_EIDCard::SignXades(const char * const* paths, unsigned int n_paths, const char *output_path)
{

	PTEID_ByteArray out;
	
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXades((const char **)paths, n_paths, output_path);
	out.Append(ca.GetBytes(), ca.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_EIDCard::SignXades(PTEID_ByteArray to_be_signed, const char *URL)
{

	PTEID_ByteArray out;
	
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXades(CByteArray(to_be_signed.GetBytes(), 
			to_be_signed.Size()), URL);
	out.Append(ca.GetBytes(), ca.Size());

	END_TRY_CATCH

	return out;


}

PTEID_ByteArray PTEID_EIDCard::SignXadesT(const char *const * path, unsigned int n_paths, const char *output_path)
{

	PTEID_ByteArray out;
	
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXadesT((const char **)path, n_paths, output_path);
	out.Append(ca.GetBytes(), ca.Size());

	END_TRY_CATCH

	return out;
}

PTEID_ByteArray PTEID_EIDCard::SignXadesT(PTEID_ByteArray to_be_signed, const char *URL)
{

	PTEID_ByteArray out;
	
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXadesT(CByteArray(to_be_signed.GetBytes(), 
			to_be_signed.Size()), URL);
	out.Append(ca.GetBytes(), ca.Size());

	END_TRY_CATCH

	return out;


}
}
