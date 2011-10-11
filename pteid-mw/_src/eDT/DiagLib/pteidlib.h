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
#ifndef __DIAGLIB_PTEIDLIB_H__
#define __DIAGLIB_PTEIDLIB_H__

#include "reader.h"

/*
extern "C"
{

int pteidlibIsAvailable(bool *available);
int pteidlibGetReaderList(Reader_LIST *readersList);
int pteidlibGetCardList(Card_LIST *cardList);
int pteidlibGetCardInfo (Card_ID id, Card_INFO *info);
int pteidlibIsOcspAvailable(Card_ID id, bool *available);

}
*/

typedef int(* fct_pteidlibIsAvailable)(bool *available);
typedef int(* fct_pteidlibGetReaderList)(Reader_LIST *readersList);
typedef int(* fct_pteidlibGetCardList)(Card_LIST *cardList);
typedef int(* fct_pteidlibGetCardInfo) (Card_ID id, Card_INFO *info);
typedef int(* fct_pteidlibIsOcspAvailable)(Card_ID id, bool *available);

typedef struct t_PTeidlibWrapper {
	HMODULE hPTeidLib;
	fct_pteidlibIsAvailable pteidlibIsAvailable;
	fct_pteidlibGetReaderList pteidlibGetReaderList;
	fct_pteidlibGetCardList pteidlibGetCardList;
	fct_pteidlibGetCardInfo pteidlibGetCardInfo;
	fct_pteidlibIsOcspAvailable pteidlibIsOcspAvailable;
} PTeidlibWrapper;

PTeidlibWrapper *loadPTeidLibWrapper();
void unloadPTeidLibWrapper();


#endif //__DIAGLIB_PTEIDLIB_H__

