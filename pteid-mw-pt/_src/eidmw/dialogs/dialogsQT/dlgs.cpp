/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012, 2016-2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
/********************************************************************************
********************************************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "errno.h"

#include "../dialogs.h"
#include "../langUtil.h"

#include <map>

#include "Log.h"
#include "Util.h"
#include "MWException.h"
#include "eidErrors.h"
#include "Config.h"
#include "Thread.h"
#include "prefix.h"

using namespace eIDMW;

std::map<unsigned long, DlgRunningProc *> dlgPinPadInfoCollector;
unsigned long dlgPinPadInfoCollectorIndex = 0;

std::map<unsigned long, DlgRunningProc *> dlgCMDMsgCollector;
unsigned long dlgCMDMsgCollectorIndex = 0;

pid_t current_dlg_pid = 0;

std::string csServerName = "pteiddialogsQTsrv";

bool bRandInitialized = false;

static bool g_bSystemCallsFail = false;

/************************
 *       DIALOGS
 ************************/

// TODO: Add Keypad possibility in DlgAskPin(s)
void eIDMW::writeAskPinArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgAskPINArguments *pinArg = (DlgAskPINArguments *)(arg);
	int len = sizeof(pinArg->operation) + sizeof(pinArg->usage) + 50 * sizeof(wchar_t) + sizeof(pinArg->pinInfo) +
			  (PIN_MAX_LENGTH + 1) * sizeof(wchar_t) + sizeof(pinArg->returnValue);

	buffer = (char *)malloc(len);
	initBuffer = buffer;
	memcpy(buffer, &pinArg->operation, sizeof(pinArg->operation));
	buffer += sizeof(pinArg->operation);
	memcpy(buffer, &pinArg->usage, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(buffer, &pinArg->pinName, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(buffer, &pinArg->pinInfo, sizeof(pinArg->pinInfo));
	buffer += sizeof(pinArg->pinInfo);
	memcpy(buffer, &pinArg->pin, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(buffer, &pinArg->returnValue, sizeof(pinArg->returnValue));
	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::readAskPinArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgAskPINArguments *pinArg = (DlgAskPINArguments *)(arg);
	int len = sizeof(pinArg->operation) + sizeof(pinArg->usage) + (50 * sizeof(wchar_t)) + sizeof(pinArg->pinInfo) +
			  ((PIN_MAX_LENGTH + 1) * sizeof(wchar_t)) + sizeof(pinArg->returnValue);
	buffer = (char *)malloc(len);
	read(fd, buffer, len);
	initBuffer = buffer;

	memcpy(&pinArg->operation, buffer, sizeof(pinArg->operation));
	buffer += sizeof(pinArg->operation);
	memcpy(&pinArg->usage, buffer, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(&pinArg->pinName, buffer, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(&pinArg->pinInfo, buffer, sizeof(pinArg->pinInfo));
	buffer += sizeof(pinArg->pinInfo);
	memcpy(&pinArg->pin, buffer, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(&pinArg->returnValue, buffer, sizeof(pinArg->returnValue));
	free(initBuffer);
}

void eIDMW::writeAskPinsArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgAskPINsArguments *pinArg = (DlgAskPINsArguments *)(arg);
	int len = sizeof(pinArg->operation) + sizeof(pinArg->usage) + 50 * sizeof(wchar_t) + sizeof(pinArg->pin1Info) +
			  (PIN_MAX_LENGTH + 1) * sizeof(wchar_t) +  sizeof(pinArg->pin2Info) +
			  (PIN_MAX_LENGTH + 1) * sizeof(wchar_t) + sizeof(pinArg->returnValue);

	buffer = (char *)malloc(len);
	initBuffer = buffer;
	memcpy(buffer, &pinArg->operation, sizeof(pinArg->operation));
	buffer += sizeof(pinArg->operation);
	memcpy(buffer, &pinArg->usage, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(buffer, &pinArg->pinName, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(buffer, &pinArg->pin1Info, sizeof(pinArg->pin1Info));
	buffer += sizeof(pinArg->pin1Info);
	memcpy(buffer, &pinArg->pin1, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(buffer, &pinArg->pin2Info, sizeof(pinArg->pin2Info));
	buffer += sizeof(pinArg->pin2Info);
	memcpy(buffer, &pinArg->pin2, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(buffer, &pinArg->returnValue, sizeof(pinArg->returnValue));

	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::readAskPinsArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgAskPINsArguments *pinArg = (DlgAskPINsArguments *)(arg);
	int len = sizeof(pinArg->operation) + sizeof(pinArg->usage) + 50 * sizeof(wchar_t) + sizeof(pinArg->pin1Info) +
			  (PIN_MAX_LENGTH + 1) * sizeof(wchar_t) +  sizeof(pinArg->pin2Info) +
			  (PIN_MAX_LENGTH + 1) * sizeof(wchar_t) + sizeof(pinArg->returnValue);
	buffer = (char *)malloc(len);
	read(fd, buffer, len);
	initBuffer = buffer;

	memcpy(&pinArg->operation, buffer, sizeof(pinArg->operation));
	buffer += sizeof(pinArg->operation);
	memcpy(&pinArg->usage, buffer, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(&pinArg->pinName, buffer, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(&pinArg->pin1Info, buffer, sizeof(pinArg->pin1Info));
	buffer += sizeof(pinArg->pin1Info);
	memcpy(&pinArg->pin1, buffer, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(&pinArg->pin2Info, buffer, sizeof(pinArg->pin2Info));
	buffer += sizeof(pinArg->pin2Info);
	memcpy(&pinArg->pin2, buffer, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);
	memcpy(&pinArg->returnValue, buffer, sizeof(pinArg->returnValue));

	free(initBuffer);
}

void eIDMW::readBadPinArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgBadPinArguments *pinArg = (DlgBadPinArguments *)(arg);
	int len = sizeof(pinArg->usage) + 50 * sizeof(wchar_t) + sizeof(pinArg->ulRemainingTries) + sizeof(pinArg->returnValue);

	buffer = (char *)malloc(len);
	read(fd, buffer, len);
	initBuffer = buffer;

	memcpy(&pinArg->usage, buffer, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(&pinArg->pinName, buffer, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(&pinArg->ulRemainingTries, buffer, sizeof(pinArg->ulRemainingTries));
	buffer += sizeof(pinArg->ulRemainingTries);
	memcpy(&pinArg->returnValue, buffer, sizeof(pinArg->returnValue));

	free(initBuffer);
}
void eIDMW::writeBadPinArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgBadPinArguments *pinArg = (DlgBadPinArguments *)(arg);
	int len = sizeof(pinArg->usage) + 50 * sizeof(wchar_t) + sizeof(pinArg->ulRemainingTries) + sizeof(pinArg->returnValue);

	buffer = (char *)malloc(len);
	initBuffer = buffer;

	memcpy(buffer, &pinArg->usage, sizeof(pinArg->usage));
	buffer += sizeof(pinArg->usage);
	memcpy(buffer, &pinArg->pinName, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(buffer, &pinArg->ulRemainingTries, sizeof(pinArg->ulRemainingTries));
	buffer += sizeof(pinArg->ulRemainingTries);
	memcpy(buffer, &pinArg->returnValue, sizeof(pinArg->returnValue));

	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::writeDisplayPinpadInfoArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgDisplayPinpadInfoArguments *pinPadInfo = (DlgDisplayPinpadInfoArguments *)(arg);
	int len = sizeof(pinPadInfo->operation) + 100 * sizeof(wchar_t) + sizeof(pinPadInfo->usage)
			  + 50 * sizeof(wchar_t) + 200 * sizeof(wchar_t) + sizeof(pinPadInfo->infoCollectorIndex) +
			  sizeof(pinPadInfo->tRunningProcess) + sizeof(pinPadInfo->returnValue);
	buffer = (char *)malloc(len);
	initBuffer = buffer;
	memcpy(buffer, &pinPadInfo->operation, sizeof(pinPadInfo->operation));
	buffer += sizeof(pinPadInfo->operation);
	memcpy(buffer, &pinPadInfo->reader, 100 * sizeof(wchar_t));
	buffer += 100 * sizeof(wchar_t);
	memcpy(buffer, &pinPadInfo->usage, sizeof(pinPadInfo->usage));
	buffer += sizeof(pinPadInfo->usage);
	memcpy(buffer, &pinPadInfo->pinName, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(buffer, &pinPadInfo->message, 200 * sizeof(wchar_t));
	buffer += 200 * sizeof(wchar_t);
	memcpy(buffer, &pinPadInfo->infoCollectorIndex, sizeof(pinPadInfo->infoCollectorIndex));
	buffer += sizeof(pinPadInfo->infoCollectorIndex);
	memcpy(buffer, &pinPadInfo->tRunningProcess, sizeof(pinPadInfo->tRunningProcess));
	buffer += sizeof(pinPadInfo->tRunningProcess);
	memcpy(buffer, &pinPadInfo->returnValue, sizeof(pinPadInfo->returnValue));

	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::readDisplayPinpadInfoArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgDisplayPinpadInfoArguments *pinPadInfo = (DlgDisplayPinpadInfoArguments *)(arg);
	int len = sizeof(pinPadInfo->operation) + 100 * sizeof(wchar_t) + sizeof(pinPadInfo->usage)
			  + 50 * sizeof(wchar_t) + 200 * sizeof(wchar_t) + sizeof(pinPadInfo->infoCollectorIndex) +
			  sizeof(pinPadInfo->tRunningProcess) + sizeof(pinPadInfo->returnValue);
	buffer = (char *)malloc(len);
	initBuffer = buffer;
	int sizeRead = read(fd, buffer, len);
	if(sizeRead == -1) {
		MWLOG(LEV_ERROR, MOD_DLG, L"eIDMW:: Error when waiting to read");
		free(buffer);
		return;
	}
	else if(sizeRead == 1) {
		MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW:: Close read");
		pinPadInfo->returnValue = DLG_OK;
		free(buffer);
		return;
	}

	memcpy(&pinPadInfo->operation, buffer, sizeof(pinPadInfo->operation));
	buffer += sizeof(pinPadInfo->operation);
	memcpy(&pinPadInfo->reader, buffer, 100 * sizeof(wchar_t));
	buffer += 100 * sizeof(wchar_t);
	memcpy(&pinPadInfo->usage, buffer, sizeof(pinPadInfo->usage));
	buffer += sizeof(pinPadInfo->usage);
	memcpy(&pinPadInfo->pinName, buffer, 50 * sizeof(wchar_t));
	buffer += 50 * sizeof(wchar_t);
	memcpy(&pinPadInfo->message, buffer, 200 * sizeof(wchar_t));
	buffer += 200 * sizeof(wchar_t);
	memcpy(&pinPadInfo->infoCollectorIndex, buffer, sizeof(pinPadInfo->infoCollectorIndex));
	buffer += sizeof(pinPadInfo->infoCollectorIndex);
	memcpy(&pinPadInfo->tRunningProcess, buffer, sizeof(pinPadInfo->tRunningProcess));
	buffer += sizeof(pinPadInfo->tRunningProcess);
	memcpy(&pinPadInfo->returnValue, buffer, sizeof(pinPadInfo->returnValue));

	free(initBuffer);
}

void eIDMW::writeAskInputCMDArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgAskInputCMDArguments *cmdArg = (DlgAskInputCMDArguments *)(arg);

	int len = (sizeof(wchar_t) * 50) + sizeof(cmdArg->operation) + sizeof(cmdArg->isValidateOtp) +
			  sizeof(cmdArg->callbackWasCalled) + ((PIN_MAX_LENGTH + 1) * sizeof(wchar_t)) + sizeof(cmdArg->askForId) +
			  sizeof(cmdArg->returnValue);
	buffer = (char *)malloc(len);
	initBuffer = buffer;

	memcpy(buffer, &cmdArg->inOutId, sizeof(wchar_t) * 50);
	buffer += sizeof(wchar_t) * 50;
	memcpy(buffer, &cmdArg->operation, sizeof(cmdArg->operation));
	buffer += sizeof(cmdArg->operation);

	memcpy(buffer, &cmdArg->isValidateOtp, sizeof(cmdArg->isValidateOtp));
	buffer += sizeof(cmdArg->isValidateOtp);

	memcpy(buffer, &cmdArg->callbackWasCalled, sizeof(cmdArg->callbackWasCalled));
	buffer += sizeof(cmdArg->callbackWasCalled);

	memcpy(buffer, &cmdArg->Code, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);

	memcpy(buffer, &cmdArg->askForId, sizeof(cmdArg->askForId));
	buffer += sizeof(cmdArg->askForId);

	memcpy(buffer, &cmdArg->returnValue, sizeof(cmdArg->returnValue));

	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::readAskInputCMDArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgAskInputCMDArguments *cmdArg = (DlgAskInputCMDArguments *)(arg);

	int len = (sizeof(wchar_t) * 50) + sizeof(cmdArg->operation) + sizeof(cmdArg->isValidateOtp) +
			  sizeof(cmdArg->callbackWasCalled) + ((PIN_MAX_LENGTH + 1) * sizeof(wchar_t)) + sizeof(cmdArg->askForId) +
			  sizeof(cmdArg->returnValue);
	buffer = (char *)malloc(len);
	read(fd, buffer, len);
	initBuffer = buffer;

	memcpy(&cmdArg->inOutId, buffer, sizeof(wchar_t) * 50);
	buffer += sizeof(wchar_t) * 50;
	memcpy(&cmdArg->operation, buffer, sizeof(cmdArg->operation));
	buffer += sizeof(cmdArg->operation);

	memcpy(&cmdArg->isValidateOtp, buffer, sizeof(cmdArg->isValidateOtp));
	buffer += sizeof(cmdArg->isValidateOtp);

	memcpy(&cmdArg->callbackWasCalled, buffer, sizeof(cmdArg->callbackWasCalled));
	buffer += sizeof(cmdArg->callbackWasCalled);

	memcpy(&cmdArg->Code, buffer, (PIN_MAX_LENGTH + 1) * sizeof(wchar_t));
	buffer += (PIN_MAX_LENGTH + 1) * sizeof(wchar_t);

	memcpy(&cmdArg->askForId, buffer, sizeof(cmdArg->askForId));
	buffer += sizeof(cmdArg->askForId);

	memcpy(&cmdArg->returnValue, buffer, sizeof(cmdArg->returnValue));

	free(initBuffer);
}

void eIDMW::writeCMDMessageArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgCMDMessageArguments *cmdArg = (DlgCMDMessageArguments *)(arg);

	int len = (sizeof(wchar_t) * 50) + sizeof(cmdArg->operation) + sizeof(cmdArg->type) + sizeof(cmdArg->returnValue) +
			  sizeof(cmdArg->cmdMsgCollectorIndex) + sizeof(cmdArg->tRunningProcess);
	buffer = (char *)malloc(len);
	initBuffer = buffer;
	memcpy(buffer, &cmdArg->message, sizeof(wchar_t) * 50);
	buffer += sizeof(wchar_t) * 50;
	memcpy(buffer, &cmdArg->operation, sizeof(cmdArg->operation));
	buffer += sizeof(cmdArg->operation);
	memcpy(buffer, &cmdArg->type, sizeof(cmdArg->type));
	buffer += sizeof(cmdArg->type);
	memcpy(buffer, &cmdArg->returnValue, sizeof(cmdArg->returnValue));
	buffer += sizeof(cmdArg->returnValue);
	memcpy(buffer, &cmdArg->cmdMsgCollectorIndex, sizeof(cmdArg->cmdMsgCollectorIndex));
	buffer += sizeof(cmdArg->cmdMsgCollectorIndex);
	memcpy(buffer, &cmdArg->tRunningProcess, sizeof(cmdArg->tRunningProcess));

	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::writeDlgPickDeviceArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgPickDeviceArguments *dlgArg = (DlgPickDeviceArguments *)(arg);
	int len = sizeof(dlgArg->outDevice) + sizeof(dlgArg->returnValue);
	buffer = (char *)malloc(len);
	initBuffer = buffer;
	memcpy(buffer, &dlgArg->outDevice, sizeof(dlgArg->outDevice));
	buffer += sizeof(dlgArg->outDevice);
	memcpy(buffer, &dlgArg->returnValue, sizeof(dlgArg->returnValue));
	write(fd, initBuffer, len);
	free(initBuffer);
}

void eIDMW::readDlgPickDeviceArguments(int fd, void *arg)
{
	char *buffer;
	char *initBuffer;
	DlgPickDeviceArguments *dlgArg = (DlgPickDeviceArguments *)(arg);
	int len = sizeof(dlgArg->outDevice) + sizeof(dlgArg->returnValue);
	buffer = (char *)malloc(len);
	read(fd, buffer, len);
	initBuffer = buffer;
	memcpy(&dlgArg->outDevice, buffer, sizeof(dlgArg->outDevice));
	buffer += sizeof(dlgArg->outDevice);
	memcpy(&dlgArg->returnValue, buffer, sizeof(dlgArg->returnValue));
	free(initBuffer);
}

void eIDMW::readCMDMessageArguments(int fd, void *arg) {
	char *buffer;
	char *initBuffer;
	DlgCMDMessageArguments *cmdArg = (DlgCMDMessageArguments *)(arg);
	int len = (sizeof(wchar_t) * 50) + sizeof(cmdArg->operation) + sizeof(cmdArg->type) + sizeof(cmdArg->returnValue) +
			  sizeof(cmdArg->cmdMsgCollectorIndex) + sizeof(cmdArg->tRunningProcess);
	buffer = (char *)malloc(len);
	int sizeRead = read(fd, buffer, len);
	if(sizeRead == -1) {
		MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW:: Error when waiting to read");
		free(buffer);
		return;
	}
	else if(sizeRead == 1) {
		MWLOG(LEV_DEBUG, MOD_DLG, L"eIDMW:: Close read");
		cmdArg->returnValue = DLG_OK;
		free(buffer);
		return;
	}
	initBuffer = buffer;

	memcpy(&cmdArg->message, buffer, sizeof(wchar_t) * 50);
	buffer += sizeof(wchar_t) * 50;
	memcpy(&cmdArg->operation, buffer, sizeof(cmdArg->operation));
	buffer += sizeof(cmdArg->operation);
	memcpy(&cmdArg->type, buffer, sizeof(cmdArg->type));
	buffer += sizeof(cmdArg->type);
	memcpy(&cmdArg->returnValue, buffer, sizeof(cmdArg->returnValue));
	buffer += sizeof(cmdArg->returnValue);
	memcpy(&cmdArg->cmdMsgCollectorIndex, buffer, sizeof(cmdArg->cmdMsgCollectorIndex));
	buffer += sizeof(cmdArg->cmdMsgCollectorIndex);
	memcpy(&cmdArg->tRunningProcess, buffer, sizeof(cmdArg->tRunningProcess));
	free(initBuffer);
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPin(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName,
									DlgPinInfo pinInfo, wchar_t *wsPin, unsigned long ulPinBufferLen,
									void *wndGeometry) {
	DlgRet lRet = DLG_CANCEL;

	DlgAskPINArguments oData;

	try {
		oData.operation = operation;
		oData.usage = usage;
		wcscpy_s(oData.pinName, sizeof(oData.pinName) / sizeof(wchar_t), wsPinName);
		oData.pinInfo = pinInfo;
		wcscpy_s(oData.pin, sizeof(oData.pin) / sizeof(wchar_t), wsPin);

		MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgAskPin print debug before call qt");
		CallQTServerPipe(DLG_ASK_PIN, readAskPinArguments, writeAskPinArguments, (void *)&oData, wndGeometry);
		MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgAskPin print debug after call qt");
		lRet = oData.returnValue;

		if (lRet == DLG_OK) {
			wcscpy_s(wsPin, ulPinBufferLen, oData.pin);
		}

		// detach from the segment

		// delete the random file
	} catch (...) {

		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT DlgRet eIDMW::DlgAskPins(DlgPinOperation operation, DlgPinUsage usage, const wchar_t *wsPinName,
									 DlgPinInfo pin1Info, wchar_t *wsPin1, unsigned long ulPin1BufferLen,
									 DlgPinInfo pin2Info, wchar_t *wsPin2, unsigned long ulPin2BufferLen,
									 void *wndGeometry) {

	DlgRet lRet = DLG_CANCEL;

	DlgAskPINsArguments oData;

	try {
		oData.operation = operation;
		oData.usage = usage;
		wcscpy_s(oData.pinName, sizeof(oData.pinName) / sizeof(wchar_t), wsPinName);
		oData.pin1Info = pin1Info;
		oData.pin2Info = pin2Info;
		wcscpy_s(oData.pin1, sizeof(oData.pin1) / sizeof(wchar_t), wsPin1);
		wcscpy_s(oData.pin2, sizeof(oData.pin2) / sizeof(wchar_t), wsPin2);
		CallQTServerPipe(DLG_ASK_PINS, readAskPinsArguments, writeAskPinsArguments, (void *)&oData, wndGeometry);
		lRet = oData.returnValue;

		if (lRet == DLG_OK) {
			wcscpy_s(wsPin1, ulPin1BufferLen, oData.pin1);
			wcscpy_s(wsPin2, ulPin2BufferLen, oData.pin2);
		}

	} catch (...) {
		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT DlgRet eIDMW::DlgBadPin(DlgPinUsage usage, const wchar_t *wsPinName, unsigned long ulRemainingTries,
									void *wndGeometry) {
	DlgRet lRet = DLG_CANCEL;

	DlgBadPinArguments oData;

	try {
		oData.usage = usage;
		wcscpy_s(oData.pinName, sizeof(oData.pinName) / sizeof(wchar_t), wsPinName);
		oData.ulRemainingTries = ulRemainingTries;
		CallQTServerPipe(DLG_BAD_PIN, readBadPinArguments, writeBadPinArguments, (void *)&oData, wndGeometry);
		lRet = oData.returnValue;

	} catch (...) {

		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation, const wchar_t *wsReader, DlgPinUsage usage,
											   const wchar_t *wsPinName, const wchar_t *wsMessage,
											   unsigned long *pulHandle, void *wndGeometry) {
	DlgRet lRet = DLG_CANCEL;

	DlgDisplayPinpadInfoArguments oData;
	int pipe1[2]; // parent -> child
	int pipe2[2]; // child -> parent

	try {
		MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo called");


		oData.operation = operation;
		wcscpy_s(oData.reader, sizeof(oData.reader) / sizeof(wchar_t), wsReader);
		oData.usage = usage;
		wcscpy_s(oData.pinName, sizeof(oData.pinName) / sizeof(wchar_t), wsPinName);
		wcscpy_s(oData.message, sizeof(oData.message) / sizeof(wchar_t), wsMessage);
		oData.infoCollectorIndex = ++dlgPinPadInfoCollectorIndex;

		int firstPipeResult = pipe(pipe1);
		if (firstPipeResult == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo Failed to launch the first pipe error: %s",
				  strerror(errno));
			return DLG_ERR;
		}
		int secondPipeResult = pipe(pipe2);
		if (secondPipeResult == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo Failed to launch the second pipe error: %s",
				  strerror(errno));
			return DLG_ERR;
		}
		pid_t pid = fork();
		if (pid == 0) {
			char indexBuff[2];
			char pipe1Buff[10];
			char pipe2Buff[10];
			std::string csServerPath = STRINGIFY(EIDMW_PREFIX) "/bin/";
#ifdef __APPLE__
			csServerPath += "pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv";
#else
			csServerPath += csServerName;
#endif
			snprintf(indexBuff, sizeof(indexBuff), "%d", DLG_DISPLAY_PINPAD_INFO);

			snprintf(pipe1Buff, sizeof(pipe1Buff), "%d", pipe1[0]);
			snprintf(pipe2Buff, sizeof(pipe2Buff), "%d", pipe2[1]);

			int resultExec = execl(csServerPath.c_str(), csServerPath.c_str(), indexBuff, pipe1Buff, pipe2Buff, NULL);
			if(resultExec == -1) {
				MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo Failed to execute dialog executable error: %s",
					  strerror(errno));
				exit(-1);
			}
			exit(0);

		}
		else if(pid == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo Failed to create a new process error: %s",
				  strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}
		else {
			oData.tRunningProcess = pid;
			writeDisplayPinpadInfoArguments(pipe1[1], (void*)&oData);
			DlgRunningProc *ptRunningProc = new DlgRunningProc();

			ptRunningProc->tRunningProcess = oData.tRunningProcess;
			ptRunningProc->pipe2 = pipe2;

			dlgPinPadInfoCollector[dlgPinPadInfoCollectorIndex] = ptRunningProc;

			if (pulHandle)
				*pulHandle = dlgPinPadInfoCollectorIndex;

			readDisplayPinpadInfoArguments(pipe2[0], (void*)&oData);

			delete ptRunningProc;
			dlgPinPadInfoCollector[dlgPinPadInfoCollectorIndex] = NULL;
			dlgPinPadInfoCollector.erase(dlgPinPadInfoCollectorIndex);

			lRet = oData.returnValue;
			close(pipe1[0]);
			close(pipe1[1]);
			close(pipe2[0]);
			close(pipe2[1]);

			if (lRet != DLG_OK) {
				throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
			}
		}


		// for the killing need to store:
		// - the shared memory area to be released (unique with the filename?)
		// - the child process ID
		// - the handle (because the user will use it)

	} catch (...) {
		MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgDisplayPinpadInfo failed");
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);

		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT void eIDMW::DlgClosePinpadInfo(unsigned long ulHandle) {
	// check if we have this handle
	std::map<unsigned long, DlgRunningProc *>::iterator pIt = dlgPinPadInfoCollector.find(ulHandle);

	if (pIt != dlgPinPadInfoCollector.end()) {

		// check if the process is still running
		// and send SIGTERM if so
		if (!kill(pIt->second->tRunningProcess, 0)) {

			MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgClosePinpadInfo :  sending kill signal to process %d",
				  pIt->second->tRunningProcess);

			if (kill(pIt->second->tRunningProcess, SIGINT)) {

				MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgClosePinpadInfo sent signal SIGINT to proc %d Error: %s ",
					  pIt->second->tRunningProcess, strerror(errno));

				throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
			}
			else {
				write(pIt->second->pipe2[1], "1", 1);
			}

		} else {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgClosePinpadInfo sent signal 0 to proc %d : Error %s ",
				  pIt->second->tRunningProcess, strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}
	}
}

DLGS_EXPORT DlgRet eIDMW::DlgAskInputCMD(DlgCmdOperation operation, bool isValidateOtp, wchar_t *csOutCode,
										 unsigned long ulOutCodeBufferLen, wchar_t *csInOutId, unsigned long ulOutIdLen,
										 const wchar_t *csUserName, unsigned long ulUserNameBufferLen,
										 std::function<void(void)> *fSendSmsCallback) {
	MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgAskInputCMD called");
	DlgRet lRet = DLG_CANCEL;

	DlgAskInputCMDArguments oData;

	try {
		oData.isValidateOtp = isValidateOtp;
		oData.operation = operation;
		oData.askForId = ulOutIdLen != 0;

		if (isValidateOtp) {
			wcsncpy(oData.inOutId, csInOutId, sizeof(oData.inOutId) / sizeof(wchar_t));
		} else {
			// Cached mobile number for CMD PIN dialog
			wcsncpy(oData.inOutId, csInOutId, ulOutIdLen);
		}

		CallQTServerPipe(DLG_ASK_CMD_INPUT, readAskInputCMDArguments, writeAskInputCMDArguments, (void *)&oData);
		lRet = oData.returnValue;

		/* If the callback button to send the sms was pressed, call callback and reopen the dialog.
		  callbackWasCalled in oData is set to true to disable the button. */
		if (oData.returnValue == DLG_CALLBACK) {
			(*fSendSmsCallback)();
			oData.callbackWasCalled = true;
			oData.returnValue = DLG_CANCEL;
			CallQTServerPipe(DLG_ASK_CMD_INPUT, readAskInputCMDArguments, writeAskInputCMDArguments, (void *)&oData);
			lRet = oData.returnValue;
		}

		if (lRet == DLG_OK) {
			if (!isValidateOtp) {
				wcscpy_s(csInOutId, ulOutIdLen, oData.inOutId);
			}

			wcscpy_s(csOutCode, ulOutCodeBufferLen, oData.Code);
		}
	} catch (...) {
		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT DlgRet eIDMW::DlgPickDevice(DlgDevice *outDevice) {
	MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgPickDevice called");
	DlgRet lRet = DLG_CANCEL;

	DlgPickDeviceArguments oData;

	try {
		CallQTServerPipe(DLG_PICK_DEVICE, readDlgPickDeviceArguments, writeDlgPickDeviceArguments, (void *)&oData);
		lRet = oData.returnValue;
		if (lRet == DLG_OK) {
			*outDevice = oData.outDevice;
		}

	} catch (...) {

		return DLG_ERR;
	}
	return lRet;
}

DLGS_EXPORT DlgRet eIDMW::DlgCMDMessage(DlgCmdOperation operation, DlgCmdMsgType type, bool isOtp,
										unsigned long *pulHandle) {
	const wchar_t *message = NULL;
	if (isOtp) {
		message = GETSTRING_DLG(SendingOtp);
	} else {
		message = GETSTRING_DLG(ConnectingWithServer);
	}
	return DlgCMDMessage(operation, type, message, pulHandle);
}

DLGS_EXPORT DlgRet eIDMW::DlgCMDMessage(DlgCmdOperation operation, DlgCmdMsgType type, const wchar_t *message,
										unsigned long *pulHandle) {
	MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgCMDMessage called");
	DlgRet lRet = DLG_CANCEL;

	DlgCMDMessageArguments oCmdMessageData;

	int pipe1[2]; // parent -> child
	int pipe2[2]; // child -> parent
	try {
		oCmdMessageData.type = type;
		oCmdMessageData.operation = operation;
		wcscpy_s(oCmdMessageData.message, sizeof(oCmdMessageData.message) / sizeof(wchar_t), message);
		oCmdMessageData.cmdMsgCollectorIndex = ++dlgCMDMsgCollectorIndex;

		std::string csServerPath = STRINGIFY(EIDMW_PREFIX) "/bin/";
#ifdef __APPLE__
		csServerPath += "pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv";
#else
		csServerPath += csServerName;
#endif

		int firstPipeResult = pipe(pipe1);
		if (firstPipeResult == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCMDMessage Failed to launch the first pipe error: %s",
				  strerror(errno));
			return DLG_ERR;
		}
		int secondPipeResult = pipe(pipe2);
		if (secondPipeResult == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCMDMessage Failed to launch the second pipe error: %s",
				  strerror(errno));
			return DLG_ERR;
		}
		pid_t pid = fork();
		if (pid == 0) {
			char indexBuff[2];
			char pipe1Buff[10];
			char pipe2Buff[10];
			snprintf(indexBuff, sizeof(indexBuff), "%d", DLG_CMD_MSG);

			snprintf(pipe1Buff, sizeof(pipe1Buff), "%d", pipe1[0]);
			snprintf(pipe2Buff, sizeof(pipe2Buff), "%d", pipe2[1]);

			int resultExec = execl(csServerPath.c_str(), csServerPath.c_str(), indexBuff, pipe1Buff, pipe2Buff, NULL);
			if(resultExec == -1) {
				MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCMDMessage Failed to execute dialog executable error: %s",
					  strerror(errno));
				exit(-1);
			}
			exit(0);
		}
		else if(pid == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCMDMessage Failed to create a new process error: %s",
				  strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}
		else {
			oCmdMessageData.tRunningProcess = pid;
			writeCMDMessageArguments(pipe1[1], (void*)&oCmdMessageData);


			DlgRunningProc *ptRunningProc = new DlgRunningProc();

			ptRunningProc->tRunningProcess = oCmdMessageData.tRunningProcess;
			ptRunningProc->pipe2 = pipe2;

			dlgCMDMsgCollector[dlgCMDMsgCollectorIndex] = ptRunningProc;

			if (pulHandle)
				*pulHandle = dlgCMDMsgCollectorIndex;

			readCMDMessageArguments(pipe2[0], (void*)&oCmdMessageData);

			delete ptRunningProc;
			dlgCMDMsgCollector[dlgCMDMsgCollectorIndex] = NULL;
			dlgCMDMsgCollector.erase(dlgCMDMsgCollectorIndex);

			lRet = oCmdMessageData.returnValue;
			close(pipe1[0]);
			close(pipe1[1]);
			close(pipe2[0]);
			close(pipe2[1]);
		}
	} catch (...) {

		MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCMDMessage failed");
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		return DLG_ERR;
	}

	return lRet;
}

DLGS_EXPORT void eIDMW::DlgCloseCMDMessage(unsigned long ulHandle) {
	MWLOG(LEV_DEBUG, MOD_DLG, L"DlgCloseCMDMessage() called: handle=%lu", ulHandle);
	// check if we have this handle
	std::map<unsigned long, DlgRunningProc *>::iterator pIt = dlgCMDMsgCollector.find(ulHandle);

	if (pIt != dlgCMDMsgCollector.end()) {

		// check if the process is still running
		// and send SIGTERM if so
		if (!kill(pIt->second->tRunningProcess, 0)) {

			MWLOG(LEV_DEBUG, MOD_DLG, L"  eIDMW::DlgCloseCMDMessage :  sending kill signal to process %d pipe2 : %d",
				  pIt->second->tRunningProcess, pIt->second->pipe2[1]);

			if (kill(pIt->second->tRunningProcess, SIGINT)) {

				MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCloseCMDMessage sent signal SIGINT to proc %d Error: %s ",
					  pIt->second->tRunningProcess, strerror(errno));

				throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
			}
			else {
				write(pIt->second->pipe2[1], "1", 1);
			}

		} else {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::DlgCloseCMDMessage sent signal 0 to proc %d : Error %s ",
				  pIt->second->tRunningProcess, strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
		}

		// memory is cleaned up in the child process
	}
}

DLGS_EXPORT void eIDMW::DlgCloseAskInputCMD() {
	// If the current_dlg_pid is different than 0
	if (current_dlg_pid != 0) {
		// If the process is running
		if (!kill(current_dlg_pid, 0)) {
			// Kills the process
			kill(current_dlg_pid, SIGINT);
		}
	}
}

/***************************
 *       Helper Functions
 ***************************/

void eIDMW::CallQTServerPipe(const DlgFunctionIndex index, readArgument readFunc, writeArgument writeFunc, void *args,
							 void *wndGeometry) {
	Type_WndGeometry *pWndGeometry = (Type_WndGeometry *)wndGeometry;

	std::string csServerPath = STRINGIFY(EIDMW_PREFIX) "/bin/";
#ifdef __APPLE__
	csServerPath += "pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv";
#else
	csServerPath += csServerName;
#endif

	int pipe1[2]; // parent -> child
	int pipe2[2]; // child -> parent

	int firstPipeResult = pipe(pipe1);
	if (firstPipeResult == -1) {
		MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServerPipe Failed to launch the first pipe error: %s",
			  strerror(errno));
		return;
	}
	int secondPipeResult = pipe(pipe2);
	if (secondPipeResult == -1) {
		MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServerPipe Failed to launch the second pipe error: %s",
			  strerror(errno));
		return;
	}
	pid_t pid = fork();
	if (pid == 0) {
		char indexBuff[2];
		char pipe1Buff[10];
		char pipe2Buff[10];
		snprintf(indexBuff, sizeof(indexBuff), "%d", index);

		snprintf(pipe1Buff, sizeof(pipe1Buff), "%d", pipe1[0]);
		snprintf(pipe2Buff, sizeof(pipe2Buff), "%d", pipe2[1]);

		int resultExec = execl(csServerPath.c_str(), csServerPath.c_str(), indexBuff, pipe1Buff, pipe2Buff, NULL);
		if(resultExec == -1) {
			MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServerPipe Failed to execute dialog executable error: %s",
				  strerror(errno));
			exit(-1);
		}

		exit(0);
	} else if (pid == -1) {
		MWLOG(LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServerPipe Failed to create a new process error: %s",
			  strerror(errno));
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
	} else {
		current_dlg_pid = pid;
		writeFunc(pipe1[1], args);
		readFunc(pipe2[0], args);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		current_dlg_pid = 0;
	}
}

void eIDMW::CallQTServer(const DlgFunctionIndex index, const char *csFilename, void *wndGeometry) {
	char csCommand[150];
	Type_WndGeometry *pWndGeometry = (Type_WndGeometry *)wndGeometry;

	std::string csServerPath = STRINGIFY(EIDMW_PREFIX) "/bin/";
#ifdef __APPLE__
	csServerPath += "pteiddialogsQTsrv.app/Contents/MacOS/";
#endif

	sprintf(csCommand, "%s/%s %i %s", csServerPath.c_str(), csServerName.c_str(), index, csFilename);

	if ((pWndGeometry != NULL) && (pWndGeometry->x >= 0) && (pWndGeometry->y >= 0) && (pWndGeometry->width >= 0) &&
		(pWndGeometry->height >= 0)) {
		int len = strlen(csCommand);
		sprintf(&csCommand[len], " %i %i %i %i", pWndGeometry->x, pWndGeometry->y, pWndGeometry->width,
				pWndGeometry->height);
	}

	int code = system(csCommand);
	if (code != 0) {
		MWLOG(g_bSystemCallsFail ? LEV_WARN : LEV_ERROR, MOD_DLG, L"  eIDMW::CallQTServer %i %s : %s ", index,
			  csFilename, strerror(errno));
		if (!g_bSystemCallsFail)
			throw CMWEXCEPTION(EIDMW_ERR_UNKNOWN);
	}
	return;
}
bool eIDMW::getWndCenterPos(Type_WndGeometry *pWndGeometry, int desktop_width, int desktop_height, int wnd_width,
							int wnd_height, Type_WndGeometry *outWndGeometry) {

	if (outWndGeometry == NULL || pWndGeometry == NULL)
		return false;
	if ((desktop_width < 0) || (desktop_height < 0) || (wnd_width < 0) || (wnd_height < 0))
		return false;

	memset(outWndGeometry, -1, sizeof(Type_WndGeometry));

	outWndGeometry->x = pWndGeometry->x + ((pWndGeometry->width - wnd_width) / 2);
	outWndGeometry->y = pWndGeometry->y + ((pWndGeometry->height - wnd_height) / 2);

	if (outWndGeometry->x < 0 || outWndGeometry->x > desktop_width)
		return false;
	if (outWndGeometry->y < 0 || outWndGeometry->y > desktop_height)
		return false;

	return true;
}
