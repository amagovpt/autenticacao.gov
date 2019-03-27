import QtQuick 2.6

PageHelpAboutForm {
    Connections {
        target: controler
    }
    Component.onCompleted: {
        propertyTextVersion.text = qsTranslate("PageHelpAbout","STR_HELP_APP_VERSION") + " : "
                + controler.getAppVersion()
    }
}
