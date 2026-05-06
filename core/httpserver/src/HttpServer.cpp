#include "../include/HttpServer.h"
#include "web_content.h"
#include <QHostAddress>
#include <QHostAddress>


HttpServer::HttpServer(quint16 port, QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    // Запускаем TCP-сервер на указанном порту
    if (m_server->listen(QHostAddress::Any, port)) {
        qInfo().noquote()
            << "[HttpServer] Запущен на порту" << port;
        connect(m_server, &QTcpServer::newConnection,
                this, &HttpServer::onNewConnection);
    } else {
        qWarning().noquote()
            << "[HttpServer] Не удалось запустить на порту" << port
            << "-" << m_server->errorString();
    }
}

HttpServer::~HttpServer() {
    if (m_server) m_server->close();
}

// Обработка нового TCP-подключения
void HttpServer::onNewConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;

    // Читаем HTTP-запрос (жду только первую строку + заголовки)
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        QByteArray request = socket->readAll();

        // Парсим первую строку: "GET / HTTP/1.1"
        QList<QByteArray> lines = request.split('\n');
        if (lines.isEmpty()) {
            socket->close();
            return;
        }

        QByteArray requestLine = lines.first().trimmed();
        qInfo().noquote()
            << "[HttpServer] Запрос:" << QString::fromUtf8(requestLine);

        // Поддерживаем только GET /
        if (requestLine.startsWith("GET / ")) {
            // Отдаём HTML-страницу клиента
            QByteArray response = buildResponse("text/html",
                QByteArray(webContentHtml));
            socket->write(response);
        } else {
            // Всё остальное — 404
            QByteArray response = buildResponse("text/plain",
                "404 Not Found", 404);
            socket->write(response);
        }

        socket->flush();
        socket->close();
    });

    // Тайм-аут на случай, если клиент ничего не шлёт
    connect(socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater);
}

// Формирование HTTP-ответа
QByteArray HttpServer::buildResponse(
    const QByteArray &contentType,
    const QByteArray &body,
    int statusCode) const
{
    QByteArray statusText = (statusCode == 200) ? "OK" : "Not Found";

    QByteArray response;
    response += "HTTP/1.1 " + QByteArray::number(statusCode) + " " + statusText + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "\r\n";
    response += body;

    return response;
}
