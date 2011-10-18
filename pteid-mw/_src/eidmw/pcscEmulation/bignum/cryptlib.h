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
#ifndef HEADER_CRYPTLIB_H
#define HEADER_CRYPTLIB_H

#include <stdlib.h>
#include <string.h>

#define OPENSSL_free(p) free(p)
#define OPENSSL_malloc(p) malloc(p)

#define BNerr(f,r) printf("Error in %d: 0x%0x (%d)\n", (f), (r), (r))

void OPENSSL_cleanse(void *ptr, size_t len);

/* fatal error */
#define ERR_R_FATAL				64
#define	ERR_R_MALLOC_FAILURE			(1|ERR_R_FATAL)
#define	ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED	(2|ERR_R_FATAL)
#define	ERR_R_PASSED_NULL_PARAMETER		(3|ERR_R_FATAL)
#define	ERR_R_INTERNAL_ERROR			(4|ERR_R_FATAL)

#endif
