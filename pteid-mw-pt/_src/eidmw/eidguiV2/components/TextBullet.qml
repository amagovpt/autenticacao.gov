import QtQuick 2.6

import "../scripts/Constants.js" as Constants

Item {
    width: 2 * Constants.SIZE_TEXT_LIST_BULLETD
    height: Constants.SIZE_TEXT_LIST_BULLETD
    Rectangle {
        width: Constants.SIZE_TEXT_LIST_BULLETD
        height: Constants.SIZE_TEXT_LIST_BULLETD
        color: Constants.COLOR_TEXT_BODY
        radius: width * 0.5
    }
}
