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
/* File : eidlib.i */
#ifdef SWIGCSHARP
	%module pteidlib_dotNet
#elif SWIGJAVA
 	%module pteidlibJava_Wrapper
#elif SWIGPYTHON
   %module pteid

   %typemap(python,out) unsigned char *
   {
        $result = PyBytes_FromStringAndSize( (const char *)$1, arg1->Size());
   }

#elif SWIGPERL
    %module Pteid
#elif SWIGPHP
    %module pteid
#elif SWIGJAVASCRIPT
    %module pteid
#else
		TODO
#endif


%newobject eIDMW::PTEID_PDFSignature::getOccupiedSectors(int page);
%newobject eIDMW::PTEID_ScapConnection::postSoapRequest(char *endpoint, char *soapAction, char *soapBody);

%{
#include "eidlib.h"
#include "eidlibException.h"
%}

//Define to avoid swig to create definition for NOEXPORT_PTEIDSDK methods
#define PTEIDSDK_API
#define NOEXPORT_PTEIDSDK SLASH(/)
#define SLASH(s) /##s

/***************************************************************************************
***                            FILE : eidErrors.h                                ***
****************************************************************************************/

%include "../common/eidErrors.h"		//This file contains only define... It doesn't need any typemap

/***************************************************************************************
***                            FILE : eidlibdefines.h                                ***
****************************************************************************************/

%include "eidlibdefines.h"		//This file contains only enum/struct... It doesn't need any typemap

/***************************************************************************************
***                            FILE : eidlibException.h                              ***
****************************************************************************************/

/***************************************************************************************
***                            FILE : CardPteidAddr.h                                ***
****************************************************************************************/

%include "../applayer/CardPteidAddr.h"		//This file contains only define... It doesn't need any typemap

#ifdef SWIGCSHARP	/********************** C# SPECIFICS *************************/

%typemap(csbase) 	eIDMW::PTEID_Exception "System.ApplicationException";

#elif SWIGJAVA		/********************** JAVA SPECIFICS ***********************/

// Here we tell SWIG to create the java class PTEID_Exception as: 'PTEID_Exception extends java.lang.Exception'
%typemap(javabase)  eIDMW::PTEID_Exception "java.lang.Exception";

#elif SWIGPYTHON

#elif SWIGPERL

#elif SWIGPHP

#elif SWIGJAVASCRIPT

#else
    TODO
#endif

%include "eidlibException.h"	//This file contains the exceptions and should not include try/catch

/***************************************************************************************
***                             FILE : eidlib.h                                      ***
****************************************************************************************/

//Name for operator= becomes assign
%rename(assign) operator=;

#ifdef SWIGCSHARP /********************** C# SPECIFICS ***********************/

///////////////////////////////////////// Exception /////////////////////////////////////////////
%exception {
	try
	{
		$action
    }
    catch (eIDMW::PTEID_Exception& e)
	{
		SWIG_CSharpSetPendingExceptionCustom(e.GetError());
		return $null;
    }
}

%{
	// Code to handle throwing of C# CustomApplicationException from C/C++ code.
	// The equivalent delegate to the callback, CSharpExceptionCallback_t, is CustomExceptionDelegate
	// and the equivalent customExceptionCallback instance is customDelegate
	typedef void (SWIGSTDCALL* CSharpExceptionCallback_t)(long);
	CSharpExceptionCallback_t customExceptionCallback = NULL;

	extern "C" SWIGEXPORT
	void SWIGSTDCALL CustomExceptionRegisterCallback(CSharpExceptionCallback_t customCallback)
	{
		customExceptionCallback = customCallback;
	}

	// Note that SWIG detects any method calls named starting with
	// SWIG_CSharpSetPendingException for warning 845
	static void SWIG_CSharpSetPendingExceptionCustom(long code)
	{
		customExceptionCallback(code);
	}
%}

%pragma(csharp) imclasscode=%{
	class CustomExceptionHelper
	{
		// C# delegate for the C/C++ customExceptionCallback
		public delegate void CustomExceptionDelegate(Int32 pteid_excode);
		static CustomExceptionDelegate customDelegate = new CustomExceptionDelegate(SetPendingCustomException);

		[DllImport("$dllimport", EntryPoint="CustomExceptionRegisterCallback")]
		public static extern void CustomExceptionRegisterCallback(CustomExceptionDelegate customCallback);

		static void SetPendingCustomException(Int32 pteid_excode)
		{
			PTEID_Exception ex;

				 if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_PARAM_RANGE)		ex = new PTEID_ExParamRange();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_BAD_USAGE)		ex = new PTEID_ExBadUsage();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_NOT_SUPPORTED)	ex = new PTEID_ExCmdNotSupported();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_NO_CARD)			ex = new PTEID_ExNoCardPresent();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_CMD_NOT_ALLOWED)	ex = new PTEID_ExCmdNotAllowed();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_NO_READER)		ex = new PTEID_ExNoReader();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_UNKNOWN)			ex = new PTEID_ExUnknown();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_CARDTYPE_UNKNOWN)	ex = new PTEID_ExCardTypeUnknown();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_RELEASE_NEEDED)	ex = new PTEID_ExReleaseNeeded();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_BAD_TRANSACTION)	ex = new PTEID_ExBadTransaction();
			else if(pteid_excode==pteidlib_dotNet.EIDMW_ERR_CERT_NOROOT)		ex = new PTEID_ExCertNoRoot();
			else															ex = new PTEID_Exception(pteid_excode);

			SWIGPendingException.Set(ex);
		}

		static CustomExceptionHelper()
		{
			CustomExceptionRegisterCallback(customDelegate);
		}
	}
	static CustomExceptionHelper exceptionHelper = new CustomExceptionHelper();
%}

%define CSHARP_CODE_THROW
"if ($modulePINVOKE.SWIGPendingException.Pending) throw $modulePINVOKE.SWIGPendingException.Retrieve();"
%enddef

///////////////////////////////////////// ByteArray /////////////////////////////////////////////
%typemap(ctype) 				unsigned char * "unsigned char *"
%typemap(imtype,out="IntPtr") 	unsigned char * "byte[]"
%typemap(cstype) 				unsigned char * "byte[]"
%typemap(in) 					unsigned char * %{ $1 = $input; %}
%typemap(out) 					unsigned char * %{ $result = $1; %}
%typemap(csin) 					unsigned char * "$csinput"
%typemap(csin) 					const char * const *  "$csinput"

%typemap(csout, excode=CSHARP_CODE_THROW) unsigned char *
{
	byte[] rslt = new byte[Size()];
	IntPtr pt = $imcall;
	$excode
	Marshal.Copy(pt, rslt, 0,(int) Size());
	return rslt;
}

///////////////////////////////////////// PTEID_LOG /////////////////////////////////////////////
%typemap(ctype) (const char *format,...) "char *"

