import QtQuick 2.6

ListModel {
    ListElement {
        name: "Cartão"
        subdata:[
            ListElement {
                name: "Identidade"
                url: "contentPages/card/PageCardIdentify.qml"
            },
            ListElement {
                name: "Outros dados"
                url: "contentPages/card/PageCardOtherData.qml"
            },
            ListElement {
                name: "Morada"
                url: "contentPages/card/PageCardAdress.qml"
            },
            ListElement {
                name: "Notas"
                url: "contentPages/card/PageCardNotes.qml"
            }
        ]
    }
    ListElement {
        name: "Assinatura"
        subdata:[
            ListElement {
                name: "Simples"
                url: "contentPages/services/PageServicesSignPdf.qml"
            },
            ListElement {
                name: "Avançada"
                url: "contentPages/services/PageServicesSignProfessional.qml"
            }
        ]
    }
    ListElement {
        name: "Segurança"
        subdata:[
            ListElement {
                name: "Certificados"
                url: "contentPages/security/PageSecurityCertificates.qml"
            },
            ListElement {
                name: "Códigos PIN"
                url: "contentPages/security/PageSecurityPinCodes.qml"
            }
        ]
    }
}
