/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

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
