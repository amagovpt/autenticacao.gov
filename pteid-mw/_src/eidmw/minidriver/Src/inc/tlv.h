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
#include "globmdrv.h"

#define ID_FILE_STRUCTURE_VERSION          0x00
#define ID_CARD_NUMBER                     0x01
#define ID_CHIP_NUMBER                     0x02
#define ID_CARD_VALIDITY_BEGIN             0x03
#define ID_CARD_VALIDITY_END               0x04
#define ID_CARD_DELIVIRY_MUNICIPALITY      0x05
#define ID_NATIONAL_NUMBER                 0x06
#define ID_NAME                            0x07
#define ID_FIRST_TWO_GIVEN_NAMES           0x08
#define ID_FIRST_LETTER_THIRD_GIVEN_NAME   0x09
#define ID_NATIONALITY                     0x0A
#define ID_BIRTH_LOCATION                  0x0B
#define ID_BIRTH_DATE                      0x0C
#define ID_SEX                             0x0D
#define ID_NOBLE_CONDITION                 0x0E
#define ID_DOCUMENT_TYPE                   0x0F
#define ID_SPECIAL_STATUS                  0x10
#define ID_PHOTO_HASH                      0x11

#define ID_ADDRESS_STREET_NUMBER           0x01
#define ID_ADDRESS_ZIP                     0x02
#define ID_ADDRESS_MUNICIPALITY            0x03

extern DWORD TLVGetField(PBYTE pbFile, DWORD dwFileLen, PBYTE pbData, DWORD cbData, PDWORD pdwDataLen, BYTE bField);