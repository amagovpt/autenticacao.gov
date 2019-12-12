/*-****************************************************************************

 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2017-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 José Pinto - <jose.pinto@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/

import QtQuick 2.6

import "../../scripts/Constants.js" as Constants
import "../../scripts/Functions.js" as Functions

//Import C++ defined enums
import eidguiV2 1.0

PageCardNotesForm {

    Keys.onPressed: {
        console.log("PageCardNotesForm onPressed:" + event.key)
        if(propertyEditNotes.focus === false){
            Functions.detectBackKeys(event.key, Constants.MenuState.SUB_MENU)
        } else {
            if(Functions.detectBackKeysTextEdit(event.key)) {
                propertyProgressBar.forceActiveFocus()
            } else {
                propertyEditNotes.forceActiveFocus()
            }
        }
    }
    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalPersoDataLoaded: {
            console.log("QML: onSignalPersoDataLoaded!")
            mainFormID.propertyPageLoader.propertyLoadedText = persoNotes;

            if(mainFormID.propertyPageLoader.propertyUnsavedNotes){
                propertyEditNotes.text = mainFormID.propertyPageLoader.propertyBackupText;
            }
            else{
                propertyEditNotes.text = mainFormID.propertyPageLoader.propertyLoadedText;
            }

            propertyBusyIndicator.running = false
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyNotesText.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Card Notes onSignalCardAccessError")
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
            else if (error_code == GAPI.CardUserPinCancel) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_CANCELED")
            }
            else if (error_code == GAPI.CardPinTimeout) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_PIN_TIMEOUT")
            }
            else {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_ACCESS_ERROR")
            }
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, true);
            propertyEditNotes.text = ""
            enableInput(false)
            propertyBusyIndicator.running = false
        }


        onSignalSetPersoDataFile: {
            mainFormID.propertyPageLoader.activateGeneralPopup(titleMessage, statusMessage, false);
        }
        onSignalCardChanged: {
            var titlePopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ")
            var bodyPopup = ""
            var returnSubMenuWhenClosed = false
            console.log("Card Notes Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                returnSubMenuWhenClosed = true
                propertyEditNotes.text = ""
                enableInput(false)
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startReadingPersoNotes()
                enableInput(true)
            }
            else{
                bodyPopup = qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                propertyEditNotes.text = ""
                enableInput(false)
            }

            mainFormID.propertyPageLoader.propertyUnsavedNotes = false
            mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, returnSubMenuWhenClosed)
        }
    }

    propertyEditNotes {
        onCursorRectangleChanged: {
            ensureVisible(propertyEditNotes.cursorRectangle)
        }
        onTextChanged: {
            // check for unsaved notes
            mainFormID.propertyPageLoader.propertyUnsavedNotes 
                = (propertyEditNotes.text != mainFormID.propertyPageLoader.propertyLoadedText);

            var strLenght = gapi.getStringByteLength(propertyEditNotes.text);
            propertyProgressBar.value = strLenght / (Constants.PAGE_NOTES_MAX_NOTES_LENGHT)
            var progressBarPerCentText = propertyProgressBar.value * 100
            progressBarText = " "+ progressBarPerCentText.toFixed(0) + "%"

            if (strLenght > Constants.PAGE_NOTES_MAX_NOTES_LENGHT) {
                var titlePopup = qsTr("STR_NOTES_PAGE_WARNING")
                var bodyPopup = qsTr("STR_NOTES_PAGE_MAX_SIZE")
                mainFormID.propertyPageLoader.activateGeneralPopup(titlePopup, bodyPopup, false)
                var cursor = propertyEditNotes.cursorPosition;
                propertyEditNotes.text = propertyEditNotes.previousText;
                if (cursor > propertyEditNotes.length) {
                    propertyEditNotes.cursorPosition = propertyEditNotes.length;
                } else {
                    propertyEditNotes.cursorPosition = cursor-1;
                }
            }
            propertyEditNotes.previousText = propertyEditNotes.text
        }
    }

    propertyMouseAreaFlickable{
        onClicked: {
            // TODO: Move cursor to the clicked position
            propertyEditNotes.forceActiveFocus()
        }
    }

    propertySaveNotes{
        onClicked: {
            gapi.startWritingPersoNotes(propertyEditNotes.text);
            mainFormID.propertyPageLoader.propertyUnsavedNotes = false;
            propertyBackupText = "";
            propertyLoadedText = "";
        }
    }

    function ensureVisible(r)
    {
        if (propertyFlickNotes.contentY >= r.y){
            propertyFlickNotes.contentY = r.y
        }else if (propertyFlickNotes.contentY+propertyFlickNotes.height <= r.y+r.height){
            propertyFlickNotes.contentY = r.y+r.height-propertyFlickNotes.height;
        }
    }
    function listProperties(item)
    {
        for (var p in item)
            console.log(p + ": " + item[p]);
    }
    function enableInput(b){
        propertyEditNotes.enabled = b
        propertySaveNotes.enabled = b
    }


    Component.onCompleted: {
        propertyBusyIndicator.running = true
        gapi.startReadingPersoNotes()
    }

    Component.onDestruction: {
        if ( mainFormID.propertyPageLoader.propertyUnsavedNotes ){
            mainFormID.propertyPageLoader.propertyBackupText = propertyEditNotes.text;
        }
    }
}
