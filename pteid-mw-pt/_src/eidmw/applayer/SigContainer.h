#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signature.xml"
#include <cstdio>
#include "ByteArray.h"

namespace eIDMW
{
	CByteArray *ExtractSignature(const char *archive_path);
	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file);
;
}

#endif
