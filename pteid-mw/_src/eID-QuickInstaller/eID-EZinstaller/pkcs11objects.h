//-----------------------------------------------------------------------------
//
//$Id: pkcs11objects.h,v 1.5 2008/07/16 14:09:21 DH Exp $
//
//Release: $Name:  $
//
//Project:  PKCS11 library interface
//Naam:     PKCS11Objects
//Auteur:   DH (danny.heijl@telenet.be)
//Abstract: Delphi PKCS11 abstraction classes
//
//Origineel:          2005-10-25
//Laatste wijziging:  2005-10-25
//
//-----------------------------------------------------------------------------

//History :
//
//- 2005-10-21 : original (DH), a home project
//
//- 2008-07-05: Translated from Delphi Object Pascal to C++ with the aid of Delphi2cpp (DH)
//
#ifndef INCLUDED_PKCS11OBJECTS_H
#define INCLUDED_PKCS11OBJECTS_H

#include <string>
#include <vector>
using namespace std;
#include "cryptoki.h"

#define UINT unsigned int
#define byte unsigned char

/// storage type for PKCS11 object values
typedef unsigned char* Byte_Array;
/// Slot status: Token in the slot or not

typedef enum _TSlotState {
    ssTokenAbsent, ssTokenPresent
} TSlotState;

/* forward declarations */

class CPKCS11;
class CP11SlotInfo;
class CP11Thread;
class CP11TokenInfo;
class CP11SessionInfo;
class CP11Mechanism;
class CP11Object;

/// Slotlist
typedef std::vector <CP11SlotInfo*> SlotList;
typedef SlotList::iterator SlotListIterator;

// Mechanismlist
typedef std::vector <CP11Mechanism*> MechanismList;
typedef MechanismList::iterator MechanismListIterator;

// P11Object list 
typedef std::vector <CP11Object*> CP11ObjectList;
typedef CP11ObjectList::iterator CP11ObjectListIterator;

/// Slot event callback routine (Token inserted/removed events)
typedef void (*TSlotChangeEvent)(TSlotState SlotState, CP11SlotInfo Slot);

/// token change listeners
typedef std::vector<TSlotChangeEvent> ListenerList;
typedef ListenerList::iterator ListenerListIterator;


/// Represents an instance of a PKCS11 library

class CPKCS11 {
private:
    std::string FCryptOkiVersion;
    std::string FDescription;
    void* FLibHandle;
    std::string FLibraryVersion;
    std::string FManufacturerId;
    //TP11Thread* FMonitorThread;
    CK_FUNCTION_LIST_PTR FP11;
    CK_C_INITIALIZE_ARGS FInitArgs;
    /// Contains (and owns!) the SlotInfo objects
    SlotList FSlots;
    /// The array of curre,tly registered slot event listeners
    ListenerList FStateChangeListeners;
    /// Return the number of slots
    unsigned int GetSlotCount();
    /// Returns a SlotInfo
    CP11SlotInfo& GetSlot(UINT Index);

protected:

public:
    /// Create an instance of a PKCS11 library
    CPKCS11(std::string const& LibraryName);
    /// Release PKCS11 module
    ~CPKCS11(void)/*override*/;
    ///// Add new slot change listener
    //void AddTokenListener(TSlotChangeEvent OnSlotChange);
    ///// Remove a slot listener
    //void RemoveTokenListener(TSlotChangeEvent OnSlotChange);

    void SlotChanged(CK_SLOT_ID Slot);

    /// Check PKCS11 CK_RV return value and convert error to string representation /* STATIC */
    static void Check_RV(CK_RV rv, std::string const& FunctionName);

    /// Convert byte array to Delphi AnsiString /* STATIC */
    static std::string Utf8ArrayToString(CK_UTF8CHAR *data);
    static std::string P11StringToString(CK_UTF8CHAR_PTR src, size_t size);


    //* properties*//

