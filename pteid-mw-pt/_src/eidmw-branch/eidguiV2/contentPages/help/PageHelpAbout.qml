import QtQuick 2.6

PageHelpAboutForm {
    Connections {
        target: controler
    }
    propertyTextLinkCC.onLinkActivated: {
        Qt.openUrlExternally(link)
    }
    Component.onCompleted: {
        propertyTextVersion.text = qsTranslate("PageHelpAbout","STR_HELP_APP_VERSION") + " : "
                + controler.getAppVersion()
    }
}
