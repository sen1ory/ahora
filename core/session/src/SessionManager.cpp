#include "../include/SessionManager.h"
#include "../include/QuizDef.h"
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>


SessionManager::SessionManager(QObject *parent)
    : QAbstractListModel(parent)
{
    m_questions.clear();
    for (const auto &q : s_quizQuestions) {
        m_questions.append(q.text);
    }
}

int SessionManager::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_teams.size();
}

QVariant SessionManager::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_teams.size())
        return {};

    const TeamData *team = m_teams.at(index.row());

    switch (role) {
    case NameRole:
        return team->name;
    case StatusesRole:
        return team->statuses;
    case TeamIdRole:
        return team->id;
    case AnswersRole:
        return team->answers;
    default:
        return {};
    }
}

QHash<int, QByteArray> SessionManager::roleNames() const {
    return {
        {NameRole,     "name"},
        {StatusesRole, "statuses"},
        {TeamIdRole,   "teamId"},
        {AnswersRole,  "answers"}
    };
}

QString SessionManager::teamName(int row) const {
    if (row < 0 || row >= m_teams.size()) return {};
    return m_teams.at(row)->name;
}

QStringList SessionManager::teamStatuses(int row) const {
    if (row < 0 || row >= m_teams.size()) return {};
    return m_teams.at(row)->statuses;
}

QString SessionManager::teamId(int row) const {
    if (row < 0 || row >= m_teams.size()) return {};
    return m_teams.at(row)->id;
}

QStringList SessionManager::teamAnswers(int row) const {
    if (row < 0 || row >= m_teams.size()) return {};
    return m_teams.at(row)->answers;
}

QStringList SessionManager::teamAnswersById(const QString &teamId) const {
    for (const auto *team : m_teams) {
        if (team->id == teamId) return team->answers;
    }
    return {};
}

QStringList SessionManager::teamStatusesById(const QString &teamId) const {
    for (const auto *team : m_teams) {
        if (team->id == teamId) return team->statuses;
    }
    return {};
}

QString SessionManager::teamNameById(const QString &teamId) const {
    for (const auto *team : m_teams) {
        if (team->id == teamId) return team->name;
    }
    return {};
}

void SessionManager::setScore(const QString &teamId, int questionId, int score) {
    Q_UNUSED(teamId)
    Q_UNUSED(questionId)
    Q_UNUSED(score)
}

void SessionManager::approveTextAnswer(const QString &teamId, int questionId, bool correct) {
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i]->id == teamId) {
            m_teams[i]->statuses[questionId] = correct ? "green" : "red";
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {StatusesRole});

            if (m_teams[i]->socket && m_teams[i]->socket->isValid()) {
                QJsonObject result;
                result["type"] = "result";
                result["questionId"] = questionId;
                result["status"] = m_teams[i]->statuses[questionId];
                m_teams[i]->socket->sendTextMessage(
                    QJsonDocument(result).toJson(QJsonDocument::Compact));
            }
            return;
        }
    }
}

QString SessionManager::addTeam(const QString &name, QWebSocket *socket) {
    const QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    beginInsertRows(QModelIndex(), m_teams.size(), m_teams.size());
    auto *team = new TeamData;
    team->id = id;
    team->name = name;
    team->statuses = {"white", "white", "white"};
    team->answers = {"", "", ""};
    team->socket = socket;
    m_teams.append(team);
    endInsertRows();

    emit teamCountChanged();

    qInfo().noquote() << "[SessionManager] Команда добавлена:" << name << "id:" << id
                      << "Всего команд:" << m_teams.size();
    return id;
}

void SessionManager::updateAnswer(const QString &teamId, int questionId, const QString &status, const QStringList &answers) {
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i]->id == teamId) {
            m_teams[i]->statuses[questionId] = status;
            if (!answers.isEmpty()) {
                m_teams[i]->answers[questionId] = answers.join(", ");
            }
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {StatusesRole, AnswersRole});

            qInfo().noquote() << "[SessionManager] Ответ от" << m_teams[i]->name
                              << "на вопрос" << questionId
                              << "статус:" << status
                              << "ответ:" << m_teams[i]->answers[questionId];
            return;
        }
    }
}

void SessionManager::removeTeam(const QString &teamId) {
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i]->id == teamId) {
            beginRemoveRows(QModelIndex(), i, i);
            delete m_teams[i];
            m_teams.removeAt(i);
            endRemoveRows();
            emit teamCountChanged();
            qInfo().noquote() << "[SessionManager] Команда удалена, id:" << teamId;
            return;
        }
    }
}

TeamData *SessionManager::findBySocket(QWebSocket *socket) const {
    for (auto *team : m_teams) {
        if (team->socket == socket) return team;
    }
    return nullptr;
}

void SessionManager::broadcastTimerAction(const QString &action) {
    QJsonObject msg;
    msg["type"] = "timer";
    msg["action"] = action;
    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact);
    for (auto *team : m_teams) {
        if (team->socket && team->socket->isValid()) {
            team->socket->sendTextMessage(QString::fromUtf8(data));
        }
    }
}
