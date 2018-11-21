import QtQuick 2.6
import QtQuick.Controls 2.1

import "scripts/Constants.js" as Constants

Loader{
    id: theloader
    opacity:1
    width: parent.width
    height: parent.height

    property alias propertyGeneralPopUp: generalPopUp
    property alias propertyRectPopUp: rectPopUp
    property alias propertyGeneralTitleText: titleText
    property alias propertyGeneralPopUpLabelText: labelText

    // Backup data about unfinished advance signature
    property alias propertyBackupfilesModel: backupfilesModel
    property bool propertyAnimationExtendedFinished: false
    property bool propertyBackupFormatPades: true
    property bool propertyBackupTempSign: false
    property bool propertyBackupSignAdd: false
    property variant attributeListBackup: []
    property bool propertyBackupSignShow: true
    property double propertyBackupCoordX: 0
    property double propertyBackupCoordY: 0
    property bool propertyBackupSignReduced: false
    property int propertyBackupPage: 1
    property bool propertyBackupLastPage: false
    property string propertyBackupLocal: ""
    property string propertyBackupReason: ""
    property string propertyBackupMobileNumber: ""
    property int propertyBackupMobileIndicatorIndex: 0
	property bool propertyBackupFromSignaturePage: false

    ListModel {
        id: backupfilesModel
    }

    Dialog {
        id: generalPopUp
        width: 400
        height: 200
        visible: false

        font.family: lato.name
        // Center dialog in the main view
        x: - mainMenuView.width - subMenuView.width
           + mainView.width * 0.5 - generalPopUp.width * 0.5
        y: parent.height * 0.5 - generalPopUp.height * 0.5

        header: Label {
            id: titleText
            text: "Error"
            elide: Label.ElideRight
            padding: 24
            bottomPadding: 0
            font.bold: true
            font.pixelSize: Constants.SIZE_TEXT_MAIN_MENU
            color: Constants.COLOR_MAIN_BLUE
        }
        Item {
            id: rectPopUp
            width: generalPopUp.availableWidth
            height: generalPopUp.availableHeight - generalPopUp.padding

            Keys.enabled: true
            Keys.onPressed: {
                  if(event.key===Qt.Key_Enter || event.key===Qt.Key_Return)
                  {
                      generalPopUp.close()
                  }
            }

            Item {
                id: rectLabelText
                width: parent.width
                height: parent.height - Constants.HEIGHT_BOTTOM_COMPONENT - Constants.SIZE_TEXT_V_SPACE
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: labelText
                    verticalAlignment: Text.AlignVCenter
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: Constants.SIZE_TEXT_LABEL
                    font.family: lato.name
                    color: Constants.COLOR_TEXT_LABEL
                    height: parent.height
                    width: parent.width
                    anchors.bottom: parent.bottom
                    wrapMode: Text.Wrap
                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                }
            }
            Button {
                width: Constants.WIDTH_BUTTON
                height: Constants.HEIGHT_BOTTOM_COMPONENT
                text: "OK"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: rectLabelText.bottom
                font.pixelSize: Constants.SIZE_TEXT_FIELD
                font.family: lato.name
                font.capitalization: Font.MixedCase
                onClicked: generalPopUp.close()
            }
        }
    }

    anchors{
        horizontalCenter: parent.horizontalCenter
        verticalCenter: parent.verticalCenter
    }
}
