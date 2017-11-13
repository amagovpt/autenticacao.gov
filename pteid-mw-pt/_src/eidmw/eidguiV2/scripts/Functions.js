// Functions.js

function getMainMenuWidth(parentWidth){
    var handColor

    switch(mainFormID.state) {
    case "STATE_FIRST_RUN":
        handColor = parentWidth * 2 * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
        break;
    case "STATE_HOME":
        handColor = parentWidth
        break;
    case "STATE_EXPAND":
        handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
        break;
    default: //STATE_NORMAL
        handColor = parentWidth * Constants.MAIN_MENU_VIEW_RELATIVE_SIZE
    }
    return handColor
}
function getSubMenuWidth(parentWidth){
    var handColor

    switch(mainFormID.state) {
    case "STATE_FIRST_RUN":
        handColor = 0
        break;
    case "STATE_HOME":
        handColor = 0
        break;
    case "STATE_EXPAND":
        handColor = parentWidth * Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE
        break;
    default: //STATE_NORMAL
        handColor = parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE
    }
    return handColor
}
function getContentPagesMenuWidth(parentWidth){
    var handColor

    switch(mainFormID.state) {
    case "STATE_FIRST_RUN":
        handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
        break;
    case "STATE_HOME":
        handColor = 0
        break;
    case "STATE_EXPAND":
        handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE +
                parentWidth * Constants.SUB_MENU_VIEW_RELATIVE_SIZE -
                parentWidth * Constants.SUB_MENU_EXPAND_VIEW_RELATIVE_SIZE
        break;
    default: //STATE_NORMAL
        handColor = parentWidth * Constants.CONTENT_PAGES_VIEW_RELATIVE_SIZE
    }
    return handColor
}

function getIsVisibleSubMenuViewHorizontalLine(index)
{
    var handVisible
    if(mainFormID.propertySubMenuListView.count -1 === index ||
            mainFormID.propertySubMenuListView.currentIndex === index ||
            mainFormID.propertySubMenuListView.currentIndex -1 === index)
    {
        handVisible =  false
    }else{
        handVisible =  true
    }
    return handVisible
}

function getSubNameColor(index, containsMouse)
{
    var handColor
    if(mainFormID.propertySubMenuListView.currentIndex === index)
    {
        handColor =  Constants.COLOR_TEXT_SUB_MENU_SELECTED
    }else{
        if(containsMouse === true)
        {
            handColor = Constants.COLOR_TEXT_SUB_MENU_MOUSE_OVER
        }else{
            handColor = Constants.COLOR_TEXT_SUB_MENU_DEFAULT
        }
    }
    return handColor
}

function getSubMenuArrowSource(index, containsMouse)
{
    var handSource = "images/arrow-right_white_AMA.png"
    if(mainFormID.propertySubMenuListView.currentIndex === index)
    {
        handSource =  "images/arrow-right_white_AMA.png"
    }else{
        if(containsMouse === true)
        {
            handSource = "images/arrow-right_hover.png"
        }else{
            handSource = ""
        }
    }

    return handSource
}

function getMainMenuArrowSource(index, containsMouse)
{
    var handSource = "images/arrow-right_white_AMA.png"

    if(mainFormID.propertyMainMenuListView.currentIndex === index)
    {
        handSource =  "images/arrow-right_white_AMA.png"
    }else{
        if(containsMouse === true)
        {
            handSource = "images/arrow-right_hover.png"
        }else{
            handSource = ""
        }
    }
    return handSource
}

function getBottomMenuImgSource(index, containsMouse)
{
    var handSource = "images/help_icon.png"

    if(mainFormID.propertyMainMenuBottomListView.currentIndex === index)
    {
        handSource =  mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
    }else{
        if(containsMouse === true)
        {
            handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrlSel
        }else{
            handSource = mainFormID.propertyMainMenuBottomListView.model.get(index).imageUrl
        }
    }

    return handSource
}
