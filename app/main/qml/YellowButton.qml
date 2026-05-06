import QtQuick
import QtQuick.Controls

Button {
    id: root

    property int fontSize: 14
    property int radiusVal: 6
    property bool bold: false

    hoverEnabled: true

    readonly property var _baseColors: enabled ? ["#d79921", "#fabd2f"] : ["#504945", "#504945"]
    readonly property color _hoverBg: hovered ? _baseColors[1] : _baseColors[0]
    readonly property color _borderColor: enabled ? "#1d2021" : "#3c3836"

    contentItem: Text {
        text: root.text
        color: root.enabled ? "#1d2021" : "#665c54"
        font.bold: root.bold
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
