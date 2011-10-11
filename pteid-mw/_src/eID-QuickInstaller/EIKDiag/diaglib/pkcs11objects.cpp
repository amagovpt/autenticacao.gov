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

#include "pkcs11objects.h"
#include "errorfmt.h"

/*-----------------------------------------------------------------------------
  Methode:      Create
  Datum:        2005-10-25
  Argumenten:   const LibraryName: string
  Resultaat:    None
  Omschrijving: Load and initialize the PKCS11 library
                Get the slotlist
                Create a session for each slot with a token
                Start the slotevent listener thread (TokenListener)
-----------------------------------------------------------------------------*/
CPKCS11::CPKCS11(std::string const& LibraryName) : FWindowHandle(0) {

    typedef CK_RV(*FCGetFunctionList)(CK_FUNCTION_LIST_PTR *);

    FCGetFunctionList CGetFunctionList = NULL;

    CK_RV rv;
    CK_INFO info;
    unsigned long ulCount;
    CK_SLOT_INFO SlotInfo;

    char str[128];


    FLibHandle = LoadLibrary(LibraryName.c_str());
    if (FLibHandle == 0) {
        string msg = "Can\'t load PKCS11 DLL " + LibraryName;
        throw (CErrorFmt::FormatError(GetLastError(), msg.c_str()));
    }
    try {
        CGetFunctionList = (FCGetFunctionList) GetProcAddress(FLibHandle, "C_GetFunctionList");
        if ((CGetFunctionList == NULL)) {
            throw ( string("Can\'t find PKCS11 entrypoint C_GetFunctionList()"));
        }
        rv = CGetFunctionList(&FP11);
        Check_RV(rv, "C_GetFunctionList");
        /* Initialize PKCS11 library */
        FInitArgs.CreateMutex = 00;
        FInitArgs.DestroyMutex = 00;
        FInitArgs.LockMutex = 00;
        FInitArgs.UnlockMutex = 00;
        FInitArgs.flags = CKF_OS_LOCKING_OK;
        FInitArgs.pReserved = 00;
        rv = FP11->C_Initialize(&FInitArgs);
        Check_RV(rv, "C_Initialize");
        try {
            /* get pkcs11 library info */
            rv = FP11->C_GetInfo(&info);
            Check_RV(rv, "C_GetInfo");
            sprintf_s(str, sizeof (str), "PKCS11V%d.%d", info.cryptokiVersion.major, info.cryptokiVersion.minor);
            FCryptOkiVersion = str;
            sprintf_s(str, sizeof (str), "V%d.%d", info.libraryVersion.major, info.libraryVersion.minor);
            FLibraryVersion = str;
            FDescription = Utf8ArrayToString(info.libraryDescription);
            FManufacturerId = Utf8ArrayToString(info.manufacturerID);

            /* get slotlist, first get number of slots present */
            ulCount = 0;
            rv = FP11->C_GetSlotList(0, 00, &ulCount);
            Check_RV(rv, "C_GetSlotList");
            /* and now get the slotlist */
            const auto_vec<CK_SLOT_ID> lSlotList(new CK_SLOT_ID[ulCount]);
            rv = FP11->C_GetSlotList(0, lSlotList.get(), &ulCount);
            Check_RV(rv, "C_GetSlotList");

            /* get slotinfo for each slot */
            for (UINT slotNo = 0; slotNo < ulCount; ++slotNo) {
                /* get slotinfo */
                CK_SLOT_ID slid = lSlotList.get()[slotNo];
                rv = FP11->C_GetSlotInfo(slid, &SlotInfo);
                Check_RV(rv, "C_GetSlotInfo");
                FSlots.push_back(new CP11SlotInfo(FP11, slid, SlotInfo));
            }

        } catch (string exc) {
            FP11->C_Finalize(00);
            FP11 = 00;
            throw exc;
        }

    } catch (string exc) {
        if (FLibHandle != 0) {
            FreeLibrary(FLibHandle);
            FLibHandle = 0;
        }
        FP11 = 00;
        throw exc;
    }

    /* start the slot event listener thread */
    CP11ThreadParams* params = new CP11ThreadParams;
    params->FP11 = this->FP11;
    params->P11 = this;
    FSlotMonitor.CreateThread((void *) params);

}

/*-----------------------------------------------------------------------------
  Methode:      Destroy
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    None
  Omschrijving: Cleanup
-----------------------------------------------------------------------------*/
CPKCS11::~CPKCS11() {

    /* make sure the slot listener is termintated before destroying the slotlist */
    //if(  FMonitorThread != NULL )
    //{
    //  FMonitorThread.Terminate;
    //  Windows.Sleep( 300 );
    //  FMonitorThread.Free;
    //}
    //if(  FWindowhANDLE != 0 )
    //  DeallocateHWnd( FWindowhANDLE );

    FSlotMonitor.Terminate();
    Sleep(501);

    /* get rid of the slotinfo */
    SlotListIterator slit;
    for (slit = FSlots.begin(); slit != FSlots.end(); slit++) {
        delete *slit;
    }

    /* clean up the PKCS11 library */
    if (FP11 != NULL)
        FP11->C_Finalize(00);
    Sleep(50);
    FreeLibrary(FLibHandle);
}

/*-----------------------------------------------------------------------------
  Methode:      Utf8ArrayToString
  Auteur:       DH
  Datum:        2005-10-26
  Argumenten:   const data: array of CK_UTF8CHAR
  Resultaat:    string
  Omschrijving: Convert array of UTF8 characters to AnsiString *STATIC *
-----------------------------------------------------------------------------*/
std::string CPKCS11::Utf8ArrayToString(CK_UTF8CHAR *data) {
    std::string Result = (char *) data;
    return Result;
}

/*-----------------------------------------------------------------------------
  Methode:      GetSlotCount
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    Cardinal
  Omschrijving: Return the number of slots
-----------------------------------------------------------------------------*/
unsigned int CPKCS11::GetSlotCount() {
    return (unsigned int) FSlots.size();
}

/*-----------------------------------------------------------------------------
  Methode:      GetSlot
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Index: integer
  Resultaat:    TP11SlotInfo
  Omschrijving: Return the current P11Slotinfo for a particular slot
-----------------------------------------------------------------------------*/
CP11SlotInfo& CPKCS11::GetSlot(UINT Index) {
    if ((Index >= 0) && (Index < FSlots.size())) {
        return *FSlots.at(Index);
    } else {
        char str[128];
        sprintf_s(str, sizeof (str), "TPKCS11.GetSlot: index out of range (%d)", Index);
        throw string(str);
    }

}

/*-----------------------------------------------------------------------------
  Methode:      AddTokenListener
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   OnSlotChange: TSlotChangeEvent
  Resultaat:    None
  Omschrijving: Register a new slot listener
                Notify it immediately of the current situation
-----------------------------------------------------------------------------*/
//void TPKCS11::AddTokenListener(TSlotChangeEvent OnSlotChange)
//{
//  //int i;
//  //TP11SlotInfo Slot;
//
//  ///* register the slot listener */
//  //i = Succ( Length( FStateChangeListeners ) );
//  //SetLength( FStateChangeListeners , i );
//  //FStateChangeListeners[ Pred( i ) ] = OnSlotChange;
//  ///* inform the new listener of any slots with a token,
//  //  use the windows message method just like the thread would have done */
//  //{ long i_end = Pred( FSlots.Count )+1 ; for( i = 0 ; i < i_end ; ++i )
//  //{
//  //  Slot = TP11SlotInfo( FSlots.Items[ i ] );
//  //  if(  ( Slot.Flags && Pkcs11T.CKF_TOKEN_PRESENT ) != 0 )
//  //  {
//  //    while(  !PostMessage( FWindowhANDLE , CM_P11SLOTEVENT , 0 , Slot.SlotId ) )
//  //      Sleep( 1 );
//  //  }
//  //}}
//}

