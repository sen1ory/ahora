import QtQuick
import QtQuick.Controls

Button {
    id: root

    // defaults {{{
    property int fontSize: 14
    property int radiusVal: 6
    property bool bold: false
    property bool selectable: false
    property bool selected: false
    property string colorVariant: "gray"
    property string actualColorVariant: selectable ? "gray" : colorVariant
    // }}}

    hoverEnabled: true
    
    // colors {{{
    readonly property var _variantColors: {
            "off":   [["#282828", "#282828"], ["#504945", "#504945"], "#3c3836"],
            "gray":  [["#3c3836", "#665c54"], ["#ebdbb2", "#ebdbb2"], "#3c3836"],
            "blue":  [["#458588", "#83a598"], ["#1d2021", "#1d2021"], "#458588"],
            "aqua":  [["#689d6a", "#8ec07c"], ["#1d2021", "#1d2021"], "#689d6a"],
            "green": [["#98971a", "#b8bb26"], ["#1d2021", "#1d2021"], "#98971a"],
            "red":   [["#cc241d", "#fb4934"], ["#1d2021", "#1d2021"], "#cc241d"]
    }

    // selectable button should be gray (but any possible)
    readonly property var _baseColors: {
        if (!enabled) return _variantColors["off"][0]
        if (selected) return _variantColors[colorVariant][0]
        if (!selected) return _variantColors[actualColorVariant][0]
        return _variantColors[0]
    }
    readonly property color _hoverBg: hovered ? _baseColors[1] : _baseColors[0]

    readonly property color _fontColor: {
        if (!enabled) return _variantColors["off"][1]
        if (selected) return _variantColors[colorVariant][1]
        if (!selected) return _variantColors[actualColorVariant][1]
        return _variantColors[1]
    }
    readonly property color _hoverFontColor: hovered ? _fontColor[1] : _fontColor[0]

    readonly property color _borderColor: {
        if (!enabled) return _variantColors["off"][2]
        if (selected) return _variantColors[colorVariant][2]
        if (!selected) return _variantColors[actualColorVariant][2]
        return _variantColors[2]
    }
    // }}}

    contentItem: Text {
        text: root.text
        color: root._hoverFontColor

        font.bold: root.selected
        font.pixelSize: root.fontSize

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
