#ifndef SIGCONTAINER_H
#define SIGCONTAINER_H

#define SIG_INTERNAL_PATH "META-INF/signatures.xml"
#define TS_INTERNAL_PATH "META-INF/ts_resp.bin"

#include <cstdio>
#include "ByteArray.h"
#include "MiscUtil.h"
#include <zip.h>

#ifdef WIN32
#define NL "\r\n"
#else
#define NL "\n"
#endif

namespace eIDMW
{



	void StoreSignatureToDisk(CByteArray& sig, const char **paths, int num_paths, const char *output_file);

}

#endif