/*-----------------------------------------------------------------------------
  Methode:      Check_RV
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   rv: CK_RV; const FunctionName: string
  Resultaat:    None
  Omschrijving: Check the PKCS11 CK_RV return code
                raise an exception with the string error code if not CKR_OK  * STATIC *
-----------------------------------------------------------------------------*/
void CPKCS11::Check_RV(CK_RV rv, std::string const& FunctionName) {

    std::string rv_string;

    if (rv != CKR_OK) {
        rv_string = "";
        switch (rv) {
            case CKR_OK: rv_string = "CKR_OK";
                break;
            case CKR_CANCEL: rv_string = "CKR_CANCEL";
                break;
            case CKR_HOST_MEMORY: rv_string = "CKR_HOST_MEMORY";
                break;
            case CKR_SLOT_ID_INVALID: rv_string = "CKR_SLOT_ID_INVALID";
                break;
                // CKR_FLAGS_INVALID was removed for v2.0
                // CKR_GENERAL_ERROR and CKR_FUNCTION_FAILED are new for v2.0
            case CKR_GENERAL_ERROR: rv_string = "CKR_GENERAL_ERROR";
                break;
            case CKR_FUNCTION_FAILED: rv_string = "CKR_FUNCTION_FAILED";
                break;
                // CKR_ARGUMENTS_BAD, CKR_NO_EVENT, CKR_NEED_TO_CREATE_THREADS
                // and CKR_CANT_LOCK are new for v2.01 
            case CKR_ARGUMENTS_BAD: rv_string = "CKR_ARGUMENTS_BAD";
                break;
            case CKR_NO_EVENT: rv_string = "CKR_NO_EVENT";
                break;
            case CKR_NEED_TO_CREATE_THREADS: rv_string = "CKR_NEED_TO_CREATE_THREADS";
                break;
            case CKR_CANT_LOCK: rv_string = "CKR_CANT_LOCK";
                break;
            case CKR_ATTRIBUTE_READ_ONLY: rv_string = "CKR_ATTRIBUTE_READ_ONLY";
                break;
            case CKR_ATTRIBUTE_SENSITIVE: rv_string = "CKR_ATTRIBUTE_SENSITIVE";
                break;
            case CKR_ATTRIBUTE_TYPE_INVALID: rv_string = "CKR_ATTRIBUTE_TYPE_INVALID";
                break;
            case CKR_ATTRIBUTE_VALUE_INVALID: rv_string = "CKR_ATTRIBUTE_VALUE_INVALID";
                break;
            case CKR_DATA_INVALID: rv_string = "CKR_DATA_INVALID";
                break;
            case CKR_DATA_LEN_RANGE: rv_string = "CKR_DATA_LEN_RANGE";
                break;
            case CKR_DEVICE_ERROR: rv_string = "CKR_DEVICE_ERROR";
                break;
            case CKR_DEVICE_MEMORY: rv_string = "CKR_DEVICE_MEMORY";
                break;
            case CKR_DEVICE_REMOVED: rv_string = "CKR_DEVICE_REMOVED";
                break;
            case CKR_ENCRYPTED_DATA_INVALID: rv_string = "CKR_ENCRYPTED_DATA_INVALID";
                break;
            case CKR_ENCRYPTED_DATA_LEN_RANGE: rv_string = "CKR_ENCRYPTED_DATA_LEN_RANGE";
                break;
            case CKR_FUNCTION_CANCELED: rv_string = "CKR_FUNCTION_CANCELED";
                break;
            case CKR_FUNCTION_NOT_PARALLEL: rv_string = "CKR_FUNCTION_NOT_PARALLEL";
                break;
                // CKR_FUNCTION_NOT_SUPPORTED is new for v2.0
            case CKR_FUNCTION_NOT_SUPPORTED: rv_string = "CKR_FUNCTION_NOT_SUPPORTED";
                break;
            case CKR_KEY_HANDLE_INVALID: rv_string = "CKR_KEY_HANDLE_INVALID";
                break;
                // CKR_KEY_SENSITIVE was removed for v2.0
            case CKR_KEY_SIZE_RANGE: rv_string = "CKR_KEY_SIZE_RANGE";
                break;
            case CKR_KEY_TYPE_INCONSISTENT: rv_string = "CKR_KEY_TYPE_INCONSISTENT";
                break;
                // CKR_KEY_NOT_NEEDED, CKR_KEY_CHANGED, CKR_KEY_NEEDED,
                // CKR_KEY_INDIGESTIBLE, CKR_KEY_FUNCTION_NOT_PERMITTED, 
                // CKR_KEY_NOT_WRAPPABLE, and CKR_KEY_UNEXTRACTABLE are new for 
                // v2.0 
            case CKR_KEY_NOT_NEEDED: rv_string = "CKR_KEY_NOT_NEEDED";
                break;
            case CKR_KEY_CHANGED: rv_string = "CKR_KEY_CHANGED";
                break;
            case CKR_KEY_NEEDED: rv_string = "CKR_KEY_NEEDED";
                break;
            case CKR_KEY_INDIGESTIBLE: rv_string = "CKR_KEY_INDIGESTIBLE";
                break;
            case CKR_KEY_FUNCTION_NOT_PERMITTED: rv_string = "CKR_KEY_FUNCTION_NOT_PERMITTED";
                break;
            case CKR_KEY_NOT_WRAPPABLE: rv_string = "CKR_KEY_NOT_WRAPPABLE";
                break;
            case CKR_KEY_UNEXTRACTABLE: rv_string = "CKR_KEY_UNEXTRACTABLE";
                break;
            case CKR_MECHANISM_INVALID: rv_string = "CKR_MECHANISM_INVALID";
                break;
            case CKR_MECHANISM_PARAM_INVALID: rv_string = "CKR_MECHANISM_PARAM_INVALID";
                break;
                // CKR_OBJECT_CLASS_INCONSISTENT and CKR_OBJECT_CLASS_INVALID
                // were removed for v2.0 
            case CKR_OBJECT_HANDLE_INVALID: rv_string = "CKR_OBJECT_HANDLE_INVALID";
                break;
            case CKR_OPERATION_ACTIVE: rv_string = "CKR_OPERATION_ACTIVE";
                break;
            case CKR_OPERATION_NOT_INITIALIZED: rv_string = "CKR_OPERATION_NOT_INITIALIZED";
                break;
            case CKR_PIN_INCORRECT: rv_string = "CKR_PIN_INCORRECT";
                break;
            case CKR_PIN_INVALID: rv_string = "CKR_PIN_INVALID";
                break;
            case CKR_PIN_LEN_RANGE: rv_string = "CKR_PIN_LEN_RANGE";
                break;
                // CKR_PIN_EXPIRED and CKR_PIN_LOCKED are new for v2.0
            case CKR_PIN_EXPIRED: rv_string = "CKR_PIN_EXPIRED";
                break;
            case CKR_PIN_LOCKED: rv_string = "CKR_PIN_LOCKED";
                break;
            case CKR_SESSION_CLOSED: rv_string = "CKR_SESSION_CLOSED";
                break;
            case CKR_SESSION_COUNT: rv_string = "CKR_SESSION_COUNT";
                break;
            case CKR_SESSION_HANDLE_INVALID: rv_string = "CKR_SESSION_HANDLE_INVALID";
                break;
            case CKR_SESSION_PARALLEL_NOT_SUPPORTED: rv_string = "CKR_SESSION_PARALLEL_NOT_SUPPORTED";
                break;
            case CKR_SESSION_READ_ONLY: rv_string = "CKR_SESSION_READ_ONLY";
                break;
            case CKR_SESSION_EXISTS: rv_string = "CKR_SESSION_EXISTS";
                break;
                // CKR_SESSION_READ_ONLY_EXISTS and 
                // CKR_SESSION_READ_WRITE_SO_EXISTS are new for v2.0 
            case CKR_SESSION_READ_ONLY_EXISTS: rv_string = "CKR_SESSION_READ_ONLY_EXISTS";
                break;
            case CKR_SESSION_READ_WRITE_SO_EXISTS: rv_string = "CKR_SESSION_READ_WRITE_SO_EXISTS";
                break;
            case CKR_SIGNATURE_INVALID: rv_string = "CKR_SIGNATURE_INVALID";
                break;
            case CKR_SIGNATURE_LEN_RANGE: rv_string = "CKR_SIGNATURE_LEN_RANGE";
                break;
            case CKR_TEMPLATE_INCOMPLETE: rv_string = "CKR_TEMPLATE_INCOMPLETE";
                break;
            case CKR_TEMPLATE_INCONSISTENT: rv_string = "CKR_TEMPLATE_INCONSISTENT";
                break;
            case CKR_TOKEN_NOT_PRESENT: rv_string = "CKR_TOKEN_NOT_PRESENT";
                break;
            case CKR_TOKEN_NOT_RECOGNIZED: rv_string = "CKR_TOKEN_NOT_RECOGNIZED";
                break;
            case CKR_TOKEN_WRITE_PROTECTED: rv_string = "CKR_TOKEN_WRITE_PROTECTED";
                break;
            case CKR_UNWRAPPING_KEY_HANDLE_INVALID: rv_string = "CKR_UNWRAPPING_KEY_HANDLE_INVALID";
                break;
            case CKR_UNWRAPPING_KEY_SIZE_RANGE: rv_string = "CKR_UNWRAPPING_KEY_SIZE_RANGE";
                break;
            case CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT: rv_string = "CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT";
                break;
            case CKR_USER_ALREADY_LOGGED_IN: rv_string = "CKR_USER_ALREADY_LOGGED_IN";
                break;
            case CKR_USER_NOT_LOGGED_IN: rv_string = "CKR_USER_NOT_LOGGED_IN";
                break;
            case CKR_USER_PIN_NOT_INITIALIZED: rv_string = "CKR_USER_PIN_NOT_INITIALIZED";
                break;
            case CKR_USER_TYPE_INVALID: rv_string = "CKR_USER_TYPE_INVALID";
                break;
                // CKR_USER_ANOTHER_ALREADY_LOGGED_IN and CKR_USER_TOO_MANY_TYPES 
                // are new to v2.01 
            case CKR_USER_ANOTHER_ALREADY_LOGGED_IN: rv_string = "CKR_USER_ANOTHER_ALREADY_LOGGED_IN";
                break;
            case CKR_USER_TOO_MANY_TYPES: rv_string = "CKR_USER_TOO_MANY_TYPES";
                break;
            case CKR_WRAPPED_KEY_INVALID: rv_string = "CKR_WRAPPED_KEY_INVALID";
                break;
            case CKR_WRAPPED_KEY_LEN_RANGE: rv_string = "CKR_WRAPPED_KEY_LEN_RANGE";
                break;
            case CKR_WRAPPING_KEY_HANDLE_INVALID: rv_string = "CKR_WRAPPING_KEY_HANDLE_INVALID";
                break;
            case CKR_WRAPPING_KEY_SIZE_RANGE: rv_string = "CKR_WRAPPING_KEY_SIZE_RANGE";
                break;
            case CKR_WRAPPING_KEY_TYPE_INCONSISTENT: rv_string = "CKR_WRAPPING_KEY_TYPE_INCONSISTENT";
                break;
            case CKR_RANDOM_SEED_NOT_SUPPORTED: rv_string = "CKR_RANDOM_SEED_NOT_SUPPORTED";
                break;
                // These are new to v2.0
            case CKR_RANDOM_NO_RNG: rv_string = "CKR_RANDOM_NO_RNG";
                break;
                // These are new to v2.11
            case CKR_DOMAIN_PARAMS_INVALID: rv_string = "CKR_DOMAIN_PARAMS_INVALID";
                break;
                // These are new to v2.0
            case CKR_BUFFER_TOO_SMALL: rv_string = "CKR_BUFFER_TOO_SMALL";
                break;
            case CKR_SAVED_STATE_INVALID: rv_string = "CKR_SAVED_STATE_INVALID";
                break;
            case CKR_INFORMATION_SENSITIVE: rv_string = "CKR_INFORMATION_SENSITIVE";
                break;
            case CKR_STATE_UNSAVEABLE: rv_string = "CKR_STATE_UNSAVEABLE";
                break;
                // These are new to v2.01
            case CKR_CRYPTOKI_NOT_INITIALIZED: rv_string = "CKR_CRYPTOKI_NOT_INITIALIZED";
                break;
            case CKR_CRYPTOKI_ALREADY_INITIALIZED: rv_string = "CKR_CRYPTOKI_ALREADY_INITIALIZED";
                break;
            case CKR_MUTEX_BAD: rv_string = "CKR_MUTEX_BAD";
                break;
            case CKR_MUTEX_NOT_LOCKED: rv_string = "CKR_MUTEX_NOT_LOCKED";
                break;
            case CKR_FUNCTION_REJECTED: rv_string = "CKR_FUNCTION_REJECTED";
                break;
            case CKR_VENDOR_DEFINED: rv_string = "CKR_VENDOR_DEFINED";
                break;
            default:
                rv_string = "CKR_UNKNOWN_ERROR_CODE";
                break;
        }
        char str[256];
        sprintf_s(str, sizeof (str), "%s : error code = 0x%x (%s)", FunctionName.c_str(), rv, rv_string.c_str());
        throw string(str);
    }
}


