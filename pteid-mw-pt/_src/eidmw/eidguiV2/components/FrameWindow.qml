import QtQuick 2.6
import QtQuick.Window 2.2

/* Constants imports */
import "../scripts/Constants.js" as Constants

Item {
    id: frameWindow

    property alias propertyMouseRegion: mouseRegion
    property string propertySide: ""

    Item{
        id: containerMouseMovCalc
        width: parent.width ; height: parent.height
        MouseArea {
            id: mouseRegion
            anchors.fill: parent

            property variant lastPos: ""

            cursorShape: Qt.SizeVerCursor

            onPressed: lastPos = controler.getCursorPos()

            onPositionChanged: {

                var newPos = controler.getCursorPos()
                var delta = Qt.point(newPos.x-lastPos.x, newPos.y-lastPos.y)

                switch(propertySide) {
                case "LEFT":
                    if(mainWindow.width-delta.x > Constants.SCREEN_MINIMUM_WIDTH){
                        console.log(delta)
                        mainWindow.width = mainWindow.width-delta.x
                        mainWindow.x =  mainWindow.x+delta.x
                    }else{
                        mainWindow.width = Constants.SCREEN_MINIMUM_WIDTH
                    }
                    break;
                case "RIGHT":
                    if(mainWindow.width+delta.x > Constants.SCREEN_MINIMUM_WIDTH){
                        mainWindow.width = mainWindow.width+delta.x
                    }else{
                        mainWindow.width = Constants.SCREEN_MINIMUM_WIDTH
                    }
                    break;
                case "BOTTOM":
                    if(mainWindow.height+delta.y > Constants.SCREEN_MINIMUM_HEIGHT){
                        mainWindow.height = mainWindow.height+delta.y
                    }else{
                        mainWindow.height = Constants.SCREEN_MINIMUM_HEIGHT
                    }
                    break;
                default:
                    break;
                }
                lastPos = newPos;
            }
        }
    }
}