    /// Version of PKCS11

    std::string get_CryptokiVersion() {
        return FCryptOkiVersion;
    }
    /// Library Description

    std::string get_Description() {
        return FDescription;
    }
    /// PKCS11 Library version

    std::string get_LibraryVersion() {
        return FLibraryVersion;
    }
    /// Library Manufacturer Id

    std::string get_ManufacturerId() {
        return FManufacturerId;
    }
    /// Pointer to the PKCS11 function table

    CK_FUNCTION_LIST_PTR get_P11() {
        return FP11;
    }
    /// The number of slots found

    unsigned long get_SlotCount() {
        return GetSlotCount();
    }
    /// A PKCS11 Slot

    CP11SlotInfo& get_Slot(int index) {
        return GetSlot(index);
    }
};



/// Contains the current info of an instance of a PKCS11 slot
/// These Slots are are destroyed and recreated on each slot event
/// (token insertion and removal)
/// This means that after any exception you should no longer try to reference
/// a TP11Slot or it's associated TP11TokenInfo

class CP11SlotInfo {
private:
    std::string FFirmwareVersion;
    unsigned long FFlags;
    std::string FHardwareVersion;
    std::string FManufacturerId;
    CK_FUNCTION_LIST_PTR FP11;
    std::string FSlotDescription;
    /// The PKCS11 slot identifier
    CK_SLOT_ID FSlotId;
    /// The field FCurrentTokenInfo contains the TP11TokenInfo holding the
    /// info of the currently inserted Token.
    /// It is only instantiated when needed, not when a token is inserted.
    CP11TokenInfo* FCurrentTokenInfo;
    /// Returns True if a token is currently inserted
    bool GetHaveToken();
    /// token state
    bool _tokenRemoved;
    bool _tokenInserted;
    friend class CPKCS11;
protected:
public:
    /// Create a SlotInfo object
    CP11SlotInfo(CK_FUNCTION_LIST_PTR P11FunctionList, CK_SLOT_ID SlotId, CK_SLOT_INFO SlotInfo);
    virtual ~CP11SlotInfo(void);
    /// Update TokenInfo 
    void UpdateTokenInfo();
    /// Update TokenInfo, login 
    void UpdateTokenInfo(const std::string& PinCode);
    /// Return TokenInfo
    CP11TokenInfo& GetTokenInfo();

    //* properties*//

    /// Reader Firmware Version

    std::string get_FirmwareVersion() {
        return FFirmwareVersion;
    }
    /// Reader Flags

    unsigned long get_Flags() {
        return FFlags;
    }
    /// Hardware version

    std::string get_HardwareVersion() {
        return FHardwareVersion;
    }
    /// Is there currently a token inserted ?

    bool HaveToken() {
        return GetHaveToken();
    }
    /// Reader Manufacturer

    std::string get_ManufacturerId() {
        return FManufacturerId;
    }
    /// Slot description

    std::string get_SlotDescription() {
        return FSlotDescription;
    }
    /// The PKCS11 Slot Id

    CK_SLOT_ID get_SlotId() {
        return FSlotId;
    }
    /// token removed or inserted

    bool IsTokenChanged() {
        return _tokenRemoved || _tokenInserted;
    }

};


/// A Token Mechanism (algorithm for hashing, signing etc...)

class CP11Mechanism {
private:
    /// PKCS11 mechanism flags
    CK_FLAGS FFlags;
    int FMaxKeySize;
    /// The PKCS11 mechanism type
    CK_MECHANISM_TYPE FMechType;
    std::string FMechTypeString;
    int FMinKeySize;
protected:
public:
    CP11Mechanism(CK_MECHANISM_TYPE aMechType, CK_MECHANISM_INFO aMechInfo);
    virtual ~CP11Mechanism(void);

    //* properties*//

    /// Mechanism Flags

    CK_FLAGS get_Flags() {
        return FFlags;
    }
    /// Mechanism Maximum Key Size