/*-----------------------------------------------------------------------------
  Methode:      RemoveTokenListener
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   OnSlotChange: TSlotChangeEvent
  Resultaat:    None
  Omschrijving: Remove a particular tokenlistener callback from the list
-----------------------------------------------------------------------------*/
//void TPKCS11::RemoveTokenListener(TSlotChangeEvent OnSlotChange) {
//  //int i;
//  //int j;
//
//  //if(  FStateChangeListeners == 00 )
//  //  return;
//  //{ long i_end = High( FStateChangeListeners )+1 ; for( i = Low( FStateChangeListeners ) ; i < i_end ; ++i )
//  //{
//  //  if(  &OnSlotChange == &FStateChangeListeners[ i ] )
//  //    &FStateChangeListeners[ i ] = 00;
//  //  { long j_end = Pred( High( FStateChangeListeners ) )+1 ; for( j = i ; j < j_end ; ++j )
//  //  {
//  //    FStateChangeListeners[ j ] = FStateChangeListeners[ Succ( j ) ];
//  //  }}
//  //  SetLength( FStateChangeListeners , Pred( Length( FStateChangeListeners ) ) );
//  //  return;
//  //}}
//  //throw Exception.Create( "TPKCS11.RemoveSlotListener: attempt to remove an unregsitered listener!" );
//}

/*-----------------------------------------------------------------------------
  Methode:      Wndproc
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   var Message: TMessage
  Resultaat:    None
  Omschrijving: The slotevent monitor thread has sent a message with a slot event
                Process the slot event
                Update the Slots array with the new SlotInfo
                Alert the registered TokenListeners
-----------------------------------------------------------------------------*/
void CPKCS11::SlotEventWndproc(MSG message) {
    //CK_SLOT_ID Slot;
    //TSlotChangeEvent OnSlotChange;
    //int i;
    //CK_RV rv;
    //CK_SLOT_INFO SlotInfo;
    //TP11SlotInfo NewSlotInfo;
    //TSlotState NewState;
    //int j;

    ///*?*//* WITH  Message */
    //{
    //  if(  Msg == CM_P11SLOTEVENT )
    //  {
    //    try {
    //      Slot = CK_SLOT_ID( lParam );
    //      { long i_end = Pred( FSlots.Count )+1 ; for( i = 0 ; i < i_end ; ++i )
    //      {
    //        if(  TP11SlotInfo( FSlots.Items[ i ] ).SlotId == Slot )
    //        {
    //          /* get new slotinfo */
    //          rv = FP11->C_GetSlotInfo( Slot , &SlotInfo );
    //          Check_RV( rv , "C_GetSlotInfo" );
    //          /* create a TP11SlotInfo object (without token info yet) */
    //          NewSlotInfo = TP11SlotInfo.Create( FP11 , Slot , SlotInfo );
    //          /* store new slotinfo, old one is automatically freed by TObjectList */
    //          FSlots.Items[ i ] = NewSlotInfo;
    //          if(  ( NewSlotInfo.Flags && Pkcs11T.CKF_TOKEN_PRESENT ) != 0 )
    //            NewState = PKCS11Objects.ssTokenPresent;else
    //            NewState = PKCS11Objects.ssTokenAbsent;
    //          /* execute any registered callback procedures */
    //          { long j_end = High( FStateChangeListeners )+1 ; for( j = Low( FStateChangeListeners ) ; j < j_end ; ++j )
    //          {
    //            OnSlotChange = FStateChangeListeners[ j ];
    //            OnSlotChange( NewState , NewSlotInfo );
    //          }}
    //          return;
    //        }
    //      }}
    //    
    //      } catch(...) {;//Application.HandleException(Self);
    //    }
    //  }
    //  else
    //  {
    //    result = DefWindowProc( FWindowhANDLE , Msg , wParam , lParam );
    //  }
    //}
}

std::string CPKCS11::P11StringToString(CK_UTF8CHAR_PTR src, size_t size) {

    auto_vec<char> str(new char[size + 1]);
    int j = 0;
    for (int i = (int) size - 1; i > 0; --i) {
        if (src[i] != ' ') {
            j = i;
            break;
        }
    }
    char *p = str.get();
    for (int i = 0; i <= j; ++i) {
        *p++ = src[i];
    }
    *p = 0;
    string result = str.get();
    return result;

}

void CPKCS11::SlotChanged(CK_SLOT_ID Slot) {

    CK_SLOT_INFO SlotInfo;
    CK_RV rv;


    for (SlotListIterator slit = this->FSlots.begin(); slit != this->FSlots.end(); ++slit) {

        CP11SlotInfo& p11slot = *(*slit);

        if (p11slot.get_SlotId() == Slot) {
            rv = FP11->C_GetSlotInfo(Slot, &SlotInfo);
            Check_RV(rv, "C_GetSlotInfo");
            p11slot.FFlags = SlotInfo.flags;
            if ((SlotInfo.flags && CKF_TOKEN_PRESENT) != 0) {
                p11slot._tokenInserted = true;
                p11slot._tokenRemoved = false;
            } else {
                p11slot._tokenInserted = false;
                p11slot._tokenRemoved = true;
            }
        }
    }

    return;

}

/* TP11Thread */

unsigned long CP11Thread::Execute(void *parameters) {

    CP11ThreadParams* par = (CP11ThreadParams *) parameters;
    this->FP11 = par->FP11;
    this->P11 = par->P11;
    delete par;

    UINT rv;
    CK_SLOT_ID Slot;

    while (!this->_terminated) {
        /* wait for a slot event, non-blocking */
        Sleep(250);
        if (P11->get_SlotCount() == 0)
            continue;
        rv = FP11->C_WaitForSlotEvent(CKF_DONT_BLOCK, &Slot, 00);
        /* shutting down ? */
        if (rv == CKR_CRYPTOKI_NOT_INITIALIZED)
            return -1;
        /* nothing happened */
        if (rv == CKR_NO_EVENT)
            continue;
        /* something happened, pass the slot to the PKCS11 instance on the main thread  */
        if (rv == CKR_OK) {
            /* inform main thread */
            P11->SlotChanged(Slot);
        }
    }
    return 0;
}

/* TP11SlotInfo */

/*-----------------------------------------------------------------------------
  Methode:      Create
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   P11FunctionList: CK_FUNCTION_LIST_PTR; SlotId: CK_SLOT_ID; SlotInfo: CK_SLOT_INFO
  Resultaat:    None
  Omschrijving: Create a new SlotInfo object
-----------------------------------------------------------------------------*/
CP11SlotInfo::CP11SlotInfo(CK_FUNCTION_LIST_PTR P11FunctionList, CK_SLOT_ID SlotId, CK_SLOT_INFO SlotInfo)
: FCurrentTokenInfo(NULL), FP11(P11FunctionList), FSlotId(SlotId), _tokenRemoved(false), _tokenInserted(false) {

    FManufacturerId = CPKCS11::P11StringToString(&SlotInfo.manufacturerID[0], sizeof (SlotInfo.manufacturerID));
    FSlotDescription = CPKCS11::P11StringToString(&SlotInfo.slotDescription[0], sizeof (SlotInfo.slotDescription));
    const auto_vec<char> str(new char[32]);
    sprintf_s(str.get(), 32, "%d.%d", SlotInfo.firmwareVersion.major, SlotInfo.firmwareVersion.minor);
    FFirmwareVersion = str.get();
    sprintf_s(str.get(), 32, "%d.%d", SlotInfo.hardwareVersion.major, SlotInfo.hardwareVersion.minor);
    FHardwareVersion = str.get();
    FFlags = SlotInfo.flags;
    _tokenInserted = false;
    _tokenRemoved = false;
}

/*-----------------------------------------------------------------------------
  Methode:      Destroy
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    None
  Omschrijving: Clean up
-----------------------------------------------------------------------------*/
CP11SlotInfo::~CP11SlotInfo() {
    if (NULL != FCurrentTokenInfo)
        delete FCurrentTokenInfo;
}

/*-----------------------------------------------------------------------------
  Methode:      GetHaveToken
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    boolean
  Omschrijving: Is there currently a token in this slot ?
-----------------------------------------------------------------------------*/
bool CP11SlotInfo::GetHaveToken() {
    return ( FFlags & CKF_TOKEN_PRESENT) != 0;
}

/*-----------------------------------------------------------------------------
  Methode:      GetTokenInfo
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    TP11TokenInfo
  Omschrijving: return the TokenInfo object for this slot
 ----------------------------------------------------------------------------- */
CP11TokenInfo& CP11SlotInfo::GetTokenInfo() {

    if ((FFlags & CKF_TOKEN_PRESENT) == 0) {
        throw string("TP11Token.GetTokenInfo: no token present.");
    }
    return *FCurrentTokenInfo;

}

/*-----------------------------------------------------------------------------
  Methode:      UpdateTokenInfo
  Auteur:       DH
  Datum:        2008-07-16
  Argumenten:   None
  Resultaat:    
  Omschrijving: update the TokenInfo object for this slot
 ----------------------------------------------------------------------------- */
void CP11SlotInfo::UpdateTokenInfo() {
    return UpdateTokenInfo("");
}

/*-----------------------------------------------------------------------------
  Methode:      UpdateTokenInfo
  Auteur:       DH
  Datum:        2008-07-16
  Argumenten:   None
  Resultaat:    
  Omschrijving: update the TokenInfo object for this slot, login
 ----------------------------------------------------------------------------- */

