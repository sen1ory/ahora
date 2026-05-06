import QtQuick
import QtQuick.Controls

Button {
    id: root

    // ---- existing properties ----
    property int fontSize: 14
    property int radiusVal: 6
    property bool bold: false
    property bool selectable: false
    property bool selected: false
    property string colorVariant: "gray"
    property string actualColorVariant: selectable ? "gray" : colorVariant
    property real letterSpacing: 0

    // custom color support {{{
    property bool useCustomColors: false
    property color customBg: "#282828"
    property color customBgHover: "#282828"
    property color customFont: "#ebdbb2"
    property color customFontHover: "#ebdbb2"
    property color customBorder: "#3c3836"
    // }}}

    hoverEnabled: true

    // color maps {{{
    readonly property var _variantColors: {
        "off":    [["#282828", "#282828"], ["#504945", "#504945"], "#3c3836"],
        "gray":   [["#3c3836", "#665c54"], ["#ebdbb2", "#ebdbb2"], "#3c3836"],
        "blue":   [["#458588", "#83a598"], ["#1d2021", "#1d2021"], "#458588"],
        "aqua":   [["#689d6a", "#8ec07c"], ["#1d2021", "#1d2021"], "#689d6a"],
        "green":  [["#98971a", "#b8bb26"], ["#1d2021", "#1d2021"], "#98971a"],
        "yellow": [["#d79921", "#fabd2f"], ["#1d2021", "#1d2021"], "#d79921"],
        "red":    [["#cc241d", "#fb4934"], ["#1d2021", "#1d2021"], "#cc241d"]
    }
    // }}}

    // ---- overridden color getters (using custom colors when enabled) ----
    readonly property color _hoverBg: {
        if (useCustomColors) return hovered ? customBgHover : customBg
        if (!enabled)       return _variantColors["off"][0][0]
        if (selected)       return _variantColors[colorVariant][0][hovered ? 1 : 0]
        return _variantColors[actualColorVariant][0][hovered ? 1 : 0]
    }

    readonly property color _hoverFontColor: {
        if (useCustomColors) return hovered ? customFontHover : customFont
        if (!enabled)       return _variantColors["off"][1][0]
        if (selected)       return _variantColors[colorVariant][1][hovered ? 1 : 0]
        return _variantColors[actualColorVariant][1][hovered ? 1 : 0]
    }

    readonly property color _borderColor: {
        if (useCustomColors) return customBorder
        if (!enabled)       return _variantColors["off"][2]
        if (selected)       return _variantColors[colorVariant][2]
        return _variantColors[actualColorVariant][2]
    }

    contentItem: Text {
        text: root.text
        color: root._hoverFontColor
        font.bold: root.selected
        font.pixelSize: root.fontSize
        font.letterSpacing: root.letterSpacing   // NEW
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        color: root._hoverBg
        border.color: root._borderColor
        radius: root.radiusVal
        border.width: 1
    }
}