///////////////////////////////////////// unsigned long & /////////////////////////////////////////////
%typemap(ctype) 				unsigned long & "unsigned long *"
%typemap(imtype) 				unsigned long & "ref uint"
%typemap(cstype) 				unsigned long & "ref uint"
%typemap(csin) 					unsigned long & "ref $csinput"


//////////////////////////////////////////////// const char * ////////////////////////////////////////////////
// char* returned from middleware are utf8 encoded. We need to add some code as C# expects ANSI char*       //
// add  -DSWIG_CSHARP_NO_STRING_HELPER  to your swig command line in order to include this code instead of  //
// the standard string helper in csharphead.swg                                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

%{
/* Callback for returning strings to C# without leaking memory */
typedef char * (SWIGSTDCALL* SWIG_CSharpStringHelperCallback)(const char *);
static SWIG_CSharpStringHelperCallback SWIG_csharp_string_callback = NULL;

#ifdef __cplusplus
extern "C" SWIGEXPORT
#endif
void SWIGSTDCALL SWIG_RegisterStringCallback(SWIG_CSharpStringHelperCallback callback)
{
  SWIG_csharp_string_callback = callback;
}
%}

%pragma(csharp) imclasscode=%{
 protected class SWIGStringHelper {

    public delegate string SWIGStringDelegate(IntPtr message);
    static SWIGStringDelegate stringDelegate = new SWIGStringDelegate(FromUTF8);

    [DllImport("$dllimport", EntryPoint="SWIG_RegisterStringCallback")]
    public static extern void SWIG_RegisterStringCallback(SWIGStringDelegate stringDelegate);

    static string FromUTF8(IntPtr cString) {
        System.Collections.ArrayList myAL = new System.Collections.ArrayList();
        Int32 i = 0;
        Byte b;
        do
        {
            b = System.Runtime.InteropServices.Marshal.ReadByte(cString, i++);
            myAL.Add(b);
        } while (b != 0);
        Byte[] byteString;
        byteString = (Byte[]) myAL.ToArray( typeof(Byte) );

        char[] charData = new char[byteString.Length];
        System.Text.Decoder d = System.Text.Encoding.UTF8.GetDecoder();
        d.GetChars(byteString, 0, byteString.Length, charData, 0);
        string result = new string(charData);
        return result;
    }

    static SWIGStringHelper() {
		SWIG_RegisterStringCallback(stringDelegate);
    }
  }

  static protected SWIGStringHelper swigStringHelper = new SWIGStringHelper();
%}

///////////////////////////////////////// const char * const * /////////////////////////////////////////////
%{
typedef void * (SWIGSTDCALL* CUSTOM_CSharpStringArrayHelperCallback)(void *, long);
static CUSTOM_CSharpStringArrayHelperCallback CUSTOM_CSharpStringArrayCallback = NULL;

#ifdef __cplusplus
extern "C" SWIGEXPORT
#endif
void SWIGSTDCALL CUSTOM_RegisterStringArrayCallback(CUSTOM_CSharpStringArrayHelperCallback callback)
{
  CUSTOM_CSharpStringArrayCallback = callback;
}
%}

%pragma(csharp) imclasscode=
%{
//This will contain the result of the function that return a char**
static public string[] custom_StringArrayResult;

protected class CUSTOM_StringArrayHelper
{
	public delegate void CUSTOM_StringArrayDelegate(IntPtr p, Int32 size);
	static CUSTOM_StringArrayDelegate stringArrayDelegate = new CUSTOM_StringArrayDelegate(FillStringArrayResult);

	[DllImport("$dllimport", EntryPoint="CUSTOM_RegisterStringArrayCallback")]
	public static extern void CUSTOM_RegisterStringArrayCallback(CUSTOM_StringArrayDelegate stringArrayDelegate);

	static void FillStringArrayResult(IntPtr ap, Int32 size)
	{
		IntPtr[] p = new IntPtr[size];
		Marshal.Copy(ap, p, 0, size);

		custom_StringArrayResult = new string[size];
		for (Int32 i = 0; i < size; i++)
		{
			custom_StringArrayResult[i] = Marshal.PtrToStringAnsi(p[i]);
		}
	}

	static CUSTOM_StringArrayHelper()
	{
		CUSTOM_RegisterStringArrayCallback(stringArrayDelegate);
	}
}

static protected CUSTOM_StringArrayHelper custom_StringArrayHelper = new CUSTOM_StringArrayHelper();
%}

%typemap(imtype, out="IntPtr", inattributes="[MarshalAs(UnmanagedType.LPArray)]") const char * const *  "string[]"
%typemap(ctype)		           const char ** "const char * const *"
%typemap(ctype) 					const char * const *  "const char * const *"
%typemap(cstype) 					const char * const * "string[]"
%typemap(out) 						const char * const *
{
	long size=0;
	for(char **p=$1;*p!=NULL;p++)
		size++;

	$result = (const char *const*)CUSTOM_CSharpStringArrayCallback((void *)$1, size);
}

//%typemap(in) 					const char * const * %{ $1 = $input; %}


%typemap(csout, excode=CSHARP_CODE_THROW) const char * const *
{
	IntPtr p = $imcall;
	$excode
	string[] rslt = $imclassname.custom_StringArrayResult;
	return rslt;
}


///////////////////////////////////////// SetEventCallback /////////////////////////////////////////////
//------------------------------------------------------------
//Define the C# callback function into the C++ code
//------------------------------------------------------------
%{
typedef void (SWIGSTDCALL* CUSTOM_CSharpSetEventHelperCallback)(long, unsigned long, void *);
static CUSTOM_CSharpSetEventHelperCallback CUSTOM_CSharpSetEventCallback = NULL;

extern "C" SWIGEXPORT
void SWIGSTDCALL CUSTOM_RegisterSetEventCallback(CUSTOM_CSharpSetEventHelperCallback callback)
{
  CUSTOM_CSharpSetEventCallback = callback;
}

//This is the function registered to the lower level callback for every SetEventCallback
void SetEventCallback_WrapperCpp(long lRet, unsigned long ulState, void *pvRef)
{
	//The goal is just to call the SetEventCallback_WrapperCSharp with the correct parameter
	CUSTOM_CSharpSetEventCallback(lRet,ulState,pvRef);
}
%}

//------------------------------------------------------------
//Change the call to the C++ PTEID_ReaderContext::SetEventCallback
//------------------------------------------------------------
%typemap(ctype)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "long"
%typemap(in)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) ""
%feature("except")	eIDMW::PTEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
    try
    {
	  //Always register the SetEventCallback_WrapperCpp function
      result = (unsigned long)(arg1)->SetEventCallback(&SetEventCallback_WrapperCpp,(void*)arg3);
    }
    catch (eIDMW::PTEID_Exception& e)
    {
      SWIG_CSharpSetPendingExceptionCustom(e.GetError());
      return 0;
    }
}

