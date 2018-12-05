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
"Mais Informação:" NL
"" NL
"Download da Aplicação Oficial do Cartão de Cidadão:" NL
"https://www.autenticacao.gov.pt/cc-aplicacao" NL
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
"More Info:" NL
"" NL
"Download Portuguese ID Card Management application:" NL
"https://www.autenticacao.gov.pt/cc-aplicacao" NL
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

	void AddReadMe(zip_t *pZip)
	{
		zip_source_t *source = zip_source_buffer(pZip, SIGCONTAINER_README, strlen(SIGCONTAINER_README), 0);
		if (source == NULL ||
			zip_file_add(pZip, "META-INF/README.txt", source, ZIP_FL_ENC_GUESS) < 0) 
		{
			zip_source_free(source);
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to add README.txt to zip container");
			return;
		}
	}
	void AddManifestFile(zip_t *pZip)
	{
		zip_source_t *source = zip_source_buffer(pZip, "", 0, 0);
		if (source == NULL ||
			zip_file_add(pZip, "META-INF/manifest.xml", source, ZIP_FL_ENC_GUESS) < 0)
		{
			zip_source_free(source);
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to add manifest.xml to zip container");
			return;
		}
	}

	void AddMimeTypeFile(zip_t *pZip, int num_paths)
	{
		int index;

		const char * MIMETYPE_ASIC_S = "application/vnd.etsi.asic-s+zip";
		const char * MIMETYPE_ASIC_E = "application/vnd.etsi.asic-e+zip";

		const char *mimetype = num_paths > 1 ? MIMETYPE_ASIC_E : MIMETYPE_ASIC_S;

		//We need to store the file with no compression to act as a kind of "magic number" within the zip container
		zip_source_t *source = zip_source_buffer(pZip, mimetype, strlen(mimetype), 0);
		if (source == NULL ||
			(index = zip_file_add(pZip, "mimetype", source, ZIP_FL_ENC_GUESS)) < 0)
		{
			zip_source_free(source);
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to add mymetype file to zip container");
			return;
		}
		if(zip_set_file_compression(pZip, index, ZIP_CM_STORE, 0) < 0)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to set store compression of mymetype");
		}
	}

	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file)
	{

		const char *absolute_path = NULL;
		char *zip_entry_name= NULL;

		int status = 0;
		zip_t *pZip = NULL;

		MWLOG(LEV_DEBUG, MOD_APL, "StoreSignatureToDisk() called with output_file = %s",output_file);

		pZip = zip_open(output_file, ZIP_CREATE | ZIP_TRUNCATE, &status);
		if(!pZip) {
			MWLOG(LEV_DEBUG, MOD_APL, "zip_open() failed, error %d\n", status);
			return;
		}
	
		//Add a mimetype file as defined in the ASIC standard ETSI TS 102 918
		//It needs to be stored first in the archive and uncompressed so it can be used as a kind of magic number
		//for systems that use them
		AddMimeTypeFile(pZip, num_paths);
		// The manisfest.xml file is required for ASIC-E validation by the online DSS validation tool but it is not being used as it
		// is not required by the standard. As a workaround, to be able to validate with such tools, the manifest file is added empty.
		if(num_paths > 1)
		{
			AddManifestFile(pZip);
		}
		// Append the referenced files to the zip file
		for (unsigned int  i = 0; i < num_paths; i++)
		{
			absolute_path = paths[i];
			MWLOG(LEV_DEBUG, MOD_APL, "Adding file %s to archive", absolute_path);

			zip_entry_name = Basename((char *)absolute_path);
			zip_source_t *source = zip_source_file(pZip, absolute_path, 0, -1);
			if (source == NULL ||
				zip_file_add(pZip, zip_entry_name, source, ZIP_FL_ENC_GUESS) < 0)
			{
				zip_source_free(source);
				MWLOG(LEV_ERROR, MOD_APL, L"Failed to add %s to zip container", zip_entry_name);
				return;
			}
		}

		//Add the signature file to the container
		zip_source_t *source = zip_source_buffer(pZip, sig.GetBytes(), sig.Size(), 0);
		if (source == NULL ||
			zip_file_add(pZip, SIG_INTERNAL_PATH, source, ZIP_FL_ENC_GUESS) < 0)
		{
			zip_source_free(source);
			MWLOG(LEV_ERROR, MOD_APL, L"Failed to add signature to zip container");
			return;
		}

		//Add a README file to the container
		AddReadMe(pZip);

		if (zip_close(pZip) < 0)
		{
			free(pZip);
			MWLOG(LEV_ERROR, MOD_APL, "zip_close failed with error %d",
				zip_error_strerror(zip_get_error(pZip)));
			return;
		}
	}
};
