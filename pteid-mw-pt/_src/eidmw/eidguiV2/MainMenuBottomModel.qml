import QtQuick 2.6

ListModel {
    ListElement {
        name: "Definitions"
        expand: false
        imageUrl: "images/definitions_icon.png"
        imageUrlSel: "images/definitions_icon_selected.png"
        subdata:[
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_CUSTOM_SIGN")
                expand: false
                url: "contentPages/definitions/PageDefinitionsSignature.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_ATTRIBUTES")
                expand: false
                url: "contentPages/definitions/PageDefinitionsSCAP.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_APP_SETTINGS")
                expand: false
                url: "contentPages/definitions/PageDefinitionsApp.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_APP_DATA")
                expand: false
                url: "contentPages/definitions/PageDefinitionsData.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_UPDATES")
                expand: false
                url: "contentPages/definitions/PageDefinitionsUpdates.qml"
            }
        ]
    }
    ListElement {
        name: "Help"
        expand: false
        imageUrl: "images/help_icon.png"
        imageUrlSel: "images/help_icon_selected.png"
        subdata:[
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_DOCUMENTATION")
                expand: false
                url: "contentPages/help/PageHelpDocOnline.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_ABOUT")
                expand: false
                url: "contentPages/help/PageHelpAbout.qml"
            }
        ]
    }
}
