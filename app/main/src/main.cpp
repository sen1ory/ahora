#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

// custom module includes
#include "HttpServer.h"
#include "WsServer.h"
#include "SessionManager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // --- AhoraTheme singleton (QML singleton requires manual instantiation) ---
    QQmlComponent themeComponent(&engine,
        QUrl("qrc:/qt/qml/Ahora_app_main/qml/AhoraTheme.qml"));
    QObject *ahoraTheme = themeComponent.create();
    if (!ahoraTheme) {
        qCritical() << "Failed to create AhoraTheme:" << themeComponent.errorString();
        return -1;
    }
    qmlRegisterSingletonInstance("Ahora_app_main", 1, 0, "AhoraTheme", ahoraTheme);
    // --- End AhoraTheme setup ------------------------------------

    SessionManager sessionManager;
    HttpServer httpServer(8080);
    WsServer wsServer(&sessionManager, 8081);

    qInfo().noquote() << "[Main] Registering SessionManager in QML as SM";
    qmlRegisterSingletonInstance("Ahora_app_main", 1, 0, "SM", &sessionManager);

    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
