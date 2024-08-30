#include "IcaoDg3.h"

#include <Util.h>

#include <openssl/asn1.h>
#include <openssl/asn1t.h>

#include <vector>

namespace eIDMW {

const unsigned char FIR[] = {0x46, 0x49, 0x52};

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

struct BIT_HEADERDG3_st {
	ASN1_STRING *version;
	ASN1_STRING *biometric_type;
	ASN1_STRING *biometric_subtype;
	ASN1_STRING *creation_datetime;
	ASN1_STRING *validity_period;
	ASN1_STRING *creator;
	ASN1_STRING *format_owner; // Mandatory field
	ASN1_STRING *format_type;  // Mandatory field
};

typedef struct BIT_HEADERDG3_st BIT_HEADERDG3;

struct BIT_st {
	BIT_HEADERDG3 *bit_header;
	ASN1_STRING *bit_content;
};

typedef struct BIT_st BITDG3;

ASN1_SEQUENCE(BIT_HEADERDG3) = {
	ASN1_IMP_EX(BIT_HEADERDG3, version, ASN1_OCTET_STRING, 0x0, ASN1_TFLG_OPTIONAL),
	ASN1_IMP_EX(BIT_HEADERDG3, biometric_type, ASN1_OCTET_STRING, 0x1, ASN1_TFLG_OPTIONAL),
	ASN1_IMP_EX(BIT_HEADERDG3, biometric_subtype, ASN1_OCTET_STRING, 0x2, ASN1_TFLG_OPTIONAL),
	ASN1_IMP_EX(BIT_HEADERDG3, creation_datetime, ASN1_OCTET_STRING, 0x3, ASN1_TFLG_OPTIONAL),
	ASN1_IMP_EX(BIT_HEADERDG3, validity_period, ASN1_OCTET_STRING, 0x5, ASN1_TFLG_OPTIONAL),
	ASN1_IMP_EX(BIT_HEADERDG3, creator, ASN1_OCTET_STRING, 0x6, ASN1_TFLG_OPTIONAL),
	ASN1_IMP(BIT_HEADERDG3, format_owner, ASN1_OCTET_STRING, 0x7),
	ASN1_IMP(BIT_HEADERDG3, format_type, ASN1_OCTET_STRING, 0x8)

} ASN1_SEQUENCE_END(BIT_HEADERDG3);

IMPLEMENT_ASN1_FUNCTIONS(BIT_HEADERDG3);

/* Biometric Information Template */
ASN1_SEQUENCE(BITDG3) = {ASN1_IMP_EX(BITDG3, bit_header, BIT_HEADERDG3, 0x1, 0),
						 ASN1_EX_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_APPLICATION, 0x2E, BITDG3, bit_content,
									  ASN1_OCTET_STRING)} ASN1_SEQUENCE_END(BITDG3);

IMPLEMENT_ASN1_FUNCTIONS(BITDG3);

BiometricInfoDG3::BiometricInfoDG3(const BITDG3 &bio) {
	BIT_HEADER header;
	header.version = bio.bit_header->version;
	header.biometric_type = bio.bit_header->biometric_type;
	header.biometric_subtype = bio.bit_header->biometric_subtype;
	header.creation_datetime = bio.bit_header->creation_datetime;
	header.validity_period = bio.bit_header->validity_period;
	header.creator = bio.bit_header->creator;
	header.format_owner = bio.bit_header->format_owner;
	header.format_type = bio.bit_header->format_type;
	m_biometricTemplate.reset(new BiometricHeaderTemplate(header));

	const unsigned char *fingerData = bio.bit_content->data;

	if (fingerData[0] != FIR[0] || fingerData[1] != FIR[1] || fingerData[2] != FIR[2]) {
		printf("It's not reading FIR! %02x %02x %02x\n", fingerData[0], fingerData[1], fingerData[2]);
		return;
	}
	fingerData += 4;
	printf("It is reading FIR!!\n");
	m_specVersion = std::string((const char *)fingerData, 4); // version is 4 bytes
	fingerData += 4;
	m_recordLength = readTwoBytes(fingerData);
	fingerData += 2;
	m_recordLength = m_recordLength << 32;
	m_recordLength += (fingerData[0] << 24) | (fingerData[1] << 16) | (fingerData[2] << 8) | (fingerData[3]);
	fingerData += 4;
	// identifier owner and type is here on ANSI

	m_scannerId = readTwoBytes(fingerData);
	m_scannerId = m_scannerId & 0x0FFF;
	fingerData += 2;
	m_imageAcquisitionLevel = readTwoBytes(fingerData);
	fingerData += 2;
	m_numFingersOrPalmImages = *fingerData;
	fingerData++;
	m_scaleUnits = *fingerData;
	fingerData++;

	m_xScanResolution = readTwoBytes(fingerData);
	fingerData += 2;

	m_yScanResolution = readTwoBytes(fingerData);
	fingerData += 2;

	m_xImageResolution = readTwoBytes(fingerData);
	fingerData += 2;

	m_yImageResolution = readTwoBytes(fingerData);
	fingerData += 2;

	m_pixelDepth = *fingerData;
	fingerData++;
	m_imageCompressionAlgorithm = *fingerData;
	fingerData++;
	m_reserved = readTwoBytes(fingerData);
	fingerData += 2;
	printf("print debug this numberfingerorpalm: %d sizeRecording: %llu\n", m_numFingersOrPalmImages, m_recordLength);
	for (int i = 0; i < m_numFingersOrPalmImages; i++) {
		std::shared_ptr<BiometricInfoImage> fingerImage = std::make_shared<BiometricInfoImage>();
		fingerImage->m_length = (fingerData[0] << 24) | (fingerData[1] << 16) | (fingerData[2] << 8) | (fingerData[3]);
		fingerData += 4;

		fingerImage->m_fingerPalmPosition = *fingerData;
		fingerData++;

		fingerImage->m_countOfViews = *fingerData;
		fingerData++;

		fingerImage->m_viewMumber = *fingerData;
		fingerData++;

		fingerImage->m_quality = *fingerData;
		fingerData++;

		fingerImage->m_impressionType = *fingerData;
		fingerData++;

		fingerImage->m_horizontalLineLength = readTwoBytes(fingerData);
		fingerData += 2;

		fingerImage->m_verticalLineLength = readTwoBytes(fingerData);
		fingerData += 2;

		fingerImage->m_reserved = *fingerData;
		fingerData++;

		unsigned int imageLength = fingerImage->m_length - 14;
		fingerImage->m_imageData = CByteArray(fingerData, imageLength);
		fingerData += imageLength;
		m_fingerImageVec.push_back(fingerImage);
		printf("print debug this number length of finger: %d\n", fingerImage->m_length);
	}
}

