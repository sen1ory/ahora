#include "../include/ipadress.h"
#include <QNetworkInterface>


IpAdress::IpAdress(QObject *parent)
    : QObject(parent)
{
    m_ip = discoverIp();
}

void IpAdress::refresh() {
    QString newIp = discoverIp();
    if (m_ip != newIp) {
        m_ip = newIp;
        emit ipChanged();
    }
}

QString IpAdress::discoverIp() {
    for (const QHostAddress &addr : QNetworkInterface::allAddresses()) {
        if (addr != QHostAddress::LocalHost && addr.protocol() == QAbstractSocket::IPv4Protocol) {
            return addr.toString();
        }
    }
    return QStringLiteral("127.0.0.1");
}
