#ifndef IPADRESS_H
#define IPADRESS_H


#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQml/QQmlEngine>


class IpAdress : public QObject {
    Q_OBJECT
    QML_ELEMENT
    // Текущий выбранный IP для QR-кода
    Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)
    // Список ВСЕХ IPv4 адресов (кроме localhost) для выбора
    Q_PROPERTY(QStringList allIps READ allIps NOTIFY ipsChanged)

public:
    IpAdress(QObject *parent = nullptr);

    // геттеры {{{
    QString ip() const { return m_ip; }
    QStringList allIps() const { return m_allIps; }
    // }}}

    // Обновить список IP и попытаться выбрать лучший
    Q_INVOKABLE void refresh();
    // Принудительно выбрать IP из списка
    Q_INVOKABLE void selectIp(const QString &ip);

signals:
    // сигналы {{{
    void ipChanged();
    void ipsChanged();
    // }}}

private:
    // Заполнить m_allIps и выбрать лучший
    void discoverIps();

    // IP параметры {{{
    QString m_ip;
    QStringList m_allIps;
    // }}}
};


#endif