import QtQuick 2.6

ListModel {
    ListElement {
        name: "Identidade"
        url: "contentPages/card/PageCardIdentify.qml"
    }
    ListElement {
        name: "Outros Dados"
        url: "contentPages/card/PageCardOtherData.qml"
    }
    ListElement {
        name: "Morada"
        url: "contentPages/card/PageCardAdress.qml"
    }
    ListElement {
        name: "Certificados"
        url: "contentPages/card/PageCardCertificates.qml"
    }
    ListElement {
        name: "Códigos PIN"
        url: "contentPages/card/PageCardPinCodes.qml"
    }
    ListElement {
        name: "Notas"
        url: "contentPages/card/PageCardNotes.qml"
    }
}
