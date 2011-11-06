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
#ifndef __DIAGLIB_MIDDLEWARE_H__
#define __DIAGLIB_MIDDLEWARE_H__

#include <string.h>
#include <vector>

#include "report.h"

#include "software.h"
#include "reader.h"
#include "card.h"

typedef enum e_MW_VERSION {
	MW_VERSION_23,
	MW_VERSION_24,
	MW_VERSION_25,
	MW_VERSION_26,
	MW_VERSION_30,
	MW_VERSION_35_IS,
	MW_VERSION_35_PRO_IS,
	MW_VERSION_35_MSI,
	MW_VERSION_35_PRO_MSI,
	MW_VERSION_351_OR_GREATER,
	MW_VERSION_351_PRO_OR_GREATER,
	MW_VERSION_MAC
} MW_VERSION;

typedef Soft_ID MW_ID;
typedef std::vector<MW_ID> MW_LIST;

typedef struct t_MW_INFO {
	MW_ID id;
	MW_VERSION Version;
	int BuildNumber;
	std::wstring LabelVersion;
	std::wstring DisplayName;
} MW_INFO;

int mwGetList(MW_LIST *middlewareList);
int mwGetInfo (MW_ID middleware, MW_INFO *info);

int mwIsPteidlibAvailable(bool *available);
int mwGetReaderList(Reader_LIST *readersList);
int mwGetCardList(Card_LIST *cardList);
int mwGetCardInfo (Card_ID id, Card_INFO *info);

int mwUninstall (MW_ID middleware);

void cardContributeInfo(const Card_INFO &info);
int mwReportInfo(Report_TYPE type, const MW_INFO &info);
int mwReportList(Report_TYPE type, const MW_LIST &mwList, const wchar_t *Title=NULL);

#endif //__DIAGLIB_MIDDLEWARE_H__

