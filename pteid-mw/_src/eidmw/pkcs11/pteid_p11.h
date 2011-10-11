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
#ifndef __pteid_p11_h__
#define __pteid_p11_h__

#ifndef _WIN32
#include "include/rsaref220/unix.h"
#include "include/rsaref220/pkcs11.h"
#else
#include "include/rsaref220/win32.h"
#pragma pack(push, cryptoki, 1)
#include "include/rsaref220/pkcs11.h"
#pragma pack(pop, cryptoki)
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL	0
#else
#define NULL	((void *) 0)
#endif
#endif



/************************/
/* differences with 2.20*/
/************************/
/*#define CK_TRUE   TRUE
#define CK_FALSE  FALSE
#define CKM_SHA256_RSA_PKCS            0x00000040
#define CKM_SHA384_RSA_PKCS            0x00000041
#define CKM_SHA512_RSA_PKCS            0x00000042
*/

#endif
