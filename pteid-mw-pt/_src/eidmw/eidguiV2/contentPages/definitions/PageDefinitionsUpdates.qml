import QtQuick 2.6
import QtQuick.Controls 2.1

//Import C++ defined enums
import eidguiV2 1.0

PageDefinitionsUpdatesForm {
    Connections {
        target: controler
        onSignalAutoUpdateFail: {
            console.log("onSignalAutoUpdateFail")
            if (error_code == GAPI.GenericError) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_ERROR")
            }else if (error_code == GAPI.NoUpdatesAvailable) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NO_UPDATES")
            }else if (error_code == GAPI.LinuxNotSupported) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_LINUX_NOT_SUPPORTED")
            }else if (error_code == GAPI.UnableSaveFile) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_SAVE_FILE")
            }else if (error_code == GAPI.DownloadFailed) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_DOWNLOAD_FAIL")
            }
            propertyProgressBar.indeterminate = false
            propertyProgressBar.visible = false
            propertyButtonSearch.visible = true
            propertyButtonStartUpdate.visible = false
        }
        onSignalAutoUpdateProgress: {
            propertyProgressBar.indeterminate = false
            propertyProgressBar.visible = true
            propertyProgressBar.value = value
        }
        onSignalAutoUpdateAvailable: {
            propertyTextDescription.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_AVAILABLE")
            propertyButtonSearch.visible = true
            propertyButtonStartUpdate.visible = true
            propertyProgressBar.visible = false
        }
        onSignalStartUpdate: {
            propertyTextDescription.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_STARTING_DOWNLOAD") + " " + filename
        }
    }
    propertyButtonSearch {
        onClicked: {
            console.log("propertyButtonSearch clicked")
            propertyButtonStartUpdate.visible = false
            propertyProgressBar.indeterminate = true
            propertyProgressBar.visible = true
            propertyButtonSearch.visible = false
            propertyTextDescription.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
            controler.autoUpdates()
        }
    }
    propertyButtonStartUpdate {
        onClicked: {
            console.log("propertyButtonStartUpdate clicked")
            propertyProgressBar.visible = true
            propertyProgressBar.indeterminate = false
            propertyButtonSearch.visible = false
            controler.startUpdate()
        }
    }
}
