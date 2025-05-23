/*-****************************************************************************

 * Copyright (C) 2012, 2014, 2016-2024 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

//========================================================================
//
// PDFDoc.cc
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005, 2006, 2008 Brad Hards <bradh@frogmouth.net>
// Copyright (C) 2005, 2007-2009, 2011, 2012 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2008 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2008, 2010 Pino Toscano <pino@kde.org>
// Copyright (C) 2008, 2010, 2011 Carlos Garcia Campos <carlosgc@gnome.org>
// Copyright (C) 2009 Eric Toombs <ewtoombs@uwaterloo.ca>
// Copyright (C) 2009 Kovid Goyal <kovid@kovidgoyal.net>
// Copyright (C) 2009, 2011 Axel Struebing <axel.struebing@freenet.de>
// Copyright (C) 2010, 2011 Hib Eris <hib@hiberis.nl>
// Copyright (C) 2010 Jakub Wilk <ubanus@users.sf.net>
// Copyright (C) 2010 Ilya Gorenbein <igorenbein@finjan.com>
// Copyright (C) 2010 Srinivas Adicherla <srinivas.adicherla@geodesic.com>
// Copyright (C) 2010 Philip Lorenz <lorenzph+freedesktop@gmail.com>
// Copyright (C) 2011, 2012 Thomas Freitag <Thomas.Freitag@alfa.de>
// Copyright (C) 2012 Fabio D'Urso <fabiodurso@hotmail.it>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include <config.h>

#ifdef USE_GCC_PRAGMAS
#pragma implementation
#endif

#include <iostream>
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <memory>
#include <time.h>
#if defined(_DEBUG) && !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>

#endif
#include <sys/stat.h>
#include "goo/gstrtod.h"
#include "goo/GooString.h"
#include "poppler-config.h"
#include "Page.h"
#include "Catalog.h"
#include "Stream.h"
#include "XRef.h"
#include "Linearization.h"
#include "FlateEncoder.h"
#include "Link.h"
#include "Error.h"
#include "ErrorCodes.h"
#include "Lexer.h"
#include "Parser.h"
#include "SecurityHandler.h"
#include "Decrypt.h"
#ifndef DISABLE_OUTLINE
#include "Outline.h"
#endif
#include "PDFDoc.h"
#include "Hints.h"

extern "C"
{
//Definition of our own memmem(), generally its only available in Linux
//and Mac OSX Lion and later so it's better to just use our own implementation
POPPLER_API void *memmem(const void *haystack, size_t n, const void *needle, size_t m);
}

//------------------------------------------------------------------------

#define headerSearchSize 1024	// read this many bytes at beginning of
				//   file to look for '%PDF'
#define pdfIdLength 32   // PDF Document IDs (PermanentId, UpdateId) length

#define linearizationSearchSize 1024	// read this many bytes at beginning of
					// file to look for linearization
					// dictionary

#define xrefSearchSize 1024	// read this many bytes at end of file
				//   to look for 'startxref'


//------------------------------------------------------------------------
// PDFDoc
//------------------------------------------------------------------------

void PDFDoc::init()
{
  ok = gFalse;
  errCode = errNone;
  fileName = NULL;
  file = NULL;
  str = NULL;
  xref = NULL;
  linearization = NULL;
  catalog = NULL;
  hints = NULL;
#ifndef DISABLE_OUTLINE
  outline = NULL;
#endif
  startXRefPos = ~(Guint)0;
  secHdlr = NULL;
  pageCache = NULL;
  signature_mode = gFalse;
  m_image_data_jpeg = NULL;
  m_attribute_supplier = NULL;
  m_attribute_name = NULL;
  m_image_length = 0;
}

PDFDoc::PDFDoc()
{
  init();
}

PDFDoc::PDFDoc(GooString *fileNameA, GooString *ownerPassword,
	       GooString *userPassword, void *guiDataA) {
  Object obj;
  int size = 0;
#ifdef _WIN32
  int n, i;
#endif

  init();

  fileName = fileNameA;
  guiData = guiDataA;
#ifdef _WIN32
  n = fileName->getLength();
  fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
  for (i = 0; i < n; ++i) {
    fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
  }
  fileNameU[n] = L'\0';
#endif

  struct stat buf;
  if (stat(fileName->getCString(), &buf) == 0) {
     size = buf.st_size;
     this->fileSize = buf.st_size;
  }

  // try to open file
#ifdef VMS
  file = fopen(fileName->getCString(), "rb", "ctx=stm");
#else
  file = fopen(fileName->getCString(), "rb");
#endif
  if (file == NULL) {
    // fopen() has failed.
    // Keep a copy of the errno returned by fopen so that it can be 
    // referred to later.
    fopenErrno = errno;
    error(errIO, -1, "Couldn't open file '{0:t}': {0:s}.", fileName, strerror(errno));
    errCode = errOpenFile;
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, size, &obj);

  ok = setup(ownerPassword, userPassword);
}

#ifdef _WIN32
PDFDoc::PDFDoc(wchar_t *fileNameA, int fileNameLen, GooString *ownerPassword,
	       GooString *userPassword, void *guiDataA) {
  Object obj;
  int i;

  init();

  guiData = guiDataA;

  // save both Unicode and 8-bit copies of the file name
  fileName = new GooString();
  fileNameU = (wchar_t *)gmallocn(fileNameLen + 1, sizeof(wchar_t));
  for (i = 0; i < fileNameLen; ++i) {
    fileName->append((char)fileNameA[i]);
    fileNameU[i] = fileNameA[i];
  }
  fileNameU[fileNameLen] = L'\0';


  // try to open file
  struct _stat buf;
  int size;
   
  if (_wstat(fileNameU, &buf) == 0) {
      size = buf.st_size;
	  fileSize = size;
  }
  file = _wfopen(fileNameU, L"rb");
  
  if (!file) {
    error(errIO, -1, "Couldn't open file '{0:t}'", fileName);
    errCode = errOpenFile;
    return;
  }

  // create stream
  obj.initNull();
  str = new FileStream(file, 0, gFalse, size, &obj);

  ok = setup(ownerPassword, userPassword);
}
#endif

PDFDoc::PDFDoc(BaseStream *strA, GooString *ownerPassword,
	       GooString *userPassword, void *guiDataA) {
#ifdef _WIN32
  int n, i;
#endif

  init();
  guiData = guiDataA;
  if (strA->getFileName()) {
    fileName = strA->getFileName()->copy();
#ifdef _WIN32
    n = fileName->getLength();
    fileNameU = (wchar_t *)gmallocn(n + 1, sizeof(wchar_t));
    for (i = 0; i < n; ++i) {
      fileNameU[i] = (wchar_t)(fileName->getChar(i) & 0xff);
    }
    fileNameU[n] = L'\0';
#endif
  } else {
    fileName = NULL;
#ifdef _WIN32
    fileNameU = NULL;
#endif
  }
  str = strA;
  ok = setup(ownerPassword, userPassword);
}

GBool PDFDoc::setup(GooString *ownerPassword, GooString *userPassword) {
  str->setPos(0, -1);
  if (str->getPos() < 0)
  {
    error(errSyntaxError, -1, "Document base stream is not seekable");
    return gFalse;
  }

  str->reset();

  // check footer
  // Adobe does not seem to enforce %%EOF, so we do the same
//  if (!checkFooter()) return gFalse;
  
  // check header
  checkHeader();

  GBool wasReconstructed = false;

  // read xref table
  xref = new XRef(str, getStartXRef(), getMainXRefEntriesOffset(), &wasReconstructed);
  if (!xref->isOk()) {
    error(errSyntaxError, -1, "Couldn't read xref table");
    errCode = xref->getErrorCode();
    return gFalse;
  }

  // check for encryption
  if (!checkEncryption(ownerPassword, userPassword)) {
    errCode = errEncrypted;
    return gFalse;
  }

  // read catalog
  catalog = new Catalog(this);
  if (catalog && !catalog->isOk()) {
    if (!wasReconstructed)
    {
      // try one more time to contruct the Catalog, maybe the problem is damaged XRef 
      delete catalog;
      delete xref;
      xref = new XRef(str, 0, 0, NULL, true);
      catalog = new Catalog(this);
    }

    if (catalog && !catalog->isOk()) {
      error(errSyntaxError, -1, "Couldn't read page catalog");
      errCode = errBadCatalog;
      return gFalse;
    }
  }

  // done
  return gTrue;
}

PDFDoc::~PDFDoc() {
  if (pageCache) {
    for (int i = 0; i < getNumPages(); i++) {
      if (pageCache[i]) {
        delete pageCache[i];
      }
    }
    gfree(pageCache);
  }
  delete secHdlr;
#ifndef DISABLE_OUTLINE
  if (outline) {
    delete outline;
  }
#endif
  if (catalog) {
    delete catalog;
  }
  if (xref) {
    delete xref;
  }
  if (hints) {
    delete hints;
  }
  if (linearization) {
    delete linearization;
  }
  if (str) {
    delete str;
  }
  if (file) {
    fclose(file);
  }
  if (fileName) {
    delete fileName;
  }
#ifdef _WIN32
  if (fileNameU) {
    gfree(fileNameU);
  }
#endif
}


// Check for a %%EOF at the end of this stream
GBool PDFDoc::checkFooter() {
  // we look in the last 1024 chars because Adobe does the same
  char *eof = new char[1025];
  int pos = str->getPos();
  str->setPos(1024, -1);
  int i, ch;
  for (i = 0; i < 1024; i++)
  {
    ch = str->getChar();
    if (ch == EOF)
      break;
    eof[i] = ch;
  }
  eof[i] = '\0';

  bool found = false;
  for (i = i - 5; i >= 0; i--) {
    if (strncmp (&eof[i], "%%EOF", 5) == 0) {
      found = true;
      break;
    }
  }
  if (!found)
  {
    error(errSyntaxError, -1, "Document has not the mandatory ending %%EOF");
    errCode = errDamaged;
    delete[] eof;
    return gFalse;
  }
  delete[] eof;
  str->setPos(pos);
  return gTrue;
}

void PDFDoc::addCustomSignatureImage(unsigned char *image_data, unsigned long image_length)
{
  m_image_data_jpeg = image_data;
  m_image_length = image_length;
}

void PDFDoc::addSCAPAttributes(const char *attributeSupplier, const char *attributeName) {
    m_attribute_supplier = attributeSupplier;
    m_attribute_name = attributeName;
}
  
// Check for a PDF header on this stream.  Skip past some garbage
// if necessary.
void PDFDoc::checkHeader() {
  char hdrBuf[headerSearchSize+1];
  char *p;
  char *tokptr;
  int i;

  pdfMajorVersion = 0;
  pdfMinorVersion = 0;
  for (i = 0; i < headerSearchSize; ++i) {
    hdrBuf[i] = str->getChar();
  }
  hdrBuf[headerSearchSize] = '\0';
  for (i = 0; i < headerSearchSize - 5; ++i) {
    if (!strncmp(&hdrBuf[i], "%PDF-", 5)) {
      break;
    }
  }
  if (i >= headerSearchSize - 5) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  str->moveStart(i);
  if (!(p = strtok_r(&hdrBuf[i+5], " \t\n\r", &tokptr))) {
    error(errSyntaxWarning, -1, "May not be a PDF file (continuing anyway)");
    return;
  }
  sscanf(p, "%d.%d", &pdfMajorVersion, &pdfMinorVersion);
  // We don't do the version check. Don't add it back in.
}

GBool PDFDoc::checkEncryption(GooString *ownerPassword, GooString *userPassword) {
  Object encrypt;
  GBool encrypted;
  GBool ret;

  xref->getTrailerDict()->dictLookup("Encrypt", &encrypt);
  if ((encrypted = encrypt.isDict())) {
    if ((secHdlr = SecurityHandler::make(this, &encrypt))) {
      if (secHdlr->isUnencrypted()) {
	// no encryption
	ret = gTrue;
      } else if (secHdlr->checkEncryption(ownerPassword, userPassword)) {
	// authorization succeeded
       	xref->setEncryption(secHdlr->getPermissionFlags(),
			    secHdlr->getOwnerPasswordOk(),
			    secHdlr->getFileKey(),
			    secHdlr->getFileKeyLength(),
			    secHdlr->getEncVersion(),
			    secHdlr->getEncRevision(),
			    secHdlr->getEncAlgorithm());
	ret = gTrue;
      } else {
	// authorization failed
	ret = gFalse;
      }
    } else {
      // couldn't find the matching security handler
      ret = gFalse;
    }
  } else {
    // document is not encrypted
    ret = gTrue;
  }
  encrypt.free();
  return ret;
}

GBool PDFDoc::isOk()
{ 
	return ok; 
}

#define SIGFLAGS_SIGNATURES_EXIST   0x1
#define SIGFLAGS_APPEND_ONLY        0x2

GBool PDFDoc::isSigned() {
    GBool ret;
    Object sigFlags;
    if (getCatalog()->getAcroForm()->isNull())
    {
    	return gFalse;
    }
    getCatalog()->getAcroForm()->dictLookup("SigFlags", &sigFlags);

    if (sigFlags.isInt()) {
        ret = sigFlags.getInt() & SIGFLAGS_SIGNATURES_EXIST;
    } else {
        ret = gFalse;
    }
    sigFlags.free();

    return ret;
}


char *PDFDoc::getOccupiedSectors(int page)
{
	GooString *sectors = new GooString();
  Page *p = getPage(page);
	Object type, obj1;

	if(!p)
  {
    delete sectors;
		return NULL;
  }

	Annots *annotations = p->getAnnots();
	
	//Find the Signature Field and retrieve /Contents
	for (int i = 0; i != annotations->getNumAnnots(); i++)
	{
	    Object sector;	
	    Annot *an = annotations->getAnnot(i);
	    if (an->getType() != Annot::typeWidget)
		    continue;
	    Object f = an->getDict();

	    f.dictLookup("Type", &type);
	    f.dictLookup("FT", &obj1);

	    if (obj1.isNull())
		   continue;
	    //Search for signature fields
	    if (strcmp(obj1.getName(), "Sig") == 0)
	    {
		    f.dictLookup("SigSector", &sector);
		    if (sector.isNull())
			    continue;

        std::unique_ptr<GooString> sector_str(sectors->getLength() == 0 ? GooString::format("{0:d}", sector.getInt()) :
             GooString::format(",{0:d}", sector.getInt()));

		    // If we find a signature field marked
		    // with SigSector save this value 
		    sectors->append(sector_str.get());

	    }
	}
	
	return sectors->getCString();

}


GBool PDFDoc::isReaderEnabled()
{

	return getCatalog()->getUS3Dict();

}

std::unordered_set<int> PDFDoc::getSignaturesIndexesUntilLastTimestamp()
{
    std::unordered_set<int> indexes;
    Object *acro_form = getCatalog()->getAcroForm();
    Object fields, f, sig_dict, type, obj1;
    if (acro_form->isNull())
        return indexes;
    acro_form->dictLookup("Fields", &fields);
    //Sanity check: some malformed AcroForms don't have Fields array
    if (!fields.isArray())
      return indexes;

    // FIXME: this assumes the references for the latest signatures are appended to the
    // Fields dict. Most PDF creators should append.
    size_t lastField = fields.arrayGetLength()-1;
    for (int i = lastField; i >= 0; i--) 
    {
        fields.arrayGet(i, &f);
		if (!f.isDict()) {
			continue;
		}

        f.dictLookup("Type", &type);
        f.dictLookup("FT", &obj1);
        if (!type.isName() || !obj1.isName())
          continue;
        if (strcmp(type.getName(), "Annot") == 0
            && strcmp(obj1.getName(), "Sig") == 0)
        {

            f.dictLookup("V", &sig_dict);
			if (!sig_dict.isDict()) {
				continue;
			}
			indexes.insert(lastField - i);

            sig_dict.dictLookup("Type", &type);
            sig_dict.dictLookup("SubFilter", &obj1);
            if (type.isName() && obj1.isName()) {
              if (strcmp(type.getName(), "DocTimeStamp") == 0
                  && strcmp(obj1.getName(), "ETSI.RFC3161") == 0) {
                  break;
              }
			}
        }
    }

    return indexes;
}

/* Parameter sigIdx is the index for the signature dict in the document fields array 
*/
int PDFDoc::getSignatureContents(unsigned char **contents, int sigIdx)
{
	Object *acro_form = getCatalog()->getAcroForm();
	Object fields, f, sig_dict, contents_obj, type, obj1;

	if (acro_form->isNull())
		return 0;
	acro_form->dictLookup("Fields", &fields);
	int lastField = fields.arrayGetLength()-1;
	if (sigIdx > lastField) {
		error(errInternal, -1, "Signature field index %d greater than fields array length: %d", sigIdx, fields.arrayGetLength());
		return 0;
	}
	fields.arrayGet(lastField-sigIdx, &f);
	if (!f.isDict()) {
		error(errInternal, -1, "Signature field of index %d is invalid!", sigIdx);
		return 0;
	}

	f.dictLookup("Type", &type);
	f.dictLookup("FT", &obj1);
	if (strcmp(type.getName(), "Annot") == 0
			    && strcmp(obj1.getName(), "Sig") == 0)
	{
		f.dictLookup("V", &sig_dict);
		//Signature field may be empty so "/V" dictionary is optional
		if (sig_dict.isDict()) {

			sig_dict.dictLookup("Contents", &contents_obj);
			if (contents_obj.isString())
			{
				GooString *str = contents_obj.getString();
				int ret = str->getLength();
				*contents = (unsigned char *)malloc(ret);
				memcpy(*contents, str->getCString(), ret);
				return ret;
			}
		}

	}
	else {
		error(errInternal, -1, "Signature field index is wrong!: %d", sigIdx);
	}

	return 0;
}

