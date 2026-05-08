#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>


// HttpServer — minimal HTTP server on port 8080
// Serves a static HTML page (the web quiz client)
// No routing or complex logic — just GET / → page
class HttpServer : public QObject {
    Q_OBJECT

public:
    explicit HttpServer(quint16 port = 8080, QObject *parent = nullptr);
    ~HttpServer();

private slots:
    void onNewConnection();

private:
    // Builds an HTTP response with status line, headers, and body
    QByteArray buildResponse(const QByteArray &contentType,
                             const QByteArray &body,
                             int statusCode = 200) const;
    QTcpServer *m_server;
};

#endif
