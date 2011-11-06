#include "jpcsc.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static jfieldID CtxField;
static jfieldID CardField;
static jfieldID ProtoField;

static jfieldID ReaderStateField;
static jfieldID CurrentStateField;
static jfieldID EventStateField;
static jfieldID ProtoStateField;
static jfieldID T0GetResponseField;
static jfieldID AtrStateField;
static jfieldID ResendOnWrongLeField;


/* Signal exception, nullpointer or pcsc. */
static void pcscex_throw(JNIEnv *env, const char *msg, int reason);
static void npex_throw(JNIEnv *env, const char *msg);

#ifdef DEBUG
/* Log reader state. */
static void pcsc_readerstatea_log(SCARD_READERSTATE_A *rsa);
static void jpcsc_log(const char *fmt, ...);
#define JPCSC_LOG(x) jpcsc_log x;
#else /* DEBUG */
#define JPCSC_LOG(x) ;
#define pcsc_readerstatea_log(x) ;
#endif /* DEBUG */

/** Windows offers SCARD_CTL_CODE */
#ifndef WIN32
#define SCARD_CTL_CODE(x) x
#endif /* WIN32 */


static jobjectArray pcsc_list_readers(JNIEnv *env, jobject _this, const char *groupSpec);
static jobjectArray pcsc_multi2jstring(JNIEnv *env, char *spec, const char *msg);


/*
 * Default receive buffer size.
 */
#define RECEIVE_BUFFER_SIZE 260



/**
 * Table to stringify error codes. 
 */
struct _pcsc_code{ int code; char *msg; } _pcsc_codes[] = {
 { SCARD_E_CANCELLED, "SCARD_E_CANCELLED", },
 { SCARD_E_CANT_DISPOSE, "SCARD_E_CANT_DISPOSE", },
 { SCARD_E_INSUFFICIENT_BUFFER, "SCARD_E_INSUFFICIENT_BUFFER", },
 { SCARD_E_INVALID_ATR, "SCARD_E_INVALID_ATR", },
 { SCARD_E_INVALID_HANDLE, "SCARD_E_INVALID_HANDLE", },
 { SCARD_E_INVALID_PARAMETER, "SCARD_E_INVALID_PARAMETER", },
 { SCARD_E_INVALID_TARGET, "SCARD_E_INVALID_TARGET", },
 { SCARD_E_INVALID_VALUE, "SCARD_E_INVALID_VALUE", },
 { SCARD_E_NO_MEMORY, "SCARD_E_NO_MEMORY", },
 { SCARD_F_COMM_ERROR, "SCARD_F_COMM_ERROR", },
 { SCARD_F_INTERNAL_ERROR, "SCARD_F_INTERNAL_ERROR", },
 { SCARD_F_UNKNOWN_ERROR, "SCARD_F_UNKNOWN_ERROR", },
 { SCARD_F_WAITED_TOO_LONG, "SCARD_F_WAITED_TOO_LONG", },
 { SCARD_E_UNKNOWN_READER, "SCARD_E_UNKNOWN_READER", },
 { SCARD_E_TIMEOUT, "SCARD_E_TIMEOUT", },
 { SCARD_E_SHARING_VIOLATION, "SCARD_E_SHARING_VIOLATION", },
 { SCARD_E_NO_SMARTCARD, "SCARD_E_NO_SMARTCARD", },
 { SCARD_E_UNKNOWN_CARD, "SCARD_E_UNKNOWN_CARD", },
 { SCARD_E_PROTO_MISMATCH, "SCARD_E_PROTO_MISMATCH", },
 { SCARD_E_NOT_READY, "SCARD_E_NOT_READY", },
 { SCARD_E_SYSTEM_CANCELLED, "SCARD_E_SYSTEM_CANCELLED", },
 { SCARD_E_NOT_TRANSACTED, "SCARD_E_NOT_TRANSACTED", },
 { SCARD_E_READER_UNAVAILABLE, "SCARD_E_READER_UNAVAILABLE", },
    
 { SCARD_W_UNSUPPORTED_CARD, "SCARD_W_UNSUPPORTED_CARD", },
 { SCARD_W_UNRESPONSIVE_CARD, "SCARD_W_UNRESPONSIVE_CARD", },
 { SCARD_W_UNPOWERED_CARD, "SCARD_W_UNPOWERED_CARD", },
 { SCARD_W_RESET_CARD, "SCARD_W_RESET_CARD", },
 { SCARD_W_REMOVED_CARD, "SCARD_W_REMOVED_CARD", },

 { SCARD_E_PCI_TOO_SMALL, "SCARD_E_PCI_TOO_SMALL", },
 { SCARD_E_READER_UNSUPPORTED, "SCARD_E_READER_UNSUPPORTED", },
 { SCARD_E_DUPLICATE_READER, "SCARD_E_DUPLICATE_READER", },
 { SCARD_E_CARD_UNSUPPORTED, "SCARD_E_CARD_UNSUPPORTED", },
 { SCARD_E_NO_SERVICE, "SCARD_E_NO_SERVICE", },
 { SCARD_E_SERVICE_STOPPED, "SCARD_E_SERVICE_STOPPED", },
};



/*
 * Signal a jpcsc exception.
 */