    int get_MaxKeySize() {
        return FMaxKeySize;
    }
    /// A Token Mechanism string representation

    CK_MECHANISM_TYPE get_MechType() {
        return FMechType;
    }
    /// Mechanism type as string

    std::string get_MechTypeString() {
        return FMechTypeString;
    }
    /// Mechanism Minimum Key Size

    int get_MinKeySize() {
        return FMinKeySize;
    }
};


/// A PKCS11 Token (smartcard)

class CP11TokenInfo {
private:
    std::string FFirmwareVersion;
    /// PKCS11 token flags
    CK_FLAGS FFlags;
    unsigned long FFreePrivateMemory;
    unsigned long FFreePublicMemory;
    std::string FHardwareVersion;
    unsigned long FMaxRwSessionCount;
    unsigned long FMaxSessionCount;
    /// Contains (and owns!) the list of available mechanisms on this token
    MechanismList FMechanisms;
    CK_FUNCTION_LIST_PTR FP11;
    unsigned long FRwSessionCount;
    unsigned long FSessionCount;
    CP11SessionInfo* FSessionInfo;
    CP11SlotInfo* FSlot;
    std::string FTokenLabel;
    std::string FTokenModel;
    std::string FTokenSerial;
    unsigned long FTotalPrivateMemory;
    unsigned long FTotalPublicMemory;
    std::string FUTCTime;
    CP11Mechanism& GetMechanism(UINT Index);
    int GetMechanismCount();
    CP11SessionInfo& GetSessionInfo();
protected:
public:
    CP11TokenInfo(CP11SlotInfo* Slot, CK_FUNCTION_LIST_PTR P11FunctionList, CK_TOKEN_INFO& TokenInfo, std::string const& PinCode = "");
    virtual ~CP11TokenInfo(void);

    //* properties*//

    /// Token firmware version

    std::string get_FirmwareVersion() {
        return FFirmwareVersion;
    }
    /// Token flags

    CK_FLAGS get_Flags() {
        return FFlags;
    }
    /// Free private memory

    unsigned long get_FreePrivateMemory() {
        return FFreePrivateMemory;
    }
    /// Free public memory

    unsigned long get_FreePublicMemory() {
        return FFreePublicMemory;
    }
    /// Token hardware version

    std::string get_HardwareVersion() {
        return FHardwareVersion;
    }
    /// Maximum number of concurrent read/write sessions

    unsigned long get_MaxRwSessionCount() {
        return FMaxRwSessionCount;
    }
    /// Maximum number of open sessions

    unsigned long get_MaxSessionCount() {
        return FMaxSessionCount;
    }
    /// PKCS11 mechanisms supported

    CP11Mechanism get_Mechanism(int index) {
        return GetMechanism(index);
    }
    /// Number of available mechanisms

    int get_MechanismCount() {
        return GetMechanismCount();
    }
    /// Currently open R/W sessions

    unsigned long get_RwSessionCount() {
        return FRwSessionCount;
    }
    /// Currently open sessions

    unsigned long get_SessionCount() {
        return FSessionCount;
    }
    /// The SessionInfo object associated with this token

    CP11SessionInfo& get_SessionInfo() {
        return GetSessionInfo();
    }
    /// Token Label 'Belpic (Basic PIN)'

    std::string get_TokenLabel() {
        return FTokenLabel;
    }
    /// Token Model

    std::string get_TokenModel() {
        return FTokenModel;
    }
    /// Serial Number (HEX)

    std::string get_TokenSerial() {
        return FTokenSerial;
    }
    /// Total private memory available

    unsigned long get_TotalPrivateMemory() {
        return FTotalPrivateMemory;
    }
    /// Total public memory available

    unsigned long get_TotalPublicMemory() {
        return FTotalPublicMemory;
    }
    /// UTC Time

    std::string get_UTCTime() {
        return FUTCTime;
    }

};

