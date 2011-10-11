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

#ifndef __CARDSISDEF_H__
#define __CARDSISDEF_H__

namespace eIDMW
{

#define SIS_FILE_ID					"dada"

#define SIS_FIELD_ID_POS_Name						165
#define SIS_FIELD_ID_LEN_Name						24
#define SIS_FIELD_ID_POS_Surname					117
#define SIS_FIELD_ID_LEN_Surname					48
#define SIS_FIELD_ID_POS_Initials					189
#define SIS_FIELD_ID_LEN_Initials					1
#define SIS_FIELD_ID_POS_Gender						190
#define SIS_FIELD_ID_LEN_Gender						1
#define SIS_FIELD_ID_POS_DateOfBirth				191
#define SIS_FIELD_ID_LEN_DateOfBirth				4
#define SIS_FIELD_ID_POS_SocialSecurityNumber		107
#define SIS_FIELD_ID_LEN_SocialSecurityNumber		6
#define SIS_FIELD_ID_POS_LogicalNumber				80
#define SIS_FIELD_ID_LEN_LogicalNumber				4
#define SIS_FIELD_ID_POS_DateOfIssue				113
#define SIS_FIELD_ID_LEN_DateOfIssue				4
#define SIS_FIELD_ID_POS_ValidityBeginDate			84
#define SIS_FIELD_ID_LEN_ValidityBeginDate			4
#define SIS_FIELD_ID_POS_ValidityEndDate			88
#define SIS_FIELD_ID_LEN_ValidityEndDate			4

#define CARDTYPE_NAME_PTEID_SIS						"sis"

#define SIS_TLV_TAG_VERSION							0x00
#define SIS_TLV_TAG_FILE_ID							0x01
#define SIS_TLV_TAG_CARDTYPE						0x0B

#define SIS_CSV_TAG_VERSION							0
#define SIS_CSV_TAG_CARDTYPE						1
#define SIS_CSV_TAG_FILE_ID							12

#define SIS_XML_PATH_FILE_ID	"/pteid_card/data/files/file_data"

#define SIS_FILE_ID_SIZE							404

}

#endif //__CARDSISDEF_H__
