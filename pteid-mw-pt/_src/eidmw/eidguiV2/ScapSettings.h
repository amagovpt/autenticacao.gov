/*-****************************************************************************

 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2021 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

#ifndef _SCAPSETTINGS_H_
#define _SCAPSETTINGS_H_

#include <Qt>
#include <QtCore>
#include <QString>
#include <QSettings>
#include <string>

#include "eidlib.h"

class ScapSettings
{
    #define RECV_TIMEOUT        60
    #define SEND_TIMEOUT        60
    #define CONNECT_TIMEOUT     60

    #define SCAP_ATTRIBUTES_OK          200

    // Service attributes error codes
    #define SCAP_ATTRIBUTES_EXPIRED     401    // Citizen only has expired attributes 
    #define SCAP_ZERO_ATTRIBUTES        402    // Citizen has no attributes 
    #define SCAP_ATTRIBUTES_NOT_VALID   403    // Attributes request do not match the citizen's attributes

    #define SCAP_GENERIC_ERROR_CODE            404
    #define SCAP_CLOCK_ERROR_CODE              405
    #define SCAP_SECRETKEY_ERROR_CODE          406
    #define SCAP_ATTR_POSSIBLY_EXPIRED_WARNING 407

    #define SCAP_MAX_CLOCK_DIF          5*60    // 5 minutos

    // Service autentication error codes
    #define SCAP_TOTP_FAILED_ERROR_CODE     802     // TOTP validation has failed
    #define SCAP_ACCOUNT_MATCH_ERROR_CODE   803     // Account match has failed
    #define SCAP_REQUEST_ERROR_CODE         805     // Request with invalid or missing fields

public:
    //------------------------------------------------------
    // ctor
    //------------------------------------------------------
    ScapSettings( void )
        : m_pteid_language("en")

    {
        //----------------------------------------------------------
        // Check always what is set in the registry
        //----------------------------------------------------------

        //----------------------------------------------------------
        // check the GUI language
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_LANGUAGE);
            QString lng = config.getString();
            m_pteid_language = lng;
        }


        //----------------------------------------------------------
        // check the Cache dir
        //----------------------------------------------------------
        {
			eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_CACHEDIR);
#ifdef WIN32
			QString cacheDir = QString::fromLatin1(config.getString());
#else
			QString cacheDir = config.getString();
#endif
            m_cache_dir = cacheDir;
        }

        //---------------------------------------------------------
        // Check proxy settings
        //---------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_HOST);
            QString server_host = config.getString();
            m_scap_server_host = server_host;

        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_PORT);
            QString server_port = config.getString();
            m_scap_server_port = server_port;

        }

        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APPID);
            m_appID = config.getString();
        }
    }
    ~ScapSettings( void )
    {
    }

    QString const& getPTEIDLanguage ( void ) const
    {
        return m_pteid_language;
    }

    void setScapServerHost ( QString const& host)
    {
        m_scap_server_host = host;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_HOST);
        config.setString(host.toUtf8());
    }

    void setSecretKey(std::string secretKey, QString nic)
    {
        QString nic_no_check = nic;

        // Remove NIC Check Digit
        if(nic_no_check.size() == 9)
            nic_no_check = nic_no_check.left(8);

        QByteArray array(secretKey.data(), secretKey.length());
        std::string str = "scap_apikey_" + nic_no_check.toStdString() ;
        const char* pName = str.c_str();
        eIDMW::PTEID_Config config(pName, L"general", L"");
        config.setString(array.toHex());
    }

    void resetScapKeys()
    {
        std::string str = "scap_apikey_";
        const char* pName = str.c_str();
        eIDMW::PTEID_Config config(pName, L"general", L"");
        try{
            setAppID("");
            config.DeleteKeysByPrefix();
        }
        catch (...)
        {
            PTEID_LOG(eIDMW::PTEID_LOG_LEVEL_DEBUG, "ScapSignature",
                "DeleteKeys Fail");
        }
    }

    std::string getSecretKey(QString nic) {

        QString nic_no_check = nic;

        // Remove NIC Check Digit
        if(nic_no_check.size() == 9)
            nic_no_check = nic_no_check.left(8);

        std::string str = "scap_apikey_" + nic_no_check.toStdString();

        const char* pName = str.c_str();
        eIDMW::PTEID_Config config(pName, L"general", L"");
        std::string temp_secretKey = config.getString();
        QByteArray temp_array(temp_secretKey.data(), temp_secretKey.length());
        QByteArray m_secretKey_array =  QByteArray::fromHex(temp_array);

        return m_secretKey_array.constData();
    }

    void setAppID(QString const& appID)
    {
        m_appID = appID;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APPID);

        config.setString(m_appID.toUtf8());
    }

    QString const getAppID(void)
    {
        return m_appID;
    }

    QString getScapServerHost ( void )
    {
        return m_scap_server_host;
    }

    void setScapServerPort(int server_port)
    {
        m_scap_server_port = server_port;

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_PORT);
        config.setLong(server_port);
    }

    QString getScapServerPort()
    {
        return m_scap_server_port;
    }

    QString getCacheDir ( void )
    {
        return m_cache_dir;
    }



private:
    QString m_pteid_language;
    QString m_cache_dir;
    QString m_appID;

    QString m_scap_server_host;
    QString m_scap_server_port;
};

#endif // _SETTINGS_H_

