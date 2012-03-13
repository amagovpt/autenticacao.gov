#include <fstream>
#include <cstring>

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

	Container::Container(const char *zip_path)
	{
	  memset(&zip_archive, 0, sizeof(zip_archive));
	  mz_bool status = mz_zip_reader_init_file(&zip_archive, zip_path, 0);
	  if (!status)
	     MWLOG(LEV_ERROR, MOD_APL, L"Error in mz_zip_reader_init_file!\n");

	}

	Container::~Container()
	{
		//Close the zip file and other buffers
		mz_zip_reader_end(&zip_archive);

	}

	CByteArray *Container::ExtractFile(const char *entry)
	{
		void *p;
		size_t uncompressed_size = 0;
		CByteArray *ba = new CByteArray();
		p = mz_zip_reader_extract_file_to_heap(&zip_archive, entry, &uncompressed_size, 0);
		if (!p)
		{
			MWLOG(LEV_ERROR, MOD_APL, L"Error in ExtractFile() %s\n", entry);
			return NULL;
		}

		ba->Append ((const unsigned char *)p, uncompressed_size);

		return ba;
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
			MWLOG(LEV_ERROR, MOD_APL, L"SigContainer::readFile() Error opening file %s\n", path);

		file.close();
		return in;
	}
	
	CByteArray* Container::ExtractSignature()
	{

		return ExtractFile(SIG_INTERNAL_PATH);

	}

	CByteArray* Container::ExtractTimestamp()
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
			if (strcmp(file_stat.m_filename, SIG_INTERNAL_PATH) != 0
				&& strcmp(file_stat.m_filename, "README.txt") != 0
				&& strcmp(file_stat.m_filename, TS_INTERNAL_PATH) != 0)
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

	
	void AddReadMe(const char *output)
	{

		mz_bool status = MZ_FALSE;

		status = mz_zip_add_mem_to_archive_file_in_place (output, "README.txt", README, strlen(README),
				"", (unsigned short)0, MZ_BEST_COMPRESSION);

		if (!status)
		{
			MWLOG (LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for README.txt");
		}
	}

	void StoreSignatureToDisk(CByteArray& sig, CByteArray *ts_data, const char **paths, int num_paths, const char *output_file)
	{

		int file_size = 0;
		char *ptr_content = NULL;
		const char *absolute_path = NULL;
		char *zip_entry_name= NULL;
		char *utf8_filename;
		mz_bool status;

		MWLOG(LEV_DEBUG, MOD_APL, L"StoreSignatureToDisk() called with output_file = %s\n",output_file); 

		//Try to delete the output file first...
		if (unlink(output_file) == 0)
		    MWLOG(LEV_DEBUG, MOD_APL, L"StoreSignatureToDisk() overwriting output file %s\n",output_file); 

		// Append the referenced files to the zip file
		for (unsigned int  i = 0; i < num_paths; i++)
		{   
			absolute_path = paths[i]; 	
			ptr_content = readFile(absolute_path, &file_size);
			MWLOG(LEV_DEBUG, MOD_APL, L"Compressing %d bytes from file %s\n", file_size, absolute_path);

			zip_entry_name = Basename((char *)absolute_path);


#ifdef WIN32
			utf8_filename = new char[strlen(zip_entry_name)*2];
			latin1_to_utf8((unsigned char *)zip_entry_name, (unsigned char *)utf8_filename);
			zip_entry_name = utf8_filename;
			MWLOG (LEV_DEBUG, MOD_APL, L"Compressing filename (after conversion): %s\n", zip_entry_name);
#endif

			status = mz_zip_add_mem_to_archive_file_in_place(output_file, zip_entry_name, ptr_content,
					file_size, "", (unsigned short)0, MZ_BEST_COMPRESSION);
			if (!status)
			{
				MWLOG (LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed with argument %s",
						zip_entry_name);
				return;
			}

			free(ptr_content);
		}

		//Add the signature file to the container

		status = mz_zip_add_mem_to_archive_file_in_place(output_file, SIG_INTERNAL_PATH, sig.GetBytes(),
				sig.Size(), "", (unsigned short)0, MZ_BEST_COMPRESSION);
		if (!status)
		{   
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for the signature file");
			return ;
		}

		if (ts_data != NULL)
		{
			status = mz_zip_add_mem_to_archive_file_in_place(output_file, TS_INTERNAL_PATH, ts_data->GetBytes(),
					ts_data->Size(), "", (unsigned short)0, MZ_BEST_COMPRESSION);

			if (!status)
			{   
				MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for the Timestamp file");
				return ;
			}
		}

		//Add a README file to the container 

		AddReadMe(output_file);

	}
};
