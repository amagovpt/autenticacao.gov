/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * XSECBinHTTPURIInputStream := Re-implementation of the Xerces
 *							    BinHTTPURLInputStream.  Allows us to make
 *								some small changes to support the requirements
 *								of XMLDSIG (notably re-directs)
 *
 * NOTE: Much code taken from Xerces, and the cross platform interfacing is
 * no-where near as nice.
 *
 * $Id: XSECBinHTTPURIInputStream.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

#include <xsec/utils/winutils/XSECBinHTTPURIInputStream.hpp>

#define _WINSOCKAPI_

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

#include <xsec/utils/winutils/XSECBinHTTPURIInputStream.hpp>
#include <xsec/framework/XSECError.hpp>

#include "../../utils/XSECAutoPtr.hpp"

static HMODULE gWinsockLib = NULL;
static LPFN_GETHOSTBYNAME gWSgethostbyname = NULL;
static LPFN_INET_ADDR gWSinet_addr = NULL;
static LPFN_GETHOSTBYADDR gWSgethostbyaddr = NULL;
static LPFN_HTONS gWShtons = NULL;
static LPFN_SOCKET gWSsocket = NULL;
static LPFN_CONNECT gWSconnect = NULL;
static LPFN_SEND gWSsend = NULL;
static LPFN_RECV gWSrecv = NULL;
static LPFN_SHUTDOWN gWSshutdown = NULL;
static LPFN_CLOSESOCKET gWSclosesocket = NULL;
static LPFN_WSACLEANUP gWSACleanup = NULL;

bool XSECBinHTTPURIInputStream::fInitialized = false;
XMLMutex* XSECBinHTTPURIInputStream::fInitMutex = 0;

// Ported from Xerces 2.x, now missing on 3.x, so I'm inlining it.
static void* compareAndSwap(void** toFill, const void* const newValue, const void* const toCompare) {
#if defined _WIN64
    return ::InterlockedCompareExchangePointer(toFill, (void*)newValue, (void*)toCompare);
#else
    void*   result;
    __asm
    {
        mov             eax, toCompare;
        mov             ebx, newValue;
        mov             ecx, toFill
        lock cmpxchg    [ecx], ebx;
        mov             result, eax;
    }
    return result;
#endif
}

void XSECBinHTTPURIInputStream::Initialize() {
    //
    // Initialize the WinSock library here.
    //
    WORD        wVersionRequested;
    WSADATA     wsaData;

	LPFN_WSASTARTUP startup = NULL;
	if(gWinsockLib == NULL) {
		gWinsockLib = LoadLibrary(_T("WSOCK32"));
		if(gWinsockLib == NULL) {
			ThrowXML(NetAccessorException, XMLExcepts::NetAcc_InitFailed);
		}
		else {
			startup = (LPFN_WSASTARTUP) GetProcAddress(gWinsockLib,_T("WSAStartup"));
			gWSACleanup = (LPFN_WSACLEANUP) GetProcAddress(gWinsockLib,_T("WSACleanup"));
			gWSgethostbyname = (LPFN_GETHOSTBYNAME) GetProcAddress(gWinsockLib,_T("gethostbyname"));
			gWSinet_addr = (LPFN_INET_ADDR) GetProcAddress(gWinsockLib,_T("inet_addr"));
			gWSgethostbyaddr = (LPFN_GETHOSTBYADDR) GetProcAddress(gWinsockLib,_T("gethostbyaddr"));
			gWShtons = (LPFN_HTONS) GetProcAddress(gWinsockLib,_T("htons"));
			gWSsocket = (LPFN_SOCKET) GetProcAddress(gWinsockLib,_T("socket"));
			gWSconnect = (LPFN_CONNECT) GetProcAddress(gWinsockLib,_T("connect"));
			gWSsend = (LPFN_SEND) GetProcAddress(gWinsockLib,_T("send"));
			gWSrecv = (LPFN_RECV) GetProcAddress(gWinsockLib,_T("recv"));
			gWSshutdown = (LPFN_SHUTDOWN) GetProcAddress(gWinsockLib,_T("shutdown"));
			gWSclosesocket = (LPFN_CLOSESOCKET) GetProcAddress(gWinsockLib,_T("closesocket"));

			if(startup == NULL ||
				gWSACleanup == NULL ||
				gWSgethostbyname == NULL ||
				gWSinet_addr == NULL ||
				gWSgethostbyaddr == NULL ||
				gWShtons == NULL ||
				gWSsocket == NULL ||
				gWSconnect == NULL ||
				gWSsend == NULL ||
				gWSrecv == NULL ||
				gWSshutdown == NULL ||
				gWSclosesocket == NULL)
			{
				gWSACleanup = NULL;
				Cleanup();
				ThrowXML(NetAccessorException, XMLExcepts::NetAcc_InitFailed);
			}
		}
	}
    wVersionRequested = MAKEWORD( 2, 2 );
    int err = (*startup)(wVersionRequested, &wsaData);
    if (err != 0)
    {
        // Call WSAGetLastError() to get the last error.
        ThrowXML(NetAccessorException, XMLExcepts::NetAcc_InitFailed);
    }
    fInitialized = true;
}