/*TODO: login eruit */
void CP11SlotInfo::UpdateTokenInfo(const std::string& PinCode) {

    CK_TOKEN_INFO ti;
    CK_RV rv;

    this->_tokenInserted = false;
    this->_tokenRemoved = false;

    if ((FFlags & CKF_TOKEN_PRESENT) == 0) {
        delete FCurrentTokenInfo;
        FCurrentTokenInfo = NULL;
        return;
    }

    rv = FP11->C_GetTokenInfo(FSlotId, &ti);
    CPKCS11::Check_RV(rv, "C_GetTokenInfo");
    /* lazy instantiation, create only when needed */
    if (NULL == FCurrentTokenInfo)
        FCurrentTokenInfo = new CP11TokenInfo(this, FP11, ti, PinCode);
    else {
        delete FCurrentTokenInfo;
        FCurrentTokenInfo = new CP11TokenInfo(this, FP11, ti, PinCode);
    }
}

/* TP11Token */

/*-----------------------------------------------------------------------------
  Methode:      Create
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Slot: TP11SlotInfo; P11FunctionList: CK_FUNCTION_LIST_PTR; const TokenInfo: CK_TOKEN_INFO
  Resultaat:    None
  Omschrijving: Create a new TokenInfo object
                Open a session with the token and enumerate all mechanisms
 ----------------------------------------------------------------------------- */

/*TODO: session manueel laten maken: GetSession(PinCode), FreeSession */
CP11TokenInfo::CP11TokenInfo(CP11SlotInfo *Slot, CK_FUNCTION_LIST_PTR P11FunctionList, CK_TOKEN_INFO& TokenInfo, std::string const& PinCode)
: FP11(P11FunctionList), FSlot(Slot), FSessionInfo(NULL) {

    unsigned long ulCount;
    CK_RV rv;
    CK_MECHANISM_INFO MechInfo;

    const auto_vec<char> str(new char[32]);
    /* store general token info */
    sprintf_s(str.get(), 32, "%d.%d", TokenInfo.firmwareVersion.major, TokenInfo.firmwareVersion.minor);
    FFirmwareVersion = str.get();
    sprintf_s(str.get(), 32, "%d.%d", TokenInfo.hardwareVersion.major, TokenInfo.hardwareVersion.major);
    FHardwareVersion = str.get();
    FFlags = TokenInfo.flags;
    FTokenLabel = CPKCS11::P11StringToString(TokenInfo.label, sizeof (TokenInfo.label));
    FTokenModel = CPKCS11::P11StringToString(TokenInfo.model, sizeof (TokenInfo.model));
    FTokenSerial = CPKCS11::P11StringToString(TokenInfo.serialNumber, sizeof (TokenInfo.serialNumber));
    FFreePrivateMemory = TokenInfo.ulFreePrivateMemory;
    FFreePublicMemory = TokenInfo.ulFreePublicMemory;
    FTotalPrivateMemory = TokenInfo.ulTotalPrivateMemory;
    FTotalPublicMemory = TokenInfo.ulTotalPublicMemory;
    FMaxSessionCount = TokenInfo.ulMaxSessionCount;
    FMaxRwSessionCount = TokenInfo.ulMaxRwSessionCount;
    FSessionCount = TokenInfo.ulSessionCount;
    FRwSessionCount = TokenInfo.ulRwSessionCount;

    FUTCTime = CPKCS11::Utf8ArrayToString(TokenInfo.utcTime);

    /* Store all avaliable mechanisms on the token */

    /* First get the number of mechanisms found   */
    ulCount = 0;
    rv = FP11->C_GetMechanismList(FSlot->get_SlotId(), 00, &ulCount);
    CPKCS11::Check_RV(rv, "C_GetMechanismList");
    /* and then get the mechanism list */
    auto_vec<CK_MECHANISM_TYPE> MechList(new CK_MECHANISM_TYPE[ ulCount ]);
    rv = FP11->C_GetMechanismList(FSlot->get_SlotId(), &MechList.get()[ 0 ], &ulCount);
    CPKCS11::Check_RV(rv, "C_GetMechanismList");
    /* FMechanisms will contain the TP11Mechanism objects */
    /* iterate over all mechanisms and build the list of TP11Mechanism objects */
    for (UINT mech = 0; mech < ulCount; ++mech) {
        rv = FP11->C_GetMechanismInfo(FSlot->get_SlotId(), MechList.get()[ mech ], &MechInfo);
        CPKCS11::Check_RV(rv, "C_GetMechanismInfo");
        FMechanisms.push_back(new CP11Mechanism(MechList[ mech ], MechInfo));
    }

    /* Create a session with the token */
    FSessionInfo = new CP11SessionInfo(*this, FP11, FSlot->get_SlotId(), PinCode);

}

CP11TokenInfo::~CP11TokenInfo() {
    if (NULL != FSessionInfo)
        delete FSessionInfo;
    MechanismListIterator mit;
    for (mit = FMechanisms.begin(); mit != FMechanisms.end(); mit++) {
        delete *mit;
    }
}

/*-----------------------------------------------------------------------------
  Methode:      GetMechanism
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Index: integer
  Resultaat:    TP11Mechanism
  Omschrijving: return a mechanism
-----------------------------------------------------------------------------*/
CP11Mechanism& CP11TokenInfo::GetMechanism(UINT Index) {
    if ((Index >= 0) && (Index < FMechanisms.size())) {
        return *FMechanisms.at(Index);
    } else {
        char str[128];
        sprintf_s(str, sizeof (str), "TP11Token.GetMechanism: index out of range (%d)", Index);
        throw string(str);
    }
}

/*-----------------------------------------------------------------------------
  Methode:      GetMechanismCount
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    integer
  Omschrijving: return number of mechanisms
-----------------------------------------------------------------------------*/
int CP11TokenInfo::GetMechanismCount() {
    return (int) FMechanisms.size();
}

/*-----------------------------------------------------------------------------
  Methode:      GetSessionInfo
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    TP11SessionInfo
  Omschrijving: return the SessionInfo object for this token
-----------------------------------------------------------------------------*/
CP11SessionInfo& CP11TokenInfo::GetSessionInfo() {
    if (FSessionInfo != NULL)
        return *FSessionInfo;
    else
        throw string("TP11TokenInfo.GetSessionInfo: no session info present.");
}

/* TP11Mechanism */

