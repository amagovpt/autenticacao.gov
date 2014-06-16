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
 * txfmout:= tool to output the results of the various transforms
 *		     used when validating the attached signature.
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: txfmout.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

// XSEC

#include <xsec/utils/XSECPlatformUtils.hpp>
#include <xsec/framework/XSECProvider.hpp>
#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/dsig/DSIGReference.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/enc/XSECCryptoException.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/utils/XSECBinTXFMInputStream.hpp>

#if defined(_WIN32)
#include <xsec/utils/winutils/XSECURIResolverGenericWin32.hpp>
#else
#include <xsec/utils/unixutils/XSECURIResolverGenericUnix.hpp>
#endif

// General

#include <memory.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
# define _MAX_PATH PATH_MAX
#else
# if defined(HAVE_DIRECT_H)
#  include <direct.h>
# endif
#endif


#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLNetAccessor.hpp>
#include <xercesc/util/XMLUri.hpp>

XERCES_CPP_NAMESPACE_USE

using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;

#ifndef XSEC_NO_XALAN

// XALAN

#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>

XALAN_USING_XALAN(XPathEvaluator)
XALAN_USING_XALAN(XalanTransformer)

#endif

#ifdef XSEC_NO_XALAN

std::ostream& operator<< (std::ostream& target, const XMLCh * s)
{
    char *p = XMLString::transcode(s);
    target << p;
    XSEC_RELEASE_XMLCH(p);
    return target;
}

#endif

// ---------------------------------------------------------------------------
//  Outputter
// ---------------------------------------------------------------------------

class outputter {

public:

	outputter();
	~outputter();

	// Set methods

	// Will tell not to use cout and open on this base
	void setFilename(const char * name);
	// Will append a number and re-open for each "open" call
	void setNewFilePerOpen();

	// Re-open the file if necessary (new output)
	void openSection();
	// Close if necessary (output section finished)
	void closeSection();
	// Close of and finish
	void closeAll();

	// Output a buffer
	void output(const unsigned char * buf, unsigned int sz);

	// Info
	int getIndex(void);

private:

	char			* m_name;			// Name of the file (or base name)
	bool			m_cout;				// Are we using cout?
	bool			m_newFilePerOpen;	// Should we re-open?
	bool			m_fileOpen;			// Do we have an open file we should close?
	int				m_counter;			// The counter
	ofstream		m_out;				// Current output file

};

outputter::outputter() :
m_name(0),
m_cout(true),
m_newFilePerOpen(false),
m_fileOpen(false),
m_counter(0) {

}

outputter::~outputter() {

	if (m_fileOpen == true) {

		m_out.close();

		m_fileOpen = false;

	}

	if (m_name != 0)
		delete[] m_name;

}

void outputter::setFilename(const char * name) {

	m_name = strdup(name);
	m_cout = false;

}

void outputter::setNewFilePerOpen() {

	m_newFilePerOpen = true;

}

void outputter::openSection() {

	if (m_cout == true)
		return;

	if (m_fileOpen == true && m_newFilePerOpen == false)
		return;

	if (m_out.is_open() != 0) {

		m_out.close();

	}

	char * buf = new char[strlen(m_name) + 10];
	strcpy(buf, m_name);

	if (m_newFilePerOpen == true) {

		char numBuf[10];
		sprintf(numBuf, "%d", m_counter);
		//_itoa(m_counter, numBuf, 10);
		strcat(buf, ".");
		strcat(buf, numBuf);

	}

	m_out.open(buf, ios::out | ios::binary);
	m_fileOpen = true;
	delete [] buf;
}

void outputter::closeSection() {

	m_counter++;

	if (m_cout == false && m_newFilePerOpen == true && m_out.is_open() != 0) {

		m_out.close();
		m_fileOpen = false;

	}


}

int outputter::getIndex(void) {

	return m_counter;

}

void outputter::closeAll() {

	if (m_out.is_open() != 0)
		m_out.close();

	m_fileOpen = false;

}


