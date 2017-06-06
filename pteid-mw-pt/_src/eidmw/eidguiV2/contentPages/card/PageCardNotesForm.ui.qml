import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

/* Constants imports */
import "../../scripts/Constants.js" as Constants
import "../../components" as Components


Item {
    anchors.fill: parent

    property alias propertyFlickNotes: flick
    property alias propertyEditNotes: edit

    Item {
        id: rowTop
        width: parent.width - Constants.WIDTH_PHOTO_IMAGE - 2 * Constants.SIZE_ROW_H_SPACE
        height: parent.height * Constants.SIZE_ROW_TOP_V_RELATIVE

        Item{
            id: rectTopLeft
            width: parent.width
            height: parent.height
        }
        Item{
            id: rectToRight
            width: Constants.WIDTH_PHOTO_IMAGE + Constants.SIZE_ROW_H_SPACE
            height: parent.height
            anchors.left: rectTopLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE

            Image {
                anchors.bottom: parent.bottom
                width: parent.width
                antialiasing: true
                fillMode: Image.PreserveAspectFit
                source: "../../images/logo_cartao_cidadao.png"
            }
        }
    }

    Item {
        id: rowText
        width: parent.width
        height: parent.height * Constants.PAGE_NOTES_TEXT_V_RELATIVE
        anchors.top: rowTop.bottom
        anchors.topMargin: Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectTextLeft
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * Constants.PAGE_NOTES_LEFT_RELATIVE_SIZE
            height: parent.height
            Text {
                id: notesText
                text: "Notas"
                x: Constants.SIZE_TEXT_FIELD_H_SPACE
                font.pixelSize: Constants.SIZE_TEXT_LABEL
                font.family: lato.name
                color: Constants.COLOR_TEXT_LABEL
                height: Constants.SIZE_TEXT_LABEL
            }
            DropShadow {
                anchors.fill: rectFieldFlick
                horizontalOffset: Constants.FORM_SHADOW_H_OFFSET
                verticalOffset: Constants.FORM_SHADOW_V_OFFSET
                radius: Constants.FORM_SHADOW_RADIUS
                samples: Constants.FORM_SHADOW_SAMPLES
                color: Constants.COLOR_FORM_SHADOW
                source: rectFieldFlick
            }
            RectangularGlow {
                id: effectGlowFlick
                anchors.fill: rectFieldFlick
                glowRadius: Constants.FORM_GROW_RADIUS
                spread: Constants.FORM_GLOW_SPREAD
                color: Constants.COLOR_FORM_GLOW
                cornerRadius: Constants.FORM_GLOW_CORNER_RADIUS
            }
            Rectangle {
                id: rectFieldFlick
                width: parent.width
                height: parent.height - notesText.height - Constants.SIZE_ROW_V_SPACE
                color: "white"
                anchors.top :notesText.bottom
                anchors.topMargin: Constants.SIZE_TEXT_V_SPACE
                opacity: 1
                Flickable {
                    id: flick
                    x: Constants.SIZE_TEXT_FIELD_H_SPACE
                    y: Constants.SIZE_TEXT_FIELD_V_SPACE
                    width: parent.width - 2 * Constants.SIZE_TEXT_FIELD_H_SPACE
                    height: parent.height - 2 * Constants.SIZE_TEXT_FIELD_V_SPACE
                    boundsBehavior: Flickable.StopAtBounds
                    maximumFlickVelocity: 2500
                    flickableDirection: Flickable.VerticalFlick
                    contentWidth: edit.paintedWidth
                    contentHeight: edit.paintedHeight
                    clip: true

                    TextEdit {
                        id: edit
                        property string previousText: text
                        width: flick.width
                        height: flick.height
                        focus: true
                        wrapMode: TextEdit.Wrap
                        font.pixelSize: Constants.SIZE_TEXT_FIELD
                        color: Constants.COLOR_TEXT_BODY
                    }
                }
            }
        }
        Item{
            id: rectTextRight
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * Constants.PAGE_NOTES_RIGHT_RELATIVE_SIZE
            height: parent.height - notesText.height -  Constants.SIZE_TEXT_V_SPACE -  Constants.SIZE_TEXT_V_SPACE
            anchors.left: rectTextLeft.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            anchors.bottom: rectTextLeft.bottom

            // Attach scrollbars to the right and bottom edges of the view.
            Components.ScrollBar {
                id: verticalScrollBar
                property alias flickable: flick
                width: 12;
                height: parent.height
                y:  - Constants.SIZE_TEXT_V_SPACE
                opacity: 1
                orientation: Qt.Vertical
                position: flick.visibleArea.yPosition
                pageSize: flick.visibleArea.heightRatio
                propertyBackground.color: Constants.COLOR_MAIN_BLUE
                propertyBar.color: Constants.COLOR_MAIN_DARK_GRAY
                propertyBackground.radius: 0
                propertyBar.radius: 0
            }
        }
    }

    Item {
        id: rowNotesCount
        width: parent.width
        height: Constants.SIZE_TEXT_LABEL
                + Constants.SIZE_TEXT_V_SPACE
                + 2 * Constants.SIZE_TEXT_FIELD
        anchors.top: rowText.bottom
        anchors.topMargin: 2 * Constants.SIZE_ROW_V_SPACE

        Item{
            id: rectNotesCount
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            Components.LabelTextBoxForm{
                propertyDateText.text: "Bytes Disponíveis"
                propertyDateField.text: Constants.PAGE_NOTES_MAX_NOTES_LENGHT - edit.length + " / "
                                        + Constants.PAGE_NOTES_MAX_NOTES_LENGHT
            }
        }
        Item{
            id: rectNotesComfirm
            width: (parent.width - 1 * Constants.SIZE_ROW_H_SPACE ) * 0.50
            height: parent.height
            anchors.left: rectNotesCount.right
            anchors.leftMargin: Constants.SIZE_ROW_H_SPACE
            Components.Button {
                id: button
                buttonText: "Gravar Notas"
                width: parent.width
                height: parent.height
                color: Constants.COLOR_MAIN_SOFT_GRAY
                hoverColor: Constants.COLOR_MAIN_DARK_GRAY
                pressColor: Constants.COLOR_MAIN_BLUE
            }
        }
    }
}
