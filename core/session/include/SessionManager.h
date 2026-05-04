#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H


#include <QAbstractListModel>
#include <QHash>
#include <QString>
#include <QStringList>


class QWebSocket;


// Структура одной команды в модели
struct TeamData {
    QString id;              // UUID команды
    QString name;            // Имя команды
    QStringList statuses;    // Статусы ответов (3 шт): "white","green","red","orange"
    QStringList answers;     // Тексты ответов на каждый вопрос
    QWebSocket *socket;      // Сокет для отправки сообщений клиенту
};

// SessionManager — модель для QML, хранит список подключенных команд
// Каждая строка = одна команда. Роли: name, statuses, teamId
class SessionManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int teamCount READ teamCount NOTIFY teamCountChanged)
    Q_PROPERTY(QStringList questions READ questions CONSTANT)

public:
    // Роли модели для QML делегатов {{{
    enum Roles {
        NameRole = Qt::UserRole + 1,
        StatusesRole,
        TeamIdRole,
        AnswersRole
    };
    // }}}

    explicit SessionManager(QObject *parent = nullptr);

    // QAbstractListModel надстройка {{{
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    // }}}

    // QML-доступные свойства {{{
    int teamCount() const { return m_teams.size(); }
    QStringList questions() const { return m_questions; }
    // }}}

    // Доступ к данным команды по строке {{{
    Q_INVOKABLE QString teamName(int row) const;
    Q_INVOKABLE QStringList teamStatuses(int row) const;
    Q_INVOKABLE QString teamId(int row) const;
    Q_INVOKABLE QStringList teamAnswers(int row) const;
    // }}}

    // Доступ к данным команды по UUID {{{
    Q_INVOKABLE QStringList teamAnswersById(const QString &teamId) const;
    Q_INVOKABLE QStringList teamStatusesById(const QString &teamId) const;
    Q_INVOKABLE QString teamNameById(const QString &teamId) const;
    // }}}

    // Установка баллов за вопрос {{{
    Q_INVOKABLE void setScore(const QString &teamId, int questionId, int score);
    // }}}

    // Ручное подтверждение текстового ответа админом {{{
    Q_INVOKABLE void approveTextAnswer(const QString &teamId, int questionId, bool correct);
    // }}}

    // Методы, вызываемые из WsServer {{{
    QString addTeam(const QString &name, QWebSocket *socket);   // возвращает UUID
    void updateAnswer(const QString &teamId, int questionId, const QString &status, const QStringList &answers = {});
    void removeTeam(const QString &teamId);
    // }}}

    // Поиск команды по сокету {{{
    TeamData *findBySocket(QWebSocket *socket) const;
    // }}}

    // Рассылка timer-сообщений всем подключенным клиентам {{{
    Q_INVOKABLE void broadcastTimerAction(const QString &action);
    // }}}

signals:
    void teamCountChanged();

private:
    QList<TeamData *> m_teams;
    QStringList m_questions;   // Тексты вопросов для QML
};


#endif