//------------------------------------------------------------
//Include the delegate definition in the PINVOKE.cs file
//------------------------------------------------------------
%pragma(csharp) imclassimports=
%{
using System;
using System.Runtime.InteropServices;

public delegate void PTEID_SetEventDelegate(Int32 lRet, uint ulState, IntPtr pvRef);

%}

//------------------------------------------------------------
//Define and Register the C#  static SetEventCallBack
//------------------------------------------------------------
%pragma(csharp) imclasscode=
%{
internal class CUSTOM_SetEventHelper
{
	internal delegate void Internal_SetEventDelegate(Int32 lRet, uint ulState, IntPtr pvRef);
	static Internal_SetEventDelegate setEventDelegate = new Internal_SetEventDelegate(Internal_SetEventCallback);

	internal class delegateRef
	{
		public uint handleRef;
		public PTEID_SetEventDelegate functionRef;
		public IntPtr ptrRef;
	}

	internal static int countRef=0;
	internal static System.Collections.Hashtable delegateRefs = new System.Collections.Hashtable();

	[DllImport("$dllimport", EntryPoint="CUSTOM_RegisterSetEventCallback")]
	internal static extern void CUSTOM_RegisterSetEventCallback(Internal_SetEventDelegate setEventDelegate);

	static void Internal_SetEventCallback(Int32 lRet, uint ulState, IntPtr pvRef)
	{
		//Call the proper delegate with ptrRef
		int lRef=pvRef.ToInt32();

		if(delegateRefs.ContainsKey(lRef))
		{
			delegateRef delegateCallback = (delegateRef)delegateRefs[lRef];
			delegateCallback.functionRef(lRet,ulState,delegateCallback.ptrRef);
		}
	}

	static CUSTOM_SetEventHelper()
	{
		CUSTOM_RegisterSetEventCallback(setEventDelegate);
	}
}

static protected CUSTOM_SetEventHelper custom_SetEventHelper = new CUSTOM_SetEventHelper();
%}

//------------------------------------------------------------
//Define the type for intermediate function
//------------------------------------------------------------
%typemap(imtype) 	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "Int32"
%typemap(imtype) 	void *pvRef "IntPtr"

//------------------------------------------------------------
//Overload the C# PTEID_ReaderContext::SetEventCallback
//------------------------------------------------------------
%typemap(cstype)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "PTEID_SetEventDelegate"
%typemap(cstype)	void *pvRef "IntPtr"
%typemap(csin)		void (* callback)(long lRet, unsigned long ulState, void *pvRef) ""		//Not used but avoid generated SWIGTYPE extra files
%typemap(csin)		void *pvRef ""	                                                        //idem

%warnfilter(844) eIDMW::PTEID_ReaderContext::SetEventCallback;
%typemap(csout) unsigned long eIDMW::PTEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
	$modulePINVOKE.CUSTOM_SetEventHelper.countRef++;
	IntPtr ptrCount = new IntPtr(pteidlib_dotNetPINVOKE.CUSTOM_SetEventHelper.countRef);

	//We add the references to the hash table
	$modulePINVOKE.CUSTOM_SetEventHelper.delegateRef callbackRef =  new pteidlib_dotNetPINVOKE.CUSTOM_SetEventHelper.delegateRef();
	callbackRef.functionRef = callback;
	callbackRef.ptrRef = pvRef;
	callbackRef.handleRef = 0;
	$modulePINVOKE.CUSTOM_SetEventHelper.delegateRefs.Add($modulePINVOKE.CUSTOM_SetEventHelper.countRef,callbackRef);

	//We call the SetEvent with the internal reference (countRef)
	uint ulRslt = $modulePINVOKE.PTEID_ReaderContext_SetEventCallback(swigCPtr, 0, ptrCount);
	if ($modulePINVOKE.SWIGPendingException.Pending)
	{
		$modulePINVOKE.CUSTOM_SetEventHelper.delegateRefs.Remove(callbackRef);
		throw $modulePINVOKE.SWIGPendingException.Retrieve();
	}
	else
	{
		callbackRef.handleRef = ulRslt;
	}

	return ulRslt;
}

//------------------------------------------------------------
//Overload the C# PTEID_ReaderContext::StopEventCallback
//------------------------------------------------------------
%typemap(csout, excode=CSHARP_CODE_THROW) void StopEventCallback(unsigned long ulHandle)
{
	$imcall;
	$excode

	//Remove ulHandle from delegateRefs
	foreach($modulePINVOKE.CUSTOM_SetEventHelper.delegateRef callbackRef in $modulePINVOKE.CUSTOM_SetEventHelper.delegateRefs.Values )
    {
      if(callbackRef.handleRef==ulHandle)
      {
          $modulePINVOKE.CUSTOM_SetEventHelper.delegateRefs.Remove(callbackRef);
          break;
      }
    }
}
///////////////////////////////////////// PTEID_Card &PTEID_ReaderContext::getCard() /////////////////////////////////////////////
%warnfilter(844) eIDMW::PTEID_ReaderContext::getCard;

///////////////////////////////////////// PTEID_XMLDoc& PTEID_EIDCard::getDocument(PTEID_DocumentType type) /////////////////////////////////////////////
%warnfilter(844) eIDMW::PTEID_EIDCard::getDocument;
%typemap(csout) eIDMW::PTEID_XMLDoc& eIDMW::PTEID_EIDCard::getDocument(eIDMW::PTEID_DocumentType type)
{
	switch(type)
	{
	case PTEID_DocumentType.PTEID_DOCTYPE_ID:
		return getID();
	case PTEID_DocumentType.PTEID_DOCTYPE_ADDRESS:
		return getAddr();
	/*case PTEID_DocumentType.PTEID_DOCTYPE_PICTURE:
		return getPicture();*/
	case PTEID_DocumentType.PTEID_DOCTYPE_INFO:
		return getVersionInfo();
	case PTEID_DocumentType.PTEID_DOCTYPE_PINS:
		return getPins();
	case PTEID_DocumentType.PTEID_DOCTYPE_CERTIFICATES:
		return getCertificates();
	default:
		throw new PTEID_ExDocTypeUnknown();
	}
}
%ignore eIDMW::PTEID_PDFSignature::getPdfSignature();
#elif SWIGJAVA	/********************** JAVA SPECIFICS ***********************/

///////////////////////////////////////// ByteArray /////////////////////////////////////////////
%typemap(jni)          const unsigned char* "jbyteArray"
%typemap(jtype)        const unsigned char* "byte[]"
%typemap(jstype)       const unsigned char* "byte[]"
%typemap(jstype) 	   const char * const * "String[]"
%include "various.i"
%apply char **STRING_ARRAY { char ** }
//%typemap(jtype) 	   const char * const * "void *"

