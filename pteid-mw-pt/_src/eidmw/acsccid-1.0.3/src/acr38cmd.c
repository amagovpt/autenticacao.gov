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

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pcsclite.h>
#include <ifdhandler.h>
#include <reader.h>

#include "misc.h"
#include "acr38cmd.h"
#include "openct/proto-t1.h"
#include "ccid.h"
#include "defs.h"
#include "ccid_ifdhandler.h"
#include "debug.h"
#include "ccid_usb.h"

#define ACR38_HEADER_SIZE		4
#define ACR38_STATUS_OFFSET		1

// Card type
#define CARD_TYPE_MCU_AUTO		0
#define CARD_TYPE_I2C_AUTO		1	// Not supported
#define CARD_TYPE_I2C_1K		2
#define CARD_TYPE_I2C_2K		3
#define CARD_TYPE_I2C_4K		4
#define CARD_TYPE_I2C_8K		5
#define CARD_TYPE_I2C_16K		6
#define CARD_TYPE_I2C_32K		7
#define CARD_TYPE_I2C_64K		8
#define CARD_TYPE_I2C_128K		9
#define CARD_TYPE_I2C_256K		10
#define CARD_TYPE_I2C_512K		11
#define CARD_TYPE_I2C_1024K		12
#define CARD_TYPE_AT88SC153		13
#define CARD_TYPE_AT88SC1608	14
#define CARD_TYPE_SLE4418		15
#define CARD_TYPE_SLE4428		16
#define CARD_TYPE_SLE4432		17
#define CARD_TYPE_SLE4442		18
#define CARD_TYPE_SLE4406		19
#define CARD_TYPE_SLE4436		20
#define CARD_TYPE_SLE5536		21
#define CARD_TYPE_MCU_T0		22
#define CARD_TYPE_MCU_T1		23
#define CARD_TYPE_AUTO			24	// Not supported

// ACR38 card type
#define ACR38_CARD_TYPE_MCU_AUTO			0x00
#define ACR38_CARD_TYPE_I2C_1K_16K			0x01
#define ACR38_CARD_TYPE_I2C_32K_1024K		0x02
#define ACR38_CARD_TYPE_AT88SC153			0x03
#define ACR38_CARD_TYPE_AT88SC1608			0x04
#define ACR38_CARD_TYPE_SLE4418_4428		0x05
#define ACR38_CARD_TYPE_SLE4432_4442		0x06
#define ACR38_CARD_TYPE_SLE4406_4436_5536	0x07
#define ACR38_CARD_TYPE_MCU_T0				0x0C
#define ACR38_CARD_TYPE_MCU_T1				0x0D

// ACR38 option
#define ACR38_OPTION_NONE			0x00	// No option
#define ACR38_OPTION_EMV_MODE		0x10	// EMV mode
#define ACR38_OPTION_MEMCARD_MODE	0x20	// Memory card mode

// Fix problem using pcsc-lite 1.6.x header files
#ifndef IFD_ERROR_INSUFFICIENT_BUFFER
#define IFD_ERROR_INSUFFICIENT_BUFFER 618
#endif

static RESPONSECODE ACR38_CmdXfrBlockTPDU_T0(unsigned int reader_index,
	unsigned int tx_length, unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[]);

static RESPONSECODE ACR38_CmdXfrBlockTPDU_T1(unsigned int reader_index,
	unsigned int tx_length, unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[]);

static RESPONSECODE ACR38_CmdSelectCardType(unsigned int reader_index, unsigned char card_type);

static RESPONSECODE ACR38_CmdSetOption(unsigned int reader_index, unsigned char option);

