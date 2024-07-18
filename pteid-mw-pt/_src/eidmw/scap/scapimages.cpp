/*-****************************************************************************

 * Copyright (C) 2022 André Guerreiro - <aguerreiro1985@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#include <QString>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QByteArray>
#include <QDebug>
#include <QCryptographicHash>

#include <string>

#include "scapclient.h"
#include "scapsettings.h"
#include "Log.h"

namespace eIDMW {

static QString get_logos_cache_path() {
	ScapSettings settings;
	return settings.getCacheDir() + "/scap_logos/";
}

static QString get_unique_image_filename(const std::string &provider_id) {
	QCryptographicHash unique_filename(QCryptographicHash::Sha256);
	assert(provider_id.size() <= INT_MAX);
	unique_filename.addData(provider_id.c_str(), (int) provider_id.size());

	return "/scap_" + unique_filename.result().toHex(0) + ".jpeg";
}

static QImage apply_image_seal_transformations(QImage image) {
	const int MAX_IMAGE_HEIGHT = 77;
	const int MAX_IMAGE_WIDTH = 351;
	float RECOMMENDED_RATIO = MAX_IMAGE_HEIGHT / (float)MAX_IMAGE_WIDTH;
	float ACTUAL_RATIO = image.height() / (float)image.width();

	// Only scale image when needed
	if (image.height() != MAX_IMAGE_HEIGHT || image.width() != MAX_IMAGE_WIDTH) {
		// Fit custom image to available space, keeping aspect ratio
		if (ACTUAL_RATIO >= RECOMMENDED_RATIO)
			image = image.scaledToHeight(MAX_IMAGE_HEIGHT, Qt::SmoothTransformation);
		else
			image = image.scaledToWidth(MAX_IMAGE_WIDTH, Qt::SmoothTransformation);
	}

	// Create blank image
	QImage final_img = QImage(MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT, QImage::Format_RGB32);
	final_img.fill(QColor(Qt::white).rgb());
	QPainter painter(&final_img);

	// Center scaled custom image in the white background
	int position_x = (int)((MAX_IMAGE_WIDTH / 2) - image.width() / 2);
	int position_y = (int)((MAX_IMAGE_HEIGHT / 2) - image.height() / 2);
	painter.drawImage(position_x, position_y, image);

	return final_img;
}

bool save_scap_image(const std::string &provider_id, const std::string &b64_image) {

	qDebug() << "Called " << __FUNCTION__ << " with provider_id " << provider_id.c_str();

	// TODO: Using QT 5.15 we can decode Base-64 data in strict mode
	// QByteArray img_data = QByteArray::fromBase64(QByteArray::fromStdString(b64_image),
	// QByteArray::AbortOnBase64DecodingErrors);
	QByteArray img_data = QByteArray::fromBase64(QByteArray::fromStdString(b64_image));

	if (img_data.size() == 0) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to decode base64 input image!");
		return false;
	}

	qDebug() << "Logo byte count: " << img_data.size();

	QImage original_image;
	if (!original_image.loadFromData(img_data)) {
		MWLOG(LEV_ERROR, MOD_SCAP,
			  "Failed to load logo from provider with id '%s'. Expected base64-encoded image file.",
			  provider_id.c_str());
		return false;
	}
	QImage image = apply_image_seal_transformations(original_image);

	QString cache_path(get_logos_cache_path());
	QDir cache_dir; // Create cache dir for logos if it does not exist
	if (!cache_dir.mkpath(cache_path)) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to create SCAP logos cache directory!");
		return false;
	}

	cache_path += get_unique_image_filename(provider_id);
	if (!image.save(cache_path)) {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to save logo cache file");
		return false;
	}

	return true;
}

static std::string get_provider_with_logo(const std::vector<ScapAttribute> &attributes) {
	std::string result;
	for (const auto &attribute : attributes) {
		if (!attribute.provider.logo.empty()) {
			if (!result.empty() && result != attribute.provider.nipc) {
				// more than one provider has logo, return empty string
				return {};
			}
			result = attribute.provider.nipc;
		}
	}

	return result;
}

QString get_scap_image_path(const std::vector<ScapAttribute> &attributes) {
	std::string provider_id = get_provider_with_logo(attributes);
	if (provider_id.empty()) {
		return {};
	}

	QString cache_path(get_logos_cache_path());
	cache_path += get_unique_image_filename(provider_id);

	if (!QFile::exists(cache_path)) {
		return {};
	}

	return cache_path;
}

QByteArray get_scap_image_data(const std::vector<ScapAttribute> &attributes) {
	QString cache_path(get_scap_image_path(attributes));
	if (cache_path.isEmpty()) {
		return {};
	}

	QByteArray result;
	if (QFile::exists(cache_path)) {
		QFile image_file(cache_path);
		image_file.open(QIODevice::ReadOnly);
		result = image_file.readAll();
		image_file.close();
	} else {
		MWLOG(LEV_ERROR, MOD_SCAP, "Failed to load image from cache! File does not exist. Path: %s",
			  cache_path.toStdString().c_str());
	}

	return result;
}

} // namespace eIDMW
