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
    #define SCAP_ATTRIBUTES_EXPIRED     401
    #define SCAP_ZERO_ATTRIBUTES        402
    #define SCAP_ATTRIBUTES_NOT_VALID   403

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
        // check the Cache language
        //----------------------------------------------------------
        {
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_CACHEDIR);
            QString cacheDir = config.getString();
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
            eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APIKEY);

            std::string temp_secretKey = config.getString();

            QByteArray temp_array(temp_secretKey.data(), temp_secretKey.length());

            QByteArray m_secretKey_array =  QByteArray::fromHex(temp_array);

            m_secretKey = m_secretKey_array.constData();

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

    void setSecretKey(std::string secretKey)
    {
        m_secretKey = secretKey;

        QByteArray array(secretKey.data(), secretKey.length());

        eIDMW::PTEID_Config config(eIDMW::PTEID_PARAM_GENERAL_SCAP_APIKEY);

        config.setString(array.toHex());
    }

    std::string getSecretKey() {
        return m_secretKey;
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
    std::string m_secretKey;

    QString m_scap_server_host;
    QString m_scap_server_port;
};

#endif // _SETTINGS_H_

