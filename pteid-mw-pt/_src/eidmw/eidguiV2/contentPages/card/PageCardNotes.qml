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
            propertyEditNotes.text = persoNotes
            propertyBusyIndicator.running = false
            if(mainFormID.propertyPageLoader.propertyForceFocus)
                propertyNotesText.forceActiveFocus()
        }
        onSignalCardAccessError: {
            console.log("Card Notes onSignalCardAccessError")

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

            propertyEditNotes.text = ""
            propertyBusyIndicator.running = false
        }


        onSignalSetPersoDataFile: {
            propertyGeneralTitleText.text = titleMessage
            propertyGeneralPopUpLabelText.text = statusMessage
            propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
        onSignalCardChanged: {
            console.log("Card Notes Page onSignalCardChanged")
            if (error_code == GAPI.ET_CARD_REMOVED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_REMOVED")
                propertyEditNotes.text = ""
            }
            else if (error_code == GAPI.ET_CARD_CHANGED) {
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                         qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_CHANGED")
                propertyBusyIndicator.running = true
                gapi.startReadingPersoNotes()
            }
            else{
                mainFormID.propertyPageLoader.propertyGeneralTitleText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ")
                mainFormID.propertyPageLoader.propertyGeneralPopUpLabelText.text =
                        qsTranslate("Popup Card","STR_POPUP_CARD_READ_UNKNOWN")
                propertyEditNotes.text = ""
            }

            mainFormID.propertyPageLoader.propertyGeneralPopUp.visible = true;
            mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
        }
    }

    propertyEditNotes {
        onCursorRectangleChanged: {
            ensureVisible(propertyEditNotes.cursorRectangle)
        }
        onTextChanged: {
            var strLenght = gapi.getStringByteLength(propertyEditNotes.text);
            propertyProgressBar.value = strLenght / (Constants.PAGE_NOTES_MAX_NOTES_LENGHT)
            var progressBarPerCentText = propertyProgressBar.value * 100
            progressBarText = " "+ progressBarPerCentText.toFixed(0) + "%"

            if (strLenght > Constants.PAGE_NOTES_MAX_NOTES_LENGHT) {
                propertyGeneralTitleText.text = qsTr("STR_NOTES_PAGE_WARNING")
                propertyGeneralPopUpLabelText.text = qsTr("STR_NOTES_PAGE_MAX_SIZE")
                propertyPageLoader.propertyGeneralPopUp.visible = true;
                mainFormID.propertyPageLoader.propertyRectPopUp.forceActiveFocus();
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
            gapi.startWritingPersoNotes(propertyEditNotes.text)
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


    Component.onCompleted: {
        propertyBusyIndicator.running = true
        gapi.startReadingPersoNotes()
    }
}