void XSECBinHTTPURIInputStream::Cleanup() {
	if(fInitialized)
	{
		if(gWSACleanup) (*gWSACleanup)();
		gWSACleanup = NULL;
		FreeLibrary(gWinsockLib);
		gWinsockLib = NULL;
		gWSgethostbyname = NULL;
		gWSinet_addr = NULL;
		gWSgethostbyaddr = NULL;
		gWShtons = NULL;
		gWSsocket = NULL;
		gWSconnect = NULL;
		gWSsend = NULL;
		gWSrecv = NULL;
		gWSshutdown = NULL;
		gWSclosesocket = NULL;

      fInitialized = false;
      delete fInitMutex;
      fInitMutex = 0;
	}
}


hostent* XSECBinHTTPURIInputStream::gethostbyname(const char* name)
{
	return (*gWSgethostbyname)(name);
}

unsigned long XSECBinHTTPURIInputStream::inet_addr(const char* cp)
{
	return (*gWSinet_addr)(cp);
}

hostent* XSECBinHTTPURIInputStream::gethostbyaddr(const char* addr,int len,int type)
{
	return (*gWSgethostbyaddr)(addr,len,type);
}

unsigned short XSECBinHTTPURIInputStream::htons(unsigned short hostshort)
{
	return (*gWShtons)(hostshort);
}

unsigned short XSECBinHTTPURIInputStream::socket(int af,int type,int protocol)
{
	return (unsigned short) (*gWSsocket)(af,type,protocol);
}

int XSECBinHTTPURIInputStream::connect(unsigned short s,const sockaddr* name,int namelen)
{
	return (*gWSconnect)(s,name,namelen);
}

int XSECBinHTTPURIInputStream::send(unsigned short s,const char* buf,int len,int flags)
{
	return (*gWSsend)(s,buf,len,flags);
}

int XSECBinHTTPURIInputStream::recv(unsigned short s,char* buf,int len,int flags)
{
	return (*gWSrecv)(s,buf,len,flags);
}

int XSECBinHTTPURIInputStream::shutdown(unsigned int s,int how)
{
	return (*gWSshutdown)(s,how);
}

int XSECBinHTTPURIInputStream::closesocket(unsigned int socket)
{
	return (*gWSclosesocket)(socket);
}

