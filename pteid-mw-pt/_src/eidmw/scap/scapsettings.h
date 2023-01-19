/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2022 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/
#pragma once

#include <QString>
#include <string>

#include "eidlib.h"

namespace eIDMW {

class ScapSettings
{
public:
    ScapSettings();

    void setScapServerHost(const QString &host);
    std::string getScapServerHost();
    void setScapServerPort(int server_port);
    std::string getScapServerPort();

    void setSecretKey(const std::string &secretKey, const std::string &nic);
    std::string getSecretKey(const std::string &nic);
    void resetScapKeys();

    void setAppID(const QString &appID);
    const QString getAppID();

    QString getCacheDir();

    static bool secretKeyExists();

private:
    QString m_scap_server_host;
    QString m_scap_server_port;

    QString m_appID;
    QString m_cache_dir;
};

};
