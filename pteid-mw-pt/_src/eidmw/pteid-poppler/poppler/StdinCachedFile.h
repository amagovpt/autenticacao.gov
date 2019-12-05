/*-****************************************************************************

 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

//========================================================================
//
// StdinCachedFile.h
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2010 Hib Eris <hib@hiberis.nl>
// Copyright 2010 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#ifndef STDINCACHELOADER_H
#define STDINCACHELOADER_H

#include "CachedFile.h"

class StdinCacheLoader : public CachedFileLoader {

public:

  size_t init(GooString *dummy, CachedFile* cachedFile);
  int load(const std::vector<ByteRange> &ranges, CachedFileWriter *writer);

};

#endif

