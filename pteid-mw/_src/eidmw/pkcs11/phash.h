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
#if !defined(__HASH__)
#define __HASH__

#include "pteid_p11.h"

#ifdef __cplusplus
extern "C" {
#endif


int hash_init(CK_MECHANISM_PTR pMechanism, void **pphashinfo, unsigned int *size);
int hash_update(void *phashinfo, char *p, unsigned long l);
int hash_final(void *phashinfo, unsigned char *p, unsigned long *l);


#ifdef __cplusplus
   }
#endif


#endif