unsigned int XSECBinHTTPURIInputStream::getSocketHandle(const XMLUri&  urlSource) {

    //
    // Pull all of the parts of the URL out of th urlSource object, and transcode them
    //   and transcode them back to ASCII.
    //
    const XMLCh*        hostName = urlSource.getHost();
    XSECAutoPtrChar     hostNameAsCharStar(hostName);

    const XMLCh*        path = urlSource.getPath();
    XSECAutoPtrChar     pathAsCharStar(path);

    const XMLCh*        fragment = urlSource.getFragment();
    XSECAutoPtrChar     fragmentAsCharStar(fragment);

    const XMLCh*        query = urlSource.getQueryString();
    XSECAutoPtrChar     queryAsCharStar(query);

    unsigned short      portNumber = (unsigned short) urlSource.getPort();

	// If no number is set, go with port 80
	if (portNumber == USHRT_MAX)
		portNumber = 80;

    //
    // Set up a socket.
    //
    struct hostent*     hostEntPtr = 0;
    struct sockaddr_in  sa;


    if ((hostEntPtr = gethostbyname(hostNameAsCharStar.get())) == NULL)
    {
        unsigned long  numAddress = inet_addr(hostNameAsCharStar.get());
        if (numAddress == INADDR_NONE)
        {
            // Call WSAGetLastError() to get the error number.
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported resolving IP address");
        }
        if ((hostEntPtr =
                gethostbyaddr((const char *) &numAddress,
                              sizeof(unsigned long), AF_INET)) == NULL)
        {
            // Call WSAGetLastError() to get the error number.
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported resolving IP address");
        }
    }

    memcpy((void *) &sa.sin_addr,
           (const void *) hostEntPtr->h_addr, hostEntPtr->h_length);
    sa.sin_family = hostEntPtr->h_addrtype;
    sa.sin_port = htons(portNumber);

    SOCKET s = socket(hostEntPtr->h_addrtype, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported creating socket");
    }

    if (connect((unsigned short) s, (struct sockaddr *) &sa, sizeof(sa)) == SOCKET_ERROR)
    {
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported connecting to socket");
    }


    // Set a flag so we know that the headers have not been read yet.
    bool fHeaderRead = false;

    // The port is open and ready to go.
    // Build up the http GET command to send to the server.
    // To do:  We should really support http 1.1.  This implementation
    //         is weak.

    memset(fBuffer, 0, sizeof(fBuffer));

    strcpy(fBuffer, "GET ");
    strcat(fBuffer, pathAsCharStar.get());

    if (queryAsCharStar.get() != 0)
    {
        // Tack on a ? before the fragment
        strcat(fBuffer,"?");
        strcat(fBuffer, queryAsCharStar.get());
    }

    if (fragmentAsCharStar.get() != 0)
    {
        strcat(fBuffer, fragmentAsCharStar.get());
    }
    strcat(fBuffer, " HTTP/1.0\r\n");


    strcat(fBuffer, "Host: ");
    strcat(fBuffer, hostNameAsCharStar.get());
    if (portNumber != 80)
    {
        strcat(fBuffer, ":");
        int i = (int) strlen(fBuffer);
        _itoa(portNumber, fBuffer+i, 10);
    }
    strcat(fBuffer, "\r\n\r\n");

    // Send the http request
    int lent = (int) strlen(fBuffer);
    int  aLent = 0;
    if ((aLent = send((unsigned short) s, fBuffer, lent, 0)) != lent)
    {
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported writing to socket");
    }


    //
    // get the response, check the http header for errors from the server.
    //
    memset(fBuffer, 0, sizeof(fBuffer));
    aLent = recv((unsigned short) s, fBuffer, sizeof(fBuffer)-1, 0);
    if (aLent == SOCKET_ERROR || aLent == 0)
    {
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
    }

    fBufferEnd = fBuffer+aLent;
    *fBufferEnd = 0;

    do {
        // Find the break between the returned http header and any data.
        //  (Delimited by a blank line)
        // Hang on to any data for use by the first read from this XSECBinHTTPURIInputStream.
        //
        fBufferPos = strstr(fBuffer, "\r\n\r\n");
        if (fBufferPos != 0)
        {
            fBufferPos += 4;
            *(fBufferPos-2) = 0;
            fHeaderRead = true;
        }
        else
        {
            fBufferPos = strstr(fBuffer, "\n\n");
            if (fBufferPos != 0)
            {
                fBufferPos += 2;
                *(fBufferPos-1) = 0;
                fHeaderRead = true;
            }
            else
            {
                //
                // Header is not yet read, do another recv() to get more data...
                aLent = (unsigned int) recv((unsigned short) s, fBufferEnd, ((int) sizeof(fBuffer) - 1) - (int) (fBufferEnd - fBuffer), 0);
                if (aLent == SOCKET_ERROR || aLent == 0)
                {
                    // Call WSAGetLastError() to get the error number.
			        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
                }
                fBufferEnd = fBufferEnd + aLent;
                *fBufferEnd = 0;
            }
        }
    } while(fHeaderRead == false);

    // Make sure the header includes an HTTP 200 OK response.
    //
    char *p = strstr(fBuffer, "HTTP");
    if (p == 0)
    {
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
    }

    p = strchr(p, ' ');
    if (p == 0)
    {
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
    }

    int httpResponse = atoi(p);

    // Check for redirect or permanently moved
    if (httpResponse == 302 || httpResponse == 301)
    {
        //Once grows, should use a switch
        char redirectBuf[256];
        int q;

        // Find the "Location:" string
        p = strstr(p, "Location:");
        if (p == 0)
        {
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
        }
        p = strchr(p, ' ');
		if (p == 0)
		{
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
		}

		// Now read
		p++;
		for (q=0; q < 255 && p[q] != '\r' && p[q] !='\n'; ++q)
			redirectBuf[q] = p[q];

		redirectBuf[q] = '\0';

		// Try to find this location
		XSECAutoPtrXMLCh redirectBufTrans(redirectBuf);

		return getSocketHandle(XMLUri(redirectBufTrans.get()));
	}
    else if (httpResponse != 200)
    {
        // Most likely a 404 Not Found error.
        //   Should recognize and handle the forwarding responses.
        //
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Unknown HTTP response received");
    }

    return (unsigned int) s;
}


