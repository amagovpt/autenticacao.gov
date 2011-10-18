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
#ifndef __DIAGLIB_BEIDLIB_H__
#define __DIAGLIB_BEIDLIB_H__

#include "reader.h"

/*
extern "C"
{

int beidlibIsAvailable(bool *available);
int beidlibGetReaderList(Reader_LIST *readersList);
int beidlibGetCardList(Card_LIST *cardList);
int beidlibGetCardInfo (Card_ID id, Card_INFO *info);
int beidlibIsOcspAvailable(Card_ID id, bool *available);

}
*/

typedef int(* fct_beidlibIsAvailable)(bool *available);
typedef int(* fct_beidlibGetReaderList)(Reader_LIST *readersList);
typedef int(* fct_beidlibGetCardList)(Card_LIST *cardList);
typedef int(* fct_beidlibGetCardInfo) (Card_ID id, Card_INFO *info);
typedef int(* fct_beidlibIsOcspAvailable)(Card_ID id, bool *available);

typedef struct t_BeidlibWrapper {
	HMODULE hBeidLib;
	fct_beidlibIsAvailable beidlibIsAvailable;
	fct_beidlibGetReaderList beidlibGetReaderList;
	fct_beidlibGetCardList beidlibGetCardList;
	fct_beidlibGetCardInfo beidlibGetCardInfo;
	fct_beidlibIsOcspAvailable beidlibIsOcspAvailable;
} BeidlibWrapper;

BeidlibWrapper *loadBeidLibWrapper();
void unloadBeidLibWrapper();


#endif //__DIAGLIB_BEIDLIB_H__

