import QtQuick 2.0
import QtQuick.Window 2.2

Rectangle {
        property alias propertyBackground: background_image
        property alias propertyDragImage: dragImage
        property alias propertyCoordX: dragTarget.coord_x
        property alias propertyCoordY: dragTarget.coord_y
        id: pdfPreview

        //color: "white"
        DropArea {
            id: dragTarget
            width: parent.width
            height: parent.height

            //Properties to store current signature position
            property real coord_x
            property real coord_y

            Image {
                id: background_image
                x: 0
                y: 0
                sourceSize.width: dragTarget.width
                sourceSize.height: dragTarget.height
                fillMode: Image.PreserveAspectFit
            }

            onDropped: {
                console.info("onDropped: X: " +drop.x + " Y:" +drop.y)
                coord_x = drop.x
                coord_y = drop.y
            }
        }
        Image {
            id: dragImage
            width: 0.29715 * background_image.width
            height: 0.1 *background_image.height

            x: background_image.width < dragImage.x ? 0 : dragImage.x
            y: background_image.height < dragImage.y ? 0: dragImage.y
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
        }

}