XSECBinHTTPURIInputStream::XSECBinHTTPURIInputStream(const XMLUri& urlSource)
      : fSocketHandle(0)
      , fBytesProcessed(0)
{
    if(!fInitialized)
    {
        if (!fInitMutex)
        {
            XMLMutex* tmpMutex = new XMLMutex;
            if (compareAndSwap((void**)&fInitMutex, tmpMutex, 0))
            {
                // Someone beat us to it, so let's clean up ours
                delete tmpMutex;
            }
         }
         XMLMutexLock lock(fInitMutex);
         if (!fInitialized)
         {
             Initialize();
         }
    }

	fSocketHandle = getSocketHandle(urlSource);

}

void XSECBinHTTPURIInputStream::ExternalInitialize(void) {

    if(!fInitialized)
    {
        if (!fInitMutex)
        {
            XMLMutex* tmpMutex = new XMLMutex;
            if (compareAndSwap((void**)&fInitMutex, tmpMutex, 0))
            {
                // Someone beat us to it, so let's clean up ours
                delete tmpMutex;
            }
         }
         XMLMutexLock lock(fInitMutex);
         if (!fInitialized)
         {
             Initialize();
         }
    }
}


XSECBinHTTPURIInputStream::~XSECBinHTTPURIInputStream()
{
    shutdown(fSocketHandle, SD_BOTH);
    closesocket(fSocketHandle);
}


//
//  readBytes
//
xsecsize_t XSECBinHTTPURIInputStream::readBytes(XMLByte* const    toFill
                                    , const xsecsize_t    maxToRead)
{

    xsecsize_t len = (xsecsize_t) (fBufferEnd - fBufferPos);
    if (len > 0)
    {
        // If there's any data left over in the buffer into which we first
        //   read from the server (to get the http header), return that.
        if (len > maxToRead)
            len = maxToRead;
        memcpy(toFill, fBufferPos, len);
        fBufferPos += len;
    }
    else
    {
        // There was no data in the local buffer.
        // Read some from the socket, straight into our caller's buffer.
        //
        len = recv(fSocketHandle, (char *) toFill, maxToRead, 0);
        if (len == SOCKET_ERROR)
        {
            // Call WSAGetLastError() to get the error number.
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported reading socket");
        }
    }

    fBytesProcessed += len;
    return len;
}

#ifdef XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE
const XMLCh* XSECBinHTTPURIInputStream::getContentType() const {
    return NULL;
}
#endif
