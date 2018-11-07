#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <Qt>
#include <QtCore>
#include <QString>
#include <QSettings>

#include "eidlib.h"


#define STR_LANGUAGE		"EN"

class ScapSettings
{
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

    QString m_scap_server_host;
    QString m_scap_server_port;
};

#endif // _SETTINGS_H_

