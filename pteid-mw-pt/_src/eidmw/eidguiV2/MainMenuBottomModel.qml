import QtQuick 2.6

ListModel {
    ListElement {
        name: "Definições"
        expand: false
        imageUrl: "images/definitions_icon.png"
        imageUrlSel: "images/definitions_icon_selected.png"
        subdata:[
            ListElement {
                subName: "Personalização da Assinatura"
                expand: false
                url: "contentPages/definitions/PageDefinitionsParameters.qml"
            },
            ListElement {
                subName: "Configuração da aplicação"
                expand: false
                url: "contentPages/definitions/PageDefinitionsApp.qml"
            },
            ListElement {
                subName: "Atualizações"
                expand: false
                url: "contentPages/definitions/PageDefinitionsUpdates.qml"
            }
        ]
    }
    ListElement {
        name: "Ajuda"
        expand: false
        imageUrl: "images/help_icon.png"
        imageUrlSel: "images/help_icon_selected.png"
        subdata:[
            ListElement {
                subName: "Documentação"
                expand: false
                url: "contentPages/help/PageHelpDocOnline.qml"
            },
            ListElement {
                subName: "Acerca"
                expand: false
                url: "contentPages/help/PageHelpAbout.qml"
            }
        ]
    }
}
