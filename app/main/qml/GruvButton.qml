import QtQuick
import QtQuick.Controls
import Ahora_app_main

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
    property color customBg:         AhoraTheme.bgMedium
    property color customBgHover:    AhoraTheme.bgMedium
    property color customFont:       AhoraTheme.textPrimary
    property color customFontHover:  AhoraTheme.textPrimary
    property color customBorder:     AhoraTheme.bgBorder
    // }}}

    hoverEnabled: true

    // color maps {{{
    readonly property var _variantColors: {
        "off":    [[AhoraTheme.bgMedium, AhoraTheme.bgMedium],
                   [AhoraTheme.textMuted, AhoraTheme.textMuted],
                   AhoraTheme.bgBorder],

        "gray":   [[AhoraTheme.bgBorder, AhoraTheme.bgMuted],
                   [AhoraTheme.textPrimary, AhoraTheme.textPrimary],
                   AhoraTheme.bgBorder],

        "red":    [[AhoraTheme.accentRed, AhoraTheme.accentRedBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentRed],

        "orange": [[AhoraTheme.accentOrange, AhoraTheme.accentOrangeBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentRed],

        "yellow": [[AhoraTheme.accentYellow, AhoraTheme.accentYellowBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentYellow],

        "green":  [[AhoraTheme.accentGreen, AhoraTheme.accentGreenBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentGreen],


        "aqua":   [[AhoraTheme.accentAqua, AhoraTheme.accentAquaBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentAqua],

        "blue":   [[AhoraTheme.accentBlue, AhoraTheme.accentBlueBright],
                   [AhoraTheme.bgDark, AhoraTheme.bgDark],
                   AhoraTheme.accentBlue]
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