/*-----------------------------------------------------------------------------
  Methode:      Create
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   aMechType: CK_MECHANISM_TYPE; aMechInfo: CK_MECHANISM_INFO
  Resultaat:    None
  Omschrijving: decode the mechanism into a string
-----------------------------------------------------------------------------*/
CP11Mechanism::CP11Mechanism(CK_MECHANISM_TYPE aMechType, CK_MECHANISM_INFO aMechInfo) {
    FMechType = aMechType;
    FMechTypeString = "Unknown Mechanism";
    switch (FMechType) {
        case CKM_RSA_PKCS_KEY_PAIR_GEN: FMechTypeString = "CKM_RSA_PKCS_KEY_PAIR_GEN";
            break;
        case CKM_RSA_PKCS: FMechTypeString = "CKM_RSA_PKCS";
            break;
        case CKM_RSA_9796: FMechTypeString = "CKM_RSA_9796";
            break;
        case CKM_RSA_X_509: FMechTypeString = "CKM_RSA_X_509";
            break;
            // CKM_MD2_RSA_PKCS, CKM_MD5_RSA_PKCS, and CKM_SHA1_RSA_PKCS */
            // are new for v2.0. They are mechanisms which hash and sign */
        case CKM_MD2_RSA_PKCS: FMechTypeString = "CKM_MD2_RSA_PKCS";
            break;
        case CKM_MD5_RSA_PKCS: FMechTypeString = "CKM_MD5_RSA_PKCS";
            break;
        case CKM_SHA1_RSA_PKCS: FMechTypeString = "CKM_SHA1_RSA_PKCS";
            break;
            // CKM_RIPEMD128_RSA_PKCS, CKM_RIPEMD160_RSA_PKCS, and */
            // CKM_RSA_PKCS_OAEP are new for v2.10 */
        case CKM_RIPEMD128_RSA_PKCS: FMechTypeString = "CKM_RIPEMD128_RSA_PKCS";
            break;
        case CKM_RIPEMD160_RSA_PKCS: FMechTypeString = "CKM_RIPEMD160_RSA_PKCS";
            break;
        case CKM_RSA_PKCS_OAEP: FMechTypeString = "CKM_RSA_PKCS_OAEP";
            break;
            // CKM_RSA_X9_31_KEY_PAIR_GEN, CKM_RSA_X9_31, CKM_SHA1_RSA_X9_31, */
            // CKM_RSA_PKCS_PSS, and CKM_SHA1_RSA_PKCS_PSS are new for v2.11 */
        case CKM_RSA_X9_31_KEY_PAIR_GEN: FMechTypeString = "CKM_RSA_X9_31_KEY_PAIR_GEN";
            break;
        case CKM_RSA_X9_31: FMechTypeString = "CKM_RSA_X9_31";
            break;
        case CKM_SHA1_RSA_X9_31: FMechTypeString = "CKM_SHA1_RSA_X9_31";
            break;
        case CKM_RSA_PKCS_PSS: FMechTypeString = "CKM_RSA_PKCS_PSS";
            break;
        case CKM_SHA1_RSA_PKCS_PSS: FMechTypeString = "CKM_SHA1_RSA_PKCS_PSS";
            break;
        case CKM_DSA_KEY_PAIR_GEN: FMechTypeString = "CKM_DSA_KEY_PAIR_GEN";
            break;
        case CKM_DSA: FMechTypeString = "CKM_DSA";
            break;
        case CKM_DSA_SHA1: FMechTypeString = "CKM_DSA_SHA1";
            break;
        case CKM_DH_PKCS_KEY_PAIR_GEN: FMechTypeString = "CKM_DH_PKCS_KEY_PAIR_GEN";
            break;
        case CKM_DH_PKCS_DERIVE: FMechTypeString = "CKM_DH_PKCS_DERIVE";
            break;
            // CKM_X9_42_DH_KEY_PAIR_GEN, CKM_X9_42_DH_DERIVE, */
            // CKM_X9_42_DH_HYBRID_DERIVE, and CKM_X9_42_MQV_DERIVE are new for */
            // v2.11 */
        case CKM_X9_42_DH_KEY_PAIR_GEN: FMechTypeString = "CKM_X9_42_DH_KEY_PAIR_GEN";
            break;
        case CKM_X9_42_DH_DERIVE: FMechTypeString = "CKM_X9_42_DH_DERIVE";
            break;
        case CKM_X9_42_DH_HYBRID_DERIVE: FMechTypeString = "CKM_X9_42_DH_HYBRID_DERIVE";
            break;
        case CKM_X9_42_MQV_DERIVE: FMechTypeString = "CKM_X9_42_MQV_DERIVE";
            break;
            // CKM_SHA256/384/512 are new for v2.20 */ */
        case CKM_SHA256_RSA_PKCS: FMechTypeString = "CKM_SHA256_RSA_PKCS";
            break;
        case CKM_SHA384_RSA_PKCS: FMechTypeString = "CKM_SHA384_RSA_PKCS";
            break;
        case CKM_SHA512_RSA_PKCS: FMechTypeString = "CKM_SHA512_RSA_PKCS";
            break;
        case CKM_SHA256_RSA_PKCS_PSS: FMechTypeString = "CKM_SHA256_RSA_PKCS_PSS";
            break;
        case CKM_SHA384_RSA_PKCS_PSS: FMechTypeString = "CKM_SHA384_RSA_PKCS_PSS";
            break;
        case CKM_SHA512_RSA_PKCS_PSS: FMechTypeString = "CKM_SHA512_RSA_PKCS_PSS";
            break;
        case CKM_RC2_KEY_GEN: FMechTypeString = "CKM_RC2_KEY_GEN";
            break;
        case CKM_RC2_ECB: FMechTypeString = "CKM_RC2_ECB";
            break;
        case CKM_RC2_CBC: FMechTypeString = "CKM_RC2_CBC";
            break;
        case CKM_RC2_MAC: FMechTypeString = "CKM_RC2_MAC";
            break;
            // CKM_RC2_MAC_GENERAL and CKM_RC2_CBC_PAD are new for v2.0*/ */
        case CKM_RC2_MAC_GENERAL: FMechTypeString = "CKM_RC2_MAC_GENERAL";
            break;
        case CKM_RC2_CBC_PAD: FMechTypeString = "CKM_RC2_CBC_PAD";
            break;
        case CKM_RC4_KEY_GEN: FMechTypeString = "CKM_RC4_KEY_GEN";
            break;
        case CKM_RC4: FMechTypeString = "CKM_RC4";
            break;
        case CKM_DES_KEY_GEN: FMechTypeString = "CKM_DES_KEY_GEN";
            break;
        case CKM_DES_ECB: FMechTypeString = "CKM_DES_ECB";
            break;
        case CKM_DES_CBC: FMechTypeString = "CKM_DES_CBC";
            break;
        case CKM_DES_MAC: FMechTypeString = "CKM_DES_MAC";
            break;
            // CKM_DES_MAC_GENERAL and CKM_DES_CBC_PAD are new for v2.0*/ */
        case CKM_DES_MAC_GENERAL: FMechTypeString = "CKM_DES_MAC_GENERAL";
            break;
        case CKM_DES_CBC_PAD: FMechTypeString = "CKM_DES_CBC_PAD";
            break;
        case CKM_DES2_KEY_GEN: FMechTypeString = "CKM_DES2_KEY_GEN";
            break;
        case CKM_DES3_KEY_GEN: FMechTypeString = "CKM_DES3_KEY_GEN";
            break;
        case CKM_DES3_ECB: FMechTypeString = "CKM_DES3_ECB";
            break;
        case CKM_DES3_CBC: FMechTypeString = "CKM_DES3_CBC";
            break;
        case CKM_DES3_MAC: FMechTypeString = "CKM_DES3_MAC";
            break;
            // CKM_DES3_MAC_GENERAL, CKM_DES3_CBC_PAD, CKM_CDMF_KEY_GEN, */
            // CKM_CDMF_ECB, CKM_CDMF_CBC, CKM_CDMF_MAC, */
            // CKM_CDMF_MAC_GENERAL, and CKM_CDMF_CBC_PAD are new for v2.0 */
        case CKM_DES3_MAC_GENERAL: FMechTypeString = "CKM_DES3_MAC_GENERAL";
            break;
        case CKM_DES3_CBC_PAD: FMechTypeString = "CKM_DES3_CBC_PAD";
            break;
        case CKM_CDMF_KEY_GEN: FMechTypeString = "CKM_CDMF_KEY_GEN";
            break;
        case CKM_CDMF_ECB: FMechTypeString = "CKM_CDMF_ECB";
            break;
        case CKM_CDMF_CBC: FMechTypeString = "CKM_CDMF_CBC";
            break;
        case CKM_CDMF_MAC: FMechTypeString = "CKM_CDMF_MAC";
            break;
        case CKM_CDMF_MAC_GENERAL: FMechTypeString = "CKM_CDMF_MAC_GENERAL";
            break;
        case CKM_CDMF_CBC_PAD: FMechTypeString = "CKM_CDMF_CBC_PAD";
            break;
        case CKM_DES_OFB64: FMechTypeString = "CKM_DES_OFB64";
            break;
        case CKM_DES_OFB8: FMechTypeString = "CKM_DES_OFB8";
            break;
        case CKM_DES_CFB64: FMechTypeString = "CKM_DES_CFB64";
            break;
        case CKM_DES_CFB8: FMechTypeString = "CKM_DES_CFB8";
            break;
        case CKM_MD2: FMechTypeString = "CKM_MD2";
            break;
            // CKM_MD2_HMAC and CKM_MD2_HMAC_GENERAL are new for v2.0*/ */
        case CKM_MD2_HMAC: FMechTypeString = "CKM_MD2_HMAC";
            break;
        case CKM_MD2_HMAC_GENERAL: FMechTypeString = "CKM_MD2_HMAC_GENERAL";
            break;
        case CKM_MD5: FMechTypeString = "CKM_MD5";
            break;
            // CKM_MD5_HMAC and CKM_MD5_HMAC_GENERAL are new for v2.0*/ */
        case CKM_MD5_HMAC: FMechTypeString = "CKM_MD5_HMAC";
            break;
        case CKM_MD5_HMAC_GENERAL: FMechTypeString = "CKM_MD5_HMAC_GENERAL";
            break;
        case CKM_SHA_1: FMechTypeString = "CKM_SHA_1";
            break;
            // CKM_SHA_1_HMAC and CKM_SHA_1_HMAC_GENERAL are new for v2.0*/ */
        case CKM_SHA_1_HMAC: FMechTypeString = "CKM_SHA_1_HMAC";
            break;
        case CKM_SHA_1_HMAC_GENERAL: FMechTypeString = "CKM_SHA_1_HMAC_GENERAL";
            break;
            // CKM_RIPEMD128, CKM_RIPEMD128_HMAC, */
            // CKM_RIPEMD128_HMAC_GENERAL, CKM_RIPEMD160, CKM_RIPEMD160_HMAC, */
            // and CKM_RIPEMD160_HMAC_GENERAL are new for v2.10 */
        case CKM_RIPEMD128: FMechTypeString = "CKM_RIPEMD128";
            break;
        case CKM_RIPEMD128_HMAC: FMechTypeString = "CKM_RIPEMD128_HMAC";
            break;
        case CKM_RIPEMD128_HMAC_GENERAL: FMechTypeString = "CKM_RIPEMD128_HMAC_GENERAL";
            break;
        case CKM_RIPEMD160: FMechTypeString = "CKM_RIPEMD160";
            break;
        case CKM_RIPEMD160_HMAC: FMechTypeString = "CKM_RIPEMD160_HMAC";
            break;
        case CKM_RIPEMD160_HMAC_GENERAL: FMechTypeString = "CKM_RIPEMD160_HMAC_GENERAL";
            break;
            // CKM_SHA256/384/512 are new for v2.20 */ */
        case CKM_SHA256: FMechTypeString = "CKM_SHA256";
            break;
        case CKM_SHA256_HMAC: FMechTypeString = "CKM_SHA256_HMAC";
            break;
        case CKM_SHA256_HMAC_GENERAL: FMechTypeString = "CKM_SHA256_HMAC_GENERAL";
            break;
        case CKM_SHA384: FMechTypeString = "CKM_SHA384";
            break;
        case CKM_SHA384_HMAC: FMechTypeString = "CKM_SHA384_HMAC";
            break;
        case CKM_SHA384_HMAC_GENERAL: FMechTypeString = "CKM_SHA384_HMAC_GENERAL";
            break;
        case CKM_SHA512: FMechTypeString = "CKM_SHA512";
            break;
        case CKM_SHA512_HMAC: FMechTypeString = "CKM_SHA512_HMAC";
            break;
        case CKM_SHA512_HMAC_GENERAL: FMechTypeString = "CKM_SHA512_HMAC_GENERAL";
            break;
            // All of the following mechanisms are new for v2.0*/ */
            // Note that CAST128 and CAST5 are the same algorithm*/ */
        case CKM_CAST_KEY_GEN: FMechTypeString = "CKM_CAST_KEY_GEN";
            break;
        case CKM_CAST_ECB: FMechTypeString = "CKM_CAST_ECB";
            break;
        case CKM_CAST_CBC: FMechTypeString = "CKM_CAST_CBC";
            break;
        case CKM_CAST_MAC: FMechTypeString = "CKM_CAST_MAC";
            break;
        case CKM_CAST_MAC_GENERAL: FMechTypeString = "CKM_CAST_MAC_GENERAL";
            break;
        case CKM_CAST_CBC_PAD: FMechTypeString = "CKM_CAST_CBC_PAD";
            break;
        case CKM_CAST3_KEY_GEN: FMechTypeString = "CKM_CAST3_KEY_GEN";
            break;
        case CKM_CAST3_ECB: FMechTypeString = "CKM_CAST3_ECB";
            break;
        case CKM_CAST3_CBC: FMechTypeString = "CKM_CAST3_CBC";
            break;
        case CKM_CAST3_MAC: FMechTypeString = "CKM_CAST3_MAC";
            break;
        case CKM_CAST3_MAC_GENERAL: FMechTypeString = "CKM_CAST3_MAC_GENERAL";
            break;
        case CKM_CAST3_CBC_PAD: FMechTypeString = "CKM_CAST3_CBC_PAD";
            break;
        case CKM_CAST128_KEY_GEN: FMechTypeString = "CKM_CAST128_KEY_GEN";
            break;
        case CKM_CAST128_ECB: FMechTypeString = "CKM_CAST128_ECB";
            break;
        case CKM_CAST128_CBC: FMechTypeString = "CKM_CAST128_CBC";
            break;
        case CKM_CAST128_MAC: FMechTypeString = "CKM_CAST128_MAC";
            break;
        case CKM_CAST128_MAC_GENERAL: FMechTypeString = "CKM_CAST128_MAC_GENERAL";
            break;
        case CKM_CAST128_CBC_PAD: FMechTypeString = "CKM_CAST128_CBC_PAD";
            break;
        case CKM_RC5_KEY_GEN: FMechTypeString = "CKM_RC5_KEY_GEN";
            break;
        case CKM_RC5_ECB: FMechTypeString = "CKM_RC5_ECB";
            break;
        case CKM_RC5_CBC: FMechTypeString = "CKM_RC5_CBC";
            break;
        case CKM_RC5_MAC: FMechTypeString = "CKM_RC5_MAC";
            break;
        case CKM_RC5_MAC_GENERAL: FMechTypeString = "CKM_RC5_MAC_GENERAL";
            break;
        case CKM_RC5_CBC_PAD: FMechTypeString = "CKM_RC5_CBC_PAD";
            break;
        case CKM_IDEA_KEY_GEN: FMechTypeString = "CKM_IDEA_KEY_GEN";
            break;
        case CKM_IDEA_ECB: FMechTypeString = "CKM_IDEA_ECB";
            break;
        case CKM_IDEA_CBC: FMechTypeString = "CKM_IDEA_CBC";
            break;
        case CKM_IDEA_MAC: FMechTypeString = "CKM_IDEA_MAC";
            break;
        case CKM_IDEA_MAC_GENERAL: FMechTypeString = "CKM_IDEA_MAC_GENERAL";
            break;
        case CKM_IDEA_CBC_PAD: FMechTypeString = "CKM_IDEA_CBC_PAD";
            break;
        case CKM_GENERIC_SECRET_KEY_GEN: FMechTypeString = "CKM_GENERIC_SECRET_KEY_GEN";
            break;
        case CKM_CONCATENATE_BASE_AND_KEY: FMechTypeString = "CKM_CONCATENATE_BASE_AND_KEY";
            break;
        case CKM_CONCATENATE_BASE_AND_DATA: FMechTypeString = "CKM_CONCATENATE_BASE_AND_DATA";
            break;
        case CKM_CONCATENATE_DATA_AND_BASE: FMechTypeString = "CKM_CONCATENATE_DATA_AND_BASE";
            break;
        case CKM_XOR_BASE_AND_DATA: FMechTypeString = "CKM_XOR_BASE_AND_DATA";
            break;
        case CKM_EXTRACT_KEY_FROM_KEY: FMechTypeString = "CKM_EXTRACT_KEY_FROM_KEY";
            break;
        case CKM_SSL3_PRE_MASTER_KEY_GEN: FMechTypeString = "CKM_SSL3_PRE_MASTER_KEY_GEN";
            break;
        case CKM_SSL3_MASTER_KEY_DERIVE: FMechTypeString = "CKM_SSL3_MASTER_KEY_DERIVE";
            break;
        case CKM_SSL3_KEY_AND_MAC_DERIVE: FMechTypeString = "CKM_SSL3_KEY_AND_MAC_DERIVE";
            break;
            // CKM_SSL3_MASTER_KEY_DERIVE_DH, CKM_TLS_PRE_MASTER_KEY_GEN, */
            // CKM_TLS_MASTER_KEY_DERIVE, CKM_TLS_KEY_AND_MAC_DERIVE, and */
            // CKM_TLS_MASTER_KEY_DERIVE_DH are new for v2.11 */
        case CKM_SSL3_MASTER_KEY_DERIVE_DH: FMechTypeString = "CKM_SSL3_MASTER_KEY_DERIVE_DH";
            break;
        case CKM_TLS_PRE_MASTER_KEY_GEN: FMechTypeString = "CKM_TLS_PRE_MASTER_KEY_GEN";
            break;
        case CKM_TLS_MASTER_KEY_DERIVE: FMechTypeString = "CKM_TLS_MASTER_KEY_DERIVE";
            break;
        case CKM_TLS_KEY_AND_MAC_DERIVE: FMechTypeString = "CKM_TLS_KEY_AND_MAC_DERIVE";
            break;
        case CKM_TLS_MASTER_KEY_DERIVE_DH: FMechTypeString = "CKM_TLS_MASTER_KEY_DERIVE_DH";
            break;
            // CKM_TLS_PRF is new for v2.20 */ */
        case CKM_TLS_PRF: FMechTypeString = "CKM_TLS_PRF";
            break;
        case CKM_SSL3_MD5_MAC: FMechTypeString = "CKM_SSL3_MD5_MAC";
            break;
        case CKM_SSL3_SHA1_MAC: FMechTypeString = "CKM_SSL3_SHA1_MAC";
            break;
        case CKM_MD5_KEY_DERIVATION: FMechTypeString = "CKM_MD5_KEY_DERIVATION";
            break;
        case CKM_MD2_KEY_DERIVATION: FMechTypeString = "CKM_MD2_KEY_DERIVATION";
            break;
        case CKM_SHA1_KEY_DERIVATION: FMechTypeString = "CKM_SHA1_KEY_DERIVATION";
            break;
            // CKM_SHA256/384/512 are new for v2.20 */ */
        case CKM_SHA256_KEY_DERIVATION: FMechTypeString = "CKM_SHA256_KEY_DERIVATION";
            break;
        case CKM_SHA384_KEY_DERIVATION: FMechTypeString = "CKM_SHA384_KEY_DERIVATION";
            break;
        case CKM_SHA512_KEY_DERIVATION: FMechTypeString = "CKM_SHA512_KEY_DERIVATION";
            break;
        case CKM_PBE_MD2_DES_CBC: FMechTypeString = "CKM_PBE_MD2_DES_CBC";
            break;
        case CKM_PBE_MD5_DES_CBC: FMechTypeString = "CKM_PBE_MD5_DES_CBC";
            break;
        case CKM_PBE_MD5_CAST_CBC: FMechTypeString = "CKM_PBE_MD5_CAST_CBC";
            break;
        case CKM_PBE_MD5_CAST3_CBC: FMechTypeString = "CKM_PBE_MD5_CAST3_CBC";
            break;
        case CKM_PBE_MD5_CAST128_CBC: FMechTypeString = "CKM_PBE_MD5_CAST128_CBC";
            break;
        case CKM_PBE_SHA1_CAST128_CBC: FMechTypeString = "CKM_PBE_SHA1_CAST128_CBC";
            break;
        case CKM_PBE_SHA1_RC4_128: FMechTypeString = "CKM_PBE_SHA1_RC4_128";
            break;
        case CKM_PBE_SHA1_RC4_40: FMechTypeString = "CKM_PBE_SHA1_RC4_40";
            break;
        case CKM_PBE_SHA1_DES3_EDE_CBC: FMechTypeString = "CKM_PBE_SHA1_DES3_EDE_CBC";
            break;
        case CKM_PBE_SHA1_DES2_EDE_CBC: FMechTypeString = "CKM_PBE_SHA1_DES2_EDE_CBC";
            break;
        case CKM_PBE_SHA1_RC2_128_CBC: FMechTypeString = "CKM_PBE_SHA1_RC2_128_CBC";
            break;
        case CKM_PBE_SHA1_RC2_40_CBC: FMechTypeString = "CKM_PBE_SHA1_RC2_40_CBC";
            break;
            // CKM_PKCS5_PBKD2 is new for v2.10*/ */
        case CKM_PKCS5_PBKD2: FMechTypeString = "CKM_PKCS5_PBKD2";
            break;
        case CKM_PBA_SHA1_WITH_SHA1_HMAC: FMechTypeString = "CKM_PBA_SHA1_WITH_SHA1_HMAC";
            break;
        case CKM_WTLS_PRE_MASTER_KEY_GEN: FMechTypeString = "CKM_WTLS_PRE_MASTER_KEY_GEN";
            break;
        case CKM_WTLS_MASTER_KEY_DERIVE: FMechTypeString = "CKM_WTLS_MASTER_KEY_DERIVE";
            break;
        case CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC: FMechTypeString = "CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC";
            break;
        case CKM_WTLS_PRF: FMechTypeString = "CKM_WTLS_PRF";
            break;
        case CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE: FMechTypeString = "CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE";
            break;
        case CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE: FMechTypeString = "CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE";
            break;
            // CKM_CMS_SIG is new for v2.20 */ */
        case CKM_CMS_SIG: FMechTypeString = "CKM_CMS_SIG";
            break;
        case CKM_KEY_WRAP_LYNKS: FMechTypeString = "CKM_KEY_WRAP_LYNKS";
            break;
        case CKM_KEY_WRAP_SET_OAEP: FMechTypeString = "CKM_KEY_WRAP_SET_OAEP";
            break;
            // Fortezza mechanisms*/ */
        case CKM_SKIPJACK_KEY_GEN: FMechTypeString = "CKM_SKIPJACK_KEY_GEN";
            break;
        case CKM_SKIPJACK_ECB64: FMechTypeString = "CKM_SKIPJACK_ECB64";
            break;
        case CKM_SKIPJACK_CBC64: FMechTypeString = "CKM_SKIPJACK_CBC64";
            break;
        case CKM_SKIPJACK_OFB64: FMechTypeString = "CKM_SKIPJACK_OFB64";
            break;
        case CKM_SKIPJACK_CFB64: FMechTypeString = "CKM_SKIPJACK_CFB64";
            break;
        case CKM_SKIPJACK_CFB32: FMechTypeString = "CKM_SKIPJACK_CFB32";
            break;
        case CKM_SKIPJACK_CFB16: FMechTypeString = "CKM_SKIPJACK_CFB16";
            break;
        case CKM_SKIPJACK_CFB8: FMechTypeString = "CKM_SKIPJACK_CFB8";
            break;
        case CKM_SKIPJACK_WRAP: FMechTypeString = "CKM_SKIPJACK_WRAP";
            break;
        case CKM_SKIPJACK_PRIVATE_WRAP: FMechTypeString = "CKM_SKIPJACK_PRIVATE_WRAP";
            break;
        case CKM_SKIPJACK_RELAYX: FMechTypeString = "CKM_SKIPJACK_RELAYX";
            break;
        case CKM_KEA_KEY_PAIR_GEN: FMechTypeString = "CKM_KEA_KEY_PAIR_GEN";
            break;
        case CKM_KEA_KEY_DERIVE: FMechTypeString = "CKM_KEA_KEY_DERIVE";
            break;
        case CKM_FORTEZZA_TIMESTAMP: FMechTypeString = "CKM_FORTEZZA_TIMESTAMP";
            break;
        case CKM_BATON_KEY_GEN: FMechTypeString = "CKM_BATON_KEY_GEN";
            break;
        case CKM_BATON_ECB128: FMechTypeString = "CKM_BATON_ECB128";
            break;
        case CKM_BATON_ECB96: FMechTypeString = "CKM_BATON_ECB96";
            break;
        case CKM_BATON_CBC128: FMechTypeString = "CKM_BATON_CBC128";
            break;
        case CKM_BATON_COUNTER: FMechTypeString = "CKM_BATON_COUNTER";
            break;
        case CKM_BATON_SHUFFLE: FMechTypeString = "CKM_BATON_SHUFFLE";
            break;
        case CKM_BATON_WRAP: FMechTypeString = "CKM_BATON_WRAP";
            break;
            // CKM_ECDSA_KEY_PAIR_GEN is deprecated in v2.11, */
            // CKM_EC_KEY_PAIR_GEN is preferred */
        case CKM_EC_KEY_PAIR_GEN: FMechTypeString = "CKM_EC_KEY_PAIR_GEN";
            break;
        case CKM_ECDSA: FMechTypeString = "CKM_ECDSA";
            break;
        case CKM_ECDSA_SHA1: FMechTypeString = "CKM_ECDSA_SHA1";
            break;
            // CKM_ECDH1_DERIVE, CKM_ECDH1_COFACTOR_DERIVE, and CKM_ECMQV_DERIVE */
            // are new for v2.11 */
        case CKM_ECDH1_DERIVE: FMechTypeString = "CKM_ECDH1_DERIVE";
            break;
        case CKM_ECDH1_COFACTOR_DERIVE: FMechTypeString = "CKM_ECDH1_COFACTOR_DERIVE";
            break;
        case CKM_ECMQV_DERIVE: FMechTypeString = "CKM_ECMQV_DERIVE";
            break;
        case CKM_JUNIPER_KEY_GEN: FMechTypeString = "CKM_JUNIPER_KEY_GEN";
            break;
        case CKM_JUNIPER_ECB128: FMechTypeString = "CKM_JUNIPER_ECB128";
            break;
        case CKM_JUNIPER_CBC128: FMechTypeString = "CKM_JUNIPER_CBC128";
            break;
        case CKM_JUNIPER_COUNTER: FMechTypeString = "CKM_JUNIPER_COUNTER";
            break;
        case CKM_JUNIPER_SHUFFLE: FMechTypeString = "CKM_JUNIPER_SHUFFLE";
            break;
        case CKM_JUNIPER_WRAP: FMechTypeString = "CKM_JUNIPER_WRAP";
            break;
        case CKM_FASTHASH: FMechTypeString = "CKM_FASTHASH";
            break;
            // CKM_AES_KEY_GEN, CKM_AES_ECB, CKM_AES_CBC, CKM_AES_MAC, */
            // CKM_AES_MAC_GENERAL, CKM_AES_CBC_PAD, CKM_DSA_PARAMETER_GEN, */
            // CKM_DH_PKCS_PARAMETER_GEN, and CKM_X9_42_DH_PARAMETER_GEN are */
            // new for v2.11 */
        case CKM_AES_KEY_GEN: FMechTypeString = "CKM_AES_KEY_GEN";
            break;
        case CKM_AES_ECB: FMechTypeString = "CKM_AES_ECB";
            break;
        case CKM_AES_CBC: FMechTypeString = "CKM_AES_CBC";
            break;
        case CKM_AES_MAC: FMechTypeString = "CKM_AES_MAC";
            break;
        case CKM_AES_MAC_GENERAL: FMechTypeString = "CKM_AES_MAC_GENERAL";
            break;
        case CKM_AES_CBC_PAD: FMechTypeString = "CKM_AES_CBC_PAD";
            break;
        case CKM_BLOWFISH_KEY_GEN: FMechTypeString = "CKM_BLOWFISH_KEY_GEN";
            break;
        case CKM_BLOWFISH_CBC: FMechTypeString = "CKM_BLOWFISH_KEY_GEN";
            break;
        case CKM_TWOFISH_KEY_GEN: FMechTypeString = "CKM_BLOWFISH_KEY_GEN";
            break;
        case CKM_TWOFISH_CBC: FMechTypeString = "CKM_BLOWFISH_KEY_GEN";
            break;
            // CKM_xxx_ENCRYPT_DATA mechanisms are new for v2.20 */ */
        case CKM_DES_ECB_ENCRYPT_DATA: FMechTypeString = "CKM_DES_ECB_ENCRYPT_DATA";
            break;
        case CKM_DES_CBC_ENCRYPT_DATA: FMechTypeString = "CKM_DES_CBC_ENCRYPT_DATA";
            break;
        case CKM_DES3_ECB_ENCRYPT_DATA: FMechTypeString = "CKM_DES3_ECB_ENCRYPT_DATA";
            break;
        case CKM_DES3_CBC_ENCRYPT_DATA: FMechTypeString = "CKM_DES3_CBC_ENCRYPT_DATA";
            break;
        case CKM_AES_ECB_ENCRYPT_DATA: FMechTypeString = "CKM_AES_ECB_ENCRYPT_DATA";
            break;
        case CKM_AES_CBC_ENCRYPT_DATA: FMechTypeString = "CKM_AES_CBC_ENCRYPT_DATA";
            break;
        case CKM_DSA_PARAMETER_GEN: FMechTypeString = "CKM_DSA_PARAMETER_GEN";
            break;
        case CKM_DH_PKCS_PARAMETER_GEN: FMechTypeString = "CKM_DH_PKCS_PARAMETER_GEN";
            break;
        case CKM_X9_42_DH_PARAMETER_GEN: FMechTypeString = "CKM_X9_42_DH_PARAMETER_GEN";
            break;
        case CKM_VENDOR_DEFINED: FMechTypeString = "CKM_VENDOR_DEFINED";
            break;
    }
    FFlags = aMechInfo.flags;
    FMaxKeySize = aMechInfo.ulMaxKeySize;
    FMinKeySize = aMechInfo.ulMinKeySize;
}

