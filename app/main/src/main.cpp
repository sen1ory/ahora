#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// Наши модули
#include "HttpServer.h"
#include "WsServer.h"
#include "SessionManager.h"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // === Создаём и запускаем серверы ===

    // SessionManager — модель команд, доступна из QML
    // Создаётся ДО серверов, потому что WsServer на него ссылается
    SessionManager sessionManager;

    // HttpServer — раздаёт HTML-страницу клиента на порту 8080
    HttpServer httpServer(8080);

    // WsServer — WebSocket для общения с браузерами на порту 8081
    WsServer wsServer(&sessionManager, 8081);

    // === Экспортируем SessionManager в QML ===
    // В QML к нему можно обращаться как sessionManager.teamCount, sessionManager.teamName(row) и т.д.
    qInfo().noquote() << "[Main] Экспортируем SessionManager в QML как sessionManager";
    engine.rootContext()->setContextProperty("sessionManager", &sessionManager);

    // === Загружаем QML ===
    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
