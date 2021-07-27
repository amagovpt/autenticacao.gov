/*-****************************************************************************

 * Copyright (C) 2017 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguel.figueira@caixamagica.pt>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

import QtQuick 2.6
import QtQuick.Window 2.2

/* Constants imports */
import "../scripts/Constants.js" as Constants

Rectangle {
    id: pdfPreview

    property alias propertyBackground: background_image
    property alias propertyDragSigRect: dragSigRect
    property alias propertyDragSigReasonText: sigReasonText
    property alias propertyDragSigSignedByText: sigSignedByText
    property alias propertyDragSigSignedByNameText: sigSignedByNameText
    property alias propertyDragSigNumIdText: sigNumIdText
    property alias propertyDragSigDateText: sigDateText
    property alias propertyDragSigLocationText: sigLocationText
    property alias propertyDragSigImg: dragSigImage
    property alias propertyDragSigWaterImg: dragSigWaterImage
    property alias propertyDragSigCertifiedByText: sigCertifiedByText
    property alias propertyDragSigAttributesText: sigAttributesText
    property alias propertycontainerMouseMovCalcBottom: containerMouseMovCalcBottom
    property alias propertycontainerMouseMovCalcRight: containerMouseMovCalcRight

    property real propertySigLineHeight: dragSigRect.height * 0.1
    property bool propertyReducedChecked: false

    property alias propertyCoordX: dragTarget.coord_x
    property alias propertyCoordY: dragTarget.coord_y

    // Properties used to convert to postscript points (1 px == 0.75 points)
    // Signature have a static size
    property real propertyConvertPixelToPts: 1 / 0.75
    property real propertySigWidthDefault: 178
    property real propertySigHeightDefault: 90
    property real propertySigWidthMin: 89
    property real propertySigHeightMin: 45
    
    property real propertyPDFHeightScaleFactor: background_image.height * propertyConvertPixelToPts
                                               / (propertyPdfOriginalHeight / propertyConvertPixelToPts)
    property real propertyPDFWidthScaleFactor: background_image.width * propertyConvertPixelToPts
                                               / (propertyPdfOriginalWidth / propertyConvertPixelToPts)

    property real propertySigFontSizeBig: 8 * propertyPDFHeightScaleFactor
    property real propertySigFontSizeSmall: 6 * propertyPDFHeightScaleFactor
    property real propertyCurrentAttrsFontSize: 8

    //Properties to store Pdf original size
    property real propertyPdfOriginalWidth: 0
    property real propertyPdfOriginalHeight: 0

    property real stepSizeX : width * 0.1
    property real stepSizeY : height * 0.1

    property bool sealHasChanged: false
    property string propertyFileName: ""

    color: Constants.COLOR_MAIN_SOFT_GRAY

    FontLoader {
        id: myriad
        name: "MyriadPro"
        source: controler.getFontFile("myriad")
    }

    Keys.onUpPressed: {
        moveUp(stepSizeY)
        toggleFocus()
    }

    Keys.onDownPressed: {
        moveDown(stepSizeY)
        toggleFocus()
    }

    Keys.onLeftPressed: {
        moveLeft(stepSizeX)
        toggleFocus()
    }

    Keys.onRightPressed: {
        moveRight(stepSizeX)
        toggleFocus()
    }

    Text {
        id: positionText
        text: "X " + Math.round(dragTarget.coord_x*100) + "% " + "Y " + Math.round(dragTarget.coord_y*100) + "%"
        visible: false
        Accessible.role: Accessible.StaticText
        Accessible.name: text
    }

    Accessible.role: Accessible.Canvas
    Accessible.name: qsTranslate("PageServicesSign", "STR_SIGN_NAV_FILE_PREVIEW")
                     + propertyFileName + "."
                     + qsTranslate("PageServicesSign", "STR_SIGN_NAV_DESCRIPTION")

    DropArea {
        id: dragTarget
        width: parent.width
        height: parent.height

        //Properties to store current signature position
        property real coord_x : 0
        property real coord_y : 0

        //Properties to store last signature positions
        property real lastCoord_x : 0
        property real lastCoord_y : 0

        //Properties to store last screen size
        property real lastScreenWidth : 0
        property real lastScreenHeight : 0

        Image {
            id: background_image
            sourceSize.width: dragTarget.width
            sourceSize.height: dragTarget.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            asynchronous: true

            Rectangle {
                width: dragSigRect.width + 2 * Constants.FOCUS_BORDER
                height: dragSigRect.height + 2 * Constants.FOCUS_BORDER
                x: dragSigRect.x - Constants.FOCUS_BORDER
                y: dragSigRect.y - Constants.FOCUS_BORDER
                border.width: Constants.FOCUS_BORDER
                border.color: pdfPreview.activeFocus || positionText.activeFocus ? Constants.COLOR_MAIN_DARK_GRAY
                             : Constants.COLOR_GREY_BUTTON_BACKGROUND
                color: "transparent"
                visible: width >= Constants.FOCUS_BORDER && background_image.status != Image.Null && dragSigRect.visible
            }
            Item {
                id: dragSigRect
                width: (propertySigWidthDefault) * propertyPDFWidthScaleFactor
                height: (propertySigHeightDefault) * propertyPDFHeightScaleFactor

                Drag.active: dragArea.drag.active
                opacity: background_image.status == Image.Ready ? 1.0 : 0.0

                Item {
                    id: containerMouseMovCalcRight
                    z:10
                    width: Constants.FOCUS_BORDER; height: parent.height
                    anchors.left: parent.right
                    MouseArea {
                        id: mouseRegioncontainerMouseMovCalcRight
                        anchors.fill: parent

                        property variant lastPos: ""

                        cursorShape: Qt.SizeHorCursor

                        onPressed: lastPos = controler.getCursorPos()

                        onPositionChanged: {

                            var newPos = controler.getCursorPos()
                            var delta = Qt.point(newPos.x-lastPos.x, newPos.y-lastPos.y)

                            var diff = (background_image.width - background_image.width) / 2
                            if (dragSigRect.x + dragSigRect.width + delta.x > background_image.width) {
                                dragSigRect.width = dragSigRect.width
                            }
                            else if(dragSigRect.width+delta.x > (propertySigWidthMin) * propertyPDFWidthScaleFactor){
                                dragSigRect.width = dragSigRect.width+delta.x
                            }
                            else {
                                dragSigRect.width = (propertySigWidthMin) * propertyPDFWidthScaleFactor
                            }
                            lastPos = newPos;

                            gapi.resizeSignPreview(
                                parseInt(dragSigRect.width / propertyPDFWidthScaleFactor), 
                                parseInt(dragSigRect.height / propertyPDFHeightScaleFactor))
                        }
                    }
                }

                Item {
                    id: containerMouseMovCalcBottom
                    z:10
                    width: parent.width ; height: Constants.FOCUS_BORDER
                    anchors.top: dragSigRect.bottom
                    MouseArea {
                        id: mouseRegioncontainerMouseMovCalcBottom
                        anchors.fill: parent

                        property variant lastPos: ""

                        cursorShape: Qt.SizeVerCursor

                        onPressed: lastPos = controler.getCursorPos()
                            
                        onPositionChanged: {

                            var newPos = controler.getCursorPos()
                            var delta = Qt.point(newPos.x-lastPos.x, newPos.y-lastPos.y)

                            if (dragSigRect.y + dragSigRect.height + delta.y > background_image.height) {
                                dragSigRect.height = dragSigRect.height
                            }
                            else if(dragSigRect.height+delta.y > (propertySigHeightMin) * propertyPDFWidthScaleFactor){
                                dragSigRect.height = (dragSigRect.height+delta.y)
                            }else{
                                dragSigRect.height = (propertySigHeightMin) * propertyPDFHeightScaleFactor
                            }

                            lastPos = newPos;

                            gapi.resizeSignPreview(
                                parseInt(dragSigRect.width / propertyPDFWidthScaleFactor), 
                                parseInt(dragSigRect.height / propertyPDFHeightScaleFactor))
                        }
                    }
                }

                Text {
                    id: sigReasonText
                    font.pixelSize: propertySigFontSizeBig
                    font.italic: true
                    height: font.pixelSize + Constants.SIZE_SIGN_SEAL_TEXT_V_SPACE
                    width: parent.width - 4
                    clip: true
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_LABEL
                    text: ""
                    anchors.top: dragSigRect.top
                    anchors.topMargin: 2
                    x: 2
                }

                Image {
                    id: dragSigWaterImage
                    height: propertyReducedChecked ? propertySigHeightDefault * propertyPDFHeightScaleFactor * 0.8 : propertySigHeightDefault * propertyPDFHeightScaleFactor * 0.4
                    fillMode: Image.PreserveAspectFit
                    anchors.top: sigReasonText.bottom
                    anchors.topMargin: 2
                    x: 2
                }
                Image {
                    id: dragSigImage
                    height: propertySigHeightDefault * propertyPDFHeightScaleFactor * 0.3
                    fillMode: Image.PreserveAspectFit
                    anchors.bottom: dragSigRect.bottom
                    cache: false
                    x: 2
                    Rectangle {
                        color: "white"
                        height: parent.height
                        width: parent.width
                        anchors.fill: parent
                        z: parent.z - 1
                    }
                }
                Text {
                    id: sigSignedByText
                    font.pixelSize: propertySigFontSizeBig
                    height: font.pixelSize + Constants.SIZE_SIGN_SEAL_TEXT_V_SPACE
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigReasonText.bottom
                    clip: true
                    text: ""
                    x: 2
                }
                Text {
                    id: sigSignedByNameText
                    font.pixelSize: propertySigFontSizeBig
                    font.family: myriad.name
                    font.bold: true
                    width: parent.width
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigReasonText.bottom
                    anchors.left: sigSignedByText.right
                    clip: true
                    text: ""
                    x: 2
                }
                Text {
                    id: sigNumIdText
                    font.pixelSize: !sigCertifiedByText.visible ? propertySigFontSizeBig : propertySigFontSizeSmall
                    height: font.pixelSize + Constants.SIZE_SIGN_SEAL_TEXT_V_SPACE
                    width: parent.width - 4
                    clip: true
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigSignedByNameText.bottom
                    text: ""
                    x: 2
                }
                Text {
                    id: sigDateText
                    font.pixelSize: !sigCertifiedByText.visible ? propertySigFontSizeBig : propertySigFontSizeSmall
                    height: font.pixelSize + Constants.SIZE_SIGN_SEAL_TEXT_V_SPACE
                    width: parent.width - 4
                    clip: true
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigNumIdText.visible ? sigNumIdText.bottom : sigSignedByNameText.bottom
                    text: qsTranslate("PageServicesSign", "STR_SIGN_DATE") + ": " + getData()
                    x: 2
                }
                Text {
                    id: sigLocationText
                    font.pixelSize: !sigCertifiedByText.visible ? propertySigFontSizeBig : propertySigFontSizeSmall
                    height: font.pixelSize + Constants.SIZE_SIGN_SEAL_TEXT_V_SPACE
                    width: parent.width - 4
                    clip: true
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigDateText.visible ? sigDateText.bottom : sigDateText.anchors.top
                    text: ""
                    x: 2
                }
                Text {
                    id: sigCertifiedByText
                    font.pixelSize: propertySigFontSizeSmall
                    visible: false
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigLocationText.text == "" ? sigLocationText.anchors.top : sigLocationText.bottom
                    clip: true
                    text: qsTranslate("PageServicesSign","STR_SCAP_CERTIFIED_BY")
                    x: 2
                }
                Text {
                    id: sigAttributesText
                    font.pixelSize: propertyCurrentAttrsFontSize * propertyPDFHeightScaleFactor
                    lineHeight: 0.8 // smaller line spacing to match real seal
                    visible: false
                    font.family: myriad.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigCertifiedByText.bottom
                    anchors.bottom: parent.bottom
                    clip: true
                    text: qsTranslate("PageServicesSign","STR_SCAP_CERTIFIED_ATTRIBUTES")
                    x: 2
                }

                MouseArea {
                    id: dragArea
                    width: dragSigRect.width + dragSigMoveImage.width * 0.5
                    height: dragSigRect.height + dragSigMoveImage.height * 0.5
                    onReleased: parent.Drag.drop()
                    drag.target: parent
                    drag.axis: Drag.XAndYAxis
                    drag.minimumX: 0
                    drag.maximumX: background_image.width - dragSigRect.width
                    drag.minimumY: 0
                    drag.maximumY: background_image.height - dragSigRect.height
                }
                onHeightChanged: {
                    if(dragSigRect.x === 0 && dragSigRect.y === 0){
                        dragTarget.coord_x = 0
                        dragTarget.coord_y = dragSigRect.height / background_image.height
                    }else{
                        dragTarget.coord_x = (dragSigRect.x) / background_image.width
                        dragTarget.coord_y = (dragSigRect.y + dragSigRect.height) / background_image.height
                    }
                    if((dragSigRect.x + dragSigRect.width) > background_image.width
                            || (dragSigRect.y + dragSigRect.height) > background_image.height){
                        dragSigRect.x = 0
                        dragSigRect.y = 0
                    }

                    if(propertyReducedChecked){
                        propertySigLineHeight = propertyDragSigRect.height * 0.2
                        dragSigImage.height = 0
                    }else{
                        propertySigLineHeight = propertyDragSigRect.height * 0.1
                        dragSigImage.height = propertySigHeightDefault * propertyPDFHeightScaleFactor * 0.3
                    }
                }
            }

            Image {
                id: dragSigMoveImage
                height: dragSigRect.height * 0.5
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: dragSigRect.bottom
                anchors.horizontalCenter: dragSigRect.left

                visible: fileLoaded
                source: "qrc:/images/icons-move.png"
            }

            Image {
                id: dragSigResizeImage
                height: dragSigRect.height * 0.5
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: dragSigRect.bottom
                anchors.horizontalCenter: dragSigRect.right

                visible: fileLoaded
                source: "qrc:/images/icons-resize.png"
            }

            onWidthChanged: {
                dragSigRect.x = dragTarget.lastCoord_x / dragTarget.lastScreenWidth * background_image.width
                dragSigRect.y = dragTarget.lastCoord_y / dragTarget.lastScreenHeight * background_image.height

                updateSignPreviewSize()

                propertyPageLoader.propertyBackupBackgroundWidth = background_image.width
                propertyPageLoader.propertyBackupBackgroundHeight = background_image.height
            }
        }
    }

    function updateSignPreviewSize() {
        if (dragTarget.lastScreenWidth != 0 && dragTarget.lastScreenHeight != 0 && background_image.width != 0 && background_image.height != 0) {
            
            dragSigRect.width = dragSigRect.width / dragTarget.lastScreenWidth* background_image.width
            dragSigRect.height = dragSigRect.height / dragTarget.lastScreenHeight * background_image.height
        }
        console.log("Height: " + dragSigRect.height + "Width: " + dragSigRect.width)
/*
        if (dragSigRect.height < (propertySigHeightMin) * propertyPDFHeightScaleFactor) {
            dragSigRect.height = (propertySigHeightMin) * propertyPDFHeightScaleFactor
        }

        if (dragSigRect.width < (propertySigWidthMin) * propertyPDFWidthScaleFactor) {
            dragSigRect.width = (propertySigWidthMin) * propertyPDFWidthScaleFactor
        }

        if (dragSigRect.height == 0 || dragSigRect.width == 0) {
            dragSigRect.height = (propertySigHeightDefault) * propertyPDFHeightScaleFactor
            dragSigRect.width = (propertySigHeightDefault) * propertyPDFWidthScaleFactor
        }
*/
        if(propertyReducedChecked){
            dragSigImage.height = 0
        }else{
            dragSigImage.height = dragSigImage.height / dragTarget.lastScreenHeight * background_image.height
        }
    }

    function updateSignPreview(){

        dragTarget.coord_x = (dragSigRect.x) / background_image.width
        dragTarget.coord_y = (dragSigRect.y + dragSigRect.height) / background_image.height
        dragTarget.lastCoord_x = dragSigRect.x
        dragTarget.lastCoord_y = dragSigRect.y
        dragTarget.lastScreenWidth = background_image.width
        dragTarget.lastScreenHeight = background_image.height
    }
    function setSignPreview(droped_x,droped_y){

        dragSigRect.x = droped_x
        dragSigRect.y = droped_y
    }

    function moveUp(positions) {
        var x = dragSigRect.x
        var y = dragSigRect.y

        var outOfBounds = y - positions < 0
        var diff = outOfBounds ? Math.abs(Math.floor(0 - y)) : positions
        if (diff === 0 || y === 0) {
            return
        }

        y = y - diff
        setSignPreview(x, y)
        updateSignPreview()
    }

    function moveDown(positions) {
        var pos = dragSigRect.y + dragSigRect.height
        var outOfBounds = pos >= background_image.height

        // move down number of positions or the difference between current position and bottom of the page
        var diff = outOfBounds ? 0 : Math.min(positions, Math.floor(background_image.height - pos))
        
        // nothing to update return
        if (diff === 0) {
            return
        }

        var x = dragSigRect.x
        var y = dragSigRect.y + diff
        setSignPreview(x, y)
        updateSignPreview()
    }

    function moveLeft(positions) {
        var x = dragSigRect.x
        var y = dragSigRect.y
        var outOfBounds = x - positions < 0
        var diff = outOfBounds ? Math.abs(Math.floor(0 - x)) : positions

        if (diff === 0 || x === 0) {
            return
        }

        x = x - diff
        setSignPreview(x, y)
        updateSignPreview()
    }

    function moveRight(positions) {
        var pos = dragSigRect.x + dragSigRect.width

        var outOfBounds = pos >= background_image.width
        
        // move to the right positions or difference between current position and right side of a page
        var diff = outOfBounds ? 0 : Math.min(positions, Math.floor(background_image.width - pos))
        
        if (diff === 0) {
            return
        }

        var x = dragSigRect.x + diff
        var y = dragSigRect.y
        setSignPreview(x, y)
        updateSignPreview()
    }

    // this is a hack to make screen reader say x and y positions
    // after the user changed the position of the signature seal.
    // Other approaches included using a Slider Component
    // or Text component with Accessible.role: Accessible.Indicator however the screen reader
    // does not automatically utter the change so we had to fallback to change the focus with a Timer to utter the position
    function toggleFocus() {
        dummyText.forceActiveFocus()
        console.log("Signature Preview: " + positionText.text)
    }
    Timer {
        id: delayFocusPositionText
        interval: 20
        repeat: false
        running: false
        onTriggered: {
            positionText.forceActiveFocus()
        }
    }
    Text {
        id: dummyText
        text: ""
        visible: false
        Accessible.name: text
        onFocusChanged: if (activeFocus) delayFocusPositionText.start()
    }

    function getData(){
        var time = Qt.formatDateTime(new Date(), "yyyy.MM.dd hh:mm:ss")

        function pad(number, length){
            var str = "" + number
            while (str.length < length) {
                str = '0'+str
            }
            return str
        }

        var offset = new Date().getTimezoneOffset()
        offset = ((offset<0? '+':'-')+ // Note the reversed sign!
                  pad(parseInt(Math.abs(offset/60)), 2)+
                  pad(Math.abs(offset%60), 2))

        time += " " + offset

        return time
    }
}
