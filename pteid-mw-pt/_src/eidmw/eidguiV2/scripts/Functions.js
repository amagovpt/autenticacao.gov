// Functions.js

function getMainMenuWidth(parentWidth) {
    var handColor

    switch (mainFormID.state) {
        case Constants.MenuState.FIRST_RUN:
            handColor = parentWidth * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            break;
        case Constants.MenuState.HOME:
            handColor = parentWidth
            break;
        case Constants.MenuState.EXPAND:
            handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
    }
    return handColor
}
function getSubMenuWidth(parentWidth) {
    var handColor

    switch (mainFormID.state) {
        case Constants.MenuState.FIRST_RUN:
            handColor = 0
            break;
        case Constants.MenuState.HOME:
            handColor = 0
            break;
        case Constants.MenuState.EXPAND:
            handColor = parentWidth * Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
    }
    return handColor
}
function getContentPagesMenuWidth(parentWidth) {
    var handColor

    switch (mainFormID.state) {
        case Constants.MenuState.FIRST_RUN:
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
            break;
        case Constants.MenuState.HOME:
            handColor = 0
            break;
        case Constants.MenuState.EXPAND:
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE +
                    parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE -
                    parentWidth * Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE
            break;
        default: //STATE_NORMAL
            handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
    }
    return handColor
}

function getIsVisibleSubMenuViewHorizontalLine(index) {
    var handVisible
    if (mainFormID.propertySubMenuListView.count - 1 === index ||
            mainFormID.propertySubMenuListView.currentIndex === index ||
            mainFormID.propertySubMenuListView.currentIndex - 1 === index) {
        handVisible = false
    } else {
        handVisible = true
    }
    return handVisible
}

function getSubNameColor(index, containsMouse) {
    var handColor
    if (mainFormID.propertySubMenuListView.currentIndex === index) {
        handColor = Constants.COLOR_TEXT_SUB_MENU_SELECTED
    } else {
        if (containsMouse === true) {
            handColor = Constants.COLOR_TEXT_SUB_MENU_MOUSE_OVER
        } else {
            handColor = Constants.COLOR_TEXT_SUB_MENU_DEFAULT
        }
    }
    return handColor
}

function getSubMenuArrowSource(index, containsMouse) {
    var handSource = Constants.ARROW_RIGHT;
    if (mainFormID.propertySubMenuListView.currentIndex === index) {
        handSource = Constants.ARROW_RIGHT;
    } else {
        if (containsMouse === true) {
            handSource = Constants.ARROW_RIGHT_HOVER;
        } else {
            handSource = "";
        }
    }

    return handSource
}

function getMainMenuArrowSource(index, containsMouse) {
    var handSource = Constants.ARROW_RIGHT;

    if (mainFormID.propertyMainMenuListView.currentIndex === index) {
        handSource = Constants.ARROW_RIGHT;
    } else {
        if (containsMouse === true) {
            handSource = Constants.ARROW_RIGHT_HOVER;
        } else {
            handSource = "";
        }
    }
    return handSource
}

function getBottomMenuImgSource(index, containsMouse) {
    var handSource = "images/help_icon.png"

    if (mainFormID.propertyMainMenuBottomListView.currentIndex === index) {
        handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
    } else {
        if (containsMouse === true) {
            handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
        } else {
            handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrl
        }
    }

    return handSource
}

function goToHome() {
    mainFormID.state = Constants.MenuState.HOME;
    mainFormID.propertySubMenuListView.currentIndex = -1;
    // Clear list model and then load a new sub menu
    mainFormID.propertyPageLoader.source = "";
    mainFormID.propertySubMenuListView.model.clear();

    if (mainFormID.propertyMainMenuListView.currentIndex != -1) {
        mainFormID.propertyMainMenuBottomListView.currentIndex = -1;
        mainFormID.propertyMainMenuListView.forceActiveFocus();
    } else {
        mainFormID.propertyMainMenuListView.currentIndex = -1;
        mainFormID.propertyMainMenuBottomListView.forceActiveFocus();
    }
}