/// A PKCS11 object (certificate, key ...)

class CP11Object {
private:
    CK_OBJECT_HANDLE FhObject;
    CK_SESSION_HANDLE FhSession;
    unsigned long FObjClass;
    std::string FObjClassString;
    std::string FObjLabel;
    int FObjSize;
    Byte_Array FObjValue;
    CK_FUNCTION_LIST_PTR FP11;
    /// Return PKCS11 object class string representation
    std::string GetObjectClassString(CK_OBJECT_CLASS objClass);
    /// Return PKCS11 Object Class Attribute
    unsigned long GetObjectClass(CK_OBJECT_HANDLE hObject, unsigned long hSession);
    CK_OBJECT_HANDLE GetObjectHandle();
    /// Return PKCS11 Object Label Attribute
    std::string GetObjectLabel(CK_OBJECT_HANDLE hObject, unsigned long hSession);
    /// Return PKCS11 Object Value Attribute
    Byte_Array GetObjectValue(CK_OBJECT_HANDLE hObject, unsigned long hSession);
    /// Return a PKCS11 Object Attribute
    void GetObjAttribute(CK_OBJECT_HANDLE hObject, unsigned long hSession,
            CK_ATTRIBUTE & Template);
protected:
public:
    CP11Object(CK_FUNCTION_LIST_PTR P11FunctionList, CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject);
    virtual ~CP11Object(void);

    //* properties*//

    /// Object Class

    unsigned long get_objClass() {
        return FObjClass;
    }
    /// String representation of object class

    std::string get_ObjClassString() {
        return FObjClassString;
    }
    /// PKCS11 Object Handle

    CK_OBJECT_HANDLE get_ObjectHandle() {
        return GetObjectHandle();
    }
    /// Object Label

    std::string get_ObjLabel() {
        return FObjLabel;
    }
    /// Size of the object if known, or -1

    int get_ObjSize() {
        return FObjSize;
    }
    /// The actual value of the object

    Byte_Array get_ObjValue() {
        return FObjValue;
    }
};

/// A session with a PKCS11 token

class CP11SessionInfo {
private:
    unsigned long FhSession;
    bool FLoggedIn;
    CP11ObjectList FObjects;
    CK_FUNCTION_LIST_PTR FP11;
    CP11TokenInfo& FTokenInfo;
    int GetObjectCount();
    CP11Object& GetObjects(UINT Index);
    CK_SESSION_HANDLE GetSessionHandle();
protected:
public:
    CP11SessionInfo(CP11TokenInfo& Token, CK_FUNCTION_LIST_PTR P11FunctionList, CK_SLOT_ID SlotId, const std::string& PinCode = "");
    virtual ~CP11SessionInfo(void);
    /// Compute a message digest on a byte string
    Byte_Array Digest(CK_MECHANISM_TYPE Algorithm, const std::string& DataMessage, int *length);
    /// Login to the token
    bool Login(CK_USER_TYPE Mode, const std::string& PinCode);
    /// Logout from the token
    bool Logout();
    /// Get a pkcs11 digital signature
    Byte_Array Sign(CP11Object KeyObject, CK_MECHANISM_TYPE Algorithm, const std::string& DataMessage, int *length);

    //* properties*//

    /// Logged In status

    bool get_LoggedIn() {
        return FLoggedIn;
    }
    /// Number of Objects on the token

    int get_ObjectCount() {
        return GetObjectCount();
    }
    /// Return a pkcs11 object from this token

    CP11Object& get_Objects(int index) {
        return GetObjects(index);
    }
    /// The PKCS11 session handle

    CK_SESSION_HANDLE get_SessionHandle() {
        return GetSessionHandle();
    }
    /// The owning Token

    CP11TokenInfo& get_TokenInfo() {
        return FTokenInfo;
    }
};




#endif//INCLUDED_PKCS11OBJECTS_H
//END
