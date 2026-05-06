#include <QTest>
#include "QuizDef.h"

class TestQuizAnswer : public QObject {
    Q_OBJECT

private slots:
    void testSingleCorrect() {
        QVector<QuestionDef> quiz = {
            {0, "single", "Test?", {"A", "B", "C"}, {"B"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"B"}), "green");
    }

    void testSingleIncorrect() {
        QVector<QuestionDef> quiz = {
            {0, "single", "Test?", {"A", "B", "C"}, {"B"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A"}), "red");
        QCOMPARE(checkQuizAnswer(quiz, 0, {"C"}), "red");
    }

    void testSingleEmpty() {
        QVector<QuestionDef> quiz = {
            {0, "single", "Test?", {"A", "B", "C"}, {"B"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {}), "red");
    }

    void testMultipleAllCorrect() {
        QVector<QuestionDef> quiz = {
            {0, "multiple", "Test?", {"A", "B", "C", "D"}, {"A", "C"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A", "C"}), "green");
    }

    void testMultiplePartialCorrect() {
        QVector<QuestionDef> quiz = {
            {0, "multiple", "Test?", {"A", "B", "C", "D"}, {"A", "C"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A"}), "red");
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A", "B"}), "red");
    }

    void testMultipleAllWrong() {
        QVector<QuestionDef> quiz = {
            {0, "multiple", "Test?", {"A", "B", "C", "D"}, {"A", "C"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"B", "D"}), "red");
    }

    void testMultipleExtraAnswer() {
        QVector<QuestionDef> quiz = {
            {0, "multiple", "Test?", {"A", "B", "C", "D"}, {"A", "C"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A", "B", "C"}), "red");
    }

    void testTextAnswer() {
        QVector<QuestionDef> quiz = {
            {0, "text", "Test?", {}, {}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"anything"}), "orange");
        QCOMPARE(checkQuizAnswer(quiz, 0, {}), "orange");
    }

    void testOutOfBounds() {
        QVector<QuestionDef> quiz = {
            {0, "single", "Test?", {"A", "B"}, {"A"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, -1, {"A"}), "white");
        QCOMPARE(checkQuizAnswer(quiz, 1, {"A"}), "white");
        QCOMPARE(checkQuizAnswer(quiz, 999, {"A"}), "white");
    }

    void testEmptyQuiz() {
        QVector<QuestionDef> quiz;
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A"}), "white");
    }

    void testOrderInsensitive() {
        QVector<QuestionDef> quiz = {
            {0, "multiple", "Test?", {"A", "B", "C"}, {"C", "A"}}
        };
        QCOMPARE(checkQuizAnswer(quiz, 0, {"A", "C"}), "green");
        QCOMPARE(checkQuizAnswer(quiz, 0, {"C", "A"}), "green");
    }
};

QTEST_MAIN(TestQuizAnswer)
#include "test_quizanswer.moc"
