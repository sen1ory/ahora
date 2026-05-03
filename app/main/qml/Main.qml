import QtQuick
import QtQuick.Controls
import Ahora_app_qrcode
import Ahora_core_ipadress


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

    // IP-адрес для QR-кода (получаем из IpAdress)
    IpAdress {
        id: ipAdress
    }

    // Авто-обновление IP каждые 2 секунды
    // Нужно для случая, когда прога запущена ДО подключения к хотспоту
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: ipAdress.refresh()
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
            text: "http://" + ipAdress.ip + ":8080/"
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
            text: "http://" + ipAdress.ip + ":8080/"
            autoIp: false       // Не используем autoIp — сами формируем URL
            foreground: "#ebdbb2"
            background: "#1d2021"
            border: 6
        }

        // === Селектор IP: показывает все доступные адреса ===
        // Нужен для случая, когда IpAdress не угадал сеть хотспота
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
                model: ipAdress.allIps

                Button {
                    required property string modelData

                    text: modelData
                    font.pixelSize: 13
                    font.bold: (modelData === ipAdress.ip)

                    // Стиль: выбранный IP — жёлтый, остальные — серые
                    contentItem: Text {
                        text: modelData
                        color: (modelData === ipAdress.ip) ? "#1d2021" : "#ebdbb2"
                        font.pixelSize: 13
                        font.bold: (modelData === ipAdress.ip)
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: (modelData === ipAdress.ip) ? "#d79921" : "#3c3836"
                        radius: 6
                        border.color: "#504945"
                        border.width: 1
                    }

                    // При клике выбираем этот IP и обновляем QR
                    onClicked: {
                        ipAdress.selectIp(modelData)
                        console.log("Выбран IP:", modelData)
                    }
                }
            }

            // Кнопка обновления списка IP
            Button {
                text: "🔄"
                font.pixelSize: 16
                implicitWidth: 36
                implicitHeight: 32

                contentItem: Text {
                    text: "↻"
                    color: "#ebdbb2"
                    font.pixelSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: "#3c3836"
                    radius: 6
                    border.color: "#504945"
                    border.width: 1
                }

                onClicked: {
                    ipAdress.refresh()
                    console.log("Список IP обновлён")
                }
            }
        }

        // Кнопка перехода на панель администратора
        Button {
            id: startButton
            anchors {
                top: ipList.bottom
                topMargin: 20
                horizontalCenter: parent.horizontalCenter
            }
            width: 260
            height: 50
            text: "Перейти на страницу квиза"

            // Стилизация кнопки
            contentItem: Text {
                text: startButton.text
                color: "#1d2021"
                font.bold: true
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: "#d79921"
                radius: 8
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
        sessionManager: sessionManager  // Передаём из контекста

        // При нажатии "Назад" возвращаемся к QR-коду
        onGoBack: {
            currentView = "QR_VIEW"
            console.log("Вернулись к QR-коду")
        }
    }
}
