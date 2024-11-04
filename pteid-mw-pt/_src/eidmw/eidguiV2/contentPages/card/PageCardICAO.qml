/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018-2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Controls 2.1
import QtGraphicalEffects 1.0


import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageCardICAOForm {
    property string outputFile : ""

    Keys.onPressed: {
        console.log("PageCardICAOForm onPressed:" + event.key)
        if(propertyCancelExport.focus){
            propertySavePhotoButton.visible = true
            collapseAnimation.start()
            propertyPhoto.forceActiveFocus()
        } else {
            Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
        }
    }

    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalReaderContext: {
            propertyBusyIndicator.running = false
        }
        onSignalICAODataChanged: {
            console.log("Data Card ICAO --> Data Changed")
            //console.trace();
            propertyTextBoxName.propertyDateField.text = gapi.getDataICAOValue(GAPI.SecondaryIdentifier)
            propertyTextBoxSurName.propertyDateField.text = gapi.getDataICAOValue(GAPI.PrimaryIdentifier)
            propertyTextBoxSex.propertyDateField.text = gapi.getDataICAOValue(GAPI.Gender)

            propertyTextBoxNacionality.propertyDateField.text = gapi.getDataICAOValue(GAPI.Nat)
            propertyTextBoxDateOfBirth.propertyDateField.text = gapi.getDataICAOValue(GAPI.DateOfBirth)
            propertyTextBoxDocumentNum.propertyDateField.text = gapi.getDataICAOValue(GAPI.DocumentNumber)
            propertyTextBoxExpirydate.propertyDateField.text = gapi.getDataICAOValue(GAPI.DateOfExpiry)
            propertyTextBoxCountry.propertyDateField.text = gapi.getDataICAOValue(GAPI.IssuingState)
            propertyPhoto.source = ""
            propertyPhoto.cache = false
            propertyPhoto.source = "image://myimageprovider/photoICAO.png"
            propertySavePhotoButton.enabled = true
            propertyBusyIndicator.running = false
            mainFormID.propertyPageLoader.propertyGeneralPopUp.close()
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                        propertyGivenNameTextForm.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Card ICAO Page onSignalCardAccessError")
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
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)

            propertyTextBoxName.propertyDateField.text = ""
            propertyTextBoxSurName.propertyDateField.text = ""
            propertyTextBoxSex.propertyDateField.text = ""
            propertyTextBoxNacionality.propertyDateField.text = ""
            propertyTextBoxDateOfBirth.propertyDateField.text = ""
            propertyTextBoxDocumentNum.propertyDateField.text = ""
            propertyTextBoxExpirydate.propertyDateField.text = ""
            propertyTextBoxCountry.propertyDateField.text = ""
            propertyPhoto.source = ""
            propertyPhoto.cache = false
            propertyBusyIndicator.running = false
            propertySavePhotoButton.enabled = false
        }
        onSignalIncompatibleCard: {
            console.log("Card ICAO Page onSignalCardAccessError")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
            var bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_INCOMPATIBLE_ICAO")
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true)
            propertyTextBoxName.propertyDateField.text = ""
            propertyTextBoxSurName.propertyDateField.text = ""
            propertyTextBoxSex.propertyDateField.text = ""
            propertyTextBoxNacionality.propertyDateField.text = ""
            propertyTextBoxDateOfBirth.propertyDateField.text = ""
            propertyTextBoxDocumentNum.propertyDateField.text = ""
            propertyTextBoxExpirydate.propertyDateField.text = ""
            propertyTextBoxCountry.propertyDateField.text = ""
            propertyPhoto.source = ""
            propertyPhoto.cache = false
            propertyBusyIndicator.running = false
            propertySavePhotoButton.enabled = false
        }
        onSignalCardChanged: {
            console.log("Card Identify Page onSignalCardChanged")
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                returnSubMenuWhenClosed = true
                propertyTextBoxName.propertyDateField.text = ""
                propertyTextBoxSurName.propertyDateField.text = ""
                propertyTextBoxSex.propertyDateField.text = ""
                propertyTextBoxNacionality.propertyDateField.text = ""
                propertyTextBoxDateOfBirth.propertyDateField.text = ""
                propertyTextBoxDocumentNum.propertyDateField.text = ""
                propertyTextBoxExpirydate.propertyDateField.text = ""
                propertyTextBoxCountry.propertyDateField.text = ""
                propertyPhoto.source = ""
                propertyPhoto.cache = false
                propertySavePhotoButton.enabled = false
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startCardICAOReading()
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                returnSubMenuWhenClosed = true;
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
        }

        // Add variable to signal
        onSignalContactlessCANNeeded: {
            console.log("QML: Contactless CAN is needed!!")
            paceDialogLoader.active = true
            paceDialogLoader.item.isICAOCAN = true
        }

        onSignalSaveCardPhotoFinished: {
            if(success) {
                createsuccess_dialog.open()
                createdSuccTitle.forceActiveFocus()
            } else {
                var titlePopup = qsTranslate("Popup Card","STR_POPUP_ERROR")
                var bodyPopup = qsTranslate("GAPI","STR_SAVE_PHOTO_ERROR")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
            }
        }

        onSignalErrorPace: {
            if(error_code === GAPI.PaceBadToken){
                console.log("QML: The stored CAN is wrong!! Will ask again!!")
                paceDialogLoader.active = true
                paceDialogLoader.item.isICAOCAN = true
            }
        }
    }

    Connections {
        target: paceDialogLoader.status === Loader.Ready ? paceDialogLoader.item : null
        onClosed: {
            paceDialogLoader.active = false
        }
    }

    Loader {
        id: paceDialogLoader
        active: false
        anchors.verticalCenter: parent.verticalCenter
        source: "../definitions/DialogRequestPaceSecret.qml"
        onLoaded: {
            paceDialogLoader.item.afterPaceAction = GAPI.IdentityData
            paceDialogLoader.item.open()
        }
    }

    Dialog {
        id: createsuccess_dialog
        width: 400
        height: 200
        visible: false
        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - createsuccess_dialog.width * 0.5
        y: parent.height * 0.5 - createsuccess_dialog.height * 0.5

        header: Label {
            id: createdSuccTitle
            text: qsTranslate("PageCardPrint","STR_CREATE_SUCESS")
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: activeFocus
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
            KeyNavigation.tab: openFileText
            KeyNavigation.right: openFileText
            KeyNavigation.down: openFileText
        }

        Item {
            width: createsuccess_dialog.availableWidth
            height: 50

            Keys.enabled: true
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                {
                    showCreatedFile()
                }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: openFileText
                    text: qsTranslate("PageCardPrint", "STR_CREATE_OPEN")
                    font.bold: activeFocus
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width - 48
                    wrapMode: Text.Wrap
                    KeyNavigation.tab: cancelButton
                    KeyNavigation.right: cancelButton
                    KeyNavigation.down: cancelButton
                    KeyNavigation.backtab: createdSuccTitle
                    KeyNavigation.up: createdSuccTitle
                }
            }
        }
        Item {
            width: createsuccess_dialog.availableWidth
            height: Constants.HEIGHT_BOTTOM_COMPONENT
            y: 80
            Item {
                width: parent.width
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    id: cancelButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_CANCEL")
                    anchors.left: parent.left
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    KeyNavigation.tab: openButton
                    KeyNavigation.right: openButton
                    KeyNavigation.backtab: openFileText
                    KeyNavigation.up: openFileText
                    highlighted: activeFocus
                    onClicked: {
                        createsuccess_dialog.close()
                        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
                }
                Button {
                    id: openButton
                    width: Constants.WIDTH_BUTTON
                    height: Constants.HEIGHT_BOTTOM_COMPONENT
                    text: qsTranslate("Popup File","STR_POPUP_FILE_OPEN")
                    anchors.right: parent.right
                    font.pixelSize: Constants.SIZE_TEXT_FIELD
                    font.family: lato.name
                    font.capitalization: Font.MixedCase
                    KeyNavigation.tab: createdSuccTitle
                    KeyNavigation.right: createdSuccTitle
                    KeyNavigation.backtab: cancelButton
                    KeyNavigation.up: cancelButton
                    highlighted: activeFocus
                    onClicked: {
                        showCreatedFile()
                    }
                    Keys.onEnterPressed: clicked()
                    Keys.onReturnPressed: clicked()
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
    }

    propertySavePhotoDialogOutput {
        onAccepted: {
            outputFile = propertySavePhotoDialogOutput.file.toString()
            console.log("Saving photo to: " + outputFile)

            var file = decodeURIComponent(Functions.stripFilePrefix(outputFile))
            gapi.startSavingCardPhoto(file)
        }
    }

    PropertyAnimation {
        id: expandAnimation
        target: propertyExportRect
        properties: "y"
        to: 0
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
    }
    PropertyAnimation {
        id: collapseAnimation
        target: propertyExportRect
        properties: "y"
        to: propertyExportRect.parent.height + Constants.FOCUS_BORDER
        duration: mainFormID.propertShowAnimation ? Constants.ANIMATION_CHANGE_OPACITY : 0
    }

    propertySavePhotoButton {
        onClicked: {
            propertySavePhotoButton.visible = false
            expandAnimation.start()
            propertyPngButton.forceActiveFocus()
        }
    }
    propertyCancelExportMouseArea {
        onClicked: {
            propertySavePhotoButton.visible = true
            collapseAnimation.start()
        }
    }

    propertyPngButton {
        onClicked: {
            propertySavePhotoDialogOutput.currentFile = propertySavePhotoDialogOutput.folder + "/FotoDoCidadao.png"
            propertySavePhotoDialogOutput.nameFilters = ["PNG (*.png)"];
            propertySavePhotoDialogOutput.open();
        }
    }

    propertyJpegButton {
        onClicked: {
            propertySavePhotoDialogOutput.currentFile = propertySavePhotoDialogOutput.folder + "/FotoDoCidadao.jpg"
            propertySavePhotoDialogOutput.nameFilters = ["JPEG (*.jpg)"];
            propertySavePhotoDialogOutput.open();
        }
    }

    function showCreatedFile(){
        Qt.openUrlExternally(outputFile)
        createsuccess_dialog.close()
        mainFormID.opacity = Constants.OPACITY_MAIN_FOCUS
    }

    // Here is what is called after changing pages
    Component.onCompleted: {
        console.log("Page Card ICAO mainWindow Completed")
        propertyBusyIndicator.running = true
        gapi.startCardICAOReading()
    }
}
