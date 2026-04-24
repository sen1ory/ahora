#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
