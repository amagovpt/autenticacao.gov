/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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
#pragma once

#ifndef __ZrcConsts__
#define __ZrcConsts__

const static string COOKIE = "ZrcA";
const char* _WRITE_ERR = "Could not write to archive stream.";
const char* _NOT_OPEN_ERROR = "Archive is not open.";
const char* _INVALID_FORMAT = "Invalid ZrcArchive format.";
const char* _READ_ERROR = "Could not read from archive stream.";
const char* _INVALID_STRING_LENGTH = "Header corrupt (stringlength > 512)."; 

#endif __ZrcConsts__