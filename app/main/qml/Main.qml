import QtQuick
import QtQuick.Controls
import Ahora_app_qrcode
import Ahora_core_ipaddress


// Main.qml — главное окно приложения Ahora
// Два состояния:
//   1. QR_VIEW — показывает QR-код для подключения клиентов + список IP
//   2. ADMIN_PANEL — панель администратора со списком команд
Window {
    id: root
    width: 800
    height: 600
    color: "#1d2021"
    visible: true
    title: "Ahora — Квиз"

    // Текущее состояние: "QR_VIEW" или "ADMIN_PANEL"
    property string currentView: "QR_VIEW"

    // IP-адрес для QR-кода (получаем из IpAddress)
    IpAddress {
        id: ipAddress
    }

    // Авто-обновление IP каждые 2 секунды
    // Нужно для случая, когда прога запущена ДО подключения к хотспоту
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: ipAddress.refresh()
    }

    // === Состояние 1: QR-код ===
    Rectangle {
        id: qrView
        anchors.fill: parent
        color: "#1d2021"
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
            color: "#a89984"
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
            color: "#d79921"
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
            autoIp: false       // Не используем autoIp — сами формируем URL
            foreground: "#ebdbb2"
            background: "#1d2021"
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
            color: "#a89984"
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

                Button {
                    required property string modelData
                    // colors {{{
                    readonly property color fontColor: (modelData === ipAddress.ip) ? "#1d2021" : "#ebdbb2"
                    readonly property var baseColors: (modelData === ipAddress.ip)
                                                        ? ["#d79921", "#fabd2f"]
                                                        : ["#3c3836", "#665c54"]
                    readonly property color hoverBaseColor: hovered ? baseColors[1] : baseColors[0]
                    readonly property color borderColor: (modelData === ipAddress.ip) ? "#1d2021" : "#3c3836"
                    // }}}

                    hoverEnabled: true // for hover events

                    text: modelData
                    font.pixelSize: 13
                    font.bold: (modelData === ipAddress.ip)

                    // Стиль: выбранный IP — жёлтый, остальные — серые
                    contentItem: Text {
                        text: modelData
                        font.pixelSize: 13
                        color: fontColor
                        font.bold: (modelData === ipAddress.ip)

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: hoverBaseColor
                        border.color: borderColor

                        radius: 6
                        border.width: 1
                    }

                    // При клике выбираем этот IP и обновляем QR
                    onClicked: {
                        ipAddress.selectIp(modelData)
                        console.log("Выбран IP:", modelData)
                    }
                }
            }

            Button {
                // colors {{{
                readonly property color fontColor: "#ebdbb2"
                readonly property var baseColors: ["#3c3836", "#665c54"]
                readonly property color hoverBaseColor: hovered ? baseColors[1] : baseColors[0]
                readonly property color borderColor: "#3c3836"
                // }}}

                hoverEnabled: true // for hover events

                text: "↻"
                font.pixelSize: 16

                contentItem: Text {
                    text: "↻"
                    color: parent.fontColor
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: parent.hoverBaseColor
                    border.color: parent.borderColor

                    radius: 6
                    border.width: 1
                }

                onClicked: {
                    ipAddress.refresh()
                    console.log("Список IP обновлён")
                }
            }
        }

        // Кнопка перехода на панель администратора
        Button {
            id: startButton

            // colors {{{
            readonly property color fontColor: "#1d2021"
            readonly property var baseColors: ["#d79921", "#fabd2f"]
            readonly property color hoverBaseColor: hovered ? baseColors[1] : baseColors[0]
            readonly property color borderColor: "#1d2021"
            // }}}

            hoverEnabled: true // for hover events

            anchors {
                top: ipList.bottom
                topMargin: 20
                horizontalCenter: parent.horizontalCenter
            }
            width: 400
            height: 50
            text: "Перейти на страницу квиза"

            // Стилизация кнопки
            contentItem: Text {
                text: startButton.text
                color: parent.fontColor
                font.bold: true
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: parent.hoverBaseColor
                border.color: parent.borderColor

                radius: 8
                border.width: 1
            }

            // При нажатии переключаемся на панель администратора
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
