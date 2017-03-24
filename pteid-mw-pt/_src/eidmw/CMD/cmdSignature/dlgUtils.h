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
#ifndef DLG_UTILS_H
#define DLG_UTILS_H
/* For filenames we need to maintain latin-1 or UTF-8 native encoding */
//This macro's argument is a QString
#ifdef _WIN32
	#define getPlatformNativeString(s) s.toLatin1().constData()
#else
	#define getPlatformNativeString(s) s.toUtf8().constData()
#endif

#endif /* DLG_UTILS_H */
