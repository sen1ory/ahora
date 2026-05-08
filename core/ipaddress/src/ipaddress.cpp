#include "../include/ipaddress.h"
#include <QNetworkInterface>


IpAddress::IpAddress(QObject *parent)
    : QObject(parent)
{
    discoverIps();
}

// Refresh IP list. Automatically selects the best one:
// 1) Prefers non-WiFi interfaces (hotspot, USB tethering, ethernet)
// 2) Falls back to the first non-loopback IPv4
void IpAddress::refresh() {
    discoverIps();
    emit ipsChanged();
    emit ipChanged();
}

// Force-select an IP from the list
void IpAddress::selectIp(const QString &ip) {
    if (m_allIps.contains(ip) && m_ip != ip) {
        m_ip = ip;
        emit ipChanged();
    }
}

// Fill m_allIps and select the best m_ip
void IpAddress::discoverIps() {
    QStringList newIps;
    QString bestIp;

    // Collect all interfaces with their names
    const QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();

    for (const QNetworkInterface &iface : ifaces) {
        // Skip inactive and loopback interfaces
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

            // Priority: hotspot/USB/ethernet/p2p interfaces rank higher than WiFi
            // "usb" — USB tethering (phone), "eth" — ethernet, "p2p" — WiFi Direct
            // If the current bestIp lacks this priority, replace it
            if (bestIp.isEmpty()) {
                bestIp = ipStr;
            } else {
                bool currentIsWifi = false;
                bool candidateIsWifi = false;

                // Determine interface type for best and candidate
                // WiFi interfaces usually contain "wlan" or "wi-fi" in the name
                QString bestIfaceName = ifaceName; // simplified: compare with current name

                // Find the interface for the current bestIp
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

                // Determine candidate type
                QString candidateType = "wifi";
                if (ifaceName.contains("usb") || ifaceName.contains("eth") ||
                    ifaceName.contains("p2p") || ifaceName.contains("enp") ||
                    ifaceName.contains("ap")) {
                    candidateType = "preferred";
                }

                // If candidate is preferred and current best is WiFi, swap
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