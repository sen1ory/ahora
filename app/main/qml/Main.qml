import QtQuick
import QtQuick.Controls

Window {
    id: root
    width: 640
    height: 480
    color: "#1d2021"
    visible: true
    title: qsTr("Hello World")

    property bool qrCodeVisible: false

    Button {
        id: showButton
        text: "Show QR-Code"
        width: root.width / 3
        height: root.height / 8
        anchors.centerIn: parent
        visible: !qrCodeVisible

        onClicked: {
            qrCodeVisible = true
            console.log("Showed qr-code")
        }
        onHoveredChanged: {
            color: "#665c54"
        }
    }

    Button {
        id: hideButton
        text: "Hide QR-Code"
        width: root.width / 3
        height: root.height / 8
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        visible: qrCodeVisible

        onClicked: {
            qrCodeVisible = false
            console.log("Hid qr-code")
        }
        onHoveredChanged: {
            color: "#665c54"
        }
    }

    Rectangle {
        id: qrCodePlaceholder
        width: Math.min(root.width, root.height) / 2
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: hideButton.bottom
        anchors.topMargin: 20
        visible: qrCodeVisible
        color: "#ffffff"

        Image {
            anchors.fill: parent
            source: "image://qrgen/Hello World"
        }
    }
}