function getSubMenuName(index) {

    if (mainFormID.propertyMainMenuListView.currentIndex != -1) {
        return "Sub Menu de "
                + qsTranslate("MainMenuModel", mainFormID.propertyMainMenuListView.model.get(
                                  mainFormID.propertyMainMenuListView.currentIndex).name)
                + controler.autoTr

    }
    else if (mainFormID.propertyMainMenuBottomListView.currentIndex != -1) {
        return "Sub Menu de "
                + qsTranslate("MainMenuModel", mainFormID.propertyMainMenuBottomListView.model.get(
                                  mainFormID.propertyMainMenuBottomListView.currentIndex).name)
                + controler.autoTr
    } else {
        return "Sub Menu"
    }
}

function goToSubMenu() {
    mainFormID.state = Constants.MenuState.NORMAL;
    console.log("submenu index at ", mainFormID.propertySubMenuListView.currentIndex)
    console.log("MODEL at index at ", mainFormID.propertySubMenuListView.model)

    if (mainFormID.propertySubMenuListView.currentIndex != -1) {

        if (mainFormID.propertyMainMenuListView.currentIndex != -1) {
            if (mainFormID.propertyMainMenuListView.model.get(
                        mainFormID.propertyMainMenuListView.currentIndex).expand === true) {
                mainFormID.propertyPageLoader.source =
                        "../" + mainFormID.propertyMainMenuListView.model.get(
                            mainFormID.propertyMainMenuListView.currentIndex).subdata.get(0).url
                mainFormID.propertySubMenuListView.currentIndex = 0
            }
        } else {
            if (mainFormID.propertyMainMenuBottomListView.model.get(
                        mainFormID.propertyMainMenuBottomListView.currentIndex).expand === true) {
                mainFormID.propertyPageLoader.source =
                        "../" + mainFormID.propertyMainMenuListView.model.get(
                            mainFormID.propertyMainMenuListView.currentIndex).subdata.get(0).url
                mainFormID.propertySubMenuListView.currentIndex = 0
            }
        }
        mainFormID.propertySubMenuListView.forceActiveFocus();
    } else {
        goToHome();
    }
}

function detectBackKeys(key, menuState) {
    console.log("MenuState is ", menuState)
    if (key === Qt.Key_Back || key === Qt.Key_Left
            || key === Qt.Key_Back || key === Qt.Key_Delete
            || key === Qt.Key_Escape || key === 16777219) {
        switch (menuState) {
            case Constants.MenuState.HOME:
                goToHome();
                break;
            case Constants.MenuState.SUB_MENU:
                goToSubMenu();
                break;
        }
    }
}
function detectBackKeysTextEdit(key) {
    if (key === Qt.Key_Back || key === Qt.Key_Left
            || key === Qt.Key_Back || key === Qt.Key_Delete
            || key === Qt.Key_Escape || key === 16777219) {
        return true;
    }
    return false;
}
function stripFilePrefix(filePath) {
    if (Qt.platform.os === "windows") {
        return filePath.replace(/^(file:\/{3})|(file:)|(qrc:\/{3})|(http:\/{3})/, "")
    }
    else {
        return filePath.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/, "");
    }
}
function replaceFileSuffix(url, suffix){
    //replaces file extension if there is one, by given suffix.
    var filename = url.split(/[\\/]/).pop()
    if (filename.lastIndexOf('.') > 0)
        filename = filename.substring(0, filename.lastIndexOf('.'))

    return url.substring(0, url.lastIndexOf('/') + 1 ) + filename + suffix
}
function filterText(text){
    var temp = text.replace(/<[^>]*>/g, '')
    temp = temp.replace(/\*/g, '')
    temp = temp.replace(/\#/g, '')
    return temp
}
