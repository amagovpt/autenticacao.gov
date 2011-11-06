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

#ifndef EXPORT_H
#define EXPORT_H

#ifdef WIN32

#ifdef EIDMW_CMN_EXPORT
#define EIDMW_CMN_API __declspec(dllexport)
#else
#define EIDMW_CMN_API __declspec(dllimport)
#endif

#ifdef EIDMW_CAL_EXPORT
#define EIDMW_CAL_API __declspec(dllexport)
#else
#define EIDMW_CAL_API __declspec(dllimport)
#endif

#ifdef EIDMW_PF_EXPORT
#define EIDMW_PF_API __declspec(dllexport)
#else
#define EIDMW_PF_API __declspec(dllimport)
#endif

#ifdef EIDMW_PCSCEMU_EXPORT
#define EIDMW_PCSCEMU_API __declspec(dllexport)
#else
#define EIDMW_PCSCEMU_API __declspec(dllimport)
#endif

#ifdef EIDMW_APPLAYER_EXPORTS
#define EIDMW_APL_API __declspec(dllexport)
#else
#define EIDMW_APL_API __declspec(dllimport)
#endif

#else

#define EIDMW_CMN_API

#define EIDMW_CAL_API

#define EIDMW_PCSCEMU_API

#define EIDMW_APL_API

#endif

#endif
