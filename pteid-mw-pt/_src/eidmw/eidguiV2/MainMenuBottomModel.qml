import QtQuick 2.6

ListModel {
    ListElement {
        name: "Definições"
        expand: false
        imageUrl: "images/definicoes_icon.png"
        imageUrlSel: "images/definicoes_icon_sel.png"
        subdata:[
            ListElement {
                subName: "Parâmetros"
                expand: false
                url: "contentPages/definitions/PageDefinitionsParameters.qml"
            }
        ]
    }
    ListElement {
        name: "Ajuda"
        expand: false
        imageUrl: "images/ajuda_icon.png"
        imageUrlSel: "images/ajuda_icon_sel.png"
        subdata:[
            ListElement {
                subName: "Documentação Online"
                expand: false
                url: "contentPages/help/PageHelpDocOnline.qml"
            },
            ListElement {
                subName: "Atualizações"
                expand: false
                url: "contentPages/help/PageHelpUpdates.qml"
            },
            ListElement {
                subName: "Acerca"
                expand: false
                url: "contentPages/help/PageHelpAbout.qml"
            }
        ]
    }
}
