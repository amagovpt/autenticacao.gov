//========================================================================
//
// Page.cc
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
// Copyright (C) 2005 Jeff Muizelaar <jeff@infidigm.net>
// Copyright (C) 2005-2012 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2006-2008 Pino Toscano <pino@kde.org>
// Copyright (C) 2006 Nickolay V. Shmyrev <nshmyrev@yandex.ru>
// Copyright (C) 2006 Scott Turner <scotty1024@mac.com>
// Copyright (C) 2006-2011 Carlos Garcia Campos <carlosgc@gnome.org>
// Copyright (C) 2007 Julien Rebetez <julienr@svn.gnome.org>
// Copyright (C) 2008 Iñigo Martínez <inigomartinez@gmail.com>
// Copyright (C) 2008 Brad Hards <bradh@kde.org>
// Copyright (C) 2008 Ilya Gorenbein <igorenbein@finjan.com>
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

#include <stddef.h>
#include <limits.h>
#include "Object.h"
#include "Array.h"
#include "Dict.h"
#include "PDFDoc.h"
#include "XRef.h"
#include "Link.h"
//#include "OutputDev.h"
#include "Annot.h"
//#include "TextOutputDev.h"
#include "Form.h"
#include "Error.h"
#include "Page.h"
#include "Catalog.h"
#include "Form.h"

//------------------------------------------------------------------------
// PDFRectangle
//------------------------------------------------------------------------

void PDFRectangle::clipTo(PDFRectangle *rect) {
  if (x1 < rect->x1) {
    x1 = rect->x1;
  } else if (x1 > rect->x2) {
    x1 = rect->x2;
  }
  if (x2 < rect->x1) {
    x2 = rect->x1;
  } else if (x2 > rect->x2) {
    x2 = rect->x2;
  }
  if (y1 < rect->y1) {
    y1 = rect->y1;
  } else if (y1 > rect->y2) {
    y1 = rect->y2;
  }
  if (y2 < rect->y1) {
    y2 = rect->y1;
  } else if (y2 > rect->y2) {
    y2 = rect->y2;
  }
}

//------------------------------------------------------------------------
// PageAttrs
//------------------------------------------------------------------------

PageAttrs::PageAttrs(PageAttrs *attrs, Dict *dict) {
  Object obj1;
  PDFRectangle mBox;
  const GBool isPage = dict->is("Page");

  // get old/default values
  if (attrs) {
    mediaBox = attrs->mediaBox;
    cropBox = attrs->cropBox;
    haveCropBox = attrs->haveCropBox;
    rotate = attrs->rotate;
    attrs->resources.copy(&resources);
  } else {
    // set default MediaBox to 8.5" x 11" -- this shouldn't be necessary
    // but some (non-compliant) PDF files don't specify a MediaBox
    mediaBox.x1 = 0;
    mediaBox.y1 = 0;
    mediaBox.x2 = 612;
    mediaBox.y2 = 792;
    cropBox.x1 = cropBox.y1 = cropBox.x2 = cropBox.y2 = 0;
    haveCropBox = gFalse;
    rotate = 0;
    resources.initNull();
  }

  // media box
  if (readBox(dict, "MediaBox", &mBox)) {
    mediaBox = mBox;
  }

  // crop box
  if (readBox(dict, "CropBox", &cropBox)) {
    haveCropBox = gTrue;
  }
  if (!haveCropBox) {
    cropBox = mediaBox;
  }

  if (isPage) {
    // cropBox can not be bigger than mediaBox
    if (cropBox.x2 - cropBox.x1 > mediaBox.x2 - mediaBox.x1)
    {
      cropBox.x1 = mediaBox.x1;
      cropBox.x2 = mediaBox.x2;
    }
    if (cropBox.y2 - cropBox.y1 > mediaBox.y2 - mediaBox.y1)
    {
      cropBox.y1 = mediaBox.y1;
      cropBox.y2 = mediaBox.y2;
    }
  }

  // other boxes
  bleedBox = cropBox;
  readBox(dict, "BleedBox", &bleedBox);
  trimBox = cropBox;
  readBox(dict, "TrimBox", &trimBox);
  artBox = cropBox;
  readBox(dict, "ArtBox", &artBox);

  // rotate
  dict->lookup("Rotate", &obj1);
  if (obj1.isInt()) {
    rotate = obj1.getInt();
  }
  obj1.free();
  while (rotate < 0) {
    rotate += 360;
  }
  while (rotate >= 360) {
    rotate -= 360;
  }

  // misc attributes
  dict->lookup("LastModified", &lastModified);
  dict->lookup("BoxColorInfo", &boxColorInfo);
  dict->lookup("Group", &group);
  dict->lookup("Metadata", &metadata);
  dict->lookup("PieceInfo", &pieceInfo);
  dict->lookup("SeparationInfo", &separationInfo);

  // resource dictionary
  dict->lookup("Resources", &obj1);
  if (obj1.isDict()) {
    resources.free();
    obj1.copy(&resources);
  }
  obj1.free();
}

