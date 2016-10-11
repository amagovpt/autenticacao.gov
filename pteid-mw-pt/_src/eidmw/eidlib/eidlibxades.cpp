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

void PTEID_EIDCard::SignXadesAIndividual(const char *const * path, unsigned int n_paths, const char *output_path)
{
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesAIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

}

PTEID_ByteArray PTEID_EIDCard::SignXadesA(const char *const * path, unsigned int n_paths, const char *output_path)
{

	PTEID_ByteArray out;
	
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXadesA((const char **)path, n_paths, output_path);
	out.Append(ca.GetBytes(), ca.Size());

	END_TRY_CATCH

	return out;
}


}