void outputter::output(const unsigned char * buf, unsigned int sz) {

	if (m_cout || m_out.is_open() == false) {

		cout.write((const char *) buf,sz);

	}
	else {

		m_out.write((const char *) buf, sz);

	}

}


// ---------------------------------------------------------------------------
//  Main Program
// ---------------------------------------------------------------------------



void printUsage(void) {

	cerr << "\nUsage: txfmout [options] <input file name>\n\n";
	cerr << "     Where options are :\n\n";
	cerr << "     --signedinfo/-s\n";
	cerr << "         Output canonicalised SignedInfo only\n";
	cerr << "     --out/-o\n";
	cerr << "         Output to the nominated file name\n";
	cerr << "     --references/-r [num]\n";
	cerr << "         Output only references. [num] defines a single reference to output\n";
	cerr << "     --newfiles/-n\n";
	cerr << "         Create a new file for each reference/SignedInfo (append .#)\n";

}

// ---------------------------------------------------------------------------
//		Reference Outputter
// ---------------------------------------------------------------------------

void outputReferenceList (DSIGReferenceList * lst, outputter & theOutputter, int refNum) {

	if (lst == 0)
		return;
	
	DSIGReference * ref;
	unsigned int sz;
	XSECBinTXFMInputStream * is;
	unsigned char buf[1024];

    DSIGReferenceList::size_type lstSz = (int) lst->getSize();

	for (DSIGReferenceList::size_type i = 0; i < lstSz; ++i) {

		ref = lst->item(i);
		if (refNum == -1 || theOutputter.getIndex() == refNum) {
			theOutputter.openSection();
	
			try {
				is = ref->makeBinInputStream();
			}
			catch (NetAccessorException e) {

				cerr << "Network error in reference " << theOutputter.getIndex() << endl;
				is = 0;
				
			}


			if (is != 0) {

				sz = (unsigned int) is->readBytes(buf, 1023);

				while (sz != 0) {
					
					buf[sz] = '\0';
					theOutputter.output(buf, sz);

					sz = (unsigned int) is->readBytes(buf, 1023);

				}

				delete is;

			}
		}
		theOutputter.closeSection();

	}

	// Look for manifests
	for (DSIGReferenceList::size_type i = 0; i < lstSz; ++i) {

		ref = lst->item(i);

		if (ref->isManifest() == true) {
			outputReferenceList(ref->getManifestReferenceList(), theOutputter, refNum);
		}

	}

}

// ---------------------------------------------------------------------------
//  Main Program
// ---------------------------------------------------------------------------