const unsigned char *getContent(const unsigned char *data, int classRequested, int asn1TagRequested,
								int returnvalueRequested, long &dataSize, long maxLength) {
	const unsigned char *old_data = NULL;
	const unsigned char *desc_data = data;
	int xclass = 0;
	int ans1Tag = 0;
	int returnValue = 0;
	long sizeGet;
	while (xclass != classRequested || asn1TagRequested != ans1Tag || returnValue != returnvalueRequested) {
		if (old_data == desc_data)
			return NULL;
		returnValue = ASN1_get_object(&desc_data, &sizeGet, &ans1Tag, &xclass, maxLength);
	}
	dataSize = sizeGet;
	return desc_data;
}

CByteArray getSizeExtended(long size) // improve this!!!
{
	CByteArray sizeExtended;
	sizeExtended.Append(0x82);

	unsigned int counter = 0;
	unsigned char tucLong[sizeof(unsigned long)];
	for (int i = sizeof(unsigned long) - 1; i >= 0; --i) {
		tucLong[i] = static_cast<unsigned char>(size % 256);
		size /= 256;
		if (tucLong[i] == 0)
			break;
		counter++;
	}

	for (int i = sizeof(unsigned long) - counter; i < sizeof(unsigned long); ++i) {
		sizeExtended.Append(tucLong[i]);
	}
	printf("print debug this sizeExtended: %s\n", sizeExtended.ToString().c_str());
	return sizeExtended;
}

eIDMW::IcaoDg3::IcaoDg3(const CByteArray &biometricData) {
	const unsigned char *originalBioData = biometricData.GetBytes();
	long size = 0;
	const unsigned char *biometricDataGroup =
		getContent(originalBioData, 0x40, 0x61, V_ASN1_CONSTRUCTED, size, biometricData.Size());

	const unsigned char *numberOfInstancesData = findASN1Object(&biometricDataGroup, size, 0x02, size);
	if (numberOfInstancesData && size == 1) {
		m_numberOfInstances = numberOfInstancesData[0];
	}

	const unsigned char *biometricTemplate = biometricDataGroup;
	for (unsigned int i = 0; i < m_numberOfInstances; ++i) {
		biometricTemplate = getContent(biometricTemplate, 0x40, 0x00, V_ASN1_CONSTRUCTED, size, biometricData.Size());

		CByteArray biometricTemplateArray;
		biometricTemplateArray.Append(0x30);
		biometricTemplateArray.Append(getSizeExtended(size));
		biometricTemplateArray.Append(biometricTemplate, size);
		const unsigned char *biometricTemplateArrayData = biometricTemplateArray.GetBytes();
		BITDG3 *bitDG3 =
			d2i_BITDG3(NULL, (const unsigned char **)&biometricTemplateArrayData, biometricTemplateArray.Size());
		if (bitDG3) {
			std::unique_ptr<BiometricInfoDG3> data = std::make_unique<BiometricInfoDG3>(*bitDG3);
			m_vector.push_back(std::move(data));
		} else { // error!!!!
		}

		biometricTemplate += size - 1;
	}
}

} // namespace eIDMW
