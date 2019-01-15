//========================================================================
//
// Catalog.h
//
// Copyright 1996-2007 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2005 Kristian Høgsberg <krh@redhat.com>
// Copyright (C) 2005, 2007, 2009-2011 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2005 Jonathan Blandford <jrb@redhat.com>
// Copyright (C) 2005, 2006, 2008 Brad Hards <bradh@frogmouth.net>
// Copyright (C) 2007 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2008, 2011 Pino Toscano <pino@kde.org>
// Copyright (C) 2010 Hib Eris <hib@hiberis.nl>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#ifndef CATALOG_H
#define CATALOG_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include "Object.h"

#define ESTIMATED_LEN 30000
#define PLACEHOLDER_LEN ESTIMATED_LEN
#define CUSTOM_IMAGE_BITMAP_WIDTH 185
#define CUSTOM_IMAGE_BITMAP_HEIGHT 41
#include <vector>

class PDFDoc;
class XRef;
class Object;
class Page;
class PageAttrs;
struct Ref;
class LinkDest;
class PageLabelInfo;
class Form;
class OCGs;
class ViewerPreferences;
class FileSpec;
class PDFRectangle;

//------------------------------------------------------------------------
// NameTree
//------------------------------------------------------------------------

class NameTree {
public:
  NameTree();
  ~NameTree();
  void init(XRef *xref, Object *tree);
  GBool lookup(GooString *name, Object *obj);
  int numEntries() { return length; };
  // iterator accessor, note it returns a shallow copy, do not free the object
  Object getValue(int i);
  GooString *getName(int i);

private:
  struct Entry {
    Entry(Array *array, int index);
    ~Entry();
    GooString name;
    Object value;
    void free();
    static int cmp(const void *key, const void *entry);
  };

  void parse(Object *tree);
  void addEntry(Entry *entry);

  XRef *xref;
  Object *root;
  Entry **entries;
  int size, length; // size is the number of entries in
                    // the array of Entry*
                    // length is the number of real Entry
};


class SignatureSignerInfo {
public:
  const char * name;
  const char * civil_number;
  //Professional attributes info
  const char * attribute_provider;
  const char * attribute_name;
};

//------------------------------------------------------------------------
// Catalog
//------------------------------------------------------------------------

class Catalog {
public:

  // Constructor.
  Catalog(PDFDoc *docA);

  // Destructor.
  ~Catalog();

  // Is catalog valid?
  GBool isOk() { return ok; }

  GBool setSigFlags(Object *obj, int value);

  GBool addSigRefToPage(Ref *, Object* sig_ref);

  int setSignatureByteRange(unsigned long sig_contents_offset, unsigned long estimated_len,
		  unsigned long filesize);

  // Get number of pages.
  int getNumPages();

  bool getUS3Dict();

  void setIncrementalSignature(bool);

  void prepareSignature(PDFRectangle *rect, SignatureSignerInfo *signer_info, Ref *first_page_ref, const char *location,
	                      const char *reason, unsigned long, int page, int sig_sector, 
    unsigned char *img_data, unsigned long img_length, bool isPTLanguage, bool isCCSignature);
  
  Ref addFontDict(const char *basefont, const char *name);
  Ref addImageXObject(int width, int height, unsigned char *data, unsigned long length_in_bytes);

  Ref newXObject(char *plain_text_stream,
	 int height, int width, bool needs_font, bool needs_image, unsigned char *img_data = NULL, unsigned long img_length = 0);

  void addSignatureAppearance(Object *signature_field, SignatureSignerInfo *signer_info,
	     char *date_str,	const char* location, const char* reason, int rect_x, int rect_y,
		 unsigned char *img_data, unsigned long img_length, int rotate_signature, bool isPTLanguage);
  void addSignatureAppearanceSCAP(Object *signature_field, SignatureSignerInfo *signer_info,
             char *date_str,	const char* location, const char* reason, int rect_x, int rect_y,
                 unsigned char *img_data, unsigned long img_length, int rotate_signature, bool isPTLanguage);
  void addSignatureAppearance(Object *parent, int, int);
  void closeSignature(const char *signature_contents, unsigned long len);

