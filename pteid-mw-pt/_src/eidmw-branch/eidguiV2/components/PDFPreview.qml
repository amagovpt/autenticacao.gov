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

    color: Constants.COLOR_MAIN_SOFT_GRAY

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
                width: dragSigRect.width
                height: dragSigRect.height
                x: dragSigRect.x
                y: dragSigRect.y
                border.width : 1
                border.color: Constants.COLOR_MAIN_DARK_GRAY
                opacity: 0.7
                visible: dragSigRect.visible
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
                anchors.left: dragSigRect.right
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
    function updateSignPreview(droped_x,droped_y){

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