static void pcscex_throw(JNIEnv *env, const char *msg, int reason){
    jstring jmsg = (*env)->NewStringUTF(env, msg);
    jobject exobj;
    jmethodID exm;
    jclass excl;
    int ret;

    excl = (*env)->FindClass(env, PCSC_EX_CLASSNAME); 
    if (excl == NULL){
	(*env)->ExceptionDescribe(env);
	(*env)->ExceptionClear(env);
	assert(0);
    }

    exm = (*env)->GetMethodID(env, excl, "<init>", "(Ljava/lang/String;I)V");
    assert(exm != NULL);

    exobj = (*env)->NewObject(env, excl, exm, jmsg, reason);
    if (exobj == NULL){
	(*env)->ExceptionDescribe(env);
	(*env)->ExceptionClear(env);
	assert(0);
    }

    ret = (*env)->Throw(env, exobj);
    assert(ret == 0);
}


/*
 * Signal a nullpointer exception.
 */
static void npex_throw(JNIEnv *env, const char *msg){
    jclass npcl;
    int ret;
    npcl = (*env)->FindClass(env, NP_EX_CLASSNAME);
    assert(npcl != NULL);
    ret = (*env)->ThrowNew(env, npcl, msg);
    assert(ret == 0);
}



/*
 * Return platform-dependent pcsc codes, and initiale variables used in the native code.
 */
