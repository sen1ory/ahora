#include "../include/WsServer.h"
#include "../../session/include/SessionManager.h"
#include "../../session/include/QuizDef.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QHostAddress>


WsServer::WsServer(SessionManager *sessionManager, quint16 port, QObject *parent)
    : QObject(parent)
    , m_sessionManager(sessionManager)
    , m_server(new QWebSocketServer("Ahora WsServer",
                                    QWebSocketServer::NonSecureMode, this))
{
    // Start WebSocket server on the specified port
    if (m_server->listen(QHostAddress::Any, port)) {
        qInfo().noquote()
            << "[WsServer] Started on port" << port;
        connect(m_server, &QWebSocketServer::newConnection,
                this, &WsServer::onNewConnection);
    } else {
        qWarning().noquote()
            << "[WsServer] Failed to start on port" << port;
    }
}

WsServer::~WsServer() {
    if (m_server) m_server->close();
}

// New WebSocket connection
void WsServer::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;

    qInfo().noquote()
        << "[WsServer] New connection:" << socket->peerAddress().toString();

    // Connect signals
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WsServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WsServer::onDisconnected);
}

// Text message received from client
void WsServer::onTextMessageReceived(const QString &message) {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (!socket) return;

    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject msg = doc.object();
    QString type = msg.value("type").toString();

    if (type == "join") {
        handleJoin(socket, msg);
    } else if (type == "answer") {
        handleAnswer(socket, msg);
    }
}

// Client disconnected
void WsServer::onDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (!socket) return;

    // Find team by socket and remove from model
    QString teamId = m_socketToTeamId.value(socket);
    if (!teamId.isEmpty()) {
        m_sessionManager->removeTeam(teamId);
        m_socketToTeamId.remove(socket);
        qInfo().noquote()
            << "[WsServer] Team disconnected, id:" << teamId;
    }

    socket->deleteLater();
}

// Handle join: register team and send quiz questions
void WsServer::handleJoin(QWebSocket *socket, const QJsonObject &msg) {
    QString name = msg.value("name").toString().trimmed();
    if (name.isEmpty()) {
        QJsonObject err;
        err["type"] = "error";
        err["message"] = "Team name cannot be empty";
        socket->sendTextMessage(
            QJsonDocument(err).toJson(QJsonDocument::Compact));
        return;
    }

    // Register team in SessionManager
    QString teamId = m_sessionManager->addTeam(name, socket);
    m_socketToTeamId[socket] = teamId;

    qInfo().noquote()
        << "[WsServer] Team '" << name << "' joined, id:"
        << teamId;

    // Send join confirmation
    QJsonObject joined;
    joined["type"] = "joined";
    joined["id"] = teamId;
    socket->sendTextMessage(
        QJsonDocument(joined).toJson(QJsonDocument::Compact));

    // Send quiz questions (without correct answers!)
    // We intentionally omit the `correct` field to prevent client-side cheating.
    // Text questions are graded manually by the admin. 
    QJsonObject quizMsg;
    quizMsg["type"] = "quiz";
    QJsonArray questions;
    const auto &quizDefs = m_sessionManager->quizQuestions();
    for (const auto &q : quizDefs) {
        QJsonObject qObj;
        qObj["id"] = q.id;
        qObj["type"] = q.type;
        qObj["text"] = q.text;
        QJsonArray opts;
        for (const auto &opt : q.options) {
            opts.append(opt);
        }
        qObj["options"] = opts;
        questions.append(qObj);
    }
    quizMsg["questions"] = questions;
    socket->sendTextMessage(
        QJsonDocument(quizMsg).toJson(QJsonDocument::Compact));
}

// Handle answer: validate and update status
void WsServer::handleAnswer(QWebSocket *socket, const QJsonObject &msg) {
    // Find team by socket
    QString teamId = m_socketToTeamId.value(socket);
    if (teamId.isEmpty()) return;

    int questionId = msg.value("questionId").toInt(-1);
    if (questionId < 0 || questionId >= m_sessionManager->quizQuestions().size()) return;

    // Collect answers from JSON array
    QJsonArray answersArr = msg.value("answers").toArray();
    QStringList answers;
    for (const auto &a : answersArr) {
        answers.append(a.toString());
    }

    qInfo().noquote() << "[WsServer] Answer received from team" << teamId
                      << "on question" << questionId
                      << "answers:" << answers;

    // Validate answer and get status
    QString status = checkAnswer(questionId, answers);

    // Update status and answers in the model
    m_sessionManager->updateAnswer(teamId, questionId, status, answers);

    // Send result back to client
    QJsonObject result;
    result["type"] = "result";
    result["questionId"] = questionId;
    result["status"] = status;
    socket->sendTextMessage(
        QJsonDocument(result).toJson(QJsonDocument::Compact));
}

// Check answer: returns "green", "red", or "orange"
QString WsServer::checkAnswer(int questionId, const QStringList &answers) const {
    return checkQuizAnswer(m_sessionManager->quizQuestions(), questionId, answers);
}