PageAttrs::~PageAttrs() {
  lastModified.free();
  boxColorInfo.free();
  group.free();
  metadata.free();
  pieceInfo.free();
  separationInfo.free();
  resources.free();
}

void PageAttrs::clipBoxes() {
  cropBox.clipTo(&mediaBox);
  bleedBox.clipTo(&mediaBox);
  trimBox.clipTo(&mediaBox);
  artBox.clipTo(&mediaBox);
}

GBool PageAttrs::readBox(Dict *dict, const char *key, PDFRectangle *box) {
  PDFRectangle tmp;
  double t;
  Object obj1, obj2;
  GBool ok;

  dict->lookup(key, &obj1);
  if (obj1.isArray() && obj1.arrayGetLength() == 4) {
    ok = gTrue;
    obj1.arrayGet(0, &obj2);
    if (obj2.isNum()) {
      tmp.x1 = obj2.getNum();
    } else {
      ok = gFalse;
    }
    obj2.free();
    obj1.arrayGet(1, &obj2);
    if (obj2.isNum()) {
      tmp.y1 = obj2.getNum();
    } else {
      ok = gFalse;
    }
    obj2.free();
    obj1.arrayGet(2, &obj2);
    if (obj2.isNum()) {
      tmp.x2 = obj2.getNum();
    } else {
      ok = gFalse;
    }
    obj2.free();
    obj1.arrayGet(3, &obj2);
    if (obj2.isNum()) {
      tmp.y2 = obj2.getNum();
    } else {
      ok = gFalse;
    }
    obj2.free();
    if (tmp.x1 == 0 && tmp.x2 == 0 && tmp.y1 == 0 && tmp.y2 == 0)
      ok = gFalse;
    if (ok) {
      if (tmp.x1 > tmp.x2) {
	t = tmp.x1; tmp.x1 = tmp.x2; tmp.x2 = t;
      }
      if (tmp.y1 > tmp.y2) {
	t = tmp.y1; tmp.y1 = tmp.y2; tmp.y2 = t;
      }
      *box = tmp;
    }
  } else {
    ok = gFalse;
  }
  obj1.free();
  return ok;
}

//------------------------------------------------------------------------
// Page
//------------------------------------------------------------------------