CP11Mechanism::~CP11Mechanism() {
}

/* TP11SessionInfo */

/*-----------------------------------------------------------------------------
  Methode:      Create
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   P11FunctionList: CK_FUNCTION_LIST_PTR; SlotId: CK_SLOT_ID
  Resultaat:    None
  Omschrijving: Open a session, enumerate all objects and store in a list
 ----------------------------------------------------------------------------- */
CP11SessionInfo::CP11SessionInfo(CP11TokenInfo& Token, CK_FUNCTION_LIST_PTR P11FunctionList, CK_SLOT_ID SlotId, std::string const& PinCode)
: FP11(P11FunctionList), FTokenInfo(Token), FLoggedIn(false) {

    CK_RV rv;
    unsigned long ulCount;
    const int MAX_OBJS = 32;
    CK_OBJECT_HANDLE hObjects[MAX_OBJS];
    UINT i;

    /* open session */
    rv = FP11->C_OpenSession(SlotId, CKF_SERIAL_SESSION | CKF_RW_SESSION, 00, 00, &FhSession);
    CPKCS11::Check_RV(rv, "C_OpenSession");
    if (PinCode != "")
        this->Login(CKU_USER, PinCode);
    else if (PinCode == "xxxx")
        this->Login(CKU_USER, "");
    /* get objects */
    rv = FP11->C_FindObjectsInit(FhSession, 00, 0);
    CPKCS11::Check_RV(rv, "C_FindObjectsInit");
    while (true) {
        /* get next object handles */
        ulCount = 0;
        rv = FP11->C_FindObjects(FhSession, &hObjects[ 0 ], MAX_OBJS, &ulCount);
        CPKCS11::Check_RV(rv, "C_FindObjects");
        if (ulCount == 0)
            break;
        /* add the new objects to the list */
        for (i = 0; i < ulCount; ++i) {
            FObjects.push_back(new CP11Object(FP11, FhSession, hObjects[ i ]));
        }
    } /* list objects */
    rv = FP11->C_FindObjectsFinal(FhSession);
    CPKCS11::Check_RV(rv, "C_FindObjectsFinal");

}

