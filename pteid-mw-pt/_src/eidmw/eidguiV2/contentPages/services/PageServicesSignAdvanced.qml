import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignAdvancedForm {

    property bool isAnimationFinished: mainFormID.propertyPageLoader.propertyAnimationExtendedFinished
    property string propertyOutputSignedFile : ""

    ToolTip {
        id: controlToolTip
        timeout: Constants.TOOLTIP_TIMEOUT_MS
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
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalAttributesLoaded:{
            console.log("Sign advanced - Signal SCAP attributes loaded")

            for(var i = 0; i < attribute_list.length; i=i+3)
            {
                // Fix the differences between entities and companies attributes about uppercase style
                attribute_list[i+1] = toTitleCase(attribute_list[i+1])
                entityAttributesModel.append({
                                                 entityName: attribute_list[i],
                                                 citizenName: attribute_list[i+1],
                                                 attribute: attribute_list[i+2],
                                                 checkBoxAttr: false
                                             });
            }

            for (var i = 0; i < propertyPageLoader.attributeListBackup.length; i++){
                console.log(propertyPageLoader.attributeListBackup[i])
                entityAttributesModel.get(propertyPageLoader.attributeListBackup[i]).checkBoxAttr = true
            }

            propertyBusyIndicator.running = false
        }
        onSignalPdfSignSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            signsuccess_dialog.visible = true
            propertyBusyIndicator.running = false
            // test time stamp
            if(error_code == GAPI.SignMessageTimestampFailed){
                if(propertyListViewFiles.count > 1){
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_MULTI_FAILED")
                }else{
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_FAILED")
                }
            }else{ // Sign with time stamp succefull
                signsuccess_dialog.propertySignSuccessDialogText.text = ""
            }
        }
        onSignalPdfSignFail: {
            console.log("Sign failed with error code: " + error_code)

            if (error_code == GAPI.SignFilePermissionFailed) {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_FILE_PERMISSION_FAIL")
            } else if (error_code == GAPI.PDFFileUnsupported) {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILE_UNSUPPORTED")
            } else {
                signerror_dialog.propertySignFailDialogText.text = qsTranslate("PageServicesSign","STR_SIGN_GENERIC_ERROR") + " " + error_code
            }
            
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalSCAPServiceTimeout: {
            gapi.startPingSCAP()
        }
        onSignalSCAPServiceFail: {
            console.log("Sign advanced - Signal SCAP Service Fail")
            if(pdfsignresult == GAPI.ScapAttributesExpiredError ||
                    pdfsignresult == GAPI.ScapZeroAttributesError ||
                    pdfsignresult == GAPI.ScapNotValidAttributesError){
                console.log("ScapAttributesExpiredError")
                signerror_dialog.propertySignFailDialogText.text =
                        qsTranslate("PageServicesSign","STR_SCAP_NOT_VALID_ATTRIBUTES")
            } else {
                console.log("ScapGenericError")
                gapi.startPingSCAP()
            }
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalSCAPPingSuccess: {
            signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("PageServicesSign","STR_SIGN_SCAP_SERVICE_FAIL")
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalSCAPPingFail: {
            console.log("Sign advanced - Signal SCAP ping fail")
            signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("PageServicesSign","STR_SCAP_PING_FAIL_FIRST")
                    + "\n\n"
                    + qsTranslate("PageServicesSign","STR_SCAP_PING_FAIL_SECOND")
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            propertyOutputSignedFile = ""
        }
        onSignalCardAccessError: {
            console.log("Sign Advanced Page onSignalCardAccessError")
            if(cardLoaded){
                if (error_code == GAPI.NoReaderFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                }
                else if (error_code == GAPI.NoCardFound) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                }
                else if (error_code == GAPI.SodCardReadError) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                }
                else if(error_code == GAPI.PinBlocked) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (error_code == GAPI.CardUserPinCancel) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
                }
                else if (error_code == GAPI.CardPinTimeout) {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
                }
                else {
                    mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                            qsTranslate("Popup Card","STR_POPUP_ERROR")
                    mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                            qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                if(error_code != GAPI.CardUserPinCancel && error_code != GAPI.CardPinTimeout)
                    cardLoaded = false
            }
            propertyBusyIndicator.running = false
        }
        onSignalCardDataChanged: {
            console.log("Services Sign Advanced --> Data Changed")
            //console.trace();
            propertyPDFPreview.propertyDragSigSignedByNameText.text =
                    qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Givenname)
                    + " " +  gapi.getDataCardIdentifyValue(GAPI.Surname)

            propertyPDFPreview.propertyDragSigNumIdText.text = qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                    + gapi.getDataCardIdentifyValue(GAPI.Documentnum)
            propertyBusyIndicator.running = false
            cardLoaded = true
        }
        onSignalCardChanged: {
            console.log("Services Sign Advanced onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                propertyPDFPreview.propertyDragSigNumIdText.text =  qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
    }
    Connections {
        target: image_provider_pdf
        onSignalPdfSourceChanged: {
            console.log("Receive signal onSignalPdfSourceChanged pdfWidth = "+pdfWidth+" pdfHeight = "+pdfHeight);
            propertyPDFPreview.propertyPdfOriginalWidth=pdfWidth
            propertyPDFPreview.propertyPdfOriginalHeight=pdfHeight
            propertyPDFPreview.setSignPreview(propertyPageLoader.propertyBackupCoordX,propertyPageLoader.propertyBackupCoordY)
        }
    }

    Components.DialogCMD{
        id: dialogSignCMD
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
        property alias propertySignSuccessDialogText: labelText

        header: Label {
            text: {
                if(propertyListViewFiles.count > 1) {
                    qsTranslate("PageServicesSign", "STR_SIGN_SUCESS_MULTI")
                } else {
                    qsTranslate("PageServicesSign", "STR_SIGN_SUCESS")
                }
            }
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: signsuccess_dialog.availableWidth
            height: 50

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    signSuccessShowSignedFile()
                }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
            Item {
                id: rectLabelOpenText
                width: parent.width
                height: 50

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectLabelText.bottom
                Text {
                    id: labelOpenText
                    text: {
                        if(propertyListViewFiles.count > 1 || propertyRadioButtonXADES.checked) {
                            qsTranslate("PageServicesSign", "STR_SIGN_OPEN_MULTI")
                        } else {
                            qsTranslate("PageServicesSign", "STR_SIGN_OPEN")
                        }
                    }

                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                }
            }
        }
        Item {
            width: signsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signsuccess_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                }
                Button {
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signSuccessShowSignedFile()
                    }
                }
            }
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }
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

        property alias propertySignFailDialogText: text_sign_error

        header: Label {
            text: qsTranslate("PageServicesSign","STR_SIGN_FAIL")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            width: signerror_dialog.availableWidth
            height: 50
            Text {
                id: text_sign_error
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: parent.height
                width: parent.width - 48
                wrapMode: Text.Wrap
            }
        }
        standardButtons: DialogButtonBox.Ok
    }
    propertyMouseAreaToolTipPades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILES")
            controlToolTip.x = propertyMouseAreaToolTipPadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
        }
    }
    propertyMouseAreaToolTipXades{
        onEntered: {
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PACKAGE")
            controlToolTip.x = propertyMouseAreaToolTipXadesX - controlToolTip.width * 0.5
            controlToolTip.y = propertyMouseAreaToolTipY + 22
            controlToolTip.open()
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
            propertyPageLoader.propertyBackupCoordX = propertyPDFPreview.propertyDragSigRect.x
            propertyPageLoader.propertyBackupCoordY = propertyPDFPreview.propertyDragSigRect.y

            updateUploadedFiles(filesArray)
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

            updateUploadedFiles(filesArray)

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
            propertyBusyIndicator.running = true
            var loadedFilePath = propertyListViewFiles.model.get(0).fileUrl
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogOutput.fileUrl.toString()
            outputFile = decodeURIComponent(stripFilePrefix(outputFile))
            if (propertyRadioButtonPADES.checked) {
                var page = propertySpinBoxControl.value
                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked

                var coord_x = -1
                var coord_y = -1
                if(propertyCheckSignShow.checked){
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                console.log("Output filename: " + outputFile)
                console.log("Signing in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page + " timestamp: " + isTimestamp)

                propertyOutputSignedFile = outputFile;
                if(propertySwitchSignAdd.checked) {
                    //In the new SCAP implementation we use our PDFSignature class as a simple signature
                    //so we don't need to convert coordinates
                    //coord_x = gapi.getPageSize(page).width * coord_x
                    //coord_y = gapi.getPageSize(page).height * (1 - coord_y)

                    var attributeList = []
                    var count = 0
                    for (var i = 0; i < entityAttributesModel.count; i++){
                        if(entityAttributesModel.get(i).checkBoxAttr == true){
                            attributeList[count] = i
                            count++
                        }
                    }
                    console.log("QML AttributeList: ", attributeList)
                    gapi.startSigningSCAP(loadedFilePath, outputFile, page, coord_x, coord_y,
                                          location,reason,0, attributeList)
                } else {

                    gapi.startSigningPDF(loadedFilePath, outputFile, page, coord_x, coord_y,
                                         reason, location, isTimestamp, isSmallSignature)
                }
            }
            else {
                propertyOutputSignedFile = outputFile;
                propertyOutputSignedFile =
                        propertyOutputSignedFile.substring(0, propertyOutputSignedFile.lastIndexOf('/'))
                if (propertyListViewFiles.count == 1){
                    gapi.startSigningXADES(loadedFilePath, outputFile, isTimestamp)
                }else{
                    var batchFilesArray = []
                    for(var i = 0; i < propertyListViewFiles.count; i++){
                        batchFilesArray[i] =  propertyListViewFiles.model.get(i).fileUrl;
                    }
                    gapi.startSigningBatchXADES(batchFilesArray, outputFile, isTimestamp)
                }
            }
        }
    }
    propertyFileDialogCMDOutput {
        onAccepted: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            dialogSignCMD.open()
        }
    }
    propertyFileDialogBatchOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogBatchOutput.folder.toString()
            outputFile = decodeURIComponent(stripFilePrefix(outputFile))
            if (propertyRadioButtonPADES.checked) {

                if(propertyCheckLastPage.checked){
                    var page = 0 // Sign last page in all documents
                }else{
                    var page = propertySpinBoxControl.value
                }

                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked
                var coord_x = -1
                var coord_y = -1
                if(propertyCheckSignShow.checked){
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                console.log("Output filename: " + outputFile)
                console.log("Signing Batch in position coord_x: " + coord_x
                            + " and coord_y: "+coord_y + " page: " + page + " timestamp: " + isTimestamp)

                var batchFilesArray = []
                for(var i = 0; i < propertyListViewFiles.count; i++){
                    batchFilesArray[i] =  propertyListViewFiles.model.get(i).fileUrl;
                }

                // remove duplicate fileUrls
                batchFilesArray = batchFilesArray.filter(onlyUnique);

                propertyOutputSignedFile = outputFile;
                gapi.startSigningBatchPDF(batchFilesArray, outputFile, page, coord_x, coord_y,
                                          reason, location, isTimestamp, isSmallSignature)

            }
        }
    }

    propertyTextFieldReason{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
            propertyPageLoader.propertyBackupLocal = propertyTextFieldReason.text
        }
    }
    propertyTextFieldLocal{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text
            propertyPageLoader.propertyBackupReason = propertyTextFieldLocal.text
        }
    }

    propertyCheckSignReduced{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignReduced = propertyCheckSignReduced.checked
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
    ListModel {
        id: entityAttributesModel
        onCountChanged: {
            propertyTextAttributesMsg.visible = false
            propertyMouseAreaTextAttributesMsg.enabled = false
            propertyMouseAreaTextAttributesMsg.z = 0
        }
    }

    Component {
        id: attributeListDelegate
        Rectangle {
            id: container
            width: parent.width - Constants.SIZE_ROW_H_SPACE
            height: columnItem.height + 15
            color: Constants.COLOR_MAIN_SOFT_GRAY

            CheckBox {
                id: checkboxSel
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                anchors.verticalCenter: parent.verticalCenter
                checked: checkBoxAttr
                onCheckedChanged: {
                    entityAttributesModel.get(index).checkBoxAttr = checkboxSel.checked
                    var count = 0
                    for (var i = 0; i < entityAttributesModel.count; i++){
                        if(entityAttributesModel.get(i).checkBoxAttr == true){
                            propertyPageLoader.attributeListBackup[count] = i
                            count++
                        }
                    }
                }
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    text: "<b>" + citizenName + " </b> - " + entityName + " - " + attribute
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
            }

        }
    }

    propertyMouseAreaTextAttributesMsg{
        onClicked: {
			propertyPageLoader.propertyBackupFromSignaturePage = true
            // Jump to Menu Definitions - PageDefinitionsSCAP
            mainFormID.state = "STATE_NORMAL"
            mainFormID.propertySubMenuListView.model.clear()
            for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
                console.log("Sub Menu indice " + i + " - "
                            + mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName);
                mainFormID.propertySubMenuListView.model
                .append({
                            "subName": qsTranslate("MainMenuBottomModel",
                                                   mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName),
                            "expand": mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i)
                            .expand,
                            "url": mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i)
                            .url
                        })
            }
            mainFormID.propertyMainMenuListView.currentIndex = -1
            mainFormID.propertyMainMenuBottomListView.currentIndex = 0
            mainFormID.propertySubMenuListView.currentIndex = 1
            mainFormID.propertyPageLoader.source = "/contentPages/definitions/PageDefinitionsSCAP.qml"
        }
    }

    propertySwitchSignTemp{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupTempSign = propertySwitchSignTemp.checked
        }
    }

    propertyCheckSignShow{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignShow = propertyCheckSignShow.checked
            if(propertyCheckSignShow.checked){
                propertyCheckSignReduced.enabled = true
                propertyCheckLastPage.enabled = true
                propertySpinBoxControl.enabled = true
                propertySpinBoxControl.up.indicator.enabled = true
                propertySpinBoxControl.down.indicator.enabled = true
                propertySpinBoxControl.up.indicator.opacity = 1
                propertySpinBoxControl.down.indicator.opacity = 1
                propertyTextSpinBox.visible = true
                propertyPageText.opacity = 1
            }else{
                propertyCheckSignReduced.enabled = false
                propertyCheckLastPage.enabled = false
                propertySpinBoxControl.enabled = false
                propertySpinBoxControl.up.indicator.enabled = false
                propertySpinBoxControl.down.indicator.enabled = false
                propertySpinBoxControl.up.indicator.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
                propertySpinBoxControl.down.indicator.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
                propertyTextSpinBox.visible = false
                propertyPageText.opacity = Constants.OPACITY_SIGNATURE_TEXT_DISABLED
            }
        }
    }

    propertySwitchSignAdd {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignAdd = propertySwitchSignAdd.checked
            if(propertySwitchSignAdd.checked){
                console.log("propertySwitchSignAdd checked")
                propertyBusyIndicator.running = true
                propertySwitchSignTemp.checked = false
                propertySwitchSignTemp.enabled = false
                propertyCheckSignShow.checked = true
                propertyCheckSignShow.enabled = false
                propertyCheckSignReduced.checked = false
                propertyCheckSignReduced.enabled = false
                propertyRadioButtonXADES.enabled = false
                propertyTextAttributesMsg.visible = true
                propertyMouseAreaTextAttributesMsg.enabled = true
                propertyMouseAreaTextAttributesMsg.z = 1
                // Load attributes from cache (all, isShortDescription)
                gapi.startLoadingAttributesFromCache(2, 1)
            }else{
                console.log("propertySwitchSignAdd not checked")
                entityAttributesModel.clear()
                propertySwitchSignTemp.enabled = true
                propertyCheckSignReduced.enabled = true
                propertyCheckSignShow.enabled = true
                propertyRadioButtonXADES.enabled = true
                propertyTextAttributesMsg.visible = false
                propertyMouseAreaTextAttributesMsg.enabled = false
                propertyMouseAreaTextAttributesMsg.z = 0
            }
        }
    }

    propertyFileDialog {

        onAccepted: {
            console.log("You chose file(s): " + propertyFileDialog.fileUrls)
            console.log("Num files: " + propertyFileDialog.fileUrls.length)

            updateUploadedFiles(propertyFileDialog.fileUrls)

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
            filesModel.clear()
            propertyPageLoader.propertyBackupfilesModel.clear()
        }
    }

    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            if (gapi.getTriesLeftSignPin() === 0) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup PIN","STR_POPUP_ERROR")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SIGN_BLOCKED")
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
            }else{
                if (propertyListViewFiles.count == 1){
                    propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
                    if (propertyRadioButtonPADES.checked) {
                        if(propertySwitchSignAdd.checked){
                            var count = 0
                            //15 MB filesize limit for SCAP
                            var MAX_SIZE = 15 * 1024 *1024
                            var outputNativePath = stripFilePrefix(propertyListViewFiles.model.get(0).fileUrl)
                            for (var i = 0; i < entityAttributesModel.count; i++){
                                if(entityAttributesModel.get(i).checkBoxAttr == true){
                                    count++
                                }
                            }
                            if(count == 0) {
                                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                        qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                        qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                            }
                            else if (gapi.getFileSize(outputNativePath) > MAX_SIZE) {
                                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                        qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                        qsTranslate("PageServicesSign","STR_SCAP_MAX_FILESIZE") + " 15 MB"
                                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                            }
                            else {
                                var outputFile = propertyListViewFiles.model.get(0).fileUrl
                                //Check if filename has extension and remove it.
                                if( outputFile.lastIndexOf('.') > 0)
                                    var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                                propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                                propertyFileDialogOutput.open()
                            }
                        }else{
                            var outputFile =  propertyListViewFiles.model.get(0).fileUrl
                            //Check if filename has extension and remove it.
                            if( outputFile.lastIndexOf('.') > 0)
                                var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                            propertyFileDialogOutput.filename = outputFile + "_signed.pdf"
                            propertyFileDialogOutput.open()
                        }
                    }
                    else {
                        var outputFile = propertyListViewFiles.model.get(0).fileUrl
                        //Check if filename has extension and remove it.
                        if( outputFile.lastIndexOf('.') > 0)
                            var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))

                        propertyFileDialogOutput.filename = outputFile + "_xadessign.ccsigned"
                        propertyFileDialogOutput.open()
                    }
                }else{
                    if (propertySwitchSignAdd.checked){
                        mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                        mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                qsTranslate("PageServicesSign","STR_MULTI_FILE_ATTRIBUTES_WARNING_MSG")
                        mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
                    }else{
                        var outputFile = propertyListViewFiles.model.get(propertyListViewFiles.count-1).fileUrl
                        //Check if filename has extension and remove it.
                        if( outputFile.lastIndexOf('.') > 0)
                            var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                        //Check if filename has file name and remove it.
                        if( outputFile.lastIndexOf('/') > 0)
                            var outputFile = outputFile.substring(0, outputFile.lastIndexOf('/'))

                        if(propertyRadioButtonPADES.checked){
                            propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT_FOLDER")
                            propertyFileDialogBatchOutput.open()
                        }else{

                            propertyFileDialogOutput.filename = outputFile + "/" + "xadessign.ccsigned"
                            propertyFileDialogOutput.open()
                        }
                    }
                }
            }
        }
    }
    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD" )
            if (propertyListViewFiles.count == 1){
                var outputFile =  filesModel.get(0).fileUrl
                //Check if filename has extension and remove it.
                if( outputFile.lastIndexOf('.') > 0)
                    var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf"
                propertyFileDialogCMDOutput.open()
            }else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("PageServicesSign","STR_MULTI_FILE_CMD_WARNING")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("PageServicesSign","STR_MULTI_FILE_CMD_WARNING_MSG")
                mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            }
        }
    }

    propertyTextSpinBox{
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    propertyFileDialog{

        nameFilters: propertyRadioButtonPADES.checked ?
                         [ "PDF document (*.pdf)", "All files (*)" ] :
                         [ "All files (*)" ]
    }

    propertyRadioButtonPADES{
        onCheckedChanged:{
            propertyPageLoader.propertyBackupFormatPades = propertyRadioButtonPADES.checked
            if(propertyRadioButtonPADES.checked){
                propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                        qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
                propertyTextFieldReason.enabled = true
                propertyTextFieldLocal.enabled = true
                propertyTextFieldReason.opacity = 1.0
                propertyTextFieldLocal.opacity = 1.0
                propertySwitchSignAdd.enabled = true
                propertyCheckSignShow.enabled = true
                propertyCheckSignReduced.enabled = true
                propertyButtonSignCMD.enabled = true
                filesModel.clear()
                for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
                {
                    filesModel.append({
                                          "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                      })
                }
            }else{
                propertyTextDragMsgImg.text =
                        qsTranslate("PageServicesSign","STR_SIGN_NOT_PREVIEW")
                propertyTextFieldReason.enabled = false
                propertyTextFieldLocal.enabled = false
                propertyTextFieldReason.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertyTextFieldLocal.opacity = Constants.OPACITY_SERVICES_SIGN_ADVANCE_TEXT_DISABLED
                propertySwitchSignAdd.enabled = false

                propertyCheckSignShow.enabled = false
                propertyCheckSignReduced.enabled = false
                propertyButtonSignCMD.enabled = false
                filesModel.clear()
                for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
                {
                    filesModel.append({
                                          "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                      })
                }
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
                            for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
                            {
                                if(propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                        === filesModel.get(index).fileUrl)
                                    propertyPageLoader.propertyBackupfilesModel.remove(index)
                            }
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
            console.log("propertyListViewFiles onCountChanged count:"
                        + propertyListViewFiles.count)
            if(filesModel.count === 0) {
                fileLoaded = false
                propertyTextDragMsgImg.visible = true
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
                if(propertyRadioButtonPADES.checked){
                    propertyTextDragMsgImg.visible = false
                    // Preview the last file selected
                    var loadedFilePath = filesModel.get(filesModel.count-1).fileUrl
                    var pageCount = gapi.getPDFpageCount(loadedFilePath)
                    if(pageCount > 0){
                        propertyTextSpinBox.maximumLength = maxTextInputLength(pageCount)
                        propertyBusyIndicator.running = true
                        if(propertyCheckLastPage.checked==true
                                || propertySpinBoxControl.value > pageCount)
                            propertySpinBoxControl.value = pageCount
                        if(propertySpinBoxControl.value > getMinimumPage())
                            propertySpinBoxControl.value = getMinimumPage()
                        updateIndicators(pageCount)
                        propertyPDFPreview.propertyBackground.cache = false
                        propertyPDFPreview.propertyBackground.source =
                                "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
                        var urlCustomImage = gapi.getCachePath()+"/CustomSignPicture.png"
                        if(gapi.getUseCustomSignature() && gapi.customSignImageExist()){
                            if (Qt.platform.os === "windows") {
                                urlCustomImage = "file:///"+urlCustomImage
                            }else{
                                urlCustomImage = "file://"+urlCustomImage
                            }
                            propertyPDFPreview.propertyDragSigImg.source = urlCustomImage
                        }else{
                            propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC.png"
                        }
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
                        propertyBusyIndicator.running = false
                    }else{
                        filesModel.remove(propertyListViewFiles.count-1)
                        mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                                qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                        if(pageCount === -1){
                            console.log("Error loading pdf file")
                            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                    qsTranslate("PageServicesSign","STR_LOAD_ADVANCED_PDF_ERROR_MSG")
                        }else if(pageCount === -2){
                            console.log("Error loading pdf encrypted file")
                            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                    qsTranslate("PageServicesSign","STR_LOAD_ENCRYPTED_PDF_ERROR_MSG")
                        }else{
                            console.log("Generic Error loading pdf file")
                            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                                    qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")
                        }
                        mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
                        mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
                    }
                }else{
                    propertyTextDragMsgImg.visible = true
                }
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            var loadedFilePath = filesModel.get(filesModel.count-1).fileUrl
            var maxPageAllowed = getMinimumPage()
            if(propertySpinBoxControl.value > maxPageAllowed){
                propertySpinBoxControl.value = 1
            }
            propertyPDFPreview.propertyBackground.source =
                    "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            propertyPageLoader.propertyBackupPage =  propertySpinBoxControl.value           
            updateIndicators(maxPageAllowed)
        }
    }
    propertyCheckLastPage {
        onCheckedChanged: {
            var loadedFilePath = propertyListViewFiles.model.get(filesModel.count-1).fileUrl
            propertyPageLoader.propertyBackupLastPage =  propertyCheckLastPage.checked
            var pageCount = gapi.getPDFpageCount(loadedFilePath)

            if(propertyCheckLastPage.checked){
                propertySpinBoxControl.value = pageCount
                propertySpinBoxControl.enabled = false
                propertyPageText.enabled = false
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + pageCount
            }else{
                propertySpinBoxControl.enabled = true
                propertyPageText.enabled = true
                propertyTextSpinBox.visible = true
                if(propertySpinBoxControl.value > getMinimumPage())
                    propertySpinBoxControl.value = getMinimumPage()
                propertyPDFPreview.propertyBackground.source =
                        "image://pdfpreview_imageprovider/"+loadedFilePath + "?page=" + propertySpinBoxControl.value
            }
        }
    }

    onIsAnimationFinishedChanged: {
        if(isAnimationFinished == true){
            loadUnfinishedSignature()
        }
    }

    Component.onCompleted: {
        console.log("Page Services Sign Advanced mainWindowCompleted")
        propertyPageLoader.propertyBackupFromSignaturePage = false
        propertyBusyIndicator.running = true
        gapi.startCardReading()
    }

    function loadUnfinishedSignature() {
        // Load backup data about unfinished advanced signature
        propertyRadioButtonPADES.checked = propertyPageLoader.propertyBackupFormatPades
        propertyRadioButtonXADES.checked = !propertyPageLoader.propertyBackupFormatPades
        propertySwitchSignTemp.checked = propertyPageLoader.propertyBackupTempSign
        propertySwitchSignAdd.checked = propertyPageLoader.propertyBackupSignAdd
        propertyCheckSignShow.checked = propertyPageLoader.propertyBackupSignShow
        propertyCheckSignReduced.checked = propertyPageLoader.propertyBackupSignReduced

        filesModel.clear()
        for(var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++)
        {
            filesModel.append({
                                  "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                              })
        }

        propertySpinBoxControl.value = propertyPageLoader.propertyBackupPage
        propertyCheckLastPage.checked = propertyPageLoader.propertyBackupLastPage
        propertyTextFieldReason.text = propertyPageLoader.propertyBackupLocal
        propertyTextFieldLocal.text = propertyPageLoader.propertyBackupReason

        if (gapi.getShortcutFlag() > 0){

            var newFileUrl = {
                "fileUrl": gapi.getShortcutInputPDF()
            };

            if (!containsFile(newFileUrl, filesModel)){
                filesModel.append(newFileUrl)
            }
        }
        propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
    }

    function stripFilePrefix(filePath) {
        if (Qt.platform.os === "windows") {
            return filePath.replace(/^(file:\/{3})|(file:)|(qrc:\/{3})|(http:\/{3})/,"")
        }
        else {
            return filePath.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,"");
        }
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
        for(var i = 0 ; i < filesModel.count ; i++ ){
            var loadedFilePath = filesModel.get(i).fileUrl
            var pageCount = gapi.getPDFpageCount(loadedFilePath)
            if(minimumPage == 0 || pageCount < minimumPage)
                minimumPage = pageCount

            console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount
                        + "minimum: " + minimumPage)
        }
        return minimumPage
    }

    function updateIndicators(pageCount){
        propertySpinBoxControl.up.indicator.visible = true
        propertySpinBoxControl.down.indicator.visible = true
        propertySpinBoxControl.up.indicator.enabled = true
        propertySpinBoxControl.down.indicator.enabled = true

        if(propertySpinBoxControl.value === pageCount
                || propertySpinBoxControl.value === getMinimumPage()){
            propertySpinBoxControl.up.indicator.visible = false
            propertySpinBoxControl.up.indicator.enabled = false
        }
        if (propertySpinBoxControl.value === 1){
            propertySpinBoxControl.down.indicator.visible = false
            propertySpinBoxControl.down.indicator.enabled = false
        }
    }
	function openSignedFile(){
        if (Qt.platform.os === "windows") {
			if (propertyOutputSignedFile.substring(0, 2) == "//" ){
				propertyOutputSignedFile = "file:" + propertyOutputSignedFile
			}else{
				propertyOutputSignedFile = "file:///" + propertyOutputSignedFile
			}
        }else{
            propertyOutputSignedFile = "file://" + propertyOutputSignedFile
        }
        console.log("Open Url Externally: " + propertyOutputSignedFile)
        Qt.openUrlExternally(propertyOutputSignedFile)
    }
    function signSuccessShowSignedFile(){
        openSignedFile()
        signsuccess_dialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
    }
    function toTitleCase(str) {
        return str.replace(/\w\S*/g, function(txt){
            return txt.charAt(0).toUpperCase() + txt.substr(1).toLowerCase();
        });
    }
    function onlyUnique(value, index, self) {
        return self.indexOf(value) === index;
    }
    //should only used by a fileModel list because it uses .count instead of .length
    function containsFile(obj, list) {
        var i;
        for (i = 0; i < list.count; i++) {
            if (list.get(i).fileUrl.toString() === obj.fileUrl) {
                console.log("File already uploaded");
                return true;
            }
        }
        return false;
    }
    function updateUploadedFiles(fileList){
        var fileAlreadyUploaded = false
        for(var i = 0; i < fileList.length; i++){
            var path = fileList[i];
            console.log("Adding file: " + path)

            path = decodeURIComponent(stripFilePrefix(path))

            var newFileUrl = {
                "fileUrl": path
            };

            if (!containsFile(newFileUrl, filesModel)){
                filesModel.append(newFileUrl)
                propertyPageLoader.propertyBackupfilesModel.append(newFileUrl)
            } else {
                fileAlreadyUploaded = true
            }
        }

        if (fileAlreadyUploaded){
            mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                    qsTranslate("PageServicesSign","STR_FILE_UPLOAD_FAIL")
            mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                    qsTranslate("PageServicesSign","STR_FILE_ALREADY_UPLOADED")
            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus()
        }
    }
    function maxTextInputLength(num){
        //given number of pages returns maximum length that TextInput should accept
        return Math.ceil(Math.log(num + 1) / Math.LN10);
    }
}
