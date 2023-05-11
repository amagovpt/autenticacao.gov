/*-****************************************************************************

 * Copyright (C) 2017-2022 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 * Copyright (C) 2018-2019 Veniamin Craciun - <veniamin.craciun@caixamagica.pt>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions
import "../../components" as Components

//Import C++ defined enums
import eidguiV2 1.0

PageServicesSignForm {

    property bool isAnimationFinished: mainFormID.propertyPageLoader.propertyAnimationExtendedFinished
    property string propertyOutputSignedFile : ""
    property string ownerNameBackup: ""
    property string ownerAttrBackup: ""
    property string ownerEntitiesBackup: ""

    property string current_file: filesModel.get(propertyListViewFiles.currentIndex).fileUrl

    Keys.onRightPressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jump_to_definitions_SCAP()
    }

    Keys.onSpacePressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jump_to_definitions_SCAP()
    }

    Keys.onReturnPressed: {
        if(propertyTextAttributesMsg.activeFocus)
            jump_to_definitions_SCAP()
    }

    Keys.onPressed: {
        console.log("PageServicesSignForm onPressed:" + event.key)
        Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)

        // CTRL + V
        if ((event.key == Qt.Key_V) && (event.modifiers & Qt.ControlModifier)){
            filesArray = controler.getFilesFromClipboard();
            propertyDropArea.dropped(filesArray);
        }
    }

    Connections {
        target: gapi

        onSignalGenericError: {
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalAttributesLoaded: (institution_attributes, enterprise_attributes) => {
            console.log("Sign advanced - Signal SCAP attributes loaded")
            append_attributes_to_model(institution_attributes)
            append_attributes_to_model(enterprise_attributes)

            for (var i = 0; i < propertyPageLoader.attributeListBackup.length; i++){
                entityAttributesModel.get(i).checkBoxAttr = propertyPageLoader.attributeListBackup[i]
            }

            for (var i = 0; i < entityAttributesModel.count; ++i) {
                for (var j = 0; j < propertyPageLoader.selectedAttributesListBackup.length; ++j) {
                    if (entityAttributesModel.get(i).id == propertyPageLoader.selectedAttributesListBackup[j]) {
                        entityAttributesModel.get(i).checkBoxAttr = true
                        break;
                    }
                }
            }

            propertyBusyIndicator.running = false
            propertyListViewEntities.currentIndex = 0
            entityAttributesModel.count > 0
                    ? propertyListViewEntities.forceActiveFocus()
                    : propertyTextAttributesMsg.forceActiveFocus()

            propertyItemOptions.height = propertyOptionsHeight

            // Scroll down to see attributes rectangle
            if (!propertyFlickable.atYEnd && fileLoaded) {
                var velocity = Math.min(get_max_flick_velocity(), Constants.FLICK_Y_VELOCITY_MAX_ATTR_LIST)
                propertyFlickable.flick(0, - velocity)
            }
        }

        onSignalPdfSignSuccess: (error_code) => {
            signsuccess_dialog.open()
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            // test time stamp
            if (error_code == GAPI.SignMessageTimestampFailed) {
                if (propertyListViewFiles.count > 1 && propertyRadioButtonPADES.checked) {
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_MULTI_FAILED")
                } else {
                    signsuccess_dialog.propertySignSuccessDialogText.text =
                            qsTranslate("PageServicesSign","STR_TIME_STAMP_FAILED")
                }
            } else if (error_code == GAPI.SignMessageLtvFailed) {
                signsuccess_dialog.propertySignSuccessDialogText.text = qsTranslate("GAPI","STR_LTV_FAILED")
            } else { // Sign with time stamp succefull
                signsuccess_dialog.propertySignSuccessDialogText.text = ""
            }
        }

        onSignalSignatureFinished: {
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            gapi.setShortcutOutput("")
        }

        onSignalPdfSignFail: (error_code, index) => {
            console.log("Sign failed with error code: " + error_code)

            var error_text = ""
            if (index != -1) { //batch signature failed
                const filename = Functions.fileBaseName(propertyListViewFiles.model.get(index).fileUrl)
                error_text = qsTranslate("PageServicesSign","STR_SIGN_BATCH_FAILED") + filename
            }

            var generic_text = ""
            if (error_code == GAPI.SignFilePermissionFailed) {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_FILE_PERMISSION_FAIL")
            } else if (error_code == GAPI.PDFFileUnsupported) {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILE_UNSUPPORTED")
            } else {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_GENERIC_ERROR") + " " + error_code
            }

            show_error_message(error_text, generic_text)
        }

        onSignalPdfBatchSignFail: (error_code, filename) => {
            const batch_error_text = qsTranslate("PageServicesSign","STR_SIGN_BATCH_FAILED") + filename
            var generic_text
            if (error_code == GAPI.SignFilePermissionFailed) {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_FILE_PERMISSION_FAIL")
            } else if (error_code == GAPI.PDFFileUnsupported) {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILE_UNSUPPORTED")
            } else {
                generic_text = qsTranslate("PageServicesSign","STR_SIGN_GENERIC_ERROR") + " " + error_code
            }
            show_error_message(batch_error_text, generic_text)
        }

        onSignalSCAPServiceFail: (pdfsignresult) => {
            console.log("Sign advanced - Signal SCAP Service Fail")
            signerror_dialog.propertySignFailDialogGenericText.text = ""

            if (pdfsignresult == GAPI.ScapAttributesExpiredError ||
                    pdfsignresult == GAPI.ScapZeroAttributesError ||
                    pdfsignresult == GAPI.ScapNotValidAttributesError) {
                console.log("ScapAttributesExpiredError")
                signerror_dialog.propertySignFailDialogText.text =
                        qsTranslate("PageServicesSign","STR_SCAP_NOT_VALID_ATTRIBUTES")

                // Show button to load attributes
                closeButtonError.visible = false
                buttonLoadAttr.visible = true
                buttonCancelAttr.visible = true
            }
            //XX: this error code is not thrown in the current SCAP implementation
            else if (pdfsignresult === GAPI.ScapAttrPossiblyExpiredWarning) {
                console.log("ScapAttrPossiblyExpiredWarning")
                signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("PageServicesSign","STR_SIGN_SCAP_SERVICE_FAIL") + "<br><br>"
                    + qsTranslate("GAPI","STR_SCAP_CHECK_EXPIRED_ATTR")

                closeButtonError.visible = false
                buttonLoadAttr.visible = true
                buttonCancelAttr.visible = true
            }
            else if(pdfsignresult === GAPI.ScapClockError) {
                console.log("ScapClockError")
                signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("GAPI","STR_SCAP_CLOCK_ERROR")
            }
            else if(pdfsignresult === GAPI.ScapSecretKeyError) {
                console.log("ScapSecretKeyError")
                signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("GAPI","STR_SCAP_SECRETKEY_ERROR")
                closeButtonError.visible = false
                buttonLoadAttr.visible = true
                buttonCancelAttr.visible = true
            }
            else {
                console.log("ScapGenericError")
                signerror_dialog.propertySignFailDialogText.text =
                    qsTranslate("PageServicesSign","STR_SIGN_SCAP_SERVICE_FAIL")
            }
            signerror_dialog.visible = true
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            propertyOutputSignedFile = ""
        }

        onSignalSCAPConnectionFailed: {
            console.log("Scap connection failed")

            const text_top = qsTranslate("PageServicesSign", "STR_SCAP_PING_FAIL_FIRST")
            const text_bottom = qsTranslate("PageServicesSign", "STR_SCAP_PING_FAIL_SECOND")
            show_error_message(text_top, text_bottom)
        }

        onSignalSCAPBadCredentials: {
            console.log("Scap Bad Credentials")

            const text_top = qsTranslate("PageServicesSign", "STR_SCAP_BAD_CREDENTIALS")
            show_error_message(text_top, "")
        }

        onSignalSCAPPossibleProxyMisconfigured: {
            console.log("Scap possible misconfiguration of proxy")

            var text_top = qsTranslate("PageServicesSign","STR_SCAP_PING_FAIL_FIRST")
            var text_bottom = qsTranslate("PageServicesSign", "STR_SCAP_PING_FAIL_SECOND")
                + "<br>" + qsTranslate("GAPI","STR_VERIFY_PROXY")

            show_error_message(text_top, text_bottom)
        }

        onSignalCacheRemovedLegacy: {
            console.log("Removed Legacy Scap cache files")

            signerror_dialog.propertySignFailDialogTitle.text = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
            const text_top = qsTranslate("PageDefinitionsSCAP","STR_SCAP_LEGACY_CACHE")

            show_error_message(text_top, "")

            // Show button to load attributesText
            closeButtonError.visible = false
            buttonLoadAttr.visible = true
            buttonCancelAttr.visible = true
        }

        onSignalCacheNotReadable: {
            console.log("Scap cache not readable")

            signerror_dialog.propertySignFailDialogTitle.text = qsTranslate("PageDataApp","STR_PERMISSIONS_CACHE")
            var text_top = qsTranslate("PageDataApp","STR_CACHE_NOT_READABLE")

            show_error_message(text_top, "")
        }

        onSignalCanceledSignature: {
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalCardAccessError: (error_code) => {
            console.log("Sign Advanced Page onSignalCardAccessError")
            if (cardLoaded) {
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
                    + "<br/><br/>" + qsTranslate("Popup Card","STR_GENERIC_CARD_ERROR_MSG")
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
                else if (error_code == GAPI.IncompatibleReader) {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_INCOMPATIBLE_READER")
                }
                else {
                    bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
                }
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                if (error_code != GAPI.CardUserPinCancel && error_code != GAPI.CardPinTimeout)
                    cardLoaded = false
            }
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            propertyButtonAdd.forceActiveFocus()
        }

        onSignalSignCertDataChanged: (ownerName, NIC) => {
            console.log("Services Sign Advanced --> Certificate Data Changed")

            ownerNameBackup = ownerName
            update_wrapped_name()
            if (gapi.getUseNumId()){
                propertyPDFPreview.propertyDragSigNumIdText.text =
                        qsTranslate("GAPI","STR_NIC") + ": " + NIC
            } else {
                propertyPDFPreview.propertyDragSigNumIdText.visible = false
            }

            propertyPDFPreview.propertyDragSigDateText.visible = gapi.getUseDate()

            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            propertyButtonAdd.forceActiveFocus()
            cardLoaded = true
        }

        onSignalCardChanged: (error_code) => {
            console.log("Services Sign Advanced onSignalCardChanged")
            const titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyPDFPreview.propertyDragSigSignedByNameText.text =
                        qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY")
                if (gapi.getUseNumId()) {
                    propertyPDFPreview.propertyDragSigNumIdText.text =  qsTranslate("GAPI","STR_NIC") + ": "
                } else {
                    propertyPDFPreview.propertyDragSigNumIdText.visible = false
                }

                propertyPDFPreview.propertyDragSigDateText.visible = gapi.getUseDate()

                cardLoaded = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                cardLoaded = true
                signCertExpired = false

                gapi.startGettingInfoFromSignCert();
                gapi.startCheckSignatureCertValidity();
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                cardLoaded = false
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }

        onSignalSignCertExpired: {
            console.log("Services Sign onSignalSignCertExpired")
            const titlePopup = qsTranslate("PageServicesSign", "STR_WARNING")
            const bodyPopup = qsTranslate("PageServicesSign","STR_EXPIRED_SIGN_CERT")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            signCertExpired = true

            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalSignCertSuspended: {
            console.log("Services Sign onSignalSignCertRevoked")
            const titlePopup = qsTranslate("PageServicesSign", "STR_WARNING")
            const bodyPopup = qsTranslate("PageServicesSign","STR_SUSPENDED_SIGN_CERT")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalSignCertRevoked: {
            console.log("Services Sign onSignalSignCertRevoked")
            const titlePopup = qsTranslate("PageServicesSign", "STR_WARNING")
            const bodyPopup = qsTranslate("PageServicesSign","STR_REVOKED_SIGN_CERT")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)

            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        }

        onSignalCustomSignImageRemoved: {
            console.log("Services Sign onSignalCustomSignImageRemoved")
            const titlePopup = qsTranslate("PageServicesSign", "STR_WARNING")
            const bodyPopup = qsTranslate("PageServicesSign","STR_CUSTOM_IMAGE_REMOVED")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }

        onSignalStartCheckCCSignatureCert: {
            console.log("Services Sign onSignalStartCheckCCSignatureCert")
            propertyBusyIndicator.running = true
            mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS
        }

        onSignalOkSignCertificate: {
            propertyBusyIndicator.running = false
            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            perform_signature_CC()
        }
    }

    Connections {
        target: controler

        onSignalRetrieveStoredSignOptions: (options) => {
            propertyPageLoader.propertyBackupFormatPades = propertyRadioButtonPADES.checked = options[0]
            propertyRadioButtonXADES.checked = !propertyRadioButtonPADES.checked
            propertyPageLoader.propertyBackupTempSign = propertySwitchSignTemp.checked = options[1]
            propertyPageLoader.propertyBackupAddLTV = propertyCheckboxLTV.checked = options[2]
            propertyPageLoader.propertyBackupSignShow = propertyCheckSignShow.checked = options[3]
            propertyPageLoader.propertyBackupSignReduced = propertyCheckSignReduced.checked = options[4]
            propertyPageLoader.propertyBackupReason = propertyTextFieldReason.text = options[5]
            propertyPageLoader.propertyBackupLocal = propertyTextFieldLocal.text = options[6]
            propertyPDFPreview.propertySealWidthTemp = options[7]
            propertyPDFPreview.propertySealHeightTemp = options[8]
            propertyPageLoader.propertyBackupSwitchAddAttributes = propertySwitchAddAttributes.checked = options[9]
            propertyPageLoader.selectedAttributesListBackup = options[10]
            propertyPDFPreview.sealHasChanged = true
            propertyPDFPreview.loaded_persistent_options = true

            //propertyPDFPreview.updateSealData()
        }
     }

    Connections {
        target: image_provider_pdf

        onSignalPdfSourceChanged: (original_width) => {
            propertyPDFPreview.propertyPdfOriginalWidth = original_width
            propertyPDFPreview.forceActiveFocus()
        }
    }

    Connections {
        target: propertyPDFPreview
        onUpdateSealData: {
            var width = to_real_seal_size(propertyPDFPreview.propertyDragSigRect.width)
            var height = to_real_seal_size(propertyPDFPreview.propertyDragSigRect.height)

            propertyPageLoader.propertyBackupSealWidth = width
            propertyPageLoader.propertyBackupSealHeight = height

            gapi.resizePDFSignSeal(parseInt(width),parseInt(height))

            update_seal_font_size()

            update_wrapped_name();
            update_wrapped_location(propertyTextFieldLocal.text);
            if (propertySwitchAddAttributes.checked) update_SCAP_info_on_preview();
        }
    }

    propertyButtonArrowHelp {
        onClicked: Functions.showHelp(!propertyShowHelp)
    }

    propertyArrowHelpMouseArea {
        onClicked: Functions.showHelp(!propertyShowHelp)
    }

    PropertyAnimation {
        id: expandAnimation
        target: propertyRectHelp
        properties: "height"
        to: Constants.HEIGHT_HELP_EXPANDED
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
    }

    PropertyAnimation {
        id: collapseAnimation
        target: propertyRectHelp
        properties: "height"
        to: Constants.HEIGHT_HELP_COLLAPSED
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
    }

    propertyButtonArrowOptions {
        onClicked: Functions.showOptions(!propertyShowOptions)
    }

    propertyArrowOptionsMouseArea {
        onClicked: Functions.showOptions(!propertyShowOptions)
    }

    PropertyAnimation {
        id: expandAnimationOptions
        target: propertyItemOptions
        properties: "height"
        to: propertyOptionsHeight
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
        onRunningChanged: {
            // Scroll down to see attributes rectangle
            if (!expandAnimationOptions.running
                    && propertyFlickable.contentHeight > propertyFlickable.height) {
                propertyFlickable.flick(0, - get_max_flick_velocity())
            }
        }
    }

    PropertyAnimation {
        id: collapseAnimationOptions
        target: propertyItemOptions
        properties: "height"
        to: 0
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
    }

     ToolTip {
        property var maxWidth: 500
        id: controlToolTip
        contentItem:
            Text {
                id: tooltipText
                text: controlToolTip.text
                font: controlToolTip.font
                color: Constants.COLOR_MAIN_BLACK
                wrapMode: Text.WordWrap
                onTextChanged: {
                    controlToolTip.width = Math.min(controlToolTip.maxWidth, controlToolTip.implicitWidth)
                }
            }

        background: Rectangle {
            border.color: Constants.COLOR_MAIN_DARK_GRAY
            color: Constants.COLOR_MAIN_SOFT_GRAY
        }

        Timer {
            id: tooltipExitTimer
            interval: Constants.TOOLTIP_TIMEOUT_MS
            onTriggered: {
                controlToolTip.close()
                stop()
            }
        }
    }

    propertyMouseAreaToolTipPades{
        onEntered: {
            tooltipExitTimer.stop()
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PDF_FILES")
            controlToolTip.x = propertyRadioButtonPADES.mapToItem(controlToolTip.parent,0,0).x
                    + propertyRadioButtonPADES.width + Constants.SIZE_IMAGE_TOOLTIP * 0.5
                    - controlToolTip.width * 0.5
            controlToolTip.y = propertyRadioButtonPADES.mapToItem(controlToolTip.parent,0,0).y
                    - controlToolTip.height - Constants.SIZE_SPACE_IMAGE_TOOLTIP
            controlToolTip.open()
        }
        onExited: {
            tooltipExitTimer.start()
        }
    }

    propertyMouseAreaToolTipXades{
        onEntered: {
            tooltipExitTimer.stop()
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_SIGN_PACKAGE")
            controlToolTip.x = propertyRadioButtonXADES.mapToItem(controlToolTip.parent,0,0).x
                    + propertyRadioButtonXADES.width + Constants.SIZE_IMAGE_TOOLTIP * 0.5
                    - controlToolTip.width * 0.5
            controlToolTip.y = propertyRadioButtonXADES.mapToItem(controlToolTip.parent,0,0).y
                    - controlToolTip.height - Constants.SIZE_SPACE_IMAGE_TOOLTIP
            controlToolTip.open()
        }
        onExited: {
            tooltipExitTimer.start()
        }
    }

    propertyMouseAreaToolTipLTV{
        onEntered: {
            tooltipExitTimer.stop()
            controlToolTip.close()
            controlToolTip.text = qsTranslate("PageServicesSign","STR_LTV_TOOLTIP")
            controlToolTip.x = propertyCheckboxLTV.mapToItem(controlToolTip.parent,0,0).x
                    + propertyCheckboxLTV.width + Constants.SIZE_IMAGE_TOOLTIP * 0.5
                    - controlToolTip.width * 0.5
            controlToolTip.y = propertyCheckboxLTV.mapToItem(controlToolTip.parent,0,0).y
                    - controlToolTip.height - Constants.SIZE_SPACE_IMAGE_TOOLTIP
            controlToolTip.open()
        }
        onExited: {
            tooltipExitTimer.start()
        }
    }

    propertyCheckboxLTV {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupAddLTV = propertyCheckboxLTV.checked
        }
    }

    propertyDropArea {
        onEntered: {
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            update_uploaded_files(filesArray)
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyDropFileArea {
        onEntered: {
            filesArray = drag.urls
            console.log("Num files: "+filesArray.length);
        }
        onDropped: {
            update_uploaded_files(filesArray)
        }
        onExited: {
            console.log ("onExited");
            filesArray = []
        }
    }

    propertyFileDialogOutput {
        onAccepted: {
            var output = propertyFileDialogOutput.file.toString()
            output = decodeURIComponent(Functions.stripFilePrefix(output))
            sign_CC(output)
        }
    }

    propertyFileDialogCMDOutput {
        onAccepted: {
            sign_CMD()
        }
    }

    propertyFileDialogBatchCMDOutput {
        onAccepted: {
            sign_CMD()
        }
    }

    propertyFileDialogBatchOutput {
        onAccepted: {
            var output = propertyFileDialogBatchOutput.folder.toString()
            output = decodeURIComponent(Functions.stripFilePrefix(output))
            sign_CC(output)
        }
    }

    propertyTextFieldReason{
        onTextChanged: {
            propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
            propertyPageLoader.propertyBackupReason = propertyTextFieldReason.text
        }
    }

    propertyTextFieldLocal{
        onTextChanged: {
            update_wrapped_location(propertyTextFieldLocal.text);
            propertyPageLoader.propertyBackupLocal = propertyTextFieldLocal.text
        }
    }

    propertyCheckSignReduced{
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignReduced = propertyCheckSignReduced.checked
            if (propertyCheckSignReduced.checked) {
                    propertyPDFPreview.propertyDragSigRect.height =
                        propertyPDFPreview.propertySigHeightReducedDefault * propertyPDFPreview.propertyPDFScaleFactor
                    propertyPDFPreview.propertyDragSigRect.width =
                        propertyPDFPreview.propertySigWidthReducedDefault * propertyPDFPreview.propertyPDFScaleFactor
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.2
                propertyPDFPreview.propertyDragSigReasonText.visible = false
                propertyPDFPreview.propertyDragSigLocationText.visible = false
                propertyPDFPreview.propertyDragSigReasonText.text = ""
                propertyPDFPreview.propertyDragSigLocationText.text = ""
            } else {
                propertyPDFPreview.propertyDragSigRect.height =
                propertyPDFPreview.propertySigHeightDefault * propertyPDFPreview.propertyPDFScaleFactor
                propertyPDFPreview.propertyDragSigRect.width =
                propertyPDFPreview.propertySigWidthDefault * propertyPDFPreview.propertyPDFScaleFactor
                propertyPDFPreview.propertySigLineHeight = propertyPDFPreview.propertyDragSigRect.height * 0.1
                propertyPDFPreview.propertyDragSigReasonText.visible = true
                propertyPDFPreview.propertyDragSigLocationText.visible = true
                propertyPDFPreview.propertyDragSigReasonText.text = propertyTextFieldReason.text
                propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text === "" ? "" :
                qsTranslate("PageServicesSign", "STR_SIGN_LOCATION") + ": " + propertyTextFieldLocal.text
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

    propertyListViewEntities {
        onFocusChanged: {
            if (propertyListViewEntities.focus)
                propertyListViewEntities.currentIndex = 0
        }
    }

    Component {
        id: attributeListDelegate
        Rectangle {
            id: container
            width: parent.width
            height: Math.max(entityText.contentHeight + Constants.SIZE_TEXT_V_SPACE, checkboxSel.height)
            Keys.onSpacePressed: {
                checkboxSel.checked = !checkboxSel.checked
            }
            Keys.onTabPressed: {
                focus_forward();
                handle_key_pressed(event.key, attributeListDelegate)
            }
            Keys.onDownPressed: {
                focus_forward();
                handle_key_pressed(event.key, attributeListDelegate)
            }
            Keys.onBacktabPressed: {
                focus_backward();
                handle_key_pressed(event.key, attributeListDelegate)
            }
            Keys.onUpPressed: {
                focus_backward();
                handle_key_pressed(event.key, attributeListDelegate)
            }

            Component.onCompleted: {
                propertyListViewHeight += height
            }

            color: propertyListViewEntities.currentIndex === index && propertyListViewEntities.focus
                    ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY

            Accessible.role: Accessible.CheckBox
            Accessible.name: Functions.filterText(entityText.text)
            Accessible.checked: checkboxSel.checked

            CheckBox {
                id: checkboxSel
                font.family: lato.name
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.capitalization: Font.MixedCase
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: Constants.SIZE_LISTVIEW_SPACING
                padding: 0
                enabled: fileLoaded
                checked: checkBoxAttr
                onCheckedChanged: {
                    entityAttributesModel.get(index).checkBoxAttr = checkboxSel.checked
                    propertyPageLoader.attributeListBackup[index] = checkboxSel.checked

                    propertyPageLoader.selectedAttributesListBackup = []
                    for (var i = 0; i < entityAttributesModel.count; ++i) {
                        if (entityAttributesModel.get(i).checkBoxAttr) {
                            propertyPageLoader.selectedAttributesListBackup.push(entityAttributesModel.get(i).id)
                        }
                    }
                    update_SCAP_info_on_preview()
                }
                onFocusChanged: {
                    if(focus) propertyListViewEntities.currentIndex = index
                }
                Keys.forwardTo: container
            }
            Column {
                id: columnItem
                anchors.left: checkboxSel.right
                width: parent.width - checkboxSel.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Constants.SIZE_LISTVIEW_SPACING
                Text {
                    id: entityText
                    text: "<b>" + citizenName + "</b> - " + entityName + " - " + attribute
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.capitalization: Font.MixedCase
                }
            }
            function focus_forward() {
                checkboxSel.focus = true
                if (propertyListViewEntities.currentIndex == propertyListViewEntities.count - 1) {
                    propertyPDFPreview.forceActiveFocus()
                } else {
                    propertyListViewEntities.currentIndex++
                }
            }
            function focus_backward() {
                checkboxSel.focus = true
                if (propertyListViewEntities.currentIndex == 0) {
                    propertySwitchAddAttributes.forceActiveFocus()
                } else {
                    propertyListViewEntities.currentIndex--
                }
            }
        }
    }

    propertyMouseAreaTextAttributesMsg {
        onClicked: {
            jump_to_definitions_SCAP()
        }
    }

    propertySwitchSignTemp {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupTempSign = propertySwitchSignTemp.checked
        }
    }

    propertyCheckSignShow {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSignShow = propertyCheckSignShow.checked
        }
    }

    propertySwitchAddAttributes {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupSwitchAddAttributes = propertySwitchAddAttributes.checked
            if (propertySwitchAddAttributes.checked) {
                propertyBusyIndicator.running = true
                propertyCheckSignReduced.checked = false
                propertyTextAttributesMsg.visible = true
                propertyMouseAreaTextAttributesMsg.enabled = true
                propertyMouseAreaTextAttributesMsg.z = 1
                propertyPDFPreview.propertyDragSigCertifiedByText.visible = true
                propertyPDFPreview.propertyDragSigAttributesText.visible = true
                gapi.startLoadingAttributesFromCache(false)
            } else {
                propertyTextAttributesMsg.visible = false
                propertyMouseAreaTextAttributesMsg.enabled = false
                propertyMouseAreaTextAttributesMsg.z = 0

                entityAttributesModel.clear()
                propertyListViewHeight = 0
                propertyItemOptions.height = propertyOptionsHeight
                propertyPDFPreview.propertyDragSigCertifiedByText.visible = false
                propertyPDFPreview.propertyDragSigAttributesText.visible = false
                update_image_on_seal()
                propertyPDFPreview.forceActiveFocus()
            }
        }
    }

    propertyFileDialog {
        onAccepted: {
            console.log("Num files: " + propertyFileDialog.files.length)

            update_uploaded_files(propertyFileDialog.files)
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
            gapi.startCCSignatureCertCheck();
        }
    }

    propertyButtonSignCMD {
        onClicked: {
            console.log("Sign with CMD")
            dialogSignCMD.enableConnections()

            if (!gapi.checkCMDSupport()) {
                const titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                const bodyPopup = qsTranslate("Popup Card","STR_POPUP_NO_CMD_SUPPORT")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                return
            }

            if (propertySwitchAddAttributes.checked && number_of_attributes_selected() == 0) {
                const titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                const bodyPopup = qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                return;
            }

            var shortcutOutput = get_shortcut_output() // passed through CLI

            const prefix = (Qt.platform.os === "windows" ? "file:///" : "file://")

            if (propertyListViewFiles.count == 1) {
                if (shortcutOutput) {
                    var inputFile = propertyListViewFiles.model.get(0).fileUrl
                    inputFile = inputFile.substring(inputFile.lastIndexOf('/'), inputFile.length - 1)
                    shortcutOutput = prefix + shortcutOutput + Functions.replaceFileSuffix(inputFile, "_signed.pdf")
                    propertyFileDialogCMDOutput.file = shortcutOutput
                    propertyFileDialogCMDOutput.accepted()
                    gapi.setShortcutOutput("")
                    return
                }

                const outputFile = propertyListViewFiles.model.get(0).fileUrl
                const newSuffix = propertyRadioButtonPADES.checked ? "_signed.pdf" : "_xadessign.asics"
                if (contains_package_asic()) {
                    //no need to get output file in this case; we are adding a signature to an existing ASiC
                    sign_CMD()
                } else {
                    propertyFileDialogCMDOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
                    propertyFileDialogCMDOutput.currentFile = prefix + Functions.replaceFileSuffix(outputFile, newSuffix)
                    propertyFileDialogCMDOutput.open()
                }
            } else {
                if (propertyRadioButtonPADES.checked) {
                    if(shortcutOutput) {
                        propertyFileDialogBatchCMDOutput.folder = shortcutOutput
                        propertyFileDialogBatchCMDOutput.accepted()
                        return
                    }
                    propertyFileDialogBatchCMDOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT_FOLDER")
                    propertyFileDialogBatchCMDOutput.open()
                } else {
                    if (contains_package_asic()) {
                        const titlePopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_TITLE")
                        const bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ASIC")
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                    } else {
                        var outputFolderPath = propertyListViewFiles.model.get(propertyListViewFiles.count-1).fileUrl
                        if (outputFolderPath.lastIndexOf('/') >= 0)
                            outputFolderPath = outputFolderPath.substring(0, outputFolderPath.lastIndexOf('/'))

                        propertyFileDialogCMDOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT")
                        propertyFileDialogCMDOutput.currentFile = prefix + outputFolderPath + "/xadessign.asice";
                        propertyFileDialogCMDOutput.open()
                    }
                }
            }
        }
    }

    propertyTextSpinBox {
        text: propertySpinBoxControl.textFromValue(propertySpinBoxControl.value, propertySpinBoxControl.locale)
    }

    propertyFileDialog {
        nameFilters: propertyRadioButtonPADES.checked ?
            [ "PDF document (*.pdf)", "All files (*)" ] :
            [ "All files (*)" ]
    }

    propertyRadioButtonPADES {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupFormatPades = propertyRadioButtonPADES.checked

            //TODO: stop clearing and re-adding files to model in this case
            filesModel.clear()
            for (var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++) {
                filesModel.append({
                    "fileUrl": propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl,
                    "isASiC": propertyPageLoader.propertyBackupfilesModel.get(i).isASiC
                })
            }

            if (propertyRadioButtonPADES.checked) {
                propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                    qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
            } else {
                propertyTextDragMsgImg.text = qsTranslate("PageServicesSign","STR_SIGN_NOT_PREVIEW")
            }
        }
    }

    Component {
        id: listViewFilesDelegate
        Rectangle {
            id: rectlistViewFilesDelegate
            width: parent.width
            height: fileItem.height
            color:  propertyListViewFiles.currentIndex === index
                    ? Constants.COLOR_MAIN_DARK_GRAY : Constants.COLOR_MAIN_SOFT_GRAY

            Keys.onSpacePressed: {
                console.log("Delete file index:" + index);
                const temp = propertyListViewFiles.currentIndex //FIXME: is this needed?
                remove_file_from_list(index)
                if (propertyListViewFiles.currentIndex > 0) {
                    propertyListViewFiles.currentIndex -= temp - 1
                }
            }

            Keys.onTabPressed: {
                if (propertyListViewFiles.currentIndex == propertyListViewFiles.count -1) {
                    propertyListViewFiles.currentIndex = 0;
                    handle_key_pressed(event.key, "")
                    propertyButtonAdd.forceActiveFocus()
                } else {
                    propertyListViewFiles.currentIndex++
                    handle_key_pressed(event.key, "")
                }
            }

            MouseArea {
                id: mouseAreaFileName
                anchors.fill: parent
                hoverEnabled : true
                onClicked: {
                    propertyListViewFiles.currentIndex = index

                    if (propertyRadioButtonPADES.checked) {
                        const pageCount = gapi.getPDFpageCount(fileUrl)
                        propertyTextSpinBox.maximumLength = max_text_input_length(pageCount)
                        propertySpinBoxControl.value =
                            propertyCheckLastPage.checked ? pageCount : propertySpinBoxControl.value

                        update_current_loaded_file()
                    }
                }
            }

            Item {
                id: fileItem
                width: parent.width
                height: Math.max(fileName.contentHeight, iconRemove.height) + Constants.SIZE_LISTVIEW_IMAGE_SPACE

                Image {
                    id: iconASiCContainer
                    visible: contains_package_asic() && propertyListViewFiles.count === 1
                    x: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    width: visible ? Constants.SIZE_IMAGE_FILE_REMOVE : 0
                    height: Constants.SIZE_IMAGE_FILE_REMOVE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    source: "../../images/zip.png"
                }

                Text {
                    id: fileName
                    width: parent.width - iconRemove.width - iconASiCContainer.width - 2 * Constants.SIZE_LISTVIEW_IMAGE_SPACE
                    text: fileUrl
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: iconASiCContainer.visible ? iconASiCContainer.right : parent.left
                    anchors.leftMargin: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    color: Constants.COLOR_TEXT_BODY
                    wrapMode: Text.WrapAnywhere
                }

                Image {
                    id: iconRemove
                    width: Constants.SIZE_IMAGE_FILE_REMOVE
                    height: Constants.SIZE_IMAGE_FILE_REMOVE
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    source:  set_button_delete_icon()
                    MouseArea {
                        id: mouseAreaIconDelete
                        anchors.fill: parent
                        hoverEnabled : true
                        onClicked: {
                            console.log("Delete file index:" + index);
                            remove_file_from_list(index)
                        }
                    }

                    function set_button_delete_icon() {
                        if (mouseAreaIconDelete.containsMouse) {
                            if (propertyListViewFiles.currentIndex === index) {
                                return "../../images/remove_file_hover.png"
                            }
                            return "../../images/remove_file_hover_blue.png"
                        }
                        return "../../images/remove_file.png"
                    }
                }
            }
        }
    }

    Component {
        id: asicFilesDelegate

        Rectangle {
            id: rectAsicFilesDelegate
            width: parent.width - Constants.SIZE_ASIC_LISTVIEW_TEXT_OFFSET
            height: fileItemInASiC.height

            x: Constants.SIZE_ASIC_LISTVIEW_TEXT_OFFSET
            color: Constants.COLOR_MAIN_SOFT_GRAY

            Item {
                id: fileItemInASiC
                width: parent.width
                height: Math.max(fileNameInASiC.contentHeight, iconExtract.height) + Constants.SIZE_LISTVIEW_IMAGE_SPACE

                Text {
                    id: fileNameInASiC
                    width: parent.width - iconExtract.width - Constants.SIZE_LISTVIEW_IMAGE_SPACE
                    text: fileUrl
                    x: Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    font.family: lato.name
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: Constants.COLOR_TEXT_BODY
                    wrapMode: Text.WrapAnywhere
                }

                Image {
                    id: iconExtract
                    width: Constants.SIZE_IMAGE_FILE_EXTRACT
                    height: Constants.SIZE_IMAGE_FILE_EXTRACT
                    x: fileNameInASiC.width + Constants.SIZE_LISTVIEW_IMAGE_SPACE * 0.5
                    antialiasing: true
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                    source: {
                        mouseAreaIconExtract.containsMouse ?
                            "../../images/download_hover_blue.png" :
                            "../../images/download_icon_blue.png"
                    }
                    MouseArea {
                        id: mouseAreaIconExtract
                        anchors.fill: parent
                        hoverEnabled : true
                        onClicked: {
                            if (filesModel.count == 1) {
                                var container_path = filesModel.get(0).fileUrl
                                gapi.extractFileFromASiC(container_path, fileNameInASiC.text)
                            }
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: asicFilesModel
    }

    ListModel {
        id: filesModel

        onCountChanged: {
            console.log("filesModel onCountChanged: count = " + filesModel.count)
            fileLoaded = filesModel.count > 0

            propertyTextDragMsgImg.visible = !fileLoaded

            if (!fileLoaded) {
                propertyPDFPreview.propertyBackground.source = ""
                propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
                    qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")

                if (propertyRadioButtonXADES.checked) {
                    propertyTextDragMsgImg.text = qsTranslate("PageServicesSign","STR_SIGN_NOT_PREVIEW")
                }

                propertyButtonAdd.forceActiveFocus()

                asicFilesModel.clear()
            } else {
                if (propertyRadioButtonPADES.checked) {
                    // Preview the last file selected
                    propertyListViewFiles.currentIndex = filesModel.count - 1

                    const pageCount = gapi.getPDFpageCount(current_file)
                    if (pageCount > 0) {
                        propertyTextSpinBox.maximumLength = max_text_input_length(pageCount)
                        if (propertyCheckLastPage.checked || propertySpinBoxControl.value > pageCount) {
                            propertySpinBoxControl.value = pageCount
                        }
                        update_indicators(pageCount)
                        update_current_loaded_file()
                    } else {
                        remove_file_from_list(filesModel.count-1)

                        const titlePopup = qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR")
                        const link = "https://amagovpt.github.io/docs.autenticacao.gov/user_manual.html#problemas-com-ficheiros-pdf-não-suportados"

                        var bodyPopup = ""
                        if (pageCount === Constants.UNSUPPORTED_PDF_ERROR) {
                            console.log("Error loading pdf file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_ADVANCED_PDF_ERROR_MSG")
                        }
                        else if (pageCount === Constants.ENCRYPTED_PDF_ERROR) {
                            console.log("Error loading pdf encrypted file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_ENCRYPTED_PDF_ERROR_MSG")
                                + " " + qsTranslate("PageDefinitionsApp", "STR_MORE_INFO")
                                + "<a href=" + link + ">"
                                + qsTranslate("PageDefinitionsApp", "STR_HERE") + "</a>."
                        }
                        else if (pageCount === Constants.XFA_FORM_PDF_ERROR) {
                            console.log("Error loading pdf with XFA forms")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_XFA_FORM_PDF_ERROR_MSG")
                                + " " + qsTranslate("PageDefinitionsApp", "STR_MORE_INFO")
                                + "<a href=" + link + ">"
                                + qsTranslate("PageDefinitionsApp", "STR_HERE") + "</a>."
                        } else {
                            console.log("Generic Error loading pdf file")
                            bodyPopup = qsTranslate("PageServicesSign","STR_LOAD_PDF_ERROR_MSG")
                        }
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false, link)
                    }
                } else {
                    propertyTextDragMsgImg.visible = true
                    if (contains_package_asic() && filesModel.count == 1) {
                        const asicFilename = filesModel.get(0).fileUrl.toString()
                        const asicFiles = gapi.listFilesInASiC(asicFilename)

                        for (var i = 0; i < asicFiles.length; i++) {
                            asicFilesModel.append({ "fileUrl": asicFiles[i] })
                        }
                    } else {
                        asicFilesModel.clear()
                    }
                }
                propertyListViewFiles.forceActiveFocus()
            }
        }
    }

    propertySpinBoxControl {
        onValueChanged: {
            const maxPageAllowed = propertyCheckLastPage.checked ? gapi.getPDFpageCount(current_file) : get_minimum_page()
            if (propertySpinBoxControl.value > maxPageAllowed) {
                propertySpinBoxControl.value = maxPageAllowed
            }
            propertyPageLoader.propertyBackupPage = propertySpinBoxControl.value

            update_current_loaded_file()
            update_indicators(maxPageAllowed)
        }
    }

    propertyCheckLastPage {
        onCheckedChanged: {
            propertyPageLoader.propertyBackupLastPage = propertyCheckLastPage.checked

            if (propertyCheckLastPage.checked) {
                propertySpinBoxControl.enabled = false
                propertyPageText.enabled = false
                propertySpinBoxControl.value = gapi.getPDFpageCount(current_file)
            } else {
                propertySpinBoxControl.enabled = true
                propertyPageText.enabled = true
                propertyTextSpinBox.visible = true
                propertySpinBoxControl.value = get_minimum_page()
            }

            update_current_loaded_file()
        }
    }

    onIsAnimationFinishedChanged: {
        if (isAnimationFinished) {
            load_unfinished_signature()
        }
    }

    Component.onCompleted: {
        console.log("PageServicesSign onCompleted")

        propertyPageLoader.propertyBackupFromSignaturePage = false
        propertyBusyIndicator.running = true

        if (gapi.getShortcutFlag() == GAPI.ShortcutIdSign) {
            var paths = gapi.getShortcutPaths()
            for (var i = 0; i < paths.length; i++) {
                paths[i] = gapi.getAbsolutePath(paths[i])
            }
            update_uploaded_files(paths)

            propertyTextFieldLocal.text = gapi.getShortcutLocation()
            propertyTextFieldReason.text = gapi.getShortcutReason()
            propertySwitchSignTemp.checked = gapi.getShortcutTsa()

            // do not update fields next time (includes input, local, reason, tsa, ...)
            gapi.setShortcutFlag(GAPI.ShortcutIdNone)
        } else {
            controler.getSignatureOptions()
        }

        if (!propertyShowHelp)
            propertyRectHelp.height = Constants.HEIGHT_HELP_COLLAPSED
        if (propertyShowOptions)
            propertyItemOptions.height = propertyOptionsHeight

        update_image_on_seal()

        propertyPDFPreview.propertyDragSigSignedByNameText.text =
            qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY")
        propertyPDFPreview.propertyDragSigNumIdText.text =
            qsTranslate("GAPI","STR_NIC") + ": "

        propertyPDFPreview.propertyDragSigNumIdText.visible = gapi.getUseNumId()
        propertyPDFPreview.propertyDragSigDateText.visible = gapi.getUseDate()

        signCertExpired = false //FIXME: this boolean is not needed
        gapi.startGettingInfoFromSignCert();
        gapi.startCheckSignatureCertValidity();
    }

    Component.onDestruction: {
        gapi.closeAllPdfPreviews();
        gapi.setShortcutOutput("")
    }

    Components.DialogCMD{
        id: dialogSignCMD
    }

    Dialog {
        id: signsuccess_dialog
        width: 400
        height: 220
        visible: false
        font.family: lato.name
        modal: true
        closePolicy: Popup.CloseOnEscape

        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - signsuccess_dialog.height * 0.5
        property alias propertySignSuccessDialogText: labelText

        header: Label {
            id: titleText
            text: {
                if (propertyListViewFiles.count > 1) {
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
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return || event.key===Qt.Key_Space) {
                    open_show_signed_file_dialog()
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
                height: childRect.height
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    width: parent.width
                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    wrapMode: Text.Wrap
                }

                Text {
                    id: labelOpenText
                    width: parent.width
                    text: (propertyListViewFiles.count > 1 || propertyRadioButtonXADES.checked) ?
                            qsTranslate("PageServicesSign", "STR_SIGN_OPEN_MULTI") :
                            qsTranslate("PageServicesSign", "STR_SIGN_OPEN")

                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    wrapMode: Text.Wrap

                    anchors.top: labelText.text == "" ? parent.top : labelText.bottom
                    anchors.topMargin: labelText.text == "" ? 0 : Constants.SIZE_ROW_V_SPACE
                }
            }
        }

        Item {
            width: signsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 100
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
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
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
                        open_show_signed_file_dialog()
                    }
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectPopUp
                    KeyNavigation.down: rectPopUp
                    KeyNavigation.right: rectPopUp
                    KeyNavigation.backtab: closeButton
                    KeyNavigation.up: closeButton
                    highlighted: activeFocus
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
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
        modal: true
        closePolicy: Popup.CloseOnEscape

        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - signerror_dialog.width * 0.5
        y: parent.height * 0.5 - signerror_dialog.height * 0.5

        property alias propertySignFailDialogTitle: titleTextError
        property alias propertySignFailDialogText: text_sign_error
        property alias propertySignFailDialogGenericText: text_sign_generic_error

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
            width: parent.width
            height: parent.height

            Accessible.role: Accessible.AlertMessage
            Accessible.name: qsTranslate("Popup Card","STR_SHOW_WINDOWS")
                             + titleTextError.text + text_sign_error.text + text_sign_generic_error
            KeyNavigation.tab: closeButtonError.visible ? closeButtonError : (buttonCancelAttr.visible ? buttonCancelAttr : buttonLoadAttr)
            KeyNavigation.right: closeButtonError.visible ? closeButtonError : (buttonCancelAttr.visible ? buttonCancelAttr : buttonLoadAttr)
            KeyNavigation.down: closeButtonError.visible ? closeButtonError : (buttonCancelAttr.visible ? buttonCancelAttr : buttonLoadAttr)
            KeyNavigation.backtab: buttonLoadAttr.visible ? buttonLoadAttr : (buttonCancelAttr.visible ? buttonCancelAttr : closeButtonError)
            KeyNavigation.up: buttonLoadAttr.visible ? buttonLoadAttr : (buttonCancelAttr.visible ? buttonCancelAttr : closeButtonError)
            Column{
                spacing: 10
                Text {
                    id: text_sign_error
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    width: rectPopUpError.width
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                    maximumLineCount: 3
                }
                Text {
                    id: text_sign_generic_error
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    width: rectPopUpError.width
                    wrapMode: Text.Wrap
                }
            }

        }
        Item {
            width: signerror_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT

            anchors.bottom: parent.bottom
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: closeButtonError
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: "OK"
                    visible: true
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
                    KeyNavigation.tab: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.right: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.down: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.backtab: rectPopUpError
                    KeyNavigation.up: rectPopUpError
                    highlighted: activeFocus
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: buttonLoadAttr
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_LOAD_SCAP_ATTRIBUTES")
                    visible: false
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                        propertyPageLoader.attributeListBackup = []
                        gapi.startRemovingAttributesFromCache(GAPI.ScapAttrAll)
                        signerror_dialog.close()
                        jump_to_definitions_SCAP()
                    }
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: rectPopUpError
                    KeyNavigation.down: rectPopUpError
                    KeyNavigation.right: rectPopUpError
                    KeyNavigation.backtab: buttonCancelAttr.visible ? buttonCancelAttr : (closeButtonError.visible ? closeButtonError : rectPopUpError)
                    KeyNavigation.up: buttonCancelAttr.visible ? buttonCancelAttr : (closeButtonError.visible ? closeButtonError : rectPopUpError)
                    highlighted: activeFocus
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: buttonCancelAttr
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("PageServicesSign","STR_POPUP_CANCEL")
                    visible: false
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    onClicked: {
                        signerror_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    KeyNavigation.tab: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.right: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.down: buttonLoadAttr.visible ? buttonLoadAttr : rectPopUpError
                    KeyNavigation.backtab: rectPopUpError
                    KeyNavigation.up: rectPopUpError
                    highlighted: activeFocus
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
            }
        }

        onOpened: {
            rectPopUpError.forceActiveFocus()
        }

        onClosed: {
            //reset title and visible buttons when closing dialog
            titleTextError.text = qsTranslate("PageServicesSign","STR_SIGN_FAIL")
            closeButtonError.visible = true
            buttonLoadAttr.visible = false
            buttonCancelAttr.visible = false

            mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
            mainFormID.propertyPageLoader.forceActiveFocus()
        }
    }

    function update_current_loaded_file() {
        propertyPDFPreview.propertyBackground.cache = false
        propertyPDFPreview.propertyBackground.source = "image://pdfpreview_imageprovider/"
            + current_file + "?page=" + propertySpinBoxControl.value
        propertyPDFPreview.propertyFileName = Functions.fileBaseName(current_file)
    }

    function update_image_on_seal() {
        var image_path = gapi.getCachePath() + "/CustomSignPicture.jpg"
        if (gapi.getUseCustomSignature() && gapi.customSignImageExist()) {
            if (Qt.platform.os === "windows") {
                image_path = "file:///" + image_path
            } else {
                image_path = "file://" + image_path
            }
            propertyPDFPreview.propertyDragSigImg.source = image_path
        } else {
            propertyPDFPreview.propertyDragSigImg.source = "qrc:/images/logo_CC_seal.png"
        }
    }

    function append_attributes_to_model(attributes) {
        for (var provider in attributes) {
            for (var i = 0; i < attributes[provider].length; i++) {
                // We have to receive the attribute ids in the same array for now...
                // citizen name - attribute [id] "exampleID"
                const attribute_and_id = attributes[provider][i].split("[id]")

                const name_attributes = attribute_and_id[0].split("[at_name]")

                entityAttributesModel.append({
                    citizenName: Functions.toTitleCase(name_attributes[0]),
                    entityName: provider,
                    attribute: name_attributes[1],
                    id: attribute_and_id[1].trim(),
                    checkBoxAttr: false
                });
            }
        }
    }

    function load_unfinished_signature() {
        // Load backup data about unfinished signature
        propertyRadioButtonPADES.checked = propertyPageLoader.propertyBackupFormatPades
        propertyRadioButtonXADES.checked = !propertyPageLoader.propertyBackupFormatPades
        propertySwitchSignTemp.checked = propertyPageLoader.propertyBackupTempSign
        propertyCheckboxLTV.checked = propertyPageLoader.propertyBackupAddLTV
        propertySwitchAddAttributes.checked = propertyPageLoader.propertyBackupSwitchAddAttributes
        propertyCheckSignShow.checked = propertyPageLoader.propertyBackupSignShow
        propertyCheckSignReduced.checked = propertyPageLoader.propertyBackupSignReduced
        propertyTextFieldReason.text = propertyPageLoader.propertyBackupReason
        propertyTextFieldLocal.text = propertyPageLoader.propertyBackupLocal

        for (var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; ++i) {
            filesModel.append(propertyPageLoader.propertyBackupfilesModel.get(i))
        }

        propertySpinBoxControl.value = propertyPageLoader.propertyBackupPage
        propertyCheckLastPage.checked = propertyPageLoader.propertyBackupLastPage

        //postion depends on preview size
        const seal_pos_x = propertyPageLoader.propertyBackupCoordX * propertyPDFPreview.propertyBackground.width
        const seal_pos_y = propertyPageLoader.propertyBackupCoordY * propertyPDFPreview.propertyBackground.height
        propertyPDFPreview.setSignPreview(seal_pos_x, seal_pos_y)

        propertyTextDragMsgListView.text = propertyTextDragMsgImg.text =
            qsTranslate("PageServicesSign","STR_SIGN_DROP_MULTI")
    }

    function get_minimum_page() {
        // Function to detect minimum number of pages of all loaded pdfs to sign
        var minimumPage = 0
        for (var i = 0 ; i < filesModel.count ; i++ ) {
            const loadedFilePath = filesModel.get(i).fileUrl
            const pageCount = gapi.getPDFpageCount(loadedFilePath)
            if (minimumPage == 0 || pageCount < minimumPage)
                minimumPage = pageCount

        }
        return minimumPage
    }

    function update_indicators(pageCount) {
        propertySpinBoxControl.up.indicator.visible = true
        propertySpinBoxControl.down.indicator.visible = true
        propertySpinBoxControl.up.indicator.enabled = true
        propertySpinBoxControl.down.indicator.enabled = true

        if (propertySpinBoxControl.value === pageCount || propertySpinBoxControl.value === get_minimum_page()) {
            propertySpinBoxControl.up.indicator.visible = false
            propertySpinBoxControl.up.indicator.enabled = false
        }
        if (propertySpinBoxControl.value === 1) {
            propertySpinBoxControl.down.indicator.visible = false
            propertySpinBoxControl.down.indicator.enabled = false
        }
    }

    function open_show_signed_file_dialog() {
        signsuccess_dialog.close()

        if (!Functions.openSignedFiles()) {
            const multiple_files = propertyListViewFiles.count > 1 || propertyRadioButtonXADES.checked == 1

            var titlePopup = (multiple_files ? qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_TITLE_MULTI")
                : qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_TITLE")) + controler.autoTr

            var bodyPopup = (multiple_files ? qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR_MULTI")
                : qsTranslate("PageServicesSign","STR_SIGN_OPEN_ERROR")) + controler.autoTr

            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        }
    }

    function only_unique(value, index, self) {
        return self.indexOf(value) === index;
    }

    //should only used by a fileModel list because it uses .count instead of .length
    function contains_file(obj, list) {
        var i;
        for (i = 0; i < list.count; i++) {
            if (list.get(i).fileUrl.toString() === obj.fileUrl) {
                console.log("File already uploaded");
                return true;
            }
        }
        return false;
    }

    function append_file_to_model(path) {
        var alreadyUploaded = false;
        var newFileUrl = {
            "fileUrl": path,
            "isASiC" : propertyRadioButtonXADES.checked ? gapi.isASiC(path) : false
        };

        if (!contains_file(newFileUrl, propertyPageLoader.propertyBackupfilesModel)) {
            propertyPageLoader.propertyBackupfilesModel.append(newFileUrl);
            filesModel.append(newFileUrl);
        } else {
            alreadyUploaded = true;
        }
        return alreadyUploaded;
    }

    function contains_package_asic() {
        for (var i = 0; i < filesModel.count; i++) {
            if (filesModel.get(i).isASiC) {
                return true;
            }
        }
        return false;
    }

    function update_uploaded_files(fileList) {
        for (var i = 0; i < fileList.length; i++) {
            const path = decodeURIComponent(Functions.stripFilePrefix(fileList[i]));

            if (gapi.isFile(path)) {

                // XAdES related checks should only be done if XAdES configuration is enabled
                if (propertyRadioButtonXADES.checked) {
                    if (gapi.isASiC(path) && filesModel.count > 0) {
                        // asic container must be the only file on the list - warning
                        const titlePopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_TITLE")
                        const bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_ONLY_FILE")
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                        return
                    } else if (contains_package_asic()) {
                        // asic container already on the list - warning
                        const titlePopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_TITLE")
                        const bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_ALREADY_ON_LIST")
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                        return
                    }
                }

                const fileAlreadyUploaded = append_file_to_model(path);
                if (fileAlreadyUploaded) {
                    // file already uploaded - error
                    const titlePopup = qsTranslate("PageServicesSign","STR_FILE_UPLOAD_FAIL")
                    const bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ALREADY_UPLOADED")
                    mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                }

            } else if (gapi.isDirectory(path)) {
                const filesInDir = gapi.getFilesFromDirectory(path);
                if (filesInDir instanceof Array) {
                    update_uploaded_files(filesInDir); // it's a folder do it again recursively
                }
            }
        }
    }

    function max_text_input_length(num){
        //given number of pages returns maximum length that TextInput should accept
        //returns number of digits in an integer
        return Math.ceil(Math.log(num + 1) / Math.LN10);
    }

    function number_of_attributes_selected() {
        var count = 0
        for (var i = 0; i < entityAttributesModel.count; i++) {
            if (entityAttributesModel.get(i).checkBoxAttr) {
                count++
            }
        }
        return count
    }

    function jump_to_definitions_SCAP() {
        propertyPageLoader.propertyBackupFromSignaturePage = true

        // Jump to Menu Definitions - PageDefinitionsSCAP
        mainFormID.state = Constants.MenuState.NORMAL
        mainFormID.propertySubMenuListView.model.clear()

        for (var i = 0; i < mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.count; ++i) {
            const submenu_entry = mainFormID.propertyMainMenuBottomListView.model.get(0).subdata.get(i)

            mainFormID.propertySubMenuListView.model.append({
                "subName": qsTranslate("MainMenuBottomModel", submenu_entry.subName),
                "expand": submenu_entry.expand,
                "url": submenu_entry.url
            })
        }

        mainFormID.propertyMainMenuListView.currentIndex = -1
        mainFormID.propertyMainMenuBottomListView.currentIndex = 0
        mainFormID.propertySubMenuListView.currentIndex = 1
        mainFormID.propertyPageLoader.source = "/contentPages/definitions/PageDefinitionsSCAP.qml"
    }

    function toggle_switch(element) {
        element.checked = !element.checked
    }

    function toggle_radio(element) {
        if (!element.checked) {
            element.checked = true
        }
    }

    function perform_signature_CC() {
        if (gapi.doGetTriesLeftSignPin() === 0) {
            var titlePopup = qsTranslate("Popup PIN", "STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("Popup PIN", "STR_POPUP_CARD_PIN_SIGN_BLOCKED")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
        } else {
            // If application was started with signAdvanced and output option from command line
            var shortcutOutput = get_shortcut_output()
            if (shortcutOutput) {
                if (propertyListViewFiles.count == 1) {
                    var inputFile = propertyListViewFiles.model.get(0).fileUrl
                    inputFile = inputFile.substring(inputFile.lastIndexOf('/'), inputFile.length - 1)
                    shortcutOutput = shortcutOutput + Functions.replaceFileSuffix(inputFile, "_signed.pdf")
                }
                sign_CC(shortcutOutput)
                return;
            }

            if (propertySwitchAddAttributes.checked && number_of_attributes_selected() == 0) {
                //SCAP switch checked but no attributes selected
                var titlePopup = qsTranslate("PageServicesSign","STR_SCAP_WARNING")
                var bodyPopup = qsTranslate("PageServicesSign","STR_SCAP_ATTRIBUTES_NOT_SELECT")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                return
            }

            var prefix = (Qt.platform.os === "windows" ? "file:///" : "file://");
            if (propertyListViewFiles.count == 1) {
                propertyFileDialogOutput.title = qsTranslate("Popup File", "STR_POPUP_FILE_OUTPUT")

                var outputFile = propertyListViewFiles.model.get(0).fileUrl
                var newSuffix = propertyRadioButtonPADES.checked ? "_signed.pdf" : "_xadessign.asics"

                if (contains_package_asic()) {
                    var output = decodeURIComponent(Functions.stripFilePrefix(outputFile))
                    sign_CC(output)
                } else {
                    propertyFileDialogOutput.currentFile = prefix + Functions.replaceFileSuffix(outputFile, newSuffix)
                    propertyFileDialogOutput.open()
                }
            } else {
                if (propertyRadioButtonPADES.checked) {
                    propertyFileDialogBatchOutput.title = qsTranslate("Popup File","STR_POPUP_FILE_OUTPUT_FOLDER")
                    propertyFileDialogBatchOutput.open()
                } else {
                    if (contains_package_asic()) {
                        var titlePopup = qsTranslate("PageServicesSign","STR_FILE_ASIC_TITLE")
                        var bodyPopup = qsTranslate("PageServicesSign","STR_FILE_ASIC")
                        mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                    } else {
                        var outputFolderPath = propertyListViewFiles.model.get(propertyListViewFiles.count-1).fileUrl
                        if (outputFolderPath.lastIndexOf('/') >= 0)
                            outputFolderPath = outputFolderPath.substring(0, outputFolderPath.lastIndexOf('/'))
                        propertyFileDialogOutput.currentFile = prefix + outputFolderPath + "/xadessign.asice";
                        propertyFileDialogOutput.open()
                    }
                }
            }
        }
    }

    function sign_CC(outputFile) {
        propertyPageLoader.persist_signature_options()

        propertyBusyIndicator.running = true
        mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS

        const isTimestamp = propertySwitchSignTemp.checked
        const isLTV = propertyCheckboxLTV.checked
        const pades = propertyRadioButtonPADES.checked

        propertyOutputSignedFile = outputFile;

        var input_files = []
        for (var i = 0; i < propertyListViewFiles.count; i++) {
            input_files[i] =  propertyListViewFiles.model.get(i).fileUrl;
        }
        input_files = input_files.filter(only_unique); //TODO: check for uniqueness on "file-loading" instead

        if (pades) {
            const page = propertySpinBoxControl.value
            const reason = propertyTextFieldReason.text
            const location = propertyTextFieldLocal.text
            const isSmallSignature = propertyCheckSignReduced.checked
            const isLastPage = propertyCheckLastPage.checked
            var coord_x = -1
            var coord_y = -1
            if (propertyCheckSignShow.checked) {
                coord_x = propertyPDFPreview.propertyCoordX
                coord_y = propertyPDFPreview.propertyCoordY
            }

            console.log("Signing in position coord_x: " + coord_x + " and coord_y: " + coord_y + " page: "
                + page + " timestamp: " + isTimestamp + " ltv: " + isLTV + " lastPage: " + isLastPage)

            const scap = propertySwitchAddAttributes.checked
            if (scap) {
                const attr_list = get_selected_attributes_ids()

                gapi.startSigningSCAP(input_files, outputFile, page, coord_x, coord_y,
                    location, reason, isTimestamp, isLTV, isLastPage, attr_list)
            } else {
                if (propertyListViewFiles.count == 1) {
                    gapi.startSigningPDF(input_files[0], outputFile, page, coord_x, coord_y,
                        reason, location, isTimestamp, isLTV, isSmallSignature)
                } else {
                    gapi.startSigningBatchPDF(input_files, outputFile, page, coord_x, coord_y,
                        reason, location, isTimestamp, isLTV, isSmallSignature, isLastPage)
                }
            }
        } else {  //XAdES signature
            propertyOutputSignedFile = propertyOutputSignedFile.substring(0, propertyOutputSignedFile.lastIndexOf('/'))
            if (propertyListViewFiles.count == 1) {
                gapi.startSigningXADES(input_files[0], outputFile, isTimestamp, isLTV, contains_package_asic())
            } else {
                gapi.startSigningBatchXADES(input_files, outputFile, isTimestamp, isLTV)
            }
        }
    }

    function determine_output_filename() {
        var outputFile = ""
        const pades = propertyRadioButtonPADES.checked
        if (dialogSignCMD.isSignSingleFile() || !pades) {
            if (contains_package_asic()) {
                //FIXME: LINUX and MAC "file:///" ?
                const prefix = (Qt.platform.os === "windows" ? "file:///" : "file://")
                const outputFile = propertyListViewFiles.model.get(0).fileUrl
                const newSuffix = pades ? "_signed.pdf" : "_xadessign.asics" //FIXME: checking for PAdES when contains_package_asic is true??
                outputFile = prefix + Functions.replaceFileSuffix(outputFile, newSuffix)
            } else {
                outputFile = propertyFileDialogCMDOutput.file.toString()
            }
        } else {
            outputFile = propertyFileDialogBatchCMDOutput.folder.toString()
        }
        outputFile = decodeURIComponent(Functions.stripFilePrefix(outputFile))

        return outputFile
    }

    function sign_CMD() {
        propertyPageLoader.persist_signature_options()

        propertyBusyIndicator.running = true
        mainFormID.opacity = Constants.OPACITY_POPUP_FOCUS

        const isTimestamp = propertySwitchSignTemp.checked
        const isLTV = propertyCheckboxLTV.checked
        const pades = propertyRadioButtonPADES.checked

        //TODO: is this 'SignSingleFile' property still usefull?
        dialogSignCMD.setSignSingleFile(filesModel.count == 1)

        const outputFile = determine_output_filename()
        propertyOutputSignedFile = outputFile

        var inputFiles = []
        for (var i = 0; i < propertyListViewFiles.count; i++) {
            inputFiles[i] = propertyListViewFiles.model.get(i).fileUrl;
        }

        if (pades) {
            const page = propertySpinBoxControl.value
            const isLastPage = propertyCheckLastPage.checked
            const reason = propertyTextFieldReason.text
            const location = propertyTextFieldLocal.text
            const isSmallSignature = propertyCheckSignReduced.checked

            var coord_x = -1
            var coord_y = -1
            if (propertyCheckSignShow.checked) {
                coord_x = propertyPDFPreview.propertyCoordX
                coord_y = propertyPDFPreview.propertyCoordY
            }

            const scap_signature = propertySwitchAddAttributes.checked
            if (scap_signature) {
                const attributeList = get_selected_attributes_ids()
                gapi.signScapWithCMD(inputFiles, outputFile, attributeList, page, coord_x, coord_y,
                    reason, location, isTimestamp, isLTV, isLastPage)
            } else {
                gapi.signCMD(inputFiles, outputFile, page, coord_x, coord_y, reason, location, isTimestamp,
                    isLTV,isSmallSignature, isLastPage)
            }
        } else {
            propertyOutputSignedFile = propertyOutputSignedFile.substring(0, propertyOutputSignedFile.lastIndexOf('/'))

            gapi.startSigningXADESWithCMD(inputFiles, outputFile, isTimestamp, isLTV, contains_package_asic())
        }
    }

    function get_shortcut_output() {
        var output = gapi.getShortcutOutput()
        if (output == "") {
            return null
        }

        output = gapi.getAbsolutePath(output)
        //TODO: is this check needed? what is wrong with "//"
        if (output.charAt(output.length - 1) != "/") {
            output += "/"
        }
        return output
    }

    function handle_key_pressed(key, callingObject) {
        const direction = get_kb_nav_direction(key)
        switch (direction) {
            case Constants.DIRECTION_UP:
                if (callingObject === propertyTitleHelp && !propertyFlickable.atYEnd) {
                    propertyFlickable.flick(0, - get_max_flick_velocity())
                }
                else if (callingObject === attributeListDelegate && !propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, Constants.FLICK_Y_VELOCITY_ATTR_LIST);
                }
                else if (!propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, Constants.FLICK_Y_VELOCITY)
                }
                break;

            case Constants.DIRECTION_DOWN:
                if (callingObject === propertyRadioButtonXADES && !fileLoaded && !propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, get_max_flick_velocity());
                }
                else if (callingObject === propertyButtonSignWithCC && !propertyButtonSignCMD.enabled && !propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, get_max_flick_velocity());
                }
                else if (callingObject === propertyButtonSignCMD && !propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, get_max_flick_velocity());
                }
                else if (callingObject === propertyListViewEntities && !propertyFlickable.atYBeginning) {
                    propertyFlickable.flick(0, get_max_flick_velocity());
                }
                else if (callingObject === attributeListDelegate && !propertyFlickable.atYEnd) {
                    propertyFlickable.flick(0, - Constants.FLICK_Y_VELOCITY_ATTR_LIST);
                }
                else if (!propertyFlickable.atYEnd) {
                    propertyFlickable.flick(0, - Constants.FLICK_Y_VELOCITY)
                }
                break;
        }
    }

    function get_kb_nav_direction(key) {
        var direction = Constants.NO_DIRECTION;
        if (key == Qt.Key_Backtab || key == Qt.Key_Up || key == Qt.Key_Left) {
            direction = Constants.DIRECTION_UP;
        }
        else if (key == Qt.Key_Tab || key == Qt.Key_Down || key == Qt.Key_Right) {
            direction = Constants.DIRECTION_DOWN;
        }
        return direction;
    }

    function update_wrapped_name() {
        const scap_selected = (propertySwitchAddAttributes.checked && number_of_attributes_selected() != 0)
        const max_lines = scap_selected ? Constants.NAME_SCAP_MAX_LINES : Constants.NAME_MAX_LINES
        const wrapped_name = gapi.getWrappedText(ownerNameBackup, max_lines, Constants.SEAL_NAME_OFFSET)

        propertyPDFPreview.propertyDragSigSignedByNameText.text = qsTranslate("PageDefinitionsSignature","STR_CUSTOM_SIGN_BY") + " <b>" +
            wrapped_name.join("<br>") + "</b>";
    }

    function update_wrapped_location(location) {
        const wrapped_location = gapi.getWrappedText(location, Constants.LOCATION_MAX_LINES, Constants.SEAL_LOCATION_OFFSET)

        propertyPDFPreview.propertyDragSigLocationText.text = propertyTextFieldLocal.text === "" ? "" :
            qsTranslate("PageServicesSign", "STR_SIGN_LOCATION") + ": " + wrapped_location.join("<br>");
    }

    function get_max_flick_velocity() {
        // use visible area of flickable object to calculate a smooth flick velocity
        return 200 + Constants.FLICK_Y_VELOCITY_MAX * (1 - propertyFlickable.visibleArea.heightRatio)
    }

    function remove_file_from_list(index) {
        gapi.closePdfPreview(filesModel.get(index).fileUrl);
        for (var i = 0; i < propertyPageLoader.propertyBackupfilesModel.count; i++) {
            if (propertyPageLoader.propertyBackupfilesModel.get(i).fileUrl === filesModel.get(index).fileUrl) {
                propertyPageLoader.propertyBackupfilesModel.remove(i)
            }
        }
        filesModel.remove(index)
    }

    function update_SCAP_info_on_preview() {
        propertyPDFPreview.propertyDragSigCertifiedByText.text = qsTranslate("PageServicesSign", "STR_SCAP_CERTIFIED_BY")
        propertyPDFPreview.propertyDragSigAttributesText.text = qsTranslate("PageServicesSign", "STR_SCAP_CERTIFIED_ATTRIBUTES")

        var attribute_ids = get_selected_attributes_ids()

        if (attribute_ids != []) {
            var attributesToWrap = gapi.getSCAPAttributesText(attribute_ids)

            const entitiesText = attributesToWrap[0]
            const attributesText = attributesToWrap[1]

            ownerEntitiesBackup = entitiesText
            ownerAttrBackup = attributesText

            update_seal_font_size()

            const entities = gapi.getWrappedText(entitiesText, Constants.PROVIDER_SCAP_MAX_LINES, Constants.SEAL_PROVIDER_NAME_OFFSET)
            const attributes = gapi.getWrappedText(attributesText, Constants.ATTR_SCAP_MAX_LINES, Constants.SEAL_ATTR_NAME_OFFSET)
            const fontSize = attributesToWrap[2]

            propertyPDFPreview.propertyDragSigCertifiedByText.text += " <b>" + entities.join("<br>") + "</b>";
            propertyPDFPreview.propertyDragSigAttributesText.text += " <b>" + attributes.join("<br>") + "</b>";
            propertyPDFPreview.propertyCurrentAttrsFontSize = fontSize

            var scap_logo = gapi.getSCAPProviderLogo(attribute_ids)
            if (scap_logo.length != 0) {
                propertyPDFPreview.propertyDragSigImg.source = "file:///" + scap_logo
            } else {
                update_image_on_seal()
            }
        }
    }

    function get_selected_attributes_ids() {
        var attributeList = []
        for (var i = 0; i < entityAttributesModel.count; i++) {
            if (entityAttributesModel.get(i).checkBoxAttr) {
                attributeList.push(entityAttributesModel.get(i).id)
            }
        }

        return attributeList
    }

    function update_seal_font_size() {
        const is_reduced = propertyCheckSignReduced.checked
        const reason = propertyTextFieldReason.text
        const name = ownerNameBackup
        const nic = gapi.getUseNumId()
        const date = gapi.getUseDate()
        const location = propertyTextFieldLocal.text
        const entities = ownerEntitiesBackup
        const attributes = ownerAttrBackup
        const width = to_real_seal_size(propertyPDFPreview.propertyDragSigRect.width)
        const height = to_real_seal_size(propertyPDFPreview.propertyDragSigRect.height)

        const calculated_font_size = gapi.getSealFontSize(is_reduced, reason, name, nic, date, location, entities, attributes, width, height)

        propertyPDFPreview.propertyFontSize = calculated_font_size
        propertyPDFPreview.propertyFontMargin = calculated_font_size > 5 ? 1 : 0
    }

    function to_real_seal_size(value) {
        return value / propertyPDFPreview.propertyPDFScaleFactor / propertyPDFPreview.propertyConvertPtsToPixel
    }

    function from_real_seal_size(value) {
        return value * propertyPDFPreview.propertyPDFScaleFactor * propertyPDFPreview.propertyConvertPtsToPixel
    }

    function show_error_message(text, generic_text) {
        signerror_dialog.propertySignFailDialogText.text = text
        signerror_dialog.propertySignFailDialogGenericText.text = generic_text

        dialogSignCMD.close()
        signerror_dialog.visible = true
        propertyBusyIndicator.running = false
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
        propertyOutputSignedFile = ""
    }
}