JNIEXPORT jintArray JNICALL GEN_FUNCNAME(PCSC_NativeInitialize)(JNIEnv *env, jclass _this, jclass contextClass, jclass cardClass, jclass stateClass){
    jintArray jcodes;
    jboolean isCopy;
    jint *codes;

    // initialize codes
    jcodes = (*env)->NewIntArray(env, 113);
    if (jcodes == NULL){
	pcscex_throw(env, "Fatal Error: can't initialize PCSC codes", 0);
	return NULL;
    }
    codes = (*env)->GetIntArrayElements(env, jcodes, &isCopy);
    if (codes == NULL){
	pcscex_throw(env, "Fatal Error: can't initialize PCSC codes", 0);
	return NULL;
    }

    codes[0] = SCARD_S_SUCCESS;

    codes[1] = SCARD_SCOPE_USER;
    codes[2] = SCARD_SCOPE_TERMINAL;
    codes[3] = SCARD_SCOPE_SYSTEM;
#ifdef SCARD_SCOPE_GLOBAL
    codes[4] = SCARD_SCOPE_GLOBAL;
#else
    codes[4] = SCARD_SCOPE_SYSTEM;
#endif

    codes[5] = SCARD_PROTOCOL_T0;
    codes[6] = SCARD_PROTOCOL_T1;
    codes[7] = SCARD_PROTOCOL_RAW;
#ifdef SCARD_PROTOCOL_ANY
    codes[8] = SCARD_PROTOCOL_ANY;
#else
    codes[8] = SCARD_PROTOCOL_DEFAULT;
#endif

    codes[9] = SCARD_SHARE_EXCLUSIVE;
    codes[10] = SCARD_SHARE_SHARED;
    codes[11] = SCARD_SHARE_DIRECT;

    codes[12] = SCARD_LEAVE_CARD;
    codes[13] = SCARD_RESET_CARD;
    codes[14] = SCARD_UNPOWER_CARD;
    codes[15] = SCARD_EJECT_CARD;

    codes[16] = SCARD_UNKNOWN;
    codes[17] = SCARD_ABSENT;
    codes[18] = SCARD_PRESENT;
    codes[19] = SCARD_SWALLOWED;
    codes[20] = SCARD_POWERED;
    codes[21] = SCARD_NEGOTIABLE;
    codes[22] = SCARD_SPECIFIC;

    codes[23] = SCARD_STATE_UNAWARE;
    codes[24] = SCARD_STATE_IGNORE;
    codes[25] = SCARD_STATE_CHANGED;
    codes[26] = SCARD_STATE_UNKNOWN;
    codes[27] = SCARD_STATE_UNAVAILABLE;
    codes[28] = SCARD_STATE_EMPTY;
    codes[29] = SCARD_STATE_PRESENT;
    codes[30] = SCARD_STATE_ATRMATCH;
    codes[31] = SCARD_STATE_EXCLUSIVE;
    codes[32] = SCARD_STATE_INUSE;
    codes[33] = SCARD_STATE_MUTE;

    codes[34] =  SCARD_E_CANCELLED;
    codes[35] =  SCARD_E_CANT_DISPOSE;
    codes[36] =  SCARD_E_INSUFFICIENT_BUFFER;
    codes[37] =  SCARD_E_INVALID_ATR;
    codes[38] =  SCARD_E_INVALID_HANDLE;
    codes[39] =  SCARD_E_INVALID_PARAMETER;
    codes[40] =  SCARD_E_INVALID_TARGET;
    codes[41] =  SCARD_E_INVALID_VALUE;
    codes[42] =  SCARD_E_NO_MEMORY;
    codes[43] =  SCARD_F_COMM_ERROR;
    codes[44] =  SCARD_F_INTERNAL_ERROR;
    codes[45] =  SCARD_F_UNKNOWN_ERROR;
    codes[46] =  SCARD_F_WAITED_TOO_LONG;
    codes[47] =  SCARD_E_UNKNOWN_READER;
    codes[48] =  SCARD_E_TIMEOUT;
    codes[49] =  SCARD_E_SHARING_VIOLATION;
    codes[50] =  SCARD_E_NO_SMARTCARD;
    codes[51] =  SCARD_E_UNKNOWN_CARD;
    codes[52] =  SCARD_E_PROTO_MISMATCH;
    codes[53] =  SCARD_E_NOT_READY;
    codes[54] =  SCARD_E_SYSTEM_CANCELLED;
    codes[55] =  SCARD_E_NOT_TRANSACTED;
    codes[56] =  SCARD_E_READER_UNAVAILABLE;
    
    codes[57] =  SCARD_W_UNSUPPORTED_CARD;
    codes[58] =  SCARD_W_UNRESPONSIVE_CARD;
    codes[59] =  SCARD_W_UNPOWERED_CARD;
    codes[60] =  SCARD_W_RESET_CARD;
    codes[61] =  SCARD_W_REMOVED_CARD;

    codes[62] =  SCARD_E_PCI_TOO_SMALL;
    codes[63] =  SCARD_E_READER_UNSUPPORTED;
    codes[64] =  SCARD_E_DUPLICATE_READER;
    codes[65] =  SCARD_E_CARD_UNSUPPORTED;
    codes[66] =  SCARD_E_NO_SERVICE;
    codes[67] =  SCARD_E_SERVICE_STOPPED;
    codes[68] =  INFINITE;

#if defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES)
   codes[69] =  SCARD_ATTR_VENDOR_NAME;
    codes[70] =  SCARD_ATTR_VENDOR_IFD_TYPE;
    codes[71] =  SCARD_ATTR_VENDOR_IFD_VERSION;
    codes[72] =  SCARD_ATTR_VENDOR_IFD_SERIAL_NO;
    codes[73] =  SCARD_ATTR_CHANNEL_ID;
    codes[74] =  SCARD_ATTR_ASYNC_PROTOCOL_TYPES;
    codes[75] =  SCARD_ATTR_DEFAULT_CLK;
    codes[76] =  SCARD_ATTR_MAX_CLK;
    codes[77] =  SCARD_ATTR_DEFAULT_DATA_RATE;
    codes[78] =  SCARD_ATTR_MAX_DATA_RATE;
    codes[79] =  SCARD_ATTR_MAX_IFSD;
    codes[80] =  SCARD_ATTR_SYNC_PROTOCOL_TYPES;
    codes[81] =  SCARD_ATTR_POWER_MGMT_SUPPORT;
    codes[82] =  SCARD_ATTR_USER_TO_CARD_AUTH_DEVICE;
    codes[83] =  SCARD_ATTR_USER_AUTH_INPUT_DEVICE;
    codes[84] =  SCARD_ATTR_CHARACTERISTICS;

    codes[85] =  SCARD_ATTR_CURRENT_PROTOCOL_TYPE;
    codes[86] =  SCARD_ATTR_CURRENT_CLK;
    codes[87] =  SCARD_ATTR_CURRENT_F;
    codes[88] =  SCARD_ATTR_CURRENT_D;
    codes[89] =  SCARD_ATTR_CURRENT_N;
    codes[90] =  SCARD_ATTR_CURRENT_W;
    codes[91] =  SCARD_ATTR_CURRENT_IFSC;
    codes[92] =  SCARD_ATTR_CURRENT_IFSD;
    codes[93] =  SCARD_ATTR_CURRENT_BWT;
    codes[94] =  SCARD_ATTR_CURRENT_CWT;
    codes[95] =  SCARD_ATTR_CURRENT_EBC_ENCODING;
    codes[96] =  SCARD_ATTR_EXTENDED_BWT;

    codes[97] =  SCARD_ATTR_ICC_PRESENCE;
    codes[98] =  SCARD_ATTR_ICC_INTERFACE_STATUS;
    codes[99] =  SCARD_ATTR_CURRENT_IO_STATE;
    codes[100] =  SCARD_ATTR_ATR_STRING;
    codes[101] =  SCARD_ATTR_ICC_TYPE_PER_ATR;

    codes[102] =  SCARD_ATTR_ESC_RESET;
    codes[103] =  SCARD_ATTR_ESC_CANCEL;
    codes[104] =  SCARD_ATTR_ESC_AUTHREQUEST;
    codes[105] =  SCARD_ATTR_MAXINPUT;

    codes[106] =  SCARD_ATTR_DEVICE_UNIT;
    codes[107] =  SCARD_ATTR_DEVICE_IN_USE;
    codes[108] =  SCARD_ATTR_DEVICE_FRIENDLY_NAME_A;
    codes[109] =  SCARD_ATTR_DEVICE_SYSTEM_NAME_A;
    codes[110] =  SCARD_ATTR_DEVICE_FRIENDLY_NAME_W;
    codes[111] =  SCARD_ATTR_DEVICE_SYSTEM_NAME_W;
    codes[112] =  SCARD_ATTR_SUPRESS_T1_IFS_REQUEST;
#endif /* defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES) */

    (*env)->ReleaseIntArrayElements(env, jcodes, codes, 0);


    // get fields
    CtxField = (*env)->GetFieldID(env, contextClass, "ctx", "J");
    assert(CtxField != NULL);
    CardField = (*env)->GetFieldID(env, cardClass, "card", "J");
    assert(CardField != NULL);
    ProtoField = (*env)->GetFieldID(env, cardClass, "proto", "I");
    assert(ProtoField != NULL);
    T0GetResponseField = (*env)->GetFieldID(env, cardClass, "t0getResponse", "Z");
    assert(T0GetResponseField != NULL);

    ReaderStateField = (*env)->GetFieldID(env, stateClass, "szReader", "Ljava/lang/String;");
    assert(ReaderStateField != NULL);
    CurrentStateField = (*env)->GetFieldID(env, stateClass, "dwCurrentState", "I");
    assert(CurrentStateField != NULL);
    EventStateField = (*env)->GetFieldID(env, stateClass, "dwEventState", "I");
    assert(EventStateField != NULL);
    AtrStateField = (*env)->GetFieldID(env, stateClass, "rgbAtr", "[B");
    assert(AtrStateField != NULL);
    ProtoStateField = (*env)->GetFieldID(env, stateClass, "proto", "I");
    assert(ProtoStateField != NULL);
    ResendOnWrongLeField = (*env)->GetFieldID(env, cardClass, "resendOnWrongLe", "Z");
    assert(ResendOnWrongLeField != NULL);

    return jcodes;
}

