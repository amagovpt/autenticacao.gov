#include <time.h>
#include "eidlib.h"

#include "eidlibException.h"
#include "InternalUtil.h"
#include "MWException.h"
#include "eidErrors.h"

#include "APLCard.h"
#include "SigVerifier.h"
#include "ByteArray.h"

namespace eIDMW
{
	PTEID_SigVerifier::PTEID_SigVerifier(const char * container_path)
	{

		m_impl = new SignatureVerifier(container_path);

	}
	PTEID_SigVerifier::~PTEID_SigVerifier()
	{
		delete m_impl;
	}

/** Validates an XML-DSIG or XAdES signature
 *
 *  This method is intended to validate XADES signatures produced with PTEID_EIDCard::SignXades() method
 *  even though any conforming signature should work
 *
 */
int PTEID_SigVerifier::Verify()
{

	return m_impl->Verify();
}

char *PTEID_SigVerifier::GetSigner()
{

	return m_impl->GetSigner();

}

char *PTEID_SigVerifier::GetTimestampString()
{


	return m_impl->GetTimestampString();

}


long long PTEID_SigVerifier::GetUnixTimestamp()
{


	return m_impl->GetUnixTimestamp();

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
	//
	PTEID_ByteArray out;

	return out;
}


void PTEID_EIDCard::SignXadesIndividual(const char *const * path, unsigned int n_paths, const char *output_path)
{

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

}

void PTEID_EIDCard::SignXadesTIndividual(const char *const * path, unsigned int n_paths, const char *output_path)
{

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesTIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

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

	return out;

}
}