%typemap(out)          const unsigned char*
{
	$result = jenv->NewByteArray(arg1->Size());
	jenv->SetByteArrayRegion($result,0,arg1->Size(),(const jbyte*) $1);
}

%typemap(javaout)	const unsigned char*
{
return $jnicall;
}

%typemap(in)           const unsigned char*
{
	jint size = jenv->GetArrayLength($input);
	$1 = (unsigned char*) jenv->GetByteArrayElements($input,0);
}

%typemap(javain) const unsigned char* "$javainput"


/*
%typemap(in) const char * const * (jint size) {

  int i = 0;
    size = jenv->GetArrayLength($input);
    $1 = (char **) malloc((size+1)*sizeof(char *));
    // make a copy of each string
    for (i = 0; i<size; i++) {
        jstring j_string = (jstring)jenv->GetObjectArrayElement($input, i);
        $1[i] = (char *) jenv->GetStringUTFChars( j_string, 0);
        jenv->DeleteLocalRef(j_string);
    }
    $1[i] = 0;

}

%typemap(freearg) const char * const * (jint size) {
	//Free the individual strings and then the array of pointers
   for (int i = 0; $1[i] != 0; i++) {
   	free($1[i]);
   }
   free($1);
}
*/

//------------------------------------------------
// This allows a C++ function to return a 'const char * const *' as a Java String array
// The code will be put in the CPP-wrapper to convert the 'const char * const *' coming
// from a function in the C++ library to a Java String[]
// This is put in for the function 'const char* const *ReaderSet::readerList()'
//------------------------------------------------
%typemap(out) const char * const * {
    int i;
    int len=0;
    jstring temp_string;
    const jclass clazz = jenv->FindClass("java/lang/String");

	//------------------------------------------
	// Count the nr of elements returned from C++. This is allowed her since
	// the last element is assumed to be NULL.
	//------------------------------------------
    while ($1[len])
    {
		len++;
	}

	//------------------------------------------
	// create a java array of strings
	//------------------------------------------
    jresult = jenv->NewObjectArray(len, clazz, NULL);
    /* exception checking omitted */

	//------------------------------------------
	// for each element returned from C++
	//------------------------------------------
    for (i=0; i<len; i++)
    {
      temp_string = jenv->NewStringUTF(*result++);
      jenv->SetObjectArrayElement(jresult, i, temp_string);
      jenv->DeleteLocalRef(temp_string);
    }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use for the 'const char * const *' */
//%typemap(jni) const char * const * "jobjectArray"
//%typemap(jtype) const char * const * "String[]"
//%typemap(jstype) const char * const * "String[]"

/* These 2 typemaps handle the conversion of the jtype to jstype typemap type
   and vice versa for the 'const char * const *'
%typemap(javain) const char * const * "$javainput"
%typemap(javaout) const char * const * {
    return $jnicall;
  }
*/

///////////////////////////////////////// Exception /////////////////////////////////////////////

//------------------------------------------------------------
// code handling C++ exceptions
// The C++ exception will be caught using the base class. A Java exception
// will then be constructed and thrown to the calling Java application
// This code is put in a separate class to meke the C++ code generation
// smaller.
//------------------------------------------------------------
%{
	#include "eidErrors.h"
	class CustomExceptionHelper
	{
	public:
		static void throwJavaException( unsigned long err, JNIEnv* jenv )
		{
			jclass		clazz;
			jmethodID	methodID;
			std::string classDescription = "pt/gov/cartaodecidadao";

			switch(err)
			{
			case EIDMW_ERR_RELEASE_NEEDED:
				classDescription += "/PTEID_ExReleaseNeeded";
				break;
			case EIDMW_ERR_PARAM_RANGE:
				classDescription += "/PTEID_ExParamRange";
				break;
			case EIDMW_ERR_CMD_NOT_ALLOWED:
				classDescription += "/PTEID_ExCmdNotAllowed";
				break;
			case EIDMW_ERR_NOT_SUPPORTED:
				classDescription += "/PTEID_ExCmdNotSupported";
				break;
			case EIDMW_ERR_NO_CARD:
				classDescription += "/PTEID_ExNoCardPresent";
				break;
			case EIDMW_ERR_CARDTYPE_UNKNOWN:
				classDescription += "/PTEID_ExCardTypeUnknown";
				break;
			case EIDMW_ERR_CERT_NOROOT:
				classDescription += "/PTEID_ExCertNoRoot";
				break;
			case EIDMW_ERR_BAD_USAGE:
				classDescription += "/PTEID_ExBadUsage";
				break;
			case EIDMW_ERR_BAD_TRANSACTION:
				classDescription += "/PTEID_ExBadTransaction";
				break;
			case EIDMW_ERR_NO_READER:
				classDescription += "/PTEID_ExNoReader";
				break;
			case EIDMW_ERR_UNKNOWN:
				classDescription += "/PTEID_ExUnknown";
				break;
			case EIDMW_SOD_UNEXPECTED_VALUE:
			case EIDMW_SOD_UNEXPECTED_ASN1_TAG:
			case EIDMW_SOD_UNEXPECTED_ALGO_OID:
			case EIDMW_SOD_ERR_HASH_NO_MATCH_ID:
			case EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS:
			case EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE:
			case EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY:
			case EIDMW_SOD_ERR_VERIFY_SOD_SIGN:
				classDescription += "/PTEID_ExSOD";
				clazz = jenv->FindClass(classDescription.c_str());
				if (clazz)
				{
					methodID   = jenv->GetMethodID(clazz, "<init>", "(I)V");
					if(methodID)
					{
						jthrowable  exc   = static_cast<jthrowable>(jenv->NewObject(clazz, methodID, err));
						jint success = jenv->Throw(exc);
					}
				}
				return;
				break;
			case EIDMW_ERR_CARD:
			default:
				classDescription += "/PTEID_Exception";
				clazz = jenv->FindClass(classDescription.c_str());
				if (clazz)
				{
					methodID   = jenv->GetMethodID(clazz, "<init>", "(I)V");
					if(methodID)
					{
						jthrowable  exc   = static_cast<jthrowable>(jenv->NewObject(clazz, methodID, err));
						jint success = jenv->Throw(exc);
					}
				}
				return;
				break;
			}
			clazz = jenv->FindClass(classDescription.c_str());
			if (clazz)
			{
				methodID   = jenv->GetMethodID(clazz, "<init>", "()V");
				if(methodID)
				{
					jthrowable  exc   = static_cast<jthrowable>(jenv->NewObject(clazz, methodID));
					jint success = jenv->Throw(exc);
				}
			}
		}
	};
%}
//------------------------------------------------------------
// Code to insert on the C++ wrapper side to generate a Java exception
// when an exception is thrown in the C++ EID lib
//------------------------------------------------------------
%define JAVA_CODE_THROW
{
		try {
			$action
			}
		catch (eIDMW::PTEID_Exception& e)
		{
			long err = e.GetError();
			CustomExceptionHelper::throwJavaException(err,jenv);
			return $null;
		}
		catch (std::exception& e)
		{
			std::string err = e.what();
			jclass clazz = jenv->FindClass("java/lang/Exception");
			jenv->ThrowNew(clazz, err.c_str());
			return $null;
		}
}
%enddef
%define JAVA_CODE_THROW_releaseSDK
{
		try {
			$action
			while(WrapperCppDataContainer.size()>0)
			{
			  delete WrapperCppDataContainer.back();
			  WrapperCppDataContainer.pop_back();
			}
		}
		catch (eIDMW::PTEID_Exception& e)
		{
			long err = e.GetError();
			CustomExceptionHelper::throwJavaException(err,jenv);
			return $null;
		}
		catch (std::exception& e)
		{
			std::string err = e.what();
			jclass clazz = jenv->FindClass("java/lang/Exception");
			jenv->ThrowNew(clazz, err.c_str());
			return $null;
		}
}
%enddef

%define JAVA_CODE_THROW_ReleaseMemory
{
		try {
			$action
			while(WrapperCppDataContainer.size()>0)
			{
			  delete WrapperCppDataContainer.back();
			  WrapperCppDataContainer.pop_back();
			}
		}
		catch (eIDMW::PTEID_Exception& e)
		{

    		for (int i=0; i < size3; i++)
 				delete[] arg3[i];
    		delete[] arg3;

			long err = e.GetError();
			CustomExceptionHelper::throwJavaException(err,jenv);
			return $null;
		}
		catch (std::exception& e)
		{
			for (int i=0; i<size3; i++)
 				delete[] arg3[i];

    		delete[] arg3;
			std::string err = e.what();
			jclass clazz = jenv->FindClass("java/lang/Exception");
			jenv->ThrowNew(clazz, err.c_str());
			return $null;
		}
}
%enddef

//------------------------------------------------------------
// from here on, the functions are processed that possibly generate an
// exception.
// It is done per function because there seems no way in SWIG to let it
// generate the exception catching for all functions except the contructors
// and destructors.
//------------------------------------------------------------
// class PTEID_Object : none
//------------------------------------------------------------
// class PTEID_ByteArray: none
//------------------------------------------------------------
// class PTEID_ReaderSet:
//------------------------------------------------------------
%javaexception("PTEID_Exception") initSDK			JAVA_CODE_THROW
%javaexception("PTEID_Exception") releaseSDK		JAVA_CODE_THROW_releaseSDK
%javaexception("PTEID_Exception") releaseReaders	JAVA_CODE_THROW
%javaexception("PTEID_Exception") isReadersChanged	JAVA_CODE_THROW
%javaexception("PTEID_Exception") readerList		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getReader			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getReaderByName	JAVA_CODE_THROW
%javaexception("PTEID_Exception") readerCount		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getReaderName		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getReaderByNum	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getReaderByCardSerialNumber	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") flushCache		JAVA_CODE_THROW
%javaexception("PTEID_Exception") instance		JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_ReaderContext:
//------------------------------------------------------------
%javaexception("PTEID_Exception") getName			JAVA_CODE_THROW
%javaexception("PTEID_Exception") isCardPresent		JAVA_CODE_THROW
%javaexception("PTEID_Exception") releaseCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") isCardChanged		JAVA_CODE_THROW
%javaexception("PTEID_Exception") isCardChanged		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCardType 		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCard			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getEIDCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") BeginTransaction	JAVA_CODE_THROW
%javaexception("PTEID_Exception") EndTransaction	JAVA_CODE_THROW
%javaexception("PTEID_Exception") isVirtualReader	JAVA_CODE_THROW
%javaexception("PTEID_Exception") SetEventCallback	JAVA_CODE_THROW
%javaexception("PTEID_Exception") StopEventCallback	JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Card:
//------------------------------------------------------------

%javaexception("PTEID_Exception") getType			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDocument		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getRawData		JAVA_CODE_THROW
%javaexception("PTEID_Exception") sendAPDU			JAVA_CODE_THROW
%javaexception("PTEID_Exception") Sign			JAVA_CODE_THROW
%javaexception("PTEID_Exception") readFile			JAVA_CODE_THROW
%javaexception("PTEID_Exception") writeFile			JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_MemoryCard: none
//------------------------------------------------------------
//------------------------------------------------------------
// class PTEID_SmartCard
//------------------------------------------------------------
%javaexception("PTEID_Exception") selectApplication	JAVA_CODE_THROW
%javaexception("PTEID_Exception") sendAPDU			JAVA_CODE_THROW
%javaexception("PTEID_Exception") readFile			JAVA_CODE_THROW
%javaexception("PTEID_Exception") writeFile			JAVA_CODE_THROW
%javaexception("PTEID_Exception") pinCount			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPins			JAVA_CODE_THROW
%javaexception("PTEID_Exception") certificateCount	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCertificates	JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_EIDCard
//------------------------------------------------------------
%javaexception("PTEID_Exception") isApplicationAllowed	JAVA_CODE_THROW
%javaexception("PTEID_Exception") Activate				JAVA_CODE_THROW
%javaexception("PTEID_Exception") isTestCard			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAllowTestCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") setAllowTestCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getXMLCCDoc			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getID					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAddr				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPicture			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getVersionInfo		JAVA_CODE_THROW
%javaexception("PTEID_Exception") SignXades		        JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") SignXadesT		    JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") SignXadesA		    JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") SignPDF               JAVA_CODE_THROW
%javaexception("PTEID_Exception") SignXadesIndividual	JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") SignXadesTIndividual	JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") SignXadesAIndividual	JAVA_CODE_THROW_ReleaseMemory
%javaexception("PTEID_Exception") doSODCheck			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getRootCAPubKey		JAVA_CODE_THROW
%javaexception("PTEID_Exception") readPersonalNotes		JAVA_CODE_THROW
%javaexception("PTEID_Exception") writePersonalNotes	JAVA_CODE_THROW
%javaexception("PTEID_Exception") clearPersonalNotes	JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_CCXML_Doc ccxml
//------------------------------------------------------------
%javaexception("PTEID_Exception") getCCXML			JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_XMLDoc
//------------------------------------------------------------
%javaexception("PTEID_Exception") getXML			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCSV			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getTLV			JAVA_CODE_THROW
%javaexception("PTEID_Exception") writeXmlToFile	JAVA_CODE_THROW
%javaexception("PTEID_Exception") writeCsvToFile	JAVA_CODE_THROW
%javaexception("PTEID_Exception") writeTlvToFile	JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Biometric: none
//------------------------------------------------------------
//------------------------------------------------------------
// class PTEID_Crypto: none
//------------------------------------------------------------
//------------------------------------------------------------
// class PTEID_CardVersionInfo
//------------------------------------------------------------
%javaexception("PTEID_Exception") isActive								JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSerialNumber						JAVA_CODE_THROW
%javaexception("PTEID_Exception") getComponentCode						JAVA_CODE_THROW
%javaexception("PTEID_Exception") getOsNumber							JAVA_CODE_THROW
%javaexception("PTEID_Exception") getOsVersion							JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSoftmaskNumber						JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSoftmaskVersion					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAppletVersion						JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGlobalOsVersion					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAppletInterfaceVersion				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPKCS1Support						JAVA_CODE_THROW
%javaexception("PTEID_Exception") getKeyExchangeVersion					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAppletLifeCycle					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGraphicalPersonalisation			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getElectricalPersonalisation			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getElectricalPersonalisationInterface	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSignature							JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_EId
//------------------------------------------------------------
%javaexception("PTEID_Exception") getDocumentVersion		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDocumentType			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGivenName				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSurname				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGender					JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDateOfBirth			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLocationOfBirth		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getNobility				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getNationality			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCivilianIdNumber		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDuplicata			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSpecialOrganization JAVA_CODE_THROW
%javaexception("PTEID_Exception") getMemberOfFamily		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLogicalNumber			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDocumentPAN			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getValidityBeginDate		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getValidityEndDate		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLocalofRequest			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAddressVersion		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreet				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZipCode			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCountry				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSpecialStatus		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPhoto          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPhotoRaw          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getHeight	          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDocumentNumber         JAVA_CODE_THROW
%javaexception("PTEID_Exception") getTaxNo	          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSocialSecurityNumber   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getHealthNumber          	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getIssuingEntity          JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLocalofRequest         JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGivenNameFather        JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSurnameFather          JAVA_CODE_THROW
%javaexception("PTEID_Exception") getGivenNameMother        JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSurnameMother          JAVA_CODE_THROW
%javaexception("PTEID_Exception") getParents	          	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPersoData	          	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getValidation         JAVA_CODE_THROW
%javaexception("PTEID_Exception") getMRZ1	          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getMRZ2	          		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getMRZ3        	  		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAccidentalIndications 	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPhotoObj				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCardAuthKeyObj			JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Address
//------------------------------------------------------------
%javaexception("PTEID_Exception") getMunicipality		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDistrict			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreetName			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCivilParish		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAbbrStreetType		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreetType			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAbbrBuildingType	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getBuildingType		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDoorNo				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getFloor				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSide				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLocality			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPlace				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZip4				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZip3				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPostalLocality		JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_EIdFullDoc: none
//------------------------------------------------------------
//------------------------------------------------------------
// class PTEID_Pins
//------------------------------------------------------------
%javaexception("PTEID_Exception") count 	   		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPinByNumber	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPinByPinRef	JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Pin
//------------------------------------------------------------
%javaexception("PTEID_Exception") getIndex		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getType		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getId			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getUsageCode	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getFlags		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLabel		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSignature	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getTriesLeft	JAVA_CODE_THROW
%javaexception("PTEID_Exception") verifyPin		JAVA_CODE_THROW
%javaexception("PTEID_Exception") changePin		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPinRef 	JAVA_CODE_THROW
%javaexception("PTEID_Exception") unlockPin		JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Certificates
//------------------------------------------------------------
%javaexception("PTEID_Exception") countFromCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") countAll			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCertFromCard	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCert			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getRoot			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCA				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSignature		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAuthentication	JAVA_CODE_THROW
%javaexception("PTEID_Exception") addCertificate	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCert			JAVA_CODE_THROW
%javaexception("PTEID_Exception") addToSODCAs		JAVA_CODE_THROW
%javaexception("PTEID_Exception") resetSODCAs		JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Certificate
//------------------------------------------------------------
%javaexception("PTEID_Exception") getLabel			JAVA_CODE_THROW
%javaexception("PTEID_Exception") getID				JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCertData		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSerialNumber	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getOwnerName		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getIssuerName		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getValidityBegin	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getValidityEnd	JAVA_CODE_THROW
%javaexception("PTEID_Exception") getKeyLength		JAVA_CODE_THROW
%javaexception("PTEID_Exception") isRoot			JAVA_CODE_THROW
%javaexception("PTEID_Exception") isFromPteidValidChain	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") isFromCard		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getIssuer			JAVA_CODE_THROW
%javaexception("PTEID_Exception") countChildren		JAVA_CODE_THROW
%javaexception("PTEID_Exception") getChildren		JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Config
//------------------------------------------------------------
%javaexception("PTEID_Exception") getString	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLong	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") setString	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") setLong	   JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Sod
//------------------------------------------------------------
%javaexception("PTEID_Exception") getData	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getHash	   JAVA_CODE_THROW

