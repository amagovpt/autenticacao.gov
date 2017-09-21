import QtQuick 2.6

ListModel {
    ListElement {
        name: QT_TR_NOOP("STR_MENU_SIGN_CARD")
        expand: false
        subdata:[
            ListElement {
                name: QT_TR_NOOP("STR_MENU_IDENTITY")
                expand: false
                url: "contentPages/card/PageCardIdentify.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_OTHER_DATA")
                expand: false
                url: "contentPages/card/PageCardOtherData.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_ADDRESS")
                expand: false
                url: "contentPages/card/PageCardAdress.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_NOTES")
                expand: false
                url: "contentPages/card/PageCardNotes.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_PRINT")
                expand: false
                url: "contentPages/card/PageCardPrint.qml"
            }
        ]
    }
    ListElement {
        name: QT_TR_NOOP("STR_MENU_SIGN")
        expand: true
        subdata:[
            ListElement {
                name: QT_TR_NOOP("STR_MENU_SIGN_SIMPLE")
                expand: true
                url: "contentPages/services/PageServicesSignSimple.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_SIGN_ADVANCE")
                expand: true
                url: "contentPages/services/PageServicesSignAdvanced.qml"
            }
        ]
    }
    ListElement {
        name: QT_TR_NOOP("STR_MENU_SECURITY")
        expand: false
        subdata:[
            ListElement {
                name: QT_TR_NOOP("STR_MENU_CERTIFICATES")
                expand: false
                url: "contentPages/security/PageSecurityCertificates.qml"
            },
            ListElement {
                name: QT_TR_NOOP("STR_MENU_PIN_CODES")
                expand: false
                url: "contentPages/security/PageSecurityPinCodes.qml"
            }
        ]
    }
}
