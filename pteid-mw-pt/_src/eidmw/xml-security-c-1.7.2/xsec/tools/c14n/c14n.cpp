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
 * c14n := tool to dump a XML file to the console after canonacalising it thru
 *			c14n
 *
 * $Id: c14n.cpp 1125514 2011-05-20 19:08:33Z scantor $
 *
 */

//XSEC includes



//#include <Include/PlatformDefinitions.hpp>
//#include <cassert>

#include <memory.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>

// XSEC

#include <xsec/canon/XSECC14n20010315.hpp>
#include <xsec/utils/XSECNameSpaceExpander.hpp>
#include <xsec/utils/XSECPlatformUtils.hpp>

XERCES_CPP_NAMESPACE_USE

using std::endl;
using std::cout;
using std::cerr;

void printUsage(void) {

	cerr << "\nUsage: c14n [-n] [-x] [-1.1] [-id ID] <input file name>\n";
	cerr << "       -n = No comments\n";
    cerr << "       -1.1 = Use c14n 1.1\n";
    cerr << "       -x = Use exclusive c14n\n";
	cerr << "       -id ID = References node to canonicalize by ID\n\n";

}

int main(int argc, char **argv) {

    const char* id = NULL;
	bool printComments = true;
    bool exclusive = false;
    bool inclusive11 = false;

	// Check arguments

	if (argc < 2) {
		printUsage();
		exit (1);
	}

	if (argc > 2) {
		for (int i = 1; i < argc - 1; ++i) {

			if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "-N"))
				printComments = false;
            else if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "-X"))
                exclusive = true;
            else if (!strcmp(argv[i], "-1.1"))
                inclusive11 = true;
            else if (!strcmp(argv[i], "-id") && argc > i + 1)
                id = argv[++i];
			else {
				cerr << "Unknown option %s\n\n";
				printUsage();
				exit (1);
			}
		}
	}

				

	// Initialise the XML system

	try {

		XMLPlatformUtils::Initialize();
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
    parser->setValidationScheme(XercesDOMParser::Val_Never);
	parser->setDoSchema(false);
	parser->setCreateEntityReferenceNodes(false);

	// Now parse out file

	bool errorsOccured = false;
	xsecsize_t errorCount = 0;
    try
    {
    	parser->parse(argv[argc-1]);
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
		exit (1);

	}

	/*

		Now that we have the parsed file, get the DOM document and start looking at it

	*/
	
	DOMNode *subtree = NULL;
	DOMDocument *theDOM = parser->getDocument();
    if (id) {
        XMLCh* temp = XMLString::transcode(id);
        subtree = theDOM->getElementById(temp);
        XSEC_RELEASE_XMLCH(temp);
        if (!subtree) {
            cerr << "ID reference did not resolve" << endl;
            exit(1);
        }
    }

	// Create the canonicalizer
    XSECC14n20010315* canon=NULL;
    if (subtree)
	    canon = new XSECC14n20010315(theDOM, subtree);
    else
        canon = new XSECC14n20010315(theDOM);
	canon->setCommentsProcessing(printComments);
	canon->setUseNamespaceStack(true);
    if (inclusive11)
        canon->setInclusive11();
    else if (exclusive)
        canon->setExclusive();

	// canon->XPathSelectNodes("(/descendant-or-self::node() | /descendant-or-self::node()/attribute::* | /descendant-or-self::node()/namespace::*)[ self::ietf:e1 or (parent::ietf:e1 and not(self::text() or self::e2)) or count (id(\"E3\") | ancestor-or-self::node()) = count (ancestor-or-self::node())]");

	char buffer[512];
	xsecsize_t res = canon->outputBuffer((unsigned char *) buffer, 128);


	while (res != 0) {
		buffer[res] = '\0';
		cout << buffer;
		res = canon->outputBuffer((unsigned char *) buffer, 128);
	}

	cout << endl;

    delete canon;
    delete parser;

    XSECPlatformUtils::Terminate();
	XMLPlatformUtils::Terminate();

	return 0;
}
