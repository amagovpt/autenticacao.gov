#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signature.xml"
#include <cstdio>
#include "ByteArray.h"
#include "MiscUtil.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

namespace eIDMW
{

	class Container
	{
		public:
			Container(const char *zip_path);
			CByteArray *ExtractFile(const char *entry);
			CByteArray *ExtractSignature();
			//Array of pointers to tHashedFiles
			tHashedFile **getHashes(int *);
	
		private:
			mz_zip_archive zip_archive;

	};


	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file);
}

#endif
