#include "../include/HttpServer.h"
#include "web_content.h"
#include <QHostAddress>
#include <QHostAddress>


HttpServer::HttpServer(quint16 port, QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    // Start TCP server on the specified port
    if (m_server->listen(QHostAddress::Any, port)) {
        qInfo().noquote()
            << "[HttpServer] Started on port" << port;
        connect(m_server, &QTcpServer::newConnection,
                this, &HttpServer::onNewConnection);
    } else {
        qWarning().noquote()
            << "[HttpServer] Failed to start on port" << port
            << "-" << m_server->errorString();
    }
}

HttpServer::~HttpServer() {
    if (m_server) m_server->close();
}

// Handle new TCP connection
void HttpServer::onNewConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;

    // Read HTTP request (first line + headers only)
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        QByteArray request = socket->readAll();

        // Parse first line: "GET / HTTP/1.1"
        QList<QByteArray> lines = request.split('\n');
        if (lines.isEmpty()) {
            socket->close();
            return;
        }

        QByteArray requestLine = lines.first().trimmed();
        qInfo().noquote()
            << "[HttpServer] Request:" << QString::fromUtf8(requestLine);

        // Only support GET /
        if (requestLine.startsWith("GET / ")) {
            // Return the HTML client page
            const auto &html = webContentHtml();
            QByteArray response = buildResponse("text/html",
                QByteArray(html.data(), static_cast<int>(html.size())));
            socket->write(response);
        } else {
            // Everything else → 404
            QByteArray response = buildResponse("text/plain",
                "404 Not Found", 404);
            socket->write(response);
        }

        socket->flush();
        socket->close();
    });

    // Timeout in case the client sends nothing
    connect(socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater);
}

// Build HTTP response with status line, content-type, content-length, headers, and body
// This is a minimal implementation — supports only 200 and 404 status codes.
// CORS header is set to allow connections from any origin for mobile clients.
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