Page::Page(PDFDoc *docA, int numA, Dict *pageDict, Ref pageRefA, PageAttrs *attrsA, Form *form) {
  Object tmp;
	
  ok = gTrue;
  doc = docA;
  xref = doc->getXRef();
  num = numA;
  duration = -1;
  annots = NULL;

  pageObj.initDict(pageDict);
  pageRef = pageRefA;

  // get attributes
  attrs = attrsA;
  attrs->clipBoxes();

  // transtion
  pageDict->lookupNF("Trans", &trans);
  if (!(trans.isRef() || trans.isDict() || trans.isNull())) {
    error(errSyntaxError, -1, "Page transition object (page {0:d}) is wrong type ({1:s})",
	  num, trans.getTypeName());
    trans.free();
  }

  // duration
  pageDict->lookupNF("Dur", &tmp);
  if (!(tmp.isNum() || tmp.isNull())) {
    error(errSyntaxError, -1, "Page duration object (page {0:d}) is wrong type ({1:s})",
	  num, tmp.getTypeName());
  } else if (tmp.isNum()) {
    duration = tmp.getNum();
  }
  tmp.free();

  // annotations
  pageDict->lookupNF("Annots", &annotsObj);
  if (!(annotsObj.isRef() || annotsObj.isArray() || annotsObj.isNull())) {
    error(errSyntaxError, -1, "Page annotations object (page {0:d}) is wrong type ({1:s})",
	  num, annotsObj.getTypeName());
    annotsObj.free();
    goto err2;
  }

  // contents
  pageDict->lookupNF("Contents", &contents);
  if (!(contents.isRef() || contents.isArray() ||
	contents.isNull())) {
    error(errSyntaxError, -1, "Page contents object (page {0:d}) is wrong type ({1:s})",
	  num, contents.getTypeName());
    contents.free();
    goto err1;
  }

  // thumb
  pageDict->lookupNF("Thumb", &thumb);
  if (!(thumb.isStream() || thumb.isNull() || thumb.isRef())) {
      error(errSyntaxError, -1, "Page thumb object (page {0:d}) is wrong type ({1:s})",
            num, thumb.getTypeName());
      thumb.initNull(); 
  }

  // actions
  pageDict->lookupNF("AA", &actions);
  if (!(actions.isDict() || actions.isNull())) {
      error(errSyntaxError, -1, "Page additional action object (page {0:d}) is wrong type ({1:s})",
            num, actions.getTypeName());
      actions.initNull();
  }
  
  return;

  trans.initNull();
 err2:
  annotsObj.initNull();
 err1:
  contents.initNull();
  ok = gFalse;
}

Page::~Page() {
  delete attrs;
  delete annots;
  pageObj.free();
  annotsObj.free();
  contents.free();
  trans.free();
  thumb.free();
  actions.free();
}

Annots *Page::getAnnots() {
  if (!annots) {
    Object obj;
    annots = new Annots(doc, getAnnots(&obj));
    obj.free();
  }

  return annots;
}

void Page::addAnnot(Annot *annot) {
  Object obj1;
  Object tmp;
  Ref annotRef = annot->getRef ();

  // Make sure we have annots before adding the new one
  // even if it's an empty list so that we can safely
  // call annots->appendAnnot(annot)
  getAnnots();

  if (annotsObj.isNull()) {
    Ref annotsRef;
    // page doesn't have annots array,
    // we have to create it

    obj1.initArray(xref);
    obj1.arrayAdd(tmp.initRef (annotRef.num, annotRef.gen));
    tmp.free();

    annotsRef = xref->addIndirectObject (&obj1);
    annotsObj.initRef(annotsRef.num, annotsRef.gen);
    pageObj.dictSet ("Annots", &annotsObj);
    xref->setModifiedObject (&pageObj, pageRef);
  } else {
    getAnnots(&obj1);
    if (obj1.isArray()) {
      obj1.arrayAdd (tmp.initRef (annotRef.num, annotRef.gen));
      if (annotsObj.isRef())
        xref->setModifiedObject (&obj1, annotsObj.getRef());
      else
        xref->setModifiedObject (&pageObj, pageRef);
    }
    obj1.free();
  }

  annots->appendAnnot(annot);

  annot->setPage(&pageRef, num);
}