/*
 * Establish context.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Context_NativeEstablishContext)
  (JNIEnv *env, jobject _this, jint dwScope, jstring pvReserved1, jstring pvReserved2)
{
    int rv;
    SCARDCONTEXT ctx;
    char const *pv1, *pv2;
    jboolean b;

    pv1 = (pvReserved1 == NULL) ? NULL : (*env)->GetStringUTFChars(env, pvReserved1, &b);
    pv2 = (pvReserved2 == NULL) ? NULL : (*env)->GetStringUTFChars(env, pvReserved2, &b);
    
    rv = SCardEstablishContext(dwScope, pv1, pv2, &ctx);
  
    if (pv1 != NULL){
	(*env)->ReleaseStringUTFChars(env, pvReserved1, pv1);
    }
    if (pv2 != NULL){
	(*env)->ReleaseStringUTFChars(env, pvReserved2, pv2);
    }
    
    if (rv == SCARD_S_SUCCESS){
	(*env)->SetLongField(env, _this, CtxField, (jlong) ctx);
    }

    return rv;
}

/*
 * Release context.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Context_NativeReleaseContext)
  (JNIEnv *env, jobject _this)
{
    SCARDCONTEXT ctx;

    ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);
    
    return SCardReleaseContext(ctx);
}



/**
 * Convert a multi string to a java string array,
 */
jobjectArray pcsc_multi2jstring(JNIEnv *env, char *spec, const char *msg){
    jobjectArray result;
    jclass stringClass;
    char *cp, **tab;
    jstring js;
    int cnt = 0;

    cp = spec;
    while(*cp != 0){
	cp += (strlen(cp) + 1);
	++cnt;
    }
    
    tab = (char **)malloc(cnt * sizeof(char *));

    cnt = 0;
    cp = spec;
    while(*cp != 0){
	tab[cnt++] = cp;
	cp += (strlen(cp) + 1);
    }

    stringClass = (*env)->FindClass(env, "java/lang/String");
    assert(stringClass != NULL);

    result = (*env)->NewObjectArray(env, cnt, stringClass, NULL);
    while(cnt-- > 0){
	js = (*env)->NewStringUTF(env, tab[cnt]);
	(*env)->SetObjectArrayElement(env, result, cnt, js);
    }
    free(tab);
    return result;
}


/**
 * Return reader list. Given group spec may be null.
 */
jobjectArray pcsc_list_readers(JNIEnv *env, jobject _this, const char *groupSpec){
    jobjectArray result;
    SCARDCONTEXT ctx;
    char *mszReaders;
    LONG rv;
    DWORD sz;

    ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);
    rv = SCardListReaders(ctx, groupSpec, NULL, &sz);
    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "SCardListReaders", rv);
	return NULL;
    }

    mszReaders = (char *)malloc(sz);
    assert(mszReaders);

    rv = SCardListReaders(ctx, groupSpec, mszReaders, &sz);
    if (rv != SCARD_S_SUCCESS){
	free(mszReaders);
	pcscex_throw(env, "SCardListReaders()", rv);
	return NULL;
    }

    result = pcsc_multi2jstring(env, mszReaders, "SCardListReaders(): too many physical readers");
    free(mszReaders);
    return result;
}

/*
 * List all readers.
 */
JNIEXPORT jobjectArray JNICALL GEN_FUNCNAME(Context_NativeListReaders)
  (JNIEnv *env, jobject _this)
{
    return pcsc_list_readers(env, _this, NULL);
}


/*
 * List all readers in specified group.
 */
JNIEXPORT jobjectArray JNICALL GEN_FUNCNAME(Context_NativeListReaders__Ljava_lang_String_2)
  (JNIEnv *env, jobject _this, jstring jgroupName)
{
    char *multiStr;
    const char *cp;
    jsize cnt;
    jboolean isCopy;
    jobjectArray result;

    cnt = (*env)->GetStringUTFLength(env, jgroupName);
    multiStr = (char *)malloc(cnt + 2);
    assert(multiStr != NULL);

    cp = (*env)->GetStringUTFChars(env, jgroupName, &isCopy);
    memcpy(multiStr, cp, cnt);
    multiStr[cnt] = 0;
    multiStr[cnt + 1] = 0;
    (*env)->ReleaseStringUTFChars(env, jgroupName, cp);
    
    result = pcsc_list_readers(env, _this, multiStr);

    free(multiStr);
    return result;
}


/*
 * List all readers in specified groups.
 */
JNIEXPORT jobjectArray JNICALL GEN_FUNCNAME(Context_NativeListReaders___3Ljava_lang_String_2)
(JNIEnv *env, jobject _this, jobjectArray jgroupa){
    jobjectArray result;
    char *multiStr;
    int i, pos, cnt;
    jstring js;
    jboolean isCopy;
    const char *cp;

    cnt = 0;
    for (i = 0; i < (*env)->GetArrayLength(env, jgroupa); i++){
	cnt += (1 + (*env)->GetStringUTFLength(env, (*env)->GetObjectArrayElement(env, jgroupa, i)));
    }
    multiStr = (char *)malloc(cnt + 1);
    assert(multiStr != NULL);
    pos = 0;
    for (i = 0; i < (*env)->GetArrayLength(env, jgroupa); i++){
	js = (*env)->GetObjectArrayElement(env, jgroupa, i);
	cnt = (*env)->GetStringUTFLength(env, js);
	cp = (*env)->GetStringUTFChars(env, js, &isCopy);
	memcpy(multiStr + pos, cp, cnt);
	pos += cnt;
	multiStr[pos++] = 0;
	(*env)->ReleaseStringUTFChars(env, js, cp);
    }
    multiStr[pos] = 0;
    
    result = pcsc_list_readers(env, _this, multiStr);

    free(multiStr);
    return result;
}




