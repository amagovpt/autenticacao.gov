import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

PageSecurityPinCodesForm {

    Dialog {
        id: dialogInput
        title: "Testar PIN"
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: "PIN Introduzido correctamente"
        }
    }

    Dialog {
        id: dialogModify
        title: "Modificar PIN"
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: "PIN Modificado correctamente"
        }
    }

    buttonTest.onClicked: {
        dialogInput.open()
    }

    buttonModify.onClicked: {
        dialogModify.open()
    }
}
