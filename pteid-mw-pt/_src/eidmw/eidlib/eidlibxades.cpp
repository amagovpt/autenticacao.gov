/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2016-2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <time.h>
#include "eidlib.h"

#include "eidlibException.h"
#include "InternalUtil.h"
#include "MWException.h"
#include "eidErrors.h"

#include "APLCard.h"
#include "SigContainer.h"
#include "ByteArray.h"

namespace eIDMW
{



PTEID_ByteArray PTEID_Card::SignXades(const char *output_path, const char * const* paths, unsigned int n_paths,
	PTEID_SignatureLevel level)
{

	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXades((const char **)paths, n_paths, output_path, ConvertSignatureLevel(level));
	out.Append(ca.GetBytes(), ca.Size());
	delete (&ca);

	END_TRY_CATCH

	return out;
}


void PTEID_Card::SignXadesIndividual(const char *output_path, const char *const * path, unsigned int n_paths)
{

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

}

void PTEID_Card::SignXadesTIndividual(const char *output_path, const char *const * path, unsigned int n_paths)
{

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesTIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

}

PTEID_ByteArray PTEID_Card::SignXadesT(const char *output_path, const char *const * path, unsigned int n_paths)
{

	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXadesT((const char **)path, n_paths, output_path);
	out.Append(ca.GetBytes(), ca.Size());
	delete (&ca);

	END_TRY_CATCH

	return out;
}

void PTEID_Card::SignXadesAIndividual(const char *output_path, const char *const * path, unsigned int n_paths)
{
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	pcard->SignXadesAIndividual((const char **)path, n_paths, output_path);

	END_TRY_CATCH

}

PTEID_ByteArray PTEID_Card::SignXadesA(const char *output_path, const char *const * path, unsigned int n_paths)
{

	PTEID_ByteArray out;

	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);

	CByteArray &ca = pcard->SignXadesA((const char **)path, n_paths, output_path);
	out.Append(ca.GetBytes(), ca.Size());
	delete (&ca);

	END_TRY_CATCH

	return out;
}

void PTEID_Card::SignASiC(const char *path, PTEID_SignatureLevel level)
{
	BEGIN_TRY_CATCH

	APL_Card *pcard = static_cast<APL_Card *>(m_impl);
	pcard->SignASiC(path, ConvertSignatureLevel(level));

	END_TRY_CATCH
}

PTEID_ASICContainer::PTEID_ASICContainer(const char *input_path)
{
	m_impl = new SigContainer(input_path);
}

PTEID_ASICContainer::~PTEID_ASICContainer()
{
	delete (SigContainer *)m_impl;
}

long PTEID_ASICContainer::countInputFiles()
{

	if (m_files.size() == 0) {
		
		try {
			SigContainer *container = (SigContainer *)m_impl;
			m_files = container->listInputFiles();
		}
		catch(CMWException &e)
		{
			throw PTEID_Exception::THROWException(e);
		}
	}

	return m_files.size();
}

const char * PTEID_ASICContainer::getInputFile(long file_n)
{

	if (m_files.size() == 0) {
		
		try {
			SigContainer *container = (SigContainer *)m_impl;
			m_files = container->listInputFiles();
			
		}
		catch(CMWException &e)
		{
			throw PTEID_Exception::THROWException(e);
		}

	}

	return m_files.at(file_n).c_str();
}

void PTEID_ASICContainer::extract(const char *filename, const char * out_dir)
{
	try {
		SigContainer *container = (SigContainer *)m_impl;
		container->extract(filename, out_dir);
	}
	catch (CMWException &e) {
		throw PTEID_Exception::THROWException(e);
	}

}

}
