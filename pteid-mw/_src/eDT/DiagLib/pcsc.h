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
#ifndef __DIAGLIB_PCSC_H__
#define __DIAGLIB_PCSC_H__

#include <vector>

#include "reader.h"

int pcscIsAvailable(bool *available);

int pcscGetReaderList(Reader_LIST *readersList);
int pcscGetCardList(Card_LIST *cardList);
int pcscGetCardInfo(Card_ID id, Card_INFO *info);

//Run the connect test once
int pcscTestConnectOnce(Card_ID id, bool *success, int delay);	

//Run the connect test MaxNbrOfSuccess of time and return the number of consecutive success
int pcscTestConnect(Card_ID id, int MaxNbrOfSuccess, int delay, int *NbrOfSuccess); 

//Find the optimum delay.
//The first delay tested is delayMin and, in case of failure, it increase every delayStep until delayMax.
//The first delay that pass NbrOfSuccess consecutive success, is return in delayOptimum
int pcscOptimizeConnect(Card_ID id, int NbrOfSuccess, int delayMin,	int delayMax, int delayStep, int *delayOptimum);

//Change the default connect timing for future use
int pcscSetConnectTiming(int Delay);



//Run the transmit test once
int pcscTestTransmitOnce(Card_ID id, bool *success, int delay); 

//Run the transmit test MaxNbrOfSuccess of time and return the number of consecutive success
int pcscTestTransmit(Card_ID id, int MaxNbrOfSuccess, int delay, int *NbrOfSuccess);

//Find the optimum delay.
//The first delay tested is delayMin and, in case of failure, it increase every delayStep until delayMax.
//The first delay that pass NbrOfSuccess consecutive success, is return in delayOptimum
int pcscOptimizeTransmit(Card_ID id, int NbrOfSuccess, int delayMin, int delayMax, int delayStep, int *delayOptimum);

//Change the default transmit timing for future use
int pcscSetTransmitTiming(int Delay);


#endif //__DIAGLIB_PCSC_H__

