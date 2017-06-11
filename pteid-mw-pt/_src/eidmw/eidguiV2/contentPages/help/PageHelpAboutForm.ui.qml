import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants

Item {
    property alias propertyTextLinkCC: textLinkCC

    anchors.fill: parent
    Text {
        text: "Versão x.x.x - xxxx"
        anchors.verticalCenterOffset: -88
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.family: lato.name
    }

    Text {
        text: "Aplicação Oficial do Cartão de Cidadão Português"
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -112
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: lato.name
    }

    Text {
        text: "Caso tenha dificuldades na utilizaçãp desta aplicação consulte:"
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -54
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: lato.name
    }

    Text {
        id: textLinkCC
        color: Constants.COLOR_MAIN_BLUE
        textFormat: Text.RichText
        text: "<a href=\"https://www.autenticacao.gov.pt/o-cartao-de-cidadao\">https://www.autenticacao.gov.pt/o-cartao-de-cidadao</a>"
        font.italic: true
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -23
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: lato.name

    }

    Text {
        text: "Desenvolvido pelo Estado Português"
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 38
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: lato.name
    }

    Text {
        text: "( Agência para a Modernização Administrativa, IP e \nInstituito dos Registos e do Notaiado, IP)"
        font.pixelSize: Constants.SIZE_TEXT_BODY
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 79
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: lato.name
    }

    Image {
        id: image1
        y: parent.height * 0.1
        width: 218
        height: 41
        source: "../../images/logo_cartao_cidadao.png"
        fillMode: Image.PreserveAspectFit
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.8
        width: 384
        height: 90
        fillMode: Image.PreserveAspectFit
        source: "../../images/logo_cor400.png"
    }
}
