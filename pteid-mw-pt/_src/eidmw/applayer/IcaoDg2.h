#ifndef ICAODG2_H
#define ICAODG2_H

#include "IcaoDg2Defines.h"
#include <ByteArray.h>
#include <Export.h>

#include <memory>
#include <vector>

namespace eIDMW {

class FeaturePoint {
public:
	FeaturePoint(const unsigned char *biometricData);

	EIDMW_APL_API unsigned char type() const;
	EIDMW_APL_API unsigned char featurePoint() const;
	EIDMW_APL_API unsigned char majorCode() const;
	EIDMW_APL_API unsigned char minorCode() const;
	EIDMW_APL_API unsigned short x_coord() const;
	EIDMW_APL_API unsigned short y_coord() const;
	EIDMW_APL_API unsigned short reserved() const;

private:
	unsigned char m_type;		  // +1
	unsigned char m_featurePoint; // +1
	unsigned char m_majorCode;	  // derived from featurepoint
	unsigned char m_minorCode;	  // derived from featurepoint
	unsigned short m_x_coord;	  // +2
	unsigned short m_y_coord;	  // +2
	unsigned short m_reserved;	  // +2
};

class FaceInfoData {
public:
	EIDMW_APL_API long facialRecordDataLength() const;
	EIDMW_APL_API unsigned short numberOfFeaturePoints() const;
	EIDMW_APL_API unsigned char gender() const;
	EIDMW_APL_API Gender genderDecode() const;
	EIDMW_APL_API unsigned char eyeColor() const;
	EIDMW_APL_API EyeColor eyeColorDecode() const;
	EIDMW_APL_API unsigned char hairColour() const;
	EIDMW_APL_API HairColour hairColourDecode() const;
	EIDMW_APL_API long featureMask() const;
	EIDMW_APL_API long expression() const;
	EIDMW_APL_API long poseAngle() const;
	EIDMW_APL_API long poseAngleUncertainty() const;
	EIDMW_APL_API std::vector<FeaturePoint *> featurePoints() const;
	EIDMW_APL_API unsigned char faceImgType() const;
	EIDMW_APL_API FaceImageType faceImgTypeDecode() const;
	EIDMW_APL_API unsigned char imgDataType() const;
	EIDMW_APL_API ImageDataType imgDataTypeDecode() const;
	EIDMW_APL_API unsigned short imgWidth() const;
	EIDMW_APL_API unsigned short imgHeight() const;
	EIDMW_APL_API unsigned char colourSpace() const;
	EIDMW_APL_API ImageColourSpace colourSpaceDecode() const;
	EIDMW_APL_API unsigned char sourceType() const;
	EIDMW_APL_API SourceType sourceTypeDecode() const;
	EIDMW_APL_API unsigned short deviceType() const;
	EIDMW_APL_API unsigned short quality() const;
	EIDMW_APL_API const CByteArray &photoRawData() const;

private:
	friend class FaceInfo;
	long m_facialRecordDataLength;
	unsigned short m_numberOfFeaturePoints;
	unsigned char m_gender;
	unsigned char m_eyeColor;
	unsigned char m_hairColour;
	long m_featureMask;
	long m_expression;
	long m_poseAngle;
	std::vector<unsigned char> m_poseAngleList;
	long m_poseAngleUncertainty;
	std::vector<unsigned char> m_poseAngleUncertaintyList;
	std::vector<std::unique_ptr<FeaturePoint>> m_featurePoints;
	unsigned char m_faceImgType;
	unsigned char m_imgDataType;
	unsigned short m_imgWidth;
	unsigned short m_imgHeight;
	unsigned char m_colourSpace;
	unsigned char m_sourceType;
	unsigned short m_deviceType;
	unsigned short m_quality;
	CByteArray m_photoRawData;
};

class FaceInfo {
public:
	FaceInfo(const unsigned char *biometricData);

	EIDMW_APL_API const std::string &version() const;
	EIDMW_APL_API unsigned short encodingBytes() const;
	EIDMW_APL_API long sizeOfRecord() const;
	EIDMW_APL_API long numberOfFacialImages() const;
	EIDMW_APL_API std::vector<FaceInfoData *> faceInfoData() const;

private: // jpeg2000 5f 2e jpeg 7f 2e
	std::string m_version;
	unsigned short m_encodingBytes;
	long m_sizeOfRecord;
	long m_numberOfFacialImages;
	std::vector<std::shared_ptr<FaceInfoData>> m_faceInfoData;
};
typedef struct BIT_HEADER_st BIT_HEADER;

class BiometricHeaderTemplate {
public:
	BiometricHeaderTemplate(const BIT_HEADER &bioHeader);

	~BiometricHeaderTemplate();

	EIDMW_APL_API const CByteArray &icaoHeaderVersion() const;
	EIDMW_APL_API const CByteArray &type() const;
	EIDMW_APL_API const CByteArray &subType() const;
	EIDMW_APL_API const CByteArray &creationDateAndtime() const;
	EIDMW_APL_API const CByteArray &validPeriod() const;
	EIDMW_APL_API const CByteArray &creatorOfBiometricRefData() const;
	EIDMW_APL_API const CByteArray &formatOwner() const;
	EIDMW_APL_API const CByteArray &formatType() const;

private:
	CByteArray m_icaoHeaderVersion;
	CByteArray m_type;
	CByteArray m_subType;
	CByteArray m_creationDateAndtime;
	CByteArray m_validPeriod;
	CByteArray m_creatorOfBiometricRefData;
	CByteArray m_formatOwner;
	CByteArray m_formatType;
};
typedef struct BIT_st BIT;
class BiometricInformation {
public:
	BiometricInformation(const BIT &biometricData); // photo and template
	~BiometricInformation();
	EIDMW_APL_API BiometricHeaderTemplate *biometricTemplate() const;
	EIDMW_APL_API FaceInfo *faceInfo() const;

private:
	std::unique_ptr<BiometricHeaderTemplate> m_biometricTemplate;
	std::unique_ptr<FaceInfo> m_faceInfo;
};

class IcaoDg2 {
public:
	IcaoDg2(const CByteArray &arrayDg2);
	~IcaoDg2();

	EIDMW_APL_API unsigned int numberOfBiometrics() const;
	EIDMW_APL_API std::vector<BiometricInformation *> biometricInstances() const;

private:
	unsigned int m_numberOfBiometrics;
	std::vector<std::unique_ptr<BiometricInformation>> m_biometricInstances;
};
} // namespace eIDMW

#endif // ICAODG2_H
