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
#ifndef __util_h__
#define __util_h__

#include <stdlib.h>
#include <string.h>

//#define DUMP_TEMPLATE_MAX	32

#ifdef __cplusplus
extern "C" {
#endif

CK_RV p11_init_lock(CK_C_INITIALIZE_ARGS_PTR args);
CK_RV p11_lock();
void p11_unlock();
void p11_free_lock();
void util_init_lock(void **lock);
void util_clean_lock(void **lock);
void util_lock(void *lock);
void util_unlock(void *lock);
void memwash(char *p_in, unsigned int len);
void strcpy_n(unsigned char *to, const char *from, size_t n, char padding);

#ifdef __cplusplus
   }
#endif

#endif

