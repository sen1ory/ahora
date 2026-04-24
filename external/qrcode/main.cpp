#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qrgen.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("qrgen"), new QRGen());
    
    engine.load(QUrl(QStringLiteral("file:./Main.qml")));
    
    return app.exec();
}
