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
                name: "Outros Dados"
                url: "contentPages/card/PageCardOtherData.qml"
            },
            ListElement {
                name: "Morada"
                url: "contentPages/card/PageCardAdress.qml"
            },
            ListElement {
                name: "Certificados"
                url: "contentPages/card/PageCardCertificates.qml"
            },
            ListElement {
                name: "Códigos PIN"
                url: "contentPages/card/PageCardPinCodes.qml"
            },
            ListElement {
                name: "Notas"
                url: "contentPages/card/PageCardNotes.qml"
            }
        ]
    }
    ListElement {
        name: "Serviços"
        subdata:[
            ListElement {
                name: "Assinatura de PDF"
                url: "contentPages/services/PageServicesSignPdf.qml"
            },
            ListElement {
                name: "Assinatura de Profissionais"
                url: "contentPages/services/PageServicesSignProfessional.qml"
            }
        ]
    }
    ListElement {
        name: "Definições"
        subdata:[
            ListElement {
                name: "Parâmetros"
                url: "contentPages/definitions/PageDefinitionsParameters.qml"
            }
        ]
    }
    ListElement {
        name: "Ajuda"
        subdata:[
            ListElement {
                name: "Documentação Online"
                url: "contentPages/help/PageHelpDocOnline.qml"
            },
            ListElement {
                name: "Atualizações"
                url: "contentPages/help/PageHelpUpdates.qml"
            },
            ListElement {
                name: "Acerca"
                url: "contentPages/help/PageHelpAbout.qml"
            }
        ]
    }
}
