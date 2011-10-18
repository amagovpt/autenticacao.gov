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
/**
 * Country-specific values.
 *
 * One of the following must be specified in the project:
 * CSP_BEID, CSP_PTEID
 */

#ifndef __CSPDEFINES_H__
#define __CSPDEFINES_H__

#ifdef CSP_BEID
	#define CSP_NAME "Belgium Identity Card CSP"
	#define CSP_VERSION 0x00000100
#endif

#ifdef CSP_PTEID
	#define CSP_NAME "Portugal Identity Card CSP"
	#define CSP_VERSION 0x00000100
#endif


#endif
