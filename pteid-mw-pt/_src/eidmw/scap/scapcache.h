/*-****************************************************************************

 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#pragma once

#include "scapclient.h"
#include "scaperrors.h"

class QImage;
class QByteArray;
namespace eIDMW {

bool cache_response(const std::string &response, const std::string &id, std::vector<ScapAttribute> &out_attributes);
ScapResult<std::vector<ScapAttribute>> load_cache();
bool clear_cache();

bool save_scap_image(const std::string &provider_id, const std::string &b64_scap_logo);
QString get_scap_image_path(const std::vector<ScapAttribute> &attributes);
QByteArray get_scap_image_data(const std::vector<ScapAttribute> &attributes);

}; // namespace eIDMW