/*
 * List all groups known by the system.
 */
JNIEXPORT jobjectArray JNICALL GEN_FUNCNAME(Context_NativeListReaderGroups)
  (JNIEnv *env, jobject _this)
{
    jobjectArray result;
    SCARDCONTEXT ctx;
    char *mszGroups;
    LONG rv;
    DWORD sz;

    ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);
    rv = SCardListReaderGroups(ctx, NULL, &sz);
    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "SCardListGroups", rv);
	return NULL;
    }

    mszGroups = (char *)malloc(sz);
    assert(mszGroups);

    rv = SCardListReaderGroups(ctx, mszGroups, &sz);

    if (rv != SCARD_S_SUCCESS){
	free(mszGroups);
	pcscex_throw(env, "SCardListGroups()", rv);
	return NULL;
    }

    result = pcsc_multi2jstring(env, mszGroups, "SCardListReaderGroups(): too many reader groups");
    free(mszGroups);
    return result;
}


/*
 * Return reader status change.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Context_NativeGetStatusChange)
  (JNIEnv *env, jobject _this, jint timeout, jobjectArray jrstates)
{
    SCARD_READERSTATE *crstates;
    SCARDCONTEXT ctx;
    jobject jrdrName;
    jobject jrstate;
    jboolean isCopy;
    jbyteArray jatr;
    DWORD rv;
    int crcnt;
    char *catr;
    int i;

    ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);

    if (jrstates == NULL){
	rv = SCardGetStatusChange(ctx, INFINITE, NULL, 0);
	return rv;
    }else{
	crcnt = (*env)->GetArrayLength(env, jrstates);
	crstates = (SCARD_READERSTATE *)malloc(crcnt * sizeof(SCARD_READERSTATE));
	if (crstates == NULL){
	    pcscex_throw(env, "GetStatusChange(): invalid number of reader-states", 0);
	    return SCARD_E_INVALID_PARAMETER;
	}

	for (i = 0; i < crcnt; i++){
	    jrstate = (*env)->GetObjectArrayElement(env, jrstates, i);
	    if (jrstate == NULL){
		free(crstates);
		npex_throw(env, "GetStatusChange(): invalid null entry in ReaderState array parameter");
		return SCARD_E_INVALID_PARAMETER;
	    }
	    
	    crstates[i].dwCurrentState = (*env)->GetIntField(env, jrstate, CurrentStateField);
	    crstates[i].dwEventState = (*env)->GetIntField(env, jrstate, EventStateField);
	    
	    jrdrName = (*env)->GetObjectField(env, jrstate, ReaderStateField);
	    if (jrdrName == NULL){
		free(crstates);
		npex_throw(env, "GetStatusChange(): invalid null reader name entry in one ReaderState parameter");
		return SCARD_E_INVALID_PARAMETER;
	    }
	    
	    crstates[i].szReader = (*env)->GetStringUTFChars(env, jrdrName, &isCopy);

	    // copy ATR and check its size.
	    jatr = (jbyteArray) (*env)->GetObjectField(env, jrstate, AtrStateField);
	    if (jatr != NULL){
		catr = (*env)->GetByteArrayElements(env, jatr, &isCopy);
		crstates[i].cbAtr = (*env)->GetArrayLength(env, jatr);
		if (crstates[i].cbAtr < 0){
		    free(crstates);
		    npex_throw(env, "GetStatusChange(): invalid zero-length ATR in ReaderState parameter");
		    return SCARD_E_INVALID_PARAMETER;
		}
		if (crstates[i].cbAtr > JPCSC_ATR_SIZE){
		    free(crstates);
		    npex_throw(env, "GetStatusChange(): nvalid ATR length in ReaderState parameter");
		    return SCARD_E_INVALID_PARAMETER;
		}
		memcpy(&crstates[i].rgbAtr[0], catr, crstates[i].cbAtr);
		(*env)->ReleaseByteArrayElements(env, jatr, catr, JNI_ABORT);
	    }else{
		crstates[i].cbAtr = 0;
	    }
	}
	
	rv = SCardGetStatusChange(ctx, timeout, &crstates[0], crcnt);
	if (rv != SCARD_S_SUCCESS){
	    free(crstates);
	    return rv;
	}
	
	JPCSC_LOG(("NativeGetStatucChange(): readerstate returned ...\n"));
	pcsc_readerstatea_log(&crstates[0]);
	
	for (i = 0; i < crcnt; i++){
	    jrstate = (*env)->GetObjectArrayElement(env, jrstates, i);
	    (*env)->SetIntField(env, jrstate, CurrentStateField, crstates[i].dwCurrentState);
	    (*env)->SetIntField(env, jrstate, EventStateField, crstates[i].dwEventState);
	    jrdrName = (*env)->GetObjectField(env, jrstate, ReaderStateField);
	    (*env)->ReleaseStringUTFChars(env, jrdrName, crstates[i].szReader);
	    
	    jatr = (*env)->NewByteArray(env, crstates[i].cbAtr);
	    (*env)->SetByteArrayRegion(env, jatr, 0, crstates[i].cbAtr, crstates[i].rgbAtr);
	    (*env)->SetObjectField(env, jrstate, AtrStateField, jatr);
	}
	free(crstates);
	return rv;
    }
}

/*
 * Connect to card.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Context_NativeConnect)
  (JNIEnv *env, jobject _this, jobject _card, jstring jrdrName, jint dwSharedMode, jint dwPreferredProtocols)
{
    SCARDCONTEXT ctx;
    SCARDHANDLE card;
    const char *crdrName;
    jboolean isCopy;
    uint32_t proto;
    LONG rv;

    crdrName = (*env)->GetStringUTFChars(env, jrdrName, &isCopy);

    ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);

    JPCSC_LOG(("NativeConnect(): reader %s, mode %d, proto %d\n", crdrName, dwSharedMode, dwPreferredProtocols));
    
    rv = SCardConnect(ctx, crdrName, dwSharedMode, dwPreferredProtocols, &card, &proto);

    (*env)->ReleaseStringUTFChars(env, jrdrName, crdrName);

    if (rv != SCARD_S_SUCCESS){
	return rv;
    }

    JPCSC_LOG(("NativeConnect(): card 0x%x, proto 0x%x\n", (int) card, (int) proto));

    (*env)->SetLongField(env, _card, CardField, card);
    (*env)->SetIntField(env, _card, ProtoField, proto);

    return rv;
}


/*
 * Cancel card transaction.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Context_NativeCancel)
  (JNIEnv *env, jobject _this)
{
    SCARDCONTEXT ctx = (SCARDCONTEXT) (*env)->GetLongField(env, _this, CtxField);
    return SCardCancel(ctx);
}



/*
 * Return string represenation of given error.
 */
