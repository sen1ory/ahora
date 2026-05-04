#ifndef WSSERVER_H
#define WSSERVER_H


#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QHash>
#include <QJsonDocument>


class SessionManager;

// WsServer — WebSocket сервер на порту 8081
// Принимает подключения от браузеров, обрабатывает JSON-сообщения
// и передаёт данные в SessionManager
class WsServer : public QObject {
    Q_OBJECT

public:
    explicit WsServer(SessionManager *sessionManager, quint16 port = 8081, QObject *parent = nullptr);
    ~WsServer();

private slots:
    void onNewConnection();
    void onTextMessageReceived(const QString &message);
    void onDisconnected();

private:
    // Обработка входящих JSON сообщений {{{
    void handleJoin(QWebSocket *socket, const QJsonObject &msg);
    void handleAnswer(QWebSocket *socket, const QJsonObject &msg);
    // }}}

    // Проверка ответа и возврат статуса {{{
    QString checkAnswer(int questionId, const QStringList &answers) const;
    // }}}

    SessionManager *m_sessionManager;                 // ссылка на модель команд
    QWebSocketServer *m_server;                       // сам WebSocket-сервер
    QHash<QWebSocket*, QString> m_socketToTeamId;     // сокет → UUID команды
};


#endif