//------------------------------------------------------------
// class PTEID_Address
//------------------------------------------------------------
%javaexception("PTEID_Exception") getAddressVersion	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreet	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZipCode	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getMunicipality	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDistrict	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreetName	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getCivilParish	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAbbrStreetType	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getStreetType	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getAbbrBuildingType	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getBuildingType	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getDoorNo	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getFloor	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getSide	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getLocality	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPlace	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZip4	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getZip3	   JAVA_CODE_THROW
%javaexception("PTEID_Exception") getPostalLocality	   JAVA_CODE_THROW

///////////////////////////////////////// SetEventCallback /////////////////////////////////////////////
%typemap(ctype)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "long"
%typemap(in)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) ""

//------------------------------------------------------------
// Define the function in the C++ wrapper that will call the C++ DLL
// function 'SetEventCallback'
// This function is called by the Java side and will call on its turn the eidlib
// interface function.
// The JVM pointer has to be kept for use in the callback function.
//------------------------------------------------------------
%feature("except")	eIDMW::PTEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
    try
    {
		JavaVM *jvm;
		jenv->GetJavaVM(&jvm);		// recover the JVM pointer

		SetEventCallback_WrapperCppData* callbackData = new SetEventCallback_WrapperCppData(jvm,(long int)arg3);
		WrapperCppDataContainer.push_back(callbackData);

		//------------------------------------------------------
		// set the C++ callback function with the callbackData.
		//------------------------------------------------------
		result = (unsigned long)(arg1)->SetEventCallback(&SetEventCallback_WrapperCpp,(void*)callbackData);
		callbackData->m_handle = result;	// keep the handle. it is used to find the correct object for the delete (see: StopEventCallback)
    }
    catch (eIDMW::PTEID_Exception& e)
    {
		long err = e.GetError();
		CustomExceptionHelper::throwJavaException(err,jenv);
		return $null;
    }
	catch (std::exception& e)
	{
		std::string err = e.what();
		jclass clazz = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(clazz, err.c_str());
		return $null;
	}
}

