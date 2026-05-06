#ifndef QUIZLOADER_H
#define QUIZLOADER_H


#include "QuizDef.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDir>


inline QVector<QuestionDef> loadQuizFromFile() {
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/quiz.json",
        QDir::currentPath() + "/quiz.json"
    };

    for (const QString &path : searchPaths) {
        QFile file(path);
        if (!file.exists()) continue;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        file.close();

        if (parseError.error != QJsonParseError::NoError) {
            qWarning().noquote() << "[QuizLoader] JSON parse error in" << path
                                 << ":" << parseError.errorString() << "- using defaults";
            continue;
        }

        if (!doc.isObject()) {
            qWarning().noquote() << "[QuizLoader] Invalid JSON in" << path << "- using defaults";
            continue;
        }

        QJsonArray questionsArr = doc.object().value("questions").toArray();
        if (questionsArr.isEmpty()) {
            qWarning().noquote() << "[QuizLoader] No questions in" << path << "- using defaults";
            continue;
        }

        QVector<QuestionDef> quiz;
        for (const QJsonValue &val : questionsArr) {
            QJsonObject qObj = val.toObject();
            QuestionDef q;
            q.id = quiz.size();
            q.type = qObj.value("type").toString();
            q.text = qObj.value("text").toString();

            const QJsonArray optsArr = qObj.value("options").toArray();
            for (const QJsonValue &opt : optsArr) {
                q.options.append(opt.toString());
            }

            const QJsonArray corrArr = qObj.value("correct").toArray();
            for (const QJsonValue &corr : corrArr) {
                q.correct.append(corr.toString());
            }

            quiz.append(q);
        }

        qInfo().noquote() << "[QuizLoader] Loaded" << quiz.size()
                          << "questions from" << path;
        return quiz;
    }

    qInfo().noquote() << "[QuizLoader] No quiz.json found, using hardcoded defaults ("
                       << s_quizQuestions.size() << " questions)";
    return s_quizQuestions;
}


#endif
