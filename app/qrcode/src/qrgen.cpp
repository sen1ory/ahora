#include "qrgen.h"
#include "QR-Code-generator/cpp/qrcodegen.hpp"
#include <QPainter>


using namespace qrcodegen;


QRGen::QRGen() 
        : QQuickImageProvider(QQuickImageProvider::Image)
{}

QImage QRGen::requestImage(const QString &id,
                           QSize *size,
                           const QSize &requestedSize)
{
        QSize imgSize = requestedSize.isValid() ? requestedSize : QSize(200, 200);
        if (size) *size = imgSize;
        
        return generateQRImage(id, imgSize);
}

QImage QRGen::generateQRImage(const QString &text, 
                              const QSize   &imageSize,
                              const QColor  &foreground,
                              const QColor  &background,
                              int border)
{
        return generateQRCodeImage(text, imageSize, foreground, background, border);
}

QImage QRGen::generateQRCodeImage(const QString &text,
                                  const QSize   &imageSize,
                                  const QColor  &foreground,
                                  const QColor  &background,
                                  int border)
{
        try {
                std::string utf8Text = text.toStdString();
                QrCode qr{QrCode::encodeText(utf8Text.c_str(), QrCode::Ecc::LOW)};
                
                int qrSize{qr.getSize()};
                int totalSize{qrSize + (2 * border)};
                
                QImage image(totalSize, totalSize, QImage::Format_ARGB32);
                image.fill(background);
                
                // Draw QR code modules
                for (int y{}; y < qrSize; ++y) {
                        for (int x{}; x < qrSize; ++x) {
                                if (qr.getModule(x, y)) {
                                        image.setPixelColor(border + x, border + y, foreground);
                                }
                        }
                }
                
                // Scale to requested size
                if (imageSize.isValid() && imageSize != QSize(totalSize, totalSize)) {
                        return image.scaled(imageSize, Qt::KeepAspectRatio, Qt::FastTransformation);
                }
                
                return image;
                
        } catch (const std::exception &e) {
                qWarning() << "Failed to generate QR image:" << e.what();
                QImage image(imageSize, QImage::Format_ARGB32);
                image.fill(background);

                return image;
        }
}
