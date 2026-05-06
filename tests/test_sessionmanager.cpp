#include <QTest>
#include <QSignalSpy>
#include "SessionManager.h"

class TestSessionManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        m_manager = new SessionManager(this);
    }

    void testInitialTeamCount() {
        QCOMPARE(m_manager->teamCount(), 0);
        QCOMPARE(m_manager->questions().size(), 3);
    }

    void testAddTeam() {
        QSignalSpy spy(m_manager, &SessionManager::teamCountChanged);

        QString id = m_manager->addTeam("TestTeam", nullptr);

        QVERIFY(!id.isEmpty());
        QCOMPARE(m_manager->teamCount(), 1);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(m_manager->teamName(0), "TestTeam");
        QCOMPARE(m_manager->teamId(0), id);
        QCOMPARE(m_manager->teamStatuses(0), QStringList({"white", "white", "white"}));
        QCOMPARE(m_manager->teamAnswers(0), QStringList({"", "", ""}));
    }

    void testTeamLookupById() {
        QString id = m_manager->addTeam("LookupTeam", nullptr);

        QCOMPARE(m_manager->teamNameById(id), "LookupTeam");
        QCOMPARE(m_manager->teamStatusesById(id), QStringList({"white", "white", "white"}));
        QCOMPARE(m_manager->teamAnswersById(id), QStringList({"", "", ""}));
    }

    void testLookupNonexistent() {
        QVERIFY(m_manager->teamNameById("nonexistent-id").isEmpty());
        QVERIFY(m_manager->teamAnswersById("nonexistent-id").isEmpty());
        QVERIFY(m_manager->teamStatusesById("nonexistent-id").isEmpty());
        QCOMPARE(m_manager->teamScoreById("nonexistent-id"), 0);
    }

    void testUpdateAnswer() {
        QString id = m_manager->addTeam("AnswerTeam", nullptr);
        m_manager->updateAnswer(id, 0, "green", {"Юпитер"});
        m_manager->updateAnswer(id, 1, "red", {"Python"});
        m_manager->updateAnswer(id, 2, "orange", {"Париж"});

        QCOMPARE(m_manager->teamStatusesById(id), QStringList({"green", "red", "orange"}));
        QCOMPARE(m_manager->teamAnswersById(id), QStringList({"Юпитер", "Python", "Париж"}));
    }

    void testSetScore() {
        QString id = m_manager->addTeam("ScoreTeam", nullptr);

        m_manager->setScore(id, 0, 42);

        QCOMPARE(m_manager->teamScoreById(id), 42);
    }

    void testApproveTextAnswer() {
        QString id = m_manager->addTeam("TextTeam", nullptr);
        m_manager->updateAnswer(id, 2, "orange", {"Париж"});

        m_manager->approveTextAnswer(id, 2, true);
        QCOMPARE(m_manager->teamStatusesById(id).at(2), "green");

        m_manager->updateAnswer(id, 2, "orange", {"Лондон"});
        m_manager->approveTextAnswer(id, 2, false);
        QCOMPARE(m_manager->teamStatusesById(id).at(2), "red");
    }

    void testRemoveTeam() {
        QSignalSpy spy(m_manager, &SessionManager::teamCountChanged);
        int initialCount = m_manager->teamCount();

        QString id = m_manager->addTeam("RemoveTeam", nullptr);
        QCOMPARE(m_manager->teamCount(), initialCount + 1);

        m_manager->removeTeam(id);
        QCOMPARE(m_manager->teamCount(), initialCount);
        QVERIFY(m_manager->teamNameById(id).isEmpty());
    }

    void testRemoveNonexistent() {
        int count = m_manager->teamCount();
        m_manager->removeTeam("nonexistent");
        QCOMPARE(m_manager->teamCount(), count);
    }

    void testModelRoles() {
        QString id = m_manager->addTeam("ModelTeam", nullptr);
        QModelIndex idx = m_manager->index(m_manager->teamCount() - 1);

        QCOMPARE(m_manager->data(idx, SessionManager::NameRole).toString(), "ModelTeam");
        QCOMPARE(m_manager->data(idx, SessionManager::TeamIdRole).toString(), id);
        QCOMPARE(m_manager->data(idx, SessionManager::ScoreRole).toInt(), 0);
    }

    void testMultipleTeamsRowOrder() {
        int before = m_manager->teamCount();
        QString id1 = m_manager->addTeam("TeamA", nullptr);
        QString id2 = m_manager->addTeam("TeamB", nullptr);

        QCOMPARE(m_manager->teamName(before), "TeamA");
        QCOMPARE(m_manager->teamName(before + 1), "TeamB");
    }

    void testBroadcastTimer() {
        m_manager->broadcastTimerAction("pause");
        m_manager->broadcastTimerAction("resume");
    }

private:
    SessionManager *m_manager = nullptr;
};

QTEST_MAIN(TestSessionManager)
#include "test_sessionmanager.moc"
