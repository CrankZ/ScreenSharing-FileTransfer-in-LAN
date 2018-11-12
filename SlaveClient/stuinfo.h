#ifndef STUINFO_H
#define STUINFO_H

#include <QString>
#include <QtNetWork>

class StuInfo
{
public:
    StuInfo(){}

public:
    QString stuMAC;
    QString stuNo;
    QString stuName;
    QString stuClass;
    bool stuIsAlive;

public:
    //重构QDataStream的out的<<
    friend QDataStream &operator <<(QDataStream &out, const StuInfo *stuInfo)
    {
        out << stuInfo->stuMAC;
        out << stuInfo->stuNo;
        out << stuInfo->stuName;
        out << stuInfo->stuClass;
        out << stuInfo->stuIsAlive;
        return out;
    }

    //重构QDataStream的in的>>
    friend QDataStream &operator >>(QDataStream &in, StuInfo *stuInfo)
    {
        in >> stuInfo->stuMAC;
        in >> stuInfo->stuNo;
        in >> stuInfo->stuName;
        in >> stuInfo->stuClass;
        in >> stuInfo->stuIsAlive;
        return in;
    }
};
#endif // STUINFO_H
