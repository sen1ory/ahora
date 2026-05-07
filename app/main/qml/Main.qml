import QtQuick
import QtQuick.Controls
import Ahora_app_qrcode
import Ahora_core_ipaddress
import Ahora_app_main


//   1. QR_VIEW
//   2. ADMIN_PANEL
Window {
    id: root
    width: 800
    height: 600
    color: "#1d2021"
    visible: true
    title: "Ahora — Квиз"

    // start with
    property string currentView: "QR_VIEW"

    // get ip first
    IpAddress {
        id: ipAddress
    }

    // refresh ip every 2 seconds
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: ipAddress.refresh()
    }

    // show qr-code
    Rectangle {
        id: qrView
        anchors.fill: parent
        color: AhoraTheme.bgDark
        visible: currentView === "QR_VIEW"

        // Текст-инструкция
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

        // Показываем выбранный URL
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

        // QR-код с URL сервера
        QRCodeItem {
            id: qrCode
            width: Math.min(root.width, root.height) / 2
            height: width
            anchors {
                centerIn: parent
                verticalCenterOffset: -20
            }

            // Формируем полный URL: http://IP:8080/
            text: "http://" + ipAddress.ip + ":8080/"
            foreground: AhoraTheme.textPrimary
            background: AhoraTheme.bgDark
        }

        // === Селектор IP: показывает все доступные адреса ===
        // Нужен для случая, когда IpAddress не угадал сеть хотспота
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

        // Список IP-адресов — кликабельные кнопки
        Row {
            id: ipList

            anchors {
                top: ipsLabel.bottom
                topMargin: 6
                horizontalCenter: parent.horizontalCenter
            }
            spacing: 8

            // Динамически создаём кнопку на каждый IP
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
                        console.log("Выбран IP:", modelData)
                    }
                }
            }

            GruvButton {
                text: "↻"
                fontSize: 13

                onClicked: {
                    ipAddress.refresh()
                    console.log("Список IP обновлён")
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
                console.log("Переключились на панель администратора")
            }
        }
    }

    // === Состояние 2: Панель администратора ===
    AdminPanel {
        id: adminPanel
        anchors.fill: parent
        visible: currentView === "ADMIN_PANEL"

        // При нажатии "Назад" возвращаемся к QR-коду
        onGoBack: {
            currentView = "QR_VIEW"
            console.log("Вернулись к QR-коду")
        }
    }
}