JNIEXPORT jstring JNICALL GEN_FUNCNAME(Context_StringifyError)
  (JNIEnv *env, jclass _this, jint err)
{
    jstring emsg;
    char *errstr = NULL;

#ifdef WIN32
    char* e_generr="General error.";
    char* lpstr=NULL;
 
    HMODULE h = LoadLibrary ("winscard.dll");
    
    if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE, h, err, MAKELANGID (LANG_ENGLISH,SUBLANG_ENGLISH_US), (LPSTR) &lpstr, 0, NULL )) {
#ifdef DEBUG
	fprintf(stderr, "StringifyError(): FormatMessageA returns 0x%x \n", GetLastError());
#endif /* DEBUG */
	errstr = e_generr ;
    }else{
	errstr = lpstr;
    }
    emsg = (*env)->NewStringUTF(env, errstr);
    if (lpstr)	
	LocalFree ( lpstr );
    if (h)		
	FreeLibrary (h);
#else
    errstr = pcsc_stringify_error((LONG) err);
    emsg = (*env)->NewStringUTF(env, errstr);
#endif /* WIN32 */

    return emsg;
}





JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeDisconnect)
  (JNIEnv *env, jobject _this, jint param)
{
    SCARDHANDLE card;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    return SCardDisconnect(card, param);
}



static LONG t0_get_response(SCARDHANDLE card, int proto, char *cout, int coff, int csz, DWORD *outlen){
  SCARD_IO_REQUEST sendPci;
  char msg[5]; 
  LONG rv;

  msg[0] = msg[2] = msg[3] = 0; 
  msg[1] = (char)0xC0; 
  msg[4] = cout[coff + 1];

  JPCSC_LOG(("t0_get_response(): response 0x%x 0x%x\n", cout[coff] & 0xff, cout[coff + 1] & 0xff));

  sendPci.dwProtocol = proto;
  sendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

  *outlen = csz;
  rv =  SCardTransmit(card, &sendPci, msg, 5, NULL, cout + coff, outlen);

  JPCSC_LOG(("t0_get_response(): return code 0x%x, return length %d\n", (int) rv, (int) *outlen));
  if (*outlen >= 2) 
      JPCSC_LOG(("t0_get_response(): status 0x%x 0x%x\n",  cout[coff + *outlen - 2] & 0xff, cout[coff + *outlen - 1] & 0xff));

  return rv;
    
}

/*
 * Transmit.
 */
JNIEXPORT jbyteArray JNICALL GEN_FUNCNAME(Card_NativeTransmit___3BII)
  (JNIEnv *env, jobject _this, jbyteArray jin, jint joff, jint jlen)
{
    SCARD_IO_REQUEST sendPci;
    SCARDHANDLE card;
    char *cin;
    jboolean isCopy;
    LONG rv;
    int proto;
    char cout[RECEIVE_BUFFER_SIZE];
    DWORD clen = RECEIVE_BUFFER_SIZE;
    jbyteArray jout;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);
    proto = (int) (*env)->GetIntField(env, _this, ProtoField);

    sendPci.dwProtocol = proto;
    sendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

    cin = (*env)->GetByteArrayElements(env, jin, &isCopy);

    if (proto == SCARD_PROTOCOL_T0){
	if (5 + (cin[joff + 4] & 0xff) < jlen)   
	    jlen = 5 + (cin[joff + 4] & 0xff);
    }

    JPCSC_LOG(("NativeTransmit1(): proto %d, inlen %d\n", proto, jlen));

    rv =  SCardTransmit(card, &sendPci, cin + joff, jlen, NULL, cout, &clen);

    JPCSC_LOG(("NativeTransmit1(): return code 0x%x, return length %d\n", (int) rv, (int) clen));
    
    if ((proto == SCARD_PROTOCOL_T0) && (clen == 2)){
	if (cout[0] == 0x6c && ((*env)->GetBooleanField(env, _this, ResendOnWrongLeField) != 0)){
	    clen = RECEIVE_BUFFER_SIZE;
	    cin[joff + 4] = cout[1];
	    rv = SCardTransmit(card, &sendPci, cin + joff, jlen, NULL, cout, &clen);
	}else{
	    if ((cout[0] == 0x61) && ((*env)->GetBooleanField(env, _this, T0GetResponseField) != 0)){
		rv = t0_get_response(card, proto, cout, 0, RECEIVE_BUFFER_SIZE, &clen);
	    }
	}
    }
    
    (*env)->ReleaseByteArrayElements(env, jin, cin, JNI_ABORT);
    
    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "SCardTransmit()", rv);
	return NULL;
    }
    
    jout = (*env)->NewByteArray(env, clen);
    (*env)->SetByteArrayRegion(env, jout, 0, clen, cout);
    
    return jout;
}


