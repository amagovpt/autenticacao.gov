/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#include "Logger.h"
#include "Mutex.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

namespace EidInfra
{
// Logging is done only if this dir exists
#ifdef _WIN32
static const char logFileName[] = "C:\\Temp\\pcscproxy\\log.txt";
#else
static const char logFileName[] = "/tmp/pcscproxy/log.txt";
#endif

//////////////////////// CLOG class declaration ///////////////////////////////

class CLOG
{
public:
CLOG();
~CLOG();
bool init();
void log(const char *type, const char *format, va_list args);
private:
bool   initIsOK;
CMutex mutex;
};

CLOG logger;

/////////////////////// Exported functions ////////////////////////////////////

void logInfo(const char *format, ...)
{
	if (logger.init())
	{
		va_list args;
		va_start(args, format);
		try {
			logger.log("", format, args);
		}
		catch (...) {
			va_end(args);
		}
		va_end(args);
	}
}

void logError(const char *format, ...)
{
	if (logger.init())
	{
		va_list args;
		va_start(args, format);
		try {
			logger.log("ERR: ", format, args);
		}
		catch (...) {
			va_end(args);
		}
		va_end(args);
	}
}

std::string logGetTimeStr(const char *format)
{
	time_t    rawtime;
	struct tm timeinfo;
	char      timeBuf [20];

	time(&rawtime);
#ifdef WIN32
	localtime_s(&timeinfo, &rawtime);
#else
	timeinfo = *(localtime(&rawtime));
#endif
	strftime(timeBuf, 20, format, &timeinfo);

	return timeBuf;
}

//////////////////////// CLOG class implementation ////////////////////////////

CLOG::CLOG()
{
	// Init is done in the init() function
	initIsOK = false;
}

CLOG::~CLOG()
{
	if (initIsOK)
	{
		CAutoMutex autoMutex(&mutex);
		initIsOK = false;

		// TODO: cleanup
	}
}

bool CLOG::init()
{
	if (!initIsOK)
	{
		CAutoMutex autoMutex(&mutex);

		// Check if we can open/create the log file for writing to it
		FILE *f  = NULL;
		int  err = fopen_s(&f, logFileName, "a");
		if (err == 0 && f != NULL)
		{
			fclose(f);

			// TODO

			initIsOK = true;
		}
	}

	return initIsOK;
}

void CLOG::log(const char *type, const char *format, va_list args)
{
	CAutoMutex autoMutex(&mutex);

	if (initIsOK)
	{
		FILE *f  = NULL;
		int  err = fopen_s(&f, logFileName, "a");
		if (err == 0 && f != NULL)
		{
			if (type != NULL && strlen(type) != 0)
				fprintf_s(f, "%s", type);
			vfprintf_s(f, format, args);

			fclose(f);
		}
	}
}
}
