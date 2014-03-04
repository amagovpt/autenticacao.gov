#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signatures.xml"
#define TS_INTERNAL_PATH "META-INF/ts_resp.bin"

#include <cstdio>
#include "ByteArray.h"
#include "MiscUtil.h"
#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

#ifdef WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

namespace eIDMW
{



	class Container
	{
		public:
			Container(const char *zip_path);
			~Container();
			CByteArray &ExtractFile(const char *entry);
			CByteArray &ExtractSignature();
			CByteArray &ExtractTimestamp();
			//Array of pointers to tHashedFiles
			tHashedFile **getHashes(int *);
	
		private:
			mz_zip_archive zip_archive;


	};


	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file);

}

#endif
