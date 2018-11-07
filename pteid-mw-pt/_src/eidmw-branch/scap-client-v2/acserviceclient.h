#ifndef ACSERVICECLIENT_H
#define ACSERVICECLIENT_H

#include <vector>
#include <QString>

namespace eIDMW {
    class PTEID_ReaderContext;
}

namespace ACService {
    class ns2__AttributeSupplierListType;
    class ns3__AttributeSupplierType;
    class ns3__PersonalDataType;
    class ns2__AttributesType;
    class ns3__AttributeType;
};

class ns2__AttributeSupplierListType;
class ns3__AttributeSupplierType;
class TreeItemInfo;

class ACServiceClient
{
public:
    ACServiceClient();

    static std::vector<ACService::ns2__AttributesType *> reqAttributeSupplierListType(std::vector<ns3__AttributeSupplierType *>);
    static TreeItemInfo getAttrSupplierInfo(ACService::ns2__AttributesType *);
    static std::vector<TreeItemInfo> getChildrenTreeInfo(ACService::ns2__AttributesType *);
    static ACService::ns3__AttributeType * getAttributeType(QString, QString, std::vector<ACService::ns2__AttributesType *>);

    static std::vector<ACService::ns2__AttributesType *> loadAttributesFromCache();
};
#endif // ACSERVICECLIENT_H
