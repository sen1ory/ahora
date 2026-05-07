import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ahora_app_main


// AdminPanel — панель администратора со списком подключенных команд
Rectangle {
    id: panelRoot

    signal goBack()

    // Отладка
    Component.onCompleted: {
        console.log("AdminPanel создан, teamCount:", SM ? SM.teamCount : "null")
    }

    color: AhoraTheme.bgDark

    // === Таймер ===
    property int timerMinutes: 5
    property int timerSeconds: 0
    property int remainingSeconds: timerMinutes * 60 + timerSeconds
    property bool timerRunning: false
    property bool timerPaused: false
    property bool timerExpired: false

    Timer {
        id: countdownTimer
        interval: 1000
        running: false
        repeat: true
        onTriggered: {
            if (panelRoot.remainingSeconds > 0) {
                panelRoot.remainingSeconds -= 1
            }
            if (panelRoot.remainingSeconds <= 0) {
                countdownTimer.stop()
                panelRoot.timerRunning = false
                panelRoot.timerExpired = true
                SM.broadcastTimerAction("timeout")
            }
        }
    }

    function formatTime(secs) {
        var validSecs = Math.max(0, secs)
        var m = Math.floor(validSecs / 60)
        var s = validSecs % 60
        return (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s
    }

    function startTimer() {
        if (panelRoot.remainingSeconds <= 0) return
        panelRoot.timerRunning = true
        panelRoot.timerPaused = false
        panelRoot.timerExpired = false
        countdownTimer.start()
        SM.broadcastTimerAction("resume")
    }

    function pauseTimer() {
        countdownTimer.stop()
        panelRoot.timerRunning = false
        panelRoot.timerPaused = true
        SM.broadcastTimerAction("pause")
    }

    function resetTimer() {
        countdownTimer.stop()
        panelRoot.remainingSeconds = panelRoot.timerMinutes * 60 + panelRoot.timerSeconds
        panelRoot.timerRunning = false
        panelRoot.timerPaused = false
        panelRoot.timerExpired = false
        SM.broadcastTimerAction("reset")
    }

    // Верхняя панель с заголовком, счётчиком и кнопкой "Назад"
    Rectangle {
        id: headerBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 50
        color: AhoraTheme.bgMedium

        GruvButton {
            anchors {
                left: parent.left
                leftMargin: 12
                verticalCenter: parent.verticalCenter
            }
            text: "← Назад"
            font.pixelSize: 14

            colorVariant: "yellow"

            onClicked: panelRoot.goBack()
        }

        Text {
            anchors.centerIn: parent
            text: "Панель администратора"
            color: AhoraTheme.textPrimary
            font.bold: true
            font.pixelSize: 16
        }
        // Счётчик команд
        Text {
            anchors {
                right: parent.right
                rightMargin: 150
                verticalCenter: parent.verticalCenter
            }
            text: "Команд: " + SM.teamCount
            color: "#a89984"
            font.pixelSize: 13
        }
        // Таймер
        GruvButton {
            id: timerButton
            anchors {
                right: parent.right
                rightMargin: 12
                verticalCenter: parent.verticalCenter
            }
            text: panelRoot.formatTime(panelRoot.remainingSeconds)
            fontSize: 18
            bold: panelRoot.timerRunning || panelRoot.timerPaused || panelRoot.timerExpired
            letterSpacing: 2

            radiusVal: 6

            colorVariant: {
                if (panelRoot.timerExpired) return "red"
                if (panelRoot.timerRunning) return "green"
                if (panelRoot.timerPaused)  return "yellow"
                return "blue"
            }

            // left click: start/pause
            onClicked: {
                if (panelRoot.timerExpired) return
                if (panelRoot.timerRunning) { panelRoot.pauseTimer() }
                else if (panelRoot.timerPaused) { panelRoot.startTimer() }
                else { panelRoot.startTimer() }
            }

            // right click: context menu
            TapHandler {
                acceptedButtons: Qt.RightButton
                onTapped: timerMenu.popup()
            }

            // the settings menu (unchanged from original, but now inside the button)
            Menu {
                id: timerMenu

                MenuItem {
                    contentItem: Row {
                        spacing: 8
                        padding: 4

                        SpinBox {
                            id: minutesSpin
                            from: 0; to: 99
                            value: panelRoot.timerMinutes
                            editable: true; width: 70; height: 28
                            contentItem: Text {
                                text: minutesSpin.textFromValue(minutesSpin.value, minutesSpin.locale)
                                color: AhoraTheme.textPrimary; font.pixelSize: 13
                                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle { color: AhoraTheme.bgBorder; radius: 4 }
                        }

                        Text { text: "мин"; color: AhoraTheme.textSecondary; font.pixelSize: 13; anchors.verticalCenter: parent.verticalCenter }

                        SpinBox {
                            id: secondsSpin
                            from: 0; to: 59
                            value: panelRoot.timerSeconds
                            editable: true; width: 70; height: 28
                            contentItem: Text {
                                text: secondsSpin.textFromValue(secondsSpin.value, secondsSpin.locale)
                                color: "#ebdbb2"; font.pixelSize: 13
                                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle { color: AhoraTheme.bgBorder; radius: 4 }
                        }

                        Text { text: "сек"; color: Ahora; font.pixelSize: 13; anchors.verticalCenter: parent.verticalCenter }
                    }
                    background: Rectangle { color: AhoraTheme.bgMedium; radius: 6; border.color: AhoraTheme.bgBorderLight; border.width: 1 }
                    implicitWidth: 260; implicitHeight: 40
                }

                MenuItem {
                    contentItem: GruvButton {
                        width: parent ? parent.width : undefined
                        height: parent ? parent.height : undefined
                        text: "Установить"
                        fontSize: 13

                        bold: true
                        colorVariant: "yellow"

                        radiusVal: 6

                        onClicked: {
                            timerMenu.close()
                            panelRoot.timerMinutes = minutesSpin.value
                            panelRoot.timerSeconds = secondsSpin.value
                            panelRoot.resetTimer()
                        }
                    }
                    background: Rectangle { color: AhoraTheme.bgDark; radius: 6; border.color: AhoraTheme.bgBorderLight; border.width: 1 }
                    implicitWidth: 260; implicitHeight: 40
                }
            }
        }
    }

    // === Область карточек команд — центрирована по центру по вертикали и горизонтали ===
    ScrollView {
        id: scrollArea
        anchors {
            top: headerBar.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 8
        }
        clip: true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        ColumnLayout {
            width: scrollArea.availableWidth
            implicitHeight: Math.max(scrollArea.availableHeight, flow.implicitHeight)

            Item { Layout.fillHeight: true }

            Flow {
                id: flow
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: scrollArea.availableWidth
                spacing: 12

                // Плейсхолдер, если команд нет
                Rectangle {
                    visible: SM.teamCount === 0
                    width: parent.width; height: 60; color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "⏳ Ожидание подключения команд..."
                        color: AhoraTheme.textSecondary; font.pixelSize: 15
                    }
                }

                Repeater {
                    model: SM

                    delegate: TeamCard {
                        teamName: name
                        teamStatuses: statuses
                        teamId: teamId
                        teamAnswers: answers

                        onClicked: function(id) {
                            panelRoot.openTeamDetail(id)
                        }

                        NumberAnimation on opacity {
                            from: 0; to: 1; duration: 400; easing.type: Easing.OutCubic
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    // === Детальный попап команды ===
    property string detailTeamId: ""
    property bool detailVisible: false

    function openTeamDetail(teamId) {
        panelRoot.detailTeamId = teamId
        panelRoot.detailVisible = true
    }

    function closeTeamDetail() {
        panelRoot.detailVisible = false
        panelRoot.detailTeamId = ""
    }

    Rectangle {
        id: detailPopup
        anchors.fill: parent
        color: "#dd000000"
        visible: panelRoot.detailVisible
        z: 998

        MouseArea {
            anchors.fill: parent
            onClicked: panelRoot.closeTeamDetail()
        }

        Rectangle {
            id: detailContent
            anchors {
                centerIn: parent
                margins: 40
            }
            width: Math.min(parent.width - 80, 600)
            height: Math.min(parent.height - 80, 500)
            color: "#282828"
            radius: 12
            border.color: "#504945"
            border.width: 1

            MouseArea { anchors.fill: parent }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 6

                Text {
                    text: SM.teamNameById(panelRoot.detailTeamId) || "Команда"
                    color: "#ebdbb2"; font.bold: true; font.pixelSize: 20
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#504945" }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    Column {
                        width: parent.width
                        spacing: 12

                        Repeater {
                            model: SM.questions.length

                            delegate: Rectangle {
                                width: parent.width
                                height: questionCol.implicitHeight + 20
                                color: "#3c3836"
                                radius: 8
                                border.color: "#504945"
                                border.width: 1
                                visible: panelRoot.detailVisible

                                Column {
                                    id: questionCol
                                    anchors {
                                        fill: parent
                                        margins: 10
                                    }
                                    spacing: 6

                                    // Question text
                                    Text {
                                        text: (index + 1) + ". " + SM.questions[index]
                                        color: "#ebdbb2"; font.bold: true; font.pixelSize: 14
                                        wrapMode: Text.WordWrap; width: parent.width
                                    }

                                    // Team answer
                                    Text {
                                        text: {
                                            var answers = SM.teamAnswersById(panelRoot.detailTeamId)
                                            if (answers.length > index && answers[index] !== "")
                                                return "Ответ: " + answers[index]
                                            return "Нет ответа"
                                        }
                                        color: "#a89984"; font.pixelSize: 12
                                        wrapMode: Text.WordWrap; width: parent.width
                                    }

                                    // Status
                                    Text {
                                        text: {
                                            var statuses = SM.teamStatusesById(panelRoot.detailTeamId)
                                            if (statuses.length <= index) return ""
                                            if (statuses[index] === "white") return "⏺ Не отвечен"
                                            if (statuses[index] === "green") return "✅ Верно"
                                            if (statuses[index] === "red") return "❌ Неверно"
                                            if (statuses[index] === "orange") return "⏳ Ожидает проверки"
                                            return ""
                                        }
                                        color: {
                                            var statuses = SM.teamStatusesById(panelRoot.detailTeamId)
                                            if (statuses.length <= index) return "#665c54"
                                            if (statuses[index] === "green") return "#00CC66"
                                            if (statuses[index] === "red") return "#FF4444"
                                            if (statuses[index] === "orange") return "#FF8800"
                                            return "#665c54"
                                        }
                                        font.pixelSize: 12
                                    }

                                    // Approve/reject buttons for text question (index 2)
                                    Row {
                                        spacing: 8
                                        visible: index === 2 && SM.teamStatusesById(panelRoot.detailTeamId)[index] === "orange"

                                        GruvButton {
                                            text: "✓ Верно"
                                            height: 28
                                            fontSize: 12
                                            radiusVal: 4
                                            colorVariant: "green"
                                            onClicked: {
                                                SM.approveTextAnswer(panelRoot.detailTeamId, index, true)
                                            }
                                        }

                                        GruvButton {
                                            text: "✗ Неверно"
                                            height: 28
                                            fontSize: 12
                                            radiusVal: 4
                                            colorVariant: "red"
                                            onClicked: {
                                                SM.approveTextAnswer(panelRoot.detailTeamId, index, false)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Score section
                Rectangle { Layout.fillWidth: true; height: 1; color: "#504945" }

                Row {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Оценка:"
                        color: "#a89984"; font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    SpinBox {
                        id: scoreSpin
                        from: 0; to: 100
                        value: 0
                        width: 80; height: 30
                        contentItem: Text {
                            text: scoreSpin.value
                            color: "#ebdbb2"; font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle { color: "#3c3836"; radius: 4 }
                    }

                    GruvButton {
                        text: "Сохранить"
                        fontSize: 13
                        bold: true
                        radiusVal: 6
                        colorVariant: "yellow"
                        onClicked: {
                            SM.setScore(panelRoot.detailTeamId, 0, scoreSpin.value)
                            console.log("Сохранено", scoreSpin.value, "баллов для", panelRoot.detailTeamId)
                        }
                    }
                }

                // Close button
                GruvButton {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 200
                    text: "Закрыть"
                    fontSize: 14
                    radiusVal: 6
                    colorVariant: "gray"
                    onClicked: panelRoot.closeTeamDetail()
                }
            }
        }
    }
}
