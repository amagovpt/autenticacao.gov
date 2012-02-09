/*
	acr38cmd.h: Emulated CCID reader commands for ACR38 non-CCID reader
	Copyright (C) 2011-2012   Advanced Card Systems Ltd.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this library; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef ACR38CMD_H
#define ACR38CMD_H

RESPONSECODE ACR38_CmdPowerOn(unsigned int reader_index, unsigned int *nlength,
	/*@out@*/ unsigned char buffer[], int voltage);

RESPONSECODE ACR38_CmdPowerOff(unsigned int reader_index);

RESPONSECODE ACR38_CmdGetSlotStatus(unsigned int reader_index,
	/*@out@*/ unsigned char buffer[]);

RESPONSECODE ACR38_CmdXfrBlock(unsigned int reader_index, unsigned int tx_length,
	unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[], int protoccol);

RESPONSECODE ACR38_TransmitT0(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI);

RESPONSECODE ACR38_TransmitT1(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI);

RESPONSECODE ACR38_TransmitPPS(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI);

RESPONSECODE ACR38_Receive(unsigned int reader_index,
	/*@out@*/ unsigned int *rx_length,
	/*@out@*/ unsigned char rx_buffer[], unsigned char *chain_parameter);

RESPONSECODE ACR38_SetParameters(unsigned int reader_index, char protocol,
	unsigned int length, unsigned char buffer[]);

RESPONSECODE ACR38_SetCardVoltage(unsigned int reader_index,
	unsigned char TxBuffer[], unsigned int TxLength,
	unsigned char RxBuffer[], unsigned int *RxLength);

RESPONSECODE ACR38_SetCardType(unsigned int reader_index,
	unsigned char TxBuffer[], unsigned int TxLength,
	unsigned char RxBuffer[], unsigned int *RxLength);

RESPONSECODE ACR38_GetFirmwareVersion(unsigned int reader_index,
	char firmwareVersion[]);

#endif
