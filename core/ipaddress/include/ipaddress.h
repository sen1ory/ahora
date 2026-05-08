#ifndef IPADRESS_H
#define IPADRESS_H


#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQml/QQmlEngine>


class IpAddress : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // Currently selected IP for QR code
    Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)
    // List of ALL IPv4 addresses (excluding localhost) for manual selection
    Q_PROPERTY(QStringList allIps READ allIps NOTIFY ipsChanged)

public:
    IpAddress(QObject *parent = nullptr);

    // Getters
    QString ip() const { return m_ip; }
    QStringList allIps() const { return m_allIps; }
    // End getters

    // Refresh IP list and attempt to pick the best one
    Q_INVOKABLE void refresh();
    // Manually select an IP from the list
    Q_INVOKABLE void selectIp(const QString &ip);

signals:
    // Signals
    void ipChanged();
    void ipsChanged();
    // End signals

private:
    // Fill m_allIps and select the best one
    void discoverIps();

    // IP parameters
    QString m_ip;
    QStringList m_allIps;
    // End IP parameters
};


#endif