//TODO: The next method considers only the first signature it happens to find
// in the file
Object *PDFDoc::getByteRange()
{

	Object *acro_form = getCatalog()->getAcroForm();
	Object fields, f, sig_dict, byterange_obj, type, obj1;

	if (acro_form->isNull())
		return 0;
	acro_form->dictLookup("Fields", &fields);
	
	//Find the Signature Field and retrieve /Contents
	for (int i = 0; i != fields.arrayGetLength(); i++)
	{
	    fields.arrayGet(i, &f);

	    f.dictLookup("Type", &type);
	    f.dictLookup("FT", &obj1);
	    if (strcmp(type.getName(), "Annot") == 0
			    && strcmp(obj1.getName(), "Sig") == 0)
	    {
		f.dictLookup("V", &sig_dict);
		if (!sig_dict.isDict())
			return NULL;
		sig_dict.dictLookup("ByteRange", &byterange_obj);
		if (byterange_obj.isArray())
		{
		   return new Object(byterange_obj);
		}
	    }

	}

	return NULL;
}


/* Workaround for linearized documents */

Ref PDFDoc::getPageRef(int page)
{

  Ref pageRef;

  pageRef.num = getHints()->getPageObjectNum(page);
  if (!pageRef.num) {
    error(errSyntaxWarning, -1, "Failed to get object num from hint tables for page 1");
  }

  // check for bogus ref - this can happen in corrupted PDF files
  if (pageRef.num < 0 || pageRef.num >= xref->getNumObjects()) {
    error(errSyntaxWarning, -1, "Invalid object num ({0:d}) for page 1", pageRef.num);
  }

  pageRef.gen = xref->getEntry(pageRef.num)->gen;

  return pageRef;

}

