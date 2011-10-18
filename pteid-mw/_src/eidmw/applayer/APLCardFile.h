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

#ifndef __APLCARDFILE_H__
#define __APLCARDFILE_H__

namespace eIDMW
{

typedef enum
{
	CARDFILESTATUS_UNREAD=-1,		/**< File not read yet */
	CARDFILESTATUS_OK=0,			/**< File ok */
	CARDFILESTATUS_ERROR=1,			/**< VerifyFile return an error */
	CARDFILESTATUS_ERROR_SIGNATURE,	/**< Signature checking failed */
	CARDFILESTATUS_ERROR_HASH,		/**< Hash checking failed */
	CARDFILESTATUS_ERROR_TEST,		/**< Validate with test certificate */
	CARDFILESTATUS_ERROR_DATE,		/**< Validate with certificate containing bad date */
	CARDFILESTATUS_ERROR_CERT,		/**< Validation of certificate failed */
	CARDFILESTATUS_ERROR_NOFILE,	/**< The file doesn't exist */
	CARDFILESTATUS_ERROR_FORMAT		/**< The format of the file is wrong */
} tCardFileStatus;

}

#endif //__APLCARDFILE_H__
