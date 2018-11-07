/*
 * PhotoPteid.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: ruim
 */

#include "PhotoPteid.h"
#include <FreeImagePTEiD.h>
#include <iostream>
using namespace std;
namespace eIDMW {

	PhotoPteid::PhotoPteid(){
	}


	PhotoPteid::PhotoPteid(CByteArray &_photo, CByteArray &_cbeff, CByteArray &_facialrechdr, CByteArray &_facialinfo, CByteArray &_imageinfo){
		photoRAW = new CByteArray(_photo);
		cbeff = new CByteArray(_cbeff);
		facialrechdr = new CByteArray(_facialrechdr);
		facialinfo = new CByteArray(_facialinfo);
		imageinfo = new CByteArray(_imageinfo);
		photoPNG = NULL;

	}

	PhotoPteid::~PhotoPteid(){
		if (photoPNG)
			delete photoPNG;
		if (photoRAW)
			delete photoRAW;
		if (cbeff)
			delete cbeff;
		if (facialrechdr)
			delete facialrechdr;
		if (facialinfo)
			delete facialinfo;
		if (imageinfo)
			delete imageinfo;
	}

	CByteArray *PhotoPteid::getPhotoPNG(){

		if (!photoPNG){
			BYTE *mem_buffer = NULL;
			DWORD size_in_bytes = 0;

			FreeImage_Initialise(TRUE);

			FIMEMORY *source = FreeImage_OpenMemory((BYTE*)photoRAW->GetBytes(),(DWORD)(photoRAW->Size()));
			FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileTypeFromMemory(source, 0);
			FIBITMAP *check = FreeImage_LoadFromMemory(imageFormat, source, JP2_DEFAULT);
			FIMEMORY *destination = FreeImage_OpenMemory();
			FreeImage_SaveToMemory(FIF_PNG, check, destination, PNG_Z_BEST_COMPRESSION);
			FreeImage_AcquireMemory(destination, &mem_buffer, &size_in_bytes);
			photoPNG = new CByteArray((const unsigned char*)mem_buffer, (unsigned long)size_in_bytes);
			FreeImage_Unload(check);
			FreeImage_CloseMemory(source);
			FreeImage_CloseMemory(destination);

			FreeImage_DeInitialise();
		}

		return photoPNG;
	}

	CByteArray *PhotoPteid::getPhotoRaw(){
		return photoRAW;
	}

	CByteArray *PhotoPteid::getCbeff(){
		return cbeff;
	}

	CByteArray *PhotoPteid::getFacialrechdr(){
		return facialrechdr;
	}

	CByteArray *PhotoPteid::getFacialinfo(){
		return facialinfo;
	}

	CByteArray *PhotoPteid::getImageinfo(){
		return imageinfo;
	}

} /* namespace eIDMW */
