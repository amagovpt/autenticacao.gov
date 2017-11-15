#ifndef PDFSIGNATURECLIENT_H
#define PDFSIGNATURECLIENT_H

#include <QString>
#include <QTreeWidget>
#include "cstdio"

class SelectedTreeInfo;
class ProxyInfo;

namespace ACService{
    class ns2__AttributesType;
    class ns3__AttributeType;
}

namespace PDFSignature {
    class ns1__AttributeType;
}

class PDFSignatureInfo{
public:
    PDFSignatureInfo(int _selectedPage, int _x, int _y, bool _isPortrait){
        selectedPage = _selectedPage;
        x = _x;
        y = _y;
        portrait = _isPortrait;
    }

    int getSelectedPage(){return selectedPage;}
    int getX(){return x;}
    int getY(){return y;}
    bool isPortrait(){return portrait;}
private:
    int selectedPage;
    int x;
    int y;
    bool portrait;
};

class PDFSignatureClient
{
public:
    PDFSignatureClient();

    static bool signPDF(ProxyInfo, QString, QString, QString, QString, int, PDFSignatureInfo, std::vector<ACService::ns3__AttributeType *> &);
};

#endif // PDFSIGNATURECLIENT_H