//------------------------------------------------------------
// Define the function in the C++ wrapper that will call the C++ DLL
// function 'StopEventCallback'
// This function is called by the Java side and will call the eidlib
// function.
// Since the SetEventCallback(...) has allocated memory, we have to
// deallocate the data here.
//------------------------------------------------------------
%feature("except")	eIDMW::PTEID_ReaderContext::StopEventCallback(unsigned long ulHandle)
{
    try
    {
		(arg1)->StopEventCallback(arg2);	// call the eidlib method to stop the event with this handle
		for(size_t idx = 0
		   ;idx < WrapperCppDataContainer.size()
		   ;idx++
		   )
		{
			//----------------------------------------------------------
			// if this is the allocated data for this handle, remove it
			//----------------------------------------------------------
			if( WrapperCppDataContainer[idx]->m_handle == arg2 )
			{
				delete WrapperCppDataContainer[idx];
				WrapperCppDataContainer.erase(WrapperCppDataContainer.begin()+idx);
				break;
			}
		}
    }
    catch (eIDMW::PTEID_Exception& e)
    {
		long err = e.GetError();
		CustomExceptionHelper::throwJavaException(err,jenv);
		return $null;
    }
	catch (std::exception& e)
	{
		std::string err = e.what();
		jclass clazz = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(clazz, err.c_str());
		return $null;
	}
}