void Page::removeAnnot(Annot *annot) {
  Ref annotRef = annot->getRef();
  Object annArray;

  getAnnots(&annArray);
  if (annArray.isArray()) {
    int idx = -1;
    // Get annotation position
    for (int i = 0; idx == -1 && i < annArray.arrayGetLength(); ++i) {
      Object tmp;
      Ref currAnnot = annArray.arrayGetNF(i, &tmp)->getRef();
      tmp.free();
      if (currAnnot.num == annotRef.num && currAnnot.gen == annotRef.gen) {
        idx = i;
      }
    }

    if (idx == -1) {
      error(errInternal, -1, "Annotation doesn't belong to this page");
      annArray.free();
      return;
    }
    annots->removeAnnot(annot); // Gracefully fails on popup windows
    annArray.arrayRemove(idx);
    xref->removeIndirectObject(annotRef);

    if (annotsObj.isRef()) {
      xref->setModifiedObject (&annArray, annotsObj.getRef());
    } else {
      xref->setModifiedObject (&pageObj, pageRef);
    }
  }
  annArray.free();
}

Links *Page::getLinks() {
  return new Links(getAnnots());
}

FormPageWidgets *Page::getFormWidgets() {
  return new FormPageWidgets(getAnnots(), num, doc->getCatalog()->getForm());
}


void Page::makeBox(double hDPI, double vDPI, int rotate,
		   GBool useMediaBox, GBool upsideDown,
		   double sliceX, double sliceY, double sliceW, double sliceH,
		   PDFRectangle *box, GBool *crop) {
  PDFRectangle *mediaBox, *cropBox, *baseBox;
  double kx, ky;

  mediaBox = getMediaBox();
  cropBox = getCropBox();
  if (sliceW >= 0 && sliceH >= 0) {
    baseBox = useMediaBox ? mediaBox : cropBox;
    kx = 72.0 / hDPI;
    ky = 72.0 / vDPI;
    if (rotate == 90) {
      if (upsideDown) {
	box->x1 = baseBox->x1 + ky * sliceY;
	box->x2 = baseBox->x1 + ky * (sliceY + sliceH);
      } else {
	box->x1 = baseBox->x2 - ky * (sliceY + sliceH);
	box->x2 = baseBox->x2 - ky * sliceY;
      }
      box->y1 = baseBox->y1 + kx * sliceX;
      box->y2 = baseBox->y1 + kx * (sliceX + sliceW);
    } else if (rotate == 180) {
      box->x1 = baseBox->x2 - kx * (sliceX + sliceW);
      box->x2 = baseBox->x2 - kx * sliceX;
      if (upsideDown) {
	box->y1 = baseBox->y1 + ky * sliceY;
	box->y2 = baseBox->y1 + ky * (sliceY + sliceH);
      } else {
	box->y1 = baseBox->y2 - ky * (sliceY + sliceH);
	box->y2 = baseBox->y2 - ky * sliceY;
      }
    } else if (rotate == 270) {
      if (upsideDown) {
	box->x1 = baseBox->x2 - ky * (sliceY + sliceH);
	box->x2 = baseBox->x2 - ky * sliceY;
      } else {
	box->x1 = baseBox->x1 + ky * sliceY;
	box->x2 = baseBox->x1 + ky * (sliceY + sliceH);
      }
      box->y1 = baseBox->y2 - kx * (sliceX + sliceW);
      box->y2 = baseBox->y2 - kx * sliceX;
    } else {
      box->x1 = baseBox->x1 + kx * sliceX;
      box->x2 = baseBox->x1 + kx * (sliceX + sliceW);
      if (upsideDown) {
	box->y1 = baseBox->y2 - ky * (sliceY + sliceH);
	box->y2 = baseBox->y2 - ky * sliceY;
      } else {
	box->y1 = baseBox->y1 + ky * sliceY;
	box->y2 = baseBox->y1 + ky * (sliceY + sliceH);
      }
    }
  } else if (useMediaBox) {
    *box = *mediaBox;
  } else {
    *box = *cropBox;
    *crop = gFalse;
  }
}


