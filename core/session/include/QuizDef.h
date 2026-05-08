#ifndef QUIZDEF_H
#define QUIZDEF_H


#include <QString>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QSet>


// Structure for a single quiz question
struct QuestionDef {
    int id;              // 0, 1, 2, ...
    QString type;        // "single", "multiple", "text"
    QString text;        // Question text
    QStringList options; // Answer options (empty for text type)
    QStringList correct; // Correct answers (empty for text type, manually reviewed)
};

// Status color meanings:
//   "white"  — unanswered
//   "green"  — correct
//   "red"    — incorrect
//   "orange" — pending manual review (text answers only)
// Text answers always return "orange" because they cannot be auto-graded
inline QString checkQuizAnswer(const QVector<QuestionDef> &quiz, int questionId, const QStringList &answers) {
    if (questionId < 0 || questionId >= quiz.size())
        return "white";

    const auto &q = quiz[questionId];

    if (q.type == "text") {
        return "orange";
    }

    QSet<QString> userSet(answers.begin(), answers.end());
    QSet<QString> correctSet(q.correct.begin(), q.correct.end());

    if (userSet == correctSet) {
        return "green";
    } else {
        return "red";
    }
}

// Hardcoded default 3 quiz questions (used as fallback)
inline const QVector<QuestionDef> s_quizQuestions = {
    {0, "single",   "Какая планета самая большая?",            {"Марс", "Юпитер", "Венера"},                          {"Юпитер"}},
    {1, "multiple", "Какие из этих языков компилируемые?",   {"Python", "C++", "JavaScript", "Rust"},                {"C++", "Rust"}},
    {2, "text",     "Напишите название столицы Франции",  {},                                                     {}}
};


#endif
