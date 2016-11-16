#include "scapsignature.h"

#include <QApplication>
#include <QTranslator>
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("PTEID");

    ScapSettings scapSettings;
    QString lang = scapSettings.getPTEIDLanguage();

    QTranslator translator;
    if(lang.compare("nl") == 0){
        // Loads Portuguese versions
        bool ret = translator.load("scapsignature_pt");
        std::cout << "Loaded Portuguese version sucessfully: " << ret << std::endl;
    }
    a.installTranslator(&translator);

    ScapSignature w;
    w.setWindowIcon(QIcon(":/appicon/Images/pteid.ico"));
    w.show();

    return a.exec();
}
