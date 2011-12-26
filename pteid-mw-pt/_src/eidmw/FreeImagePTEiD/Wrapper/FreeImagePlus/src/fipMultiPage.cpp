// ==========================================================
// fipMultiPage class implementation
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

#include "FreeImagePlus.h"

fipMultiPage::fipMultiPage(BOOL_FREEIMAGE keep_cache_in_memory) : _mpage(NULL), _bMemoryCache(keep_cache_in_memory) {
}

fipMultiPage::~fipMultiPage() {
	if(_mpage) {
		// close the stream
		close(0);
	}
}

BOOL_FREEIMAGE fipMultiPage::isValid() const {
	return (NULL != _mpage) ? TRUE : FALSE;
}

BOOL_FREEIMAGE fipMultiPage::open(const char* lpszPathName, BOOL_FREEIMAGE create_new, BOOL_FREEIMAGE read_only, int flags) {
	// try to guess the file format from the filename
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(lpszPathName);

	// open the stream
	_mpage = FreeImage_OpenMultiBitmap(fif, lpszPathName, create_new, read_only, _bMemoryCache, flags);

	return (NULL != _mpage ) ? TRUE : FALSE;
}

BOOL_FREEIMAGE fipMultiPage::open(fipMemoryIO& memIO, int flags) {
	// try to guess the file format from the memory handle
	FREE_IMAGE_FORMAT fif = memIO.getFileType();

	// open the stream
	_mpage = memIO.loadMultiPage(fif, flags);

	return (NULL != _mpage ) ? TRUE : FALSE;
}

BOOL_FREEIMAGE fipMultiPage::open(FreeImageIO *io, fi_handle handle, int flags) {
	// try to guess the file format from the handle
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(io, handle, 0);

	// open the stream
	_mpage = FreeImage_OpenMultiBitmapFromHandle(fif, io, handle, flags);

	return (NULL != _mpage ) ? TRUE : FALSE;
}

BOOL_FREEIMAGE fipMultiPage::close(int flags) {
	BOOL_FREEIMAGE bSuccess = FALSE;
	if(_mpage) {
		// close the stream
		bSuccess = FreeImage_CloseMultiBitmap(_mpage, flags);
		_mpage = NULL;
	}

	return bSuccess;
}

BOOL_FREEIMAGE fipMultiPage::saveToHandle(FREE_IMAGE_FORMAT fif, FreeImageIO *io, fi_handle handle, int flags) const {
	BOOL_FREEIMAGE bSuccess = FALSE;
	if(_mpage) {
		bSuccess = FreeImage_SaveMultiBitmapToHandle(fif, _mpage, io, handle, flags);
	}

	return bSuccess;
}

BOOL_FREEIMAGE fipMultiPage::saveToMemory(FREE_IMAGE_FORMAT fif, fipMemoryIO& memIO, int flags) const {
	BOOL_FREEIMAGE bSuccess = FALSE;
	if(_mpage) {
		bSuccess = memIO.saveMultiPage(fif, _mpage, flags);
	}

	return bSuccess;
}

int fipMultiPage::getPageCount() const {
	return _mpage ? FreeImage_GetPageCount(_mpage) : 0;
}

void fipMultiPage::appendPage(fipImage& image) {
	if(_mpage) {
		FreeImage_AppendPage(_mpage, image);
	}
}

void fipMultiPage::insertPage(int page, fipImage& image) {
	if(_mpage) {
		FreeImage_InsertPage(_mpage, page, image);
	}
}

void fipMultiPage::deletePage(int page) {
	if(_mpage) {
		FreeImage_DeletePage(_mpage, page);
	}
}

BOOL_FREEIMAGE fipMultiPage::movePage(int target, int source) {
	return _mpage ? FreeImage_MovePage(_mpage, target, source) : FALSE;
}

FIBITMAP* fipMultiPage::lockPage(int page) {
	return _mpage ? FreeImage_LockPage(_mpage, page) : NULL;
}

void fipMultiPage::unlockPage(fipImage& image, BOOL_FREEIMAGE changed) {
	if(_mpage) {
		FreeImage_UnlockPage(_mpage, image, changed);
		// clear the image so that it becomes invalid.
		// this is possible because of the friend declaration
		image._dib = NULL;
		image._bHasChanged = FALSE;
	}
}

BOOL_FREEIMAGE fipMultiPage::getLockedPageNumbers(int *pages, int *count) const {
	return _mpage ? FreeImage_GetLockedPageNumbers(_mpage, pages, count) : FALSE;
}

