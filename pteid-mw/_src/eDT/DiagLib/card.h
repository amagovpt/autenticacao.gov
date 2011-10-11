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
#ifndef __DIAGLIB_CARD_H__
#define __DIAGLIB_CARD_H__

#include <string.h>
#include <vector>

#include "report.h"

#include "reader.h"

int cardGetListFromPcsc(Card_LIST *cardList);
int cardGetListFromMW(Card_LIST *cardList);
int cardGetInfo(Card_ID reader, Card_INFO *info);

int cardReportInfo(Report_TYPE type, const Card_INFO &info);
int cardReportList(Report_TYPE type, const Card_LIST &cardList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_CARD_H__