void PDFDoc::prepareSignature(PDFRectangle *rect, const char * name, const char *civil_number, 
                              const char *location, const char *reason, int page, int sector,
   		                      bool isPTLanguage, bool isCCSignature, bool showDate, bool small_signature)
{
	const char needle[] = "/Type /Sig";
	// Turn Signature mode On
	// This class-level flag affects Trailer /ID generation
	signature_mode = gTrue;
	long found = 0;
	char *base_search = NULL;

	getCatalog()->setIncrementalSignature(true);

    SignatureSignerInfo signer_info { name, civil_number, m_attribute_supplier, m_attribute_name };
	if (isLinearized())
	{
	   Ref first_page = getPageRef(page);
     
	   getCatalog()->prepareSignature(rect, &signer_info, &first_page, location,
		    reason, this->fileSize, page, sector, m_image_data_jpeg, m_image_length, 
        isPTLanguage, isCCSignature, showDate, small_signature);
	}
	else
	   getCatalog()->prepareSignature(rect, &signer_info, NULL, location,
	      reason, this->fileSize, page, sector, m_image_data_jpeg, m_image_length, 
        isPTLanguage, isCCSignature, showDate, small_signature);
	
	//Add enough space for the placeholder string
	MemOutStream mem_stream(this->fileSize + 20000); 
	OutStream * str = &mem_stream;

	//We're adding additional signature so it has to be an incremental update
	saveIncrementalUpdate(str);

	long haystack = (long)mem_stream.getData();

	//Start searching at the start of the new sig dictionary object
	base_search = (char *)mem_stream.getData() + xref->getSigDictOffset();
    size_t haystack_len = mem_stream.size() - xref->getSigDictOffset();

	found = (long)memmem(base_search, haystack_len,
			       	(const void *) needle, sizeof(needle)-1);

	m_sig_offset = found - haystack + 21;
	if (found == 0)
    {
		error(errInternal, -1, "prepareSignature: can't find signature offset. Aborting signature!");
        return;
    }
	
	getCatalog()->setSignatureByteRange(m_sig_offset, ESTIMATED_LEN, mem_stream.size());

}

/* Allocates and fills a byte array with the PDF content that will be signed 
 * i.e. everything except the placeholder hex string <0000...>
   The return value is the size of the array
*/
unsigned long PDFDoc::getSigByteArray(unsigned char **byte_array)
{
	MemOutStream mem_stream(this->fileSize+ESTIMATED_LEN +190000);
	unsigned int i = 0, ret_len = 0;
	OutStream * out_str = &mem_stream;

	saveIncrementalUpdate(out_str);

  if (m_sig_offset >= mem_stream.size()) {
    error(errInternal, -1, "getSigByteArray: m_sig_offset greater than current doc size: %d >= %d", m_sig_offset, mem_stream.size());
    return 0;
  }
	
	char * base_ptr = (char *)mem_stream.getData();

	ret_len = mem_stream.size() - ESTIMATED_LEN - 2;

	*byte_array = (unsigned char *)gmalloc(ret_len);

	memcpy((*byte_array)+i, base_ptr, m_sig_offset);
	i+= m_sig_offset;

	int len2 = mem_stream.size() - m_sig_offset - ESTIMATED_LEN -2;
	memcpy((*byte_array)+i, base_ptr + m_sig_offset+
			ESTIMATED_LEN + 2, len2);

	return ret_len;
}

void PDFDoc::closeSignature(const char *signature_contents)
{
  	getCatalog()->closeSignature(signature_contents, ESTIMATED_LEN);
}

void PDFDoc::getCertsInDSS(std::vector<ValidationDataElement *> *certs)
{
    Object *dss = getCatalog()->getDSS();

    if (!dss->isDict())
        return;

    Object certArray;
    dss->dictLookup("Certs", &certArray);

    if (!certArray.isArray())
        return;

    for (size_t i = 0; i < certArray.arrayGetLength(); i++)
    {
        Object certObj;
        certArray.arrayGet(i, &certObj);

        if (certObj.isStream())
        {
            Stream *certStream = certObj.getStream();
            GooString certRaw;
            certStream->fillGooString(&certRaw);

            ValidationDataElement *vde = new ValidationDataElement(
                (unsigned char*)certRaw.getCString(), certRaw.getLength(), ValidationDataElement::CERT);

            certs->push_back(vde);
        }

        certObj.free();
    }

    certArray.free();
}

void PDFDoc::addDSS(std::vector<ValidationDataElement *> validationData)
{
    /* Write and fill content with placeholder for the byterange. */
    MemOutStream mem_stream(this->fileSize + ESTIMATED_LEN);
    OutStream * str = &mem_stream;
    saveIncrementalUpdate(str);

    // create DSS if it does not exist
    Object *dss = getCatalog()->createDSS();

    // Add validation data to DSS dictionary and VRI
    for (size_t i = 0; i < validationData.size(); i++)
    {
        Object streamDictObj, streamObj;
        streamDictObj.initDict(xref);
        Stream *stream = new MemStream((char *)validationData[i]->getData(), 0, validationData[i]->getSize(), &streamDictObj);

        /* Compress stream if it is CRL. */
        if (validationData[i]->getType() == ValidationDataElement::CRL)
        {
            Object obj;
            streamDictObj.dictAdd(copyString("Filter"), obj.initName("FlateDecode"));
            stream = new FlateEncoder(stream);
        }
        streamObj.initStream(stream);

        Ref streamRef = xref->addIndirectObject(&streamObj);
        
        Object streamRefObj;
        streamRefObj.initRef(streamRef.num, streamRef.gen);

        const char *vriPointerType;
        Object vdeArrayObj, vdeArrayRef;

        switch (validationData[i]->getType())
        {
            case ValidationDataElement::OCSP:
                dss->dictLookup("OCSPs", &vdeArrayObj);
                dss->dictLookupNF("OCSPs", &vdeArrayRef);
                vriPointerType = "OCSP";
                break;

            case ValidationDataElement::CRL:
                dss->dictLookup("CRLs", &vdeArrayObj);
                dss->dictLookupNF("CRL", &vdeArrayRef);
                vriPointerType = "CRL";
                break;

            case ValidationDataElement::CERT:
                dss->dictLookup("Certs", &vdeArrayObj);
                dss->dictLookupNF("Cert", &vdeArrayRef);
                vriPointerType = "Cert";
                break;
        }

        vdeArrayObj.arrayAdd(&streamRefObj);

        /*If the array is indirect object we need to update it.*/
        if (vdeArrayRef.isRef())
            xref->setModifiedObject(&vdeArrayObj, vdeArrayRef.getRef());

        /* Each validation datum can be used to validate different objects
         and, for that reason, be present in multiple vri entries. */
        Object vriObj, vriRef;
        dss->dictLookup("VRI", &vriObj);
        dss->dictLookupNF("VRI", &vriRef);
        Object vriEntry;
        for (auto const& key : validationData[i]->getVriHashKeys())
        {
            vriObj.dictLookup(key.c_str(), &vriEntry);

            if (vriEntry.isNull())
            {
                vriEntry.initDict(xref);
                vriObj.dictAdd((char *)key.c_str(), &vriEntry);
            }

            Object vriPointerArrayObj;

            vriEntry.dictLookup(vriPointerType, &vriPointerArrayObj);
            if (vriPointerArrayObj.isNull())
            {
                vriPointerArrayObj.initArray(xref);
                vriEntry.dictAdd((char *)vriPointerType, &vriPointerArrayObj);
            }
            vriPointerArrayObj.arrayAdd(&streamRefObj);
        }

        /*If the VRI dict is indirect object we need to update it.*/
        if (vriRef.isRef())
            xref->setModifiedObject(&vriObj, vriRef.getRef());
    }
}

