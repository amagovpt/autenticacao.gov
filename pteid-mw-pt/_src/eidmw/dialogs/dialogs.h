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
/**
 * eID middleware Dialogs.
 */
#ifdef WIN32
#pragma once
#endif

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#include <string>

#ifdef WIN32
	#ifdef DLGSWIN32_EXPORTS
	#define DLGS_EXPORT __declspec(dllexport)
	#else
	#define DLGS_EXPORT __declspec(dllimport)
	#endif
#else
#define DLGS_EXPORT
#endif

namespace eIDMW
{

#define PIN_MAX_LENGTH     16

typedef enum {
	DLG_OK,        // OK button clicked
	DLG_CANCEL,    // Cancel button clicked
	DLG_RETRY,     // Retry button clicked
	DLG_YES,       // Yes button clicked
	DLG_NO,        // No button clicked
	DLG_ALWAYS,    // Always button clicked
	DLG_NEVER,		// Never button clicked
	DLG_BAD_PARAM, // Bad values for a parameter
	DLG_ERR,       // Something else went wrong
} DlgRet;

typedef enum {
	DLG_PIN_OP_VERIFY,
	DLG_PIN_OP_CHANGE,
	DLG_PIN_OP_UNBLOCK_NO_CHANGE,
	DLG_PIN_OP_UNBLOCK_CHANGE,
	DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK,
} DlgPinOperation;

typedef enum {
	DLG_PF_OP_READ_ID		= 0x00000001,		// Read the ID file
	DLG_PF_OP_READ_PHOTO	= 0x00000002,		// Read the photo file
	DLG_PF_OP_READ_ADDRESS	= 0x00000004,		// Read the address file
	DLG_PF_OP_READ_CERTS	= 0x00000008		// Read (one) of the user certificate(s)
} DlgPFOperation;

typedef enum {
	DLG_PIN_UNKNOWN,
	DLG_PIN_AUTH,
	DLG_PIN_SIGN,
	DLG_PIN_ACTIVATE,
	DLG_PIN_ADDRESS,
} DlgPinUsage;

const unsigned char PIN_FLAG_DIGITS = 1;
	
#ifdef WIN32
typedef struct {
	unsigned long ulMinLen;
	unsigned long ulMaxLen;    // should be 16 at most
	unsigned long ulFlags;     // PIN_FLAG_DIGITS, ...
} DlgPinInfo;
#else
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	
typedef struct {
	unsigned long long ulMinLen;
	unsigned long long ulMaxLen;    // should be 16 at most
	unsigned long long ulFlags;     // PIN_FLAG_DIGITS, ...
} DlgPinInfo;	
#endif
	
typedef enum {
	DLG_ICON_NONE,
	DLG_ICON_INFO,
	DLG_ICON_WARN,
	DLG_ICON_ERROR,
	DLG_ICON_QUESTION,
} DlgIcon;

 typedef enum {
   DLG_ASK_PIN,
   DLG_ASK_PINS,
   DLG_BAD_PIN, 
   DLG_DISPLAY_PINPAD_INFO,
   DLG_DISPLAY_MODAL,
   DLG_ASK_ACCESS, 
 } DlgFunctionIndex;

