#include "scapsignature.h"

#include <QApplication>
#include <QTranslator>
#include <iostream>
#include "settings.h"

int main(int argc, char *argv[])
{

#ifdef __APPLE__
    /*
       In MacOS we deploy the QT plugins in a specific location which is common 
       to all the QT applications (pteidgui, ScapSignature, pteiddialogs)
    */
    QCoreApplication::addLibraryPath(QString("/usr/local/Frameworks"));
#endif

    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("PTEID");

    ScapSettings scapSettings;
    QString lang = scapSettings.getPTEIDLanguage();

    QTranslator translator;

    if(lang.compare("nl") == 0) {
        // Loads Portuguese versions
        bool ret = translator.load(QCoreApplication::applicationDirPath() + "/" + "scapsignature_pt");
        std::cout << "Loaded Portuguese version sucessfully: " << ret << std::endl;
    }
    a.installTranslator(&translator);

    ScapSignature w;
    w.setWindowIcon(QIcon(":/appicon/Images/pteid.ico"));
    w.show();

    return a.exec();
}
