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

    property real propertySigLineHeight: dragSigRect.height * 0.1
    property bool propertyReducedChecked: false

    property alias propertyCoordX: dragTarget.coord_x
    property alias propertyCoordY: dragTarget.coord_y

    // Properties used to convert to postscript points (1 px == 0.75 points)
    // Signature have a static size
    property real propertyConvertPixelToPts: 1 / 0.75
    property real propertySigWidth: 178
    property real propertySigHidth: 90

    //Properties to store Pdf original size
    property real propertyPdfOriginalWidth: 0
    property real propertyPdfOriginalHeight: 0

    property real stepSizeX : 5.0
    property real stepSizeY : 10.0

    property bool sealHasChanged: false

    color: Constants.COLOR_MAIN_SOFT_GRAY

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

        Keys.onPressed: {
            if (pdfPreview.focus === false) {
                pdfPreview.forceActiveFocus()
            }
        }
    }

    Accessible.role: Accessible.Canvas
    Accessible.name: !sealHasChanged ? qsTranslate("PageServicesSign", "STR_SIGN_NAV_DESCRIPTION") : ""
    
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


            Rectangle {
                width: dragSigRect.width + 2 * Constants.FOCUS_BORDER
                height: dragSigRect.height + 2 * Constants.FOCUS_BORDER
                x: dragSigRect.x - Constants.FOCUS_BORDER
                y: dragSigRect.y - Constants.FOCUS_BORDER
                border.width: Constants.FOCUS_BORDER
                border.color: pdfPreview.activeFocus || positionText.activeFocus ? Constants.COLOR_MAIN_DARK_GRAY
                             : Constants.COLOR_GREY_BUTTON_BACKGROUND
                opacity: 0.7
                visible: width >= Constants.FOCUS_BORDER ? true : false
            }
            Item {
                id: dragSigRect
                width: (propertySigWidth) * propertyConvertPixelToPts * background_image.width
                       / (propertyPdfOriginalWidth / propertyConvertPixelToPts)
                height: (propertySigHidth) * propertyConvertPixelToPts * background_image.height
                        / (propertyPdfOriginalHeight / propertyConvertPixelToPts)

                Drag.active: dragArea.drag.active

                Text {
                    id: sigReasonText
                    font.pixelSize: propertySigLineHeight
                    font.italic: true
                    height: propertySigLineHeight
                    width: parent.width - 4
                    clip: true
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    text: ""
                    anchors.top: dragSigRect.top
                    anchors.topMargin: propertySigLineHeight * 0.60
                    x: 2
                    visible: false
                }

                Image {
                    id: dragSigWaterImage
                    height: dragSigRect.height * 0.4
                    fillMode: Image.PreserveAspectFit
                    anchors.top: sigReasonText.bottom
                    anchors.topMargin: 2
                    x: 2
                    visible: false
                }
                Text {
                    id: sigSignedByText
                    font.pixelSize:propertySigLineHeight
                    height: propertySigLineHeight
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigReasonText.bottom
                    text: ""
                    x: 2
                    visible: false
                }
                Text {
                    id: sigSignedByNameText
                    font.pixelSize: propertySigLineHeight
                    height: propertySigLineHeight
                    width: parent.width - sigSignedByText.paintedWidth - 6
                    clip: true
                    font.family: lato.name
                    font.bold: true
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigReasonText.bottom
                    anchors.left: sigSignedByText.right
                    text: ""
                    x: 2
                    visible: false
                }
                Text {
                    id: sigNumIdText
                    font.pixelSize: propertySigLineHeight
                    height: propertySigLineHeight
                    width: parent.width - 4
                    clip: true
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigSignedByText.bottom
                    text: ""
                    x: 2
                    visible: false
                }
                Text {
                    id: sigDateText
                    font.pixelSize: propertySigLineHeight
                    height: propertySigLineHeight
                    width: parent.width - 4
                    clip: true
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigNumIdText.bottom
                    text: getData()
                    x: 2
                    visible: false
                }
                Text {
                    id: sigLocationText
                    font.pixelSize: propertySigLineHeight
                    height: propertySigLineHeight
                    width: parent.width - 4
                    clip: true
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_BODY
                    anchors.top: sigDateText.bottom
                    text: ""
                    x: 2
                    visible: false
                }

                Image {
                    id: dragSigImage
                    height: dragSigRect.height * 0.3
                    fillMode: Image.PreserveAspectFit
                    anchors.top: sigLocationText.bottom
                    anchors.topMargin: parent.height * 0.1
                    x: 2
                    visible: false
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
                        propertyDragSigImg.height = 0
                        propertyDragSigWaterImg.height = 0
                    }else{
                        propertySigLineHeight = propertyDragSigRect.height * 0.1
                        propertyDragSigReasonText.height = propertySigLineHeight
                        propertyDragSigLocationText.height = propertySigLineHeight
                        propertyDragSigImg.height = propertyDragSigRect.height * 0.3
                        propertyDragSigWaterImg.height = propertyDragSigRect.height * 0.4
                    }
                }
            }
            Image {
                id: dragSigMoveImage
                height: dragSigRect.height * 0.5
                fillMode: Image.PreserveAspectFit
                anchors.top: dragSigRect.bottom
                anchors.topMargin: -dragSigMoveImage.height * 0.5
                anchors.left: dragSigRect.left
                anchors.leftMargin: -dragSigMoveImage.width * 0.5

                visible: dragSigRect.visible
                source: "qrc:/images/icons-move.png"
            }

            onWidthChanged: {
                dragSigRect.x = dragTarget.lastCoord_x / dragTarget.lastScreenWidth * background_image.width
                dragSigRect.y = dragTarget.lastCoord_y / dragTarget.lastScreenHeight * background_image.height
            }
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
    // does not automatically utter the change so we had to fallback to change the focus to utter the position
    function toggleFocus() {
        if (!sealHasChanged) {
            sealHasChanged = true
        }
        if (pdfPreview.focus === true) {
            positionText.forceActiveFocus()
            console.log("Signature Preview: " + positionText.text)
        } else {
            pdfPreview.forceActiveFocus()
        }
    }

    function getData(){
        var time = Qt.formatDateTime(new Date(), "yy.MM.dd hh:mm:ss")

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
