/*-****************************************************************************

 * Copyright (C) 2017, 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

ListModel {
    ListElement {
        name: QT_TR_NOOP("STR_MENU_DEFINITIONS")
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
                subName: QT_TR_NOOP("STR_MENU_SIGN_SETTINGS")
                expand: false
                url: "contentPages/definitions/PageDefinitionsSignSettings.qml"
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
        name: QT_TR_NOOP("STR_MENU_HELP")
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
                subName: QT_TR_NOOP("STR_MENU_ACCESSIBILITY")
                expand: false
                url: "contentPages/help/PageHelpAccessibility.qml"
            },
            ListElement {
                subName: QT_TR_NOOP("STR_MENU_ABOUT")
                expand: false
                url: "contentPages/help/PageHelpAbout.qml"
            }
        ]
    }
}