 typedef enum {
	DLG_MESSAGE_NONE,
	DLG_MESSAGE_TESTCARD,
	DLG_MESSAGE_ENTER_CORRECT_CARD,
	DLG_MESSAGE_USER_WARNING,
} DlgMessageID;

const unsigned char DLG_BUTTON_OK = 1;
const unsigned char DLG_BUTTON_CANCEL = 2;
const unsigned char DLG_BUTTON_RETRY = 4;
const unsigned char DLG_BUTTON_YES = 8;
const unsigned char DLG_BUTTON_NO = 16;

const unsigned long DLG_LANG_DEFAULT = 0; // this is no USB LANGID code
const unsigned long DLG_LANG_EN = 0x0409;
const unsigned long DLG_LANG_NL = 0x0813;
const unsigned long DLG_LANG_FR = 0x080c;
const unsigned long DLG_LANG_DE = 0x0407;
const unsigned long DLG_LANG_PT = 0x0816;

/*************************************************************************************
 * Pin dialogs
 *************************************************************************************/

/**
 * Ask for 1 PIN (e.g. to verify or unblock without changing the old PIN)
 * This dialog is modal (returns after the dlg is closed)
 * - operation: DLG_PIN_OP_VERIFY or DLG_PIN_OP_UNBLOCK_NO_CHANGE
 * - pinName: should only be used if 'usage' == DLG_PIN_UNKNOWN
 * - pinInfo: should be used to verify the format of the PIN
 *     (valid length, valid characters, ...); as long as this
 *     isn't the case, the OK button should be disabled.
 * Returns: DLG_OK if the OK button was pressed,
 *          DLG_CANCEL if the Cancel button was pressed,
 *          DLG_BAD_PARAM or DLG_ERR otherwise
 */
DLGS_EXPORT DlgRet DlgAskPin(DlgPinOperation operation,
	DlgPinUsage usage, const wchar_t *csPinName,
	DlgPinInfo pinInfo, wchar_t *csPin, unsigned long ulPinBufferLen, void *wndGeometry = 0 );

/**
 * Ask for 2 PINs, the 2nd PIN should be asked twice for confirmation
 * This dialog is modal (returns after the dlg is closed)
 * (e.g. to change a PIN, or to unblock + change a PIN)
 * - operation: DLG_PIN_OP_CHANGE or DLG_PIN_OP_UNBLOCK_CHANGE
 * - pinName: should only be used if 'usage' == DLG_PIN_UNKNOWN
 * - pinInfo: should be used to verify the format of the PINs
 *     (valid length, valid characters, ...); as long as this
 *     isn't the case, the OK button should be disabled.
 * Returns: DLG_OK if the OK button was pressed,
 *          DLG_CANCEL if the Cancel button was pressed,
 *          DLG_BAD_PARAM or DLG_ERR otherwise
 */
DLGS_EXPORT DlgRet DlgAskPins(DlgPinOperation operation,
	DlgPinUsage usage, const wchar_t *csPinName,
	DlgPinInfo pin1Info, wchar_t *csPin1, unsigned long ulPin1BufferLen,
	DlgPinInfo pin2Info, wchar_t *csPin2, unsigned long ulPin2BufferLen, void *wndGeometry = 0 );

/**
 * Display a message, e.g. "Bad PIN, x remaining attempts" or "PIN blocked".
 * - ulRemainingTries: the remaining PIN tries: if
 * Returns: DLG_OK if the OK button was pressed,
 *          DLG_CANCEL if the Cancel button was pressed,
 *          DLG_RETRY if the Retry button was pressed
 *          DLG_BAD_PARAM or DLG_ERR otherwise
 */
DLGS_EXPORT DlgRet DlgBadPin(DlgPinUsage usage, const wchar_t *csPinName,
	unsigned long ulRemainingTries, void *wndGeometry = 0 );

/************************************************************************************
 * Pin pad dialogs
 ************************************************************************************/

/**
 * Show a message explaining how to use the pinpad reader.
 * This dialog is modeless (returns after showing the dialog.
 * The dialog is closed by a call to DlgCloseMsg(), but an 'X'
 * should be present to manually close it if DlgCloseMsg()
 * hasn't been called.
 * - operation: DLG_PIN_OP_VERIFY, DLG_PIN_OP_CHANGE,
 *     DLG_PIN_OP_UNBLOCK_NO_CHANGE or DLG_PIN_OP_UNBLOCK_CHANGE
 * - pinName: should only be used if 'usage' == DLG_PIN_UNKNOWN
 * - *pulHandle: [OUT]: handle used in DlgCloseMsg(), only
 *     needed if multiple pinpad dialogs can be called
 * Returns: DLG_OK if all went fine,
 *          DLG_BAD_PARAM or DLG_ERR otherwise
 */
DLGS_EXPORT DlgRet DlgDisplayPinpadInfo(DlgPinOperation operation,
	const wchar_t *csReader, DlgPinUsage usage, const wchar_t *csPinName,
	const wchar_t *csMessage,
	unsigned long *pulHandle, void *wndGeometry = 0 );

/**
* Close the pinpad info dialog 
*/
DLGS_EXPORT void DlgClosePinpadInfo(unsigned long ulHandle);

#ifndef WIN32
/**
* Close the all the open pinpad info dialogs 
*/
 DLGS_EXPORT void DlgCloseAllPinpadInfo();
#endif


#ifndef WIN32
/************************************************************************************
 * Helper structs and functions
 ************************************************************************************/

/**
   Helper structs for the implementation of the communication
   between the dialogsQTsrv and the dialogsQT library */

struct DlgAskPINArguments {
  DlgPinOperation operation;
  DlgPinUsage usage; 
  wchar_t pinName[50];
  DlgPinInfo pinInfo; 
  wchar_t pin[PIN_MAX_LENGTH+1]; 
  DlgRet returnValue;
} ;


 struct DlgAskPINsArguments {
   DlgPinOperation operation;
   DlgPinUsage usage;
   wchar_t pinName[50];
   DlgPinInfo pin1Info;
   wchar_t pin1[PIN_MAX_LENGTH+1];
   DlgPinInfo pin2Info;
   wchar_t pin2[PIN_MAX_LENGTH+1];
   DlgRet returnValue;
 } ;

 struct DlgBadPinArguments {
   DlgPinUsage usage;
   wchar_t pinName[50];
   unsigned long long ulRemainingTries;
   DlgRet returnValue;
 } ;

 struct DlgDisplayPinpadInfoArguments {
   DlgPinOperation operation;
   wchar_t reader[100];
   DlgPinUsage usage;
   wchar_t pinName[50];
   wchar_t message[200];
   unsigned long long infoCollectorIndex;
   pid_t tRunningProcess;
   DlgRet returnValue;
 } ;

 struct DlgRunningProc{
   int iSharedMemSegmentID;
   std::string csRandomFilename;
   pid_t tRunningProcess;
 };

 struct WndGeometry{
    int x;
    int y;
    int width;
    int height;
};
typedef struct WndGeometry Type_WndGeometry;

 void InitializeRand();
 std::string RandomFileName();
 std::string CreateRandomFile();
 void DeleteFile(const char *csFilename);
 void CallQTServer(    const DlgFunctionIndex index
                    ,  const char *csFilename
                    , void *wndGeometry = 0 );

DLGS_EXPORT bool getWndCenterPos( Type_WndGeometry *pWndGeometry
                                , int desktop_width, int desktop_height
                                , int wnd_width, int wnd_height
                                , Type_WndGeometry *outWndGeometry );

#pragma pack(pop)   /* restore original alignment from stack */
#endif


}

#endif
