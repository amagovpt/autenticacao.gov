import QtQuick 2.6
import QtQuick.Controls 2.1
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardPrintForm {

    Dialog {
        id: dialogPinOK
        width: 400
        height: 200
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogPinOK.width * 0.5
        y: parent.height * 0.5 - dialogPinOK.height * 0.5

        property alias propertyDialogOkLabelText: dialogOkLabelText
        property alias propertyTextOkPin: textOkPin

        header: Label {
            id: dialogOkLabelText
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectOkPin.height

            Item {
                id: rectOkPin
                width: parent.width
                height: 90
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textOkPin
                    text: "PIN introduzido correctamente."
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: 150
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height:Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectOkPin.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: dialogPinOK.close()
            }
        }
    }
    Connections {
        target: gapi
        onSignalPdfPrintSignSucess: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = "Gerar PDF"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Ficheiro PDF assinado gerado com sucesso"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
        onSignalPdfPrintSucess: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = "Gerar PDF"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Ficheiro PDF gerado com sucesso"
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
        onSignalPdfPrintFail: {
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text = "Gerar PDF"
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "Erro ao gerar pdf. \
Verifique se tem permissoes de escrita no ficheiro  \ne no directorio de destino."
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
        onSignalCardDataChanged: {
            console.log("Data Card Print --> Data Changed")
            propertyBusyIndicator.running = false
            propertyButtonPrint.enabled = true
            propertyButtonPdf.enabled = true
            propertySwitchBasic.enabled = true
            propertySwitchAdditional.enabled = true
            propertySwitchAddress.enabled = true
            propertySwitchNotes.enabled = true
            propertySwitchPdfSign.enabled = true
            propertySwitchBasic.checked = false
            propertySwitchAdditional.checked = false
            propertySwitchAddress.checked = false
            propertySwitchNotes.checked = false
            propertySwitchPdfSign.checked = false
        }
        onSignalCardAccessError: {
            console.log("Card Print Page onSignalCardAccessError")
            if (error_code == GAPI.NoReaderFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "No card reader found!"
                disableComponents()
            }
            else if (error_code == GAPI.NoCardFound) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "No Card Found!"
                disableComponents()
            }
            else if (error_code == GAPI.SodCardReadError) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        "Consistência da informação do cartão está comprometida!"
                disableComponents()
            }
            else {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Error"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Card Access Error!"
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            propertyBusyIndicator.running = false
        }
        onSignalCardChanged: {
            console.log("Card Print Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "Cartão do Cidadão removido"
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Cartão do Cidadão inserido"
                propertyBusyIndicator.running = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        "Erro da aplicação! Por favor reinstale a aplicação:"

            }
            disableComponents()
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }

    }

    propertyFileDialogOutput {
        onAccepted: {

            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

            console.log("Output filename: " + outputFile)
            gapi.startPrintPDF(outputFile,
                               propertySwitchBasic.checked,
                               propertySwitchAdditional.checked,
                               propertySwitchAddress.checked,
                               propertySwitchNotes.checked,
                               propertySwitchPdfSign.checked)
        }
    }

    propertyButtonPdf {
        onClicked: {
            propertyFileDialogOutput.filename = "CartaoCidadao_signed.pdf"
            propertyFileDialogOutput.open()
        }
    }
    propertyButtonPrint {
        onClicked: {
            console.log("Printing file")
            gapi.startPrint("",
                               propertySwitchBasic.checked,
                               propertySwitchAdditional.checked,
                               propertySwitchAddress.checked,
                               propertySwitchNotes.checked,
                               propertySwitchPdfSign.checked)
        }
    }

    propertySwitchAddress{
        onCheckedChanged: {
            if(propertySwitchAddress.checked){
                var triesLeft = 0
                triesLeft = gapi.verifyAddressPin("")
                if (triesLeft === 3) {
                    dialogPinOK.propertyDialogOkLabelText.text = "Verificação de PIN"
                    dialogPinOK.propertyTextOkPin.text = "PIN introduzido correctamente"
                    dialogPinOK.open()
                }else {
                    propertySwitchAddress.checked = false
                }
            }
        }
    }
    propertySwitchPdfSign {
        onCheckedChanged: {
            if(propertySwitchPdfSign.checked){
                propertyButtonPrint.enabled = false
                propertyButtonPrint.checked = false
            } else {
                propertyButtonPrint.enabled = true
                propertyButtonPrint.checked = false
            }
        }
    }

    Component.onCompleted: {
        console.log("Page Card Print mainWindow Completed")
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }

    function disableComponents(){
        propertySwitchBasic.enabled = false
        propertySwitchAdditional.enabled = false
        propertySwitchAddress.enabled = false
        propertySwitchNotes.enabled = false
        propertySwitchPdfSign.enabled = false
        propertySwitchBasic.checked = false
        propertySwitchAdditional.checked = false
        propertySwitchAddress.checked = false
        propertySwitchNotes.checked = false
        propertySwitchPdfSign.checked = false
        propertyButtonPrint.enabled = false
        propertyButtonPdf.enabled = false
    }
}