//------------------------------------------------------------
//Define the type for intermediate function
//------------------------------------------------------------
%typemap(imtype) 	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "long"
%typemap(imtype) 	void *pvRef "long"

//------------------------------------------------------------
//Overload the Java PTEID_ReaderContext::SetEventCallback
// This function will only receive:
// - an interface of type Callback which will simulate the callback
// - an object that contains callback data of any kind
//------------------------------------------------------------
%typemap(jstype)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) "Callback"
%typemap(jstype)	void *pvRef "Object"
%typemap(javain)	void (* callback)(long lRet, unsigned long ulState, void *pvRef) ""		//Not used but avoid generated SWIGTYPE extra files
%typemap(javain)	void *pvRef ""															//idem

//------------------------------------------------------------
// rewrite the function SetEventCallback(...) in PTEID_ReaderContext
//------------------------------------------------------------
// we have to add the following static variables in Java to PTEID_ReaderContext:
//	static int m_counter = 0;
//	static HashMap m_CallbackContainer = new HashMap();
//
// Generate an import statement for PTEID_ReaderContext.java
// needed to access the map
//------------------------------------------------------------
%typemap(javaimports) eIDMW::PTEID_ReaderContext "import java.util.*;"

//------------------------------------------------------------
// in the Java class eIDMW::PTEID_ReaderContext, add some members
// and an extra function
//------------------------------------------------------------
%typemap(javacode) eIDMW::PTEID_ReaderContext
%{
	private static int		m_counter = 0;							// static counter to keep the nr of SetEventCallback we've done
	private static HashMap	m_CallbackContainer = new HashMap();	// static map to keep the data for each callback

	//------------------------------------------------------------
	// this function is called from C++ wrapper side as callback function.
	// It will receive the key in the map, from which it can recover
	// the callback helper class. This helper class contains the callback object
	// and the data object for this callback.
	// As such, the callback class will call, via the interface function of the callback object,
	// the Java application. The data object is passed as an argument.
	// In the application, this data from the dataobject can be read for whatever reason.
	//------------------------------------------------------------
	public static void doJavaCallBack( int iKey, long lRet, long ulState )
	{
		Integer key = new Integer(iKey);
		CallbackHelper callbackHelper = (CallbackHelper)m_CallbackContainer.get( key );
		callbackHelper.m_callbackObject.getEvent(lRet, ulState, callbackHelper.m_callbackData);
	}
%}

//------------------------------------------------------------
// On the Java interface, overwrite the function SetEventCallBack(...)
//------------------------------------------------------------
%typemap(javaout) unsigned long eIDMW::PTEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
	m_counter++;
	Integer key = new Integer(m_counter);
	m_CallbackContainer.put(key, new CallbackHelper(callback, pvRef));
	long result = pteidlibJava_WrapperJNI.PTEID_ReaderContext_SetEventCallback(swigCPtr, this, 0, m_counter);
	return result;
}

//------------------------------------------------------------
// On the Java interface, overwrite the function StopEventCallBack(...)
//------------------------------------------------------------
%typemap(javaout) void eIDMW::PTEID_ReaderContext::StopEventCallback(unsigned long ulHandle)
{
    pteidlibJava_WrapperJNI.PTEID_ReaderContext_StopEventCallback(swigCPtr, this, ulHandle);

	Set CallbackContainerKeys = m_CallbackContainer.keySet();
	Iterator itr = CallbackContainerKeys.iterator();

	while (itr.hasNext())
	{
		Integer		   key		= (Integer)itr.next();
		CallbackHelper cbHelper = (CallbackHelper)m_CallbackContainer.get(key);

		if (cbHelper.m_handle == ulHandle)
		{
			m_CallbackContainer.remove(key);
			break;
		}
	}
}

//------------------------------------------------------------
//Define the Java callback function into the C++ code
//------------------------------------------------------------
%{

//------------------------------------------------------------
// Callback function declaration on the C++ side. This function will be passed
// to the setEventCallBack() of the DLL.
// As a result, this is the function that will be called by the eidlib as callback function
// when something happens with the card in the reader.
// This function will have in the 'pvRef'-parameter the index of the callback
// In Java, this index must be used as key in the map to find the object
// with the function to be executed.
//------------------------------------------------------------
class SetEventCallback_WrapperCppData
{
public:
	SetEventCallback_WrapperCppData()
	: m_jvm(0)
	, m_index(0)
	, m_handle(-1)
	{
	}
	SetEventCallback_WrapperCppData(JavaVM *jvm, long int index)
	: m_jvm(jvm)
	, m_index(index)
	, m_handle(-1)
	{
	}
	JavaVM*		  m_jvm;		// JVM pointer storage, to be used at callback time to call the JAVA side
	long int	  m_index;		// index of the callback at the JAVA side
	unsigned long m_handle;		// handle the SetEventCallback() will generate
};

#include <vector>
static std::vector<SetEventCallback_WrapperCppData*> WrapperCppDataContainer;

extern "C" SWIGEXPORT
//------------------------------------------------------------------
// C++ internal callback function
// In the C++ wrapper we create a callback function that the eidlib
// as callback.
// As soon as the eidlib dll calls this callback function we must call
// the Java side to 'pass' the callback
// - get the JVM pointer (was stored in the callback data)
// - from the JVM, recover the java environment
// - attach the environment to the current thread if necessary
// - get the callback function ID on the Java side
// - call the static function on the java side with the necessary data
//
//------------------------------------------------------------------
void SetEventCallback_WrapperCpp(long lRet, unsigned long ulState, void *pvRef)
{
	//------------------------------------------------------------------
	// The goal is now to call Java. In this way we patch the callback function
	// from C++ to Java
	//------------------------------------------------------------------
	SetEventCallback_WrapperCppData* callbackData = (SetEventCallback_WrapperCppData*)pvRef;

	JNIEnv* env;
    JavaVM* jvm = callbackData->m_jvm;

	jint	res = jvm->GetEnv((void**)&env, JNI_VERSION_1_2);

	if ( res == JNI_EDETACHED )
	{
		JavaVMAttachArgs vm_att_args;

		vm_att_args.version = JNI_VERSION_1_2;
		vm_att_args.name	= NULL;
		vm_att_args.group	= NULL;

		res = jvm->AttachCurrentThread((void**)&env, &vm_att_args);
	}

	jclass		cls = env->FindClass("pt/gov/cartaodecidadao/PTEID_ReaderContext");
	jmethodID	mid = env->GetStaticMethodID(cls, "doJavaCallBack", "(IJJ)V");

	jvalue args[3];
	args[0].i = callbackData->m_index;
	args[1].j = lRet;
	args[2].j = ulState;

	env->CallStaticVoidMethodA(cls, mid, args);
	jvm->DetachCurrentThread();
}

%}

