import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignSimpleForm {

    Dialog {
        id: dialog
        title: "Arraste um único ficheiro"
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Label {
            text: "Para assinar múltiplos ficheiros use a opção assinatura avançada"
        }
    }
    Dialog {
        id: dialogSignCMD
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogSignCMD.width * 0.5
        y: parent.height * 0.5 - dialogSignCMD.height * 0.5

        header: Label {
            id: labelTextTitle
            text: "Assinar com Chave Móvel Digital"
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectMessage.height + rectNumTelemovel.height + rectPin.height

            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textLinkCMD
                    textFormat: Text.RichText
                    text: "<a href=\"https://cmd.autenticacao.gov.pt/Ama.Authentication.Frontend\">\
                    Click para conhecer a Chave Móvel Digital</a>"
                    font.italic: true
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                }
            }

            Item {
                id: rectNumTelemovel
                width: parent.width
                height: 50
                anchors.top: rectMessage.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: "Nº de Telemóvel"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldMobileNumber
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldMobileNumber.text === "" ? true: false
                    placeholderText: "Nº de Telemóvel? ex: +351 900000000"
                    validator: RegExpValidator { regExp: /\+[0-9]+\ [0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPin
                width: parent.width
                height: 50
                anchors.top: rectNumTelemovel.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: "PIN da Chave Móvel Digital"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldPin
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldPin.text === "" ? true: false
                    placeholderText: "PIN?"
                    echoMode : TextInput.Password
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textPinNew.right
                    anchors.bottom: parent.bottom
                }
            }
        }

        standardButtons: {
            textFieldMobileNumber.length !== 0 && textFieldPin.length !== 0
                    ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = ""
            var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
            var page = 1
            propertyCheckLastPage.checked ? page = gapi.getPDFpageCount(loadedFilePath) :
                                        page = propertySpinBoxControl.value
            var reason = ""
            var location = ""
            var isSmallSignature = ""
            var coord_x = propertyPDFPreview.propertyCoordX
            //coord_y must be the lower left corner of the signature rectangle
            var coord_y = propertyPDFPreview.propertyCoordY

            console.log("Output filename: " + outputFile)
            console.log("Signing in position coord_x: " + coord_x
                        + " and coord_y: "+coord_y)

            gapi.signCMD(textFieldMobileNumber.text,textFieldPin.text,
                         loadedFilePath,outputFile,page,
                         coord_x,coord_y,
                         reason,location,
                         isTimestamp, isSmallSignature)

            progressBarIndeterminate.visible = true
            progressBar.visible = true
            textFieldMobileNumber.text = ""
            textFieldPin.text = ""
            dialogCMDProgress.open()
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }
    Dialog {
        id: dialogCMDProgress
        width: 600
        height: 300
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - dialogCMDProgress.width * 0.5
        y: parent.height * 0.5 - dialogCMDProgress.height * 0.5

        header: Label {
            id: labelConfirmOfAddressProgressTextTitle
            text: "Assinar com Chave Móvel Digital"
            visible: true
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + progressBar.height + progressBarIndeterminate.height

            Item {
                id: rectMessageTop
                width: parent.width
                height: 150
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    text: ""
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }

            ProgressBar {
                id: progressBar
                width: parent.width * 0.5
                anchors.top: rectMessageTop.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: true
                indeterminate: false
                z:1

            }

            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width * 0.5
                anchors.top: progressBar.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                height: 20
                to: 100
                value: 0
                visible: true
                indeterminate: true
                z:1
            }

        }

        standardButtons: {
            DialogButtonBox.Ok
        }

        onAccepted: {
            mainFormID.opacity = 1.0
        }
        onRejected: {
            mainFormID.opacity = 1.0
        }
    }
    Connections {
        target: gapi
        onSignalPdfSignSucess: {
            signsuccess_dialog.visible = true
        }
        onSignalCardAccessError: {
            propertyBusyIndicator.running = false
            cardLoaded = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Simple --> Data Changed")
            //console.trace();
            propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
                    + gapi.getDataCardIdentifyValue(GAPI.Givenname)
                    + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyBusyIndicator.running = false
            cardLoaded = true
        }
        onSignalCardChanged: {
            console.log("Services Sign Simple onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
                propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                cardLoaded = false
            }
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
        }
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
                progressBar.visible = false
                progressBarIndeterminate.visible = false
            }
        }
    }
    Connections {
        target: image_provider_pdf
        onSignalPdfSourceChanged: {
            console.log("Receive signal onSignalPdfSourceChanged pdfWidth = "+pdfWidth+" pdfHeight = "+pdfHeight);
            propertyPDFPreview.propertyPdfOriginalWidth=pdfWidth
            propertyPDFPreview.propertyPdfOriginalHeight=pdfHeight
            propertyBusyIndicator.running = false
        }
    }
    Dialog {
        id: signsuccess_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - signsuccess_dialog.height * 0.5

        header: Label {
            text: "Ficheiro(s) assinado(s) com sucesso"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        standardButtons: DialogButtonBox.Ok
    }

    Dialog {
        id: signerror_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signerror_dialog.width * 0.5
        y: parent.height * 0.5 - signerror_dialog.height * 0.5

        header: Label {
            text: "Erro na assinatura de PDF"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: 16
            color: Constants.COLOR_MAIN_BLUE
        }

        standardButtons: DialogButtonBox.Ok
    }

    propertyDropArea {

        onEntered: {
            console.log("You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            if(filesArray.length > 1){
                dialog.open()
            }else{
                console.log("Adding file: " + filesArray[0])
                var path =  filesArray[0]
                //  Get the path itself without a regex
                if (Qt.platform.os === "windows") {
                    path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
                }else{
                    path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
                }
                filesModel.append({
                                      "fileUrl": path
                                  })
            }
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }
    propertyFileDialogOutput {
        onAccepted: {

            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = false
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

                var page = 1
                propertyCheckLastPage.checked ? page = gapi.getPDFpageCount(loadedFilePath) :
                                            page = propertySpinBoxControl.value
                var reason = ""
                var location = ""

                var isSmallSignature = false

                var coord_x = propertyPDFPreview.propertyCoordX

                //coord_y must be the lower left corner of the signature rectangle

                var coord_y = propertyPDFPreview.propertyCoordY

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y)

                gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                     reason, location, isTimestamp, isSmallSignature)
        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            dialogSignCMD.open()
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Adding file: " + propertyFileDialog.fileUrls[0])
            var path = propertyFileDialog.fileUrls[0];
            //  Get the path itself without a regex
            if (Qt.platform.os === "windows") {
                path = path.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                path = path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
            filesModel.append({
                                  "fileUrl": path
                              })
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    propertyMouseAreaRectMain {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyButtonRemove {
        onClicked: {
            fileLoaded = false
            filesModel.clear()
        }
    }
    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            propertyFileDialogOutput.filename = filesModel.get(0).fileUrl + "_signed.pdf"
            propertyFileDialogOutput.open()
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD")
            propertyFileDialogCMDOutput.filename = filesModel.get(0).fileUrl + "_signed.pdf"
            propertyFileDialogCMDOutput.open()
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged count:"
                        + filesModel.count)
            if(filesModel.count === 0){
                fileLoaded = false
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragSigImg.visible = false
                propertyPDFPreview.propertyDragSigSignedByText.visible = false
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = false
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
                propertyPDFPreview.propertyDragSigDateText.visible = false
                propertyPDFPreview.propertyDragSigImg.visible = false
            }else{
                fileLoaded = true
                propertyBusyIndicator.running = true
                var loadedFilePath = filesModel.get(0).fileUrl
                var pageCount = gapi.getPDFpageCount(loadedFilePath)
                console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount)
                propertySpinBoxControl.value = 1
                propertySpinBoxControl.to = pageCount
                propertyPDFPreview.propertyBackground.cache = false
                propertyPDFPreview.propertyBackground.source = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                propertyPDFPreview.propertyDragSigImg.visible = true
                propertyPDFPreview.propertyDragSigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"
                propertyPDFPreview.propertyDragSigWaterImg.visible = true
                propertyPDFPreview.propertyDragSigSignedByText.visible = true
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                propertyPDFPreview.propertyDragSigNumIdText.visible = true
                propertyPDFPreview.propertyDragSigDateText.visible = true
                propertyPDFPreview.propertyDragSigImg.visible = true
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value

            propertySpinBoxControl.up.indicator.visible = true
            propertySpinBoxControl.down.indicator.visible = true
            if(propertySpinBoxControl.value === gapi.getPDFpageCount(loadedFilePath)){
                propertySpinBoxControl.up.indicator.visible = false
            }
            if (propertySpinBoxControl.value === 1){
                propertySpinBoxControl.down.indicator.visible = false
            }

        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            if(propertyCheckLastPage.checked){
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
            }else{
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            }
        }
    }
    Component.onCompleted: {
        if (gapi.getShortcutFlag() > 0)
            filesModel.append(
                        {
                            "fileUrl": gapi.getShortcutInputPDF()
                        });

        console.log("Page Services Sign Simple mainWindowCompleted")
        propertyBusyIndicator.running = true
        propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
        propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
        gapi.startCardReading()
    }
}