/*-----------------------------------------------------------------------------
  Methode:      Destroy
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    None
  Omschrijving: Clean up
-----------------------------------------------------------------------------*/
CP11SessionInfo::~CP11SessionInfo() {

    CK_RV rv;

    if (this->FhSession != 0) {
        rv = this->FP11->C_CloseSession(FhSession);
        this->FhSession = 0;
        if (rv != CKR_SESSION_HANDLE_INVALID)
            CPKCS11::Check_RV(rv, "C_CloseSession");
    }
    CP11ObjectListIterator fit;
    for (fit = this->FObjects.begin(); fit != this->FObjects.end(); fit++) {
        delete *fit;
    }
}

/*-----------------------------------------------------------------------------
  Methode:      Digest
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Algorithm: pkcs11t.CK_MECHANISM_TYPE; DataMessage: AnsiString
  Resultaat:    AnsiString
  Omschrijving: Compute a hash
-----------------------------------------------------------------------------*/
Byte_Array CP11SessionInfo::Digest(CK_MECHANISM_TYPE Algorithm, const string& DataMessage, int *length) {

    string result;
    CK_MECHANISM DigestMechanism;
    CK_RV rv;
    unsigned long ulCount;


    result = "";

    /* Prepare mechanism */
    DigestMechanism.mechanism = Algorithm;
    DigestMechanism.pParameter = 00;
    DigestMechanism.ulParameterLen = 0;
    /* start digest operation */
    rv = FP11->C_DigestInit(FhSession, &DigestMechanism);
    CPKCS11::Check_RV(rv, "C_DigestInit");
    /* get length of hash */
    ulCount = 0;
    byte ch;
    rv = FP11->C_Digest
            (FhSession
            , (CK_BYTE_PTR) DataMessage.c_str()
            , (CK_ULONG) DataMessage.size()
            , &ch
            , &ulCount);
    if (rv != CKR_BUFFER_TOO_SMALL)
        CPKCS11::Check_RV(rv, "C_Digest");
    Byte_Array digest = new byte[ulCount];
    /* get hash */
    rv = FP11->C_Digest
            (FhSession
            , (CK_BYTE_PTR) DataMessage.c_str()
            , (CK_ULONG) DataMessage.size()
            , digest
            , &ulCount);
    CPKCS11::Check_RV(rv, "C_Digest");

    *length = ulCount;

    return digest;

}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectCount
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    integer
  Omschrijving: return the number of objects
