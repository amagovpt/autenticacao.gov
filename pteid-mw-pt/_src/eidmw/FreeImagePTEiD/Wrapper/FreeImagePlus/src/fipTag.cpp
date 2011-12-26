// ==========================================================
// fipTag class implementation
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#include <string.h>
#include "FreeImagePlus.h"

fipTag::fipTag() {
	_tag = FreeImage_CreateTag();
}

fipTag::~fipTag() {
	FreeImage_DeleteTag(_tag);
}

BOOL_FREEIMAGE fipTag::setKeyValue(const char *key, const char *value) {
	if(_tag) {
		FreeImage_DeleteTag(_tag);
		_tag = NULL;
	}
	// create a tag
	_tag = FreeImage_CreateTag();
	if(_tag) {
		BOOL_FREEIMAGE bSuccess = TRUE;
		// fill the tag
		DWORD_FREEIMAGE tag_length = (DWORD_FREEIMAGE)(strlen(value) + 1);
		bSuccess &= FreeImage_SetTagKey(_tag, key);
		bSuccess &= FreeImage_SetTagLength(_tag, tag_length);
		bSuccess &= FreeImage_SetTagCount(_tag, tag_length);
		bSuccess &= FreeImage_SetTagType(_tag, FIDT_ASCII);
		bSuccess &= FreeImage_SetTagValue(_tag, value);
		return bSuccess;
	}
	return FALSE;
}

fipTag::fipTag(const fipTag& tag) {
	_tag = FreeImage_CloneTag(tag._tag);
}

fipTag& fipTag::operator=(const fipTag& tag) {
	if(this != &tag) {
		if(_tag) FreeImage_DeleteTag(_tag);
		_tag = FreeImage_CloneTag(tag._tag);
	}
	return *this;
}

fipTag& fipTag::operator=(FITAG *tag) {
	if(_tag) FreeImage_DeleteTag(_tag);
	_tag = tag;
	return *this;
}

BOOL_FREEIMAGE fipTag::isValid() const {
	return (_tag != NULL) ? TRUE : FALSE;
}

const char* fipTag::getKey() const {
	return FreeImage_GetTagKey(_tag);
}

const char* fipTag::getDescription() const {
	return FreeImage_GetTagDescription(_tag);
}

WORD fipTag::getID() const {
	return FreeImage_GetTagID(_tag);
}

FREE_IMAGE_MDTYPE fipTag::getType() const {
	return FreeImage_GetTagType(_tag);
}

DWORD_FREEIMAGE fipTag::getCount() const {
	return FreeImage_GetTagCount(_tag);
}

DWORD_FREEIMAGE fipTag::getLength() const {
	return FreeImage_GetTagLength(_tag);
}

const void* fipTag::getValue() const {
	return FreeImage_GetTagValue(_tag);
}

BOOL_FREEIMAGE fipTag::setKey(const char *key) {
	return FreeImage_SetTagKey(_tag, key);
}

BOOL_FREEIMAGE fipTag::setDescription(const char *description) {
	return FreeImage_SetTagDescription(_tag, description);
}

BOOL_FREEIMAGE fipTag::setID(WORD id) {
	return FreeImage_SetTagID(_tag, id);
}

BOOL_FREEIMAGE fipTag::setType(FREE_IMAGE_MDTYPE type) {
	return FreeImage_SetTagType(_tag, type);
}

BOOL_FREEIMAGE fipTag::setCount(DWORD_FREEIMAGE count) {
	return FreeImage_SetTagCount(_tag, count);
}

BOOL_FREEIMAGE fipTag::setLength(DWORD_FREEIMAGE length) {
	return FreeImage_SetTagLength(_tag, length);
}

BOOL_FREEIMAGE fipTag::setValue(const void *value) {
	return FreeImage_SetTagValue(_tag, value);
}

const char* fipTag::toString(FREE_IMAGE_MDMODEL model, char *Make) const {
	return FreeImage_TagToString(model, _tag, Make);
}
