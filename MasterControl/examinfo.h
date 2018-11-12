#ifndef EXAMINFO_H
#define EXAMINFO_H

#include <QString>
#include <QtNetWork>
class ExamInfo
{
public:
    ExamInfo(){}

public:
    int time;
    QList<QString> className;
    QString paperChoice;
    QString paperCompletion;
    QString paperComprehension;
    int perScoreChoice;
    int perScoreCompletion;
    int mode = 0;
    bool isRandomItem = true;
public:
    //重构QDataStream的out的<<
    friend QDataStream &operator <<(QDataStream &out, const ExamInfo *ei)
    {
        out << ei->time;
        out << ei->className;
        out << ei->isRandomItem;
        out << ei->paperChoice;
        out << ei->paperCompletion;
        out << ei->paperComprehension;
        out << ei->perScoreChoice;
        out << ei->perScoreCompletion;
        out << ei->mode;
        return out;
    }

    //重构QDataStream的in的>>
    friend QDataStream &operator >>(QDataStream &in, ExamInfo *ei)
    {
        in >> ei->time;
        in >> ei->className;
        in >> ei->isRandomItem;
        in >> ei->paperChoice;
        in >> ei->paperCompletion;
        in >> ei->paperComprehension;
        in >> ei->perScoreChoice;
        in >> ei->perScoreCompletion;
        in >> ei->mode;
        return in;
    }
};
#endif // EXAMINFO_H
