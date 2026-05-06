#ifndef QUIZDEF_H
#define QUIZDEF_H


#include <QString>
#include <QStringList>
#include <QVector>
#include <QSet>
#include <QSet>


// Структура одного вопроса квиза
struct QuestionDef {
    int id;              // 0, 1, 2
    QString type;        // "single", "multiple", "text"
    QString text;        // Текст вопроса
    QStringList options; // Варианты ответов (пусто для text)
    QStringList correct; // Правильные ответы (пусто для text, проверяется вручную)
};

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

// Жёстко заданные 3 вопроса квиза
inline const QVector<QuestionDef> s_quizQuestions = {
    {0, "single",   "Какая планета самая большая?",           {"Марс", "Юпитер", "Венера"},                      {"Юпитер"}},
    {1, "multiple", "Какие из этих языков компилируемые?",    {"Python", "C++", "JavaScript", "Rust"},            {"C++", "Rust"}},
    {2, "text",     "Напишите название столицы Франции",      {},                                                 {}}
};


#endif
