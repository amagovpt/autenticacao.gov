#include "IcaoDg2.h"

#include "ByteArray.h"
#include "Util.h"
#include "Log.h"
#include "J2KHelper.h"

#include <vector>
#include <cstddef>
#include <openssl/asn1.h>
#include <openssl/asn1t.h>


const unsigned char FAC_CODE[] = {0x46, 0x41, 0x43};

namespace eIDMW {

/* Decoder for biometric data templates (DG2) */

struct BIT_HEADER_st {
	ASN1_STRING *version;
	ASN1_STRING *biometric_type;
	ASN1_STRING *biometric_subtype;
	ASN1_STRING *creation_datetime;
	ASN1_STRING *validity_period;
	ASN1_STRING *creator;
	ASN1_STRING *format_owner; // Mandatory field
	ASN1_STRING *format_type;  // Mandatory field
};

typedef struct BIT_HEADER_st BIT_HEADER;

struct BIT_st {
	BIT_HEADER *bit_header;
	ASN1_STRING *bit_content;
};

typedef struct BIT_st BIT;

struct BIT_GROUP_st {
	ASN1_INTEGER *number_of_instances;
	BIT *biometric_info_template;
};

typedef struct BIT_GROUP_st BIT_GROUP;

struct DG2_st {
	BIT_GROUP *bit_group;
};

typedef struct DG2_st DG2;

ASN1_SEQUENCE(BIT_HEADER) = {ASN1_IMP_EX(BIT_HEADER, version, ASN1_OCTET_STRING, 0x0, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP_EX(BIT_HEADER, biometric_type, ASN1_OCTET_STRING, 0x1, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP_EX(BIT_HEADER, biometric_subtype, ASN1_OCTET_STRING, 0x2, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP_EX(BIT_HEADER, creation_datetime, ASN1_OCTET_STRING, 0x3, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP_EX(BIT_HEADER, validity_period, ASN1_OCTET_STRING, 0x5, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP_EX(BIT_HEADER, creator, ASN1_OCTET_STRING, 0x6, ASN1_TFLG_OPTIONAL),
							 ASN1_IMP(BIT_HEADER, format_owner, ASN1_OCTET_STRING, 0x7),
							 ASN1_IMP(BIT_HEADER, format_type, ASN1_OCTET_STRING, 0x8)} ASN1_SEQUENCE_END(BIT_HEADER);
	
	IMPLEMENT_ASN1_FUNCTIONS(BIT_HEADER);

	/* Biometric Information Template */
	ASN1_SEQUENCE(BIT) = {ASN1_IMP_EX(BIT, bit_header, BIT_HEADER, 0x1, 0),
						  ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x2E, BIT, bit_content,
									   ASN1_OCTET_STRING)} ASN1_SEQUENCE_END(BIT);

	IMPLEMENT_ASN1_FUNCTIONS(BIT);

	/* Biometric Information Template group */

	ASN1_SEQUENCE(BIT_GROUP) = {ASN1_SIMPLE(BIT_GROUP, number_of_instances, ASN1_INTEGER),
								ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x60, BIT_GROUP,
											 biometric_info_template, BIT)

} ASN1_SEQUENCE_END(BIT_GROUP) IMPLEMENT_ASN1_FUNCTIONS(BIT_GROUP)

		ASN1_SEQUENCE(DG2) = {ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x61, DG2, bit_group,
										   BIT_GROUP)} ASN1_SEQUENCE_END(DG2);

	IMPLEMENT_ASN1_FUNCTIONS(DG2);
DG2 * decodeDg2(const CByteArray &biometricData) {
	if (biometricData.Size() == 0)
		return NULL;

	CByteArray process(biometricData);
	unsigned char *asn1_data = process.GetBytes();
	asn1_data[0] = 0x30;
	auto *processedDg2Data = d2i_DG2(NULL, (const unsigned char **)&asn1_data, process.Size());
	return processedDg2Data;
}

IcaoDg2::IcaoDg2(const CByteArray &arrayDg2) {
	DG2 *decoded = decodeDg2(arrayDg2);
	if (decoded == NULL) {
		MWLOG(LEV_ERROR, MOD_APL, "ICAO DG2: failed to decode main structure!");
		return;
	}

	if (!decoded->bit_group->number_of_instances || decoded->bit_group->number_of_instances->length == 0) {
		m_numberOfBiometrics = 0;
	} else {
		m_numberOfBiometrics = decoded->bit_group->number_of_instances->data[0];
	}
	m_biometricInstances.push_back(
		std::make_unique<BiometricInformation>(*decoded->bit_group->biometric_info_template)); // add to vector

	DG2_free(decoded);
}

IcaoDg2::~IcaoDg2() {}

unsigned int IcaoDg2::numberOfBiometrics() const { return m_numberOfBiometrics; }

std::vector<BiometricInformation *> IcaoDg2::biometricInstances() const {
	std::vector<BiometricInformation *> instances;
	for (const auto &instance : m_biometricInstances) {
		instances.push_back(instance.get());
	}
	return instances;
}

eIDMW::BiometricInformation::BiometricInformation(const BIT &biometricData) {
	m_faceInfo.reset(new FaceInfo(biometricData.bit_content->data, ASN1_STRING_length(biometricData.bit_content)));
	m_biometricTemplate.reset(new BiometricHeaderTemplate(*biometricData.bit_header));
}

BiometricInformation::~BiometricInformation() {}

BiometricHeaderTemplate *BiometricInformation::biometricTemplate() const { return m_biometricTemplate.get(); }

FaceInfo *BiometricInformation::faceInfo() const { return m_faceInfo.get(); }

CByteArray makeHeaderTemplate(ASN1_STRING *templateMember) {
	if (templateMember == NULL || templateMember->length == 0)
		return CByteArray();

	return CByteArray(templateMember->data, templateMember->length);
}

eIDMW::BiometricHeaderTemplate::BiometricHeaderTemplate(const BIT_HEADER &bioHeader) {
	m_icaoHeaderVersion = makeHeaderTemplate(bioHeader.version);
	m_type = makeHeaderTemplate(bioHeader.biometric_type);
	m_subType = makeHeaderTemplate(bioHeader.biometric_subtype);
	m_creationDateAndtime = makeHeaderTemplate(bioHeader.creation_datetime);
	m_validPeriod = makeHeaderTemplate(bioHeader.validity_period);
	m_creatorOfBiometricRefData = makeHeaderTemplate(bioHeader.creator);
	m_formatOwner = makeHeaderTemplate(bioHeader.format_owner);
	m_formatType = makeHeaderTemplate(bioHeader.format_type);
}

BiometricHeaderTemplate::~BiometricHeaderTemplate() {}

const CByteArray &BiometricHeaderTemplate::icaoHeaderVersion() const { return m_icaoHeaderVersion; }

const CByteArray &BiometricHeaderTemplate::type() const { return m_type; }

const CByteArray &BiometricHeaderTemplate::subType() const { return m_subType; }

const CByteArray &BiometricHeaderTemplate::creationDateAndtime() const { return m_creationDateAndtime; }

const CByteArray &BiometricHeaderTemplate::validPeriod() const { return m_validPeriod; }

const CByteArray &BiometricHeaderTemplate::creatorOfBiometricRefData() const { return m_creatorOfBiometricRefData; }

const CByteArray &BiometricHeaderTemplate::formatOwner() const { return m_formatOwner; }

const CByteArray &BiometricHeaderTemplate::formatType() const { return m_formatType; }

FaceInfo::FaceInfo(const unsigned char *biometricData, int biometricDataLen) {
	m_encodingBytes = 0x5F2E;
	const int FACEIMAGE_DATA_MIN_LEN = 46;
	//Save the original pointer value
	const unsigned char * startOfBiometricData = biometricData;
	if (biometricDataLen < FACEIMAGE_DATA_MIN_LEN) {
		MWLOG(LEV_WARN, MOD_APL, "Biometric Data too short for ISO XXX-5 face image record!");
		return;
	}
	if (biometricData[0] == FAC_CODE[0] && biometricData[1] == FAC_CODE[1] && biometricData[2] == FAC_CODE[2]) {
		biometricData = (biometricData + 4); // skip FAC code and first 00 byte 3 + 1

		m_version = std::string((const char *)biometricData, 4); // version is 4 bytes

		biometricData += 4;
		m_sizeOfRecord =
			(biometricData[0] << 24) | (biometricData[1] << 16) | (biometricData[2] << 8) | biometricData[3]; // 4 bytes
		biometricData += 4;

		m_numberOfFacialImages = readTwoBytes(biometricData); // 2 bytes
		biometricData += 2;
		for (unsigned int i = 0; i < m_numberOfFacialImages; ++i) {
			std::shared_ptr<FaceInfoData> data = std::make_unique<FaceInfoData>();
			data->m_facialRecordDataLength = (biometricData[0] << 24) | (biometricData[1] << 16) |
											 (biometricData[2] << 8) | biometricData[3]; // 4 bytes
			m_faceInfoData.push_back(data);

			biometricData += 4;

			data->m_numberOfFeaturePoints = readTwoBytes(biometricData); // 2 bytes
			biometricData += 2;

			data->m_gender = *biometricData;
			biometricData++;

			data->m_eyeColor = *biometricData;
			biometricData++;

			data->m_hairColour = *biometricData;
			biometricData++;

			data->m_featureMask = (biometricData[0] << 16) | (biometricData[1] << 8) | biometricData[2]; // 3 bytes
			biometricData += 3;
			data->m_expression = (biometricData[0] << 8) | biometricData[1]; // 2bytes
			biometricData += 2;

			data->m_poseAngle = (biometricData[0] << 16) | (biometricData[1] << 8) | biometricData[2]; // 3 bytes
			data->m_poseAngleList.push_back(biometricData[0]);										   // yaw
			data->m_poseAngleList.push_back(biometricData[1]);										   // pitch
			data->m_poseAngleList.push_back(biometricData[2]);										   // roll
			biometricData += 3;

			data->m_poseAngleUncertainty =
				(biometricData[0] << 16) | (biometricData[1] << 8) | biometricData[2]; // 3 bytes
			data->m_poseAngleUncertaintyList.push_back(biometricData[0]);			   // yaw
			data->m_poseAngleUncertaintyList.push_back(biometricData[1]);			   // pitch
			data->m_poseAngleUncertaintyList.push_back(biometricData[2]);			   // roll
			biometricData += 3;

			long numFeaturePoints = data->m_numberOfFeaturePoints;
			while (numFeaturePoints > 0) {
				data->m_featurePoints.push_back(std::make_unique<FeaturePoint>(
					biometricData)); // has 8 size, use the number of features to better process them
				biometricData += 8;
				numFeaturePoints--;
			}

			data->m_faceImgType = *biometricData;
			biometricData++;
			data->m_imgDataType = *biometricData;
			biometricData++;

			data->m_imgWidth = readTwoBytes(biometricData);
			biometricData += 2;
			data->m_imgHeight = readTwoBytes(biometricData);
			biometricData += 2;

			data->m_colourSpace = *biometricData;
			biometricData++;
			data->m_sourceType = *biometricData;
			biometricData++;

			data->m_deviceType = readTwoBytes(biometricData);
			biometricData += 2;
			data->m_quality = readTwoBytes(biometricData);
			biometricData += 2;

			ptrdiff_t offset = biometricData - startOfBiometricData;

			size_t sizePhotoRaw = data->m_facialRecordDataLength - 20 - (8 * data->m_numberOfFeaturePoints) - 12;
			if (offset + sizePhotoRaw > biometricDataLen) {
				MWLOG(LEV_WARN, MOD_APL, "Image data inconsistent with Facial Record Header!");
				sizePhotoRaw = biometricDataLen - offset;
			}

			data->m_photoRawData = CByteArray(biometricData, sizePhotoRaw);
			bool img_datatype_check = checkImageDataType(data->m_photoRawData, data->m_imgDataType);
			MWLOG(LEV_INFO, MOD_APL, "DG2 image %d data type check: %s", i, img_datatype_check ? "OK" : "NOT_OK");

			biometricData += sizePhotoRaw;
		}
	}
}

bool FaceInfo::checkImageDataType(CByteArray &photo_data, unsigned char imgDataType) {
	if (imgDataType == static_cast<unsigned char>(ImageDataType::TYPE_JPEG)) {
		const unsigned char jpeg_magic[] = {0xFF, 0xD8, 0xFF};
		return memcmp(photo_data.GetBytes(), jpeg_magic, sizeof(jpeg_magic)) == 0;
	} else if (imgDataType == static_cast<unsigned char>(ImageDataType::TYPE_JPEG2000)) {
		const unsigned char jpeg_2000_magic[] = {0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20};
		return memcmp(photo_data.GetBytes(), jpeg_2000_magic, sizeof(jpeg_2000_magic)) == 0;
	} else {
		MWLOG(LEV_ERROR, MOD_APL, "%s: Unknown value for imgDataType: %02X", __FUNCTION__, imgDataType);
		return false;
	}
}

const std::string &FaceInfo::version() const { return m_version; }

unsigned short FaceInfo::encodingBytes() const { return m_encodingBytes; }

long FaceInfo::sizeOfRecord() const { return m_sizeOfRecord; }

long FaceInfo::numberOfFacialImages() const { return m_numberOfFacialImages; }

std::vector<FaceInfoData *> FaceInfo::faceInfoData() const {
	std::vector<FaceInfoData *> faceInfoData;
	for (const auto &fi : m_faceInfoData) {
		faceInfoData.push_back(fi.get());
	}
	return faceInfoData;
}

FeaturePoint::FeaturePoint(const unsigned char *biometricData) {
	m_type = *biometricData;
	biometricData++;
	m_featurePoint = *biometricData;
	m_majorCode = (m_featurePoint & 0xF0) >> 4;
	m_minorCode = m_featurePoint & 0x0F;
	biometricData++;
	m_x_coord = readTwoBytes(biometricData);
	biometricData += 2;
	m_y_coord = readTwoBytes(biometricData);
	biometricData += 2;
	m_reserved = readTwoBytes(biometricData);
}

unsigned char FeaturePoint::type() const { return m_type; }

unsigned char FeaturePoint::featurePoint() const { return m_featurePoint; }

unsigned char FeaturePoint::majorCode() const { return m_majorCode; }

unsigned char FeaturePoint::minorCode() const { return m_minorCode; }

unsigned short FeaturePoint::x_coord() const { return m_x_coord; }

unsigned short FeaturePoint::y_coord() const { return m_y_coord; }

unsigned short FeaturePoint::reserved() const { return m_reserved; }

long FaceInfoData::facialRecordDataLength() const { return m_facialRecordDataLength; }

unsigned short FaceInfoData::numberOfFeaturePoints() const { return m_numberOfFeaturePoints; }

unsigned char FaceInfoData::gender() const { return m_gender; }

Gender FaceInfoData::genderDecode() const { return static_cast<Gender>(m_gender); }

unsigned char FaceInfoData::eyeColor() const { return m_eyeColor; }

EyeColor FaceInfoData::eyeColorDecode() const { return static_cast<EyeColor>(m_eyeColor); }

unsigned char FaceInfoData::hairColour() const { return m_hairColour; }

HairColour FaceInfoData::hairColourDecode() const { return static_cast<HairColour>(m_hairColour); }

long FaceInfoData::featureMask() const { return m_featureMask; }

long FaceInfoData::expression() const { return m_expression; }

long FaceInfoData::poseAngle() const { return m_poseAngle; }

long FaceInfoData::poseAngleUncertainty() const { return m_poseAngleUncertainty; }

std::vector<FeaturePoint *> FaceInfoData::featurePoints() const {
	std::vector<FeaturePoint *> featurePoints;
	for (const auto &fp : m_featurePoints) {
		featurePoints.push_back(fp.get());
	}
	return featurePoints;
}

unsigned char FaceInfoData::faceImgType() const { return m_faceImgType; }

FaceImageType FaceInfoData::faceImgTypeDecode() const { return static_cast<FaceImageType>(m_faceImgType); }

unsigned char FaceInfoData::imgDataType() const { return m_imgDataType; }

ImageDataType FaceInfoData::imgDataTypeDecode() const { return static_cast<ImageDataType>(m_faceImgType); }

unsigned short FaceInfoData::imgWidth() const { return m_imgWidth; }

unsigned short FaceInfoData::imgHeight() const { return m_imgHeight; }

unsigned char FaceInfoData::colourSpace() const { return m_colourSpace; }

ImageColourSpace FaceInfoData::colourSpaceDecode() const { return static_cast<ImageColourSpace>(m_colourSpace); }

unsigned char FaceInfoData::sourceType() const { return m_sourceType; }

SourceType FaceInfoData::sourceTypeDecode() const { return static_cast<SourceType>(m_sourceType); }

unsigned short FaceInfoData::deviceType() const { return m_deviceType; }

unsigned short FaceInfoData::quality() const { return m_quality; }

const CByteArray &FaceInfoData::photoRawData() const { return m_photoRawData; }

const CByteArray &FaceInfoData::photoRawDataPNG() {
	unsigned char *mem_buffer = nullptr;
	unsigned long size_in_bytes = 0;
	convert_to_png(m_photoRawData.GetBytes(), m_photoRawData.Size(), &mem_buffer, &size_in_bytes);
	m_photoRawDataPNG = CByteArray(const_cast<unsigned char *>(mem_buffer), static_cast<unsigned long>(size_in_bytes));
	if (mem_buffer) {
		free(mem_buffer);
	}
	return m_photoRawDataPNG;
}
} // namespace eIDMW
