#ifndef QRCODEITEM_H
#define QRCODEITEM_H


#include <QQuickPaintedItem>
#include <QImage>
#include <QColor>
#include <QSize>
#include <QtQml/QQmlEngine>
#include "qrcodegen.hpp"


class QRCodeItem : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)
    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)
    Q_PROPERTY(int border READ border WRITE setBorder NOTIFY borderChanged)

public:
    QRCodeItem(QQuickItem *parent = nullptr);

    // setters/getters
    QString text() const { return m_text; }
    void setText(const QString &text);

    QColor foreground() const { return m_foreground; }
    void setForeground(const QColor &color);

    QColor background() const { return m_background; }
    void setBackground(const QColor &color);

    int border() const { return m_border; }
    void setBorder(int border);
    // End setters/getters

private:
    void paint(QPainter *painter) override;

signals:
    // signals
    void textChanged();
    void foregroundChanged();
    void backgroundChanged();
    void borderChanged();
    // End signals

private:
    void generateQRImage();

    // QR code parameters
    QString m_text;
    QColor m_foreground{Qt::black};
    QColor m_background{Qt::white};
    // TODO: fix border painting (currently ignored in rendering)
    int m_border{4};
    // End QR code parameters

    QImage m_qrImage;
};


#endif
