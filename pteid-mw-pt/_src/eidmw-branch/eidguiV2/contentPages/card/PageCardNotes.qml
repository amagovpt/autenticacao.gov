import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants

//Import C++ defined enums
import eidguiV2 1.0

PageCardNotesForm {


    Connections {
        target: gapi
        onSignalGenericError: {
            propertyBusyIndicator.running = false
        }
        onSignalPersoDataLoaded: {
            console.log("QML: onSignalPersoDataLoaded!")
            loadPersoData(persoNotes)
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

            if (strLenght > Constants.PAGE_NOTES_MAX_NOTES_LENGHT) {
                propertyGeneralTitleText.text = qsTr("STR_NOTES_PAGE_WARNING")
                propertyGeneralPopUpLabelText.text = qsTr("STR_NOTES_PAGE_MAX_SIZE")
                propertyPageLoader.propertyGeneralPopUp.visible = true;
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

    function loadPersoData(text) {
        propertyEditNotes.text = text
        propertyBusyIndicator.running = false
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
        propertyEditNotes.forceActiveFocus()
        propertyBusyIndicator.running = true
        //console.log("Listing GAPI object properties in QML...")
        //listProperties(gapi)
        //gapi.signalPersoDataChanged.connect(loadPersoData)
        gapi.startReadingPersoNotes()
    }
}
