#ifndef ICAODG3_H
#define ICAODG3_H

#include "ByteArray.h"

#include "IcaoDg2.h"

#include <memory>
#include <vector>

namespace eIDMW {

class BiometricInfoImage {
public:
	EIDMW_APL_API unsigned int length() const;
	EIDMW_APL_API unsigned char fingerPalmPosition() const;
	EIDMW_APL_API unsigned char countOfViews() const;
	EIDMW_APL_API unsigned char viewMumber() const;
	EIDMW_APL_API unsigned char quality() const;
	EIDMW_APL_API unsigned char impressionType() const;
	EIDMW_APL_API unsigned short horizontalLineLength() const;
	EIDMW_APL_API unsigned short verticalLineLength() const;
	EIDMW_APL_API unsigned char reserved() const;
	EIDMW_APL_API const CByteArray &imageData() const;

private:
	friend class BiometricInfoDG3;
	unsigned int m_length;
	unsigned char m_fingerPalmPosition;
	unsigned char m_countOfViews;
	unsigned char m_viewMumber;
	unsigned char m_quality;
	unsigned char m_impressionType;
	unsigned short m_horizontalLineLength;
	unsigned short m_verticalLineLength;
	unsigned char m_reserved;

	CByteArray m_imageData;
};

typedef struct BIT_st BITDG3;

class BiometricInfoDG3 {
public:
  BiometricInfoDG3(const BITDG3 &bio);

  EIDMW_APL_API BiometricHeaderTemplate *biometricTemplate() const;
  EIDMW_APL_API const std::string &specVersion() const;
  EIDMW_APL_API unsigned long long recordLength() const;
  EIDMW_APL_API unsigned short scannerId() const;
  EIDMW_APL_API unsigned short imageAcquisitionLevel() const;
  EIDMW_APL_API unsigned char numFingersOrPalmImages() const;
  EIDMW_APL_API unsigned char scaleUnits() const;
  EIDMW_APL_API unsigned short xScanResolution() const;
  EIDMW_APL_API unsigned short yScanResolution() const;
  EIDMW_APL_API unsigned short xImageResolution() const;
  EIDMW_APL_API unsigned short yImageResolution() const;
  EIDMW_APL_API unsigned char pixelDepth() const;
  EIDMW_APL_API unsigned char imageCompressionAlgorithm() const;
  EIDMW_APL_API unsigned short reserved() const;
  EIDMW_APL_API const std::vector<std::shared_ptr<BiometricInfoImage>> &fingerImageVec() const;

  private:
  std::unique_ptr<BiometricHeaderTemplate> m_biometricTemplate;
  std::string m_specVersion;
  unsigned long long m_recordLength;
  unsigned short m_scannerId;
  unsigned short m_imageAcquisitionLevel;
  unsigned char m_numFingersOrPalmImages;
  unsigned char m_scaleUnits;
  unsigned short m_xScanResolution;
  unsigned short m_yScanResolution;
  unsigned short m_xImageResolution;
  unsigned short m_yImageResolution;
  unsigned char m_pixelDepth;
  unsigned char m_imageCompressionAlgorithm;
  unsigned short m_reserved;
  std::vector<std::shared_ptr<BiometricInfoImage>> m_fingerImageVec;
};

class IcaoDg3 {
public:
	IcaoDg3(const CByteArray &biometricData);

	EIDMW_APL_API const std::vector<std::unique_ptr<BiometricInfoDG3>> &biometricInstanceVec() const;
	EIDMW_APL_API unsigned int numberOfbiometrics() const;

private:
	unsigned int m_numberOfbiometrics;
	std::vector<std::unique_ptr<BiometricInfoDG3>> m_biometricInstanceVec;
};

} // namespace eIDMW
#endif // ICAODG3_H
