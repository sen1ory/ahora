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
    // Запускаем WebSocket-сервер на указанном порту
    if (m_server->listen(QHostAddress::Any, port)) {
        qInfo().noquote()
            << "[WsServer] Запущен на порту" << port;
        connect(m_server, &QWebSocketServer::newConnection,
                this, &WsServer::onNewConnection);
    } else {
        qWarning().noquote()
            << "[WsServer] Не удалось запустить на порту" << port;
    }
}

WsServer::~WsServer() {
    if (m_server) m_server->close();
}

// Новое WebSocket-подключение
void WsServer::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;

    qInfo().noquote()
        << "[WsServer] Новое подключение:" << socket->peerAddress().toString();

    // Подключаем сигналы
    connect(socket, &QWebSocket::textMessageReceived,
            this, &WsServer::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected,
            this, &WsServer::onDisconnected);
}

// Получено текстовое сообщение от клиента
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

// Клиент отключился
void WsServer::onDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (!socket) return;

    // Находим команду по сокету и удаляем из модели
    QString teamId = m_socketToTeamId.value(socket);
    if (!teamId.isEmpty()) {
        m_sessionManager->removeTeam(teamId);
        m_socketToTeamId.remove(socket);
        qInfo().noquote()
            << "[WsServer] Команда отключилась, id:" << teamId;
    }

    socket->deleteLater();
}

// Обработка join: регистрируем команду и отправляем вопросы
void WsServer::handleJoin(QWebSocket *socket, const QJsonObject &msg) {
    QString name = msg.value("name").toString().trimmed();
    if (name.isEmpty()) {
        QJsonObject err;
        err["type"] = "error";
        err["message"] = "Имя команды не может быть пустым";
        socket->sendTextMessage(
            QJsonDocument(err).toJson(QJsonDocument::Compact));
        return;
    }

    // Регистрируем команду в SessionManager
    QString teamId = m_sessionManager->addTeam(name, socket);
    m_socketToTeamId[socket] = teamId;

    qInfo().noquote()
        << "[WsServer] Команда '" << name << "' присоединилась, id:"
        << teamId;

    // Отправляем подтверждение регистрации
    QJsonObject joined;
    joined["type"] = "joined";
    joined["id"] = teamId;
    socket->sendTextMessage(
        QJsonDocument(joined).toJson(QJsonDocument::Compact));

    // Отправляем вопросы квиза (без правильных ответов!)
    QJsonObject quizMsg;
    quizMsg["type"] = "quiz";
    QJsonArray questions;
    for (const auto &q : s_quizQuestions) {
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

// Обработка answer: проверяем ответ и обновляем статус
void WsServer::handleAnswer(QWebSocket *socket, const QJsonObject &msg) {
    // Находим команду по сокету
    QString teamId = m_socketToTeamId.value(socket);
    if (teamId.isEmpty()) return;

    int questionId = msg.value("questionId").toInt(-1);
    if (questionId < 0 || questionId >= s_quizQuestions.size()) return;

    // Собираем ответы из JSON массива
    QJsonArray answersArr = msg.value("answers").toArray();
    QStringList answers;
    for (const auto &a : answersArr) {
        answers.append(a.toString());
    }

    qInfo().noquote() << "[WsServer] Получен ответ от команды" << teamId
                      << "на вопрос" << questionId
                      << "ответы:" << answers;

    // Проверяем ответ и получаем статус
    QString status = checkAnswer(questionId, answers);

    // Обновляем статус и ответы в модели
    m_sessionManager->updateAnswer(teamId, questionId, status, answers);

    // Отправляем результат клиенту
    QJsonObject result;
    result["type"] = "result";
    result["questionId"] = questionId;
    result["status"] = status;
    socket->sendTextMessage(
        QJsonDocument(result).toJson(QJsonDocument::Compact));
}

// Проверка ответа: возвращает "green", "red" или "orange"
QString WsServer::checkAnswer(int questionId, const QStringList &answers) const {
    if (questionId < 0 || questionId >= s_quizQuestions.size())
        return "white";

    const auto &q = s_quizQuestions[questionId];

    if (q.type == "text") {
        // Текстовый ответ — проверяется админом вручную позже
        return "orange";
    }

    // Для single/multiple: сравниваем множества ответов
    QSet<QString> userSet(answers.begin(), answers.end());
    QSet<QString> correctSet(q.correct.begin(), q.correct.end());

    if (userSet == correctSet) {
        return "green";
    } else {
        return "red";
    }
}
