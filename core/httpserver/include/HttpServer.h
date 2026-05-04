#ifndef HTTPSERVER_H
#define HTTPSERVER_H


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>


// HttpServer — минимальный HTTP сервер на порту 8080
// Отдаёт статическую HTML-страницу (веб-клиент квиза)
// Никаких роутов, сложной логики — только GET / → страница
class HttpServer : public QObject {
    Q_OBJECT

public:
    explicit HttpServer(quint16 port = 8080, QObject *parent = nullptr);
    ~HttpServer();

private slots:
    void onNewConnection();

private:
    // Формирование HTTP-ответа {{{
    QByteArray buildResponse(const QByteArray &contentType,
                             const QByteArray &body,
                             int statusCode = 200) const;
    // }}}

    QTcpServer *m_server;
};


#endif
