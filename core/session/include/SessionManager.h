#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H


#include <QAbstractListModel>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>
#include "QuizDef.h"
#include <QVector>
#include "QuizDef.h"


class QWebSocket;


// Structure for a single team in the model
struct TeamData {
    QString id;              // Team UUID
    QString name;            // Team name
    QStringList statuses;    // Answer statuses: "white", "green", "red", "orange"
    QStringList answers;     // Answer text for each question
    int score = 0;           // Team score
    QWebSocket *socket;      // Client socket for sending messages
};

// SessionManager — model for QML, stores all connected teams
// Each row = one team. Roles: name, statuses, teamId
class SessionManager : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int teamCount READ teamCount NOTIFY teamCountChanged)
    Q_PROPERTY(QStringList questions READ questions CONSTANT)

public:
    // Model roles for QML delegates
    enum Roles {
        NameRole = Qt::UserRole + 1,
        StatusesRole,
        TeamIdRole,
        AnswersRole,
        ScoreRole
    };
    // End roles

    explicit SessionManager(QObject *parent = nullptr);

    // QAbstractListModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    // End QAbstractListModel overrides

    // QML-accessible properties
    int teamCount() const { return m_teams.size(); }
    QStringList questions() const { return m_questions; }
    // End QML properties

    const QVector<QuestionDef> &quizQuestions() const { return m_quizQuestions; }

    // Team data access by row index
    Q_INVOKABLE QString teamName(int row) const;
    Q_INVOKABLE QStringList teamStatuses(int row) const;
    Q_INVOKABLE QString teamId(int row) const;
    Q_INVOKABLE QStringList teamAnswers(int row) const;
    Q_INVOKABLE QVariantMap getTeamDataMap(const QString &teamId) const;
    // End row-based access

    // Team data access by UUID
    Q_INVOKABLE QStringList teamAnswersById(const QString &teamId) const;
    Q_INVOKABLE QStringList teamStatusesById(const QString &teamId) const;
    Q_INVOKABLE QString teamNameById(const QString &teamId) const;
    Q_INVOKABLE int teamScoreById(const QString &teamId) const;
    // End UUID-based access

    // Set score for a team
    Q_INVOKABLE void setScore(const QString &teamId, int questionId, int score);
    // End setScore

    // Admin manual approval of text answer
    Q_INVOKABLE void approveTextAnswer(const QString &teamId, int questionId, bool correct);
    // End approveTextAnswer

    // Methods called from WsServer
    QString addTeam(const QString &name, QWebSocket *socket);   // возвращает UUID
    void updateAnswer(const QString &teamId, int questionId, const QString &status, const QStringList &answers = {});
    void removeTeam(const QString &teamId);
    // End WsServer methods

    // Find team by socket
    TeamData *findBySocket(QWebSocket *socket) const;
    // End findBySocket

    // Broadcast timer action to all connected clients
    Q_INVOKABLE void broadcastTimerAction(const QString &action);
    // End broadcastTimerAction

signals:
    void teamCountChanged();
    void teamDataChanged(const QString &teamId);

private:
    QList<TeamData *> m_teams;
    QStringList m_questions;   // Question texts for QML access
    QVector<QuestionDef> m_quizQuestions;
};


#endif
