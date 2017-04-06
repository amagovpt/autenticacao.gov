#include "PDFSignWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CardInformation CI_Data;
    CI_Data.Reset();
    PDFSignWindow w( NULL, 0, CI_Data);
    w.show();

    return a.exec();
}
