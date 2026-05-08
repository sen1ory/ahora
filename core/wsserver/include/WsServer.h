#ifndef WSSERVER_H
#define WSSERVER_H


#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QHash>
#include <QJsonDocument>


class SessionManager;

// WsServer — WebSocket server on port 8081
// Accepts browser connections, processes JSON messages,
// and passes data to SessionManager
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
    // Handle incoming JSON messages
    void handleJoin(QWebSocket *socket, const QJsonObject &msg);
    void handleAnswer(QWebSocket *socket, const QJsonObject &msg);
    // End handle methods

    // Check answer and return status
    QString checkAnswer(int questionId, const QStringList &answers) const;
    // End checkAnswer

    SessionManager *m_sessionManager;                 // Reference to the team model
    QWebSocketServer *m_server;                       // WebSocket server instance
    QHash<QWebSocket*, QString> m_socketToTeamId;     // Socket → team UUID mapping
};


#endif
