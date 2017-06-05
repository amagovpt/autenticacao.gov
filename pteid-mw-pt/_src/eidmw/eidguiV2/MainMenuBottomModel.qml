import QtQuick 2.6

ListModel {
    ListElement {
        name: "Definições"
        imageUrl: "images/definicoes_icon.png"
        imageUrlSel: "images/definicoes_icon_sel.png"
        subdata:[
            ListElement {
                name: "Parâmetros"
                url: "contentPages/definitions/PageDefinitionsParameters.qml"
            }
        ]
    }
    ListElement {
        name: "Ajuda"
        imageUrl: "images/ajuda_icon.png"
        imageUrlSel: "images/ajuda_icon_sel.png"
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
