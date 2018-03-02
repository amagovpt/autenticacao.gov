/* ****************************************************************************
 *
 *  PTeID Middleware Project.
 *  Copyright (C) 2012-2016
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
 *  Signature container for XAdES signature file and associated signed file(s) -
 *  It should be compliant with the ASIC specification TS 102 918 -
 *  http://www.etsi.org/deliver/etsi_ts/102900_102999/102918/01.01.01_60/ts_102918v010101p.pdf
 */

#include <fstream>
#include <cstring>
#ifndef _WIN32
//This has to be explicitly included in gcc4.7
#include <unistd.h>
#endif

#ifdef WIN32
#include <Windows.h> //CharToOem()
#include "Util.h"
#define unlink _unlink
#endif

#include "MiscUtil.h"
#include "ByteArray.h"

#include <openssl/sha.h>
#include "miniz.h"
#include "SigContainer.h"
#include "Log.h"

/*
 * Zip compression/decompression functions for signatures and
 * referenced files.
 *
 */

namespace eIDMW
{

static const char *SIGCONTAINER_README=
"############################################################" NL
"LEIA-ME" NL
"" NL
"Este ficheiro zip contém informação assinada com a respectiva assinatura em META-INF/signature.xml" NL
"Esta assinatura foi criada através da Aplicação Oficial do Cartão de Cidadão." NL
"" NL
"Para verificar a validade da assinatura, deverá na Aplicação Oficial do Cartão de Cidadão aceder ao seguinte menu:" NL
"	\"Opções\" -> \"Verificar Assinatura\" -> Introduzir o ficheiro zip a validar." NL
"" NL
"Mais Informação:" NL
"" NL
"Download da Aplicação Oficial do Cartão de Cidadão:" NL
"http://svn.gov.pt/projects/ccidadao" NL
"" NL
"Especificação Técnica da Assinatura Digital:" NL
"Xades / Xades-T" NL
"http://www.w3.org/TR/XAdES/" NL
"" NL
"############################################################" NL
"README" NL
"" NL
"This zip file includes signed information. The signature file can be found in META-INF/signature.xml" NL
"This signature was produced through Portuguese ID Card Management application." NL
"" NL
"To verify this signature, use the Portuguese ID Card Management application, following these instructions:" NL
"	\"Tools\" -> \"Verify Signature\" -> Introduce file to be verified." NL
"" NL
"More Info:" NL
"" NL
"Download Portuguese ID Card Management application:" NL
"http://svn.gov.pt/projects/ccidadao" NL
"" NL
"Signature technical specification:" NL
"Xades / Xades-T" NL
"http://www.w3.org/TR/XAdES" NL;


	char *readFile(const char *path, int *size)
	{
	#ifdef WIN32
		std::wstring utf16FileName = utilStringWiden(std::string(path));
		std::ifstream file(utf16FileName, std::ios::binary | std::ios::ate);
	#else
		std::ifstream file(path, std::ios::binary | std::ios::ate);
	#endif
		char * in;

		if (file.is_open())
		{
			*size = file.tellg();
			in = (char *)malloc(*size);
			file.seekg(0, std::ios::beg);
			file.read(in, *size);
		}
		else
		{
			MWLOG(LEV_ERROR, MOD_APL, "SigContainer::readFile() Error opening file %s", path);
			return NULL;
		}

		file.close();
		return in;
	}

	void AddReadMe(mz_zip_archive *pZip)
	{
		mz_bool status = MZ_FALSE;

		status = mz_zip_writer_add_mem(pZip, "META-INF/README.txt", SIGCONTAINER_README, strlen(SIGCONTAINER_README), MZ_BEST_COMPRESSION);

		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_writer_add_mem failed for README.txt");
		}
	}

	void AddMimeTypeFile(mz_zip_archive *pZip, int num_paths)
	{
		mz_bool status = MZ_FALSE;

		const char * MIMETYPE_ASIC_S = "application/vnd.etsi.asic-s+zip";
		const char * MIMETYPE_ASIC_E = "application/vnd.etsi.asic-e+zip";

		const char *mimetype = num_paths > 1 ? MIMETYPE_ASIC_E : MIMETYPE_ASIC_S;

		//We need to store the file with no compression to act as a kind of "magic number" within the zip container
		status = mz_zip_writer_add_mem(pZip, "mimetype", mimetype, strlen(mimetype), MZ_NO_COMPRESSION);

		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_writer_add_mem failed for mimetype. miniz error code: %d", mz_zip_get_last_error(pZip));
		}
	}

	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file)
	{

		int file_size = 0;
		char *ptr_content = NULL;
		const char *absolute_path = NULL;
		char *zip_entry_name= NULL;

		mz_bool status;
		//Zip-file object handler
		mz_zip_archive pZip;
		memset(&pZip, 0, sizeof(pZip));

		MWLOG(LEV_DEBUG, MOD_APL, "StoreSignatureToDisk() called with output_file = %s",output_file);

		//TODO: any flags to specify
		mz_uint flags = 0;
#ifdef WIN32
		std::wstring utf16FileName = utilStringWiden(std::string(output_file));
		FILE *pFile = _wfopen(utf16FileName.c_str(), L"wb");
#else
		FILE *pFile = fopen(output_file, "wb");
#endif
		status = mz_zip_writer_init_cfile(&pZip, pFile, flags);
		if (!status) {
			mz_zip_error mz_err = mz_zip_get_last_error(&pZip);
			MWLOG(LEV_DEBUG, MOD_APL, "mz_zip_writer_init_cfile() failed, error 0x%X (%s)\n", mz_err, mz_zip_get_error_string(mz_err));
		}
	
		//Add a mimetype file as defined in the ASIC standard ETSI TS 102 918
		//It needs to be stored first in the archive and uncompressed so it can be used as a kind of magic number
		//for systems that use them
		AddMimeTypeFile(&pZip, num_paths);

		// Append the referenced files to the zip file
		for (unsigned int  i = 0; i < num_paths; i++)
		{
			absolute_path = paths[i];
			ptr_content = readFile(absolute_path, &file_size);
			MWLOG(LEV_DEBUG, MOD_APL, "Compressing %d bytes from file %s", file_size, absolute_path);

			zip_entry_name = Basename((char *)absolute_path);

			status = mz_zip_writer_add_mem(&pZip, zip_entry_name, ptr_content,
					file_size, MZ_BEST_COMPRESSION);

			free(ptr_content);
			if (!status)
			{
				MWLOG (LEV_ERROR, MOD_APL, "mz_zip_add_mem_to_archive_file_in_place failed with argument %s",
						zip_entry_name);
				return;
			}
		}

		//Add the signature file to the container

		status = mz_zip_writer_add_mem(&pZip, SIG_INTERNAL_PATH, sig.GetBytes(), sig.Size(), MZ_BEST_COMPRESSION);
		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for the signature file");
			return ;
		}

		//Add a README file to the container
		AddReadMe(&pZip);

		status = mz_zip_writer_finalize_archive(&pZip);
		if (!status)
			return;

		status = mz_zip_writer_end(&pZip);
		if (!status)
			return;

		fclose(pFile);

	}
};