-----------------------------------------------------------------------------*/
int CP11SessionInfo::GetObjectCount() {
    return (int) FObjects.size();
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjects
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Index: integer
  Resultaat:    TP11Object
  Omschrijving: return a particular pkcs11 object
-----------------------------------------------------------------------------*/
CP11Object& CP11SessionInfo::GetObjects(UINT Index) {
    if ((Index < FObjects.size()) && (Index >= 0))
        return *FObjects.at(Index);
    else {
        char msg[128];
        sprintf_s(msg, sizeof (msg), "TP11SessionInfo.GetObjects: index out of range (%d)", Index);
        string exc = msg;
        throw exc;
    }
}

/*-----------------------------------------------------------------------------
  Methode:      GetSessionHandle
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    CK_SESSION_HANDLE
  Omschrijving: the pkcs11 session handle
-----------------------------------------------------------------------------*/
CK_SESSION_HANDLE CP11SessionInfo::GetSessionHandle() {
    return FhSession;
}

/*-----------------------------------------------------------------------------
  Methode:      Login
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   Mode: pkcs11t.CK_USER_TYPE; Pincode: string
  Resultaat:    boolean
  Omschrijving: login to the token with a pincode
 ----------------------------------------------------------------------------- */
bool CP11SessionInfo::Login(CK_USER_TYPE Mode, std::string const& PinCode) {

    CK_RV rv;

    if (FLoggedIn) {
        return true;
    }
    if (PinCode == "")
        rv = FP11->C_Login(FhSession, Mode, 00, 0);
    else
        rv = FP11->C_Login(FhSession, Mode, (CK_UTF8CHAR_PTR) PinCode.c_str(), (CK_ULONG) PinCode .size());
    CPKCS11::Check_RV(rv, "C_Login");
    FLoggedIn = true;
    return FLoggedIn;
}

/*-----------------------------------------------------------------------------
  Methode:      Logout
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    boolean
  Omschrijving: logout from the token
-----------------------------------------------------------------------------*/
bool CP11SessionInfo::Logout() {

    CK_RV rv;

    if (!FLoggedIn) {
        return false;
    }
    rv = FP11->C_Logout(FhSession);
    CPKCS11::Check_RV(rv, "C_Logout");
    return true;
}

/*-----------------------------------------------------------------------------
  Methode:      Sign
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   KeyObject: TP11Object; Algorithm: CK_MECHANISM_TYPE; DataMessage: AnsiString
  Resultaat:    AnsiString
  Omschrijving: sign a byte string with a key object using a particular mechanism
-----------------------------------------------------------------------------*/
Byte_Array CP11SessionInfo::Sign(CP11Object KeyObject, CK_MECHANISM_TYPE Algorithm, const string& DataMessage, int *length) {

    CK_MECHANISM SigMechanism;
    unsigned long ulCount;
    CK_RV rv;


    /* Set up mechanism */
    SigMechanism.mechanism = Algorithm;
    SigMechanism.pParameter = 00;
    SigMechanism.ulParameterLen = 0;

    /* start sign operation  */
    rv = FP11->C_SignInit(FhSession, &SigMechanism, KeyObject.get_ObjectHandle());
    CPKCS11::Check_RV(rv, "C_SignInit");

    /* get length of signature */
    ulCount = 0;
    rv = FP11->C_Sign
            (FhSession
            , (CK_BYTE_PTR) DataMessage.c_str()
            , (CK_ULONG) DataMessage.size()
            , NULL
            , &ulCount);
    if (rv != CKR_BUFFER_TOO_SMALL)
        CPKCS11::Check_RV(rv, "C_Sign");

    Byte_Array result = new unsigned char[ulCount];
    /* get signature */
    rv = FP11->C_Sign
            (FhSession
            , (CK_BYTE_PTR) DataMessage.c_str()
            , (CK_ULONG) DataMessage.size()
            , (CK_BYTE_PTR) result
            , &ulCount);
    CPKCS11::Check_RV(rv, "C_Sign");

    *length = ulCount;
    return result;
}

/* TP11Object */

/*-----------------------------------------------------------------------------
  Methode:      Create
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   P11FunctionList: CK_FUNCTION_LIST_PTR; hSession: CK_SESSION_HANDLE; hObject: CK_OBJECT_HANDLE
  Resultaat:    None
  Omschrijving: decode and store the attributes of this pkcs11 object
-----------------------------------------------------------------------------*/
CP11Object::CP11Object(CK_FUNCTION_LIST_PTR P11FunctionList, CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)
: FP11(P11FunctionList), FObjValue(NULL), FhObject(hObject), FhSession(hSession), FObjSize(-1) {

    unsigned long ulSize;
    CK_RV rv;

    /* get the size of the object -- not supported in Belpic PKCS11 */
    rv = FP11->C_GetObjectSize(FhSession, FhObject, &ulSize);
    if (rv != CKR_FUNCTION_NOT_SUPPORTED) {
        CPKCS11::Check_RV(rv, "C_GetObjectSize");
        FObjSize = ulSize;
    }

    /* get the class of the object */
    FObjClass = GetObjectClass(FhObject, hSession);
    FObjClassString = GetObjectClassString(FObjClass);

    /* get the label of the object */
    FObjLabel = GetObjectLabel(FhObject, hSession);

    /* get the object value if not a private key */
    if (FObjClass != CKO_PRIVATE_KEY) {
        FObjValue = GetObjectValue(FhObject, FhSession);
    }
}

CP11Object::~CP11Object() {
    if (FObjValue != NULL) {
        delete[] FObjValue;
    }
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectClassString
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   objClass: Cardinal
  Resultaat:    string
  Omschrijving: get string representation of the object class
 ----------------------------------------------------------------------------- */
std::string CP11Object::GetObjectClassString(CK_OBJECT_CLASS objClass) {
    std::string result;
    switch (objClass) {
        case CKO_DATA: result = "Data";
            break;
        case CKO_CERTIFICATE: result = "Certificate";
            break;
        case CKO_PUBLIC_KEY: result = "Public Key";
            break;
        case CKO_PRIVATE_KEY: result = "Private Key";
            break;
        case CKO_SECRET_KEY: result = "Secret Key";
            break;
        case CKO_HW_FEATURE: result = "HW Feature";
            break;
        case CKO_DOMAIN_PARAMETERS: result = "Domain Parameters";
            break;
        case CKO_VENDOR_DEFINED: result = "Vendor Defined";
            break;
    }
    return result;
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectClass
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   hObject: CK_OBJECT_HANDLE; hSession: Cardinal
  Resultaat:    Cardinal
  Omschrijving: decode the object class attribute
-----------------------------------------------------------------------------*/
unsigned long CP11Object::GetObjectClass(CK_OBJECT_HANDLE hObject, unsigned long hSession) {

    unsigned long result;
    CK_ATTRIBUTE Template;

    result = 0;
    Template.type = CKA_CLASS;
    GetObjAttribute(hObject, hSession, Template);
    result = *(unsigned long *) Template.pValue;
    free(Template.pValue);
    return result;
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectHandle
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   None
  Resultaat:    CK_OBJECT_HANDLE
  Omschrijving: return the pkcs11 object handle
-----------------------------------------------------------------------------*/
CK_OBJECT_HANDLE CP11Object::GetObjectHandle() {
    return FhObject;
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectLabel
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   hObject: CK_OBJECT_HANDLE; hSession: Cardinal
  Resultaat:    string
  Omschrijving: decode the object label attribute
-----------------------------------------------------------------------------*/
std::string CP11Object::GetObjectLabel(CK_OBJECT_HANDLE hObject, unsigned long hSession) {

    CK_ATTRIBUTE Template;

    Template.type = CKA_LABEL;
    GetObjAttribute(hObject, hSession, Template);
    auto_vec<char> str(new char[Template.ulValueLen + 1]);
    char *p = str.get();
    memcpy_s(p, Template.ulValueLen, Template.pValue, Template.ulValueLen);
    p[Template.ulValueLen] = 0;
    free(Template.pValue);
    string result = str.get();
    return result;
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjectValue
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   hObject: CK_OBJECT_HANDLE; hSession: Cardinal
  Resultaat:    Byte_Array
  Omschrijving: decode the object value attribute
-----------------------------------------------------------------------------*/
Byte_Array CP11Object::GetObjectValue(CK_OBJECT_HANDLE hObject, unsigned long hSession) {

    CK_ATTRIBUTE Template;

    Template.type = CKA_VALUE;
    GetObjAttribute(hObject, hSession, Template);
    unsigned char *bytes = new unsigned char[Template.ulValueLen];
    memcpy_s(bytes, Template.ulValueLen, Template.pValue, Template.ulValueLen);
    free(Template.pValue);
    FObjSize = Template.ulValueLen;
    return bytes;
}

/*-----------------------------------------------------------------------------
  Methode:      GetObjAttribute
  Auteur:       DH
  Datum:        2005-10-27
  Argumenten:   hObject: CK_OBJECT_HANDLE; hSession: Cardinal; var Template: CK_ATTRIBUTE
  Resultaat:    None
  Omschrijving: get the value of an object attribute
-----------------------------------------------------------------------------*/
void CP11Object::GetObjAttribute(CK_OBJECT_HANDLE hObject, unsigned long hSession, CK_ATTRIBUTE & Template) {
    unsigned long rv;

    Template.pValue = NULL;
    Template.ulValueLen = 0;
    rv = FP11->C_GetAttributeValue(hSession, hObject, &Template, 1);
    char str[32];
    sprintf_s(str, sizeof (str), "%d", Template.type);
    string stype = str;
    CPKCS11::Check_RV(rv, "C_GetAttributeValue:" + stype);
    Template.pValue = malloc(Template.ulValueLen);
    rv = FP11->C_GetAttributeValue(hSession, hObject, &Template, 1);
    CPKCS11::Check_RV(rv, "C_GetAttributeValue:" + stype);
}



//END
