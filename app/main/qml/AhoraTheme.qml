import QtQuick


QtObject {
    // Backgrounds
    readonly property color bgDark:          "#1d2021"   // main window bg
    readonly property color bgMedium:        "#282828"   // card, header, popup bg
    readonly property color bgBorder:        "#3c3836"   // borders, dividers
    readonly property color bgBorderLight:   "#504945"   // light border
    readonly property color bgMuted:         "#665c54"   // muted text, dot default

    // Text
    readonly property color textPrimary:     "#ebdbb2"
    readonly property color textSecondary:   "#a89984"
    readonly property color textMuted:       "#665c54"   // generic muted

    // Accent colours (Gruvbox palette)
    readonly property color accentRed:            "#cc241d"
    readonly property color accentRedBright:      "#fb4934"
    readonly property color accentOrange:         "#d65d0e"
    readonly property color accentOrangeBright:   "#fe8019"
    readonly property color accentYellow:         "#d79921"
    readonly property color accentYellowBright:   "#fabd2f"
    readonly property color accentGreen:          "#98971a"
    readonly property color accentGreenBright:    "#b8bb26"
    readonly property color accentAqua:           "#689d6a"
    readonly property color accentAquaBright:     "#8ec07c"
    readonly property color accentBlue:           "#458588"
    readonly property color accentBlueBright:     "#83a598"

    // Status indicator colours (used for question dots and answer verdicts)
    readonly property color statusGreen:    "#00CC66"
    readonly property color statusRed:      "#FF4444"
    readonly property color statusOrange:   "#FF8800"
    readonly property color statusDefault:  bgMuted

    // Overlays
    readonly property color overlayDark:    "#cc000000"
    readonly property color overlayPopup:   "#dd000000"

    // QR code (match main background / foreground)
    readonly property color qrForeground:   textPrimary
    readonly property color qrBackground:   bgDark
}
