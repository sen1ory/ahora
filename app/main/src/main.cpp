#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

// custom modules
#include "HttpServer.h"
#include "WsServer.h"
#include "SessionManager.h"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;


    // SessionManager — commands, accessible from qml
    // before server, WsServer references it
    SessionManager sessionManager;

    // HttpServer — share HTML-pages of the client on port 8080
    HttpServer httpServer(8080);

    // WsServer — WebSocket for browser communications on port 8081
    WsServer wsServer(&sessionManager, 8081);

    // === export SessionManager to QML as singleton SM ===
    // used as SM.teamCount, SM.teamName(row) and so on
    qInfo().noquote() << "[Main] Регистрируем SessionManager в QML как SM";
    qmlRegisterSingletonInstance("Ahora_app_main", 1, 0, "SM", &sessionManager);

    // === QML loading ===
    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
