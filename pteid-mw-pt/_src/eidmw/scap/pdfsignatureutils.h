/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#pragma once

#include <QString>
#include <vector>
#include "eidlibdefines.h"

namespace eIDMW {

struct SealGeometry {
	unsigned int width;
	unsigned int height;
	double x;
	double y;
};

struct PDFSignatureInfo {
	std::vector<std::string> filenames;
	std::string output;
	std::string location;
	std::string reason;
	PTEID_SignatureLevel level;
	bool is_visible;
	unsigned int selected_page;
	bool is_last_page;
	SealGeometry seal_geometry;
	bool use_custom_image;
	unsigned char *seal_image_data;
	unsigned long seal_image_length;
};

}; // namespace eIDMW
