import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ahora_app_main


// AdminPanel — admin panel with list of connected teams
Rectangle {
    id: panelRoot

    signal goBack()

    // Debug
    Component.onCompleted: {
        console.log("AdminPanel created, teamCount:", SM ? SM.teamCount : "null")
        rebuildTeamCards()
    }

    color: AhoraTheme.bgDark

    // === Timer state machine:
    //   stopped (default) → startTimer() → running → pauseTimer() → paused → startTimer() → running
    //   Any state → resetTimer() → stopped
    //   running + countdown reaches 0 → expired (broadcasts "timeout") ===
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

    // === Header bar with title, team counter, back button, and timer ===
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
        // Team counter
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
    // Timer display and control button (left-click toggles, right-click for settings)
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

            // Left click: start/pause toggle
            onClicked: {
                if (panelRoot.timerExpired) return
                if (panelRoot.timerRunning) { panelRoot.pauseTimer() }
                else if (panelRoot.timerPaused) { panelRoot.startTimer() }
                else { panelRoot.startTimer() }
            }

            // Right click: context menu for timer settings
            TapHandler {
                acceptedButtons: Qt.RightButton
                onTapped: timerMenu.popup()
            }

            // Timer settings context menu
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

                        Text { text: "сек"; color: AhoraTheme.textSecondary; font.pixelSize: 13; anchors.verticalCenter: parent.verticalCenter }
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

    // TeamCard component template for dynamic creation
    Component {
        id: teamCardComponent
        TeamCard {
            NumberAnimation on opacity {
                from: 0; to: 1; duration: 400; easing.type: Easing.OutCubic
            }
        }
    }

// Dynamically creates TeamCard instances from SM model data.
// Called whenever teamCountChanged or teamDataChanged fires.
function rebuildTeamCards() {
    var children = flow.children;
    for (var i = children.length - 1; i >= 0; i--) {
        if (children[i].objectName === "placeholder") continue;
        children[i].destroy();
    }
    for (var j = 0; j < SM.teamCount; j++) {
        var card = teamCardComponent.createObject(flow, {
            teamName: SM.teamName(j),
            teamStatuses: SM.teamStatuses(j),
            teamId: SM.teamId(j),
            teamAnswers: SM.teamAnswers(j)
        });
        card.clicked.connect(function(id) { panelRoot.openTeamDetail(id); });
    }
}

// === Team cards area — centered vertically and horizontally ===
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

                // Placeholder shown when no teams are connected
                Rectangle {
                    objectName: "placeholder"
                    visible: SM.teamCount === 0
                    width: parent.width; height: 60; color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: "⏳ Ожидание подключения команд..."
                        color: AhoraTheme.textSecondary; font.pixelSize: 15
                    }
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    Connections {
        target: SM
        function onTeamCountChanged() {
            rebuildTeamCards();
        }
        function onTeamDataChanged(teamId) {
            rebuildTeamCards();
        }
    }

    // === Team detail popup: full-screen overlay showing answers and grading controls ===
    property string detailTeamId: ""
    property bool detailVisible: false

    function openTeamDetail(teamId) {
        panelRoot.detailTeamId = teamId
        detailPopup.teamDataMap = ({})
        detailPopup.refreshTeamData()
        panelRoot.detailVisible = true
    }

    function closeTeamDetail() {
        panelRoot.detailVisible = false
        panelRoot.detailTeamId = ""
    }

    Rectangle {
        id: detailPopup

        // Reactive data map — updates automatically when teamDataChanged fires
        property var teamDataMap: ({})

        function refreshTeamData() {
            if (panelRoot.detailTeamId !== "") {
                teamDataMap = SM.getTeamDataMap(panelRoot.detailTeamId);
            }
        }

        // React to model changes: rebuild cards when teams join/leave or data updates
        Connections {
            target: SM
            function onTeamDataChanged(teamId) {
                if (teamId === panelRoot.detailTeamId) {
                    detailPopup.refreshTeamData();
                }
            }
        }

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
                    text: detailPopup.teamDataMap.name || "Команда" 
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
                                            var td = detailPopup.teamDataMap
                                            if (td.answers && index < td.answers.length && td.answers[index] !== "")
                                                return "Ответ: " + td.answers[index]
                                            return "Нет ответа"
                                        }
                                    }

                                    // Status
                                    Text {
                                        text: {
                                            var td = detailPopup.teamDataMap
                                            if (td.statuses && index < td.statuses.length) {
                                                var s = td.statuses[index];
                                                if (s === "white") return "⏺ Не отвечен"
                                                if (s === "green") return "✅ Верно"
                                                if (s === "red") return "❌ Неверно"
                                                if (s === "orange") return "⏳ Ожидает проверки"
                                            }
                                            return ""
                                        }
                                        color: {
                                            var statuses = SM.teamStatusesById(panelRoot.detailTeamId)
                                            if (statuses.length <= index) return "#665c54"
                                            if (statuses[index] === "green") return "#98971a"
                                            if (statuses[index] === "red") return "#cc241d"
                                            if (statuses[index] === "orange") return "#d65d0e"
                                            return "#665c54"
                                        }
                                        font.pixelSize: 12
                                        visible: index === 2 && (detailPopup.teamDataMap.statuses ? detailPopup.teamDataMap.statuses[index] === "orange" : false)
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
                        value: detailPopup.teamDataMap.score || 0
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
                            console.log("Saved", scoreSpin.value, "points for", panelRoot.detailTeamId)
                            detailPopup.refreshTeamData()
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
