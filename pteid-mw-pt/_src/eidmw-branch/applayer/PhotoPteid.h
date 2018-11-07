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

class PhotoPteid {
public:
	EIDMW_APL_API PhotoPteid();
	EIDMW_APL_API PhotoPteid(CByteArray &photo, CByteArray &cbeff, CByteArray &facialrechdr, CByteArray &facialinfo, CByteArray &imageinfo);
	EIDMW_APL_API virtual ~PhotoPteid();
	EIDMW_APL_API CByteArray *getPhotoPNG();						/**< Return field Photo in png format */
	EIDMW_APL_API CByteArray *getPhotoRaw();						/**< Return field Photo in the original jp2 format */
	EIDMW_APL_API CByteArray *getCbeff();
	EIDMW_APL_API CByteArray *getFacialrechdr();
	EIDMW_APL_API CByteArray *getFacialinfo();
	EIDMW_APL_API CByteArray *getImageinfo();

protected:
	EIDMW_APL_API void setPhoto(CByteArray &photo);

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