/*
 * Transmit.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeTransmit___3BII_3BI)
  (JNIEnv *env, jobject _this, jbyteArray jin, jint inoff, jint inlen, jbyteArray jout, jint outoff)
{
    SCARD_IO_REQUEST sendPci;
    SCARDHANDLE card;
    char *cin;
    char *cout;
    jboolean isCopy;
    LONG rv;
    int proto;
    DWORD outlen;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);
    proto = (int) (*env)->GetIntField(env, _this, ProtoField);

    sendPci.dwProtocol = proto;
    sendPci.cbPciLength = sizeof(SCARD_IO_REQUEST);

    cin = (*env)->GetByteArrayElements(env, jin, &isCopy);
    cout = (*env)->GetByteArrayElements(env, jout, &isCopy);
    outlen = (*env)->GetArrayLength(env, jout) - outoff;
    
    if (proto == SCARD_PROTOCOL_T0){
	if (5 + (cin[inoff + 4] & 0xff) < inlen)
	    inlen = 5 + (cin[inoff + 4] & 0xff);
    }

    JPCSC_LOG(("NativeTransmit2(): inlen %d\n", inlen));

    rv =  SCardTransmit(card, &sendPci, cin + inoff, inlen, NULL, cout + outoff, &outlen);

    JPCSC_LOG(("NativeTransmit2(): return code 0x%x, return length %d\n", (int) rv, (int) outlen));

    if ((proto == SCARD_PROTOCOL_T0) && (outlen == 2)){
	if (cout[outoff] == 0x6c && ((*env)->GetBooleanField(env, _this, ResendOnWrongLeField) != 0)){
	    outlen = (*env)->GetArrayLength(env, jout) - outoff;
	    cin[inoff + 4] = cout[outoff + 1];
	    rv = SCardTransmit(card, &sendPci, cin + inoff, inlen, NULL, cout + outoff, &outlen);
	}else{
	    if ((cout[outoff] == 0x61) && ((*env)->GetBooleanField(env, _this, T0GetResponseField) != 0)){
		rv = t0_get_response(card, proto, cout, outoff,  (*env)->GetArrayLength(env, jout) - outoff, &outlen);
	    }
	}
    }
    
    (*env)->ReleaseByteArrayElements(env, jin, cin, JNI_ABORT);
    (*env)->ReleaseByteArrayElements(env, jout, cout, 0);

    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "SCardTransmit()", rv);
	return SCARD_S_SUCCESS;
    }
    
    assert(outlen >= 0);
    return outlen;
}


/*
 * Transmit.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeStatus)
 (JNIEnv *env, jobject _this, jobject jrstate)
{
    SCARDHANDLE card;
    char rdrname[256];
    jbyteArray jatr;
    DWORD rdrlen = 256;
    DWORD pdwState;
    DWORD pdwProto;
    char pbAtr[64];
    DWORD pcbAtrLen = 64;
    LONG rv;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    rv = SCardStatus(card, &rdrname[0], &rdrlen, &pdwState, &pdwProto, &pbAtr[0], &pcbAtrLen);
    if (rv != SCARD_S_SUCCESS){
	JPCSC_LOG(("NativeCardStatus(): error 0x%lx\n", rv));
	return rv;
    }

    (*env)->SetIntField(env, jrstate, CurrentStateField, pdwState);
    (*env)->SetIntField(env, jrstate, ProtoStateField, pdwProto);

    (*env)->SetObjectField(env, jrstate, ReaderStateField, (*env)->NewStringUTF(env, rdrname));

    jatr = (*env)->NewByteArray(env, pcbAtrLen);
    (*env)->SetByteArrayRegion(env, jatr, 0, pcbAtrLen, pbAtr);
    (*env)->SetObjectField(env, jrstate, AtrStateField, jatr);

    return rv;
}


/*
 * Begin transaction.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeBeginTransaction)
 (JNIEnv *env, jobject _this)
{
    SCARDHANDLE card;
    LONG rv;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    rv = SCardBeginTransaction(card);

#ifdef WIN32
    /* Handle MS Resouce Manager bug on Win9x/Me/NT4/Winxpsp1
       http://support.microsoft.com/default.aspx?scid=kb;en-us;230031 */
    if (rv == SCARD_S_SUCCESS){
	char rdrname[256];
	DWORD rdrlen = 256;
	DWORD pdwState;
	DWORD pdwProto;
	char pbAtr[64];
	DWORD pcbAtrLen = 64;
 
	/* verify card handle not reset */
	rv = SCardStatus(card, &rdrname[0], &rdrlen, &pdwState, &pdwProto, &pbAtr[0], &pcbAtrLen);
	
	if (rv == SCARD_S_SUCCESS && pdwState < SCARD_NEGOTIABLE)
	    rv = SCARD_W_RESET_CARD;
    }
#endif /* WIN32 */

    return rv;
}