//-------------------------------------------------
// include this template file to have access to
// pointer functions via C++, needed to pass 'unsigned long&'
//-------------------------------------------------
%include cpointer.i
%pointer_functions(unsigned long, ulongp);

//-------------------------------------------------
// All the parameters of type 'unsigned long&' will be
// mapped on the java methods as 'PTEID_ulwrapper'
//-------------------------------------------------
%typemap(jstype) unsigned long& "PTEID_ulwrapper"

//--------------------------------------------------
// We don't take the method 'eIDMW::PTEID_Pin::verifyPin()' into account
// It will be removed from the java interface
//--------------------------------------------------
%ignore verifyPin();

//--------------------------------------------------
// rewrite the function verifyPin(...) on the Java side
//--------------------------------------------------
%typemap(javaout) bool eIDMW::PTEID_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg)
{
	  long		pRemaining = pteidlibJava_WrapperJNI.new_ulongp();				// get a C++ ptr
	  boolean	retval     = pteidlibJava_WrapperJNI.PTEID_Pin_verifyPin(swigCPtr, this, csPin, pRemaining, bShowDlg);

	  ulRemaining.m_long = pteidlibJava_WrapperJNI.ulongp_value(pRemaining);
	  pteidlibJava_WrapperJNI.delete_ulongp(pRemaining);
	  return retval;
}

%ignore eIDMW::PTEID_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining,bool bShowDlg, void *wndGeometry );
%ignore eIDMW::PTEID_Pin::verifyPin(const char *csPin,unsigned long &ulRemaining);
%ignore eIDMW::PTEID_PDFSignature::getPdfSignature();

// --------------------------------------
// the following should be generated for the method without parameters, but
// I dont know how to do it with SWIG
// --------------------------------------
// %typemap(javaout) eIDMW::PTEID_Pin::verifyPin()
// {
// 	  boolean retval = pteidlibJava_WrapperJNI.PTEID_Pin_verifyPin(swigCPtr, this);
// 	  return retval;
// }

//--------------------------------------------------
// We don't take the method 'eIDMW::PTEID_Pin::changePin()' into account
// It will be removed from the java interface
//--------------------------------------------------
%ignore changePin();

//--------------------------------------------------
// rewrite the function changePin(...) on the Java side
//--------------------------------------------------
%typemap(javaout) bool eIDMW::PTEID_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg)
{
	  long		pRemaining	= pteidlibJava_WrapperJNI.new_ulongp();				// get a C++ ptr
	  boolean	retval		= pteidlibJava_WrapperJNI.PTEID_Pin_changePin(swigCPtr, this, csPin1, csPin2, pRemaining, PinName, bShowDlg);

	  ulRemaining.m_long = pteidlibJava_WrapperJNI.ulongp_value(pRemaining);
	  pteidlibJava_WrapperJNI.delete_ulongp(pRemaining);
	  return retval;
}

%ignore eIDMW::PTEID_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName,bool bShowDlg, void *wndGeometry );
%ignore eIDMW::PTEID_Pin::changePin(const char *csPin1,const char *csPin2,unsigned long &ulRemaining, const char *PinName);


// --------------------------------------
// the following should be generated for the method without parameters, but
// I dont know how to do it with SWIG
// --------------------------------------
// %typemap(javaout) eIDMW::PTEID_Pin::changePin()
// {
// 	  boolean retval = pteidlibJava_WrapperJNI.PTEID_Pin_changePin(swigCPtr, this);
// 	  return retval;
// }

//%ignore isCardChanged(unsigned long &ulOldId);
//--------------------------------------------------
// rewrite the function isCardChanged(...) on the Java side
//--------------------------------------------------
%typemap(javaout) bool eIDMW::PTEID_ReaderContext::isCardChanged(unsigned long &ulOldId)
{
	  long pID = pteidlibJava_WrapperJNI.new_ulongp();				// get a C++ ptr
	  pteidlibJava_WrapperJNI.ulongp_assign(pID,ulOldId.m_long);
	  boolean retval = pteidlibJava_WrapperJNI.PTEID_ReaderContext_isCardChanged(swigCPtr, this, pID);
	  ulOldId.m_long = pteidlibJava_WrapperJNI.ulongp_value(pID);
	  pteidlibJava_WrapperJNI.delete_ulongp(pID);
	  return retval;
}

//--------------------------------------------------
// rewrite the function unlockPin(...) on the Java side
//--------------------------------------------------
%typemap(javaout) bool eIDMW::PTEID_Pin::unlockPin(const char *pszPuk, const char *pszNewPin, unsigned long &triesLeft, unsigned long flags)
{
	  long		pRemaining = pteidlibJava_WrapperJNI.new_ulongp();				// get a C++ ptr
	  boolean	retval     = pteidlibJava_WrapperJNI.PTEID_Pin_unlockPin(swigCPtr, this, pszPuk, pszNewPin, pRemaining, flags);

	  triesLeft.m_long = pteidlibJava_WrapperJNI.ulongp_value(pRemaining);
	  pteidlibJava_WrapperJNI.delete_ulongp(pRemaining);
	  return retval;
}

//--------------------------------------------------
// Ignore this method from the java interface
// However for Java the setCustomImage(const PTEID_ByteArray &image_data) is exposed
// which should be used as an alterantive
//--------------------------------------------------
%ignore eIDMW::PTEID_PDFSignature::setCustomImage(unsigned char *image_data, unsigned long img_length);

#elif SWIGPYTHON

#elif SWIGPERL

#elif SWIGPHP

#elif SWIGJAVASCRIPT

#else
		TODO
#endif

%include "eidlib.h"
