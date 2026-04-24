#ifndef IMAGEQRCODEGENERATOR_H
#define IMAGEQRCODEGENERATOR_H


#include <QObject>
#include <QImage>
#include <QQuickImageProvider>


class QRGen : public QQuickImageProvider {
Q_OBJECT
    
public:
    QRGen();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    
    Q_INVOKABLE QImage generateQRImage(const QString &text, 
                                       const QSize   &imageSize = QSize(200, 200),
                                       const QColor  &foreground = Qt::black,
                                       const QColor  &background = Qt::white,
                                       int border = 4);
    
private:
    QImage generateQRCodeImage(const QString &text, 
                               const QSize   &imageSize,
                               const QColor  &foreground,
                               const QColor  &background,
                               int border);
};


#endif // IMAGEQRCODEGENERATOR_H