int main(int argc, char **argv) {

	char					* filename = NULL;
	bool					signedInfo = true;
	bool					references = true;
	outputter				theOutputter;
	int						refNum = -1;

	if (argc < 2) {

		printUsage();
		exit (2);
	}

	// Run through parameters
	int paramCount = 1;

	while (paramCount < argc - 1) {

		if (_stricmp(argv[paramCount], "--signedinfo") == 0 || _stricmp(argv[paramCount], "-s") == 0) {
			paramCount++;
			references = false;
		}
		else if (_stricmp(argv[paramCount], "--out") == 0 || _stricmp(argv[paramCount], "-o") == 0) {
			paramCount++;
			theOutputter.setFilename(argv[paramCount++]);
		}
		else if (_stricmp(argv[paramCount], "--references") == 0 || _stricmp(argv[paramCount], "-r") == 0) {
			paramCount++;
			signedInfo = false;
			if (argv[paramCount][0] >= '0' && argv[paramCount][0] <= '9')
				refNum = atoi(argv[paramCount++]);
		}
		else if (_stricmp(argv[paramCount], "--newfiles") == 0 || _stricmp(argv[paramCount], "-n") == 0) {
			paramCount++;
			theOutputter.setNewFilePerOpen();
		}
		else {
			printUsage();
			exit(2);
		}
	}

	if (paramCount >= argc) {
		printUsage();
		exit (2);
	}

	filename = argv[paramCount];

	// Initialise the XML system

	try {

		XMLPlatformUtils::Initialize();
#ifndef XSEC_NO_XALAN
		XPathEvaluator::initialize();
		XalanTransformer::initialize();
#endif
		XSECPlatformUtils::Initialise();

	}
	catch (const XMLException &e) {

		cerr << "Error during initialisation of Xerces" << endl;
		cerr << "Error Message = : "
		     << e.getMessage() << endl;

	}

	// Create and set up the parser

	XercesDOMParser * parser = new XercesDOMParser;
	
	parser->setDoNamespaces(true);
	parser->setCreateEntityReferenceNodes(true);

	// Now parse out file

	bool errorsOccured = false;
	xsecsize_t errorCount = 0;
    try
    {
    	parser->parse(filename);
        errorCount = parser->getErrorCount();
        if (errorCount > 0)
            errorsOccured = true;
    }

    catch (const XMLException& e)
    {
        cerr << "An error occured during parsing\n   Message: "
             << e.getMessage() << endl;
        errorsOccured = true;
    }


    catch (const DOMException& e)
    {
       cerr << "A DOM error occured during parsing\n   DOMException code: "
             << e.code << endl;
        errorsOccured = true;
    }

	if (errorsOccured) {

		cout << "Errors during parse" << endl;
		exit (2);

	}

	/*

		Now that we have the parsed file, get the DOM document and start looking at it

	*/
	
	DOMNode *doc;		// The document that we parsed

	doc = parser->getDocument();
	DOMDocument *theDOM = parser->getDocument();

	// Find the signature node
	
	DOMNode *sigNode = findDSIGNode(doc, "Signature");

	// Create the signature checker

	if (sigNode == 0) {

		cerr << "Could not find <Signature> node in " << argv[argc-1] << endl;
		exit(2);
	}

	XSECProvider prov;
	DSIGSignature * sig = prov.newSignatureFromDOM(theDOM, sigNode);
	sig->registerIdAttributeName(MAKE_UNICODE_STRING("ID"));

#if defined(_WIN32)
	XSECURIResolverGenericWin32 
#else
	XSECURIResolverGenericUnix 
#endif
		theResolver;
		 
	// Map out base path of the file
	char path[_MAX_PATH];
	char baseURI[(_MAX_PATH * 2) + 10];
	getcwd(path, _MAX_PATH);

	strcpy(baseURI, "file:///");
	strcat(baseURI, path);
	strcat(baseURI, "/");
	strcat(baseURI, filename);

	// Find any ':' and "\" characters
	int lastSlash = 0;
	for (unsigned int i = 8; i < strlen(baseURI); ++i) {
		if (baseURI[i] == '\\') {
			lastSlash = i;
			baseURI[i] = '/';
		}
		else if (baseURI[i] == '/')
			lastSlash = i;
	}

	// The last "\\" must prefix the filename
	baseURI[lastSlash + 1] = '\0';

	theResolver.setBaseURI(MAKE_UNICODE_STRING(baseURI));

	sig->setURIResolver(&theResolver);


	try {

		XSECBinTXFMInputStream * is;
		XMLByte buf[1024];
		unsigned int sz;

		sig->load();
		if (references) {
			outputReferenceList(sig->getReferenceList(), theOutputter, refNum);

		}
		if (signedInfo) {
			is = sig->makeBinInputStream();
			if (is != NULL) {

				theOutputter.openSection();
				sz = (unsigned int) is->readBytes(buf, 1023);

				while (sz != 0) {
					
					buf[sz] = '\0';
					theOutputter.output(buf, sz);
	
					sz = (unsigned int) is->readBytes(buf, 1023);

				}
				theOutputter.closeSection();

				delete is;

			}
		}
	}

	catch (XSECException &e) {
		char * m = XMLString::transcode(e.getMsg());
		cerr << "An error occured during signature processing\n   Message: "
		<< m << endl;
		XSEC_RELEASE_XMLCH(m);
		errorsOccured = true;
		exit (2);
	}

	theOutputter.closeAll();

	prov.releaseSignature(sig);

	return 0;
}
