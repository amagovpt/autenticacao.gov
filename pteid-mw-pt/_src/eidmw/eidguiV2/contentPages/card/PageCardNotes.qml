import QtQuick 2.6

/* Constants imports */
import "../../scripts/Constants.js" as Constants


PageCardNotesForm {


    Connections {
        target: gapi
        onSignalPersoDataLoaded: {
            console.log("QML: onSignalPersoDataLoaded!")
            loadPersoData(persoNotes)
        }
    }

    propertyEditNotes {
        //text: gapi.getNotesFile()
        
        onCursorRectangleChanged: {
            ensureVisible(propertyEditNotes.cursorRectangle)
        }
        onTextChanged: {
            if (propertyEditNotes.length > Constants.PAGE_NOTES_MAX_NOTES_LENGHT) {
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