RESPONSECODE ACR38_CmdPowerOn(unsigned int reader_index, unsigned int *nlength,
	/*@out@*/ unsigned char buffer[], int voltage)
{
	unsigned char cmd[6];
	unsigned int cmdLen;
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);
	unsigned char option;

	/* store length of buffer[] */
	length = *nlength;

	if (ccid_descriptor->bCurrentSlotIndex == 0)
	{
		// Set option
		if ((ccid_descriptor->cardType == ACR38_CARD_TYPE_MCU_AUTO) ||
			(ccid_descriptor->cardType == ACR38_CARD_TYPE_MCU_T0) ||
			(ccid_descriptor->cardType == ACR38_CARD_TYPE_MCU_T1))
		{
			option = ACR38_OPTION_NONE;
		}
		else
		{
			option = ACR38_OPTION_MEMCARD_MODE;
		}
		(void)ACR38_CmdSetOption(reader_index, option);

		// Select card type
		(void)ACR38_CmdSelectCardType(reader_index, ccid_descriptor->cardType);
	}

	if (ccid_descriptor->bCurrentSlotIndex == 0)
	{
		// RESET_WITH_SPECIFIC_VOLTAGE
		cmd[0] = 0x01;
		cmd[1] = 0x80;
		cmd[2] = 0x00;
		cmd[3] = 0x01;
		cmd[4] = ccid_descriptor->cardVoltage;
		cmdLen = 5;
	}
	else
	{
		// Power off SAM
		(void)ACR38_CmdPowerOff(reader_index);
		usleep(100 * 1000);

		// RESET_WITH_5_VOLTS_DEFAULT_SAM
		cmd[0] = 0x01;
		cmd[1] = 0x90;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmdLen = 4;
	}

	res = WritePort(reader_index, cmdLen, cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	/* reset available buffer size */
	/* needed if we go back after a switch to ISO mode */
	*nlength = length;

	res = ReadPort(reader_index, nlength, buffer);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (buffer[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(buffer[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return IFD_COMMUNICATION_ERROR;
	}

	// If memory card had been selected
	if ((ccid_descriptor->bCurrentSlotIndex == 0) &&
		(option == ACR38_OPTION_MEMCARD_MODE))
	{
		unsigned char buffer_tmp[16];
		unsigned int bufferLen = sizeof(buffer_tmp);

		// SELECT_CARD_TYPE
		cmd[0] = 0xFF;
		cmd[1] = 0xA4;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x01;
		cmd[5] = ccid_descriptor->cardType;
		cmdLen = 6;

		(void)ACR38_CmdXfrBlockTPDU_T0(reader_index, cmdLen, cmd, &bufferLen, buffer_tmp);
	}

	/* extract the ATR */
	*nlength -= ACR38_HEADER_SIZE;
	memmove(buffer, buffer + ACR38_HEADER_SIZE, *nlength);

	return return_value;
}

RESPONSECODE ACR38_CmdPowerOff(unsigned int reader_index)
{
	unsigned char cmd[4];
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	// POWER_OFF or POWER_OFF_SAM
	cmd[0] = 0x01;
	cmd[1] = ccid_descriptor->bCurrentSlotIndex == 0 ? 0x81 : 0x91;
	cmd[2] = 0x00;
	cmd[3] = 0x00;

	res = WritePort(reader_index, sizeof(cmd), cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	length = sizeof(cmd);
	res = ReadPort(reader_index, &length, cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (cmd[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(cmd[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return_value = IFD_COMMUNICATION_ERROR;
	}

	return return_value;
}

RESPONSECODE ACR38_CmdGetSlotStatus(unsigned int reader_index,
	/*@out@*/ unsigned char buffer[])
{
	unsigned char cmd[4];
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;
	unsigned char buffer_tmp[20];
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	if (ccid_descriptor->bCurrentSlotIndex == 0)
	{
		// GET_ACR_STAT
		cmd[0] = 0x01;
		cmd[1] = 0x01;
		cmd[2] = 0x00;
		cmd[3] = 0x00;

		res = WritePort(reader_index, sizeof(cmd), cmd);
		if (res != STATUS_SUCCESS)
		{
			if (STATUS_NO_SUCH_DEVICE == res)
				return IFD_NO_SUCH_DEVICE;
			return IFD_COMMUNICATION_ERROR;
		}

		length = sizeof(buffer_tmp);
		res = ReadPort(reader_index, &length, buffer_tmp);
		if (res != STATUS_SUCCESS)
			return IFD_COMMUNICATION_ERROR;

		if (length < sizeof(buffer_tmp))
		{
			DEBUG_CRITICAL2("Not enough data received: %d bytes", length);
			return IFD_COMMUNICATION_ERROR;
		}

		if (buffer_tmp[ACR38_STATUS_OFFSET] != 0)
		{
			acr38_error(buffer_tmp[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
			return IFD_COMMUNICATION_ERROR;
		}

		/* simulate a CCID bStatus */
		switch (buffer_tmp[19])
		{
			case 3:
				buffer[7] = CCID_ICC_PRESENT_ACTIVE;
				break;

			case 1:
				buffer[7] = CCID_ICC_PRESENT_INACTIVE;
				break;

			case 0:
				buffer[7] = CCID_ICC_ABSENT;
				break;

			default:
				return_value = IFD_COMMUNICATION_ERROR;
				break;
		}
	}
	else
	{
		// SAM card is always present
		buffer[7] = CCID_ICC_PRESENT_ACTIVE;
	}

	return return_value;
}

RESPONSECODE ACR38_CmdXfrBlock(unsigned int reader_index, unsigned int tx_length,
	unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[], int protocol)
{
	RESPONSECODE return_value = IFD_SUCCESS;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);
	int old_read_timeout;

	// Use infinite timeout in T=0, short APDU and extended APDU
	old_read_timeout = ccid_descriptor->readTimeout;

	/* APDU or TPDU? */
	switch (ccid_descriptor->dwFeatures & CCID_CLASS_EXCHANGE_MASK)
	{
		case CCID_CLASS_TPDU:
			if (protocol == T_0)
			{
				ccid_descriptor->readTimeout = 0;	// Infinite
				return_value = ACR38_CmdXfrBlockTPDU_T0(reader_index,
					tx_length, tx_buffer, rx_length, rx_buffer);
			}
			else if (protocol == T_1)
			{
				return_value = ACR38_CmdXfrBlockTPDU_T1(reader_index, tx_length,
					tx_buffer, rx_length, rx_buffer);
			}
			else
				return_value = IFD_PROTOCOL_NOT_SUPPORTED;
			break;

		default:
			return_value = IFD_COMMUNICATION_ERROR;
			break;
	}

	ccid_descriptor->readTimeout = old_read_timeout;
	return return_value;
}

RESPONSECODE ACR38_TransmitT0(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI)
{
	unsigned char cmd[ACR38_HEADER_SIZE + tx_length];	/* Header + APDU buffer */
	status_t ret;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	// EXCHANGE_TPDU_T0 or EXCHANGE_SAM_TPDU_T0
	cmd[0] = 0x01;
	cmd[1] = ccid_descriptor->bCurrentSlotIndex == 0 ? 0xA0 : 0xB0;
	cmd[2] = (tx_length >> 8) & 0xFF;
	cmd[3] = tx_length & 0xFF;
	memcpy(cmd + ACR38_HEADER_SIZE, tx_buffer, tx_length);

	ret = WritePort(reader_index, ACR38_HEADER_SIZE + tx_length, cmd);
	if (STATUS_NO_SUCH_DEVICE == ret)
		return IFD_NO_SUCH_DEVICE;
	if (ret != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	return IFD_SUCCESS;
}

RESPONSECODE ACR38_TransmitT1(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI)
{
	unsigned char cmd[ACR38_HEADER_SIZE + tx_length];	/* Header + APDU buffer */
	status_t ret;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	// EXCHANGE_TPDU_T1 or EXCHANGE_SAM_TPDU_T1
	cmd[0] = 0x01;
	cmd[1] = ccid_descriptor->bCurrentSlotIndex == 0 ? 0xA1 : 0xB1;
	cmd[2] = (tx_length >> 8) & 0xFF;
	cmd[3] = tx_length & 0xFF;
	memcpy(cmd + ACR38_HEADER_SIZE, tx_buffer, tx_length);

	ret = WritePort(reader_index, ACR38_HEADER_SIZE + tx_length, cmd);
	if (STATUS_NO_SUCH_DEVICE == ret)
		return IFD_NO_SUCH_DEVICE;
	if (ret != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	return IFD_SUCCESS;
}

RESPONSECODE ACR38_TransmitPPS(unsigned int reader_index, unsigned int tx_length,
	const unsigned char tx_buffer[], unsigned short rx_length, unsigned char bBWI)
{
	unsigned char cmd[ACR38_HEADER_SIZE + tx_length];	/* Header + APDU buffer */
	status_t ret;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	// SET_CARD_PPS or SET_SAM_CARD_PPS
	cmd[0] = 0x01;
	cmd[1] = ccid_descriptor->bCurrentSlotIndex == 0 ? 0x0A : 0x0C;
	cmd[2] = (tx_length >> 8) & 0xFF;
	cmd[3] = tx_length & 0xFF;
	memcpy(cmd + ACR38_HEADER_SIZE, tx_buffer, tx_length);

	ret = WritePort(reader_index, ACR38_HEADER_SIZE + tx_length, cmd);
	if (STATUS_NO_SUCH_DEVICE == ret)
		return IFD_NO_SUCH_DEVICE;
	if (ret != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	return IFD_SUCCESS;
}

RESPONSECODE ACR38_Receive(unsigned int reader_index,
	/*@out@*/ unsigned int *rx_length,
	/*@out@*/ unsigned char rx_buffer[], unsigned char *chain_parameter)
{
	unsigned char cmd[ACR38_HEADER_SIZE + CMD_BUF_SIZE];	/* Header + APDU buffer */
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;
	status_t ret;

	length = sizeof(cmd);
	ret = ReadPort(reader_index, &length, cmd);
	if (ret != STATUS_SUCCESS)
	{
		if (STATUS_NO_SUCH_DEVICE == ret)
			return IFD_NO_SUCH_DEVICE;
		return IFD_COMMUNICATION_ERROR;
	}

	if (cmd[ACR38_STATUS_OFFSET] != 0)
	{
		ccid_error(cmd[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return IFD_COMMUNICATION_ERROR;
	}

	length -= ACR38_HEADER_SIZE;
	if (length <= *rx_length)
		*rx_length = length;
	else
	{
		DEBUG_CRITICAL2("overrun by %d bytes", length - *rx_length);
		length = *rx_length;
		return_value = IFD_ERROR_INSUFFICIENT_BUFFER;
	}

	memcpy(rx_buffer, cmd + ACR38_HEADER_SIZE, length);

	return return_value;
}

RESPONSECODE ACR38_SetParameters(unsigned int reader_index, char protocol,
	unsigned int length, unsigned char buffer[])
{
	unsigned char cmd[8];
	int i;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	DEBUG_COMM2("length: %d bytes", length);

	// SET_READER_PPS or SET_SAM_READER_PPS
	cmd[0] = 0x01;
	cmd[1] = ccid_descriptor->bCurrentSlotIndex == 0 ? 0x0B : 0x0D;
	cmd[2] = 0x00;
	cmd[3] = 0x04;
	cmd[4] = 0xFF;							// PPSS
	cmd[5] = protocol == 0 ? 0x10 : 0x11;	// PPS0: 0x10 (T=0), 0x11 (T=1)
	cmd[6] = buffer[0];						// PPS1: TA(1)
	cmd[7] = 0;								// PCK: XOR checksum
	for (i = 4; i < 7; i++)
		cmd[7] ^= cmd[i];

	if (WritePort(reader_index, sizeof(cmd), cmd) != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	length = sizeof(cmd);
	if (ReadPort(reader_index, &length, cmd) != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (cmd[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(cmd[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return IFD_COMMUNICATION_ERROR;
	}

	return IFD_SUCCESS;
}

RESPONSECODE ACR38_SetCardVoltage(unsigned int reader_index,
	unsigned char TxBuffer[], unsigned int TxLength,
	unsigned char RxBuffer[], unsigned int *RxLength)
{
	RESPONSECODE return_value = IFD_SUCCESS;
	unsigned char cardVoltage;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	if ((TxBuffer != NULL) && (TxLength >= 1))
	{
		cardVoltage = TxBuffer[0];
		if (cardVoltage <= 3)
			ccid_descriptor->cardVoltage = cardVoltage;
		else
		{
			DEBUG_CRITICAL2("Card voltage %d is not supported", cardVoltage);
			ccid_descriptor->cardVoltage = 0;
			return_value = IFD_COMMUNICATION_ERROR;
		}
	}

	if (RxLength != NULL)
		*RxLength = 0;

	DEBUG_INFO2("cardVoltage: %d", ccid_descriptor->cardVoltage);
	return return_value;
}

RESPONSECODE ACR38_SetCardType(unsigned int reader_index,
	unsigned char TxBuffer[], unsigned int TxLength,
	unsigned char RxBuffer[], unsigned int *RxLength)
{
	RESPONSECODE return_value = IFD_SUCCESS;
	unsigned long cardType;
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	if ((TxBuffer != NULL) && (TxLength >= sizeof(cardType)))
	{
		cardType = *((unsigned long *) TxBuffer);
		switch (cardType)
		{
			case CARD_TYPE_MCU_AUTO:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_MCU_AUTO;
				break;

			case CARD_TYPE_I2C_1K:
			case CARD_TYPE_I2C_2K:
			case CARD_TYPE_I2C_4K:
			case CARD_TYPE_I2C_8K:
			case CARD_TYPE_I2C_16K:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_I2C_1K_16K;
				break;

			case CARD_TYPE_I2C_32K:
			case CARD_TYPE_I2C_64K:
			case CARD_TYPE_I2C_128K:
			case CARD_TYPE_I2C_256K:
			case CARD_TYPE_I2C_512K:
			case CARD_TYPE_I2C_1024K:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_I2C_32K_1024K;
				break;

			case CARD_TYPE_AT88SC153:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_AT88SC153;
				break;

			case CARD_TYPE_AT88SC1608:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_AT88SC1608;
				break;

			case CARD_TYPE_SLE4418:
			case CARD_TYPE_SLE4428:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_SLE4418_4428;
				break;

			case CARD_TYPE_SLE4432:
			case CARD_TYPE_SLE4442:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_SLE4432_4442;
				break;

			case CARD_TYPE_SLE4406:
			case CARD_TYPE_SLE4436:
			case CARD_TYPE_SLE5536:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_SLE4406_4436_5536;
				break;

			case CARD_TYPE_MCU_T0:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_MCU_T0;
				break;

			case CARD_TYPE_MCU_T1:
				ccid_descriptor->cardType = ACR38_CARD_TYPE_MCU_T1;
				break;

			default:
				DEBUG_CRITICAL2("Card type %d is not supported", cardType);
				ccid_descriptor->cardType = ACR38_CARD_TYPE_MCU_AUTO;
				return_value = IFD_COMMUNICATION_ERROR;
				break;
		}
	}

	if (RxLength != NULL)
		*RxLength = 0;

	DEBUG_INFO2("cardType: %d", ccid_descriptor->cardType);
	return return_value;
}

RESPONSECODE ACR38_GetFirmwareVersion(unsigned int reader_index,
	char firmwareVersion[])
{
	unsigned char cmd[4];
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;
	unsigned char buffer_tmp[20];
	_ccid_descriptor *ccid_descriptor = get_ccid_descriptor(reader_index);

	// GET_ACR_STAT
	cmd[0] = 0x01;
	cmd[1] = 0x01;
	cmd[2] = 0x00;
	cmd[3] = 0x00;

	res = WritePort(reader_index, sizeof(cmd), cmd);
	if (res != STATUS_SUCCESS)
	{
		if (STATUS_NO_SUCH_DEVICE == res)
			return IFD_NO_SUCH_DEVICE;
		return IFD_COMMUNICATION_ERROR;
	}

	length = sizeof(buffer_tmp);
	res = ReadPort(reader_index, &length, buffer_tmp);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (length < sizeof(buffer_tmp))
	{
		DEBUG_CRITICAL2("Not enough data received: %d bytes", length);
		return IFD_COMMUNICATION_ERROR;
	}

	if (buffer_tmp[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(buffer_tmp[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return IFD_COMMUNICATION_ERROR;
	}

	memcpy(firmwareVersion, buffer_tmp + ACR38_HEADER_SIZE, 10);
	return return_value;
}

static RESPONSECODE ACR38_CmdXfrBlockTPDU_T0(unsigned int reader_index,
	unsigned int tx_length, unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[])
{
	RESPONSECODE return_value = IFD_SUCCESS;

	DEBUG_COMM2("T=0: %d bytes", tx_length);

	return_value = ACR38_TransmitT0(reader_index, tx_length, tx_buffer, 0, 0);
	if (return_value != IFD_SUCCESS)
		return return_value;

	return ACR38_Receive(reader_index, rx_length, rx_buffer, NULL);
}

static RESPONSECODE ACR38_CmdXfrBlockTPDU_T1(unsigned int reader_index,
	unsigned int tx_length, unsigned char tx_buffer[], unsigned int *rx_length,
	unsigned char rx_buffer[])
{
	RESPONSECODE return_value = IFD_SUCCESS;
	int ret;

	DEBUG_COMM3("T=1: %d and %d bytes", tx_length, *rx_length);

	ret = t1_transceive(&((get_ccid_slot(reader_index)) -> t1), 0,
		tx_buffer, tx_length, rx_buffer, *rx_length);

	if (ret < 0)
		return_value = IFD_COMMUNICATION_ERROR;
	else
		*rx_length = ret;

	return return_value;
}

static RESPONSECODE ACR38_CmdSelectCardType(unsigned int reader_index, unsigned char card_type)
{
	unsigned char cmd[5];
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;

	// SELECT_CARD_TYPE
	cmd[0] = 0x01;
	cmd[1] = 0x02;
	cmd[2] = 0x00;
	cmd[3] = 0x01;
	cmd[4] = card_type;

	res = WritePort(reader_index, sizeof(cmd), cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	length = sizeof(cmd);
	res = ReadPort(reader_index, &length, cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (cmd[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(cmd[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return_value = IFD_COMMUNICATION_ERROR;
	}

	return return_value;
}

static RESPONSECODE ACR38_CmdSetOption(unsigned int reader_index, unsigned char option)
{
	unsigned char cmd[5];
	status_t res;
	unsigned int length;
	RESPONSECODE return_value = IFD_SUCCESS;

	// SET_OPTION
	cmd[0] = 0x01;
	cmd[1] = 0x07;
	cmd[2] = 0x00;
	cmd[3] = 0x01;
	cmd[4] = option;

	res = WritePort(reader_index, sizeof(cmd), cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	length = sizeof(cmd);
	res = ReadPort(reader_index, &length, cmd);
	if (res != STATUS_SUCCESS)
		return IFD_COMMUNICATION_ERROR;

	if (cmd[ACR38_STATUS_OFFSET] != 0)
	{
		acr38_error(cmd[ACR38_STATUS_OFFSET], __FILE__, __LINE__, __FUNCTION__);
		return_value = IFD_COMMUNICATION_ERROR;
	}

	return return_value;
}
