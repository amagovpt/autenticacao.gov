/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */

#pragma once

#include <stdlib.h>
#include <stdio.h>

#define MAX_CONTENT    8196
#define MAX_ELEMENT    1024

namespace EidInfra
{
class Xml
{
public:
Xml(void);
virtual ~Xml(void);
bool GetNextElement(FILE * file, char *element, size_t elementMaxLength, char *content, size_t maxLength) const;
bool FindElement(FILE * file, const char *element, char *content, size_t maxLength) const;
bool seekToContentElement(FILE * file, const char *element, const char *content) const;
};
}
