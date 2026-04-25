#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qrgen.h"


int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("qrgen"), new QRGen());
    engine.loadFromModule("Ahora_app_main", "Main");
    if (engine.rootObjects().isEmpty())
        exit(-1);

    return app.exec();
}
