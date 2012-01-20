/*
 * PhotoPteid.h
 *
 *  Created on: Jan 19, 2012
 *      Author: ruim
 */
#pragma once

#ifndef PHOTOPTEID_H_
#define PHOTOPTEID_H_

#include "Export.h"
#include "ByteArray.h"

namespace eIDMW {

EIDMW_CMN_API class PhotoPteid {
public:
	PhotoPteid();
	PhotoPteid(CByteArray &photo, CByteArray &cbeff, CByteArray &facialrechdr, CByteArray &facialinfo, CByteArray &imageinfo);
	virtual ~PhotoPteid();
	CByteArray *getPhotoPNG();						/**< Return field Photo in png format */
	CByteArray *getPhotoRaw();						/**< Return field Photo in the original jp2 format */
	CByteArray *getCbeff();
	CByteArray *getFacialrechdr();
	CByteArray *getFacialinfo();
	CByteArray *getImageinfo();

protected:
	void setPhoto(CByteArray &photo);

private:
	CByteArray *cbeff;
	CByteArray *facialrechdr;
	CByteArray *facialinfo;
	CByteArray *imageinfo;
	CByteArray *photoPNG;
	CByteArray *photoRAW;
};

} /* namespace eIDMW */
#endif /* PHOTOPTEID_H_ */
