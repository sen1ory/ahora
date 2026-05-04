#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

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

    // === Экспортируем SessionManager в QML как синглтон SM ===
    // Используется как SM.teamCount, SM.teamName(row) и т.д.
    qInfo().noquote() << "[Main] Регистрируем SessionManager в QML как SM";
    qmlRegisterSingletonInstance("Ahora_app_main", 1, 0, "SM", &sessionManager);

    // === Загружаем QML ===
    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
