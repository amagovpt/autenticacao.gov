/*
 * PhotoPteid.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: ruim
 */

#include "PhotoPteid.h"
#include <iostream>
#include "J2KHelper.h"

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
			unsigned char * mem_buffer = nullptr;
			unsigned long size_in_bytes = 0;

			convert_to_png(photoRAW->GetBytes(), photoRAW->Size(), &mem_buffer, &size_in_bytes);
			photoPNG = new CByteArray(const_cast<unsigned char*>(mem_buffer), static_cast<unsigned long>(size_in_bytes));
			if (mem_buffer){
				free(mem_buffer);
			}
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
