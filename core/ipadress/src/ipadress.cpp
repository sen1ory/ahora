#include "../include/ipadress.h"
#include <QNetworkInterface>


IpAdress::IpAdress(QObject *parent)
    : QObject(parent)
{
    discoverIps();
}

// Обновить список адресов. Автоматически выбирает лучший:
// 1) Предпочитает адреса не на WiFi-интерфейсах (хотспот, usb, ethernet)
// 2) Если таких нет — берёт первый не-loopback IPv4
void IpAdress::refresh() {
    discoverIps();
    emit ipsChanged();
    emit ipChanged();
}

// Принудительно выбрать IP из списка
void IpAdress::selectIp(const QString &ip) {
    if (m_allIps.contains(ip) && m_ip != ip) {
        m_ip = ip;
        emit ipChanged();
    }
}

// Заполняем m_allIps и выбираем лучший m_ip
void IpAdress::discoverIps() {
    QStringList newIps;
    QString bestIp;

    // Собираем все интерфейсы с их именами
    const QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface &iface : ifaces) {
        // Пропускаем неактивные и loopback
        if (!(iface.flags() & QNetworkInterface::IsUp))
            continue;
        if (iface.flags() & QNetworkInterface::IsLoopBack)
            continue;

        const QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            QHostAddress addr = entry.ip();
            if (addr.protocol() != QAbstractSocket::IPv4Protocol)
                continue;

            QString ipStr = addr.toString();
            newIps.append(ipStr);

            QString ifaceName = iface.humanReadableName().toLower();

            // Приоритет: хотспот/usb/eth/p2p интерфейсы выше чем wifi
            // "usb" — usb-терринг (телефон), "eth" — ethernet, "p2p" — WiFi Direct
            // Если у текущего bestIp нет такого приоритета — заменяем
            if (bestIp.isEmpty()) {
                bestIp = ipStr;
            } else {
                bool currentIsWifi = false;
                bool candidateIsWifi = false;

                // Определяем тип интерфейса для best и для кандидата
                // Wifi интерфейсы обычно содержат "wlan" или "wi-fi" в имени
                QString bestIfaceName = ifaceName; // упрощённо: сравниваем с текущим именем

                // Ищем интерфейс для текущего bestIp
                QString bestIfaceType = "wifi"; // по умолчанию считаем wifi
                for (const QNetworkInterface &bi : ifaces) {
                    const QList<QNetworkAddressEntry> be = bi.addressEntries();
                    for (const QNetworkAddressEntry &bentry : be) {
                        if (bentry.ip().toString() == bestIp) {
                            QString biName = bi.humanReadableName().toLower();
                            if (biName.contains("usb") || biName.contains("eth") ||
                                biName.contains("p2p") || biName.contains("enp") ||
                                biName.contains("ap")) {
                                bestIfaceType = "preferred";
                            }
                            break;
                        }
                    }
                }

                // Определяем тип кандидата
                QString candidateType = "wifi";
                if (ifaceName.contains("usb") || ifaceName.contains("eth") ||
                    ifaceName.contains("p2p") || ifaceName.contains("enp") ||
                    ifaceName.contains("ap")) {
                    candidateType = "preferred";
                }

                // Если кандидат preferred, а текущий best — wifi, меняем
                if (candidateType == "preferred" && bestIfaceType == "wifi") {
                    bestIp = ipStr;
                }
            }
        }
    }

    m_allIps = newIps;

    if (!bestIp.isEmpty() && bestIp != m_ip) {
        m_ip = bestIp;
    } else if (m_ip.isEmpty() && !newIps.isEmpty()) {
        m_ip = newIps.first();
    }
}