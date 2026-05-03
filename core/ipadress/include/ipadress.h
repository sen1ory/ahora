#ifndef IPADRESS_H
#define IPADRESS_H


#include <QObject>
#include <QString>
#include <QtQml/QQmlEngine>


class IpAdress : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)

public:
    IpAdress(QObject *parent = nullptr);

    // getters {{{
    QString ip() const { return m_ip; }
    // }}}

    Q_INVOKABLE void refresh();

signals:
    // signals {{{
    void ipChanged();
    // }}}

private:
    QString discoverIp();

    // ip parameters {{{
    QString m_ip;
    // }}}
};


#endif
