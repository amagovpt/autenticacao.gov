import QtQuick 2.6

ListModel {
    ListElement {
        name: "Cartão"
        expand: false
        subdata:[
            ListElement {
                name: "Identidade"
                expand: false
                url: "contentPages/card/PageCardIdentify.qml"
            },
            ListElement {
                name: "Outros dados"
                expand: false
                url: "contentPages/card/PageCardOtherData.qml"
            },
            ListElement {
                name: "Morada"
                expand: false
                url: "contentPages/card/PageCardAdress.qml"
            },
            ListElement {
                name: "Notas"
                expand: false
                url: "contentPages/card/PageCardNotes.qml"
            },
            ListElement {
                name: "Imprimir"
                expand: false
                url: "contentPages/card/PageCardPrint.qml"
            }
        ]
    }
    ListElement {
        name: "Assinatura"
        expand: true
        subdata:[
            ListElement {
                name: "Simples"
                expand: true
                url: "contentPages/services/PageServicesSignSimple.qml"
            },
            ListElement {
                name: "Avançada"
                expand: true
                url: "contentPages/services/PageServicesSignAdvanced.qml"
            }
        ]
    }
    ListElement {
        name: "Segurança"
        expand: false
        subdata:[
            ListElement {
                name: "Certificados"
                expand: false
                url: "contentPages/security/PageSecurityCertificates.qml"
            },
            ListElement {
                name: "Códigos PIN"
                expand: false
                url: "contentPages/security/PageSecurityPinCodes.qml"
            }
        ]
    }
}
