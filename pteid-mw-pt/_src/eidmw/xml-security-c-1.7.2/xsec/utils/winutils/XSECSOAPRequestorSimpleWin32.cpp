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
 * XSECSOAPRequestorSimpleWin32 := (Very) Basic implementation of a SOAP
 *                         HTTP wrapper for testing the client code.
 *
 *
 * $Id: XSECSOAPRequestorSimpleWin32.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */


#include <xsec/utils/winutils/XSECBinHTTPURIInputStream.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/utils/XSECSafeBuffer.hpp>
#include <xsec/utils/XSECSOAPRequestorSimple.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/xkms/XKMSConstants.hpp>

#include "../../utils/XSECAutoPtr.hpp"

#define _WINSOCKAPI_

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------


XSECSOAPRequestorSimple::XSECSOAPRequestorSimple(const XMLCh * uri) : m_uri(uri) {

	XSECBinHTTPURIInputStream::ExternalInitialize();
	m_envelopeType = ENVELOPE_SOAP11;

}


// --------------------------------------------------------------------------------
//           Interface
// --------------------------------------------------------------------------------


DOMDocument * XSECSOAPRequestorSimple::doRequest(DOMDocument * request) {


	char * content = wrapAndSerialise(request);

	// First we need to serialise

    char                fBuffer[4000];
    char *              fBufferEnd;
    char *              fBufferPos;

    //
    // Pull all of the parts of the URL out of th m_uri object, and transcode them
    //   and transcode them back to ASCII.
    //
    const XMLCh*        hostName = m_uri.getHost();
    XSECAutoPtrChar     hostNameAsCharStar(hostName);

    const XMLCh*        path = m_uri.getPath();
    XSECAutoPtrChar     pathAsCharStar(path);

    const XMLCh*        fragment = m_uri.getFragment();
    XSECAutoPtrChar     fragmentAsCharStar(fragment);

    const XMLCh*        query = m_uri.getQueryString();
    XSECAutoPtrChar     queryAsCharStar(query);

    unsigned short      portNumber = (unsigned short) m_uri.getPort();

	// If no number is set, go with port 80
	if (portNumber == USHRT_MAX)
		portNumber = 80;

    //
    // Set up a socket.
    //
    struct hostent*     hostEntPtr = 0;
    struct sockaddr_in  sa;


    if ((hostEntPtr = XSECBinHTTPURIInputStream::gethostbyname(hostNameAsCharStar.get())) == NULL)
    {
        unsigned long  numAddress = XSECBinHTTPURIInputStream::inet_addr(hostNameAsCharStar.get());
        if (numAddress == INADDR_NONE)
        {
            XSEC_RELEASE_XMLCH(content);
            // Call WSAGetLastError() to get the error number.
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported resolving IP address");
        }
        if ((hostEntPtr =
                XSECBinHTTPURIInputStream::gethostbyaddr((const char *) &numAddress,
                              sizeof(unsigned long), AF_INET)) == NULL)
        {
            XSEC_RELEASE_XMLCH(content);
            // Call WSAGetLastError() to get the error number.
	        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported resolving IP address");
        }
    }

    memcpy((void *) &sa.sin_addr,
           (const void *) hostEntPtr->h_addr, hostEntPtr->h_length);
    sa.sin_family = hostEntPtr->h_addrtype;
    sa.sin_port = XSECBinHTTPURIInputStream::htons(portNumber);

    SOCKET s = XSECBinHTTPURIInputStream::socket(hostEntPtr->h_addrtype, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
    {
        XSEC_RELEASE_XMLCH(content);
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported creating socket");
    }

    if (XSECBinHTTPURIInputStream::connect((unsigned short) s, 
		(struct sockaddr *) &sa, sizeof(sa)) == SOCKET_ERROR)
    {
        XSEC_RELEASE_XMLCH(content);
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported connecting to socket");
    }


    // Set a flag so we know that the headers have not been read yet.
    bool fHeaderRead = false;

    // The port is open and ready to go.
    // Build up the http POST command to send to the server.
    // To do:  We should really support http 1.1.  This implementation
    //         is weak.

    memset(fBuffer, 0, sizeof(fBuffer));

    strcpy(fBuffer, "POST ");
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

	strcat(fBuffer, "Content-Type: text/xml; charset=utf-8\r\n");


    strcat(fBuffer, "Host: ");
    strcat(fBuffer, hostNameAsCharStar.get());
    if (portNumber != 80)
    {
        strcat(fBuffer, ":");
        int i = (int) strlen(fBuffer);
        _itoa(portNumber, fBuffer+i, 10);
    }
	strcat(fBuffer, "\r\n");

	strcat(fBuffer, "Content-Length: ");
    int i = (int) strlen(fBuffer);
    _itoa((int) strlen(content), fBuffer+i, 10);
	strcat(fBuffer, "\r\n");
	strcat(fBuffer, "SOAPAction: \"\"\r\n");

/*	strcat(fBuffer, "Connection: Close\r\n");
	strcat(fBuffer, "Cache-Control: no-cache\r\n");*/
    strcat(fBuffer, "\r\n");

	// Now the content
//	strcat(fBuffer, content);

    // Send the http request
    int lent = (int) strlen(fBuffer);
    int  aLent = 0;
    if ((aLent = XSECBinHTTPURIInputStream::send((unsigned short) s, 
		fBuffer, lent, 0)) != lent)
    {
        XSEC_RELEASE_XMLCH(content);
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported writing to socket");
    }

	// Now the content
    lent = (int) strlen(content);
    aLent = 0;
    if ((aLent = XSECBinHTTPURIInputStream::send((unsigned short) s, 
		content, lent, 0)) != lent)
    {
        XSEC_RELEASE_XMLCH(content);
        // Call WSAGetLastError() to get the error number.
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Error reported writing to socket");
    }
    XSEC_RELEASE_XMLCH(content);

    //
    // get the response, check the http header for errors from the server.
    //
    memset(fBuffer, 0, sizeof(fBuffer));
    aLent = XSECBinHTTPURIInputStream::recv((unsigned short) s, fBuffer, sizeof(fBuffer)-1, 0);
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
                aLent = XSECBinHTTPURIInputStream::recv((unsigned short) s, fBufferEnd, ((int) sizeof(fBuffer) - 1) - (int)(fBufferEnd - fBuffer), 0);
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
							"XSECSOAPRequestorSimple - Error reported reading socket");
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
		m_uri = XMLUri(XMLString::transcode(redirectBuf));

		return doRequest(request);

	}
    else if (httpResponse != 200)
    {
        // Most likely a 404 Not Found error.
        //   Should recognize and handle the forwarding responses.
        //
		char * q = strstr(p, "\n");
		if (q == NULL)
			q = strstr(p, "\r");
		if (q != NULL)
			*q = '\0';
		safeBuffer sb;
		sb.sbStrcpyIn("SOAPRequestorSimple HTTP Error : ");
		if (strlen(p) < 256)
			sb.sbStrcatIn(p);
        throw XSECException(XSECException::HTTPURIInputStreamError, sb.rawCharBuffer());
    }

	/* Now find out how long the return is */

	p = strstr(fBuffer, "Content-Length:");
	int responseLength;

	if (p == NULL) {
		// Need to work it out from the amount of data returned
		responseLength = -1;
	}
	else {

		p = strchr(p, ' ');
		p++;

		responseLength = atoi(p);
	}

	safeBuffer responseBuffer;
	lent = (int) (fBufferEnd - fBufferPos);
	responseBuffer.sbMemcpyIn(fBufferPos, lent);

	while (responseLength == -1 || lent < responseLength) {
		aLent = XSECBinHTTPURIInputStream::recv((unsigned short)s, fBuffer, sizeof(fBuffer)-1, 0);
		if (aLent > 0) {
			responseBuffer.sbMemcpyIn(lent, fBuffer, aLent);
			lent += aLent;
		}
		else {
			responseLength = 0;
		}
	}

	if (lent <= 0) {
        throw XSECException(XSECException::HTTPURIInputStreamError,
			"XSECSOAPRequestorSimple - Zero length response from server");
	}

	return parseAndUnwrap(responseBuffer.rawCharBuffer(), lent);


# if 0

	p = strstr(fBuffer, "Content-Length:");
	if (p == NULL) {
        throw XSECException(XSECException::HTTPURIInputStreamError,
							"Content-Length required in SOAP HTTP Response");
	}

	p = strchr(p, ' ');
	p++;

	int responseLength = atoi(p);

	char * responseBuffer;
	XSECnew(responseBuffer, char[responseLength]);
	ArrayJanitor<char> j_responseBuffer(responseBuffer);

	lent = fBufferEnd - fBufferPos;
	memcpy(responseBuffer, fBufferPos, lent);
	while (lent < responseLength) {
	    aLent = XSECBinHTTPURIInputStream::recv((unsigned short) s, &responseBuffer[lent], responseLength - lent, 0);
		lent += aLent;
	}
	
    return parseAndUnwrap(responseBuffer, responseLength);
#endif
}



