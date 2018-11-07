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
            }else if (error_code == GAPI.DownloadCancelled) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_TEXT")
            }else if (error_code == GAPI.InstallFailed) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_INSTALL_FAIL") + "\n\n" + qsTranslate("PageDefinitionsUpdates","STR_CONTACT_SUPPORT")
            }else if (error_code == GAPI.NetworkError) {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NETWORK_ERROR")
            }
            propertyProgressBar.visible = false
            propertyProgressBar.value = 0
            propertyProgressBar.indeterminate = false
            propertyButtonSearch.visible = true
            propertyButtonStartUpdate.visible = false
            propertyButtonCancelUpdate.visible = false
            propertyReleaseNoteScrollView.visible = false
            propertyReleaseScrollViewText.visible = false
            propertyInstalledVersion.visible = false
            propertyRemoteVersion.visible = false
        }
        onSignalAutoUpdateProgress: {
            propertyProgressBar.indeterminate = false
            propertyProgressBar.visible = true
            propertyProgressBar.value = value
            if (propertyButtonCancelUpdate.visible == false) {
                propertyButtonCancelUpdate.visible = true
            }
        }
        onSignalAutoUpdateAvailable: {
            //make sure we have something to display
            if (release_notes != ""){
                propertyReleaseNoteScrollView.visible = true
                propertyReleaseScrollViewText.text = release_notes
                propertyReleaseScrollViewText.visible = true
                propertyTextDescription.text = qsTranslate("PageDefinitionsUpdates","STR_UPDATE_AVAILABLE")
                propertyInstalledVersion.propertyDateField.text = installed_version
                propertyRemoteVersion.propertyDateField.text = remote_version

                propertyInstalledVersion.visible = true
                propertyRemoteVersion.visible = true

                propertyButtonSearch.visible = false
                propertyButtonStartUpdate.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_BUTTON_START") + ' ' +
                        qsTranslate("PageDefinitionsUpdates","STR_VERSION") + ' ' + remote_version
                propertyButtonStartUpdate.visible = true

            } else {
                propertyTextDescription.text =
                        qsTranslate("PageDefinitionsUpdates","STR_UPDATE_NO_UPDATES")
                propertyButtonSearch.visible = true
            }
            propertyProgressBar.visible = false
            propertyProgressBar.value = 0
            propertyProgressBar.indeterminate = false
        }
        onSignalStartUpdate: {
            propertyReleaseNoteScrollView.visible = false
            propertyReleaseScrollViewText.visible = false
            propertyTextDescription.text =
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_STARTING_DOWNLOAD") + " " + filename

            propertyInstalledVersion.visible = false
            propertyRemoteVersion.visible = false
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
                    qsTranslate("PageDefinitionsUpdates","STR_UPDATE_VERSION_LOOKUP")
            controler.autoUpdates()
        }
    }
    propertyButtonStartUpdate {
        onClicked: {
            console.log("propertyButtonStartUpdate clicked")
            propertyProgressBar.visible = true
            propertyProgressBar.indeterminate = false
            propertyButtonSearch.visible = false
            propertyButtonStartUpdate.visible = false //instant hide it after press guarantees that user never accidentally clicks two times
            controler.startUpdate()
        }
    }
    propertyButtonCancelUpdate {
        onClicked: {
            console.log("propertyButtonCancelUpdate clicked")
            propertyProgressBar.visible = false
            propertyButtonSearch.visible = true
            controler.userCancelledUpdateDownload()
        }
    }
}