void PDFDoc::prepareTimestamp()
{
    signature_mode = gTrue;
    getCatalog()->setIncrementalSignature(true);
    Object *timestampDictObj = new Object();
    Object obj;
    timestampDictObj->initSignatureDict(xref);
    timestampDictObj->dictAdd(copyString("Type"), obj.initName("DocTimeStamp"));
    timestampDictObj->dictAdd(copyString("Filter"), obj.initName("Adobe.PPKLite"));
    timestampDictObj->dictAdd(copyString("SubFilter"), obj.initName("ETSI.RFC3161"));

    /* Placeholder for contents needed to compute ByteRange */
    std::string contentsStr(ESTIMATED_LEN, '0');
    GooString *contents = new GooString(contentsStr.c_str());
    contents->setHexString();

    timestampDictObj->dictAdd(copyString("Contents"), obj.initString(contents));

    Ref timestampRef = xref->addIndirectObject(timestampDictObj);

    /* Add signature to signature fields in AcroForm. */
    Ref *firstPageRef = getCatalog()->getPageRef(1);
    Object signatureField, timestampRefObj;
    signatureField.initDict(xref);
    getCatalog()->fillSignatureField(&signatureField, NULL, 0, firstPageRef);
    timestampRefObj.initRef(timestampRef.num, timestampRef.gen);
    signatureField.dictAdd(copyString("V"), &timestampRefObj);
    Ref sigFieldRef = xref->addIndirectObject(&signatureField);

    //Reserve space for the padded byte range
    SignatureDict *d = dynamic_cast<SignatureDict *>(timestampDictObj->getDict());
    d->setPadding(PADDED_BYTERANGE_LEN);

    getCatalog()->addSigFieldToAcroForm(&sigFieldRef, NULL);

    /* Write and fill content with placeholder for the byterange. */
    MemOutStream mem_stream(this->fileSize + ESTIMATED_LEN);
    OutStream * str = &mem_stream;
    saveIncrementalUpdate(str);

    const char needle[] = "/ETSI.RFC3161 /Contents ";
    unsigned char *streamPtr = mem_stream.getData();
    unsigned char *haystack = streamPtr;
    unsigned char *found = 0;
    unsigned char *needlePtr = 0;

    size_t buffer_len = mem_stream.size();
    /* Find the last occurrence of needle in mem_stream buffer */
    while (needlePtr = (unsigned char *)memmem(haystack, buffer_len, needle, sizeof(needle) - 1))
    {
        found = needlePtr;
        haystack = found + sizeof(needle);

        buffer_len = mem_stream.size() - ((long)haystack - (long)streamPtr);
    }
    
    if (found == NULL)
    {
        error(errInternal, -1, "addTimestamp: can't find signature offset. Aborting timestamping!");
        return;
    }
    m_sig_offset = (long)found - (long)streamPtr + sizeof(needle) - 1;

    getCatalog()->setSignatureByteRange(m_sig_offset, ESTIMATED_LEN, mem_stream.size(), timestampDictObj, &timestampRef);

}

GBool PDFDoc::containsXfaForm() {
	Object * acroForm = NULL;
	Object xfa;
	GBool res = gFalse;
	if ((acroForm = getCatalog()->getAcroForm())->isDict()) {
		acroForm->dictLookup("XFA", &xfa);
		res = xfa.isStream() || xfa.isArray();
		xfa.free();
	}
	
	return res;
}


Links *PDFDoc::getLinks(int page) {
  Page *p = getPage(page);
  if (!p) {
    return new Links (NULL);
  }
  return p->getLinks();
}

Linearization *PDFDoc::getLinearization()
{
  if (!linearization) {
    linearization = new Linearization(str);
  }
  return linearization;
}

GBool PDFDoc::isLinearized() {
  if ((str->getLength()) &&
      (getLinearization()->getLength() == str->getLength()))
    return gTrue;
  else
    return gFalse;
}

static GBool
get_id (GooString *encodedidstring, GooString *id) {
  const char *encodedid = encodedidstring->getCString();
  char pdfid[pdfIdLength + 1];
  int n;

  if (encodedidstring->getLength() != pdfIdLength / 2)
    return gFalse;

  n = sprintf(pdfid, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	      encodedid[0] & 0xff, encodedid[1] & 0xff, encodedid[2] & 0xff, encodedid[3] & 0xff,
	      encodedid[4] & 0xff, encodedid[5] & 0xff, encodedid[6] & 0xff, encodedid[7] & 0xff,
	      encodedid[8] & 0xff, encodedid[9] & 0xff, encodedid[10] & 0xff, encodedid[11] & 0xff,
	      encodedid[12] & 0xff, encodedid[13] & 0xff, encodedid[14] & 0xff, encodedid[15] & 0xff);
  if (n != pdfIdLength)
    return gFalse;

  id->Set(pdfid, pdfIdLength);
  return gTrue;
}

GBool PDFDoc::getID(GooString *permanent_id, GooString *update_id) {
  Object obj;
  xref->getTrailerDict()->dictLookup ("ID", &obj);

  if (obj.isArray() && obj.arrayGetLength() == 2) {
    Object obj2;

    if (permanent_id) {
      if (obj.arrayGet(0, &obj2)->isString()) {
        if (!get_id (obj2.getString(), permanent_id)) {
	  obj2.free();
	  return gFalse;
	}
      } else {
        error(errSyntaxError, -1, "Invalid permanent ID");
	obj2.free();
	return gFalse;
      }
      obj2.free();
    }

    if (update_id) {
      if (obj.arrayGet(1, &obj2)->isString()) {
        if (!get_id (obj2.getString(), update_id)) {
	  obj2.free();
	  return gFalse;
	}
      } else {
        error(errSyntaxError, -1, "Invalid update ID");
	obj2.free();
	return gFalse;
      }
      obj2.free();
    }

    obj.free();

    return gTrue;
  }
  obj.free();

  return gFalse;
}

Hints *PDFDoc::getHints()
{
  if (!hints && isLinearized()) {
    hints = new Hints(str, getLinearization(), getXRef(), secHdlr);
  }

  return hints;
}

