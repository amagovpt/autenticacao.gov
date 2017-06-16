/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.6
import QtQuick.Window 2.1

/* Constants imports */
import "scripts/Constants.js" as Constants

Item {
    id: titleBar

    Rectangle {
        id: container
        width: parent.width ; height: parent.height
        color: Constants.COLOR_MAIN_BLUE
        Text {
            id: categoryText
            x: 10
            anchors {
                verticalCenter: parent.verticalCenter
            }
            elide: Text.ElideLeft
            text: mainWindow.title
            font.pixelSize: Constants.SIZE_TEXT_BODY
            font.family: lato.name
            color: "white"
            height: Constants.SIZE_TEXT_LABEL
        }
        Image {
            id: minimizeButton
            width: Constants.TITLE_BAR_SIZE_ICON
            height: Constants.TITLE_BAR_SIZE_ICON
            anchors {
                right: fullScreentButton.left; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            source: "images/minimize.png"
            MouseArea {
                anchors.fill: parent
                onClicked: mainWindow.showMinimized()
            }
        }
        Image {
            id: fullScreentButton
            width: Constants.TITLE_BAR_SIZE_ICON
            height: Constants.TITLE_BAR_SIZE_ICON
            anchors {
                right: quitButton.left; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            source: "images/maximize.png"

            MouseArea {
                anchors.fill: parent
                onClicked: getScreenState()
            }
        }
        Image {
            id: quitButton
            width: Constants.TITLE_BAR_SIZE_ICON
            height: Constants.TITLE_BAR_SIZE_ICON
            anchors {
                right: parent.right; leftMargin: 10; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            source: "images/quit.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Qt.quit()
            }
        }
        MouseArea {
            id: mouseRegion
            anchors.left: parent.left
            anchors.right: minimizeButton.left
            height: parent.height

            property variant clickPos: ""
            property variant appStartPos: ""

            onPressed: {
                // fetch global position
                clickPos = controler.getCursorPos()
                appStartPos = Qt.point(mainWindow.x, mainWindow.y)
            }
            onPositionChanged: {
                var newPos = controler.getCursorPos()
                var delta = Qt.point(newPos.x-clickPos.x, newPos.y-clickPos.y)
                mainWindow.x = appStartPos.x+delta.x;
                mainWindow.y = appStartPos.y+delta.y;
            }
        }

    }
    function getScreenState(){
        if(mainWindow.visibility === Window.FullScreen ){
            console.log("isFullScreen"+ mainWindow.visibility)
            mainWindow.showNormal()
        }else{
            console.log("not isFullScreen" + mainWindow.visibility)
            mainWindow.showFullScreen()
        }
    }
}
