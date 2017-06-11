import QtQuick 2.6

PageHelpAboutForm {
    propertyTextLinkCC.onLinkActivated: {
        Qt.openUrlExternally(link)
    }
}
