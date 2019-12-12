/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
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
    Keys.onRightPressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jumpToDefinitionsSCAP()
    }
    Keys.onSpacePressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jumpToDefinitionsSCAP()
    }
    Keys.onReturnPressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jumpToDefinitionsSCAP()
    }

    Keys.onPressed: {
        console.log("PageServicesSignAdvancedForm onPressed:" + event.key)

        if(propertyListViewEntities.focus === false){
            Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
        } else {
            propertyPDFPreview.forceActiveFocus()
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
                entityAttributesModel.get(i).checkBoxAttr = propertyPageLoader.attributeListBackup[i]
            }

            propertyBusyIndicator.running = false
            propertyListViewEntities.currentIndex = 0
            entityAttributesModel.count > 0
                    ? propertyListViewEntities.forceActiveFocus()
                    : propertyTextAttributesMsg.forceActiveFocus()
        }
        onSignalPdfSignSucess: {
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
            signsuccess_dialog.open()
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
            dialogSignCMD.close()
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
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                var bodyPopup = ""
                if (error_code == GAPI.NoReaderFound) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD_READER")
                }
                else if (error_code == GAPI.NoCardFound) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CARD")
                }
                else if (error_code == GAPI.SodCardReadError) {
                    bodyPopup = qsTranslate("Popup Card","STR_SOD_VALIDATION_ERROR")
                }
                else if(error_code == GAPI.PinBlocked) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_PIN_BLOCKED")
                }
                else if (error_code == GAPI.CardUserPinCancel) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
                }
                else if (error_code == GAPI.CardPinTimeout) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
                }
                else {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                if(error_code != GAPI.CardUserPinCancel && error_code != GAPI.CardPinTimeout)
                    cardLoaded = false
            }
            propertyBusyIndicator.running = false
            propertyButtonAdd.forceActiveFocus()
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
            propertyButtonAdd.forceActiveFocus()
        }
        onSignalCardChanged: {
            console.log("Services Sign Advanced onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + ": "
                propertyPDFPreview.propertyDragSigNumIdText.text =  qsTranslate("GAPI","STR_DOCUMENT_NUMBER") + ": "
                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                cardLoaded = true
                gapi.startCardReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

        }
    }
    Connections {
        target: image_provider_pdf
        onSignalPdfSourceChanged: {
            console.log("Receive signal onSignalPdfSourceChanged pdfWidth = "+pdfWidth+" pdfHeight = "+pdfHeight);
            propertyPDFPreview.propertyPdfOriginalWidth=pdfWidth
            propertyPDFPreview.propertyPdfOriginalHeight=pdfHeight
            propertyPDFPreview.updateSignPreview()
            propertyPDFPreview.setSignPreview(propertyPageLoader.propertyBackupCoordX * propertyPDFPreview.propertyBackground.width,propertyPageLoader.propertyBackupCoordY * propertyPDFPreview.propertyBackground.height)
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
            id: titleText
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
            font.bold: rectPopUp.activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            id: rectPopUp
            width: signsuccess_dialog.availableWidth
            height: 50

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return
                        || event.key===Qt.Key_Space)
                {
                    signSuccessShowSignedFile()
                }
            }
            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + titleText.text + labelText.text + labelOpenText.text
            KeyNavigation.tab: closeButton
            KeyNavigation.down: closeButton
            KeyNavigation.right: closeButton
            KeyNavigation.backtab: openFileButton
            KeyNavigation.up: openFileButton

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    font.bold: activeFocus
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
                    font.bold: activeFocus
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
                    id: closeButton
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
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: openFileButton
                    KeyNavigation.down: openFileButton
                    KeyNavigation.right: openFileButton
                    KeyNavigation.backtab: rectPopUp
                    KeyNavigation.up: rectPopUp
                    highlighted: activeFocus
                }
                Button {
                    id: openFileButton
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
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectPopUp
                    KeyNavigation.down: rectPopUp
                    KeyNavigation.right: rectPopUp
                    KeyNavigation.backtab: closeButton
                    KeyNavigation.up: closeButton
                    highlighted: activeFocus
                }
            }
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
        onClosed: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
        onOpened: {
            rectPopUp.forceActiveFocus()
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
            id: titleTextError
            text: qsTranslate("PageServicesSign","STR_SIGN_FAIL")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: rectPopUpError.activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            id: rectPopUpError
            width: signerror_dialog.availableWidth
            height: 50
            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + titleTextError.text + text_sign_error.text
            KeyNavigation.tab: closeButtonError
            KeyNavigation.right: closeButtonError
            KeyNavigation.down: closeButtonError
            KeyNavigation.backtab: closeButtonError
            KeyNavigation.up: closeButtonError
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
        Item {
            width: signerror_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: closeButtonError
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: "OK"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signerror_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectPopUpError
                    KeyNavigation.down: rectPopUpError
                    KeyNavigation.right: rectPopUpError
                    KeyNavigation.backtab: rectPopUpError
                    KeyNavigation.up: rectPopUpError
                    highlighted: activeFocus
                }
            }
        }
        onOpened: {
            rectPopUpError.forceActiveFocus()
        }
        onRejected:{
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
        onClosed: {
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
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
            /*console.log("Signature advanced ! You chose file(s): " + drag.urls);*/
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            // Update sign preview position variables to be used to send to sdk
            propertyPDFPreview.updateSignPreview()
            propertyPageLoader.propertyBackupCoordX = propertyPDFPreview.propertyDragSigRect.x / propertyPDFPreview.propertyBackground.width
            propertyPageLoader.propertyBackupCoordY = propertyPDFPreview.propertyDragSigRect.y / propertyPDFPreview.propertyBackground.height

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
            /*console.log("Signature advanced Drop File Area! You chose file(s): " + drag.urls);*/
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
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
            outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))
            if (propertyRadioButtonPADES.checked) {
                var page = propertySpinBoxControl.value
                var reason = propertyTextFieldReason.text
                var location = propertyTextFieldLocal.text
                var isSmallSignature = propertyCheckSignReduced.checked

                var coord_x = -1
                var coord_y = -1
                if(propertyCheckSignShow.checked){
                    propertyPDFPreview.updateSignPreview()
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                /*console.log("Output filename: " + outputFile)*/
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
            dialogSignCMD.open()
        }
    }
    propertyFileDialogBatchCMDOutput {
        onAccepted: {
            dialogSignCMD.open()
        }
    }
    propertyFileDialogBatchOutput {
        onAccepted: {
            propertyBusyIndicator.running = true
            var isTimestamp = propertySwitchSignTemp.checked
            var outputFile = propertyFileDialogBatchOutput.folder.toString()
            outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))
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
                    propertyPDFPreview.updateSignPreview()
                    coord_x = propertyPDFPreview.propertyCoordX
                    //coord_y must be the lower left corner of the signature rectangle
                    coord_y = propertyPDFPreview.propertyCoordY
                }

                /*console.log("Output filename: " + outputFile)*/
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
    propertyListViewEntities{
        onFocusChanged: {
            if(propertyListViewEntities.focus)propertyListViewEntities.currentIndex = 0
        }
    }

    Component {
        id: attributeListDelegate
        Rectangle {
            id: container
            width: parent.width - Constants.SIZE_ROW_H_SPACE
            height: columnItem.height + 15
            Keys.onSpacePressed: {
                checkboxSel.focus = true
            }
            Keys.onTabPressed: {
                checkboxSel.focus = true
                if(propertyListViewEntities.currentIndex == propertyListViewEntities.count -1){
                    propertyPDFPreview.forceActiveFocus()
                }else{
                    propertyListViewEntities.currentIndex++
                }
            }

            color:  propertyListViewEntities.currentIndex === index && propertyListViewEntities.focus
                    ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY

            Accessible.role: Accessible.CheckBox
            Accessible.name: Functions.filterText(entityText.text)

            CheckBox {
                id: checkboxSel
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                anchors.verticalCenter: parent.verticalCenter
                checked: checkBoxAttr
                onCheckedChanged: {
                    entityAttributesModel.get(index).checkBoxAttr = checkboxSel.checked
                    propertyPageLoader.attributeListBackup[index] = checkboxSel.checked
                }
                onFocusChanged: {
                    if(focus) propertyListViewEntities.currentIndex = index
                }
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width
                anchors.verticalCenter: parent.verticalCenter
                Text {
                    id: entityText
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
            jumpToDefinitionsSCAP()
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
                // Load attributes from cache (all, LongDescription)
                gapi.startLoadingAttributesFromCache(GAPI.ScapAttrAll,
                                                     GAPI.ScapAttrDescriptionLong)
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
                propertyPDFPreview.forceActiveFocus()
            }
        }
    }

    propertyFileDialog {

        onAccepted: {
            /*console.log("You chose file(s): " + propertyFileDialog.fileUrls)*/
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
            gapi.closeAllPdfPreviews();
        }
    }

    propertyButtonSignWithCC {
        onClicked: {
            console.log("Sign with CC")
            if (gapi.doGetTriesLeftSignPin() === 0) {
                var titlePopup = qsTranslate("Popup PIN","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("Popup PIN","STR_POPUP_CARD_PIN_SIGN_BLOCKED")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }else{
                if (propertyListViewFiles.count == 1){
                    propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
                    if (propertyRadioButtonPADES.checked) {
                        if(propertySwitchSignAdd.checked){
                            if(numberOfAttributesSelected() == 0) {
                                var titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                                var bodyPopup = qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
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

                        propertyFileDialogOutput.filename = outputFile + "_xadessign.asics"
                        propertyFileDialogOutput.open()
                    }
                }else{
                    if (propertySwitchSignAdd.checked){
                        var titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                        var bodyPopup = qsTranslate("PageServicesSign","STR_MULTI_FILE_ATTRIBUTES_WARNING_MSG")
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
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

                            propertyFileDialogOutput.filename = outputFile + "/" + "xadessign.asice"
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
            if (!gapi.checkCMDSupport()) {
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CMD_SUPPORT")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
            else if( propertySwitchSignAdd.checked && numberOfAttributesSelected() == 0) {
                var titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                var bodyPopup = qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                return;
            }
            else if (propertySwitchSignAdd.checked && propertyListViewFiles.count > 1){
                var titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                var bodyPopup = qsTranslate("PageServicesSign","STR_MULTI_FILE_ATTRIBUTES_WARNING_MSG")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
            else {
                if (propertyListViewFiles.count == 1){
                    var outputFile =  filesModel.get(0).fileUrl
                    //Check if filename has extension and remove it.
                    if( outputFile.lastIndexOf('.') > 0)
                        var outputFile = outputFile.substring(0, outputFile.lastIndexOf('.'))
                    propertyFileDialogCMDOutput.filename = outputFile + "_signed.pdf"
                    propertyFileDialogCMDOutput.open()
                }else{
                    propertyFileDialogBatchCMDOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT_FOLDER")
                    propertyFileDialogBatchCMDOutput.open()
                }
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
                    font.family: lato.name
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
                            gapi.closePdfPreview(filesModel.get(index).fileUrl);
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
                propertyButtonAdd.forceActiveFocus()
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
                        var titlePopup = qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                        var bodyPopup = ""
                        if(pageCount === -1){
                            console.log("Error loading pdf file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_ADVANCED_PDF_ERROR_MSG")
                        }else if(pageCount === -2){
                            console.log("Error loading pdf encrypted file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_ENCRYPTED_PDF_ERROR_MSG")
                        }else{
                            console.log("Generic Error loading pdf file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")
                        }
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                    }
                }else{
                    propertyTextDragMsgImg.visible = true
                }
                propertyTitleConf.forceActiveFocus()
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
    Component.onDestruction: {
        console.log("PageServicesSignAdvanced destruction")
        if(gapi) gapi.closeAllPdfPreviews();
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
            if(gapi.fileExists(propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl)){
                filesModel.append({
                                      "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl
                                  })
            }
        }

        propertySpinBoxControl.value = propertyPageLoader.propertyBackupPage
        propertyCheckLastPage.checked = propertyPageLoader.propertyBackupLastPage
        propertyTextFieldReason.text = propertyPageLoader.propertyBackupLocal
        propertyTextFieldLocal.text = propertyPageLoader.propertyBackupReason
        propertyPDFPreview.setSignPreview(propertyPageLoader.propertyBackupCoordX * propertyPDFPreview.propertyBackground.width,propertyPageLoader.propertyBackupCoordY * propertyPDFPreview.propertyBackground.height)

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

    function forceScrollandFocus() {
        // Force scroll and focus to the last item added
        propertyListViewFiles.positionViewAtEnd()
        //propertyListViewFiles.forceActiveFocus()
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

            /*console.log("loadedFilePath: " + loadedFilePath + " page count: " + pageCount
                        + "minimum: " + minimumPage)*/
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
        /*console.log("Open Url Externally: " + propertyOutputSignedFile)*/
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
    function appendFileToModel(path) {
        var alreadyUploaded = false;
        var newFileUrl = {
            "fileUrl": path
        };

        if (!containsFile(newFileUrl, propertyPageLoader.propertyBackupfilesModel)){
            filesModel.append(newFileUrl);
            propertyPageLoader.propertyBackupfilesModel.append(newFileUrl);
        } else {
            alreadyUploaded = true;
        }
        return alreadyUploaded;
    }

    function updateUploadedFiles(fileList){
        var fileAlreadyUploaded = false
        for(var i = 0; i < fileList.length; i++){
            var path = fileList[i];

            path = decodeURIComponent(Functions.stripFilePrefix(path));
            if (gapi.isFile(path)) {
                fileAlreadyUploaded = appendFileToModel(path);
            } else if (gapi.isDirectory(path)) {
                var filesInDir = gapi.getFilesFromDirectory(path);
                if (filesInDir instanceof Array) {
                    updateUploadedFiles(filesInDir); // it's a folder do it again recursively
                }
            }
        }

        if (fileAlreadyUploaded){
            var titlePopup = qsTranslate("PageServicesSign","STR_FILE_UPLOAD_FAIL")
            var bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ALREADY_UPLOADED")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
    }
    function maxTextInputLength(num){
        //given number of pages returns maximum length that TextInput should accept
        return Math.ceil(Math.log(num + 1) / Math.LN10);
    }

    function numberOfAttributesSelected() {
        var count = 0
        for (var i = 0; i < entityAttributesModel.count; i++){
            if(entityAttributesModel.get(i).checkBoxAttr == true){
                count++
            }
        }
        return count
    }

    function jumpToDefinitionsSCAP(){
        propertyPageLoader.propertyBackupFromSignaturePage = true
        // Jump to Menu Definitions - PageDefinitionsSCAP
        mainFormID.state = Constants.MenuState.NORMAL
        mainFormID.propertySubMenuListView.model.clear()
        for(var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
            /*console.log("Sub Menu indice " + i + " - "
                        + mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i).subName);*/
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