/*
 * Begin transaction.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeEndTransaction)
 (JNIEnv *env, jobject _this, jint dispo)
{
    SCARDHANDLE card;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    return SCardEndTransaction(card, dispo);
}


/*
 * Reconnect to card.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeReconnect)
 (JNIEnv *env, jobject _this, jint dwSharedMode, jint dwPreferredProtos, jint dwInit)
{
    SCARDHANDLE card;
    DWORD proto;
    LONG rv;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    rv = SCardReconnect(card, (DWORD) dwSharedMode, (DWORD) dwPreferredProtos, 
			(DWORD) dwInit, &proto);
    if (rv != SCARD_S_SUCCESS){
	return rv;
    }
    
    (*env)->SetIntField(env, _this, ProtoField, proto);
    return rv;
}




/*
 * Send control bytes.
 */
JNIEXPORT jbyteArray JNICALL GEN_FUNCNAME(Card_NativeControl)
  (JNIEnv *env, jobject _this, jint jcc, jbyteArray jin, jint joff, jint jlen)
{
    SCARDHANDLE card;
    char *cin;
    jboolean isCopy;
    char cout[RECEIVE_BUFFER_SIZE];
    DWORD clen = RECEIVE_BUFFER_SIZE;
    jbyteArray jout;
    LONG rv;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    cin = (*env)->GetByteArrayElements(env, jin, &isCopy);

/*
#ifndef WIN32
    rv =  SCardControl(card, cin + joff, jlen, cout, &clen);
#else
    JPCSC_LOG(("NativeControl()-win32: control code 0x%x 0x%x\n", jcc, SCARD_CTL_CODE(jcc)));
    rv =  SCardControl(card, SCARD_CTL_CODE(jcc), cin + joff, jlen, cout, clen, &clen);
#endif
*/
#if defined(WIN32) || ! defined(__OLD_PCSC_API__)
    rv =  SCardControl(card, SCARD_CTL_CODE(jcc), cin + joff, jlen, cout, clen, &clen);
#else
    rv =  SCardControl(card, cin + joff, jlen, cout, &clen);
#endif /* defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES) */

    JPCSC_LOG(("NativeControl(): returns 0x%x\n", rv));

    (*env)->ReleaseByteArrayElements(env, jin, cin, JNI_ABORT);

    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "Card.Control() failed", rv);
	return NULL;
    }

    jout = (*env)->NewByteArray(env, clen);
    (*env)->SetByteArrayRegion(env, jout, 0, clen, cout);

    return jout;
}


/*
 * Set an attribute.
 */
JNIEXPORT jint JNICALL GEN_FUNCNAME(Card_NativeSetAttrib)
    (JNIEnv *env, jobject _this, jint jdwAttribId, jbyteArray jattr, jint jattrLen)
{
#if defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES)
    SCARDHANDLE card;
    char *cattr;
    jboolean isCopy;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);
    
    cattr = (*env)->GetByteArrayElements(env, jattr, &isCopy);

    return SCardSetAttrib(card, jdwAttribId, cattr, jattrLen);
#else
    pcscex_throw(env, "Unsupported Get-/Set-Attributes on earlier PCSClite versions", 0);
    return SCARD_E_INVALID_PARAMETER;
#endif /* defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES) */
}


/*
 * Get an attribute.
 */
JNIEXPORT jbyteArray JNICALL GEN_FUNCNAME(Card_NativeGetAttrib)
  (JNIEnv *env, jobject _this, jint jdwAttribId)
{
#if defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES)
    SCARDHANDLE card;
    char cout[RECEIVE_BUFFER_SIZE];
    DWORD clen = RECEIVE_BUFFER_SIZE;
    jbyteArray jout;
    LONG rv;

    card = (SCARDHANDLE) (*env)->GetLongField(env, _this, CardField);

    rv = SCardGetAttrib(card, jdwAttribId, cout, &clen);

    if (rv != SCARD_S_SUCCESS){
	pcscex_throw(env, "SCardGetAttrib()", rv);
	return NULL;
    }

    jout = (*env)->NewByteArray(env, clen);
    (*env)->SetByteArrayRegion(env, jout, 0, clen, cout);

    return jout;
#else
    pcscex_throw(env, "Unsupported Get-/Set-Attributes on earlier PCSClite versions", 0);
    return NULL;
#endif /* defined(WIN32) || defined(HAVE_SCARD_ATTRIBUTES) */
}


#ifdef DEBUG
/*
 * Log a formatted string.
 */
static void jpcsc_log(const char *fmt, ...){
    va_list marker;
    char buf[512];
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);
    fprintf(stderr, buf);
    fflush(stderr);
}

/*
 * Log a reader state.
 */
static void pcsc_readerstatea_log(SCARD_READERSTATE_A *rsa){
    int i;
    fprintf(stderr, "SCARD_READERSTATE_A: at 0x%x\n", (unsigned int)rsa);
    fprintf(stderr, "reader: %s\n", rsa->szReader); 
    fprintf(stderr, "user data: 0x%x\n", (unsigned int)(rsa->pvUserData));
    fprintf(stderr, "current state: 0x%lx\n", rsa->dwCurrentState);
    fprintf(stderr, "event state: 0x%lx\n", rsa->dwEventState);
    fprintf(stderr, "atr: len %ld", rsa->cbAtr);
    if (rsa->cbAtr > 0){
	for (i = 0; i < rsa->cbAtr; i++){
	    fprintf(stderr, "0x%x, ", (unsigned char)(rsa->cbAtr & 0xff));
	}
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}
#endif /* DEBUG */

