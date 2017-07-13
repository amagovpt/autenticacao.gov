import QtQuick 2.0
import QtQuick.Window 2.2

Rectangle {
        property alias propertyBackground: background_image
        property alias propertyDragImage: drag_image
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
                fillMode: Image.PreserveAspectFit
            }

            onDropped: {
                console.info("onDropped: X: " +drop.x + " Y:" +drop.y)
                coord_x = drop.x
                coord_y = drop.y
            }
        }
        Image {
            width: 53
            height: 42
            id: drag_image
            Drag.active: dragArea.drag.active

            MouseArea {
                id: dragArea
                anchors.fill: parent
                onReleased: parent.Drag.drop()
                drag.target: parent
                drag.axis: Drag.XAndYAxis
                drag.minimumX: 0
                drag.maximumX: background_image.width - drag_image.width
                drag.minimumY: 0
                drag.maximumY: background_image.height - drag_image.height
            }
        }

}
