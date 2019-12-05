/*-****************************************************************************

 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

//========================================================================
//
// StdinPDFDocBuilder.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2010 Hib Eris <hib@hiberis.nl>
// Copyright 2010 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#ifndef STDINPDFDOCBUILDER_H
#define STDINPDFDOCBUILDER_H

#include "PDFDocBuilder.h"

//------------------------------------------------------------------------
// StdinPDFDocBuilder
//
// The StdinPDFDocBuilder implements a PDFDocBuilder that read from stdin.
//------------------------------------------------------------------------

class StdinPDFDocBuilder : public PDFDocBuilder {

public:

  PDFDoc *buildPDFDoc(const GooString &uri, GooString *ownerPassword = NULL,
    GooString *userPassword = NULL, void *guiDataA = NULL);
  GBool supports(const GooString &uri);

};

#endif /* STDINPDFDOCBUILDER_H */
