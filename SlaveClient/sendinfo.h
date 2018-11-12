#ifndef SENDINFO_H
#define SENDINFO_H

#include <QString>
#include <QMap>
#include <QtNetWork>


class SendInfo
{
public:
    SendInfo(){}

public:
    QString MAC;
    QString stuNo;
    QString stuName;
    QString stuClass;

    QString paperChoice;//自带题目文件，防止没有题目文件
    QString paperCompletion;//自带题目文件，防止没有题目文件
    QString paperComprehension;
    int score = 0;
    int totalScore = 100;

    QMap <int,QString> realAns;//存题号和答案
    QMap <int,bool> realAnsIsRight;//存题号和对错

    QMap <int,QString> realAnsCom;//存题号和答案
    QMap <int,bool> realAnsIsRightCom;//存题号和对错
public:

    void setTotalScore(int totalScore)
    {
        this->totalScore = totalScore;
    }

    int getTotalScore()
    {
        return totalScore;
    }

    void setPaperChoice(QString paperChoice)
    {
        this->paperChoice = paperChoice;
    }
    QString getPaperChoice()
    {
        return paperChoice;
    }

    void setpaperCompletion(QString paperCompletion)
    {
        this->paperCompletion = paperCompletion;
    }
    QString getpaperCompletion()
    {
        return paperCompletion;
    }

    void setpaperComprehension(QString paperComprehension)
    {
        this->paperComprehension = paperComprehension;
    }
    QString getpaperComprehension()
    {
        return paperComprehension;
    }

    void setMAC(QString MAC)
    {
        this->MAC = MAC;
    }
    QString getMAC()
    {
        return MAC;
    }
    void setStuNo(QString stuNo)
    {
        this->stuNo = stuNo;
    }

    QString getStuNo()
    {
        return stuNo;
    }

    void setStuName(QString stuName)
    {
        this->stuName = stuName;
    }
    QString getStuName()
    {
        return stuName;
    }

    void setStuClass(QString stuClass)
    {
        this->stuClass = stuClass;
    }
    QString getStuClass()
    {
        return stuClass;
    }

    void setScore(int score)
    {
        this->score = score;
    }
    void setStuScorePlusPlus()
    {
        this->score++;
    }
    int getScore()
    {
        return score;
    }

    void setRealAns(int titleNum,QString ans)
    {
        this->realAns.insert(titleNum,ans);
    }
    QMap <int,QString> getRealAns()
    {
        return realAns;
    }

    void setRealAnsIsRight(int titleNum,bool isRight)
    {
        this->realAnsIsRight.insert(titleNum,isRight);
    }
    QMap <int,bool> getRealAnsIsRight()
    {
        return realAnsIsRight;
    }

    void setRealAnsCom(int titleNum,QString ans)
    {
        this->realAnsCom.insert(titleNum,ans);
    }
    QMap <int,QString> getRealAnsCom()
    {
        return realAnsCom;
    }

    void setRealAnsIsRightCom(int titleNum,bool isRight)
    {
        this->realAnsIsRightCom.insert(titleNum,isRight);
    }
    QMap <int,bool> getRealAnsIsRightCom()
    {
        return realAnsIsRightCom;
    }

    //重构QDataStream的out的<<
    friend QDataStream &operator <<(QDataStream &out, const SendInfo *si)
    {
        out << si->MAC;
        out << si->stuNo;
        out << si->stuName;
        out << si->stuClass;
        out << si->score;
        out << si->realAns;
        out << si->realAnsIsRight;

        out << si->realAnsCom;
        out << si->realAnsIsRightCom;

        out << si->paperChoice;
        out << si->paperCompletion;
        out << si->paperComprehension;

        out << si->totalScore;
        return out;
    }

    //重构QDataStream的in的>>
    friend QDataStream &operator >>(QDataStream &in, SendInfo *si)
    {
        in >> si->MAC;
        in >> si->stuNo;
        in >> si->stuName;
        in >> si->stuClass;
        in >> si->score;
        in >> si->realAns;
        in >> si->realAnsIsRight;

        in >> si->realAnsCom;
        in >> si->realAnsIsRightCom;

        in >> si->paperChoice;
        in >> si->paperCompletion;
        in >> si->paperComprehension;

        in >> si->totalScore;
        return in;
    }

};



#endif // SENDINFO_H
