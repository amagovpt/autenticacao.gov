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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>

namespace EidInfra
{
/**
 * Log info.
 * E.g. logInfo("SCardConnect(%s) called\n", readerName);
 */
void logInfo(const char *format, ...);

/**
 * Log an error.
 * E.g. logError("Can't open config file: err = 0x%0x (%d)\n", err, err);
 */
void logError(const char *format, ...);

/**
 * Return the current date/time as a string.
 * E.g. logGetTimeStr("%Y-%m-%d %H:%M:%S")
 */
std::string logGetTimeStr(const char *format);
}

#endif
