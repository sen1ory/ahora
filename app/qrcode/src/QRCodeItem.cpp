#include "../include/QRCodeItem.h"
#include <QPainter>

using namespace qrcodegen;

QRCodeItem::QRCodeItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{}

void QRCodeItem::setText(const QString &text) {
    if (m_text != text) {
        m_text = text;
        emit textChanged();
        generateQRImage();
        update();
    }
}

void QRCodeItem::setForeground(const QColor &color) {
    if (m_foreground != color) {
        m_foreground = color;
        emit foregroundChanged();
        generateQRImage();
        update();
    }
}

void QRCodeItem::setBackground(const QColor &color) {
    if (m_background != color) {
        m_background = color;
        emit backgroundChanged();
        generateQRImage();
        update();
    }
}

void QRCodeItem::setBorder(int border) {
    if (m_border != border) {
        m_border = border;
        emit borderChanged();
        generateQRImage();
        update();
    }
}

void QRCodeItem::generateQRImage() {
    if (m_text.isEmpty()) {
        m_qrImage = QImage();
        return;
    }

    try {
        std::string utf8Text = m_text.toStdString();
        QrCode qr{QrCode::encodeText(utf8Text.c_str(), QrCode::Ecc::LOW)};

        int qrSize{qr.getSize()};
        int totalSize{qrSize + (2 * m_border)};

        m_qrImage = QImage(totalSize, totalSize, QImage::Format_ARGB32);
        m_qrImage.fill(m_background);

        for (int y{}; y < qrSize; ++y) {
            for (int x{}; x < qrSize; ++x) {
                if (qr.getModule(x, y)) {
                    m_qrImage.setPixelColor(m_border + x, m_border + y, m_foreground);
                }
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "Failed to generate QR image:" << e.what();
        m_qrImage = QImage();
    }
}

void QRCodeItem::paint(QPainter *painter) {
    if (m_qrImage.isNull()) {
        return;
    }

    QImage scaled = m_qrImage.scaled(
        QSize(width(), height()),
        Qt::KeepAspectRatio,
        Qt::FastTransformation
    );

    int x = (width() - scaled.width()) / 2;
    int y = (height() - scaled.height()) / 2;

    painter->drawImage(x, y, scaled);
}
