import QtQuick 2.0
import QtQuick.Window 2.2

/* Constants imports */
import "../scripts/Constants.js" as Constants

Rectangle {
        id: pdfPreview

        property alias propertyBackground: background_image
        property alias propertyDragImage: dragImage
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

                Image {
                    id: dragImage
                    width: (propertySigWidth) * propertyConvertPixelToPts * background_image.width
                           / (propertyPdfOriginalWidth / propertyConvertPixelToPts)
                    height: (propertySigHidth) * propertyConvertPixelToPts * background_image.height
                            / (propertyPdfOriginalHeight / propertyConvertPixelToPts)

                    Drag.active: dragArea.drag.active

                    MouseArea {
                        id: dragArea
                        anchors.fill: parent
                        onReleased: parent.Drag.drop()
                        drag.target: parent
                        drag.axis: Drag.XAndYAxis
                        drag.minimumX: 0
                        drag.maximumX: background_image.width - dragImage.width
                        drag.minimumY: 0
                        drag.maximumY: background_image.height - dragImage.height
                    }
                    onHeightChanged: {
                        if(dragImage.x === 0 && dragImage.y === 0){
                            dragTarget.coord_x = 0
                            dragTarget.coord_y = dragImage.height / background_image.height
                        }else{
                            dragTarget.coord_x = (dragImage.x) / background_image.width
                            dragTarget.coord_y = (dragImage.y + dragImage.height) / background_image.height
                        }
                        if((dragImage.x + dragImage.width) > background_image.width
                                || (dragImage.y + dragImage.height) > background_image.height){
                            dragImage.x = 0
                            dragImage.y = 0
                        }
                    }
                }
                onWidthChanged: {
                    dragImage.x = dragTarget.lastCoord_x / dragTarget.lastScreenWidth * background_image.width
                    dragImage.y = dragTarget.lastCoord_y / dragTarget.lastScreenHeight * background_image.height
                }
            }

            onDropped: {
                coord_x = (drop.x - background_image.x) / background_image.width
                coord_y = (drop.y + dragImage.height - background_image.y) / background_image.height
                dragTarget.lastCoord_x = dragImage.x
                dragTarget.lastCoord_y = dragImage.y
                dragTarget.lastScreenWidth = background_image.width
                dragTarget.lastScreenHeight = background_image.height
            }
        }
}
