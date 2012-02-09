#include <fstream>

#ifdef WIN32
#include <Windows.h> //CharToOem()
#endif
#include "SigContainer.h"
#include "MiscUtil.h"
#include "ByteArray.h"
#include "miniz.c"
#include "Log.h"

/*
 * Zip compression/decompression functions for signatures and
 * referenced files.
 *
 */

namespace eIDMW
{

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
			MWLOG(LEV_ERROR, MOD_APL, L"SigContainer::readFile() Error opening file %S\n", path);

		file.close();
		return in;
	}
	
	CByteArray* ExtractSignature(const char *archive_path)
	{

		size_t decompressed_size = 0; 
		CByteArray *ba = new CByteArray();
		char * sig = (char *)mz_zip_extract_archive_file_to_heap(archive_path, SIG_INTERNAL_PATH,
				&decompressed_size, 0);

		if (sig == NULL)
			MWLOG(LEV_ERROR, MOD_APL, L"Error extracting signature from file %S\n", archive_path);
		
		ba->Append((const unsigned char *)sig, decompressed_size);
		return ba;

	}

	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file)
	{

		int file_size = 0;
		char *ptr_content = NULL;
		const char *absolute_path = NULL;
		char *zip_entry_name= NULL;
		char *utf8_filename;
		mz_bool status;

		MWLOG(LEV_DEBUG, MOD_APL, L"StoreSignatureToDisk() called with output_file = %S\n",output_file); 

		//Truncate the output file first...
		Truncate(output_file);

		// Append the referenced files to the zip file
		for (unsigned int  i = 0; i < num_paths; i++)
		{   
			absolute_path = paths[i]; 	
			ptr_content = readFile(absolute_path, &file_size);
			MWLOG(LEV_DEBUG, MOD_APL, L"Compressing %d bytes from file %S\n", file_size, absolute_path);

			zip_entry_name = Basename((char *)absolute_path);


#ifdef WIN32
			//In-place conversion to the OEM Codepage (tipically CP850)
			//CharToOemBuffA(zip_entry_name, zip_entry_name, strlen(zip_entry_name));
			utf8_filename = new char[strlen(zip_entry_name)*2];
			latin1_to_utf8((unsigned char *)zip_entry_name, (unsigned char *)utf8_filename);
			zip_entry_name = utf8_filename;
			MWLOG(LEV_DEBUG, MOD_APL, L"Compressing filename (after conversion): %S\n", zip_entry_name);
#endif
			

			status = mz_zip_add_mem_to_archive_file_in_place(output_file, zip_entry_name, ptr_content,
					file_size, "", (unsigned short)0, MZ_BEST_COMPRESSION);
			if (!status)
			{   
				MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed with argument %S",
						zip_entry_name);
				return ;
			}

			free(ptr_content);
		}   

		//Append the signature file to the container

		status = mz_zip_add_mem_to_archive_file_in_place(output_file, SIG_INTERNAL_PATH, sig.GetBytes(),
				sig.Size(), "", (unsigned short)0, MZ_BEST_COMPRESSION);
		if (!status)
		{   
			MWLOG(LEV_ERROR, MOD_APL, L"mz_zip_add_mem_to_archive_file_in_place failed for the signature file");
			return ;
		}

	}
};