  // Get a page.
  Page *getPage(int i);

  // Get the reference for a page object.
  Ref *getPageRef(int i);

  // Return base URI, or NULL if none.
  GooString *getBaseURI() { return baseURI; }

  // Return the contents of the metadata stream, or NULL if there is
  // no metadata.
  GooString *readMetadata();

  // Return the structure tree root object.
  Object *getStructTreeRoot();

  // Find a page, given its object ID.  Returns page number, or 0 if
  // not found.
  int findPage(int num, int gen);

  // Find a named destination.  Returns the link destination, or
  // NULL if <name> is not a destination.
  LinkDest *findDest(GooString *name);

  Object *getDests();

  // Get the number of embedded files
  int numEmbeddedFiles() { return getEmbeddedFileNameTree()->numEntries(); }

  // Get the i'th file embedded (at the Document level) in the document
  FileSpec *embeddedFile(int i);

  // Get the number of javascript scripts
  int numJS() { return getJSNameTree()->numEntries(); }

  // Get the i'th JavaScript script (at the Document level) in the document
  GooString *getJS(int i);

  // Convert between page indices and page labels.
  GBool labelToIndex(GooString *label, int *index);
  GBool indexToLabel(int index, GooString *label);

  Object *getOutline();

  Object *getAcroForm() { return &acroForm; }

  OCGs *getOptContentConfig() { return optContent; }

  Form* getForm();

  ViewerPreferences *getViewerPreferences();

  enum PageMode {
    pageModeNone,
    pageModeOutlines,
    pageModeThumbs,
    pageModeFullScreen,
    pageModeOC,
    pageModeAttach,
    pageModeNull
  };
  enum PageLayout {
    pageLayoutNone,
    pageLayoutSinglePage,
    pageLayoutOneColumn,
    pageLayoutTwoColumnLeft,
    pageLayoutTwoColumnRight,
    pageLayoutTwoPageLeft,
    pageLayoutTwoPageRight,
    pageLayoutNull
  };

  // Returns the page mode.
  PageMode getPageMode();
  PageLayout getPageLayout();

private:

  // Get page label info.
  PageLabelInfo *getPageLabelInfo();

  PDFDoc *doc;
  XRef *xref;			// the xref table for this PDF file
  Page **pages;			// array of pages
  Ref *pageRefs;		// object ID for each page
  int lastCachedPage;
  std::vector<Dict *> *pagesList;
  std::vector<Ref> *pagesRefList;
  std::vector<PageAttrs *> *attrsList;
  std::vector<int> *kidsIdxList;
  Form *form;
  ViewerPreferences *viewerPrefs;
  Object catDict;       //****Signing patch**** The actual raw dict, for later modification
 // bool m_is_compressed;	   	//is the catalog object compressed, this matter if we need to change it
  Ref m_sig_ref;
  Object *m_sig_dict;
  bool incremental_update;
  bool small_signature_format;

  int numPages;			// number of pages
  int pagesSize;		// size of pages array
  Object dests;			// named destination dictionary
  Object names;			// named names dictionary
  NameTree *destNameTree;	// named destination name-tree
  NameTree *embeddedFileNameTree;  // embedded file name-tree
  NameTree *jsNameTree;		// Java Script name-tree
  GooString *baseURI;		// base URI for URI-type links
  Object metadata;		// metadata stream
  Object structTreeRoot;	// structure tree root dictionary
  Object outline;		// outline dictionary
  Object acroForm;		// AcroForm dictionary
  Object viewerPreferences;     // ViewerPreference dictionary
  OCGs *optContent;		// Optional Content groups
  GBool ok;			// true if catalog is valid
  PageLabelInfo *pageLabelInfo; // info about page labels
  PageMode pageMode;		// page mode
  PageLayout pageLayout;	// page layout

  GBool useCCLogo;

  GBool cachePageTree(int page); // Cache first <page> pages.
  Object *findDestInTree(Object *tree, GooString *name, Object *obj);

  Object *getNames();
  NameTree *getDestNameTree();
  NameTree *getEmbeddedFileNameTree();
  NameTree *getJSNameTree();

};

#endif
