#ifndef ICAODG3_H
#define ICAODG3_H

#include "ByteArray.h"

#include "IcaoDg2.h"

#include <memory>
#include <vector>

namespace eIDMW {

class BiometricInfoImage {
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
	unsigned int m_imageLength;
};

typedef struct BIT_st BITDG3;

class BiometricInfoDG3 {
public:
  BiometricInfoDG3(const BITDG3 &bio);

private:
	std::unique_ptr<BiometricHeaderTemplate> m_biometricTemplate;
	std::string m_specVersion;
	unsigned long long m_recordLength;
	unsigned short m_productIdentifierOwner;
	unsigned short m_productIdentifierType;
	unsigned short m_scannerId;
	unsigned short m_compliance;
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

private:
	unsigned int m_numberOfInstances;
	std::vector<std::unique_ptr<BiometricInfoDG3>> m_vector;
};

} // namespace eIDMW
#endif // ICAODG3_H
