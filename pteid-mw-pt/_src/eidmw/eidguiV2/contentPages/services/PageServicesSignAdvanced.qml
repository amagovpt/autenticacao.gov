import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignAdvancedForm {

    ToolTip {
        id: controlToolTip
        contentItem: Text {
            text: controlToolTip.text
            font: controlToolTip.font
            color: Constants.COLOR_MAIN_PRETO
        }
        background: Rectangle {
            border.color: Constants.COLOR_MAIN_DARK_GRAY
            color: Constants.COLOR_MAIN_SOFT_GRAY
        }
    }

    Connections {
        target: gapi
        onSignalOpenCMDSucess: {
            console.log("Sign Advanced - Signal Open CMD Sucess")
            progressBarIndeterminate.visible = false
            rectReturnCode.visible = true
            dialogCMDProgress.standardButtons = DialogButtonBox.Ok | DialogButtonBox.Cancel
        }
        onSignalCloseCMDSucess: {
            console.log("Sign Advanced - Signal Close CMD Sucess")
            progressBarIndeterminate.visible = false
            dialogCMDProgress.standardButtons = DialogButtonBox.Ok
        }
        onSignalPdfSignSucess: {
            signsuccess_dialog.visible = true
        }
        onSignalCardAccessError: {
            propertyBusyIndicator.running = false
            cardLoaded = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Advanced --> Data Changed")
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
            console.log("Services Sign Advanced onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =  "Cartão do Cidadão removido"
                propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
                propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text = "Cartão do Cidadão inserido"
                propertyBusyIndicator.running = true
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =  "Leitura do Cartão"
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        "Erro da aplicação! Por favor reinstale a aplicação:"
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
        }
        onSignalUpdateProgressStatus: {
            console.log("CMD sign change --> update progress status with text = " + statusMessage)
            textMessageTop.text = statusMessage
        }
        onSignalUpdateProgressBar: {
            console.log("CMD sign change --> update progress bar with value = " + value)
            progressBar.value = value
            if(value === 100) {
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
        }
    }

    Dialog {
        id: dialogSignCMD
        width: 400
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
            height: rectMessageTopLogin.height + rectMobilNumber.height + rectPin.height

            Item {
                id: rectMessageTopLogin
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTopLogin
                    text: "Introduza dados de login"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: rectMobilNumber
                width: parent.width
                height: 50
                anchors.top: rectMessageTopLogin.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinCurrent
                    text: "Número de Telemóvel"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                ComboBox {
                    id: comboBoxIndicative
                    width: parent.width * 0.20
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    model: ["+351"]
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                    visible: true
                    anchors.left: textPinCurrent.right
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldMobileNumber
                    width: parent.width * 0.25
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldMobileNumber.text === "" ? true: false
                    placeholderText: "Número?"
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: comboBoxIndicative.right
                    anchors.leftMargin:  parent.width * 0.05
                    anchors.bottom: parent.bottom
                }
            }
            Item {
                id: rectPin
                width: parent.width
                height: 50
                anchors.top: rectMobilNumber.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textPinNew
                    text: "PIN"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
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
            Item {
                id: rectMessage
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectPin.bottom
                Text {
                    id: textLinkCMD
                    textFormat: Text.RichText
                    text: "<a href=\"https://cmd.autenticacao.gov.pt/Ama.Authentication.Frontend\">\
                    Clique para conhecer a Chave Móvel Digital</a>"
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
        }

        standardButtons: {
            textFieldMobileNumber.length !== 0 && textFieldPin.length !== 0
                    ? DialogButtonBox.Ok | DialogButtonBox.Cancel : DialogButtonBox.Cancel
        }

        onAccepted: {
            var loadedFilePath = filesModel.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogCMDOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }
            var page = propertySpinBoxControl.value
            var reason = propertyTextFieldReason.text
            var location = propertyTextFieldLocal.text
            var isSmallSignature = propertyCheckSignReduced.checked
            var coord_x = propertyPDFPreview.propertyCoordX
            //coord_y must be the lower left corner of the signature rectangle
            var coord_y = propertyPDFPreview.propertyCoordY

            var mobileNumber = comboBoxIndicative.currentText + " " + textFieldMobileNumber.text

            gapi.signOpenCMD(mobileNumber,textFieldPin.text,
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
        width: 400
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
        ProgressBar {
            id: progressBar
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            height: 20
            to: 100
            value: 0
            visible: true
            indeterminate: false
            z:1
        }

        Item {
            width: parent.width
            height: rectMessageTop.height + rectReturnCode.height + progressBarIndeterminate.height
            anchors.top: progressBar.bottom

            Item {
                id: rectMessageTop
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: textMessageTop
                    text: ""
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.WordWrap
                }
            }
            Item {
                id: rectReturnCode
                width: parent.width
                height: 50
                anchors.top: rectMessageTop.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
                Text {
                    id: textReturnCode
                    text: "Introduza o código :"
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    height: parent.height
                    width: parent.width * 0.5
                    anchors.bottom: parent.bottom
                }
                TextField {
                    id: textFieldReturnCode
                    width: parent.width * 0.5
                    anchors.verticalCenter: parent.verticalCenter
                    font.italic: textFieldReturnCode.text === "" ? true: false
                    placeholderText: "Código?"
                    validator: RegExpValidator { regExp: /[0-9]+/ }
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    clip: false
                    anchors.left: textReturnCode.right
                    anchors.bottom: parent.bottom
                }
            }
            ProgressBar {
                id: progressBarIndeterminate
                width: parent.width
                anchors.top: rectReturnCode.bottom
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
            DialogButtonBox.Cancel
        }

        onAccepted: {
            console.log("Send sms_token : " + textFieldReturnCode.text)
            if( progressBar.value < 100){
                gapi.signCloseCMD(textFieldReturnCode.text)
                progressBarIndeterminate.visible = true
                rectReturnCode.visible = false
                textFieldReturnCode.text = ""
                dialogCMDProgress.open()
                dialogCMDProgress.standardButtons = DialogButtonBox.Cancel
            }else{
                dialogCMDProgress.close()
                mainFormID.opacity = 1.0
            }
        }
        onRejected: {
            mainFormID.opacity = 1.0
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

    propertyMouseAreaToolTipPades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = "Ficheiros PDF"
            controlToolTip.x = propertyMouseAreaToolTipPadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }

        onExited: {
            controlToolTip.close()
        }
    }
    propertyMouseAreaToolTipXades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = "Pacote XADES / CCSIGN"
            controlToolTip.x = propertyMouseAreaToolTipXadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
        onExited: {
            controlToolTip.close()
        }
    }

    propertyDropArea {

        onEntered: {
            console.log("Signature advanced ! You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            // Update sign preview position variables to be used to send to sdk
            propertyPDFPreview.updateSignPreview(drop.x,drop.y)
            //TODO: Validate files type
            for(var i = 0; i < filesArray.length; i++){
                console.log("Adding file: " + filesArray[i])
                var path =  filesArray[i]
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
            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }
    propertyDropFileArea {
        onEntered: {
            console.log("Signature advanced Drop File Area! You chose file(s): " + drag.urls);
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            //TODO: Validate files type
            for(var i = 0; i < filesArray.length; i++){
                console.log("Adding file: " + filesArray[i])
                var path =  filesArray[i]
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
            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialogOutput {
        onAccepted: {

            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

            if (propertyRadioButtonPADES.checked) {

                var page = propertySpinBoxControl.value
                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked
                var coord_x = propertyPDFPreview.propertyCoordX

                //coord_y must be the lower left corner of the signature rectangle

                var coord_y = propertyPDFPreview.propertyCoordY

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page)

                gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                     reason, location, isTimestamp, isSmallSignature)
            }
            else {
                gapi.startSigningXADES(loadedFilePath, outputFile, isTimestamp)
            }

        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            mainFormID.opacity = 0.5
            dialogSignCMD.open()
        }
    }
    propertyFileDialogBatchOutput {
        onAccepted: {

            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogBatchOutput.folder.toString()
            if (Qt.platform.os === "windows") {
                outputFile = outputFile.replace(/^(file:\/{3})|(qrc:\/{3})|(http:\/{3})/,"");
            }else{
                outputFile = outputFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
            }

            if (propertyRadioButtonPADES.checked) {

                var page = propertySpinBoxControl.value
                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked
                var coord_x = propertyPDFPreview.propertyCoordX

                //coord_y must be the lower left corner of the signature rectangle

                var coord_y = propertyPDFPreview.propertyCoordY

                console.log("Output filename: " + outputFile)
                console.log("Signing Batch in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page)

                var batchFilesArray = []
                for(var i = 0; i < propertyListViewFiles.count; i++){
                    batchFilesArray[i] =  propertyListViewFiles.model.get(i).fileUrl;
                }
                gapi.startSigningBatchPDF(batchFilesArray, outputFile, page, coord_x, coord_y,
                                          reason, location, isTimestamp, isSmallSignature)

            }
            else {
                // TODO: Implement signing batch in Xades mode
                gapi.startSigningXADES(loadedFilePath, outputFile, isTimestamp)
            }

        }
    }

    propertyTextFieldReason{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
        }
    }
    propertyTextFieldLocal{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text
        }
    }

    propertyCheckSignReduced{
        onCheckedChanged: {
            if(propertyCheckSignReduced.checked){
                propertyPDFPreview.propertySigHidth = 45
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.2
                propertyPDFPreview.propertyDragSigReasonText.height = 0
                propertyPDFPreview.propertyDragSigLocationText.height = 0
                propertyPDFPreview.propertyDragSigReasonText.text = ""
                propertyPDFPreview.propertyDragSigLocationText.text = ""
                propertyPDFPreview.propertyDragSigImg.height = 0
                propertyPDFPreview.propertyDragSigWaterImg.height = 0
            }else{
                propertyPDFPreview.propertySigHidth = 90
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.1
                propertyPDFPreview.propertyDragSigReasonText.height = propertyPDFPreview.propertySigLineHeight
                propertyPDFPreview.propertyDragSigLocationText.height = propertyPDFPreview.propertySigLineHeight
                propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
                propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text
                propertyPDFPreview.propertyDragSigImg.height = propertyPDFPreview.propertyDragSigRect.height * 0.3
                propertyPDFPreview.propertyDragSigWaterImg.height = propertyPDFPreview.propertyDragSigRect.height * 0.4
            }
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Num files: " + propertyFileDialog.fileUrls.length)

            for(var i = 0; i < propertyFileDialog.fileUrls.length; i++){
                console.log("Adding file: " + propertyFileDialog.fileUrls[i])
                var path = propertyFileDialog.fileUrls[i];
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

            // Force scroll and focus to the last item addded
            forceScrollandFocus()
        }
        onRejected: {
            console.log("Canceled")
        }
    }


    propertyMouseAreaRectMainRigh {
        onClicked: {
            console.log("propertyMouseAreaRectMainRigh clicked")
            propertyFileDialog.visible = true
        }
    }

    propertyMouseAreaItemOptionsFiles {
        onClicked: {
            console.log("propertyMouseAreaItemOptionsFiles clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonAdd {
        onClicked: {
            console.log("propertyButtonAdd clicked")
            propertyFileDialog.visible = true
        }
    }
    propertyButtonRemoveAll {
        onClicked: {
            console.log("Removing all files")
            propertyListViewFiles.model.clear()
        }
    }

    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            if (propertyListViewFiles.count == 1){
                propertyFileDialogBatchOutput.title = "Escolha o ficheiro de destino"
                if (propertyRadioButtonPADES.checked) {
                    var outputFile =  propertyListViewFiles.model.get(0).fileUrl
                    outputFile =  outputFile.substring(0, outputFile.lastIndexOf('.'));
                    propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                }
                else {
                    propertyFileDialogOutput.filename = "xadessign.ccsigned"
                }
                propertyFileDialogOutput.open()
            }else{
                propertyFileDialogBatchOutput.title = "Escolha a pasta de destino dos ficheiros"
                propertyFileDialogBatchOutput.open()
            }
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD" )
            var outputFile =  filesModel.get(0).fileUrl
            outputFile =  outputFile.substring(0, outputFile.lastIndexOf('.'));
            propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf"
            propertyFileDialogCMDOutput.open()
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    propertyFileDialog{

        nameFilters: propertyRadioButtonPADES.checked ?
                         ["PDF document (*.pdf)"] :
                         ["All files (*)"]
    }

    propertyRadioButtonPADES{
        onCheckedChanged:{
            if(propertyRadioButtonPADES.checked){
                propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                        "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
                propertySpinBoxControl.value = 1
                filesModel.clear()
            }else{
                propertyTextDragMsgImg.text =
                        "Pré-visualização não disponível"
                propertySpinBoxControl.value = 1
                filesModel.clear()
            }
        }
    }

    Component {
        id: listViewFilesDelegate
        Rectangle{
            id: rectlistViewFilesDelegate
            width: parent.width - propertyFilesListViewScroll.width
                   - Constants.SIZE_ROW_H_SPACE * 0.5
            color: getColorItem(mouseAreaFileName.containsMouse,
                                mouseAreaIconDelete.containsMouse)
            MouseArea {
                id: mouseAreaFileName
                anchors.fill: parent
                hoverEnabled : true
            }

            Item {
                width: parent.width
                height: parent.height

                Text {
                    id: fileName
                    text: fileUrl
                    width: parent.width - iconRemove.width - Constants.SIZE_LISTVIEW_IMAGE_SPACE
                    x: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_TEXT_BODY
                    wrapMode: Text.WrapAnywhere
                    Component.onCompleted: {
                        if(fileName.height > iconRemove.height){
                            rectlistViewFilesDelegate.height = fileName.height + 5
                        }else{
                            rectlistViewFilesDelegate.height = iconRemove.height
                        }
                    }
                }
                Image {
                    id: iconRemove
                    x: fileName.width + Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    width: Constants.SIZE_IMAGE_FILE_REMOVE
                    height: Constants.SIZE_IMAGE_FILE_REMOVE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    source:  mouseAreaIconDelete.containsMouse ?
                                 "../../images/remove_file_hover.png" :
                                 "../../images/remove_file.png"
                    MouseArea {
                        id: mouseAreaIconDelete
                        anchors.fill: parent
                        hoverEnabled : true
                        onClicked: {
                            console.log("Delete file index:" + index);
                            filesModel.remove(index)
                        }
                    }
                }
            }
        }
    }
    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged count:"
                        + propertyListViewFiles.count)
            if(filesModel.count === 0) {
                fileLoaded = false
                propertyPDFPreview.propertyBackground.source = ""
                propertyPDFPreview.propertyDragSigImg.visible = false
                propertyPDFPreview.propertyDragSigReasonText.visible = false
                propertyPDFPreview.propertyDragSigSignedByText.visible = false
                propertyPDFPreview.propertyDragSigSignedByNameText.visible = false
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
                propertyPDFPreview.propertyDragSigDateText.visible = false
                propertyPDFPreview.propertyDragSigLocationText.visible = false
                propertyPDFPreview.propertyDragSigImg.visible = false
            }
            else {
                fileLoaded = true
                propertyBusyIndicator.running = true
                if(propertyRadioButtonPADES.checked){
                    var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
                    propertySpinBoxControl.value = 1
                    propertySpinBoxControl.to = getMinimumPage()
                    propertyPDFPreview.propertyBackground.cache = false
                    propertyPDFPreview.propertyBackground.source = "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=1"
                    propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                    propertyPDFPreview.propertyDragSigImg.visible = true
                    propertyPDFPreview.propertyDragSigWaterImg.source = "qrc:/images/pteid_signature_watermark.jpg"
                    propertyPDFPreview.propertyDragSigWaterImg.visible = true
                    propertyPDFPreview.propertyDragSigReasonText.visible = true
                    propertyPDFPreview.propertyDragSigSignedByText.visible = true
                    propertyPDFPreview.propertyDragSigSignedByNameText.visible = true
                    propertyPDFPreview.propertyDragSigNumIdText.visible = true
                    propertyPDFPreview.propertyDragSigDateText.visible = true
                    propertyPDFPreview.propertyDragSigLocationText.visible = true
                    propertyPDFPreview.propertyDragSigImg.visible = true
                }

                propertyBusyIndicator.running = false
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value

            propertySpinBoxControl.up.indicator.visible = true
            propertySpinBoxControl.down.indicator.visible = true
            if(propertySpinBoxControl.value === getMinimumPage()){
                propertySpinBoxControl.up.indicator.visible = false
            }
            if (propertySpinBoxControl.value === 1){
                propertySpinBoxControl.down.indicator.visible = false
            }
        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            if(propertyCheckLastPage.checked){
                propertySpinBoxControl.value = getMinimumPage()
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + getMinimumPage()
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

        console.log("Page Services Sign Advanced mainWindowCompleted")
        propertyBusyIndicator.running = true
        propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                "Arraste para esta zona o ficheiro a assinar \nou\n clique para procurar o ficheiro"
        propertyPDFPreview.propertyDragSigSignedByNameText.text = "Assinado por:"
        propertyPDFPreview.propertyDragSigNumIdText.text = "Num. de Identificação Civil:"
        gapi.startCardReading()
    }

    function forceScrollandFocus() {
        // Force scroll and focus to the last item added
        propertyListViewFiles.positionViewAtEnd()
        propertyListViewFiles.forceActiveFocus()
        propertyListViewFiles.currentIndex = propertyListViewFiles.count -1
        if(propertyFilesListViewScroll.position > 0)
            propertyFilesListViewScroll.active = true
    }
    function getColorItem(mouseItem, mouseImage){

        var handColor
        if(mouseItem || mouseImage){
            handColor = Constants.COLOR_MAIN_DARK_GRAY
        }else{
            handColor = Constants.COLOR_MAIN_SOFT_GRAY
        }
        return handColor
    }
    function getMinimumPage(){
        // Function to detect minimum number of pages of all loaded pdfs to sign
        var minimumPage = 0
        for(var i = 0 ; i < propertyListViewFiles.count ; i++ ){
            var loadedFilePath = propertyListViewFiles.model.get(i).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            if(minimumPage == 0 || pageCount < minimumPage)
                minimumPage = pageCount

            console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount
                        + "minimum: " + minimumPage)
        }
        return minimumPage
    }
}
