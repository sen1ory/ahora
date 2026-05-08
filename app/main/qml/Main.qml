import QtQuick
import QtQuick.Controls
import Ahora_app_qrcode
import Ahora_core_ipaddress
import Ahora_app_main


// Two views:
//   1. QR_VIEW — shows QR code and IP selector
//   2. ADMIN_PANEL — admin panel with team management
Window {
    id: root
    width: 800
    height: 600
    color: "#1d2021"
    visible: true
    title: "Ahora — Квиз"

    // Start with QR view
    property string currentView: "QR_VIEW"

    // Discover IP addresses
    IpAddress {
        id: ipAddress
    }

    // Refresh IP list every 2 seconds
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: ipAddress.refresh()
    }

    // QR code view container
    Rectangle {
        id: qrView
        anchors.fill: parent
        color: AhoraTheme.bgDark
        visible: currentView === "QR_VIEW"

        // Instruction text
        Text {
            id: instructionText
            anchors {
                top: parent.top
                topMargin: 30
                horizontalCenter: parent.horizontalCenter
            }
            text: "Отсканируйте QR-код чтобы подключиться к квизу"
            color: AhoraTheme.bgBorderLight
            font.pixelSize: 16
        }

        // Show the selected URL
        Text {
            id: urlText
            anchors {
                top: instructionText.bottom
                topMargin: 8
                horizontalCenter: parent.horizontalCenter
            }
            text: "http://" + ipAddress.ip + ":8080/"
            color: AhoraTheme.accentYellow
            font.pixelSize: 14
            font.bold: true
        }

        // QR code with server URL
        QRCodeItem {
            id: qrCode
            width: Math.min(root.width, root.height) / 2
            height: width
            anchors {
                centerIn: parent
                verticalCenterOffset: -20
            }

            // Build full URL: http://IP:8080/
            text: "http://" + ipAddress.ip + ":8080/"
            foreground: AhoraTheme.textPrimary
            background: AhoraTheme.bgDark
        }

        // === IP Selector: show all available addresses ===
        // Needed when IpAddress auto-selects the wrong hotspot network
        Text {
            id: ipsLabel
            anchors {
                top: qrCode.bottom
                topMargin: 12
                horizontalCenter: parent.horizontalCenter
            }
            text: "Доступные адреса (нажмите чтобы выбрать):"
            color: AhoraTheme.bgBorderLight
            font.pixelSize: 12
        }

        // IP address list — clickable buttons
        Row {
            id: ipList

            anchors {
                top: ipsLabel.bottom
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 8

            // Dynamically create a button for each IP
            Repeater {
                model: ipAddress.allIps

                GruvButton {
                    required property string modelData
                    text: modelData
                    fontSize: 13

                    selectable: true
                    selected: modelData === ipAddress.ip

                    colorVariant: "yellow"

                    onClicked: {
                        ipAddress.selectIp(modelData)
                        console.log("Selected IP:", modelData)
                    }
                }
            }

            GruvButton {
                text: "↻"
                fontSize: 13

                onClicked: {
                    ipAddress.refresh()
                    console.log("IP list refreshed")
                }
            }
        }

        GruvButton {
            id: startButton

            anchors {
                top: ipList.bottom
                topMargin: 20
                horizontalCenter: parent.horizontalCenter
            }
            width: 400
            height: 50

            text: "Перейти на страницу квиза"
            fontSize: 16

            radiusVal: 8
            bold: true

            colorVariant: "yellow"

            onClicked: {
                currentView = "ADMIN_PANEL"
                console.log("Switched to admin panel")
            }
        }
    }

    // === State 2: Admin panel ===
    AdminPanel {
        id: adminPanel
        anchors.fill: parent
        visible: currentView === "ADMIN_PANEL"

        // On "Back", return to QR code view
        onGoBack: {
            currentView = "QR_VIEW"
            console.log("Returned to QR code")
        }
    }
}
