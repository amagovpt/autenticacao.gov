#ifndef CHILDRENINFO
#define CHILDRENINFO

#include <iostream>
#include <QString>
#include <vector>

class TreeItemInfo{
public:
    TreeItemInfo() {};
    TreeItemInfo(const char * _treeItemName,const char * _treeItemID){
        treeItemID = QString::fromUtf8(_treeItemID);
        treeItemName = QString::fromUtf8(_treeItemName);
    }

    QString getTreeItemId(){ return treeItemID; }
    QString getTreeItemName(){ return treeItemName; }

private:
    QString treeItemID;
    QString treeItemName;
};

class SelectedTreeInfo{
public:
    SelectedTreeInfo(TreeItemInfo _parent, std::vector<TreeItemInfo> _childs){
        parentItem = _parent;
        childsItem = _childs;
    }

    TreeItemInfo getParent(){ return parentItem; }
    std::vector<TreeItemInfo> getChilds(){ return childsItem; }

private:
    TreeItemInfo parentItem;
    std::vector<TreeItemInfo> childsItem;
};

#endif // CHILDRENINFO
