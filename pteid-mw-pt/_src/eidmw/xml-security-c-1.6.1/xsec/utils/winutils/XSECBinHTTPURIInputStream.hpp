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
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECBinHTTPURIInputStream.hpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 * $Log$
 * Revision 1.7  2005/02/03 13:56:22  milan
 * Apache licence fix.
 *
 * Revision 1.6  2004/04/21 10:14:49  blautenb
 * Clean windows build with overarching SimpleSOAPRequestor
 *
 * Revision 1.5  2004/04/16 12:07:23  blautenb
 * Skeleton code for XKMS MessageFactory
 *
 * Revision 1.4  2004/02/08 10:25:40  blautenb
 * Convert to Apache 2.0 license
 *
 * Revision 1.3  2003/09/11 11:11:05  blautenb
 * Cleaned up usage of Xerces namespace - no longer inject into global namespace in headers
 *
 * Revision 1.2  2003/07/05 10:30:38  blautenb
 * Copyright update
 *
 * Revision 1.1  2003/02/12 09:45:29  blautenb
 * Win32 Re-implementation of Xerces URIResolver to support re-directs
 *
 *
 */


#ifndef XSECBINHTTPURIINPUTSTREAM_HEADER
#define XSECBINHTTPURIINPUTSTREAM_HEADER

#include <xsec/framework/XSECDefs.hpp>

#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/Mutexes.hpp>

//
// This class implements the BinInputStream interface specified by the XML
// parser.
//

struct hostent;
struct sockaddr;

class XSECSOAPRequestorSimple;

class DSIG_EXPORT XSECBinHTTPURIInputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream
{
public :

    XSECBinHTTPURIInputStream(const XERCES_CPP_NAMESPACE_QUALIFIER XMLUri&  urlSource);
    ~XSECBinHTTPURIInputStream();

#ifdef XSEC_XERCES_64BITSAFE
    XMLFilePos curPos() const;
#else
    unsigned int curPos() const;
#endif
    xsecsize_t readBytes(XMLByte* const  toFill, const xsecsize_t    maxToRead);

#ifdef XSEC_XERCES_INPUTSTREAM_HAS_CONTENTTYPE
    const XMLCh* getContentType() const;
#endif

	static void Cleanup();

	friend class XSECSOAPRequestorSimple;

protected:

	/*
	 * These are called by other classes that use the loaded DLL
	 *
	 * Actually - this is cheating of the worst kind, but it
	 * provides a quick way to make these calls available outside the library
	 */

	static void ExternalInitialize(void);

	static hostent* gethostbyname(const char* name);
	static unsigned long inet_addr(const char* cp);
	static hostent* gethostbyaddr(const char* addr,int len,int type);
	static unsigned short htons(unsigned short hostshort);
	static unsigned short socket(int af,int type,int protocol);
	static int connect(unsigned short s,const sockaddr* name,int namelen);
	static int send(unsigned short s,const char* buf,int len,int flags);
	static int recv(unsigned short s,char* buf,int len,int flags);
	static int shutdown(unsigned int s,int how);
	static int closesocket(unsigned int socket);


private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSocketHandle
    //      The socket representing the connection to the remote file.
    //      We deliberately did not define the type to be SOCKET, so as to
    //      avoid bringing in any Windows header into this file.
    //  fBytesProcessed
    //      Its a rolling count of the number of bytes processed off this
    //      input stream.
    //  fBuffer
    //      Holds the http header, plus the first part of the actual
    //      data.  Filled at the time the stream is opened, data goes
    //      out to user in response to readBytes().
    //  fBufferPos, fBufferEnd
    //      Pointers into fBuffer, showing start and end+1 of content
    //      that readBytes must return.
    // -----------------------------------------------------------------------

    unsigned int        fSocketHandle;
    xsecsize_t          fBytesProcessed;
    char                fBuffer[4000];
    char *              fBufferEnd;
    char *              fBufferPos;
    static bool         fInitialized;
    static XERCES_CPP_NAMESPACE_QUALIFIER XMLMutex*    fInitMutex;

	static void Initialize();
	unsigned int getSocketHandle(const XERCES_CPP_NAMESPACE_QUALIFIER XMLUri&  urlSource);

};


inline
#ifdef XSEC_XERCES_64BITSAFE
XMLFilePos
#else
unsigned int
#endif
XSECBinHTTPURIInputStream::curPos() const
{
    return fBytesProcessed;
}


#endif // XSECBINHTTPURIINPUTSTREAM_HEADER
