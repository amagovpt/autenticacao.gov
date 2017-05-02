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
#define unlink _unlink
#endif

#include "MiscUtil.h"
#include "ByteArray.h"

#include <openssl/sha.h>
#include "miniz.c"
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

	Container::Container(const char *zip_path)
	{
	  memset(&zip_archive, 0, sizeof(zip_archive));
	  mz_bool status = mz_zip_reader_init_file(&zip_archive, zip_path, 0);
	  if (!status)
	     MWLOG(LEV_ERROR, MOD_APL, L"Error in mz_zip_reader_init_file!");

	}

	Container::~Container()
	{
		//Close the zip file and other buffers
		mz_zip_reader_end(&zip_archive);

	}

	CByteArray &Container::ExtractFile(const char *entry)
	{
		void *p;
		size_t uncompressed_size = 0;
		CByteArray *ba = new CByteArray();
		p = mz_zip_reader_extract_file_to_heap(&zip_archive, entry, &uncompressed_size, 0);
		if (!p)
		{
			MWLOG(LEV_ERROR, MOD_APL, "Error in ExtractFile() %s", entry);
			return *ba;
		}

		ba->Append ((const unsigned char *)p, uncompressed_size);
		zip_archive.m_pFree(zip_archive.m_pAlloc_opaque, p);

		return *ba;
	}

	char *readFile(const char *path, int *size)
	{
		std::ifstream file(path, std::ios::binary|std::ios::ate);
		char * in;

		if (file.is_open())
		{
			*size = file.tellg();
			in = (char *)malloc(*size);
			file.seekg(0, std::ios::beg);
			file.read (in, *size);
		}
		else
		{
			MWLOG(LEV_ERROR, MOD_APL, "SigContainer::readFile() Error opening file %s", path);
			return NULL;
		}

		file.close();
		return in;
	}

	CByteArray& Container::ExtractSignature()
	{
		return ExtractFile(SIG_INTERNAL_PATH);
	}

	CByteArray& Container::ExtractTimestamp()
	{
		return ExtractFile(TS_INTERNAL_PATH);
	}

	tHashedFile** Container::getHashes(int *pn_files)
	{
		unsigned char out[20];
		int i;
		int c= 0;
		void *p;

		int n_files = mz_zip_reader_get_num_files(&zip_archive);
		tHashedFile **hashes = new tHashedFile*[n_files];

		for (i = 0; i < n_files; i++)
		{
			size_t uncomp_size = 0;
			mz_zip_archive_file_stat file_stat;
			memset(out, 0, 20);
			if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
			{
				fprintf(stderr, "E: mz_zip_reader_file_stat() failed!\n");
				mz_zip_reader_end(&zip_archive);
				continue;
			}
			// Exclude from signed file checking the Signature itself
			// and the README file that gets added to all signed containers
			// and the "workaround timestamp response" file
			if (strstr(file_stat.m_filename, "META-INF") == 0
				&& strcmp(file_stat.m_filename, "README.txt") != 0)
			{
				p = mz_zip_reader_extract_file_to_heap(&zip_archive,file_stat.m_filename, &uncomp_size, 0);
				if (!p)
				{
					fprintf(stderr, "E: mz_zip_reader_extract_file_to_heap() failed!\n");
					mz_zip_reader_end(&zip_archive);
					continue;
				}

				CByteArray* ba = new CByteArray();

				SHA1((unsigned char *)p, uncomp_size, out);
				ba->Append(out, 20);
				tHashedFile *t = new tHashedFile();

				t->hash = ba;
				t->URI = new std::string(file_stat.m_filename);
				hashes[c] = t;
				c++;

				//Cleanup each read file buffer
				free(p);
			}
		}
		hashes[c] = NULL;
		*pn_files = n_files-1;
		return hashes;
	}


	void AddReadMe(const char *output_file)
	{

		mz_bool status = MZ_FALSE;

		status = mz_zip_add_mem_to_archive_file_in_place (output_file, "META-INF/README.txt", SIGCONTAINER_README, strlen(SIGCONTAINER_README),
				"", (unsigned short)0, MZ_BEST_COMPRESSION);

		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for README.txt");
		}
	}

	void AddMimeTypeFile(const char *output_file, int num_paths)
	{
		mz_bool status = MZ_FALSE;

		const char * MIMETYPE_ASIC_S = "application/vnd.etsi.asic-s+zip";
		const char * MIMETYPE_ASIC_E = "application/vnd.etsi.asic-e+zip";

		const char *mimetype = num_paths > 1 ? MIMETYPE_ASIC_E : MIMETYPE_ASIC_S;

		//We need to store the file with no compression to act as a kind of "magic number" within the zip container
		status = mz_zip_add_mem_to_archive_file_in_place(output_file, "mimetype", mimetype, strlen(mimetype),
				"", (unsigned short)0, MZ_NO_COMPRESSION);

		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for mimetype");
		}
	}

	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file)
	{

		int file_size = 0;
		char *ptr_content = NULL;
		const char *absolute_path = NULL;
		char *zip_entry_name= NULL;
#ifdef WIN32
		char *utf8_filename;
#endif
		mz_bool status;

		MWLOG(LEV_DEBUG, MOD_APL, "StoreSignatureToDisk() called with output_file = %s\n",output_file);

		//Try to delete the output file first...
		if (unlink(output_file) == 0)
		    MWLOG(LEV_DEBUG, MOD_APL, "StoreSignatureToDisk() overwriting output file %s\n",output_file);

		//Add a mimetype file as defined in the ASIC standard ETSI TS 102 918
		//It needs to be stored first in the archive and uncompressed so it can be used as a kind of magic number
		//for systems that use them
		AddMimeTypeFile(output_file, num_paths);

		// Append the referenced files to the zip file
		for (unsigned int  i = 0; i < num_paths; i++)
		{
			absolute_path = paths[i];
			ptr_content = readFile(absolute_path, &file_size);
			MWLOG(LEV_DEBUG, MOD_APL, "Compressing %d bytes from file %s", file_size, absolute_path);

			zip_entry_name = Basename((char *)absolute_path);


#ifdef WIN32
			utf8_filename = new char[strlen(zip_entry_name)*2];
			latin1_to_utf8((unsigned char *)zip_entry_name, (unsigned char *)utf8_filename);
			zip_entry_name = utf8_filename;
			MWLOG (LEV_DEBUG, MOD_APL, "Compressing filename (after conversion): %s", zip_entry_name);
#endif

			status = mz_zip_add_mem_to_archive_file_in_place(output_file, zip_entry_name, ptr_content,
					file_size, "", (unsigned short)0, MZ_BEST_COMPRESSION);

			free(ptr_content);
			if (!status)
			{
				MWLOG (LEV_ERROR, MOD_APL, "mz_zip_add_mem_to_archive_file_in_place failed with argument %s",
						zip_entry_name);
				return;
			}
		}

		//Add the signature file to the container

		status = mz_zip_add_mem_to_archive_file_in_place(output_file, SIG_INTERNAL_PATH, sig.GetBytes(),
				sig.Size(), "", (unsigned short)0, MZ_BEST_COMPRESSION);
		if (!status)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for the signature file");
			return ;
		}

		//Add a README file to the container
		AddReadMe(output_file);

	}
};
