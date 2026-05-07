import QtQuick
import QtQuick.Controls
import Ahora_app_main


// TeamCard — карточка одной команды на панели администратора
Rectangle {
    id: card

    required property string teamName
    required property variant teamStatuses
    required property string teamId
    required property variant teamAnswers

    signal clicked(string teamId)

    width: 240
    height: column.implicitHeight + 20
    radius: 8
    color: "#282828"
    border.color: "#3c3836"
    border.width: 1

    MouseArea {
        anchors.fill: parent
        onClicked: card.clicked(card.teamId)
        cursorShape: Qt.PointingHandCursor
    }

    Column {
        id: column
        anchors {
            fill: parent
            margins: 10
        }
        spacing: 6

        // Имя команды
        Text {
            text: card.teamName
            color: "#ebdbb2"
            font.bold: true
            font.pixelSize: 16
            elide: Text.ElideRight
            width: parent.width
        }

        // Разделитель
        Rectangle {
            width: parent.width
            height: 1
            color: "#3c3836"
        }

        // Список вопросов с цветовыми индикаторами
        Repeater {
            model: card.teamStatuses.length

            delegate: Item {
                width: parent.width
                height: 20
                visible: true

                property string status: card.teamStatuses[index] || "white"
                property string answer: (card.teamAnswers && index < card.teamAnswers.length) ? card.teamAnswers[index] : ""

                // Цветовой кружок
                Rectangle {
                    id: statusDot
                    width: 12
                    height: 12
                    radius: 6
                    anchors.verticalCenter: parent.verticalCenter

                    color: {
                        if (status === "green")  return AhoraTheme.accentGreen
                        if (status === "red")    return AhoraTheme.accentRed
                        if (status === "orange") return AhoraTheme.accentOrange
                        return AhoraTheme.bgMuted
                    }
                }

                // Текст вопроса
                Text {
                    anchors {
                        left: statusDot.right
                        leftMargin: 8
                        right: parent.right
                        rightMargin: 4
                        verticalCenter: parent.verticalCenter
                    }
                    text: {
                        var qs = SM ? SM.questions : []
                        if (qs.length > index) return qs[index]
                        return ""
                    }
                    color: "#ebdbb2"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                }
            }
        }
    }
}