int PDFDoc::savePageAs(GooString *name, int pageNo) 
{
  FILE *f;
  OutStream *outStr;
  XRef *yRef, *countRef;
  int rootNum = getXRef()->getNumObjects() + 1;

  if (pageNo < 1 || pageNo > getNumPages()) {
    error(errInternal, -1, "Illegal pageNo: {0:d}({1:d})", pageNo, getNumPages() );
    return errOpenFile;
  }
  PDFRectangle *cropBox = NULL;
  if (getCatalog()->getPage(pageNo)->isCropped()) {
    cropBox = getCatalog()->getPage(pageNo)->getCropBox();
  }
  replacePageDict(pageNo, 
    getCatalog()->getPage(pageNo)->getRotate(),
    getCatalog()->getPage(pageNo)->getMediaBox(),
    cropBox, NULL);
  Ref *refPage = getCatalog()->getPageRef(pageNo);
  Object page;
  getXRef()->fetch(refPage->num, refPage->gen, &page);

  if (!(f = fopen(name->getCString(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return errOpenFile;
  }
  outStr = new FileOutStream(f,0);

  yRef = new XRef(getXRef()->getTrailerDict());
  countRef = new XRef();
  yRef->add(0, 65535, 0, gFalse);
  writeHeader(outStr, getPDFMajorVersion(), getPDFMinorVersion());

  // get and mark info dict
  Object infoObj;
  getXRef()->getDocInfo(&infoObj);
  if (infoObj.isDict()) {
    Dict *infoDict = infoObj.getDict();
    markPageObjects(infoDict, yRef, countRef, 0);
    Object *trailerObj = getXRef()->getTrailerDict();
    if (trailerObj->isDict()) {
      Dict *trailerDict = trailerObj->getDict();
      Object ref;
      trailerDict->lookupNF("Info", &ref);
      if (ref.isRef()) {
        yRef->add(ref.getRef().num, ref.getRef().gen, 0, gTrue);
        if (getXRef()->getEntry(ref.getRef().num)->type == xrefEntryCompressed) {
          yRef->getEntry(ref.getRef().num)->type = xrefEntryCompressed;
        }
      }
      ref.free();
    }
  }
  infoObj.free();
  
  // get and mark output intents etc.
  Object catObj;
  getXRef()->getCatalog(&catObj);
  Dict *catDict = catObj.getDict();
  markPageObjects(catDict, yRef, countRef, 0);

  Dict *pageDict = page.getDict();
  markPageObjects(pageDict, yRef, countRef, 0);
  Guint objectsCount = writePageObjects(outStr, yRef, 0);

  yRef->add(rootNum,0,outStr->getPos(),gTrue);
  outStr->printf("%d 0 obj\n", rootNum);
  outStr->printf("<< /Type /Catalog /Pages %d 0 R", rootNum + 1); 
  for (int j = 0; j < catDict->getLength(); j++) {
    const char *key = catDict->getKey(j);
    if (strcmp(key, "Type") != 0 &&
      strcmp(key, "Catalog") != 0 &&
      strcmp(key, "Pages") != 0) 
    {
      if (j > 0) outStr->printf(" ");
      Object value; catDict->getValNF(j, &value);
      outStr->printf("/%s ", key);
      writeObject(&value, NULL, outStr, getXRef(), 0);
      value.free();
    }
  }
  catObj.free();
  outStr->printf(">>\nendobj\n");
  objectsCount++;

  yRef->add(rootNum + 1,0,outStr->getPos(),gTrue);
  outStr->printf("%d 0 obj\n", rootNum + 1);
  outStr->printf("<< /Type /Pages /Kids [ %d 0 R ] /Count 1 >>\n", rootNum + 2);
  outStr->printf("endobj\n");
  objectsCount++;

  yRef->add(rootNum + 2,0,outStr->getPos(),gTrue);
  outStr->printf("%d 0 obj\n", rootNum + 2);
  outStr->printf("<< ");
  for (int n = 0; n < pageDict->getLength(); n++) {
    if (n > 0) outStr->printf(" ");
    const char *key = pageDict->getKey(n);
    Object value; pageDict->getValNF(n, &value);
    if (strcmp(key, "Parent") == 0) {
      outStr->printf("/Parent %d 0 R", rootNum + 1);
    } else {
      outStr->printf("/%s ", key);
      writeObject(&value, NULL, outStr, getXRef(), 0); 
    }
    value.free();
  }
  outStr->printf(" >>\nendobj\n");
  objectsCount++;
  page.free();

  Guint uxrefOffset = outStr->getPos();
  Ref ref;
  ref.num = rootNum;
  ref.gen = 0;
  Dict *trailerDict = createTrailerDict(objectsCount, gFalse, 0, &ref, getXRef(),
                                        name->getCString(), uxrefOffset);
  writeXRefTableTrailer(trailerDict, yRef, gFalse /* do not write unnecessary entries */,
                        uxrefOffset, outStr, getXRef());
  delete trailerDict;

  outStr->close();
  fclose(f);
  delete yRef;
  delete countRef;

  return errNone;
}

int PDFDoc::saveAs(GooString *name, PDFWriteMode mode) {
  FILE *f;
  OutStream *outStr;
  int res;

  if (!(f = fopen(name->getCString(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return errOpenFile;
  }
  outStr = new FileOutStream(f,0);
  res = saveAs(outStr, mode);
  delete outStr;
  fclose(f);
  return res;
}

#ifdef WIN32
int PDFDoc::saveAs(wchar_t *name, PDFWriteMode mode) {
	FILE *f;
	OutStream *outStr;
	int res;
	if (!(f = _wfopen(name, L"wb"))) {
        
        error(errIO, -1, "Couldn't open file ");
        fprintf(stderr, "%ws \n", name); //GooString does not format wchar_t
        int errorCode = errno;
        switch (errorCode) {
          case EACCES:
            return errPermission;
          case EINTR: // process interrupted
            return errFileINTR;
          case EIO: //error writing
            return errFileIO;
          case ENAMETOOLONG: //too long of a file name
            return errFileNAMETOOLONG;
          case ENFILE: // too many files opened in the SYSTEM
            return errFileOPFLSYSTEM;
          case EMFILE: //too many files opened in this PROCESS
            return errFileOPFLPROCES;
          case ENOSPC: //full disk
            return errFileNOSPC;
          case EPERM: // operation is not permited, different from no permissions
            return errFilePERM;
          case EROFS: // read only file system
            return errFileREADONLY;
          case EXDEV: // cross device link, moving a file by renaming it, don't know if valid in this scenario  
            return errFileDEV;    
          default:
			error(errIO, -1, "Unknown not handled error: {0:d}.", errorCode);
            return errOpenFile;
        }
    }
	outStr = new FileOutStream(f, 0);
	res = saveAs(outStr, mode);
	delete outStr;
	fclose(f);
	return res;
}
#endif

int PDFDoc::saveAs(OutStream *outStr, PDFWriteMode mode) {

  // find if we have updated objects
  GBool updated = gFalse;
  for(int i=0; i<xref->getNumObjects(); i++) {
    if (xref->getEntry(i)->updated) {
      updated = gTrue;
      break;
    }
  }

  // we don't support rewriting files with Encrypt at the moment
  Object obj;
  xref->getTrailerDict()->getDict()->lookupNF("Encrypt", &obj);
  if (!obj.isNull())
  {
    obj.free();
    if (!updated && mode == writeStandard) {
      // simply copy the original file
      saveWithoutChangesAs (outStr);
    } else {
      return errEncrypted;
    }
  }
  else
  {
    obj.free();

    if (mode == writeForceRewrite) {
      saveCompleteRewrite(outStr);
    } else if (mode == writeForceIncremental) {
      saveIncrementalUpdate(outStr); 
    } else { // let poppler decide
      if(updated) { 
        saveIncrementalUpdate(outStr);
      } else {
        // simply copy the original file
        saveWithoutChangesAs (outStr);
      }
    }
  }

  return errNone;
}

int PDFDoc::saveWithoutChangesAs(GooString *name) {
  FILE *f;
  OutStream *outStr;
  int res;

  if (!(f = fopen(name->getCString(), "wb"))) {
    error(errIO, -1, "Couldn't open file '{0:t}'", name);
    return errOpenFile;
  }
  
  outStr = new FileOutStream(f,0);
  res = saveWithoutChangesAs(outStr);
  delete outStr;

  fclose(f);

  return res;
}

int PDFDoc::saveWithoutChangesAs(OutStream *outStr) {
  int c;
  
  str->reset();
  while ((c = str->getChar()) != EOF) {
    outStr->put(c);
  }
  str->close();

  return errNone;
}

void PDFDoc::saveIncrementalUpdate (OutStream* outStr)
{
  XRef *uxref;
  int c;
  //copy the original file
  str->reset();
  while ((c = str->getChar()) != EOF) {
    outStr->put(c);
  }
  str->close();

  uxref = new XRef();
  uxref->add(0, 65535, 0, gFalse);
  for(int i=0; i<xref->getNumObjects(); i++) {
    if ((xref->getEntry(i)->type == xrefEntryFree) && 
        (xref->getEntry(i)->gen == 0)) //we skip the irrelevant free objects
      continue;

    if (xref->getEntry(i)->updated) { //we have an updated object
      Ref ref;
      ref.num = i;
      ref.gen = xref->getEntry(i)->type == xrefEntryCompressed ? 0 : xref->getEntry(i)->gen;
      if (xref->getEntry(i)->type != xrefEntryFree) {
        Object obj1;
        xref->fetch(ref.num, ref.gen, &obj1);
        Guint offset = writeObject(&obj1, &ref, outStr);
        uxref->add(ref.num, ref.gen, offset, gTrue);
        obj1.free();
      } else {
        uxref->add(ref.num, ref.gen, 0, gFalse);
      }
    }
  }
  if (uxref->getNumObjects() == 0) { //we have nothing to update
    delete uxref;
    return;
  }

  Guint uxrefOffset = outStr->getPos();
  int numobjects = xref->getNumObjects();
  const char *fileNameA = fileName ? fileName->getCString() : NULL;
  Ref rootRef, uxrefStreamRef;
  rootRef.num = getXRef()->getRootNum();
  rootRef.gen = getXRef()->getRootGen();

  // Output a xref stream if there is a xref stream already
  GBool xRefStream = xref->isXRefStream();

  if (xRefStream) {
    // Append an entry for the xref stream itself
    uxrefStreamRef.num = numobjects++;
    uxrefStreamRef.gen = 0;
    uxref->add(uxrefStreamRef.num, uxrefStreamRef.gen, uxrefOffset, gTrue);
  }

  Dict *trailerDict = createTrailerDict(numobjects, gTrue, getStartXRef(), &rootRef, getXRef(),
		  fileNameA, uxrefOffset, signature_mode);
  if (xRefStream) {
    writeXRefStreamTrailer(trailerDict, uxref, &uxrefStreamRef, uxrefOffset, outStr, getXRef());
  } else {
    writeXRefTableTrailer(trailerDict, uxref, gFalse, uxrefOffset, outStr, getXRef());
  }

  delete trailerDict;
  delete uxref;
}

bool isEmptySignatureDict(Object &sig_dict, int index) {
  //const char * empty_hexstring_pat = "<00";
  bool res = false;
  Object type, contents;
  sig_dict.dictLookup("Type", &type);
  sig_dict.dictLookup("Contents", &contents);

  if (type.isName() && strcmp(type.getName(), "Sig")==0) {
    if (contents.isString()) {
      // Can't use GooString::getLength() here because it will return the size of the internal buffer
      // i.e how many bytes are used to store all the 0s of the empty signature placeholder"
      char * sig_content = contents.getString()->getCString();
      if (strlen(sig_content) == 0) {
        res = true;
      }
      
    }
  }

  type.free();
  contents.free();

  return res;
}

void PDFDoc::cleanSignatureDicts() {
  
  for (int i=0; i<xref->getNumObjects(); i++) {
    Object obj1;
    Ref ref;
    XRefEntry *entry = xref->getEntry(i);
    if (entry->type == xrefEntryUncompressed) {
      ref.num = i;
      ref.gen = xref->getEntry(i)->gen;
      xref->fetch(ref.num, ref.gen, &obj1);
      if (obj1.isDict() && isEmptySignatureDict(obj1, i)) {
#ifdef _DEBUG
         fprintf(stderr, "DEBUG: deleting empty signature dict: %d\n", ref.num);
#endif         
         xref->removeEntry(entry);
      }
      obj1.free();
    }
  }
}

void PDFDoc::saveCompleteRewrite (OutStream* outStr)
{
  outStr->printf("%%PDF-%d.%d\r\n",pdfMajorVersion,pdfMinorVersion);
  XRef *uxref = new XRef();
  uxref->add(0, 65535, 0, gFalse);
  for(int i=0; i<xref->getNumObjects(); i++) {
    Object obj1;
    Ref ref;
    XRefEntryType type = xref->getEntry(i)->type;
    if (type == xrefEntryFree) {
      ref.num = i;
      ref.gen = xref->getEntry(i)->gen;
      /* the XRef class adds a lot of irrelevant free entries, we only want the significant one
          and we don't want the one with num=0 because it has already been added (gen = 65535)*/
      if (ref.gen > 0 && ref.num > 0)
        uxref->add(ref.num, ref.gen, 0, gFalse);
    } else if (type == xrefEntryUncompressed){ 
      ref.num = i;
      ref.gen = xref->getEntry(i)->gen;
      xref->fetch(ref.num, ref.gen, &obj1);
      Guint offset = writeObject(&obj1, &ref, outStr);
      uxref->add(ref.num, ref.gen, offset, gTrue);
      obj1.free();
    } else if (type == xrefEntryCompressed) {
      ref.num = i;
      ref.gen = 0; //compressed entries have gen == 0
      xref->fetch(ref.num, ref.gen, &obj1);
      Guint offset = writeObject(&obj1, &ref, outStr);
      uxref->add(ref.num, ref.gen, offset, gTrue);
      obj1.free();
    }
  }
  Guint uxrefOffset = outStr->getPos();
  writeXRefTableTrailer(uxrefOffset, uxref, gTrue /* write all entries */,
                        uxref->getNumObjects(), outStr, gFalse /* complete rewrite */);
  delete uxref;
}

void PDFDoc::writeDictionnary (Dict* dict, OutStream* outStr, XRef *xRef, Guint numOffset)
{
  Object obj1;
  //Store offset of the signature dict object
  if (dict->isSignatureDict()) {
    xRef->saveSigDictOffset(outStr->getPos());
  }
  outStr->printf("<<");
  for (int i=0; i<dict->getLength(); i++) {
    GooString keyName(dict->getKey(i));
    GooString *keyNameToPrint = keyName.sanitizedName(gFalse /* non ps mode */);
    outStr->printf("/%s ", keyNameToPrint->getCString());
    delete keyNameToPrint;
    writeObject(dict->getValNF(i, &obj1), NULL, outStr, xRef, numOffset);
    obj1.free();
  }
  if(dict->isSignatureDict())
   {
	 SignatureDict *sig = dynamic_cast<SignatureDict *>(dict);
	 char padding[100] = { 0 };
	 memset(padding, ' ', sig->getPadding());
	 outStr->printf("%s", padding);

   }
  outStr->printf(">> ");
}

void PDFDoc::writeStream (Stream* str, OutStream* outStr)
{
  outStr->printf("stream\r\n");
  str->reset();
  for (int c=str->getChar(); c!= EOF; c=str->getChar()) {
    outStr->printf("%c", c);  
  }
  outStr->printf("\r\nendstream\r\n");
}

void PDFDoc::writeRawStream (Stream* str, OutStream* outStr)
{
  Object obj1;
  str->getDict()->lookup("Length", &obj1);
  if (!obj1.isInt()) {
    error (errSyntaxError, -1, "PDFDoc::writeRawStream, no Length in stream dict");
    return;
  }

  const int length = obj1.getInt();
  obj1.free();

  outStr->printf("stream\r\n");
  str->unfilteredReset();
  for (int i=0; i<length; i++) {
    int c = str->getUnfilteredChar();
    outStr->printf("%c", c);  
  }
  str->reset();
  outStr->printf("\r\nendstream\r\n");
}

void PDFDoc::writeString (GooString* s, OutStream* outStr)
{

	//Support for PDF hexstrings
	const char * begin_char = s->isHexString() ? "<" : "(";
	const char * end_char = s->isHexString() ? "> " : ") ";
	if (s->hasUnicodeMarker()) {
    //unicode string don't necessary end with \0
    const char* c = s->getCString();
    outStr->printf(begin_char);
    for(int i=0; i<s->getLength(); i++) {
      char unescaped = *(c+i)&0x000000ff;
      //escape if needed
      if (unescaped == '(' || unescaped == ')' || unescaped == '\\')
        outStr->printf("%c", '\\');
      outStr->printf("%c", unescaped);
    }
    outStr->printf(end_char);
  } else {
    const char* c = s->getCString();
    outStr->printf(begin_char);
    for(int i=0; i<s->getLength(); i++) {
      char unescaped = *(c+i)&0x000000ff;
      //escape if needed
      if (unescaped == '\r')
        outStr->printf("\\r");
      else if (unescaped == '\n')
        outStr->printf("\\n");
      else {
        if (unescaped == '(' || unescaped == ')' || unescaped == '\\') {
          outStr->printf("%c", '\\');
        }
        outStr->printf("%c", unescaped);
      }
    }
    outStr->printf(end_char);
  }
}

Guint PDFDoc::writeObject (Object* obj, Ref* ref, OutStream* outStr, XRef *xRef, Guint numOffset)
{
  Array *array;
  Object obj1;
  Guint offset = outStr->getPos();
  int tmp;

  if(ref) 
    outStr->printf("%i %i obj\r\n", ref->num, ref->gen);

  switch (obj->getType()) {
    case objBool:
      outStr->printf("%s ", obj->getBool()?"true":"false");
      break;
    case objInt:
      outStr->printf("%i ", obj->getInt());
      break;
    case objReal:
    {
      GooString s;
      s.appendf("{0:.10g}", obj->getReal());
      outStr->printf("%s ", s.getCString());
      break;
    }
    case objString:
      writeString(obj->getString(), outStr);
      break;
    case objName:
    {
      GooString name(obj->getName());
      GooString *nameToPrint = name.sanitizedName(gFalse /* non ps mode */);
      outStr->printf("/%s ", nameToPrint->getCString());
      delete nameToPrint;
      break;
    }
    case objNull:
      outStr->printf( "null ");
      break;
    case objArray:
      array = obj->getArray();
      outStr->printf("[");
      for (int i=0; i<array->getLength(); i++) {
        writeObject(array->getNF(i, &obj1), NULL,outStr, xRef, numOffset);
        obj1.free();
      }
      outStr->printf("] ");
      break;
    case objDict:
      writeDictionnary (obj->getDict(),outStr, xRef, numOffset);
      break;
    case objStream: 
      {
        //We can't modify stream with the current implementation (no write functions in Stream API)
        // => the only type of streams which that have been modified are internal streams (=strWeird)
        Stream *stream = obj->getStream();
        if (stream->getKind() == strWeird) {
          //we write the stream unencoded => TODO: write stream encoder
          stream->reset();
          //recalculate stream length
          tmp = 0;
          for (int c=stream->getChar(); c!=EOF; c=stream->getChar()) {
            tmp++;
          }
          obj1.initInt(tmp);
          stream->getDict()->set("Length", &obj1);

          //Remove Stream encoding
          //We need to comment it out because we're creating Streams of type strWeird 
	  //which are actually FlateEncoded, poppler stream classes are not really
	  //suited for this, so its a hack ...
	  //stream->getDict()->remove("Filter");
          //stream->getDict()->remove("DecodeParms");

          writeDictionnary (stream->getDict(),outStr, xRef, numOffset);
          writeStream (stream,outStr);
          obj1.free();
        }
	/*
	else if (stream->getKind() == strDeflate) {

		Object obj_stream_dict;
		obj_stream_dict.initDict(xRef);
		//TODO complete with ObjStm attributes
		//obj_stream.dictAdd("", );
		//TODO 
		DeflateStream *def = dynamic_cast<DeflateStream*>(stream);

	  MemOutStream *out_mem_str = new MemOutStream(256);
	  writeDictionnary(stream->getDict(), out_mem_str, xRef, numOffset);

	  def->setStream(out_mem_str);
          writeStream (stream,outStr);

	} */
	else {
          //raw stream copy
          FilterStream *fs = dynamic_cast<FilterStream*>(stream);
          if (fs) {
            BaseStream *bs = fs->getBaseStream();
            if (bs) {
              Guint streamEnd;
                if (xRef->getStreamEnd(bs->getStart(), &streamEnd)) {
                  Object val;
                  val.initInt(streamEnd - bs->getStart());
                  stream->getDict()->set("Length", &val);
                }
              }
          }
          writeDictionnary (stream->getDict(), outStr, xRef, numOffset);
          writeRawStream (stream, outStr);
        }
        break;
      }
    case objRef:
      outStr->printf("%i %i R ", obj->getRef().num + numOffset, obj->getRef().gen);
      break;
    case objCmd:
      outStr->printf("%s\n", obj->getCmd());
      break;
    case objError:
      outStr->printf("error\r\n");
      break;
    case objEOF:
      outStr->printf("eof\r\n");
      break;
    case objNone:
      outStr->printf("none\r\n");
      break;
    default:
      error(errUnimplemented, -1,"Unhandled objType : {0:d}, please report a bug with a testcase\r\n", obj->getType());
      break;
  }
  if (ref)
    outStr->printf("\r\nendobj\r\n");
  return offset;
}

Dict *PDFDoc::createTrailerDict(int uxrefSize, GBool incrUpdate, Guint startxRef,
                                Ref *root, XRef *xRef, const char *fileName, Guint fileSize, GBool is_signature_mode) 
{
  Dict *trailerDict = new Dict(xRef);
  Object obj1;
  obj1.initInt(uxrefSize);
  trailerDict->set("Size", &obj1);
  obj1.free();

  //build a new ID, as recommended in the reference, uses:
  // - current time
  // - file name
  // - file size
  // - values of entry in information dictionnary
  GooString message;
  char buffer[256];
  sprintf(buffer, "%i", (int)time(NULL));
  message.append(buffer);

  if (fileName)
    message.append(fileName);

  sprintf(buffer, "%i", fileSize);
  message.append(buffer);

  //info dict -- only use text string
  if (!xRef->getTrailerDict()->isNone() && xRef->getDocInfo(&obj1)->isDict()) {
    for(int i=0; i<obj1.getDict()->getLength(); i++) {
      Object obj2;
      obj1.getDict()->getVal(i, &obj2);  
      if (obj2.isString()) {
        message.append(obj2.getString());
      }
      obj2.free();
    }
  }
  obj1.free();

  //calculate md5 digest
  Guchar digest[16];
  md5((Guchar*)message.getCString(), message.getLength(), digest);
  obj1.initString(new GooString((const char*)digest, 16));

  //create ID array
  Object obj2,obj3,obj5;
  obj2.initArray(xRef);

  Object obj4;
  xRef->getTrailerDict()->getDict()->lookup("ID", &obj4);
  if (is_signature_mode)
  {
	  if (!obj4.isArray()) {
		  error(errSyntaxWarning, -1, "PDFDoc::createTrailerDict original file's ID entry isn't an array. Trying to continue");
	  }
	  else
	  {

		  obj4.arrayGet(0,&obj3); 

		  obj2.arrayAdd(&obj3);
		  obj2.arrayAdd(&obj3);
		  trailerDict->set("ID", &obj4); //Don't change the File ID 
	  }

  } else {
	  //new file => same values for the two identifiers
	  obj2.arrayAdd(&obj1);
	  obj1.initString(new GooString((const char*)digest, 16));
	  obj2.arrayAdd(&obj1);
	  trailerDict->set("ID", &obj2);
  }

  //obj4.free();

  obj1.initRef(root->num, root->gen);
  trailerDict->set("Root", &obj1);

  if (incrUpdate) { 
    obj1.initInt(startxRef);
    trailerDict->set("Prev", &obj1);
  }
  
  if (!xRef->getTrailerDict()->isNone()) {
    xRef->getDocInfoNF(&obj5);
    if (!obj5.isNull()) {
      trailerDict->set("Info", &obj5);
    }
  }

  return trailerDict;
}

void PDFDoc::writeXRefTableTrailer(Dict *trailerDict, XRef *uxref, GBool writeAllEntries, Guint uxrefOffset, OutStream* outStr, XRef *xRef)
{
  uxref->writeTableToFile( outStr, writeAllEntries );
  outStr->printf( "trailer\r\n");
  writeDictionnary(trailerDict, outStr, xRef, 0);
  outStr->printf( "\r\nstartxref\r\n");
  outStr->printf( "%i\r\n", uxrefOffset);
  outStr->printf( "%%%%EOF\r\n");
}

void PDFDoc::writeXRefStreamTrailer (Dict *trailerDict, XRef *uxref, Ref *uxrefStreamRef, Guint uxrefOffset, OutStream* outStr, XRef *xRef)
{
  GooString stmData;

  // Fill stmData and some trailerDict fields
  uxref->writeStreamToBuffer(&stmData, trailerDict, xRef);

  // Create XRef stream object and write it
  Object obj1;
  MemStream *mStream = new MemStream( stmData.getCString(), 0,
                                      stmData.getLength(), obj1.initDict(trailerDict) );
  writeObject(obj1.initStream(mStream), uxrefStreamRef, outStr, xRef, 0);
  obj1.free();

  outStr->printf( "startxref\r\n");
  outStr->printf( "%i\r\n", uxrefOffset);
  outStr->printf( "%%%%EOF\r\n");
}

void PDFDoc::writeXRefTableTrailer(Guint uxrefOffset, XRef *uxref, GBool writeAllEntries,
                                   int uxrefSize, OutStream* outStr, GBool incrUpdate)
{
  const char *fileNameA = fileName ? fileName->getCString() : NULL;
  // file size (doesn't include the trailer)
  unsigned int fileSize = 0;
  int c;
  str->reset();
  while ((c = str->getChar()) != EOF) {
    fileSize++;
  }
  str->close();
  Ref ref;
  ref.num = getXRef()->getRootNum();
  ref.gen = getXRef()->getRootGen();
  Dict * trailerDict = createTrailerDict(uxrefSize, incrUpdate, getStartXRef(), &ref,
                                         getXRef(), fileNameA, fileSize,signature_mode);
  writeXRefTableTrailer(trailerDict, uxref, writeAllEntries, uxrefOffset, outStr, getXRef());
  delete trailerDict;
}

void PDFDoc::writeHeader(OutStream *outStr, int major, int minor)
{
   outStr->printf("%%PDF-%d.%d\n", major, minor);
   outStr->printf("%%\xE2\xE3\xCF\xD3\n");
}

void PDFDoc::markDictionnary (Dict* dict, XRef * xRef, XRef *countRef, Guint numOffset)
{
  Object obj1;
  for (int i=0; i<dict->getLength(); i++) {
    markObject(dict->getValNF(i, &obj1), xRef, countRef, numOffset);
    obj1.free();
  }
}

void PDFDoc::markObject (Object* obj, XRef *xRef, XRef *countRef, Guint numOffset)
{
  Array *array;
  Object obj1;

  switch (obj->getType()) {
    case objArray:
      array = obj->getArray();
      for (int i=0; i<array->getLength(); i++) {
        markObject(array->getNF(i, &obj1), xRef, countRef, numOffset);
        obj1.free();
      }
      break;
    case objDict:
      markDictionnary (obj->getDict(), xRef, countRef, numOffset);
      break;
    case objStream: 
      {
        Stream *stream = obj->getStream();
        markDictionnary (stream->getDict(), xRef, countRef, numOffset);
      }
      break;
    case objRef:
      {
        if (obj->getRef().num + (int) numOffset >= xRef->getNumObjects() || xRef->getEntry(obj->getRef().num + numOffset)->type == xrefEntryFree) {
          if (getXRef()->getEntry(obj->getRef().num)->type == xrefEntryFree) {
            return;  // already marked as free => should be replaced
          }
          xRef->add(obj->getRef().num + numOffset, obj->getRef().gen, 0, gTrue);
          if (getXRef()->getEntry(obj->getRef().num)->type == xrefEntryCompressed) {
            xRef->getEntry(obj->getRef().num + numOffset)->type = xrefEntryCompressed;
          }
        }
        if (obj->getRef().num + (int) numOffset >= countRef->getNumObjects() || 
            countRef->getEntry(obj->getRef().num + numOffset)->type == xrefEntryFree)
        {
          countRef->add(obj->getRef().num + numOffset, 1, 0, gTrue);
        } else {
          XRefEntry *entry = countRef->getEntry(obj->getRef().num + numOffset);
          entry->gen++;
          if (entry->gen > 9)
            break;
        } 
        Object obj1;
        getXRef()->fetch(obj->getRef().num, obj->getRef().gen, &obj1);
        markObject(&obj1, xRef, countRef, numOffset);
        obj1.free();
      }
      break;
    default:
      break;
  }
}

void PDFDoc::replacePageDict(int pageNo, int rotate,
                             PDFRectangle *mediaBox, 
                             PDFRectangle *cropBox, Object *pageCTM)
{
  Ref *refPage = getCatalog()->getPageRef(pageNo);
  Object page;
  getXRef()->fetch(refPage->num, refPage->gen, &page);
  Dict *pageDict = page.getDict();
  pageDict->remove("MediaBox");
  pageDict->remove("CropBox");
  pageDict->remove("ArtBox");
  pageDict->remove("BleedBox");
  pageDict->remove("TrimBox");
  pageDict->remove("Rotate");
  Object mediaBoxObj;
  mediaBoxObj.initArray(getXRef());
  Object murx;
  murx.initReal(mediaBox->x1);
  Object mury;
  mury.initReal(mediaBox->y1);
  Object mllx;
  mllx.initReal(mediaBox->x2);
  Object mlly;
  mlly.initReal(mediaBox->y2);
  mediaBoxObj.arrayAdd(&murx);
  mediaBoxObj.arrayAdd(&mury);
  mediaBoxObj.arrayAdd(&mllx);
  mediaBoxObj.arrayAdd(&mlly);
  pageDict->add(copyString("MediaBox"), &mediaBoxObj);

  if (cropBox != NULL) {
    Object cropBoxObj;
    cropBoxObj.initArray(getXRef());
    Object curx;
    curx.initReal(cropBox->x1);
    Object cury;
    cury.initReal(cropBox->y1);
    Object cllx;
    cllx.initReal(cropBox->x2);
    Object clly;
    clly.initReal(cropBox->y2);
    cropBoxObj.arrayAdd(&curx);
    cropBoxObj.arrayAdd(&cury);
    cropBoxObj.arrayAdd(&cllx);
    cropBoxObj.arrayAdd(&clly);
    pageDict->add(copyString("CropBox"), &cropBoxObj);
    cropBoxObj.getArray()->incRef();
    pageDict->add(copyString("TrimBox"), &cropBoxObj);

  } else {
    mediaBoxObj.getArray()->incRef();
    pageDict->add(copyString("TrimBox"), &mediaBoxObj);
  }

  Object rotateObj;
  rotateObj.initInt(rotate);
  pageDict->add(copyString("Rotate"), &rotateObj);
  
  getXRef()->setModifiedObject(&page, *refPage);
  page.free();
}

void PDFDoc::markPageObjects(Dict *pageDict, XRef *xRef, XRef *countRef, Guint numOffset) 
{
  pageDict->remove("Names");
  pageDict->remove("OpenAction");
  pageDict->remove("Outlines");
  pageDict->remove("StructTreeRoot");

  for (int n = 0; n < pageDict->getLength(); n++) {
    const char *key = pageDict->getKey(n);
    Object value; pageDict->getValNF(n, &value);
    if (strcmp(key, "Parent") != 0 &&
	      strcmp(key, "Pages") != 0) {
      markObject(&value, xRef, countRef, numOffset);
    }
    value.free();
  }
}

Guint PDFDoc::writePageObjects(OutStream *outStr, XRef *xRef, Guint numOffset) 
{
  Guint objectsCount = 0; //count the number of objects in the XRef(s)

  for (int n = numOffset; n < xRef->getNumObjects(); n++) {
    if (xRef->getEntry(n)->type != xrefEntryFree) {
      Object obj;
      Ref ref;
      ref.num = n;
      ref.gen = xRef->getEntry(n)->gen;
      objectsCount++;
      getXRef()->fetch(ref.num - numOffset, ref.gen, &obj);
      Guint offset = writeObject(&obj, &ref, outStr, xRef, numOffset);
      xRef->add(ref.num, ref.gen, offset, gTrue);
      obj.free();
    }
  }
  return objectsCount;
}

#ifndef DISABLE_OUTLINE
Outline *PDFDoc::getOutline()
{
  if (!outline) {
    // read outline
    outline = new Outline(catalog->getOutline(), xref);
  }

  return outline;
}
#endif

PDFDoc *PDFDoc::ErrorPDFDoc(int errorCode, GooString *fileNameA)
{
  PDFDoc *doc = new PDFDoc();
  doc->errCode = errorCode;
  doc->fileName = fileNameA;

  return doc;
}

Guint PDFDoc::strToUnsigned(char *s) {
  Guint x, d;
  char *p;

  x = 0;
  for (p = s; *p && isdigit(*p & 0xff); ++p) {
    d = *p - '0';
    if (x > (UINT_MAX - d) / 10) {
      break;
    }
    x = 10 * x + d;
  }
  return x;
}

// Read the 'startxref' position.
Guint PDFDoc::getStartXRef()
{
  if (startXRefPos == ~(Guint)0) {

    if (isLinearized()) {
      char buf[linearizationSearchSize+1];
      int c, n, i;

      str->setPos(0);
      for (n = 0; n < linearizationSearchSize; ++n) {
        if ((c = str->getChar()) == EOF) {
          break;
        }
        buf[n] = c;
      }
      buf[n] = '\0';

      // find end of first obj (linearization dictionary)
      startXRefPos = 0;
      for (i = 0; i < n; i++) {
        if (!strncmp("endobj", &buf[i], 6)) {
	  i += 6;
	  //skip whitespace 
	  while (buf[i] && Lexer::isSpace(buf[i])) ++i;
	  startXRefPos = i;
	  break;
        }
      }
    } else {
      char buf[xrefSearchSize+1];
      char *p;
      int c, n, i;

      // read last xrefSearchSize bytes
      str->setPos(xrefSearchSize, -1);
      for (n = 0; n < xrefSearchSize; ++n) {
        if ((c = str->getChar()) == EOF) {
          break;
        }
        buf[n] = c;
      }
      buf[n] = '\0';

      // find startxref
      for (i = n - 9; i >= 0; --i) {
        if (!strncmp(&buf[i], "startxref", 9)) {
          break;
        }
      }
      if (i < 0) {
        startXRefPos = 0;
      } else {
        for (p = &buf[i+9]; isspace(*p); ++p) ;
        startXRefPos =  strToUnsigned(p);
      }
    }

  }

  return startXRefPos;
}

Guint PDFDoc::getMainXRefEntriesOffset()
{
  Guint mainXRefEntriesOffset = 0;

  if (isLinearized()) {
    mainXRefEntriesOffset = getLinearization()->getMainXRefEntriesOffset();
  }

  return mainXRefEntriesOffset;
}

int PDFDoc::getNumPages()
{
  if (isLinearized()) {
    int n;
    if ((n = getLinearization()->getNumPages())) {
      return n;
    }
  }

  return catalog->getNumPages();
}

Page *PDFDoc::parsePage(int page)
{
  Page *p = NULL;
  Object obj;
  Ref pageRef;
  Dict *pageDict;

  pageRef.num = getHints()->getPageObjectNum(page);
  if (!pageRef.num) {
    error(errSyntaxWarning, -1, "Failed to get object num from hint tables for page {0:d}", page);
    return NULL;
  }

  // check for bogus ref - this can happen in corrupted PDF files
  if (pageRef.num < 0 || pageRef.num >= xref->getNumObjects()) {
    error(errSyntaxWarning, -1, "Invalid object num ({0:d}) for page {1:d}", pageRef.num, page);
    return NULL;
  }

  pageRef.gen = xref->getEntry(pageRef.num)->gen;
  xref->fetch(pageRef.num, pageRef.gen, &obj);
  if (!obj.isDict("Page")) {
    obj.free();
    error(errSyntaxWarning, -1, "Object ({0:d} {1:d}) is not a pageDict", pageRef.num, pageRef.gen);
    return NULL;
  }
  pageDict = obj.getDict();

  p = new Page(this, page, pageDict, pageRef,
               new PageAttrs(NULL, pageDict), catalog->getForm());
  obj.free();

  return p;
}

Page *PDFDoc::getPage(int page)
{
  if ((page < 1) || page > getNumPages()) return NULL;
  
  if (isLinearized()) {
    if (!pageCache) {
      pageCache = (Page **) gmallocn(getNumPages(), sizeof(Page *));
      for (int i = 0; i < getNumPages(); i++) {
        pageCache[i] = NULL;
      }
    }
    if (!pageCache[page-1]) {
      pageCache[page-1] = parsePage(page);
    }
    if (pageCache[page-1]) {
       return pageCache[page-1];
    } else {
       error(errSyntaxWarning, -1, "Failed parsing page {0:d} using hint tables", page);
    }
  }
  

  return catalog->getPage(page);
}
