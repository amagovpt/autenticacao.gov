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
                url: "contentPages/services/PageServicesSignPdf.qml"
            },
            ListElement {
                name: "Avançada"
                expand: true
                url: "contentPages/services/PageServicesSignProfessional.qml"
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
