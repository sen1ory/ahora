#ifndef QUIZDEF_H
#define QUIZDEF_H


#include <QString>
#include <QStringList>
#include <QVector>


// Структура одного вопроса квиза
struct QuestionDef {
    int id;              // 0, 1, 2
    QString type;        // "single", "multiple", "text"
    QString text;        // Текст вопроса
    QStringList options; // Варианты ответов (пусто для text)
    QStringList correct; // Правильные ответы (пусто для text, проверяется вручную)
};

// Жёстко заданные 3 вопроса квиза
inline const QVector<QuestionDef> s_quizQuestions = {
    {0, "single",   "Какая планета самая большая?",           {"Марс", "Юпитер", "Венера"},                      {"Юпитер"}},
    {1, "multiple", "Какие из этих языков компилируемые?",    {"Python", "C++", "JavaScript", "Rust"},            {"C++", "Rust"}},
    {2, "text",     "Напишите название столицы Франции",      {},                                                 {}}
};


#endif
