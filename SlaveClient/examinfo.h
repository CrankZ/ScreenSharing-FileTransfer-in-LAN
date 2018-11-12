#ifndef EXAMINFO_H
#define EXAMINFO_H

#include <QString>
#include <QtNetWork>
class ExamInfo
{
public:
    ExamInfo(){}

public:
    // 考试时间，以秒为单位
    int time;
    // 班级名称，可支持多个班级
    QList<QString> className;
    // 第一个是type，然后是每题分数和题型试卷内容
    // 试卷内容包含题目和答案
    QHash<QString, QMap<int, QString>> paper;

    QMap<QString, bool> isRandomMap;
    // 整个试卷的总分
    int paperTotalScore = 0;


    // 以下为学生端发送内容
    // 还是需要MAC，因为学生的学号有可能重复。比如学生在一台电脑上输入好的学号，但是电脑有问题，换了另一台电脑，这就导致重复，在MAP里面会覆盖掉
    QString stuMAC;
    QString stuNo;
    QString stuName;
    QString stuClass;
    int stuRealScore;   // 学生考试的真实成绩
    // 学生的真实答案
    // 题型，题目序号，答案
    QMap<QString, QMap<int, QString>> stuRealAnsChoice;
    // 填空题答案，因为一道填空题可能会有多个问题
    // 所以要分开
    QMap<QString, QMap<int, QMap<int, QString>>> stuRealAnsCom;

    // 每题正确与否
    // 题型，题号，对错
    QMap<QString, QMap<int, bool>> stuAnsIsRight;

    // 题型和分数
    QMap<QString, int> stuEachTypeScore;
public:
    //重构QDataStream的out的<<
    friend QDataStream &operator <<(QDataStream &out, const ExamInfo *examInfo)
    {
        out << examInfo->time;
        out << examInfo->className;
        out << examInfo->paper;
        out << examInfo->isRandomMap;
        out << examInfo->paperTotalScore;

        // 以下为学生端发送内容
        out << examInfo->stuMAC;
        out << examInfo->stuNo;
        out << examInfo->stuName;
        out << examInfo->stuClass;
        out << examInfo->stuRealScore;
        out << examInfo->stuRealAnsChoice;
        out << examInfo->stuRealAnsCom;
        out << examInfo->stuAnsIsRight;
        out << examInfo->stuEachTypeScore;
        return out;
    }

    //重构QDataStream的in的>>
    friend QDataStream &operator >>(QDataStream &in, ExamInfo *examInfo)
    {
        in >> examInfo->time;
        in >> examInfo->className;
        in >> examInfo->paper;
        in >> examInfo->isRandomMap;
        in >> examInfo->paperTotalScore;

        // 以下为学生端发送内容
        in >> examInfo->stuMAC;
        in >> examInfo->stuNo;
        in >> examInfo->stuName;
        in >> examInfo->stuClass;
        in >> examInfo->stuRealScore;
        in >> examInfo->stuRealAnsChoice;
        in >> examInfo->stuRealAnsCom;
        in >> examInfo->stuAnsIsRight;
        in >> examInfo->stuEachTypeScore;
        return in;
    }
};
#endif // EXAMINFO_H
